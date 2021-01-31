#include "stdafx.h"
#include "device.h"


using namespace xrrng;

xrrng::Device xrrng::device;

namespace {

/**
 * Layers
 */
const std::vector<char const *> instanceLayers =
{
#if DEBUG
    "VK_LAYER_KHRONOS_validation",
#endif
};

/**
 * Instance extensions
 * (name, mandatory)
 */
const std::vector<std::pair<char const *, bool>> instanceExtensions =
{
    {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,    false},
#if DEBUG
    {VK_EXT_DEBUG_UTILS_EXTENSION_NAME,                         false},
#endif
    {VK_KHR_SURFACE_EXTENSION_NAME,                             true},
#if defined(WINDOWS)
    {VK_KHR_WIN32_SURFACE_EXTENSION_NAME,                       true},
#endif
};

/**
 * Validation features
 */
const std::vector<vk::ValidationFeatureEnableEXT> enabledValidations =
{
#if DEBUG
    vk::ValidationFeatureEnableEXT::eGpuAssisted,
    vk::ValidationFeatureEnableEXT::eGpuAssistedReserveBindingSlot,
    // TODO: add best practices when available
#endif
};

/*
 * Device extensions
 */
const std::vector<std::pair<char const *, bool>> deviceExtensions =
{
    {VK_KHR_SWAPCHAIN_EXTENSION_NAME,               true},
};

}

int vk_phy_device = 0;
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE


//-----------------------------------------------------------------------------
void
Device::Create
        ( HWND      window
        , size_t    width
        , size_t    height
)
{
    CreateInstance();

    SelectGpu();

    CreateSurface(window);

    CreateLogicalDevice();

    CreateSwapchain(width, height);

    AdoptSwapchainImages();

    CreateQueues();

    CreateSyncs();

    CreateCmdBufferPools();

#if  0
    CreateMemoryAllocator();
#endif

    // Acquire image
    State.deviceState =
        m_Device->acquireNextImageKHR(
            m_Swapchain.get(),
            UINT64_MAX,
            State.acquireSemaphores[0].get(),
            nullptr,
            &State.imageIndex);
    R_ASSERT(State.deviceState == vk::Result::eSuccess);
}



//-----------------------------------------------------------------------------
void
Device::CreateInstance()
{
    vk::DynamicLoader dl;
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

    // Select only supported layers
    const auto& supportedLayers = vk::enumerateInstanceLayerProperties();
    std::vector<pcstr> layers;
    layers.reserve(instanceLayers.size());
    
    for (const auto& required_layer : instanceLayers)
    {
        const auto& it = std::find_if(
            supportedLayers.cbegin(),
            supportedLayers.cend(),
            [&](auto layer)
            {
                return !strcmp(layer.layerName, required_layer);
            });
        if (it == supportedLayers.cend())
        {
            continue;
        }
        layers.push_back(required_layer);
        Msg("* RVK(HW): Layer `%s` enabled", required_layer);
    }

    // Check for required extensions
    const auto& supportedExtensions = vk::enumerateInstanceExtensionProperties();
    std::vector<pcstr> extensions;
    extensions.reserve(instanceExtensions.size());

    for (const auto& [required_ext, mandatory] : instanceExtensions)
    {
        const auto& it = std::find_if(
            supportedExtensions.cbegin(),
            supportedExtensions.cend(),
            [&](auto extension)
            {
                return !strcmp(extension.extensionName, required_ext);
            });
        if (it == supportedExtensions.cend())
        {
            R_ASSERT3(!mandatory, "Extension %s not found", required_ext);
            continue;
        }
        extensions.push_back(required_ext);
        Msg("* RVK(HW): Extension `%s` enabled", required_ext);
    }

    // Create instance
    const auto& appInfo = vk::ApplicationInfo()
        .setApiVersion(VK_API_VERSION_1_1)
        .setPApplicationName(Core.ApplicationTitle)
        .setEngineVersion(Core.GetBuildId())
        .setPEngineName(Core.ApplicationName);

    const auto& validationFeatures = vk::ValidationFeaturesEXT(
        enabledValidations.size(),
        enabledValidations.data());

    const auto& instanceInfo = vk::InstanceCreateInfo()
        .setPNext(&validationFeatures)
        .setPApplicationInfo(&appInfo)
        .setEnabledLayerCount(layers.size())
        .setPpEnabledLayerNames(layers.data())
        .setEnabledExtensionCount(extensions.size())
        .setPpEnabledExtensionNames(extensions.data());

    instance_ = vk::createInstanceUnique(instanceInfo/*, hostAllocationInfo*/);

    VULKAN_HPP_DEFAULT_DISPATCHER.init(instance_.get());
}


//-----------------------------------------------------------------------------
void
Device::SelectGpu()
{
    const auto& physicalDevices = instance_->enumeratePhysicalDevices();

    // Get the device specified by user
    if (!strstr(Core.Params, "-vk_phy_device"))
    {
        vk_phy_device = 0;
    }

    R_ASSERT2(
        vk_phy_device < physicalDevices.size(),
        "Invalid physical device # specified");

    gpu_ = physicalDevices[vk_phy_device];

    const auto QFamilies = gpu_.getQueueFamilyProperties();

    const bool isGpu = std::any_of(
        QFamilies.cbegin(),
        QFamilies.cend(),
        [](const auto family)
        {
            return !!(family.queueFlags & vk::QueueFlagBits::eGraphics);
        });

    R_ASSERT2(isGpu, "Selected device is not a GPU");

#if 0
    m_PhyCaps.properties = gpu_.getProperties();
    m_PhyCaps.features = gpu_.getFeatures();
    m_PhyCaps.memory = gpu_.getMemoryProperties();

    // Display name of the video board
    Msg("* GPU [vendor:%X]-[device:%X]: %s",
        m_PhyCaps.properties.vendorID,
        m_PhyCaps.properties.deviceID,
        m_PhyCaps.properties.deviceName
    );

    // Obtain driver information
    vkGetPhysicalDeviceProperties2KHR = (PFN_vkGetPhysicalDeviceProperties2KHR)
        m_Instance->getProcAddr("vkGetPhysicalDeviceProperties2KHR");
    
    if (vkGetPhysicalDeviceProperties2KHR) { // extension loaded
        auto driver_props = m_Gpu.getProperties2KHR<
            vk::PhysicalDeviceProperties2KHR,
            vk::PhysicalDeviceDriverPropertiesKHR>();

        const auto& conformance_version =
            driver_props.get<vk::PhysicalDeviceDriverPropertiesKHR>()
            .conformanceVersion;

        Msg("* GPU driver: %d.%d.%d.%d",
            conformance_version.major,
            conformance_version.minor,
            conformance_version.subminor,
            conformance_version.patch);
    }
#endif
}


//-----------------------------------------------------------------------------
void
Device::CreateLogicalDevice()
{
    // Select supported extensions
    const auto& supportedExtensions = gpu_.enumerateDeviceExtensionProperties();
    std::vector<pcstr> extensions;
    extensions.reserve(deviceExtensions.size());
    
    for (const auto& [required_ext, mandatory] : deviceExtensions)
    {
        const auto& it = std::find_if(
            supportedExtensions.cbegin(),
            supportedExtensions.cend(),
            [&](auto extension)
            {
                return !strcmp(extension.extensionName, required_ext);
            });
        if (it == supportedExtensions.cend())
        {
            R_ASSERT3(!mandatory, "Extension %s not found", required_ext);
            continue;
        }
        extensions.push_back(required_ext);
        Msg("* RVK(HW): Extension `%s` enabled", required_ext);
    }

    EnumerateQueues();

    // Request device queues
    constexpr float defaultPriority = .0F;

    std::unordered_map<uint32_t, vk::DeviceQueueCreateInfo> queueInfosMap{};
    std::vector<std::vector<float>> priorities{};

    for (const auto& [family, location] : m_QFamilies)
    {
        if (!family.has_value())
        {
            continue;
        }

        const auto familyIndex = family.value();

        if (familyIndex >= priorities.size())
        {
            priorities.resize(size_t(familyIndex) + 1);
        }
        priorities[familyIndex].emplace_back(defaultPriority);

        auto& it = queueInfosMap.find(familyIndex);

        if (it == queueInfosMap.cend())
        {
            bool hasInserted;
            std::tie(it, hasInserted) = queueInfosMap.emplace(
                familyIndex,
                vk::DeviceQueueCreateInfo
                {
                    {},
                    familyIndex,
                    1,
                    priorities[familyIndex].data()
                });
            VERIFY(hasInserted);
        }
        else
        {
            it->second.queueCount++;
            it->second.pQueuePriorities = priorities[familyIndex].data(); // need to be updated after reallocation
        }
    }

    std::vector<vk::DeviceQueueCreateInfo> queueInfos{};
    std::transform(
        queueInfosMap.cbegin(),
        queueInfosMap.cend(),
        std::back_inserter(queueInfos),
        [](const auto& info)
        {
            return info.second;
        });

    // TODO: carefully check all the features needed
    const auto& features = vk::PhysicalDeviceFeatures();

    // Create logical device
    const auto& device_info = vk::DeviceCreateInfo()
        .setPEnabledFeatures(&features)
        .setEnabledExtensionCount(extensions.size())
        .setPpEnabledExtensionNames(extensions.data())
        .setQueueCreateInfoCount(queueInfos.size())
        .setPQueueCreateInfos(queueInfos.data());

    m_Device = gpu_.createDeviceUnique(device_info/*, hostAllocationInfo*/);

    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Device.get());
}


//-----------------------------------------------------------------------------
void
Device::EnumerateQueues()
{
    const auto& queueFamilyProps = gpu_.getQueueFamilyProperties();

    uint32_t familyIndex{ 0 };
    for (const auto& props : queueFamilyProps)
    {
        uint32_t location{ 0 };
            
        if (props.queueFlags & vk::QueueFlagBits::eGraphics)
        {
            m_QFamilies[GRAPHICS] = { familyIndex, location++ };
        }

        if (props.queueFlags & vk::QueueFlagBits::eTransfer)
        {
            m_QFamilies[XFER] = { familyIndex, location++ };
        }

        if (props.queueFlags & vk::QueueFlagBits::eCompute)
        {
            m_QFamilies[COMPUTE] = { familyIndex, location++ };
        }

        const bool canPresent = gpu_.getSurfaceSupportKHR(
            familyIndex, m_WsiSurface.get());

        if (canPresent)
        {
            m_QFamilies[PRESENT] = { familyIndex, location++ };
        }

        ++familyIndex;
    }
    R_ASSERT2(
        m_QFamilies[PRESENT].first.has_value(),
        "Selected GPU doesn't support presentation");
}


//-----------------------------------------------------------------------------
void
Device::CreateSyncs()
{
    // Swapchain semaphores
    State.acquireSemaphores.resize(State.colorImages.size());
    for (auto& semaphore : State.acquireSemaphores)
    {
        semaphore = m_Device->createSemaphoreUnique({});
    }

    // Rendering semaphores
    State.renderSemaphores.resize(State.colorImages.size());
    for (auto& semaphore : State.renderSemaphores)
    {
        semaphore = m_Device->createSemaphoreUnique({});
    }

    // Frame fences
    State.frameFences.resize(State.colorImages.size());
    const auto& fenceInfo = vk::FenceCreateInfo()
        .setFlags(vk::FenceCreateFlagBits::eSignaled);
    for (auto& fence : State.frameFences)
    {
        fence = m_Device->createFenceUnique(fenceInfo);
    }
}


//-----------------------------------------------------------------------------
void
Device::CreateQueues()
{
    auto queueIndex = 0;
    for (const auto& [family, position] : m_QFamilies)
    {
        if (!family.has_value())
        {
            continue;
        }

        m_Queues[queueIndex] = m_Device->getQueue(family.value(), position);
        ++queueIndex;
    }
}


//-----------------------------------------------------------------------------
void
Device::CreateCmdBufferPools()
{
    auto poolIndex = 0;
    for (const auto& [family, position] : m_QFamilies)
    {
        if (!family.has_value())
        {
            continue;
        }

        const auto& cmdPoolInfo = vk::CommandPoolCreateInfo()
            .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
            .setQueueFamilyIndex(family.value());

        m_CmdPools[poolIndex] = m_Device->createCommandPoolUnique(cmdPoolInfo);
        ++poolIndex;
    }
}


//-----------------------------------------------------------------------------
std::vector<vk::UniqueCommandBuffer>
Device::AllocateCmdBuffers
        ( QueueType pool
        , size_t count
        , bool secondary/*= true*/
        ) const
{
    const auto& bufferAllocInfo = vk::CommandBufferAllocateInfo()
        .setCommandBufferCount(count)
        .setCommandPool(m_CmdPools[pool].get())
        .setLevel(secondary ? vk::CommandBufferLevel::eSecondary : vk::CommandBufferLevel::ePrimary);

    return m_Device->allocateCommandBuffersUnique(bufferAllocInfo);
}


//-----------------------------------------------------------------------------
void
Device::Destroy()
{
#if 0
    DestroyMemoryAllocator();

    for (auto& pool : m_CmdPools)
    {
        pool.reset();
    }

    State.renderSemaphores.clear();
    State.acquireSemaphores.clear();

    for (auto& view : State.colorViews)
    {
        m_Device->destroyImageView(view);
    }

    m_Swapchain.reset();

    m_Device.reset();

    m_WsiSurface.reset();

    m_Instance.reset();
#endif
}

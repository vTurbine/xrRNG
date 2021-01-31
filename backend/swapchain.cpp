#include "stdafx.h"

#include "device/device.h"

#include <xrEngine/defines.h>


namespace
{

const std::vector<vk::Format> desiredColors32 =
{
    vk::Format::eB8G8R8A8Unorm,
};

const std::vector<vk::Format> desiredDepths =
{
    vk::Format::eD32SfloatS8Uint,
    vk::Format::eD24UnormS8Uint,
};

}

using namespace xrrng;


//-----------------------------------------------------------------------------
void
Device::CreateSurface(HWND window)
{
#if defined(WINDOWS)
    // Request a surface from WSI
    const auto surfaceCreateInfo = vk::Win32SurfaceCreateInfoKHR()
        .setHinstance(::GetModuleHandleA(nullptr))
        .setHwnd(window);

    m_WsiSurface = instance_->createWin32SurfaceKHRUnique(surfaceCreateInfo);
#else
#   error "Only Windows platform is supported"
#endif

    // Check if the device can work with current WSI
#if defined(WINDOWS)
    const bool wsiSupported =
        gpu_.getWin32PresentationSupportKHR(*m_QFamilies[PRESENT].first);
#else
    const bool wsiSupported = false;
#endif
    R_ASSERT2(wsiSupported, "Selected GPU isn't compatible with WSI");

    // Get WSI caps
    m_WsiCaps.surface       = gpu_.getSurfaceCapabilitiesKHR(m_WsiSurface.get());
    m_WsiCaps.presentModes  = gpu_.getSurfacePresentModesKHR(m_WsiSurface.get());
    m_WsiCaps.formats       = gpu_.getSurfaceFormatsKHR(m_WsiSurface.get());

    // TODO: check for exclusive fullscreen mode
}


//-----------------------------------------------------------------------------
void
Device::CreateSwapchain
        ( size_t width
        , size_t height
        )
{
    m_SwapchainParams.fullScreen = psDeviceFlags.is(rsFullscreen);

    // Select base attachment color format
    const auto& [colorFormat, colorSpace] = SelectColorFormat();
    m_SwapchainParams.colorFormat = colorFormat;

    // Select base depth attachment format
    m_SwapchainParams.depthFormat = SelectDepthStencilFormat();

    // Set surface extents
    if (m_WsiCaps.surface.currentExtent.width == VkUndefined)
    {
        m_SwapchainParams.extent.setWidth(width);
        m_SwapchainParams.extent.setHeight(height);
    }
    else
    {
        m_SwapchainParams.extent = m_WsiCaps.surface.currentExtent;
    }

    // Composite alpha
    const auto composite_alpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    R_ASSERT2(
        m_WsiCaps.surface.supportedCompositeAlpha & composite_alpha,
        "No suitable alpha blending mode");

    // Select presentation mode
    const auto presentMode = SelectPresentationMode();

    // Select images count
    const auto minImageCount = m_WsiCaps.surface.minImageCount;
    Msg("* RVK(HW): Using %d-images chain", minImageCount);

    // Select images usage
    const auto usage =
        vk::ImageUsageFlagBits::eTransferDst | // only write to attachment
        vk::ImageUsageFlagBits::eColorAttachment;
    R_ASSERT2(
        m_WsiCaps.surface.supportedUsageFlags & usage,
        "WSI surface can't be used as target image");

    // Create swapchain
    const auto& swapchain_info = vk::SwapchainCreateInfoKHR()
        .setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity)
        .setCompositeAlpha(composite_alpha)
        .setImageFormat(m_SwapchainParams.colorFormat)
        .setImageColorSpace(colorSpace)
        .setPresentMode(presentMode)
        .setMinImageCount(minImageCount)
        .setImageUsage(usage)
        .setImageArrayLayers(1)
        .setImageExtent(m_SwapchainParams.extent)
        .setSurface(m_WsiSurface.get());

    m_Swapchain = m_Device->createSwapchainKHRUnique(swapchain_info);
}


//-----------------------------------------------------------------------------
std::pair<vk::Format, vk::ColorSpaceKHR>
Device::SelectColorFormat() const
{
    std::pair<vk::Format, vk::ColorSpaceKHR> result =
    {
        vk::Format::eUndefined,
        vk::ColorSpaceKHR::eSrgbNonlinear
    };

    R_ASSERT2(psCurrentBPP == 32, "Only 32bpp is supported");

    if (m_WsiCaps.formats.size() == 1 &&
        m_WsiCaps.formats[0].format == vk::Format::eUndefined)
    {
        // WSI doesn't care about format. Choose the best one.
        result =
            std::make_pair(desiredColors32.at(0), vk::ColorSpaceKHR::eSrgbNonlinear);
    }
    else
    {
        for (const auto format : desiredColors32)
        {
             const auto& it = std::find_if(
                m_WsiCaps.formats.cbegin(),
                m_WsiCaps.formats.cend(),
                [&](const auto supportedFormat)
                {
                    return supportedFormat.format == format;
                });
            if (it != m_WsiCaps.formats.cend())
            {
                result = std::make_pair(it->format, it->colorSpace);
                break;
            }
        }
    }
    R_ASSERT2(result.first != vk::Format::eUndefined, "No supported color format found");

    Msg("* RVK(HW): Color format: %s", to_string(result.first));
    return result;
}


//-----------------------------------------------------------------------------
vk::Format
Device::SelectDepthStencilFormat() const
{
    vk::Format result = vk::Format::eUndefined;

    for (const auto format : desiredDepths)
    {
        const auto& format_props = gpu_.getFormatProperties(format);
        // TODO: we have interest only in optimal tiling
        if (format_props.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
        {
            result = format;
            break;
        }
    }
    R_ASSERT2(result != vk::Format::eUndefined, "No supported depth format found");
    Msg("* RVK(HW): Depth format: %s", to_string(result));
    return result;
}


//-----------------------------------------------------------------------------
vk::PresentModeKHR
Device::SelectPresentationMode() const
{
    auto selectedMode = vk::PresentModeKHR::eFifo; // always supported
    if (psDeviceFlags.test(rsVSync))
    {
        const auto& supportedModes = m_WsiCaps.presentModes;

        const auto& it = std::find_if(
                supportedModes.cbegin(),
                supportedModes.cend(),
                [&](const auto mode)
                {
                    return (mode == selectedMode);
                });

        if (it == supportedModes.cend())
        {
            Msg("! RVK(HW): VSYNC requested but not supported by WSI");
        }
        else
        {
            selectedMode = vk::PresentModeKHR::eMailbox;
        }
    }

    return selectedMode;
}


//-----------------------------------------------------------------------------
void
Device::Present()
{
    auto & currentImage = State.imageIndex;

    const std::vector<vk::SwapchainKHR> swapchains {
        m_Swapchain.get()
    };
    const std::vector<vk::Semaphore> waitSemaphores {
        State.renderSemaphores[currentImage].get()
    };

    // Present image
    const auto& presentInfo = vk::PresentInfoKHR()
        .setPImageIndices(&currentImage)
        .setWaitSemaphoreCount(waitSemaphores.size())
        .setPWaitSemaphores(waitSemaphores.data())
        .setSwapchainCount(swapchains.size())
        .setPSwapchains(swapchains.data());

    State.deviceState = m_Queues[PRESENT].presentKHR(presentInfo);

    // TODO: check device state

    // Switch to the next swapchain image
    currentImage = (currentImage + 1) % State.colorImages.size();

    constexpr auto infiniteTime = std::numeric_limits<std::uint64_t>::max();

    // Acquire image
    State.deviceState =
        m_Device->acquireNextImageKHR(
            m_Swapchain.get(),
            infiniteTime, // TODO: reconsider when adding present latency control
            State.acquireSemaphores[currentImage].get(),
            nullptr,
            &currentImage);
}


//-----------------------------------------------------------------------------
DeviceState
Device::GetDeviceState() const
{
    auto result = DeviceState::Normal;

    switch (State.deviceState)
    {
    case vk::Result::eSuccess:
        break;

        // If the device was lost, do not render until we get it back
    case vk::Result::eErrorDeviceLost:
        result = DeviceState::Lost;
        break;

        // Check if the device is ready to be reset
    case vk::Result::eSuboptimalKHR:
    case vk::Result::eErrorSurfaceLostKHR:
        result = DeviceState::NeedReset;
        break;

    default:
        FATAL_F("Unhandled device state [%x]", State.deviceState);
    }

    return result;
}


//-----------------------------------------------------------------------------
void
Device::AdoptSwapchainImages()
{
    // Obtain swapchain images
    State.colorImages = m_Device->getSwapchainImagesKHR(m_Swapchain.get());

    auto const numImages = State.colorImages.size();

    // Create views for swapchain attachments
    State.colorViews.resize(numImages);

    const auto& subresourceRange = vk::ImageSubresourceRange(
        vk::ImageAspectFlagBits::eColor,
        0,  // mips
        1,  // levels
        0,  // base layer
        1   // layers
    );

    auto viewInfo = vk::ImageViewCreateInfo()
        .setViewType(vk::ImageViewType::e2D)
        .setFormat(m_SwapchainParams.colorFormat)
        .setSubresourceRange(subresourceRange);

    auto index = 0;
    for (auto &color : State.colorImages)
    {
        viewInfo.setImage(color);
        State.colorViews[index] = m_Device->createImageView(viewInfo);
        ++index;
    }
}

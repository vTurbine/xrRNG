#include "stdafx.h"

#include "device/device.h"

#include <volk/volk.h> // should be included before VMA to make vkXXX pointers available
#define VMA_IMPLEMENTATION
#define VMA_STATIC_VULKAN_FUNCTIONS 1
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#include <3rdparty/VulkanMemoryAllocator/src/vk_mem_alloc.h>


namespace
{
/**
 * Custom host memory allocator
 */
void*
XrAlloc(
    void*                   pUserData,
    size_t                  size,
    size_t                  alignment,
    VkSystemAllocationScope allocationScope)
{
    // TODO: wait for feature landing
    return nullptr;//Memory.mem_alloc_aligned(size, alignment);
}

void*
XrRealloc(
    void*                   pUserData,
    void*                   pOriginal,
    size_t                  size,
    size_t                  alignment,
    VkSystemAllocationScope allocationScope)
{
    // TODO: wait for feature landing
    return nullptr;//Memory.mem_realloc_aligned(pOriginal, size, alignment);
}

void
XrFree(
    void*                   pUserData,
    void*                   pMemory)
{
    Memory.mem_free(pMemory);
}

const vk::AllocationCallbacks hostAllocationInfo
{
    nullptr,
    &XrAlloc,
    &XrRealloc,
    &XrFree,
};
}

using namespace xrrng;

//-----------------------------------------------------------------------------
DeviceAllocation::DeviceAllocation
        ( VmaAllocator const *allocator
        )
    : allocator_ref_(allocator)
{
}

//-----------------------------------------------------------------------------
DeviceBuffer::DeviceBuffer
        ( VmaAllocator const *allocation
        )
    : DeviceAllocation(allocation)
{
}


//-----------------------------------------------------------------------------
void
DeviceBuffer::SetName
        ( std::string const & name
        )
{
    auto const& info = vk::DebugUtilsObjectNameInfoEXT()
        .setObjectType(vk::ObjectType::eBuffer)
        .setObjectHandle(reinterpret_cast<uint64_t>(buffer))
        .setPObjectName(name.c_str());
    device.m_Device->setDebugUtilsObjectNameEXT(info);
}


//-----------------------------------------------------------------------------
DeviceBuffer::~DeviceBuffer()
{
    vmaDestroyBuffer(*allocator_ref_, buffer, allocation);
}


//-----------------------------------------------------------------------------
void
Device::CreateMemoryAllocator()
{
    VmaAllocatorCreateInfo allocatorInfo{};
    allocatorInfo.physicalDevice    = gpu_;
    allocatorInfo.device            = m_Device.get();
    allocatorInfo.instance          = instance_.get();
    vmaCreateAllocator(&allocatorInfo, &allocator_);

    // Allocate command buffer(s) for memory transfer
    xfer_cmdbs_ = AllocateCmdBuffers(
        Device::QueueType::XFER,
        State.colorImages.size(),
        false  // primary
    );
}


//-----------------------------------------------------------------------------
void
Device::DestroyMemoryAllocator()
{
    vmaDestroyAllocator(allocator_);
    xfer_cmdbs_.clear();
}


//-----------------------------------------------------------------------------
BufferPtr
Device::AllocateHostBuffer
        ( size_t size
        ) const
{
    auto buffer = std::make_unique<DeviceBuffer>(&allocator_);

    const auto& bufferInfo = vk::BufferCreateInfo()
        .setSize(size)
        .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
        .setSharingMode(vk::SharingMode::eExclusive);

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
    allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; // persistently mapped

    const auto result = vmaCreateBuffer(
        allocator_,
        &static_cast<VkBufferCreateInfo>(bufferInfo),
        &allocInfo,
        &buffer->buffer,
        &buffer->allocation,
        &buffer->allocation_info
    );
    VERIFY(result == VK_SUCCESS);

    return buffer;
}


//-----------------------------------------------------------------------------
BufferPtr
Device::AllocateDeviceBuffer
        ( size_t        size
        , BufferType    type
        ) const
{
    auto buffer = std::make_unique<DeviceBuffer>(&allocator_);

    auto bufferInfo = vk::BufferCreateInfo()
        .setSize(size)
        .setUsage(vk::BufferUsageFlagBits::eTransferDst)
        .setSharingMode(vk::SharingMode::eExclusive);

    switch (type)
    {
    case BufferType::Vertex:
        bufferInfo.usage |= vk::BufferUsageFlagBits::eVertexBuffer;
        break;
    case BufferType::Index:
        bufferInfo.usage |= vk::BufferUsageFlagBits::eIndexBuffer;
        break;
    case BufferType::Uniform:
        bufferInfo.usage |= vk::BufferUsageFlagBits::eUniformBuffer;
        break;
    case BufferType::Storage:
        bufferInfo.usage |= vk::BufferUsageFlagBits::eStorageBuffer;
        break;
    default:
        FATAL_F("Unexpected buffer type");
    }

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    const auto result = vmaCreateBuffer(
        allocator_,
        &static_cast<VkBufferCreateInfo>(bufferInfo),
        &allocInfo,
        &buffer->buffer,
        &buffer->allocation,
        &buffer->allocation_info);
    VERIFY(result == VK_SUCCESS);

    return buffer;
}


//-----------------------------------------------------------------------------
void
Device::AddTransfer
        ( BufferPtr const  &cpu_buffer
        , BufferPtr const  &gpu_buffer
        , size_t            offset  /*= 0*/
        , size_t            size    /*= 0*/
        )
{
    outstanding_xfers.emplace_back(
        TransferItem {
            cpu_buffer.get(),
            gpu_buffer.get(),
            offset,
            size
        }
    );
}


//-----------------------------------------------------------------------------
void
Device::ProcessTransfer()
{
    if (outstanding_xfers.empty())
    {
        return;
    }

    auto const ctxId = State.imageIndex;

    auto const& beginInfo = vk::CommandBufferBeginInfo()
        .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    auto &cmdBuffer = xfer_cmdbs_[ctxId];
    cmdBuffer->begin(beginInfo);
    {
        for (auto const& item : outstanding_xfers)
        {
            const auto& region = vk::BufferCopy()
                .setSrcOffset(item.offset)
                .setDstOffset(item.offset)
                .setSize(item.size
                    ? item.size
                    : item.src->allocation_info.size);

            cmdBuffer->copyBuffer(
                item.src->buffer,
                item.dst->buffer,
                region
            );
        }
    }
    cmdBuffer->end();

    auto const &submitInfo = vk::SubmitInfo()
        .setCommandBufferCount(1)
        .setCommandBuffers(std::array{ cmdBuffer.get() });

    // TODO: Buffer xfer is sync by now
    m_Queues[QueueType::XFER].submit(submitInfo, {});
    m_Queues[QueueType::XFER].waitIdle();

    outstanding_xfers.clear();

    // TODO: free-up resources if needed
}


//-----------------------------------------------------------------------------
DeviceImage::DeviceImage
        ( VmaAllocator const     *allocation
        )
    : DeviceAllocation(allocation)
{
}


//-----------------------------------------------------------------------------
DeviceImage::DeviceImage
        ( vk::Image     image_
        )
    : DeviceAllocation(nullptr)
    , image(image_)
{
}


//-----------------------------------------------------------------------------
DeviceImage::~DeviceImage()
{
    if (allocator_ref_ == nullptr)
    {
        return;
    }
    vmaDestroyImage(*allocator_ref_, image, allocation);
}


//-----------------------------------------------------------------------------
void
DeviceImage::SetName
        ( std::string const & name
        )
{
    auto const& info = vk::DebugUtilsObjectNameInfoEXT()
        .setObjectType(vk::ObjectType::eImage)
        .setObjectHandle(reinterpret_cast<uint64_t>(image))
        .setPObjectName(name.c_str());
    device.m_Device->setDebugUtilsObjectNameEXT(info);
}


//-----------------------------------------------------------------------------
vk::ImageView
DeviceImage::GetView()
{
    // TODO: handle cases with different subres
    if (!view_)
    {
        const auto& range = vk::ImageSubresourceRange()
            .setAspectMask(
                type == ImageType::Depth
                    ? vk::ImageAspectFlagBits::eDepth
                    : vk::ImageAspectFlagBits::eColor)
            .setLayerCount(1)
            .setLevelCount(1);

        const auto& createInfo = vk::ImageViewCreateInfo()
            .setImage(image)
            .setViewType(vk::ImageViewType::e2D)
            .setFormat(format)
            .setSubresourceRange(range);
        view_ = device.m_Device->createImageViewUnique(createInfo);
    }

    return view_.get();
}


//-----------------------------------------------------------------------------
ImagePtr
Device::AllocateDeviceImage
        ( vk::Extent3D const   &extent
        , vk::Format            format
        , ImageType             type
        ) const
{
    auto image = std::make_unique<DeviceImage>(&allocator_);

    image->extent = extent;
    image->format = format;
    image->type   = type;

    auto imageInfo = vk::ImageCreateInfo()
        .setExtent(image->extent)
        .setFormat(image->format)
        .setImageType(vk::ImageType::e2D) // TODO: 3D for LUTs
        .setTiling(vk::ImageTiling::eOptimal)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setSamples(vk::SampleCountFlagBits::e1) // TODO: MSAA
        .setSharingMode(vk::SharingMode::eExclusive)
        .setMipLevels(1) // TODO
        .setArrayLayers(1); // TODO

    switch (image->type)
    {
    case ImageType::Texture:
        imageInfo.usage = vk::ImageUsageFlagBits::eTransferDst;
        break;
    case ImageType::RenderTarget:
        imageInfo.usage = vk::ImageUsageFlagBits::eColorAttachment;
        break;
    case ImageType::Depth:
        imageInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment
            | vk::ImageUsageFlagBits::eTransferDst;
        break;
    default:
        FATAL_F("Unexpected image type");
    }

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    auto result = vmaCreateImage(
        allocator_,
        &static_cast<VkImageCreateInfo>(imageInfo),
        &allocInfo,
        &image->image,
        &image->allocation,
        nullptr);
    VERIFY(result == VK_SUCCESS);

    return image;
}


//-----------------------------------------------------------------------------
StagedBuffer::StagedBuffer
        ( size_t        size
        , BufferType    type
        )
    : size_(size)
{
    host_buffer_    = device.AllocateHostBuffer(size);
    device_buffer_  = device.AllocateDeviceBuffer(size, type);
}


//-----------------------------------------------------------------------------
void
StagedBuffer::SetName
        ( std::string const &name
        )
{
    host_buffer_->SetName(name + ":CPU");
    device_buffer_->SetName(name + ":GPU");
}


//-----------------------------------------------------------------------------
void
StagedBuffer::Transfer()
{
    device.AddTransfer(
        host_buffer_,
        device_buffer_,
        0,  // offset
        size_
    );
}

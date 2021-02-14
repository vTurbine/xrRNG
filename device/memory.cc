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

#if 0
//-----------------------------------------------------------------------------
DeviceAllocation::DeviceAllocation(const VmaAllocator* allocator)
    : allocatorRef(allocator)
{
}

//-----------------------------------------------------------------------------
DeviceBuffer::DeviceBuffer(const VmaAllocator* allocation)
    : DeviceAllocation(allocation)
{
}

//-----------------------------------------------------------------------------
DeviceBuffer::~DeviceBuffer()
{
    Release();
}

//-----------------------------------------------------------------------------
void DeviceBuffer::Release()
{
    vmaDestroyBuffer(*allocatorRef, buffer, allocation);
}
#endif

using namespace xrrng;

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
    //m_XferBuffers = AllocateCmdBuffer(CHW::QueueType::XFER, 1);
}


//-----------------------------------------------------------------------------
void
Device::DestroyMemoryAllocator()
{
    vmaDestroyAllocator(allocator_);

    //m_XferBuffers.clear();
}

#if 0
//-----------------------------------------------------------------------------
HostBuffer* CHW::AllocateHostBuffer(size_t size) const
{
    auto* buffer = new DeviceBuffer(&m_Allocator);

    const auto& bufferInfo = vk::BufferCreateInfo()
        .setSize(size)
        .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
        .setSharingMode(vk::SharingMode::eExclusive);

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
    allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    const auto result = vmaCreateBuffer(
        m_Allocator,
        &static_cast<VkBufferCreateInfo>(bufferInfo),
        &allocInfo,
        &buffer->buffer,
        &buffer->allocation,
        &buffer->allocationInfo);
    VERIFY(result == VK_SUCCESS);

    return buffer;
}

//-----------------------------------------------------------------------------
DeviceBuffer* CHW::AllocateDeviceBuffer(size_t size, DeviceBuffer::Type type)
{
    auto* buffer = new DeviceBuffer(&m_Allocator);

    auto bufferInfo = vk::BufferCreateInfo()
        .setSize(size)
        .setUsage(vk::BufferUsageFlagBits::eTransferDst)
        .setSharingMode(vk::SharingMode::eExclusive);

    switch (type)
    {
    case DeviceBuffer::Type::Vertex:
        bufferInfo.usage |= vk::BufferUsageFlagBits::eVertexBuffer;
        break;
    case DeviceBuffer::Type::Index:
        bufferInfo.usage |= vk::BufferUsageFlagBits::eIndexBuffer;
        break;
    case DeviceBuffer::Type::Constant:
        bufferInfo.usage |= vk::BufferUsageFlagBits::eUniformBuffer;
        break;
    default:
        FATAL_F("Unexpected buffer type");
    }

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    const auto result = vmaCreateBuffer(
        m_Allocator,
        &static_cast<VkBufferCreateInfo>(bufferInfo),
        &allocInfo,
        &buffer->buffer,
        &buffer->allocation,
        &buffer->allocationInfo);
    VERIFY(result == VK_SUCCESS);

    return buffer;
}

//-----------------------------------------------------------------------------
void CHW::Transfer(
    HostBuffer* hostBuffer,
    DeviceBuffer* deviceBuffer,
    size_t offset /*= 0*/,
    size_t size /*= 0*/)
{
    const auto& beginInfo = vk::CommandBufferBeginInfo()
        .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    const auto& region = vk::BufferCopy()
        .setSrcOffset(offset)
        .setDstOffset(offset)
        .setSize(size ? size : deviceBuffer->allocationInfo.size);

    const auto bufferIndex = 0;
    auto& cmdBuffer = m_XferBuffers[bufferIndex];

    cmdBuffer->begin(beginInfo);
    cmdBuffer->copyBuffer(hostBuffer->buffer, deviceBuffer->buffer, region);
    cmdBuffer->end();

    const auto& submitInfo = vk::SubmitInfo()
        .setCommandBufferCount(1)
        .setPCommandBuffers(&cmdBuffer.get());

    // TODO: Buffer xfer is sync by now
    m_Queues[QueueType::XFER].submit(submitInfo, {});
    m_Queues[QueueType::XFER].waitIdle();
}

//-----------------------------------------------------------------------------
DeviceImage::DeviceImage(const VmaAllocator* allocation)
    : DeviceAllocation(allocation)
{
}

//-----------------------------------------------------------------------------
DeviceImage::DeviceImage(vk::Image image)
    : DeviceAllocation(nullptr)
{
    this->image = image;
}

//-----------------------------------------------------------------------------
DeviceImage::~DeviceImage()
{
    Release();
}

//-----------------------------------------------------------------------------
void DeviceImage::Release()
{
    if (allocatorRef == nullptr)
    {
        return;
    }
    vmaDestroyImage(*allocatorRef, image, allocation);
}

//-----------------------------------------------------------------------------
DeviceImage* CHW::AllocateDeviceImage(const vk::Extent3D& extent, vk::Format format, DeviceImage::Type type)
{
    auto* image = new DeviceImage(&m_Allocator);

    image->extent = extent;
    image->format = format;
    image->type   = type;

    auto imageInfo = vk::ImageCreateInfo()
        .setExtent(image->extent)
        .setFormat(image->format)
        .setImageType(vk::ImageType::e2D) // TODO: 3D for LUTs
        .setTiling(vk::ImageTiling::eOptimal)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setUsage(vk::ImageUsageFlagBits::eSampled) // TODO: performance drop?
        .setSamples(vk::SampleCountFlagBits::e1) // TODO: MSAA
        .setSharingMode(vk::SharingMode::eExclusive)
        .setMipLevels(1) // TODO
        .setArrayLayers(1); // TODO

    switch (image->type)
    {
    case DeviceImage::Type::Texture:
        imageInfo.usage = vk::ImageUsageFlagBits::eTransferDst;
        break;
    case DeviceImage::Type::ColorAttachment:
        imageInfo.usage = vk::ImageUsageFlagBits::eColorAttachment;
        break;
    case DeviceImage::Type::DepthAttachment:
        imageInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
        break;
    default:
        FATAL_F("Unexpected image type");
    }

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    auto result = vmaCreateImage(
        m_Allocator,
        &static_cast<VkImageCreateInfo>(imageInfo),
        &allocInfo,
        &image->image,
        &image->allocation,
        nullptr);
    VERIFY(result == VK_SUCCESS);

    return image;
}
#endif

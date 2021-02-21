#ifndef DEVICE_MEMORY_H_
#define DEVICE_MEMORY_H_

#include <3rdparty/VulkanMemoryAllocator/src/vk_mem_alloc.h>


enum class BufferType //TODO: replace by VK_BUFFER...
{
    Vertex,
    Index,
    Uniform,
    Image
};

enum class ImageType
{
    Texture,
    RenderTarget,
    Depth
};

struct DeviceAllocation
{
    explicit DeviceAllocation(const VmaAllocator *allocator);

    const VmaAllocator *allocator_ref_;
    VmaAllocation       allocation;
    VmaAllocationInfo   allocation_info;
};

class DeviceBuffer
    : public DeviceAllocation
{
public:
    explicit DeviceBuffer(VmaAllocator const *allocator);
    ~DeviceBuffer();

    VkBuffer buffer;
};

using BufferPtr = std::unique_ptr<DeviceBuffer>;


class DeviceImage
    : public DeviceAllocation
{
public:
    std::uint32_t   layers_count; //< Color layers
    std::uint32_t   levels_count; //< Mipmap levels
    vk::Extent3D    extent;       //< Image dimensions
    vk::Format      format;       //< Image format
    vk::ImageLayout layout{ vk::ImageLayout::eUndefined };  //< Current layout
    ImageType       type;         //< Type of image

    explicit DeviceImage(VmaAllocator const *allocator);
    explicit DeviceImage(vk::Image image_);
    ~DeviceImage();

    void SetName(std::string const &name);
    vk::ImageView CreateView();

    VkImage image;
};

using ImagePtr = std::unique_ptr<DeviceImage>;

#endif // DEVICE_MEMORY_H_

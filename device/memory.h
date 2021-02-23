#ifndef DEVICE_MEMORY_H_
#define DEVICE_MEMORY_H_

#include <3rdparty/VulkanMemoryAllocator/src/vk_mem_alloc.h>


enum class BufferType //TODO: replace by VK_BUFFER...
{
    Vertex,
    Index,
    Uniform,
    Storage,
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

    void SetName(std::string const &name);
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
    vk::ImageView GetView();

    VkImage image;
private:
    vk::UniqueImageView view_;
};

using ImagePtr = std::unique_ptr<DeviceImage>;


struct StagedBuffer
{
    StagedBuffer(size_t size, BufferType type);

    void *GetPointer() const { return host_buffer_->allocation_info.pMappedData; }
    void Transfer();

    void SetName(std::string const &name);

private:
    BufferPtr   host_buffer_;
    BufferPtr   device_buffer_;
    size_t      size_;
};


#endif // DEVICE_MEMORY_H_

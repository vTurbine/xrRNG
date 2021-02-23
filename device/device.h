#ifndef DEVICE_DEVICE_H_
#define DEVICE_DEVICE_H_

#include "device/memory.h"

#include <xrEngine/device.h>

#include <optional>


namespace xrrng
{

class Device
{
    friend class Backend;
public:
    enum QueueType
    {
        GRAPHICS,
        XFER,
        COMPUTE,
        PRESENT,

        MAX_QUEUES
    };

public:
    void Create(HWND window, size_t width, size_t height);
    void Destroy();
    DeviceState GetDeviceState() const;
    void Present();

    std::vector<vk::UniqueCommandBuffer> AllocateCmdBuffers(QueueType pool, size_t count, bool secondary=true) const;

    // Memory management
    BufferPtr AllocateHostBuffer(size_t size) const;
    BufferPtr AllocateDeviceBuffer(size_t size, BufferType type) const;
    ImagePtr  AllocateDeviceImage(vk::Extent3D const& extent, vk::Format format, ImageType type) const;

    // Debug
    void GpuMarkerBegin(vk::CommandBuffer cmdb, std::string const &name, std::array<float, 4> const &color) const;
    void GpuMarkerEnd(vk::CommandBuffer cmdb) const;

private:
    void CreateInstance();
    void SelectGpu();
    void CreateLogicalDevice();
    void EnumerateQueues();
    void CreateSurface(HWND window);
    void CreateSwapchain(size_t width, size_t height);
    void AdoptSwapchainImages();
    void CreateQueues();
    void CreateSyncs();
    void CreateCmdBufferPools();
    void CreateMemoryAllocator();
    void DestroyMemoryAllocator();

    void InitDebug();

    vk::PresentModeKHR SelectPresentationMode() const;
    vk::Format SelectDepthStencilFormat() const;
    std::pair<vk::Format, vk::ColorSpaceKHR> SelectColorFormat() const;

public:
    struct State_t
    {
        std::vector<vk::Image> colorImages;
        std::vector<vk::ImageView> colorViews;
        //std::vector<DepthAttachment> defaultDepths;
        std::vector<vk::UniqueFramebuffer> framebuffers;
        std::vector<vk::UniqueSemaphore> acquireSemaphores;
        std::vector<vk::UniqueSemaphore> renderSemaphores;
        std::vector<vk::UniqueFence> frameFences;
        uint32_t imageIndex{ 0 };
        vk::Result deviceState;
    } State;

//private:
    vk::UniqueDevice m_Device;
    std::array<vk::Queue, MAX_QUEUES> m_Queues;

    static constexpr auto VkUndefined = std::numeric_limits<uint32_t>::max();

    struct PhyCaps_t
    {
        vk::PhysicalDeviceProperties properties;
        vk::PhysicalDeviceFeatures features;
        //vk::PhysicalDeviceMemoryProperties memory;
    } phy_caps_;

    struct WsiCaps_t
    {
        vk::SurfaceCapabilitiesKHR surface;
        std::vector<vk::PresentModeKHR> presentModes;
        std::vector<vk::SurfaceFormatKHR> formats;
    } wsi_caps_;

    struct SwapchainParams_t
    {
        vk::Format colorFormat;
        vk::Format depthFormat;
        vk::Extent2D extent;
        bool fullScreen{ false };
    } swapchain_params_;

    vk::UniqueInstance instance_;
    vk::PhysicalDevice gpu_;

    using QLocation = std::pair<std::optional<uint32_t>, uint32_t>;
    std::array<QLocation, MAX_QUEUES> m_QFamilies;
    std::array<vk::UniqueCommandPool, MAX_QUEUES> m_CmdPools;

    HWND m_hWnd;
    vk::UniqueSurfaceKHR    m_WsiSurface;
    vk::UniqueSwapchainKHR  m_Swapchain;
    VmaAllocator            allocator_;
};

extern Device device;

};

#endif // DEVICE_DEVICE_H_

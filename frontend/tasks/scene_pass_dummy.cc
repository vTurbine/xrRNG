#include "stdafx.h"

#include "scene_pass_dummy.h"

#include "device/device.h"
#include "frontend/render.h"


using namespace xrrng;

ScenePass::ScenePass()
    : RenderTask("MenuPass")
{
}


void
ScenePass::Build
        ( vk::CommandBuffer &cmdL
        )
{
    /* Temporary clear pass */

    static int i = 0;
    std::array<vk::ImageMemoryBarrier, 2> barriers;

    auto subResourceRange = vk::ImageSubresourceRange()
        .setAspectMask(vk::ImageAspectFlagBits::eColor)
        .setBaseMipLevel(0)
        .setLevelCount(1)
        .setBaseArrayLayer(0)
        .setLayerCount(1);
    barriers[0]
        .setImage(device.State.colorImages[i]) // TODO: this one should be came as task resource!
        .setOldLayout(vk::ImageLayout::eUndefined) // SC image is in undefined after acquiring
        .setNewLayout(vk::ImageLayout::eGeneral)
        .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setSrcAccessMask(vk::AccessFlagBits::eMemoryRead)
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
        .setSubresourceRange(subResourceRange);

    subResourceRange.setAspectMask(vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil);
    barriers[1]
        .setImage(::frontend.frame_datas_[i].base_depth->image) // TODO: this one should be came as task resource!
        .setOldLayout(vk::ImageLayout::eUndefined)
        .setNewLayout(vk::ImageLayout::eGeneral)
        .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setSrcAccessMask(vk::AccessFlagBits::eMemoryRead)
        .setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentWrite)
        .setSubresourceRange(subResourceRange);

    cmdL.pipelineBarrier(
        vk::PipelineStageFlagBits::eAllGraphics,
        vk::PipelineStageFlagBits::eAllGraphics,
        vk::DependencyFlagBits::eDeviceGroup,
        {}, {}, barriers);

    vk::ClearColorValue clearColor(std::array<float, 4>{0.0f, 1.0f, 0.0f, 1.0f});
    vk::ImageSubresourceRange range(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
    cmdL.clearColorImage(
        device.State.colorImages[i],
        vk::ImageLayout::eGeneral,
        clearColor,
        range
    );

    vk::ClearDepthStencilValue clearDepth(0.0f, 0); // inverse
    range.aspectMask = vk::ImageAspectFlagBits::eDepth;
    cmdL.clearDepthStencilImage(
        ::frontend.frame_datas_[i].base_depth->image,
        vk::ImageLayout::eGeneral,
        clearDepth,
        range
    );

    barriers[0]
        .setOldLayout(vk::ImageLayout::eGeneral)
        .setNewLayout(vk::ImageLayout::ePresentSrcKHR);

    barriers[1]
        .setOldLayout(vk::ImageLayout::eGeneral)
        .setNewLayout(vk::ImageLayout::eDepthAttachmentOptimal);

    cmdL.pipelineBarrier(
        vk::PipelineStageFlagBits::eAllGraphics,
        vk::PipelineStageFlagBits::eAllGraphics,
        vk::DependencyFlagBits::eDeviceGroup,
        {}, {}, barriers);

    ::frontend.frame_datas_[i].base_depth->layout = vk::ImageLayout::eDepthAttachmentOptimal;

    i++;
}

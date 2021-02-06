#include "stdafx.h"

#include "scene_pass_dummy.h"

#include "device/device.h"


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
    auto subResourceRange = vk::ImageSubresourceRange()
        .setAspectMask(vk::ImageAspectFlagBits::eColor)
        .setBaseMipLevel(0)
        .setLevelCount(1)
        .setBaseArrayLayer(0)
        .setLayerCount(1);

    auto barrier = vk::ImageMemoryBarrier()
        .setImage(device.State.colorImages[i]) // TODO: this one should be came as task resource!
        .setOldLayout(vk::ImageLayout::eUndefined)
        .setNewLayout(vk::ImageLayout::eGeneral)
        .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setSrcAccessMask(vk::AccessFlagBits::eMemoryRead)
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
        .setSubresourceRange(subResourceRange);

    cmdL.pipelineBarrier(
        vk::PipelineStageFlagBits::eAllGraphics,
        vk::PipelineStageFlagBits::eAllGraphics,
        vk::DependencyFlagBits::eDeviceGroup,
        {}, {}, { barrier });

    vk::ClearColorValue clearColor(std::array<float, 4>{0.0f, 1.0f, 0.0f, 1.0f});
    vk::ImageSubresourceRange range(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
    cmdL.clearColorImage(device.State.colorImages[i], vk::ImageLayout::eGeneral, clearColor, range);

    barrier
        .setOldLayout(vk::ImageLayout::eGeneral)
        .setNewLayout(vk::ImageLayout::ePresentSrcKHR);

    cmdL.pipelineBarrier(
        vk::PipelineStageFlagBits::eAllGraphics,
        vk::PipelineStageFlagBits::eAllGraphics,
        vk::DependencyFlagBits::eDeviceGroup,
        {}, {}, { barrier });

    i++;
}

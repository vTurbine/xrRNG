#include "stdafx.h"

#include "menu_pass.h"

#include "device/device.h"


using namespace xrrng;

MenuPass::MenuPass()
    : RenderTask("MenuPass")
{
}


void
MenuPass::Init()
{
}


void
MenuPass::Exec
        ( vk::CommandBuffer &cmdL
        )
{
    device.GpuMarkerBegin(
        cmdL,
        name_,
        { 1.0f, 0.0f, 1.0f, 1.0f }
    );

    /* Temporary clear pass */
    auto const i = device.State.imageIndex;

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

    vk::ClearColorValue clearColor(std::array<float, 4>{0.0f, 0.0f, 1.0f, 1.0f});
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

    device.GpuMarkerEnd(cmdL);
}

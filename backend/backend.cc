/*!
 * \file    backend.cc
 * \brief   Render backend implementation
 */

#include "stdafx.h"

#include "backend.h"
#include "device/device.h"


using namespace xrrng;

Backend xrrng::backend;


//-----------------------------------------------------------------------------
size_t
Backend::GetContextsNum() const
{
    return device.State.colorImages.size();
}


//-----------------------------------------------------------------------------
vk::CommandBuffer
Backend::GetCommandBuffer()
{
    auto const contextIdx = device.State.imageIndex;
    return draw_buffers_[contextIdx].get();
}

//-----------------------------------------------------------------------------
void
Backend::OnDeviceCreate()
{
    /* Create primary command buffers */
    auto const numImages = device.State.colorImages.size();
    draw_buffers_ = device.AllocateCmdBuffers(
        Device::QueueType::GRAPHICS,
        numImages,
        false
    );
}


//-----------------------------------------------------------------------------
void
Backend::OnDeviceDestroy()
{
    draw_buffers_.clear();
}


//-----------------------------------------------------------------------------
void
Backend::OnFrameBegin()
{
    /* Start to write primary command buffer */
    auto const index = device.State.imageIndex;
    auto const &cmdBuffer = draw_buffers_[index];

    // Wait for previous submission complete
    auto result = device.m_Device->waitForFences(
        { device.State.frameFences[index].get() },
        true,
        UINT64_MAX
    );

    auto const &beginInfo = vk::CommandBufferBeginInfo()
        .setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse);

    // Start buffer recording
    cmdBuffer->begin(beginInfo);
}


//-----------------------------------------------------------------------------
void 
Backend::OnFrameEnd()
{
    auto const index = device.State.imageIndex;
    auto const &cmdBuffer = draw_buffers_[index];

    // Stop buffer recording
    cmdBuffer->end();

    const std::vector<vk::CommandBuffer> cmdBuffers {
        cmdBuffer.get()
    };
    const std::vector<vk::Semaphore> signalSemaphores {
        device.State.renderSemaphores[index].get()
    };
    const std::vector<vk::Semaphore> waitSemaphores {
        device.State.acquireSemaphores[index].get()
    };

    // Reset frame fence
    device.m_Device->resetFences(
        { device.State.frameFences[index].get() }
    );

    // Submit command buffer
    const vk::PipelineStageFlags pipelineStage =
        vk::PipelineStageFlagBits::eColorAttachmentOutput;

    const auto& submitInfo = vk::SubmitInfo()
        .setPWaitDstStageMask(&pipelineStage)
        .setCommandBufferCount(cmdBuffers.size())
        .setPCommandBuffers(cmdBuffers.data())
        .setSignalSemaphoreCount(signalSemaphores.size())
        .setPSignalSemaphores(signalSemaphores.data())
        .setWaitSemaphoreCount(waitSemaphores.size())
        .setPWaitSemaphores(waitSemaphores.data());
    device.m_Queues[Device::QueueType::GRAPHICS].submit(
        submitInfo,
        { device.State.frameFences[index].get() });
}

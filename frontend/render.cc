#include "stdafx.h"

#include "backend/backend.h"
#include "device/device.h"
#include "frontend/render.h"

#include <xrEngine/IGame_Persistent.h>


using namespace xrrng;

//-----------------------------------------------------------------------------
void
FrontEnd::OnDeviceCreate
        ( char const *shaders_lib_name
        )
{
    backend.OnDeviceCreate();

    auto const num_frames = backend.GetContextsNum();

    frame_datas_.resize(num_frames);
    scene_cmds_ = device.AllocateCmdBuffers(Device::QueueType::GRAPHICS, num_frames);

    for (int i = 0; i < num_frames; ++i)
    {
        // Create resources
        // ...

        // Pre-record command buffer
        // ...

        /*
         * Here should be rendergraph
         */

        auto& cmdL = scene_cmds_[i].get();
        
        auto inheritanceInfo = vk::CommandBufferInheritanceInfo();

        auto beginInfo = vk::CommandBufferBeginInfo()
            .setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse)
            .setPInheritanceInfo(&inheritanceInfo);
        cmdL.begin(beginInfo);

        if (1) // clear pass
        {
            auto subResourceRange = vk::ImageSubresourceRange()
                .setAspectMask(vk::ImageAspectFlagBits::eColor)
                .setBaseMipLevel(0)
                .setLevelCount(1)
                .setBaseArrayLayer(0)
                .setLayerCount(1);

            auto barrier = vk::ImageMemoryBarrier()
                .setImage(device.State.colorImages[i])
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
        }

        cmdL.end();
    }
}


//-----------------------------------------------------------------------------
void
FrontEnd::OnDeviceDestroy
        ( bool keep_textures
        )
{
    // ...

    backend.OnDeviceDestroy();
}


//-----------------------------------------------------------------------------
void
FrontEnd::Calculate()
{
    M_SCOPED;

    FrameData &fd = frame_datas_[0]; // TODO: take from ctx num

    // Detect current sector
    if (!fd.vPrevCameraPos.similar(::Device.vCameraPosition, 1e-4/*EPS_S*/))
    {
        M_SCOPED_N("DetectSector");

        // ...

        fd.vPrevCameraPos.set(::Device.vCameraPosition);
    }

    // Collect static geometry
    // ...

    // Collect dynamic geometry
    // ...

    // Collect lights
    // ...
}


//-----------------------------------------------------------------------------
void
FrontEnd::Render()
{
    VERIFY(g_pGamePersistent);

    // Skip menu rendering
    if (g_pGamePersistent->IsMainMenuActive())
    {
        //return;
    }

    auto &cmdL = backend.GetCommandBuffer();

    // Update resources
    // ...

    // Do streaming
    // ...

    // Submit list
    // ...
    cmdL.executeCommands(1, &scene_cmds_[device.State.imageIndex].get());
}


//-----------------------------------------------------------------------------
void
FrontEnd::SetCacheXform
        ( Fmatrix   &mView
        , Fmatrix   &mProject
        )
{
    FrameData &fd = frame_datas_[device.State.imageIndex];

    fd.mView        = mView;
    fd.mProjection  = mProject;
}

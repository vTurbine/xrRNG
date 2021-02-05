#include "stdafx.h"

#include "backend/backend.h"
#include "device/device.h"
#include "frontend/render.h"
#include "legacy/raffle/FHierrarhyVisual.h"

#include <xrEngine/IGame_Persistent.h>
#include <xrCore/FMesh.hpp>


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
FrontEnd::AddStatic
        ( dxRender_Visual &vis
        , CFrustum        &frustum
        )
{
    auto& bvolume = vis.vis;
    auto mask = frustum.getMask();

    // Check frustum visibility and calculate distance to visual's center
    auto const visibility = frustum.testSAABB(
        bvolume.sphere.P,
        bvolume.sphere.R,
        bvolume.box.data(),
        mask
    );

    if (visibility == fcvNone)
    {
        return;
    }

    switch (vis.Type)
    {
    case MT_NORMAL:
    {
        AddVisualInstance(vis);
        break;
    }
    case MT_HIERRARHY:
    {
        auto const &hvis = static_cast<FHierrarhyVisual&>(vis);
        if (visibility == fcvPartial)
        {
            for (auto const &child : hvis.children)
            {
                AddStatic(*child, frustum);
            }
        }
        else
        {
            for (auto const &child : hvis.children)
            {
                AddStaticLeaf(*child);
            }
        }
        break;
    }

    default:
        break;
    }
}


//-----------------------------------------------------------------------------
void
FrontEnd::AddStaticLeaf
        ( dxRender_Visual &vis
        )
{
    switch (vis.Type)
    {
    case MT_NORMAL:
    {
        AddVisualInstance(vis);
        break;
    }
    case MT_HIERRARHY:
    {
        auto const &hvis = static_cast<FHierrarhyVisual&>(vis);
        for (auto const &child : hvis.children)
        {
            AddVisualInstance(*child);
        }
        break;
    }
    default:
        break;
    }
}


//-----------------------------------------------------------------------------
void
FrontEnd::AddVisualInstance
        ( dxRender_Visual &vis
        )
{
}


//-----------------------------------------------------------------------------
void
FrontEnd::Calculate()
{
    M_SCOPED;

    FrameData &fd = frame_datas_[0]; // TODO: take from ctx num
    static IRender_Sector* sector = nullptr;

    // Detect current sector
    if (!fd.vPrevCameraPos.similar(::Device.vCameraPosition, 1e-4/*EPS_S*/))
    {
        M_SCOPED_N("DetectSector");

        sector = detectSector(::Device.vCameraPosition);
        fd.vPrevCameraPos.set(::Device.vCameraPosition);
    }

    ViewBase.CreateFromMatrix(::Device.mFullTransform, FRUSTUM_P_LRTB | FRUSTUM_P_NEAR);

    // Collect static geometry
    if (true)
    {
        PortalTraverser.traverse(
            sector,
            ViewBase,
            ::Device.vCameraPosition,
            ::Device.mFullTransform,
            CPortalTraverser::VQ_DEFAULT
        );

        for (auto const sector : PortalTraverser.r_sectors)
        {
            auto const *S = static_cast<CSector*>(sector);
            for (auto frustum : S->r_frustums)
            {
                AddStatic(*S->root(), frustum);
            }
        }
    }

    // Collect dynamic geometry
    if (false)
    {
        // ...
    }

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

#include "stdafx.h"

#include "console/console.h"
#include "backend/backend.h"
#include "device/device.h"
#include "frontend/render.h"
#include "frontend/tasks/menu_pass.h"
#include "frontend/tasks/scene_pass_dummy.h"
#include "legacy/raffle/FHierrarhyVisual.h"
#include "resources/manager.h"

#include <xrEngine/IGame_Persistent.h>
#include <xrCore/FMesh.hpp>


using namespace xrrng;

struct VisibilityData
{
    std::uint32_t                                   num;
    svector<std::uint32_t, R_MAX_STATIC_OBJS>       instances; // a bit wrong but handy
};

FrontEnd frontend;

//-----------------------------------------------------------------------------
void
FrontEnd::OnDeviceCreate
        ( char const *shaders_lib_name
        )
{
    backend.OnDeviceCreate();
    resources.OnDeviceCreate(shaders_lib_name);

    auto const num_frames = backend.GetContextsNum();

    menu  = std::make_unique<MenuPass>();
    scene = std::make_unique<ScenePass>();

    frame_datas_.resize(num_frames);

    for (int i = 0; i < num_frames; ++i)
    {
        frame_datas_[i].StaticGeometryList.reserve(R_MAX_STATIC_OBJS);

        // Create resources
        frame_datas_[i].base_depth = device.AllocateDeviceImage(
            {
                device.swapchain_params_.extent.width,
                device.swapchain_params_.extent.height,
                1
            },
            device.swapchain_params_.depthFormat,
            ImageType::Depth
        );
        frame_datas_[i].base_depth->SetName("base_depth#" + std::to_string(i));

        vis_instances_.emplace_back(
            StagedBuffer
            {
                sizeof(VisibilityData),
                BufferType::Storage
            }
        ).SetName("vis_instances#" + std::to_string(i));
        // ...
    }

    ld.instances_data_ = std::make_unique<StagedBuffer>(
        StagedBuffer{
            sizeof(InstanceData) * R_MAX_STATIC_OBJS,
            BufferType::Storage
        }
    );
    ld.instances_data_->SetName("instances_data");

    // Pre-record command buffers

    /*
     * Here should be rendergraph
     */
    menu->Init();
    scene->Init();
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

ICF float CalcSSA(float& distSQ, Fvector& C, float R)
{
    distSQ = ::Device.vCameraPosition.distance_to_sqr(C) + EPS;
    return R / distSQ;
}

//-----------------------------------------------------------------------------
void
FrontEnd::AddVisualInstance
        ( dxRender_Visual &vis
        )
{
    float distSQ;
    float SSA = CalcSSA(
        distSQ,
        vis.vis.sphere.P,
        vis.vis.sphere.R
    );
    
    if (SSA <= params.f_ssa_discard)
        return;

    // Add to list
    frame_datas_[0].StaticGeometryList.push_back({ SSA, &vis });
}


//-----------------------------------------------------------------------------
void
FrontEnd::Calculate()
{
    M_SCOPED;

    auto const ctxId = device.State.imageIndex;

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
        M_SCOPED_N("CollectStatic");

        /*
         * TODO: Here we can try to re-use static geometry list from the previous
         * frame in case if camera pos & dir weren't changed too much
         */
        fd.StaticGeometryList.clear(); // TODO: Previous FD! wouldn't it be realloc?

        // Gather visible sectors
        PortalTraverser.traverse(
            sector,
            ViewBase,
            ::Device.vCameraPosition,
            ::Device.mFullTransform,
            CPortalTraverser::VQ_DEFAULT
        );

        // Collect sectors visuals
        for (auto const sector : PortalTraverser.r_sectors)
        {
            auto const *S = static_cast<CSector*>(sector);
            for (auto frustum : S->r_frustums)
            {
                AddStatic(*S->root(), frustum);
            }
        }

        // Sort front to back
        std::sort(
            fd.StaticGeometryList.begin(),
            fd.StaticGeometryList.end(),
            [](const auto& f, const auto& s)
            {
                return f.first > s.first;
            }
        );

        // Upload PVS data for occ
        auto *data = static_cast<VisibilityData*>(vis_instances_[ctxId].GetPointer());
        data->instances.clear();
        data->num = fd.StaticGeometryList.size();
        for (auto &instance : fd.StaticGeometryList)
        {
            data->instances.push_back(instance.second->vis_id);
        }
        vis_instances_[ctxId].Transfer();
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
    M_SCOPED;

    auto &cmdL = backend.GetCommandBuffer();

    // Update resources
    // ...

    // Do streaming
    // ...
    device.ProcessTransfer();

    // Menu rendering
    if (g_pGamePersistent->IsMainMenuActive())
    {
        menu->Exec(cmdL);
        return;
    }

    // Submit list
    // ...
    scene->Exec(cmdL);
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

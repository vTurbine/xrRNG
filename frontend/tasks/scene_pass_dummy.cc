#include "stdafx.h"

#include "scene_pass_dummy.h"

#include "device/device.h"
#include "frontend/render.h"


using namespace xrrng;

ScenePass::ScenePass()
    : RenderTask("ScenePass")
{
}


void
ScenePass::Init()
{
    extern void Initialize_01();

    Initialize_01();
}


void
ScenePass::Exec
        ( vk::CommandBuffer &cmdL
        )
{
    device.GpuMarkerBegin(
        cmdL,
        name_,
        { 1.0f, 1.0f, 0.0f, 1.0f }
    );
     
    CopyClear(cmdL);
    DebugDrawStaticBboxes(cmdL);

    device.GpuMarkerEnd(cmdL);
}

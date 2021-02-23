#include "stdafx.h"

#include "device/device.h"
#include "scene_pass_dummy.h"
#include "legacy/debug/du_box.h"


/*
 * Given a list of static objects in the scene, draw their bounding boxes.
 * Will be used to estimate occ efficiency.
 */

using namespace xrrng;

namespace // TODO: put inside of class
{
bool initialized{ false };

BufferPtr                       box_vertices;
BufferPtr                       box_vertices_cpu;

std::vector<vk::DescriptorSet>  sets;
std::vector<vk::RenderPass>     passes;
std::vector<vk::Framebuffer>    frame_buffers;
std::vector<vk::PipelineLayout> layouts;
std::vector<vk::Pipeline>       pipelines;

void Initialize()
{
    // box vertices
    auto constexpr verticesSize = sizeof(Fvector) * DU_BOX_NUMVERTEX2;
    box_vertices_cpu = device.AllocateHostBuffer(verticesSize);
    memcpy(box_vertices_cpu->allocation_info.pMappedData, du_box_vertices2, verticesSize);
    box_vertices = device.AllocateDeviceBuffer(verticesSize, BufferType::Vertex);
    box_vertices->SetName("du_box");

    // TODO: setup transfer

    for (int i = 0; i < device.State.colorImages.size(); ++i)
    {
        // Prepare pass resources
        std::array<vk::AttachmentDescription, 2> attachments;
        attachments[0] // color
            .setLoadOp(vk::AttachmentLoadOp::eDontCare)  // we already have clear pass before
            .setStoreOp(vk::AttachmentStoreOp::eStore);

        attachments[1] // depth
            .setLoadOp(vk::AttachmentLoadOp::eDontCare)  // we already have clear pass before
            .setStoreOp(vk::AttachmentStoreOp::eStore);

        // framebuffer
        {
            const auto& fbCreateInfo = vk::FramebufferCreateInfo();
            frame_buffers.emplace_back(device.m_Device->createFramebuffer(fbCreateInfo));
        }

        // the pass
        {
            const auto& passCreateInfo = vk::RenderPassCreateInfo()
                .setAttachments(attachments);

            passes.emplace_back(device.m_Device->createRenderPass(passCreateInfo));
        }
    }
}
}

void
ScenePass::DebugDrawStaticBboxes
        ( vk::CommandBuffer &cmdb
        )
{
    static int i = 0;

    if (!initialized)
    {
        Initialize();
        initialized = true;
    }

    const auto& passInfo = vk::RenderPassBeginInfo()
        .setRenderPass(passes[i]);
    cmdb.beginRenderPass(passInfo, {});
    {
        cmdb.bindVertexBuffers(
            0,      // first binding = the only buffer
            { box_vertices->buffer },
            { 0 }   // offset
        );

        cmdb.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            layouts[i],
            0,  // first set
            sets,
            {}  // dynamic offsets
        );

        cmdb.bindPipeline(
            vk::PipelineBindPoint::eGraphics,
            pipelines[i]
        );

        cmdb.draw(
            DU_BOX_NUMVERTEX2,
            1,  // TODO: num static prims
            0,  // first vertex
            0   // first instance
        );
    }
    cmdb.endRenderPass();
    ++i;
}

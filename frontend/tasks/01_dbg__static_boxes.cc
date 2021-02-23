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
std::vector<bool> initialized{ false };

BufferPtr                       box_vertices;
BufferPtr                       box_vertices_cpu;

std::vector<vk::DescriptorSet>  sets;
std::vector<vk::RenderPass>     passes;
std::vector<vk::Framebuffer>    frame_buffers;
std::vector<vk::PipelineLayout> layouts;
std::vector<vk::Pipeline>       pipelines;
}

void
Initialize_01()
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
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setFormat(device.swapchain_params_.colorFormat)
            .setInitialLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

        attachments[1] // depth
            .setLoadOp(vk::AttachmentLoadOp::eDontCare)  // we already have clear pass before
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setFormat(device.swapchain_params_.depthFormat)
            .setInitialLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
            .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

        std::array<vk::AttachmentReference, 2> refs;
        refs[0]
            .setAttachment(0)
            .setLayout(vk::ImageLayout::eColorAttachmentOptimal);
        refs[1]
            .setAttachment(1)
            .setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

        // subpass
        const auto& subpassDesc = vk::SubpassDescription()
            .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
            .setColorAttachments(std::array{ refs[0] })
            .setPDepthStencilAttachment(&refs[1]);

        // the pass
        {
            const auto& passCreateInfo = vk::RenderPassCreateInfo()
                .setAttachments(attachments)
                .setSubpasses(std::array{ subpassDesc });

            passes.emplace_back(device.m_Device->createRenderPass(passCreateInfo));
        }

        // framebuffer
        {
            const auto& depthView = frontend.frame_datas_[i].base_depth->GetView();
            const auto& fbCreateInfo = vk::FramebufferCreateInfo()
                .setRenderPass(passes[i])
                .setWidth(device.swapchain_params_.extent.width)
                .setHeight(device.swapchain_params_.extent.height)
                .setAttachments(std::array{ device.State.colorViews[i], depthView })
                .setLayers(1);
            frame_buffers.emplace_back(device.m_Device->createFramebuffer(fbCreateInfo));
        }
    }
}

void
ScenePass::DebugDrawStaticBboxes
        ( vk::CommandBuffer &cmdb
        )
{
    auto const i = device.State.imageIndex;

    const auto& passInfo = vk::RenderPassBeginInfo()
        .setRenderPass(passes[i])
        .setFramebuffer(frame_buffers[i]);
    cmdb.beginRenderPass(passInfo, {});
    {
        /*
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
        */
    }
    cmdb.endRenderPass();
}

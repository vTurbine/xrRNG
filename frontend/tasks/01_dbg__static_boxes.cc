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

std::vector<vk::UniquePipelineLayout>   layouts;
std::vector<vk::UniquePipeline>         pipelines;
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

    device.AddTransfer(
        box_vertices_cpu,
        box_vertices,
        0,  // offset
        verticesSize
    );

    // shaders
    auto vsSb = FS.r_open("$game_shaders$", "vk//dummy_box_vs.sb");
    R_ASSERT(vsSb);
    auto const vsCodeSize = vsSb->length();
    std::vector<uint32_t> vsCode(vsCodeSize / 4);
    vsSb->r(vsCode.data(), vsCodeSize);
    auto const &vsCreateInfo = vk::ShaderModuleCreateInfo()
        .setCode(vsCode)
        .setCodeSize(vsCodeSize);
    auto vs = device.m_Device->createShaderModuleUnique(vsCreateInfo);
    auto const &vsStage = vk::PipelineShaderStageCreateInfo()
        .setModule(vs.get())
        .setStage(vk::ShaderStageFlagBits::eVertex)
        .setPName("main_vs");
    vsSb->close();

    auto psSb = FS.r_open("$game_shaders$", "vk//dummy_box_ps.sb");
    R_ASSERT(psSb);
    auto const psCodeSize = psSb->length();
    std::vector<uint32_t> psCode(psCodeSize / 4);
    psSb->r(psCode.data(), psCodeSize);
    auto const &psCreateInfo = vk::ShaderModuleCreateInfo()
        .setCode(psCode)
        .setCodeSize(psCodeSize);
    auto ps = device.m_Device->createShaderModuleUnique(psCreateInfo);
    auto const &psStage = vk::PipelineShaderStageCreateInfo()
        .setModule(ps.get())
        .setStage(vk::ShaderStageFlagBits::eFragment)
        .setPName("main_ps");
    psSb->close();

    // fixed functions
    auto const &vid = vk::VertexInputBindingDescription()
        .setInputRate(vk::VertexInputRate::eVertex)
        .setStride(3 * 4);
    auto const &vad = vk::VertexInputAttributeDescription()
        .setFormat(vk::Format::eR32G32B32Sfloat);

    auto const &vi = vk::PipelineVertexInputStateCreateInfo()
        .setVertexBindingDescriptions(std::array{ vid })
        .setVertexAttributeDescriptions(std::array{ vad });
    auto const &ia = vk::PipelineInputAssemblyStateCreateInfo()
        .setTopology(vk::PrimitiveTopology::eTriangleList);
    auto const &vp = vk::PipelineViewportStateCreateInfo()
        .setScissors(std::array
            {
                vk::Rect2D()
                    .setExtent(device.swapchain_params_.extent)
            }
        )
        .setViewports(std::array
            {
                vk::Viewport()
                    .setWidth(device.swapchain_params_.extent.width)
                    .setHeight(device.swapchain_params_.extent.height)
            }
        );
    auto const &rs = vk::PipelineRasterizationStateCreateInfo()
        .setLineWidth(1.0);
    auto const &ms = vk::PipelineMultisampleStateCreateInfo();
    auto const &cba = vk::PipelineColorBlendAttachmentState()
        .setColorWriteMask(
            vk::ColorComponentFlagBits::eA |
            vk::ColorComponentFlagBits::eR |
            vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB
        );
    auto const &cb = vk::PipelineColorBlendStateCreateInfo()
        .setAttachments(std::array{ cba });

    auto const &ds = vk::PipelineDepthStencilStateCreateInfo()
        .setDepthWriteEnable(true)
        .setDepthTestEnable(true)
        .setDepthCompareOp(vk::CompareOp::eGreater); // inverse

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
        auto const &subpassDesc = vk::SubpassDescription()
            .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
            .setColorAttachments(std::array{ refs[0] })
            .setPDepthStencilAttachment(&refs[1]);

        // the pass
        {
            auto const &passCreateInfo = vk::RenderPassCreateInfo()
                .setAttachments(attachments)
                .setSubpasses(std::array{ subpassDesc });

            passes.emplace_back(device.m_Device->createRenderPass(passCreateInfo));
        }

        // framebuffer
        {
            auto const &depthView = frontend.frame_datas_[i].base_depth->GetView();
            auto const &fbCreateInfo = vk::FramebufferCreateInfo()
                .setRenderPass(passes[i])
                .setWidth(device.swapchain_params_.extent.width)
                .setHeight(device.swapchain_params_.extent.height)
                .setAttachments(std::array{ device.State.colorViews[i], depthView })
                .setLayers(1);
            frame_buffers.emplace_back(device.m_Device->createFramebuffer(fbCreateInfo));
        }
    
        // pipeline layout
        auto const &pipelineLayoutInfo = vk::PipelineLayoutCreateInfo()
            .setSetLayoutCount(0)
            .setPushConstantRangeCount(0);
        auto const &layout = layouts.emplace_back(
            device.m_Device->createPipelineLayoutUnique(pipelineLayoutInfo)
        );

        // pipeline
        auto const &pipelineInfo = vk::GraphicsPipelineCreateInfo()
            .setPVertexInputState(&vi)
            .setPInputAssemblyState(&ia)
            .setStages(std::array{ vsStage, psStage })
            .setPViewportState(&vp)
            .setPRasterizationState(&rs)
            .setPMultisampleState(&ms)
            .setPColorBlendState(&cb)
            .setPDepthStencilState(&ds)
            .setLayout(layout.get())
            .setRenderPass(passes[i]);
        pipelines.push_back(
            device.m_Device->createGraphicsPipelineUnique(
                {},  // cache
                pipelineInfo
            )
        );
    }
}

void
ScenePass::DebugDrawStaticBboxes
        ( vk::CommandBuffer &cmdb
        )
{
    auto const i = device.State.imageIndex;

    auto const &passInfo = vk::RenderPassBeginInfo()
        .setRenderPass(passes[i])
        .setFramebuffer(frame_buffers[i]);
    cmdb.beginRenderPass(passInfo, {});
    {
        cmdb.bindVertexBuffers(
            0,      // first binding = the only buffer
            { box_vertices->buffer },
            { 0 }   // offset
        );

        /*
        cmdb.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            layouts[i],
            0,  // first set
            sets,
            {}  // dynamic offsets
        );
        */

        cmdb.bindPipeline(
            vk::PipelineBindPoint::eGraphics,
            pipelines[i].get()
        );

        cmdb.draw(
            DU_BOX_NUMVERTEX2,
            1,  //frontend.frame_datas_[0].StaticGeometryList.size(),  // num static prims
            0,  // first vertex
            0   // first instance
        );
    }
    cmdb.endRenderPass();
}

#include "device/device.h"
#include "frontend/render.h"
#include "resources/blender_screen_set.h"

#include "resources/blender_compiler.h"

/*!
 * Pipeline states stencils
 */
static vk::PipelineVertexInputStateCreateInfo state__vertex_input =
{};
static vk::PipelineInputAssemblyStateCreateInfo state__input_assembly =
{
    {},
    vk::PrimitiveTopology::eTriangleList, // FIXME: this should be defined by renderer somehow
    0
};
static vk::PipelineTessellationStateCreateInfo state__tesselation =
{};
static vk::PipelineViewportStateCreateInfo state__viewport =
{};
static vk::PipelineRasterizationStateCreateInfo state__rasterization =
{};
static vk::PipelineMultisampleStateCreateInfo state__multisampling =
{};
static vk::PipelineDepthStencilStateCreateInfo state__depth_stencil =
{};
static vk::PipelineColorBlendStateCreateInfo state__color_blend =
{};
static vk::PipelineDynamicStateCreateInfo state__dynamic =
{};


static vk::GraphicsPipelineCreateInfo pipeline_create_info =
{
    {}, // flags TODO: Allow derivatives
    0, // shader stages count
    nullptr, // shader stages pointer
    &state__vertex_input,
    &state__input_assembly,
    &state__tesselation,
    &state__viewport,
    &state__rasterization,
    &state__multisampling,
    &state__depth_stencil,
    &state__color_blend,
    &state__dynamic
};


void
BlenderCompiler::Compile
        ( ShaderElement &element
        )
{
    /* Store pointer to target element for further
     * updates while going through blender passes
     */
    shader_element = &element;

    // TBI
    blender->Compile(*this);
}

void
BlenderCompiler::PassBegin
        ( const std::string &vertex_shader
        , const std::string &fragment_shader
        )
{
    /* Load and compile shaders
     */
    pass.vertex_shader =
        frontend.resources_->CreateVertexShader(vertex_shader);
    pass.fragment_shader =
        frontend.resources_->CreateFragmentShader(fragment_shader);

    constant_table.Merge(pass.vertex_shader->constant_table);
    constant_table.Merge(pass.fragment_shader->constant_table);
}


void
BlenderCompiler::PassTexture
        ( const std::string &resource_name
        , const std::string &texture_name
        )
{
    std::string texture{ texture_name };
    //const auto &iterator = textures.find(texture);
}


void
BlenderCompiler::PassZtest
        ( bool z_test
        , bool z_write
        , bool invert_test /* = false */
        )
{
    state__depth_stencil.setDepthTestEnable(z_test);
    state__depth_stencil.setDepthWriteEnable(z_write);
    if (invert_test)
    {
        state__depth_stencil.setDepthCompareOp(vk::CompareOp::eGreater);
    }
    else
    {
        state__depth_stencil.setDepthCompareOp(vk::CompareOp::eLess);
    }
}


void
BlenderCompiler::PassEnd()
{
    CreatePipeline();

    /* Register shader pass resource and add it
     * into parent shader element
     */
    const auto pass_ptr = frontend.resources_->CreateShaderPass(pass);
    shader_element->shader_passes.push_back(pass_ptr);
}


/*!
 * \brief   Create GPO for current render pass
 *
 * TODO: probably most of the structures can be static stencils to speedup
 *       the pipeline creation.
 */
void
BlenderCompiler::CreatePipeline()
{
    /* Shader stages
     */
    std::vector<vk::PipelineShaderStageCreateInfo> shader_stages;

    if (pass.vertex_shader)
    {
        const auto &vertex = vk::PipelineShaderStageCreateInfo()
            .setModule(pass.vertex_shader->module.get())
            .setPName(pass.vertex_shader->file_name.c_str())
            .setStage(vk::ShaderStageFlagBits::eVertex);

        shader_stages.push_back(vertex);
    }

    if (pass.fragment_shader)
    {
        const auto &fragment = vk::PipelineShaderStageCreateInfo()
            .setModule(pass.fragment_shader->module.get())
            .setPName(pass.fragment_shader->file_name.c_str())
            .setStage(vk::ShaderStageFlagBits::eFragment);

        shader_stages.push_back(fragment);
    }

    /* Update states
     */

    // Vertex inputs
    const auto input_binding_description = vk::VertexInputBindingDescription()
        .setBinding(0)
        .setStride(pass.vertex_shader->stride_size)
        .setInputRate(vk::VertexInputRate::eVertex);

    state__vertex_input
        .setVertexBindingDescriptionCount(1)
        .setPVertexBindingDescriptions(&input_binding_description)
        .setVertexAttributeDescriptionCount(pass.vertex_shader->inputs.size())
        .setPVertexAttributeDescriptions(pass.vertex_shader->inputs.data());

    // Viewport
    const auto viewport = vk::Viewport()
        .setWidth(hw.draw_rect.width)
        .setHeight(hw.draw_rect.height);
    // TODO: max depth!

    const auto scissor = vk::Rect2D()
        .setExtent(hw.draw_rect);

    state__viewport
        .setViewportCount(1)
        .setPViewports(&viewport)
        .setScissorCount(1)
        .setPScissors(&scissor);

    // Create the pipeline
    const auto &pipeline_layout = vk::PipelineLayout(); // TODO

    pipeline_create_info
        .setStageCount(shader_stages.size())
        .setPStages(shader_stages.data())
        .setLayout(pipeline_layout)
        .setRenderPass({}) // It's easy since we have only one render pass
        .setSubpass(0); // This one is hardest

    pass.pipeline =
        hw.device->createGraphicsPipeline(nullptr, pipeline_create_info);
}

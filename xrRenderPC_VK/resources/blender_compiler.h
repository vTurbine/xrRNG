#ifndef RESOURCES_BLENDER_COMPILER_H_
#define RESOURCES_BLENDER_COMPILER_H_

#include <array>
#include <memory>
#include <string>
#include <vector>

#include "resources/blender.h"
#include "resources/pipeline_shader.h"
#include "resources/shader.h"
#include "resources/shader_element.h"
#include "resources/shader_pass.h"

constexpr std::size_t max_shader_stages = 2; ///< Vertex, Fragment

class BlenderCompiler
{
public:
    void Compile(ShaderElement &element);
    void PassBegin(const std::string &vertex_shader, const std::string &fragment_shader);
    void PassTexture(const std::string &name, const std::string &texture);
    void PassSampler(const std::string &name) {}
    void PassZtest(bool ztest, bool zwrite) {}
    void PassBlend(bool alpha, u32 source, u32 destination, bool test, u32 key) {}
    void PassLightingFog(bool lighting, bool fog) {}
    void PassEnd();

    std::vector<std::string> textures;
    std::vector<std::string> constants;
    std::vector<std::string> matrices;

    std::shared_ptr<ShaderElement> shader_element; ///< Compilation target
    ShaderElementType current_element; ///< Compilation target stage
    ShaderPass pass; ///< Intermediate pass data

    std::shared_ptr<Blender> blender;
    bool detail = false;

private:
    /** Graphics pipeline stages  */
    std::array<vk::PipelineShaderStageCreateInfo, max_shader_stages> shader_stages_;
};

#endif // RESOURCES_BLENDER_COMPILER_H_

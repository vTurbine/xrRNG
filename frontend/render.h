#ifndef FRONTEND_RENDER_H_
#define FRONTEND_RENDER_H_

#include "frontend/frame_data.h"
#include "legacy/legacy.h"

#include <xrEngine/pure.h>
#include <xrEngine/Render.h>


class FrontEnd final
    : public LegacyInterface
{
public:
    FrontEnd() = default;
    ~FrontEnd() final = default;

    void OnDeviceCreate(char const *shaders_lib_name) final;
    void OnDeviceDestroy(bool keep_textures) final;

    void Calculate() final;
    void Render() final;

    void SetCacheXform(Fmatrix &mView, Fmatrix &mProject) final;

private:
    std::vector<vk::UniqueCommandBuffer>    scene_cmds_;
    std::vector<FrameData>                  frame_datas_;
};

extern FrontEnd frontend;

#endif // FRONTEND_RENDER_H_

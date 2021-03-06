#ifndef FRONTEND_RENDER_H_
#define FRONTEND_RENDER_H_

#include "frontend/frame_data.h"
#include "legacy/legacy.h"

#include <xrEngine/pure.h>
#include <xrEngine/Render.h>


class RenderTask
{
public:
    explicit RenderTask(char const *name) { name_ = name; }
    virtual void Init() = 0;
    virtual void Exec(vk::CommandBuffer& cmdL) = 0;

protected:
    std::string name_;
};


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

//private:
    void AddStatic(dxRender_Visual &vis, CFrustum &frustum);
    void AddStaticLeaf(dxRender_Visual &vis);
    void AddVisualInstance(dxRender_Visual &vis);

    std::vector<FrameData>      frame_datas_;
    std::vector<StagedBuffer>   vis_instances_;

    std::unique_ptr<RenderTask> menu;
    std::unique_ptr<RenderTask> scene;
};

extern FrontEnd frontend;

#endif // FRONTEND_RENDER_H_

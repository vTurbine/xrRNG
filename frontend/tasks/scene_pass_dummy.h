#include "frontend/render.h"

class ScenePass final
    : public RenderTask
{
public:
    ScenePass();

    void Init() final;
    void Exec(vk::CommandBuffer &cmdL) final;

private:
    void CopyClear(vk::CommandBuffer &cmdb);
    void DebugDrawStaticBboxes(vk::CommandBuffer &cmdb);
};

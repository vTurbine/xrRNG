#include "frontend/render.h"

class ScenePass
    : public RenderTask
{
public:
    ScenePass();

    void Build(vk::CommandBuffer &cmdL) final;

private:
    void CopyClear(vk::CommandBuffer &cmdb);
    void DebugDrawStaticBboxes(vk::CommandBuffer &cmdb);
};

#include "frontend/render.h"

class ScenePass
    : public RenderTask
{
public:
    ScenePass();

    void Build(vk::CommandBuffer &cmdL) final;
};

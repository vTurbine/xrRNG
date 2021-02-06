#include "frontend/render.h"

class MenuPass
    : public RenderTask
{
public:
    MenuPass();

    void Build(vk::CommandBuffer &cmdL) final;
};

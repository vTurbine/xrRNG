#include "frontend/render.h"

class MenuPass
    : public RenderTask
{
public:
    MenuPass();

    void Init() final;
    void Exec(vk::CommandBuffer &cmdL) final;
};

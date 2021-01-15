#ifndef FACTORY_THUNDERBOLT_RENDER_H_
#define FACTORY_THUNDERBOLT_RENDER_H_

#include <Include/xrRender/ThunderboltRender.h>


class fThunderboltRender
    : public IThunderboltRender
{
public:
    fThunderboltRender() = default;
    ~fThunderboltRender() = default;

    void Copy(IThunderboltRender& obj) override;
    void Render(CEffect_Thunderbolt& owner) override;
};

#endif // FACTORY_THUNDERBOLT_RENDER_H_

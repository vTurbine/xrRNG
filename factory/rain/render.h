#ifndef FACTORY_RAIN_RENDER_H_
#define FACTORY_RAIN_RENDER_H_

#include <Include/xrRender/RainRender.h>


class fRainRender
    : public IRainRender
{
public:
    fRainRender() = default;
    ~fRainRender() = default;

    void Copy(IRainRender& obj) override;
    void Render(CEffect_Rain& owner) override;
    const Fsphere& GetDropBounds() const override;
};

#endif // FACTORY_RAIN_RENDER_H_

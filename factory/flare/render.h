#ifndef FACTORY_FLARE_RENDER_H_
#define FACTORY_FLARE_RENDER_H_

#include <Include/xrRender/LensFlareRender.h>


class fFlareRender
    : public IFlareRender
{
public:
    fFlareRender() = default;
    ~fFlareRender() = default;

    void Copy(IFlareRender& obj) override;
    void CreateShader(LPCSTR shader, LPCSTR textures) override;
    void DestroyShader() override;
};

#endif // FACTORY_FLARE_RENDER_H_

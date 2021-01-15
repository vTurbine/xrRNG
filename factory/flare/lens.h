#ifndef FACTORY_FLARE_LENS_H_
#define FACTORY_FLARE_LENS_H_

#include <Include/xrRender/LensFlareRender.h>


class fLensFlareRender
    : public ILensFlareRender
{
public:
    fLensFlareRender() = default;
    ~fLensFlareRender() = default;

    void Copy(ILensFlareRender& obj) override;
    void Render(CLensFlare& owner, BOOL sun, BOOL flares, BOOL gradient) override;
    void OnDeviceCreate() override;
    void OnDeviceDestroy() override;
};

#endif // FACTORY_FLARE_LENS_H_

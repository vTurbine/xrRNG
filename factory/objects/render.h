#ifndef FACTORY_OBJECTS_RENDER_H_
#define FACTORY_OBJECTS_RENDER_H_

#include <Include/xrRender/ObjectSpaceRender.h>


class fObjectSpaceRender
    : public IObjectSpaceRender
{
public:
    fObjectSpaceRender() = default;
    ~fObjectSpaceRender() = default;

    void Copy(IObjectSpaceRender& obj) override;
    void dbgRender() override;
    void dbgAddSphere(const Fsphere& sphere, u32 colour) override;
    void SetShader() override;
};

#endif // FACTORY_OBJECTS_RENDER_H_

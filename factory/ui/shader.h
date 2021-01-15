#ifndef FACTORY_UI_SHADER_H_
#define FACTORY_UI_SHADER_H_

#include <Include/xrRender/UIShader.h>


class fUIShader
    : public IUIShader
{
    friend class UiRender;

public:
    fUIShader() = default;
    ~fUIShader() = default;

    // Interface implementation
    void Copy(IUIShader& obj) override;
    void create(LPCSTR shaderName, LPCSTR textureList = nullptr) override;
    bool inited() override;
    void destroy() override;
};

#endif // FACTORY_UI_SHADER_H_

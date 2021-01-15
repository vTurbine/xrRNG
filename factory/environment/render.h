#ifndef FACTORY_ENVIRONMENT_RENDER_H_
#define FACTORY_ENVIRONMENT_RENDER_H_

#include <Include/xrRender/EnvironmentRender.h>
#include <Include/xrRender/particles_systems_library_interface.hpp>


class fEnvironmentRender
    : public IEnvironmentRender
{
public:
    fEnvironmentRender() = default;
    ~fEnvironmentRender() = default;

    void Copy(IEnvironmentRender& obj) override;
    void OnFrame(CEnvironment& env) override;
    void OnLoad() override;
    void OnUnload() override;
    void RenderSky(CEnvironment& env) override;
    void RenderClouds(CEnvironment& env) override;
    void OnDeviceCreate() override;
    void OnDeviceDestroy() override;
    particles_systems::library_interface const& particles_systems_library() override;
};

#endif // FACTORY_ENVIRONMENT_RENDER_H_

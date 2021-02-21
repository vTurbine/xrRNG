#include "stdafx.h"

#include "console/console.h"
#include "frontend/render.h"
#include "factory/factory.h"
#include "ui/ui_render.h"

#include <xrEngine/Engine.h>
#include <xrEngine/EngineAPI.h>

#include <volk/volk.h>


namespace xrrng
{

/*!
 * Statically created objects
 */
RenderFactory   factory;
UiRender        ui;


RENDERDOC_API_1_4_1 rdApi;
constexpr pcstr RENDERER_NAME = "xrRNG";

struct ModuleImpl final
    : public RendererModule
{
    xr_vector<pcstr> const&
    ObtainSupportedModes() final
    {
        static xr_vector<pcstr> modes{};

        auto const result = volkInitialize();
        if (result != VK_SUCCESS)
        {
            Msg("! Unable to initialize Vulkan loader. Make sure you have Vulkan runtime installed");
            Msg("  See https://vulkan.lunarg.com/sdk/home for details");
            return modes;
        }

        auto const apiVersion = volkGetInstanceVersion();
        Msg("* Vulkan Instance ver. %d.%d.%d",
        VK_VERSION_MAJOR(apiVersion),
        VK_VERSION_MINOR(apiVersion),
        VK_VERSION_PATCH(apiVersion));

        if (apiVersion >= VK_API_VERSION_1_1)
        {
            modes.emplace_back(RENDERER_NAME);
        }

        return modes;
    }

    void
    SetupEnv
        ( pcstr mode
        )
        final
    {
#if VK_RENDERDOC_EN
        if (HMODULE rdLib = GetModuleHandle("renderdoc.dll"))
        {
            auto RENDERDOC_GetAPI = reinterpret_cast<pRENDERDOC_GetAPI>(
                GetProcAddress( rdLib
                              , "RENDERDOC_GetAPI"
                )
            );
            auto const ret = RENDERDOC_GetAPI( eRENDERDOC_API_Version_1_4_1
                                       , reinterpret_cast<void **>(&rdApi)
            );

            if (ret != 1)
            {
                Msg("! [RVK] Failed to load RenderDoc API");
            }
        }
#endif

        GEnv.Render         = &frontend;
        GEnv.RenderFactory  = &factory;
        GEnv.DU             = nullptr;
        GEnv.UIRender       = &ui;
#ifdef DEBUG
        GEnv.DRender        = nullptr;
#endif
        InitConsoleVK();
    }
};

ModuleImpl module;

}

extern "C"
{

XR_EXPORT RendererModule *
GetRendererModule()
{
    return &xrrng::module;
}

}

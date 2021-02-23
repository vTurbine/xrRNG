#include "stdafx.h"

#include "device/device.h"

#include "debug.h"


namespace
{
VkBool32
DebugUtilsMessengerCallback
        ( VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeveritry
        , VkDebugUtilsMessageTypeFlagsEXT             messageType
        , VkDebugUtilsMessengerCallbackDataEXT const *pCallbackData
        , void                                       *pUserData
        )
{
    // TODO
    return false;
}

#if VK_RENDERDOC_EN
RENDERDOC_API_1_1_2 rdApi;
#endif
}

using namespace xrrng;

//-----------------------------------------------------------------------------
void
Device::InitDebug()
{
#if VK_RENDERDOC_EN
    HMODULE rdLib = GetModuleHandle("renderdoc.dll");
    if (rdLib == 0)
    {
        rdLib = LoadLibrary("renderdoc.dll");
    }
    if (rdLib)
    {
        auto RENDERDOC_GetAPI = reinterpret_cast<pRENDERDOC_GetAPI>(
            GetProcAddress(
                rdLib,
                "RENDERDOC_GetAPI"
            )
        );
        auto const ret = RENDERDOC_GetAPI(
            eRENDERDOC_API_Version_1_1_2,
            reinterpret_cast<void **>(&rdApi)
        );

        if (ret != 1)
        {
            Msg("! [RVK] Failed to load RenderDoc API");
        }
    }
#endif
}


//-----------------------------------------------------------------------------
void
Device::GpuMarkerBegin
        ( vk::CommandBuffer             cmdb
        , std::string const             &name
        , std::array<float, 4> const    &color
        ) const
{
    const auto& markerInfo = vk::DebugUtilsLabelEXT()
        .setPLabelName(name.c_str())
        .setColor(color);
    cmdb.beginDebugUtilsLabelEXT(markerInfo);
}


//-----------------------------------------------------------------------------
void
Device::GpuMarkerEnd
        ( vk::CommandBuffer             cmdb
        ) const
{
    cmdb.endDebugUtilsLabelEXT();
}

#ifndef STDAFX_H_
#define STDAFX_H_

#include "config.h"

#include <Common/Common.hpp>
#include <xrCore/xrCore.h>

#define VK_NO_PROTOTYPES
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

#if VK_RENDERDOC_EN
#   include <renderdoc_app.h>

extern RENDERDOC_API_1_4_1 rdApi;
#endif

#endif // STDAFX_H_

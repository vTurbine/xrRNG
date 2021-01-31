#ifndef STDAFX_H_
#define STDAFX_H_

#include "config.h"
#include "device/debug.h"

#include <Common/Common.hpp>
#include <xrCore/xrCore.h>

#define VK_NO_PROTOTYPES
#if defined(WINDOWS)
#  define VK_USE_PLATFORM_WIN32_KHR
#endif
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>

#endif // STDAFX_H_

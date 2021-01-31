#include "stdafx.h"

#include "Debug.h"
#include "HW.h"

namespace
{
VkBool32
DebugUtilsMessengerCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeveritry,
    VkDebugUtilsMessageTypeFlagsEXT             messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void*                                       pUserData)
{
    // TODO
    return false;
}
}

//-----------------------------------------------------------------------------
Debug::Debug(vk::Instance instance)
{
    // TODO
}

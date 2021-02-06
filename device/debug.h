#pragma once

#include "config.h"

#if VK_RENDERDOC_EN
#  include <renderdoc_app.h>

extern RENDERDOC_API_1_4_1 rdApi;
#endif

#if VK_ENABLE_MARKERS
#  define M_SCOPED      ZoneScoped
#  define M_SCOPED_N    ZoneScopedN
#  define M_FRAME_END   FrameMark
#else
#  define M_SCOPED
#  define M_SCOPED_N
#  define M_FRAME_END
#endif

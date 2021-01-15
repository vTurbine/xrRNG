#include "stdafx.h"
#include "factory/console/render.h"


/**
 *
 */
void
fConsoleRender::Copy
        ( IConsoleRender &obj
        )
{
    *this = *(static_cast<fConsoleRender *>(&obj));
}


/**
 *
 */
void
fConsoleRender::OnRender
        ( bool inGame
        )
{
    // TBI
}

#include "stdafx.h"
#include "factory/thunderbolt/render.h"


/**
 *
 */
void
fThunderboltRender::Copy
        ( IThunderboltRender& obj
        )
{
     *this = *((fThunderboltRender *)&obj);
}


/**
 *
 */
void
fThunderboltRender::Render
        ( CEffect_Thunderbolt& owner
        )
{
    // TBI
}

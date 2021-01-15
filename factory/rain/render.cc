#include "stdafx.h"
#include "factory/rain/render.h"


/**
 *
 */
void
fRainRender::Copy
        ( IRainRender& obj
        )
{
    *this = *((fRainRender *)&obj);
}


/**
 *
 */
void
fRainRender::Render
        ( CEffect_Rain& owner
        )
{
    // TBI
}


/**
 *
 */
const Fsphere&
fRainRender::GetDropBounds() const
{
    // TBI
    return std::move(Fsphere{});
}

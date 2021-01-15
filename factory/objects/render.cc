#include "stdafx.h"
#include "factory/objects/render.h"


/**
 *
 */
void
fObjectSpaceRender::Copy
        ( IObjectSpaceRender& obj
        )
{
    *this = *((fObjectSpaceRender *)&obj);
}


/**
 *
 */
void
fObjectSpaceRender::dbgRender()
{
    // TBI
}


/**
 *
 */
void
fObjectSpaceRender::dbgAddSphere
        ( const Fsphere& sphere
        , u32 colour
        )
{
    // TBI
}


/**
 *
 */
void
fObjectSpaceRender::SetShader()
{
    // TBI
}

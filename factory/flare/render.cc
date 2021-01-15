#include "stdafx.h"
#include "factory/flare/render.h"


/**
 *
 */
void
fFlareRender::Copy
        ( IFlareRender& obj
        )
{
    *this = *((fFlareRender *)&obj);
}


/**
 *
 */
void
fFlareRender::CreateShader
        ( LPCSTR shader
        , LPCSTR textures
        )
{
    // TBI
}


/**
 *
 */
void
fFlareRender::DestroyShader()
{
    // TBI
}

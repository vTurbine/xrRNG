#include "stdafx.h"
#include "factory/flare/lens.h"


/**
 *
 */
void
fLensFlareRender::Copy
        ( ILensFlareRender& obj
        )
{
    *this = *((fLensFlareRender *)&obj);
}


/**
 *
 */
void
fLensFlareRender::Render
        ( CLensFlare& owner
        , BOOL sun
        , BOOL flares
        , BOOL gradient
        )
{
    // TBI
}


/**
 *
 */
void
fLensFlareRender::OnDeviceCreate()
{
    // TBI
}


/**
 *
 */
void
fLensFlareRender::OnDeviceDestroy()
{
    // TBI
}

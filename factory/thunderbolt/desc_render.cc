#include "stdafx.h"
#include "factory/thunderbolt/desc_render.h"


/**
 *
 */
void
fThunderboltDescRender::Copy
        ( IThunderboltDescRender& obj
        )
{
     *this = *((fThunderboltDescRender *)&obj);
}


/**
 *
 */
void
fThunderboltDescRender::CreateModel
        ( LPCSTR modelName
        )
{
    // TBI
}


/**
 *
 */
void
fThunderboltDescRender::DestroyModel()
{
    // TBI
}

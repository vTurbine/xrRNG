#include "stdafx.h"
#include "factory/wallmark/array.h"


/**
 *
 */
void
fWallMarkArray::Copy
        ( IWallMarkArray& obj
        )
{
    *this = *((fWallMarkArray *)&obj);
}


/**
 *
 */
void
fWallMarkArray::AppendMark
        ( LPCSTR textures
        )
{
    // TBI
}


/**
 *
 */
void
fWallMarkArray::clear()
{
    // TBI
}


/**
 *
 */
bool
fWallMarkArray::empty()
{
    // TBI
    return true;
}


/**
 *
 */
wm_shader
fWallMarkArray::GenerateWallmark()
{
    // TBI
    return {};
}

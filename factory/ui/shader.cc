#include "stdafx.h"
#include "factory/ui/shader.h"


/**
 *
 */
void
fUIShader::Copy
        ( IUIShader &obj
        )
{
    *this = *(static_cast<fUIShader *>(&obj));
}


/**
 *
 */
void
fUIShader::create
        ( LPCSTR shader_name
        , LPCSTR texture_list
        )
{
}


/**
 *
 */
bool
fUIShader::inited()
{
    return false;
}


/**
 *
 */
void
fUIShader::destroy()
{
}

/*!
 * \file    ui_render.cc
 * \brief   UI renderer interface implementation
 */

#include "stdafx.h"

#include "ui/ui_render.h"

#include "factory/ui/shader.h"


UiRender ui;

/*!
 * \brief   Stub implementation
 */
void
UiRender::CreateUIGeom()
{
    // Nothing here
}


/*!
 * \brief   Stub implementation
 */
void
UiRender::DestroyUIGeom()
{
    // Nothing here
}


/*!
 * \brief   Stub implementation
 */
void
UiRender::SetAlphaRef
        ( int alpha_reference
        )
{
}


/*!
 * \brief   Dynamic scissor test control
 * \param [in] area rectangle scissor area
 */
void
UiRender::SetScissor
        ( Irect *area /* = nullptr */
        )
{
}


/*!
 * \brief   Stub implementation
 */
void
UiRender::CacheSetCullMode
        ( CullMode cull_mode
        )
{
}


/*!
 * \brief   Stub implementation
 */
void
UiRender::CacheSetXformWorld
        ( const Fmatrix &matrix
        )
{
}

/*!
 * \brief   Asks backend to bind rendering pipeline
 * \param [in] shader   Assotiated UI shader object
 */
void
UiRender::SetShader
        ( IUIShader &shader
        )
{
}


/*!
 * \brief   Returns UI element's texture extent
 * \param [out] dimensions  2D vector with texture dimensions
 */
void
UiRender::GetActiveTextureResolution
        ( Fvector2 &dimensions
        )
{
}


/*!
 * \brief   Adds vertex into vertex buffer
 * \param [in] x     X coordinate
 * \param [in] y     Y coordinate
 * \param [in] z     Z coordinate (discarded)
 * \param [in] color vertex color
 * \param [in] u     texture U coordinate
 * \param [in] v     texture V coordinate
 */
void
UiRender::PushPoint
        ( float x
        , float y
        , float z
        , u32   color
        , float u
        , float v
        )
{
}


/*!
 * \brief   Starts vertex buffer recording
 * \param [in] max_vertices     Expected amount of vertices
 * \param [in] primitive_type   Desired pipeline primitive topology
 * \param [in] point_type       Vertex input format
 */
void
UiRender::StartPrimitive
        ( u32            max_vertices
        , ePrimitiveType primitive_type
        , ePointType     point_type
        )
{
}


/*!
 * \brief   Draws vertex buffer content
 */
void
UiRender::FlushPrimitive()
{
}


/*!
 * \brief   Returns new shader name if Theora stream is available
 * \param [in] texture_name normal texture name
 * \param [in] shader_name  default shader name
 */
LPCSTR
UiRender::UpdateShaderName
        ( LPCSTR texture_name
        , LPCSTR shader_name
        )
{
    string_path path;
    const bool is_exist = FS.exist( path
                                  , "$game_textures$"
                                  , texture_name
                                  , ".ogm"
    );

    if (is_exist)
    {
        return ("hud" DELIMITER "movie");
    }
    else
    {
        return shader_name;
    }
}

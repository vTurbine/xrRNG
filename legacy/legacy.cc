/*!
 * \file    legacy.cc
 * \brief   Unused methods from original frontend interface
 */

#include "stdafx.h"
#include "legacy/legacy.h"

#include <xrEngine/Render.h>



//-----------------------------------------------------------------------------
void
LegacyInterface::OnFrame()
{
}


//-----------------------------------------------------------------------------
IRender::GenerationLevel 
LegacyInterface::GetGeneration() const
{
    return GENERATION_R2;
}


//-----------------------------------------------------------------------------
IRender::BackendAPI
LegacyInterface::GetBackendAPI() const
{
    return BackendAPI::Vulkan;
}


//-----------------------------------------------------------------------------
bool
LegacyInterface::is_sun_static()
{
    return false;
}


//-----------------------------------------------------------------------------
u32
LegacyInterface::get_dx_level()
{
    return 0;
}


//-----------------------------------------------------------------------------
void 
LegacyInterface::create()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::destroy()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::reset_begin()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::reset_end()
{
}



//-----------------------------------------------------------------------------
void
LegacyInterface::level_Load(IReader* fs)
{
}



//-----------------------------------------------------------------------------
void
LegacyInterface::level_Unload()
{
}


//-----------------------------------------------------------------------------
HRESULT
LegacyInterface::shader_compile
        ( pcstr     name
        , IReader  *fs
        , pcstr     pFunctionName
        , pcstr     pTarget
        , u32       Flags
        , void    *&result)
{
    return S_OK;
}


//-----------------------------------------------------------------------------
void
LegacyInterface::DumpStatistics
        ( IGameFont         &font
        , IPerformanceAlert *alert
        )
{
}


//-----------------------------------------------------------------------------
pcstr
LegacyInterface::getShaderPath()
{
    return "r3" DELIMITER ""; // doesn't matter: shaders will be pre-cooked
}


//-----------------------------------------------------------------------------
IRender_Sector *
LegacyInterface::getSector
        ( int id
        )
{
    return nullptr;
}


//-----------------------------------------------------------------------------
IRenderVisual *
LegacyInterface::getVisual
        ( int id
        )
{
    return nullptr;
}


//-----------------------------------------------------------------------------
IRender_Sector *
LegacyInterface::detectSector
    ( Fvector const &P
    )
{
    return nullptr;
}


//-----------------------------------------------------------------------------
IRender_Target *
LegacyInterface::getTarget()
{
    return nullptr;
}


//-----------------------------------------------------------------------------
void
LegacyInterface::flush()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::add_Occluder
        ( Fbox2 &bb_screenspace
        )
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::add_Visual
        ( IRenderable   *root
        , IRenderVisual *V
        , Fmatrix       &m
        )
{
}

//-----------------------------------------------------------------------------
void
LegacyInterface::add_Geometry
        ( IRenderVisual     *V
        , CFrustum const    &view
        )
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::add_StaticWallmark
        ( wm_shader const  &S
        , Fvector const    &P
        , float             s
        , CDB::TRI         *T
        , Fvector          *V
        )
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::add_StaticWallmark
        ( IWallMarkArray   *pArray
        , Fvector const    &P
        , float             s
        , CDB::TRI         *T
        , Fvector          *V
        )
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::clear_static_wallmarks()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::add_SkeletonWallmark
        ( Fmatrix const    *xf
        , IKinematics      *obj
        , IWallMarkArray   *pArray
        , Fvector const    &start
        , Fvector const    &dir
        , float             size
        )
{
}


//-----------------------------------------------------------------------------
IRender_ObjectSpecific *
LegacyInterface::ros_create
        ( IRenderable   *parent
        )
{
    return nullptr;
}


//-----------------------------------------------------------------------------
void
LegacyInterface::ros_destroy
        ( IRender_ObjectSpecific  *&ros
        )
{
}


//-----------------------------------------------------------------------------
IRender_Light *
LegacyInterface::light_create()
{
    return nullptr;
}


//-----------------------------------------------------------------------------
void
LegacyInterface::light_destroy
        ( IRender_Light *p_
        )
{
}


//-----------------------------------------------------------------------------
IRender_Glow *
LegacyInterface::glow_create()
{
    return nullptr;
}


//-----------------------------------------------------------------------------
void
LegacyInterface::glow_destroy
        ( IRender_Glow  *p_
        )
{
}


//-----------------------------------------------------------------------------
IRenderVisual *
LegacyInterface::model_CreateParticles
        ( pcstr name
        )
{
    return nullptr;
}


//-----------------------------------------------------------------------------
IRenderVisual *
LegacyInterface::model_Create
        ( pcstr     name
        , IReader  *data/*= 0*/
        )
{
    return nullptr;
}


//-----------------------------------------------------------------------------
IRenderVisual *
LegacyInterface::model_CreateChild
        ( pcstr     name
        , IReader  *data)
{
    return nullptr;
}


//-----------------------------------------------------------------------------
IRenderVisual *
LegacyInterface::model_Duplicate
        ( IRenderVisual *V
        )
{
    return nullptr;
}


//-----------------------------------------------------------------------------
void
LegacyInterface::model_Delete
        ( IRenderVisual   *&V
        , bool              bDiscard/*= false*/
        )
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::model_Logging
        ( bool bEnable
        )
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::models_Prefetch()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::models_Clear
        ( bool b_complete
        )
{
}


//-----------------------------------------------------------------------------
bool
LegacyInterface::occ_visible
        ( vis_data  &V
        )
{
    return false;
}


//-----------------------------------------------------------------------------
bool
LegacyInterface::occ_visible
        ( Fbox  &B
        )
{
    return false;
}


//-----------------------------------------------------------------------------
bool
LegacyInterface::occ_visible
        ( sPoly &P
        )
{
    return false;
}


//-----------------------------------------------------------------------------
void
LegacyInterface::Calculate()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::Render()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::BeforeWorldRender()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::AfterWorldRender()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::Screenshot
        ( ScreenshotMode    mode/*=SM_NORMAL*/
        , pcstr             name/*= 0*/)
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::Screenshot
        ( ScreenshotMode    mode
        , CMemoryWriter    &memory_writer
        )
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::ScreenshotAsyncBegin()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::ScreenshotAsyncEnd
        ( CMemoryWriter &memory_writer
        )
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::rmNear()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::rmFar()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::rmNormal()
{
}


//-----------------------------------------------------------------------------
u32
LegacyInterface::active_phase()
{
    return 0;
}


//-----------------------------------------------------------------------------
LegacyInterface::~LegacyInterface()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::setGamma
        ( float fGamma
        )
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::setBrightness
        ( float fGamma
        )
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::setContrast
        ( float fGamma
        )
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::updateGamma()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::OnDeviceDestroy
        ( bool bKeepTextures
        )
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::Destroy()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::Reset
        ( SDL_Window   *hWnd
        , u32          &dwWidth
        , u32          &dwHeight
        , float        &fWidth_2
        , float        &fHeight_2
        )
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::SetupStates()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::OnDeviceCreate
        ( pcstr shName
        )
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::Create
        ( SDL_Window   *hWnd
        , u32          &dwWidth
        , u32          &dwHeight
        , float        &fWidth_2
        , float        &fHeight_2
        )
{
}


//-----------------------------------------------------------------------------
void 
LegacyInterface::SetupGPU
        ( bool force_sw
        , bool force_nonpure
        , bool force_ref
        )
{
    /*
     * Nothing here.
     * We don't support concept of reference devices and
     * expect all TnL processing on HW side.
     */
}


//-----------------------------------------------------------------------------
void
LegacyInterface::overdrawBegin()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::overdrawEnd()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::DeferredLoad
        ( bool E
        )
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::ResourcesDeferredUpload()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::ResourcesDeferredUnload()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::ResourcesGetMemoryUsage
        ( u32   &m_base
        , u32   &c_base
        , u32   &m_lmaps
        , u32   &c_lmaps
        )
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::ResourcesDestroyNecessaryTextures()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::ResourcesStoreNecessaryTextures()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::ResourcesDumpMemoryUsage()
{
}


//-----------------------------------------------------------------------------
bool
LegacyInterface::HWSupportsShaderYUV2RGB()
{
    /*
     * Nothing here.
     * The YUV2RGB conversion can be done on CPU side
     * or by shader (if HW can support this). We forced
     * Theora stream decoder to use the second option.
     */

    // TODO: Software coversion is broken
    return true;
}


//-----------------------------------------------------------------------------
DeviceState
LegacyInterface::GetDeviceState()
{
    return DeviceState::Normal;
}


//-----------------------------------------------------------------------------
bool
LegacyInterface::GetForceGPU_REF()
{
    /*
     * Nothing here.
     * We don't support concept of reference devices
     */
    return false;
}


//-----------------------------------------------------------------------------
u32
LegacyInterface::GetCacheStatPolys()
{
    return 0;
}


//-----------------------------------------------------------------------------
void
LegacyInterface::BeforeFrame()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::Begin()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::Clear()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::End()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::ClearTarget()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::SetCacheXform
        ( Fmatrix   &mView
        , Fmatrix   &mProject
        )
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::OnAssetsChanged()
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::ObtainRequiredWindowFlags
        ( u32   &windowFlags
        )
{
}


//-----------------------------------------------------------------------------
void
LegacyInterface::MakeContextCurrent
        ( RenderContext context
        )
{
}



//------------------------------------------------------------------------------
void 
LegacyInterface::ScreenshotImpl
        ( ScreenshotMode    mode
        , pcstr             name
        , CMemoryWriter    *memory_writer
        )
{
    // TODO: Not this time
}

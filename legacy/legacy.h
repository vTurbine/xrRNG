/*!
 * \file    legacy.h
 * \brief   Unimplemented and not used methods from original
 *          frontend interface
 */

#ifndef FRONTEND_LEGACY_H_
#define FRONTEND_LEGACY_H_

#include <Common/Common.hpp>
#include <xrEngine/Render.h>
#include <xrEngine/pure.h>


class LegacyInterface
    : public IRender
    , public pureFrame
{
public:
    void OnFrame() final;

public:
    GenerationLevel GetGeneration() const final;
    BackendAPI GetBackendAPI() const final;
    bool is_sun_static() final;
    u32 get_dx_level() final;
    void create() final;
    void destroy() final;
    void reset_begin() final;
    void reset_end() final;
    void level_Load(IReader* fs) final;
    void level_Unload() final;
    HRESULT shader_compile(pcstr name, IReader* fs, pcstr pFunctionName, pcstr pTarget, u32 Flags, void*& result) final;
    void DumpStatistics(IGameFont& font, IPerformanceAlert* alert) final;
    pcstr getShaderPath() final;
    IRender_Sector* getSector(int id) final;
    IRenderVisual* getVisual(int id) final;
    IRender_Sector* detectSector(const Fvector& P) final;
    IRender_Target* getTarget() final;
    void flush() final;
    void add_Occluder(Fbox2& bb_screenspace) final;
    void add_Visual(IRenderable* root, IRenderVisual* V, Fmatrix& m) final;
    void add_Geometry(IRenderVisual* V, const CFrustum& view) final;
    void add_StaticWallmark(const wm_shader& S, const Fvector& P, float s, CDB::TRI* T, Fvector* V) final;
    void add_StaticWallmark(IWallMarkArray* pArray, const Fvector& P, float s, CDB::TRI* T, Fvector* V) final;
    void clear_static_wallmarks() final;
    void add_SkeletonWallmark(const Fmatrix* xf, IKinematics* obj, IWallMarkArray* pArray, const Fvector& start, const Fvector& dir, float size) final;
    IRender_ObjectSpecific* ros_create(IRenderable* parent) final;
    void ros_destroy(IRender_ObjectSpecific*&) final;
    IRender_Light* light_create() final;
    void light_destroy(IRender_Light* p_) final;
    IRender_Glow* glow_create() final;
    void glow_destroy(IRender_Glow* p_) final;
    IRenderVisual* model_CreateParticles(pcstr name) final;
    IRenderVisual* model_Create(pcstr name, IReader* data = 0) final;
    IRenderVisual* model_CreateChild(pcstr name, IReader* data) final;
    IRenderVisual* model_Duplicate(IRenderVisual* V) final;
    void model_Delete(IRenderVisual*& V, bool bDiscard = false) final;
    void model_Logging(bool bEnable) final;
    void models_Prefetch() final;
    void models_Clear(bool b_complete) final;
    bool occ_visible(vis_data& V) final;
    bool occ_visible(Fbox& B) final;
    bool occ_visible(sPoly& P) final;
    void Calculate() final;
    void Render() final;
    void BeforeWorldRender() final;
    void AfterWorldRender() final;
    void Screenshot(ScreenshotMode mode = SM_NORMAL, pcstr name = 0) final;
    void Screenshot(ScreenshotMode mode, CMemoryWriter& memory_writer) final;
    void ScreenshotAsyncBegin() final;
    void ScreenshotAsyncEnd(CMemoryWriter& memory_writer) final;
    void rmNear() final;
    void rmFar() final;
    void rmNormal() final;
    u32 active_phase () final;
    ~LegacyInterface() override;
    void setGamma(float fGamma) final;
    void setBrightness(float fGamma) final;
    void setContrast(float fGamma) final;
    void updateGamma() final;
    void OnDeviceDestroy(bool bKeepTextures) final;
    void Destroy() final;
    void Reset(SDL_Window* hWnd, u32& dwWidth, u32& dwHeight, float& fWidth_2, float& fHeight_2) final;
    void SetupStates() final;
    void OnDeviceCreate(pcstr shName) final;
    void Create(SDL_Window* hWnd, u32& dwWidth, u32& dwHeight, float& fWidth_2, float& fHeight_2) final;
    void SetupGPU(bool bForceGPU_SW, bool bForceGPU_NonPure, bool bForceGPU_REF) final;
    void overdrawBegin() final;
    void overdrawEnd() final;
    void DeferredLoad(bool E) final;
    void ResourcesDeferredUpload() final;
    void ResourcesDeferredUnload() final;
    void ResourcesGetMemoryUsage(u32& m_base, u32& c_base, u32& m_lmaps, u32& c_lmaps) final;
    void ResourcesDestroyNecessaryTextures() final;
    void ResourcesStoreNecessaryTextures() final;
    void ResourcesDumpMemoryUsage() final;
    bool HWSupportsShaderYUV2RGB() final;
    DeviceState GetDeviceState();
    bool GetForceGPU_REF();
    u32 GetCacheStatPolys() final;
    void BeforeFrame() final;
    void Begin() final;
    void Clear() final;
    void End() final;
    void ClearTarget() final;
    void SetCacheXform(Fmatrix& mView, Fmatrix& mProject) final;
    void OnAssetsChanged() final;
    void ObtainRequiredWindowFlags(u32& windowFlags) final;
    void MakeContextCurrent(RenderContext context) final;

protected:
    void ScreenshotImpl(ScreenshotMode mode, pcstr name, CMemoryWriter* memory_writer) final;
};


#endif // FRONTEND_LEGACY_H_

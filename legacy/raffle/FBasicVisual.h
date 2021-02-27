#ifndef FBasicVisualH
#define FBasicVisualH

#include <xrEngine/vis_common.h>
#include <xrEngine/stdafx.h>
#include <Include/xrRender/RenderVisual.h>

#define VLOAD_NOVERTICES (1 << 0)

// The class itself
class CKinematicsAnimated;
class CKinematics;
class IParticleCustom;

struct IRender_Mesh
{
    // format
    //ref_geom rm_geom;

    // verts
    u32 vbuf_id;
    u32 ibuf_id;
    u32 vBase;
    u32 vCount;
    u32 vStride{ 0 };
    std::unique_ptr<uint8_t[]> vertices{};

    // indices
    std::unique_ptr<uint8_t[]> indices{};
    u32 iBase;
    u32 iCount;
    u32 dwPrimitives;
    u32 fvf;

    IRender_Mesh()
    {
        vBase           = 0;
        vCount          = 0;
        iBase           = 0;
        iCount          = 0;
        dwPrimitives    = 0;
        //rm_geom         = nullptr;
    }
    virtual ~IRender_Mesh();

private:
    IRender_Mesh(const IRender_Mesh& other);
    void operator=(const IRender_Mesh& other);
};

// The class itself
class ECORE_API dxRender_Visual : public IRenderVisual
{
public:
#ifdef DEBUG
    shared_str dbg_name;
    virtual shared_str getDebugName() { return dbg_name; }
#endif
public:
    // Common data for rendering
    u32 Type; // visual's type
    vis_data vis; // visibility-data
    int shader_id;
    int vis_id{ -1 };
    std::pair<std::string, std::string> custom_mat{};

    virtual void Render(float /*LOD*/) {} // LOD - Level Of Detail  [0..1], Ignored
    virtual void Load(const char* N, IReader* data, u32 dwFlags);
    virtual void Release(); // Shared memory release
    virtual void Copy(dxRender_Visual* from);
    virtual void Spawn(){};
    virtual void Depart(){};

    virtual vis_data& getVisData() { return vis; }
    virtual u32 getType() { return Type; }
    dxRender_Visual();
    virtual ~dxRender_Visual();
};

#endif // !FBasicVisualH

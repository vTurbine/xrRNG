#include "stdafx.h"
#pragma hdrstop

#include "FBasicVisual.h"
#include "xrCore/FMesh.hpp"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IRender_Mesh::~IRender_Mesh()
{
#if 0
    rm_geom.destroy();
    _RELEASE(p_rm_Vertices);
    _RELEASE(p_rm_Indices);
#endif
}

dxRender_Visual::dxRender_Visual()
{
    Type = 0;
    //shader = nullptr;
    vis.clear();
}

dxRender_Visual::~dxRender_Visual() {}
void dxRender_Visual::Release() {}
// CStatTimer						tscreate;

void dxRender_Visual::Load(const char* N, IReader* data, u32)
{
#ifdef DEBUG
    dbg_name = N;
#endif

    // header
    VERIFY(data);
    ogf_header hdr;
    if (data->r_chunk_safe(OGF_HEADER, &hdr, sizeof(hdr)))
    {
        R_ASSERT2(hdr.format_version == xrOGF_FormatVersion, "Invalid visual version");
        Type = hdr.type;
        shader_id = hdr.shader_id;
        vis.box.set(hdr.bb.min, hdr.bb.max);
        vis.sphere.set(hdr.bs.c, hdr.bs.r);
    }
    else
    {
        FATAL("Invalid visual");
    }

    if (data->find_chunk(OGF_TEXTURE))
    {
        string256 fnT, fnS;
        data->r_stringZ(fnT, sizeof(fnT));
        data->r_stringZ(fnS, sizeof(fnS));
        custom_mat = std::make_pair(fnS, fnT);
    }
}

#define PCOPY(a) a = pFrom->a
void dxRender_Visual::Copy(dxRender_Visual* pFrom)
{
    PCOPY(Type);
    PCOPY(shader_id);
    PCOPY(vis);
#ifdef DEBUG
    PCOPY(dbg_name);
#endif
}

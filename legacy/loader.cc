#include "stdafx.h"

#include "legacy/legacy.h"
#include "legacy/raffle/FBasicVisual.h"
#include "legacy/raffle/sector.h"

#include "device/device.h"
#include "backend/backend.h"

#include <Common/LevelStructure.hpp>
#include <xrCore/stream_reader.h>
#include <xrCore/FMesh.hpp>
#include <xrEngine/IGame_Level.h>
#include <xrEngine/x_ray.h>
#include <Include/xrRender/RenderVisual.h>



LevelData ld;

//-----------------------------------------------------------------------------
void
LegacyInterface::level_Load
        ( IReader *fs
        )
{
    R_ASSERT(g_pGameLevel != nullptr);
    pApp->LoadBegin();

    // 1. Load materials
    {
        auto chunk = fs->open_chunk(fsL_SHADERS);
        R_ASSERT2(chunk, "LEVEL: missing materials");
        auto const count = chunk->r_u32();
        ld.materials.resize(count);
        for (size_t i = 0; i < count; i++)
        {
            string512 n_sh, n_tlist;
            LPCSTR n = LPCSTR(chunk->pointer());
            chunk->skip_stringZ();
            if (0 == n[0]) // skip first shader as "reserved" one
                continue;
            xr_strcpy(n_sh, n);
            pstr delim = strchr(n_sh, '/');
            *delim = 0;
            xr_strcpy(n_tlist, delim + 1);
            ld.materials[i - 1] = std::make_pair(n_sh, n_tlist);
        }
        chunk->close();
    }

    // 2. Level geometry
    {
        CStreamReader* geom = FS.rs_open("$level$", "level.geom");
        {
            if (geom->find_chunk(fsL_VB)) // vertices
            {
                CStreamReader* fs = geom->open_chunk(fsL_VB);
                u32 count = fs->r_u32();
                ld.formats.resize(count);
                ld.vertices.resize(count);

                u32 buffer_size = (64 + 1) * sizeof(VertexFormat);
                VertexFormat* dcl = (VertexFormat*)xr_alloca(buffer_size);

                for (u32 i = 0; i < count; i++)
                {
                    fs->r(dcl, buffer_size);
                    fs->advance(-(int)buffer_size);

                    u32 dcl_len = GetDeclLength(dcl) + 1;

                    ld.formats[i].resize(dcl_len);
                    fs->r(ld.formats[i].data(), dcl_len * sizeof(VertexFormat));

                    // count, size
                    u32 vCount = fs->r_u32();
                    u32 vSize = GetDeclVertexSize(dcl, 0);
                    Msg("* [Loading VB] %d verts, %d Kb", vCount, (vCount * vSize) / 1024);

                    ld.vertices[i] = std::make_unique<uint8_t[]>(vCount * vSize);
                    fs->r(ld.vertices[i].get(), vCount * vSize);
                }
                fs->close();
            }

            if (geom->find_chunk(fsL_IB)) // indices
            {
                CStreamReader* fs = geom->open_chunk(fsL_IB);
                u32 count = fs->r_u32();
                ld.indices.resize(count);
                for (u32 i = 0; i < count; i++)
                {
                    u32 iCount = fs->r_u32();
                    Msg("* [Loading IB] %d indices, %d Kb", iCount, (iCount * 2) / 1024);

                    ld.indices[i] = std::make_unique<uint8_t[]>(iCount * 2);
                    fs->r(ld.indices[i].get(), iCount * 2);
                }
                fs->close();
            }

            if (geom->find_chunk(fsL_SWIS)) // sliding windows
            {
                CStreamReader* fs = geom->open_chunk(fsL_SWIS);
                u32 item_count = fs->r_u32();

                ld.swis.resize(item_count);
                for (u32 c = 0; c < item_count; c++)
                {
                    FSlideWindowItem& swi = ld.swis[c];
                    swi.reserved[0] = fs->r_u32();
                    swi.reserved[1] = fs->r_u32();
                    swi.reserved[2] = fs->r_u32();
                    swi.reserved[3] = fs->r_u32();
                    swi.count = fs->r_u32();
                    VERIFY(NULL == swi.sw);
                    swi.sw = xr_alloc<FSlideWindow>(swi.count);
                    fs->r(swi.sw, sizeof(FSlideWindow) * swi.count);
                }
                fs->close();
            }
        }
        FS.r_close(geom);
    }

    // 3. Visuals
    {
        auto chunk = fs->open_chunk(fsL_VISUALS);
        int index = 0;
        dxRender_Visual* V = nullptr;
        ogf_header H;
        IReader* c = nullptr;
        char name[32];
        while ((c = chunk->open_chunk(index)) != nullptr)
        {
            c->r_chunk_safe(OGF_HEADER, &H, sizeof(H));
            V = model_pool_.Instance_Create(H.type);
            sprintf(name, "vis%03d", index);
            V->Load(name, c, 0);
            ld.visuals.push_back(V);

            c->close();
            index++;
        }
        chunk->close();
    }

    // 4. Sectors & portals
    {
        struct b_portal
        {
            u16 sector_front;
            u16 sector_back;
            svector<Fvector, 6> vertices;
        };

        u32 size = fs->find_chunk(fsL_PORTALS);
        R_ASSERT(0 == size % sizeof(b_portal));
        u32 count = size / sizeof(b_portal);
        ld.portals.resize(count);
        char name[32];
        for (u32 c = 0; c < count; c++)
        {
            sprintf(name, "pr%d", c);
            auto P = std::make_unique<CPortal>(name);
            ld.portals[c] = std::move(P);
        }

        IReader* S = fs->open_chunk(fsL_SECTORS);
        for (u32 i = 0;; i++)
        {
            IReader* P = S->open_chunk(i);
            if (nullptr == P)
                break;

            sprintf(name, "sc%d", i);
            auto &__S = ld.sectors.emplace_back(std::make_unique<CSector>(name));
            __S->load(*P);

            P->close();
        }
        S->close();

        if (count)
        {
            u32 i;
            CDB::Collector CL;
            fs->find_chunk(fsL_PORTALS);
            for (i = 0; i < count; i++)
            {
                b_portal P;
                fs->r(&P, sizeof(P));
                CPortal* __P = (CPortal*)ld.portals[i].get();
                __P->Setup(
                    P.vertices.begin(),
                    P.vertices.size(),
                    ld.sectors[P.sector_front].get(),
                    ld.sectors[P.sector_back].get()
                );
                for (u32 j = 2; j < P.vertices.size(); j++)
                    CL.add_face_packed_D(P.vertices[0], P.vertices[j - 1], P.vertices[j], u32(i));
            }
            if (CL.getTS() < 2)
            {
                Fvector v1, v2, v3;
                v1.set(-20000.f, -20000.f, -20000.f);
                v2.set(-20001.f, -20001.f, -20001.f);
                v3.set(-20002.f, -20002.f, -20002.f);
                CL.add_face_packed_D(v1, v2, v3, 0);
            }

            // build portal model
            ld.portals_model = std::make_unique<CDB::MODEL>();
            ld.portals_model->build(CL.getV(), int(CL.getVS()), CL.getT(), int(CL.getTS()));
        }
    }

    // 5. Lights
    {
        lights_.Load(fs);
    }

    pApp->LoadEnd();
}

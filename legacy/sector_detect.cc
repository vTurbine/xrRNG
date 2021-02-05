#include "stdafx.h"

#include "legacy/legacy.h"

#include <xrEngine/IGame_Level.h>


constexpr float RAYCAST_DISTANCE = 500.0f;

//-----------------------------------------------------------------------------
IRender_Sector *
LegacyInterface::detectSector
        ( Fvector const &position
        )
{
    Sectors_xrc.ray_options(CDB::OPT_ONLYNEAREST);

    auto* sector = detectSector(position, {0.0f, -1.0f, 0.0f});
    if (sector == nullptr)
    {
        sector = detectSector(position, {0.0f, 1.0f, 0.0f});
    }
    return sector;
}


//-----------------------------------------------------------------------------
IRender_Sector *
LegacyInterface::detectSector
        ( Fvector const &position
        , Fvector const &direction
        )
{
    // Portals model
    int id1 = -1;
    float range1 = RAYCAST_DISTANCE;
    if (ld.portals_model)
    {
        Sectors_xrc.ray_query(
            ld.portals_model.get(),
            position,
            direction,
            range1
        );

        if (Sectors_xrc.r_count())
        {
            CDB::RESULT* RP1 = Sectors_xrc.r_begin();
            id1 = RP1->id;
            range1 = RP1->range;
        }
    }

    // Geometry model
    int id2 = -1;
    float range2 = range1;
    Sectors_xrc.ray_query(
        g_pGameLevel->ObjectSpace.GetStaticModel(),
        position,
        direction,
        range2
    );

    if (Sectors_xrc.r_count())
    {
        CDB::RESULT* RP2 = Sectors_xrc.r_begin();
        id2 = RP2->id;
        range2 = RP2->range;
    }

    if (id1 < 0 && id2 < 0) // no results
    {
        return nullptr;
    }

    // Find closest face
    int faceId = id1;
    if (id1 >= 0)
    {
        if ((id2 >= 0) && (range2 <= (range1 + EPS)))
        {
            faceId = id2;
        }
    }
    else
    {
        faceId = id2;
    }

    if (faceId == id1)
    {
        // Take sector, facing to our point from portal
        CDB::TRI const &face = ld.portals_model->get_tris()[faceId];
        auto *portal = (CPortal*)ld.portals[face.dummy].get();
        return portal->getSectorFacing(position);
    }
    else
    {
        // Take triangle at ID and use it's Sector
        CDB::TRI const &face = g_pGameLevel->ObjectSpace.GetStaticTris()[faceId];
        return ld.sectors[face.sector].get();
    }
}

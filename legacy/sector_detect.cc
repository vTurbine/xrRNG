#include "stdafx.h"

#include "legacy/legacy.h"

#include <xrEngine/IGame_Level.h>


//-----------------------------------------------------------------------------
IRender_Sector *
LegacyInterface::detectSector
    ( Fvector const &P
    )
{
    IRender_Sector* S = NULL;
    Fvector dir;
    Sectors_xrc.ray_options(CDB::OPT_ONLYNEAREST);

    dir.set(0, -1, 0);
    S = detectSector(P, dir);
    if (NULL == S)
    {
        dir.set(0, 1, 0);
        S = detectSector(P, dir);
    }
    return S;
}

IRender_Sector *
LegacyInterface::detectSector(const Fvector& P, Fvector& dir)
{
    // Portals model
    int id1 = -1;
    float range1 = 500.f;
    if (ld.portals_model)
    {
        Sectors_xrc.ray_query(ld.portals_model.get(), P, dir, range1);
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
    Sectors_xrc.ray_query(g_pGameLevel->ObjectSpace.GetStaticModel(), P, dir, range2);
    if (Sectors_xrc.r_count())
    {
        CDB::RESULT* RP2 = Sectors_xrc.r_begin();
        id2 = RP2->id;
        range2 = RP2->range;
    }

    // Select ID
    int ID;
    if (id1 >= 0)
    {
        if (id2 >= 0)
            ID = (range1 <= range2 + EPS) ? id1 : id2; // both was found
        else
            ID = id1; // only id1 found
    }
    else if (id2 >= 0)
        ID = id2; // only id2 found
    else
        return 0;

    if (ID == id1)
    {
        // Take sector, facing to our point from portal
        CDB::TRI* pTri = ld.portals_model->get_tris() + ID;
        CPortal* pPortal = (CPortal*)ld.portals[pTri->dummy].get();
        return pPortal->getSectorFacing(P);
    }
    else
    {
        // Take triangle at ID and use it's Sector
        CDB::TRI* pTri = g_pGameLevel->ObjectSpace.GetStaticTris() + ID;
        return ld.sectors[pTri->sector].get();
    }
}

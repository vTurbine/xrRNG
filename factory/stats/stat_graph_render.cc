#include "stdafx.h"
#include "factory/stats/stat_graph_render.h"


/**
 *
 */
void
fStatGraphRender::Copy
        ( IStatGraphRender& obj
        )
{
    *this = *((fStatGraphRender *)&obj);
}


/**
 *
 */
void
fStatGraphRender::OnDeviceCreate()
{
    // TBI
}


/**
 *
 */
void
fStatGraphRender::OnDeviceDestroy()
{
    // TBI
}


/**
 *
 */
void
fStatGraphRender::OnRender
        ( CStatGraph& owner
        )
{
    // TBI
}

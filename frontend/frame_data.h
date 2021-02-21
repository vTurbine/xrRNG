#ifndef FRONTEND_FRAME_DATA_H_
#define FRONTEND_FRAME_DATA_H_

#include "device/memory.h"
#include "legacy/raffle/FBasicVisual.h"

#include <xrCore/_types.h>


struct FrameData
{
    Fmatrix     mView;
    Fmatrix     mProjection;

    Fvector     vPrevCameraPos;

    std::vector<std::pair<float, dxRender_Visual*>> StaticGeometryList;

    ImagePtr    base_depth;
};

#endif //FRONTEND_FRAME_DATA_H_

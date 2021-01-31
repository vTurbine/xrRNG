#ifndef FRONTEND_FRAME_DATA_H_
#define FRONTEND_FRAME_DATA_H_

#include <xrCore/_types.h>


// TODO: per frame
struct FrameData
{
    Fmatrix     mView;
    Fmatrix     mProjection;

    Fvector     vPrevCameraPos;
};

#endif //FRONTEND_FRAME_DATA_H_

#ifndef FRONTEND_RENDER_H_
#define FRONTEND_RENDER_H_

#include "legacy/legacy.h"

#include <xrEngine/pure.h>
#include <xrEngine/Render.h>


class FrontEnd final
    : public LegacyInterface
{
public:
    FrontEnd() = default;
    ~FrontEnd() = default;

    // TODO
};

extern FrontEnd frontend;

#endif // FRONTEND_RENDER_H_

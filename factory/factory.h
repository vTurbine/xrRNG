#ifndef FACTORY_FACTORY_H_
#define FACTORY_FACTORY_H_

#include "factory/ui/sequence_video_item.h"
#include "factory/ui/shader.h"
#include "factory/stats/stat_graph_render.h"
#include "factory/console/render.h"
#include "factory/wallmark/array.h"
#include "factory/flare/render.h"
#include "factory/flare/lens.h"
#include "factory/thunderbolt/render.h"
#include "factory/thunderbolt/desc_render.h"
#include "factory/rain/render.h"
#include "factory/environment/render.h"
#include "factory/environment/desc_render.h"
#include "factory/environment/desc_mixer_render.h"
#include "factory/font/render.h"
#include "factory/objects/render.h"

#include <Include/xrRender/RenderFactory.h>


#define RENDER_FACTORY_DECLARE(Class)                   \
    virtual I##Class* Create##Class();                  \
    virtual void Destroy##Class(I##Class *pObject);

class RenderFactory
    : public IRenderFactory
{
    RENDER_FACTORY_DECLARE(UISequenceVideoItem)
    RENDER_FACTORY_DECLARE(UIShader)
    RENDER_FACTORY_DECLARE(StatGraphRender)
    RENDER_FACTORY_DECLARE(ConsoleRender)
    RENDER_FACTORY_DECLARE(WallMarkArray)
    RENDER_FACTORY_DECLARE(FlareRender)
    RENDER_FACTORY_DECLARE(LensFlareRender)
    RENDER_FACTORY_DECLARE(ThunderboltRender)
    RENDER_FACTORY_DECLARE(ThunderboltDescRender)
    RENDER_FACTORY_DECLARE(RainRender)
    RENDER_FACTORY_DECLARE(EnvironmentRender)
    RENDER_FACTORY_DECLARE(EnvDescriptorMixerRender)
    RENDER_FACTORY_DECLARE(EnvDescriptorRender)
    RENDER_FACTORY_DECLARE(FontRender)
#ifdef DEBUG
    RENDER_FACTORY_DECLARE(ObjectSpaceRender)
#endif
};

extern RenderFactory factory;

#endif // FACTORY_FACTORY_H_

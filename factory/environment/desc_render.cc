#include "stdafx.h"
#include "factory/environment/desc_render.h"


/**
 *
 */
void
fEnvDescriptorRender::Copy
        ( IEnvDescriptorRender& obj
        )
{
    *this = *((fEnvDescriptorRender *)&obj);
}


/**
 *
 */
void
fEnvDescriptorRender::OnDeviceCreate
        ( CEnvDescriptor& owner
        )
{
    // TBI
}


/**
 *
 */
void
fEnvDescriptorRender::OnDeviceDestroy()
{
    // TBI
}

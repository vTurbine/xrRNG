#include "xrGame/GamePersistent.h"

#include "device/device.h"
#include "backend/backend.h"

#include "frontend/render.h"


/**
 *
 */
void
FrontEnd::Create
        ( SDL_Window *hwnd
        , u32        &width
        , u32        &height
        , float      &width_2
        , float      &height_2
        )
{
    width_2  = float(width  / 2);
    height_2 = float(height / 2);

    hw.CreateDevice(hwnd);
}


/**
 *
 */
void
FrontEnd::Destroy()
{
    hw.DestroyDevice();
}


/**
 *
 */
void
FrontEnd::OnDeviceCreate
        ( LPCSTR shader
        )
{
    backend.OnDeviceCreate();

    resources.OnDeviceCreate(shader);

    render_target = std::make_shared<RenderTarget>();

    particles_.OnCreate();

    main_menu_.Create();
}


/**
 *
 */
DeviceState
FrontEnd::GetDeviceState()
{
    DeviceState result = DeviceState::Normal;

    switch (swapchain_state_)
    {
    case vk::Result::eSuccess:
        // Nothing to do, continue;
        break;

    case vk::Result::eNotReady: // not ready, just wait
    case vk::Result::eTimeout: // a surface wasn't acquired, wait and try again
    case vk::Result::eErrorDeviceLost:
    case vk::Result::eErrorSurfaceLostKHR:
        // TODO: surface lost. What should we do?
        result = DeviceState::Lost;
        break;

    case vk::Result::eErrorOutOfDateKHR:
    case vk::Result::eSuboptimalKHR:
        // Surface params have been changed, need to re-create the swapchain
        result = DeviceState::NeedReset;
        break;

    default:
        // All other stuff like out-of-memory
        R_ASSERT2(false, "Something went wrong with presentation device..");
    };

    return result;
}


/**
 *
 */
void
FrontEnd::OnDeviceDestroy
        ( bool keep_textures
        )
{
    main_menu_.Destroy();

    particles_.OnDestroy();

    backend.OnDeviceDestroy();
}


/**
 *
 */
void
FrontEnd::Begin()
{
    const auto frame_semaphore =
        backend.frame_semaphores[current_image_].get();

    swapchain_state_ =
        hw.device->acquireNextImageKHR( hw.swapchain
                                      , std::numeric_limits<std::uint64_t>::max()
                                      , frame_semaphore
                                      , nullptr
                                      , &current_image_
    );

    backend.OnFrameBegin(current_image_);

    // TODO: or handle this in backend, like the vanilla render does?
    render_target->Set(RenderTarget::Type::Base);
}


/**
 *
 */
void
FrontEnd::End()
{
    backend.OnFrameEnd(current_image_);

    // Presentation
    const auto render_semaphore =
        backend.render_semaphores[current_image_].get();

    const auto present_info = vk::PresentInfoKHR()
        .setWaitSemaphoreCount(1)
        .setPWaitSemaphores(&render_semaphore)
        .setSwapchainCount(1)
        .setPSwapchains(&hw.swapchain)
        .setPImageIndices(&current_image_);
    
    hw.submission_q.presentKHR(present_info);
    current_image_ = (current_image_ + 1) % hw.swapchain_images.size();

    #ifdef DEBUG
    hw.device->waitIdle();
    #endif
}


//-----------------------------------------------------------------------------
IRender_Target *
FrontEnd::getTarget()
{
    return render_target.get();
}


//-----------------------------------------------------------------------------
IRenderVisual *
FrontEnd::model_Create
        ( LPCSTR   name
        , IReader *data /* = 0 */
        )
{
    return models_.CreateModel(name, data);
}


//-----------------------------------------------------------------------------
IRenderVisual *
FrontEnd::model_CreatePE
        ( LPCSTR name
        )
{
    auto effect_descriptor =
        particles_.FindParticleEffectDescriptor(name);
    VERIFY(effect_descriptor);

    auto effect = models_.CreateParticleEffect(effect_descriptor);
    return effect;
}


//-----------------------------------------------------------------------------
IRenderVisual *
FrontEnd::model_CreateParticles
        ( LPCSTR name
        )
{
    auto effect_descriptor =
        particles_.FindParticleEffectDescriptor(name);

    if (effect_descriptor)
    {
        auto effect = models_.CreateParticleEffect(effect_descriptor);
        return effect;
    }
    else
    {
        auto group_descriptor =
            particles_.FindParticleGroupDescriptor(name);
        VERIFY(group_descriptor);

        auto group = models_.CreateParticleGroup(group_descriptor);
        return group;
    }
}


//-----------------------------------------------------------------------------
void
FrontEnd::Render()
{
    VERIFY(g_pGamePersistent);

    /* In menu rendering */
    if (g_pGamePersistent->OnRenderPPUI_query())
    {
        main_menu_.Render();
        return;
    }

    /* Scene rendering */
    // TBI
}

#include "stdafx.h"

#include "soundrender_coreA.h"
#include "soundrender_targetA.h"

#include <efx.h>

CSoundRender_CoreA::CSoundRender_CoreA()
{
    pDevice = nullptr;
    pDeviceList = nullptr;
    pContext = nullptr;
    eaxSet = nullptr;
    eaxGet = nullptr;
}

CSoundRender_CoreA::~CSoundRender_CoreA() {}

BOOL CSoundRender_CoreA::EAXQuerySupport(BOOL bDeferred, const GUID* guid, u32 prop, void* val, u32 sz)
{
    if (AL_NO_ERROR != eaxGet(guid, prop, 0, val, sz))
        return FALSE;
    if (AL_NO_ERROR != eaxSet(guid, (bDeferred ? DSPROPERTY_EAXLISTENER_DEFERRED : 0) | prop, 0, val, sz))
        return FALSE;
    return TRUE;
}

BOOL CSoundRender_CoreA::EAXTestSupport(BOOL bDeferred)
{
    EAXLISTENERPROPERTIES ep;
    if (!EAXQuerySupport(bDeferred, &DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ROOM, &ep.lRoom, sizeof(LONG)))
        return FALSE;
    if (!EAXQuerySupport(bDeferred, &DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ROOMHF, &ep.lRoomHF, sizeof(LONG)))
        return FALSE;
    if (!EAXQuerySupport(bDeferred, &DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ROOMROLLOFFFACTOR, &ep.flRoomRolloffFactor, sizeof(float)))
        return FALSE;
    if (!EAXQuerySupport(bDeferred, &DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_DECAYTIME, &ep.flDecayTime, sizeof(float)))
        return FALSE;
    if (!EAXQuerySupport(bDeferred, &DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_DECAYHFRATIO, &ep.flDecayHFRatio, sizeof(float)))
        return FALSE;
    if (!EAXQuerySupport(bDeferred, &DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_REFLECTIONS, &ep.lReflections, sizeof(LONG)))
        return FALSE;
    if (!EAXQuerySupport(bDeferred, &DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_REFLECTIONSDELAY, &ep.flReflectionsDelay, sizeof(float)))
        return FALSE;
    if (!EAXQuerySupport(bDeferred, &DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_REVERB, &ep.lReverb, sizeof(LONG)))
        return FALSE;
    if (!EAXQuerySupport(bDeferred, &DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_REVERBDELAY, &ep.flReverbDelay, sizeof(float)))
        return FALSE;
    if (!EAXQuerySupport(bDeferred, &DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_ENVIRONMENTDIFFUSION, &ep.flEnvironmentDiffusion, sizeof(float)))
        return FALSE;
    if (!EAXQuerySupport(bDeferred, &DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_AIRABSORPTIONHF, &ep.flAirAbsorptionHF, sizeof(float)))
        return FALSE;
    if (!EAXQuerySupport(bDeferred, &DSPROPSETID_EAX_ListenerProperties, DSPROPERTY_EAXLISTENER_FLAGS, &ep.dwFlags, sizeof(DWORD)))
        return FALSE;
    return TRUE;
}

bool CSoundRender_CoreA::reopen_device(const char* deviceName) const
{
    if (alcIsExtensionPresent(pDevice, "ALC_SOFT_reopen_device"))
    {
        Msg("- snd has ALC_SOFT_reopen_device");

        typedef ALCboolean(ALC_APIENTRY * alcReopenDeviceSOFT_t)(ALCdevice*, const ALCchar*, const ALCint*);

        const alcReopenDeviceSOFT_t alcReopenDeviceSOFT = reinterpret_cast<alcReopenDeviceSOFT_t>(alcGetProcAddress(pDevice, "alcReopenDeviceSOFT"));

        if (alcReopenDeviceSOFT(pDevice, deviceName, nullptr))
        {
            return true;
        }
    }

    return false;
}

void CSoundRender_CoreA::_restart()
{
    if (!(bPresent && bReady))
        return;

    Msg("SOUND: restarting...");

    inherited::_restart();

    std::scoped_lock<std::mutex> m(m_bLocked);

    if (init_device_list())
    {
        pDeviceList->SelectBestDeviceId(/*notification_client.GetDefaultDeviceName().c_str()*/);

        R_ASSERT(snd_device_id >= 0 && snd_device_id < pDeviceList->GetNumDevices());
        const ALDeviceDesc& deviceDesc = pDeviceList->GetDeviceDesc(snd_device_id);

        if (reopen_device(deviceDesc.name))
        {
            const bool is_al_soft = deviceDesc.is_al_soft;

            for (u32 it = 0; it < s_targets.size(); it++)
            {
                CSoundRender_Target* T = s_targets[it];

                T->alAuxInit(AL_EFFECTSLOT_NULL);

                T->bEFX = false;
            }

            release_efx_objects();

            init_device_properties(is_al_soft);

            for (u32 it = 0; it < s_targets.size(); it++)
            {
                CSoundRender_Target* T = s_targets[it];

                if (bEFX)
                {
                    T->alAuxInit(slot);
                }

                T->bEFX = bEFX;
            }
        }
        else
        {
            Msg("! snd cannot reset device. Restart game to apply changes!");

            Msg("! snd last error %s", alcGetString(pDevice, alcGetError(pDevice)));
        }
    }
}

bool CSoundRender_CoreA::init_context(const ALDeviceDesc& deviceDesc)
{
    // OpenAL device
    pDevice = alcOpenDevice(deviceDesc.name);
    if (!pDevice)
    {
        Msg("SOUND: OpenAL: Failed to create device.");
        bPresent = FALSE;
        return false;
    }

    // Get the device specifier.
    //alcGetString(pDevice, ALC_DEVICE_SPECIFIER);

    // Create context
    pContext = alcCreateContext(pDevice, nullptr);
    if (!pContext)
    {
        alcCloseDevice(pDevice);
        pDevice = nullptr;

        Msg("SOUND: OpenAL: Failed to create context.");
        bPresent = FALSE;
        return false;
    }

    // clear errors
    alcGetError(pDevice);

    // Set active context
    AC_CHK(alcMakeContextCurrent(pContext));

    // clear errors
    alGetError();

    Msg("~[%s] OpenAL version: %s", __FUNCTION__, alGetString(AL_VERSION));

    return true;
}

bool CSoundRender_CoreA::init_device_list()
{
    xr_delete(pDeviceList);
    pDeviceList = xr_new<ALDeviceList>();

    if (0 == pDeviceList->GetNumDevices())
    {
        xr_delete(pDeviceList);

        Msg("SOUND: OpenAL: Can't create sound device.");
        bPresent = FALSE;

        return false;
    }

    return true;
}

void CSoundRender_CoreA::init_device_properties(const bool& is_al_soft)
{
    if (is_al_soft)
    {
        bool efx = alcIsExtensionPresent(pDevice, "ALC_EXT_EFX") == AL_TRUE;

        if (efx)
        {
            InitAlEFXAPI();

            bEFX = EFXTestSupport();

            Msg("[OpenAL] EFX: %s", bEFX ? "present" : "absent");
        }
    }
    else
    {
        u32 eax{};

        if (alIsExtensionPresent("EAX5.0"))
            eax = 5;
        else if (alIsExtensionPresent("EAX4.0"))
            eax = 4;
        else if (alIsExtensionPresent("EAX3.0"))
            eax = 3;
        else if (alIsExtensionPresent("EAX2.0"))
            eax = 2;

        if (eax)
        {
            // Check for EAX extension
            bEAX = true;

            eaxSet = (EAXSet)alGetProcAddress("EAXSet");
            if (eaxSet == nullptr)
                bEAX = false;
            eaxGet = (EAXGet)alGetProcAddress("EAXGet");
            if (eaxGet == nullptr)
                bEAX = false;

            if (bEAX)
            {
                bDeferredEAX = EAXTestSupport(TRUE);
                bEAX = EAXTestSupport(FALSE);
            }

            Msg("[OpenAL] EAX 2.0 extension: %s", bEAX ? "present" : "absent");
            Msg("[OpenAL] EAX 2.0 deferred: %s", bDeferredEAX ? "present" : "absent");
        }
    }
}

#define AL_STOP_SOURCES_ON_DISCONNECT_SOFT 0x19AB  // not in public yet

void CSoundRender_CoreA::_initialize(int stage)
{
    if (stage == 0)
    {
        init_device_list();

        return;
    }

    pDeviceList->SelectBestDeviceId(/*notification_client.GetDefaultDeviceName().c_str()*/);

    R_ASSERT(snd_device_id >= 0 && snd_device_id < pDeviceList->GetNumDevices());
    const ALDeviceDesc& deviceDesc = pDeviceList->GetDeviceDesc(snd_device_id);

    if (!init_context(deviceDesc))
        return;

    // initialize listener
    A_CHK(alListener3f(AL_POSITION, 0.f, 0.f, 0.f));
    A_CHK(alListener3f(AL_VELOCITY, 0.f, 0.f, 0.f));
    constexpr Fvector orient[2] = {{0.f, 0.f, 1.f}, {0.f, 1.f, 0.f}};
    A_CHK(alListenerfv(AL_ORIENTATION, &orient[0].x));
    A_CHK(alListenerf(AL_GAIN, 1.f));

    alDisable(AL_STOP_SOURCES_ON_DISCONNECT_SOFT); // not in public yet

    ALenum err = alGetError();
    if (err != AL_NO_ERROR)
    {
        Msg("!![%s] OpenAL AL_STOP_SOURCES_ON_DISCONNECT_SOFT error: %s", __FUNCTION__, alGetString(err));
    }

    const bool is_al_soft = deviceDesc.is_al_soft;
    
    init_device_properties(is_al_soft);

    inherited::_initialize(stage);

    if (stage == 1) // first initialize
    {
        // Pre-create targets
        CSoundRender_Target* T = nullptr;
        for (u32 tit = 0; tit < u32(psSoundTargets); tit++)
        {
            T = xr_new<CSoundRender_TargetA>();
            if (T->_initialize())
            {
                if (bEFX)

                {
                    T->alAuxInit(slot);
                }

                T->bAlSoft = is_al_soft;
                T->bEFX = bEFX;

                s_targets.push_back(T);
            }
            else
            {
                Msg("! SOUND: OpenAL: Max targets - [%u]", tit);
                T->_destroy();
                xr_delete(T);
                break;
            }
        }
    }

    bReady = TRUE;
    //notification_client.Start();
}

void CSoundRender_CoreA::set_master_volume(float f)
{
    if (bPresent)
    {
        A_CHK(alListenerf(AL_GAIN, f));
    }
}

void CSoundRender_CoreA::release_context()
{
    if (bEFX)
        release_efx_objects();

    // Reset the current context to NULL.
    alcMakeContextCurrent(nullptr);

    // Release the context and the device.
    alcDestroyContext(pContext);
    pContext = nullptr;
    alcCloseDevice(pDevice);
    pDevice = nullptr;
}

void CSoundRender_CoreA::_clear()
{
    //notification_client.Stop();
    bReady = FALSE;

    inherited::_clear();

    // remove targets
    CSoundRender_Target* T = nullptr;
    for (u32 tit = 0; tit < s_targets.size(); tit++)
    {
        T = s_targets[tit];
        T->_destroy();
        xr_delete(T);
    }

    release_context();

    xr_delete(pDeviceList);
}

void CSoundRender_CoreA::i_eax_set(const GUID* guid, u32 prop, void* val, u32 sz) { eaxSet(guid, prop, 0, val, sz); }
void CSoundRender_CoreA::i_eax_get(const GUID* guid, u32 prop, void* val, u32 sz) { eaxGet(guid, prop, 0, val, sz); }

void CSoundRender_CoreA::update_listener(const Fvector& P, const Fvector& D, const Fvector& N, float dt)
{
    inherited::update_listener(P, D, N, dt);

    if (!Listener.position.similar(P))
    {
        Listener.position.set(P);
        bListenerMoved = TRUE;
    }
    Listener.orientation[0].set(D.x, D.y, -D.z);
    Listener.orientation[1].set(N.x, N.y, -N.z);

    A_CHK(alListener3f(AL_POSITION, Listener.position.x, Listener.position.y, -Listener.position.z));
    A_CHK(alListener3f(AL_VELOCITY, 0.f, 0.f, 0.f));
    A_CHK(alListenerfv(AL_ORIENTATION, &Listener.orientation[0].x));
}

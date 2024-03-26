#include "stdafx.h"

#include "SoundRender_CoreA.h"

XRSOUND_API xr_token* snd_devices_token = nullptr;
XRSOUND_API u32 snd_device_id = u32(-1);

CSoundRender_CoreA* impl = nullptr;

void CSound_manager_interface::_create(int stage)
{
    if (stage == 0)
    {
        impl = xr_new<CSoundRender_CoreA>();
        Sound = SoundRender = impl;

        if (strstr(Core.Params, "-nosound"))
        {
            SoundRender->bPresent = FALSE;
            return;
        }

        SoundRender->bPresent = TRUE; // make assumption that sound present
    }

    if (!SoundRender->bPresent)
        return;

    SoundRender->_initialize(stage);
}

void CSound_manager_interface::_destroy()
{
    SoundRender->_clear();
    SoundRender = nullptr;

    Sound = nullptr;

    xr_delete(impl);
}

//////////////////////////////////////////////////////////////////////
// HudSound.cpp:	структура для работы со звуками применяемыми в
//					HUD-объектах (обычные звуки, но с доп. параметрами)
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "HudSound.h"
#include "../xr_3da/x_ray.h"

void HUD_SOUND::LoadSound(LPCSTR section, LPCSTR line, HUD_SOUND& hud_snd, int type)
{
    hud_snd.m_activeSnd = NULL;
    hud_snd.sounds.clear();

    string256 sound_line;
    strcpy_s(sound_line, line);
    int k = 0;
    while (pSettings->line_exist(section, sound_line))
    {
        SSnd& s = hud_snd.sounds.emplace_back();

        LoadSound(section, sound_line, s.snd, type, &s.volume, &s.delay, &s.freq);
        sprintf_s(sound_line, "%s%d", line, ++k);
    } // while

    ASSERT_FMT(!hud_snd.sounds.empty(), "there is no sounds [%s] for [%s]", line, section);
}

void HUD_SOUND::LoadSound(LPCSTR section, LPCSTR line, ref_sound& snd, int type, float* volume, float* delay, float* freq)
{
    LPCSTR str = pSettings->r_string(section, line);
    string256 buf_str;

    int count = _GetItemCount(str);
    R_ASSERT(count);

    _GetItem(str, 0, buf_str);
    snd.create(buf_str, st_Effect, type);

    if (volume != NULL)
    {
        *volume = 1.f;
        if (count > 1)
        {
            _GetItem(str, 1, buf_str);
            if (xr_strlen(buf_str) > 0)
                *volume = (float)atof(buf_str);
        }
    }

    if (delay != NULL)
    {
        *delay = 0;
        if (count > 2)
        {
            _GetItem(str, 2, buf_str);
            if (xr_strlen(buf_str) > 0)
                *delay = (float)atof(buf_str);
        }
    }

    if (freq != NULL)
    {
        *freq = 1.f;
        if (count > 3)
        {
            _GetItem(str, 3, buf_str);
            if (xr_strlen(buf_str) > 0)
                *freq = (float)atof(buf_str);
        }
    }
}

void HUD_SOUND::DestroySound(HUD_SOUND& hud_snd)
{
    xr_vector<SSnd>::iterator it = hud_snd.sounds.begin();
    for (; it != hud_snd.sounds.end(); ++it)
        (*it).snd.destroy();
    hud_snd.sounds.clear();

    hud_snd.m_activeSnd = NULL;
}

void HUD_SOUND::PlaySound(HUD_SOUND& hud_snd, const Fvector& position, const CObject* parent, bool b_hud_mode, bool looped, bool overlap)
{
    if (hud_snd.sounds.empty())
        return;

    if (!overlap)
        StopSound(hud_snd);

    u32 flags = b_hud_mode ? sm_2D : 0;
    if (looped)
        flags |= sm_Looped;

    hud_snd.m_activeSnd = &hud_snd.sounds[Random.randI(hud_snd.sounds.size())];
    float freq = hud_snd.m_activeSnd->freq;
    Fvector pos = (flags & sm_2D) ? Fvector{} : position;

    static const float hud_vol = READ_IF_EXISTS(pSettings, r_float, "hud_sound", "hud_sound_vol_k", 1.0f);
    float vol = hud_snd.m_activeSnd->volume * (b_hud_mode ? hud_vol : 1.0f);

    if (overlap)
    {
        hud_snd.m_activeSnd->snd.play_no_feedback(const_cast<CObject*>(parent), flags, hud_snd.m_activeSnd->delay, &pos, &vol , &freq);
    }
    else
    {
        hud_snd.m_activeSnd->snd.play_at_pos(const_cast<CObject*>(parent), pos, flags, hud_snd.m_activeSnd->delay);
        hud_snd.m_activeSnd->snd.set_volume(vol);
        hud_snd.m_activeSnd->snd.set_frequency(freq);
    }
}

void HUD_SOUND::StopSound(HUD_SOUND& hud_snd)
{
    for (auto& sound : hud_snd.sounds)
        sound.snd.stop();

    hud_snd.m_activeSnd = nullptr;
}

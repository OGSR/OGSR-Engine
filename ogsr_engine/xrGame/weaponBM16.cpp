#include "stdafx.h"
#include "weaponBM16.h"

CWeaponBM16::~CWeaponBM16() { HUD_SOUND::DestroySound(m_sndReload1); }

void CWeaponBM16::Load(LPCSTR section)
{
    inherited::Load(section);

    HUD_SOUND::LoadSound(section, "snd_reload_1", m_sndReload1, m_eSoundReload);
}

void CWeaponBM16::PlayReloadSound()
{
    if (m_magazine.size() == 1 || !HaveCartridgeInInventory(2))
        PlaySound((IsMisfire() && !sndReloadJammed.sounds.empty()) ? sndReloadJammed : m_sndReload1, get_LastFP());
    else
        PlaySound((IsMisfire() && !sndReloadJammed.sounds.empty()) ? sndReloadJammed : sndReload, get_LastFP());
}

void CWeaponBM16::UpdateSounds()
{
    inherited::UpdateSounds();

    if (m_sndReload1.playing())
        m_sndReload1.set_position(get_LastFP());
}

void CWeaponBM16::PlayAnimShoot()
{
    if (m_magazine.empty())
        return;

    string128 guns_shoot_anm;
    xr_strconcat(guns_shoot_anm, "anm_shoot", (this->IsZoomed() && !this->IsRotatingToZoom()) ? "_aim" : "", IsMisfire() ? "_jammed_" : "_",
                 std::to_string(m_magazine.size()).c_str());
    if (AnimationExist(guns_shoot_anm))
    {
        PlayHUDMotion(guns_shoot_anm, false, GetState());
        return;
    }

    switch (m_magazine.size())
    {
    case 1: PlayHUDMotion({"anim_shoot_1", "anm_shot_1"}, false, GetState()); break;
    case 2: PlayHUDMotion({"anim_shoot", "anm_shot_2"}, false, GetState()); break;
    default: PlayHUDMotion({"anim_shoot", "anm_shots"}, false, GetState()); break; // А что, у БМ бывает больше двух патронов?...
    }
}

void CWeaponBM16::PlayAnimFakeShoot()
{
    string128 guns_fakeshoot_anm;
    xr_strconcat(guns_fakeshoot_anm, "anm_fakeshoot", (IsZoomed() && !IsRotatingToZoom()) ? (IsMisfire() ? "_aim_jammed_" : "_aim_") : "_",
                 std::to_string(m_magazine.size()).c_str());
    if (AnimationExist(guns_fakeshoot_anm))
        PlayHUDMotion(guns_fakeshoot_anm, true, GetState());
}

void CWeaponBM16::PlayAnimShow()
{
    switch (m_magazine.size())
    {
    case 0: PlayHUDMotion({IsMisfire() ? "anm_show_jammed_0" : "nullptr", "anim_draw_0", "anim_draw", "anm_show_0"}, true, GetState()); break;
    case 1: PlayHUDMotion({IsMisfire() ? "anm_show_jammed_1" : "nullptr", "anim_draw_1", "anim_draw", "anm_show_1"}, true, GetState()); break;
    case 2: PlayHUDMotion({IsMisfire() ? "anm_show_jammed_2" : "nullptr", "anim_draw_2", "anim_draw", "anm_show_2"}, true, GetState()); break;
    }
}

void CWeaponBM16::PlayAnimHide()
{
    switch (m_magazine.size())
    {
    case 0: PlayHUDMotion({IsMisfire() ? "anm_hide_jammed_0" : "nullptr", "anim_holster_0", "anim_holster", "anm_hide_0"}, true, GetState()); break;
    case 1: PlayHUDMotion({IsMisfire() ? "anm_hide_jammed_1" : "nullptr", "anim_holster_1", "anim_holster", "anm_hide_1"}, true, GetState()); break;
    case 2: PlayHUDMotion({IsMisfire() ? "anm_hide_jammed_2" : "nullptr", "anim_holster_2", "anim_holster", "anm_hide_2"}, true, GetState()); break;
    }
}

void CWeaponBM16::PlayAnimReload()
{
    if (m_magazine.size() == 1 || !HaveCartridgeInInventory(2))
        PlayHUDMotion({IsMisfire() ? "anm_reload_jammed_1" : "nullptr", "anim_reload_1", "anm_reload_1"}, true, GetState());
    else
        PlayHUDMotion({IsMisfire() ? "anm_reload_jammed_2" : "nullptr", "anim_reload", "anm_reload_2"}, true, GetState());
}

void CWeaponBM16::PlayAnimIdleMoving()
{
    switch (m_magazine.size())
    {
    case 0: PlayHUDMotion({IsMisfire() ? "anm_idle_moving_jammed_0" : "nullptr", "anim_idle_moving", "anim_idle", "anm_idle_moving_0"}, true, GetState()); break;
    case 1:
        PlayHUDMotion({IsMisfire() ? "anm_idle_moving_jammed_1" : "nullptr", "anim_idle_moving_1", "anim_idle_moving", "anim_idle_1", "anm_idle_moving_1"}, true, GetState());
        break;
    case 2:
        PlayHUDMotion({IsMisfire() ? "anm_idle_moving_jammed_2" : "nullptr", "anim_idle_moving_2", "anim_idle_moving", "anim_idle_2", "anm_idle_moving_2"}, true, GetState());
        break;
    }
}

void CWeaponBM16::PlayAnimIdleMovingSlow()
{
    switch (m_magazine.size())
    {
    case 0:
        PlayHUDMotion({IsMisfire() ? "anm_idle_moving_slow_jammed_0" : "nullptr", "anm_idle_moving_slow_0", "anim_idle_moving", "anim_idle", "anm_idle_moving_0"}, true,
                      GetState());
        break;
    case 1:
        PlayHUDMotion(
            {IsMisfire() ? "anm_idle_moving_slow_jammed_1" : "nullptr", "anm_idle_moving_slow_1", "anim_idle_moving_1", "anim_idle_moving", "anim_idle_1", "anm_idle_moving_1"},
            true, GetState());
        break;
    case 2:
        PlayHUDMotion(
            {IsMisfire() ? "anm_idle_moving_slow_jammed_2" : "nullptr", "anm_idle_moving_slow_2", "anim_idle_moving_2", "anim_idle_moving", "anim_idle_2", "anm_idle_moving_2"},
            true, GetState());
        break;
    }
}

void CWeaponBM16::PlayAnimIdleMovingCrouch()
{
    switch (m_magazine.size())
    {
    case 0:
        PlayHUDMotion({IsMisfire() ? "anm_idle_moving_crouch_jammed_0" : "nullptr", "anm_idle_moving_crouch_0", "anim_idle_moving", "anim_idle", "anm_idle_moving_0"}, true,
                      GetState());
        break;
    case 1:
        PlayHUDMotion(
            {IsMisfire() ? "anm_idle_moving_crouch_jammed_1" : "nullptr", "anm_idle_moving_crouch_1", "anim_idle_moving_1", "anim_idle_moving", "anim_idle_1", "anm_idle_moving_1"},
            true, GetState());
        break;
    case 2:
        PlayHUDMotion(
            {IsMisfire() ? "anm_idle_moving_crouch_jammed_2" : "nullptr", "anm_idle_moving_crouch_2", "anim_idle_moving_2", "anim_idle_moving", "anim_idle_2", "anm_idle_moving_2"},
            true, GetState());
        break;
    }
}

void CWeaponBM16::PlayAnimIdleMovingCrouchSlow()
{
    switch (m_magazine.size())
    {
    case 0:
        PlayHUDMotion({IsMisfire() ? "anm_idle_moving_crouch_slow_jammed_0" : "nullptr", "anm_idle_moving_crouch_slow_0", "anm_idle_moving_crouch_0", "anim_idle_moving",
                       "anim_idle", "anm_idle_moving_0"},
                      true, GetState());
        break;
    case 1:
        PlayHUDMotion({IsMisfire() ? "anm_idle_moving_crouch_slow_jammed_1" : "nullptr", "anm_idle_moving_crouch_slow_1", "anm_idle_moving_crouch_1", "anim_idle_moving_1",
                       "anim_idle_moving", "anim_idle_1", "anm_idle_moving_1"},
                      true, GetState());
        break;
    case 2:
        PlayHUDMotion({IsMisfire() ? "anm_idle_moving_crouch_slow_jammed_2" : "nullptr", "anm_idle_moving_crouch_slow_2", "anm_idle_moving_crouch_2", "anim_idle_moving_2",
                       "anim_idle_moving", "anim_idle_2", "anm_idle_moving_2"},
                      true, GetState());
        break;
    }
}

void CWeaponBM16::PlayAnimIdleSprint()
{
    switch (m_magazine.size())
    {
    case 0: PlayHUDMotion({IsMisfire() ? "anm_idle_sprint_jammed_0" : "nullptr", "anim_idle_sprint", "anim_idle", "anm_idle_sprint_0"}, true, GetState()); break;
    case 1:
        PlayHUDMotion({IsMisfire() ? "anm_idle_sprint_jammed_1" : "nullptr", "anim_idle_sprint_1", "anim_idle_sprint", "anim_idle_1", "anm_idle_sprint_1"}, true, GetState());
        break;
    case 2:
        PlayHUDMotion({IsMisfire() ? "anm_idle_sprint_jammed_2" : "nullptr", "anim_idle_sprint_2", "anim_idle_sprint", "anim_idle_2", "anm_idle_sprint_2"}, true, GetState());
        break;
    }
}

void CWeaponBM16::PlayAnimSprintStart()
{
    const std::string size = std::to_string(m_magazine.size());
    const char* anm_name = IsMisfire() ? "anm_idle_sprint_start_jammed_" : "anm_idle_sprint_start_";
    const std::string result = anm_name + size;

    if (AnimationExist(result.c_str()))
        PlayHUDMotion(result.c_str(), true, GetState());
    else
    {
        SprintType = true;
        SwitchState(eIdle);
    }
}

void CWeaponBM16::PlayAnimSprintEnd()
{
    const std::string size = std::to_string(m_magazine.size());
    const char* anm_name = IsMisfire() ? "anm_idle_sprint_end_jammed_" : "anm_idle_sprint_end_";
    const std::string result = anm_name + size;

    if (AnimationExist(result.c_str()))
        PlayHUDMotion(result.c_str(), true, GetState());
    else
    {
        SprintType = false;
        SwitchState(eIdle);
    }
}

const char* CWeaponBM16::GetAnimAimName()
{
    if (auto pActor = smart_cast<const CActor*>(H_Parent()))
    {
        if (AnmIdleMovingAllowed())
        {
            if (const u32 state = pActor->get_state(); state & mcAnyMove)
            {
                if (IsScopeAttached())
                    return xr_strconcat(guns_bm_aim_anm, "anm_idle_aim_scope_moving", IsMisfire() ? "_jammed_" : "_");
                else
                    return xr_strconcat(guns_bm_aim_anm, "anm_idle_aim_moving", (state & mcFwd) ? "_forward" : ((state & mcBack) ? "_back" : ""),
                                        (state & mcLStrafe) ? "_left" : ((state & mcRStrafe) ? "_right" : ""), IsMisfire() ? "_jammed_" : "_");
            }
        }
    }
    return nullptr;
}

void CWeaponBM16::PlayAnimIdle()
{
    if (TryPlayAnimIdle())
        return;

    if (IsZoomed())
    {
        if (IsRotatingToZoom() && !IsRotatingFromZoom())
        {
            string128 guns_bm_aim_anm;
            xr_strconcat(guns_bm_aim_anm, "anm_idle_aim_start", IsMisfire() ? "_jammed_" : "_", std::to_string(m_magazine.size()).c_str());
            if (AnimationExist(guns_bm_aim_anm))
            {
                PlayHUDMotion(guns_bm_aim_anm, true, GetState());
                PlaySound(sndAimStart, get_LastFP());
                return;
            }
        }

        if (const char* guns_bm_aim_anm = GetAnimAimName())
        {
            string128 guns_bm_aim_anm_full;
            xr_strconcat(guns_bm_aim_anm_full, guns_bm_aim_anm, std::to_string(m_magazine.size()).c_str());
            if (AnimationExist(guns_bm_aim_anm_full))
            {
                PlayHUDMotion(guns_bm_aim_anm_full, true, GetState());
                return;
            }
        }

        switch (m_magazine.size())
        {
        case 0: PlayHUDMotion({IsMisfire() ? "anm_idle_aim_jammed_0" : "nullptr", "anim_idle", "anm_idle_aim_0"}, true, GetState()); break;
        case 1: PlayHUDMotion({IsMisfire() ? "anm_idle_aim_jammed_1" : "nullptr", "anim_zoomed_idle_1", "anm_idle_aim_1"}, true, GetState()); break;
        case 2: PlayHUDMotion({IsMisfire() ? "anm_idle_aim_jammed_2" : "nullptr", "anim_zoomed_idle_2", "anim_zoomedidle_2", "anm_idle_aim_2"}, true, GetState()); break;
        }
    }
    else
    {
        if (IsRotatingFromZoom() && !IsRotatingToZoom())
        {
            string128 guns_aim_anm;
            xr_strconcat(guns_aim_anm, "anm_idle_aim_end_", IsMisfire() ? "jammed_" : "", std::to_string(m_magazine.size()).c_str());
            if (AnimationExist(guns_aim_anm))
            {
                PlayHUDMotion(guns_aim_anm, true, GetState());
                PlaySound(sndAimEnd, get_LastFP());
                return;
            }
        }

        switch (m_magazine.size())
        {
        case 0: PlayHUDMotion({IsMisfire() ? "anm_idle_jammed_0" : "nullptr", "anim_idle", "anm_idle_0"}, true, GetState()); break;
        case 1: PlayHUDMotion({IsMisfire() ? "anm_idle_jammed_1" : "nullptr", "anim_idle_1", "anm_idle_1"}, true, GetState()); break;
        case 2: PlayHUDMotion({IsMisfire() ? "anm_idle_jammed_2" : "nullptr", "anim_idle_2", "anm_idle_2"}, true, GetState()); break;
        }
    }
}

void CWeaponBM16::PlayAnimCheckMisfire()
{
    string128 guns_fakeshoot_anm;
    xr_strconcat(guns_fakeshoot_anm, "anm_fakeshoot", IsMisfire() ? "_jammed_" : "_", std::to_string(m_magazine.size()).c_str());
    if (AnimationExist(guns_fakeshoot_anm))
        PlayHUDMotion(guns_fakeshoot_anm, true, GetState());
    else
        SwitchState(eIdle);
}

void CWeaponBM16::PlayAnimDeviceSwitch()
{
    PlaySound((HeadLampSwitch || NightVisionSwitch) ? sndItemOn : sndTactItemOn, get_LastFP());

    string128 guns_device_anm;
    xr_strconcat(guns_device_anm, LaserSwitch ? "anm_laser_on" : (TorchSwitch ? "anm_torch_on" : ((HeadLampSwitch || NightVisionSwitch) ? "anm_headlamp_on" : "")),
                 IsMisfire() ? "_jammed_" : "_", std::to_string(m_magazine.size()).c_str());
    if (AnimationExist(guns_device_anm))
        PlayHUDMotion(guns_device_anm, true, GetState());
    else
    {
        DeviceUpdate();
        SwitchState(eIdle);
    }
}

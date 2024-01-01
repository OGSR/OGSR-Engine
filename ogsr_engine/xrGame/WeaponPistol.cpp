#include "stdafx.h"
#include "weaponpistol.h"
#include "ParticlesObject.h"
#include "actor.h"

CWeaponPistol::CWeaponPistol(LPCSTR name) : CWeaponCustomPistol(name)
{
    m_eSoundClose = ESoundTypes(SOUND_TYPE_WEAPON_RECHARGING /*| eSoundType*/);
    m_opened = false;
}

CWeaponPistol::~CWeaponPistol(void) {}

void CWeaponPistol::net_Destroy()
{
    inherited::net_Destroy();

    // sounds
    HUD_SOUND::DestroySound(sndClose);
}

void CWeaponPistol::Load(LPCSTR section)
{
    inherited::Load(section);

    HUD_SOUND::LoadSound(section, "snd_close", sndClose, m_eSoundClose);
}

void CWeaponPistol::OnH_B_Chield()
{
    inherited::OnH_B_Chield();
    m_opened = false;
}

void CWeaponPistol::PlayAnimShow()
{
    VERIFY(GetState() == eShowing);

    m_opened = iAmmoElapsed == 0;

    if (m_opened && !IsMisfire())
        PlayHUDMotion({"anim_draw_empty", "anm_show_empty"}, false, GetState());
    else
        inherited::PlayAnimShow();
}

void CWeaponPistol::PlayAnimIdleSprint()
{
    if (m_opened && !IsMisfire())
        PlayHUDMotion({"anm_idle_sprint_empty", "anm_idle_sprint", "anim_idle_sprint_empty", "anim_idle_sprint", "anim_empty"}, true, GetState());
    else
        inherited::PlayAnimIdleSprint();
}

void CWeaponPistol::PlayAnimIdleMoving()
{
    if (m_opened && !IsMisfire())
        PlayHUDMotion({"anim_empty", "anm_idle_moving_empty"}, true, GetState());
    else
        inherited::PlayAnimIdleMoving();
}

void CWeaponPistol::PlayAnimIdleMovingSlow()
{
    if (m_opened && !IsMisfire())
        PlayHUDMotion({"anm_idle_moving_slow_empty", "anim_empty", "anm_idle_moving_empty"}, true, GetState());
    else
        inherited::PlayAnimIdleMovingSlow();
}

void CWeaponPistol::PlayAnimIdleMovingCrouch()
{
    if (m_opened && !IsMisfire())
        PlayHUDMotion({"anm_idle_moving_crouch_empty", "anim_empty", "anm_idle_moving_empty"}, true, GetState());
    else
        inherited::PlayAnimIdleMovingCrouch();
}

void CWeaponPistol::PlayAnimIdleMovingCrouchSlow()
{
    if (m_opened && !IsMisfire())
        PlayHUDMotion({"anm_idle_moving_crouch_slow_empty", "anm_idle_moving_crouch_empty", "anim_empty", "anm_idle_moving_empty"}, true, GetState());
    else
        inherited::PlayAnimIdleMovingCrouchSlow();
}

void CWeaponPistol::PlayAnimIdle()
{
    VERIFY(GetState() == eIdle);

    if (TryPlayAnimIdle())
        return;

    if (IsZoomed())
    {
        PlayAnimAim();
    }
    else if (m_opened && !IsMisfire())
    {
        if (IsRotatingFromZoom())
        {
            if (AnimationExist("anm_idle_aim_end_empty"))
            {
                PlayHUDMotion("anm_idle_aim_end_empty", true, GetState());
                PlaySound(sndAimEnd, get_LastFP());
                return;
            }
        }

        PlayHUDMotion({"anim_empty", "anm_idle_empty"}, true, GetState());
    }
    else
        inherited::PlayAnimIdle();
}

void CWeaponPistol::PlayAnimAim()
{
    if (m_opened && !IsMisfire())
    {
        if (IsRotatingToZoom())
        {
            if (AnimationExist("anm_idle_aim_start_empty"))
            {
                PlayHUDMotion("anm_idle_aim_start_empty", true, GetState());
                PlaySound(sndAimStart, get_LastFP());
                return;
            }
        }

        if (const char* guns_aim_anm = GetAnimAimName())
        {
            string128 guns_aim_anm_full;
            xr_strconcat(guns_aim_anm_full, guns_aim_anm, "_empty");
            if (AnimationExist(guns_aim_anm_full))
            {
                PlayHUDMotion(guns_aim_anm_full, true, GetState());
                return;
            }
        }

        PlayHUDMotion({"anim_empty", "anm_idle_aim_empty"}, true, GetState());
    }
    else
        inherited::PlayAnimAim();
}

void CWeaponPistol::PlayAnimReload()
{
    VERIFY(GetState() == eReload);
    if (m_opened && !IsMisfire())
        PlayHUDMotion({"anim_reload_empty", "anm_reload_empty"}, true, GetState());
    else
        inherited::PlayAnimReload();

    m_opened = false;
}

void CWeaponPistol::PlayAnimHide()
{
    VERIFY(GetState() == eHiding);
    if (m_opened && !IsMisfire())
    {
        PlaySound(sndClose, get_LastFP());
        PlayHUDMotion({"anim_close", "anm_hide_empty"}, true, GetState());
    }
    else
        inherited::PlayAnimHide();
}

void CWeaponPistol::PlayAnimShoot()
{
    string128 guns_shoot_anm;
    xr_strconcat(guns_shoot_anm, "anm_shoot", (this->IsZoomed() && !this->IsRotatingToZoom()) ? "_aim" : "", iAmmoElapsed == 1 ? "_last" : "",
                 this->IsSilencerAttached() ? "_sil" : "");

    if (AnimationExist(guns_shoot_anm))
    {
        PlayHUDMotion(guns_shoot_anm, false, GetState());
        m_opened = iAmmoElapsed < 2;
        return;
    }

    if (iAmmoElapsed > 1)
    {
        PlayHUDMotion({"anim_shoot", "anm_shots"}, false, GetState());
        m_opened = false;
    }
    else
    {
        PlayHUDMotion({"anim_shot_last", "anm_shot_l"}, false, GetState());
        m_opened = true;
    }
}

void CWeaponPistol::OnAnimationEnd(u32 state)
{
    if (state == eHiding && m_opened)
    {
        m_opened = false;
    }
    inherited::OnAnimationEnd(state);
}

void CWeaponPistol::UpdateSounds()
{
    inherited::UpdateSounds();

    if (sndClose.playing())
        sndClose.set_position(get_LastFP());
}
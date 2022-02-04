#include "stdafx.h"
#include "weaponBM16.h"

CWeaponBM16::~CWeaponBM16()
{
	HUD_SOUND::DestroySound(m_sndReload1);
}

void CWeaponBM16::Load	(LPCSTR section)
{
	inherited::Load		(section);

	HUD_SOUND::LoadSound(section, "snd_reload_1", m_sndReload1, m_eSoundReload);
}

void CWeaponBM16::PlayReloadSound()
{
	if ( m_magazine.size() == 1 || !HaveCartridgeInInventory( 2 ) )
		PlaySound	(m_sndReload1,get_LastFP());
	else
		PlaySound	(sndReload,get_LastFP());
}

void CWeaponBM16::UpdateSounds()
{
	inherited::UpdateSounds();

	if (m_sndReload1.playing())	m_sndReload1.set_position(get_LastFP());
}

void CWeaponBM16::PlayAnimShoot()
{
	if (m_magazine.empty())
		return;

	string_path guns_shoot_anm{};
	xr_strconcat(guns_shoot_anm, "anm_shoot", (this->IsZoomed() && !this->IsRotatingToZoom()) ? "_aim_" : "_", std::to_string(m_magazine.size()).c_str());
	if (AnimationExist(guns_shoot_anm)) {
		PlayHUDMotion(guns_shoot_anm, false, GetState());
		return;
	}

	switch (m_magazine.size())
	{
	case 1: PlayHUDMotion({ "anim_shoot_1", "anm_shot_1" }, false, GetState()); break;
	case 2: PlayHUDMotion({ "anim_shoot", "anm_shot_2" }, false, GetState()); break;
	default: PlayHUDMotion({ "anim_shoot", "anm_shots" }, false, GetState()); break; //А что, у БМ бывает больше двух патронов?...
	}
}

void CWeaponBM16::PlayAnimShow()
{
	VERIFY(GetState() == eShowing);

	switch (m_magazine.size())
	{
	case 0: PlayHUDMotion({ "anim_draw_0", "anim_draw", "anm_show_0" }, true, GetState()); break;
	case 1: PlayHUDMotion({ "anim_draw_1", "anim_draw", "anm_show_1" }, true, GetState()); break;
	case 2: PlayHUDMotion({ "anim_draw_2", "anim_draw", "anm_show_2" }, true, GetState()); break;
	}
}

void CWeaponBM16::PlayAnimHide()
{
	VERIFY(GetState() == eHiding);

	switch (m_magazine.size())
	{
	case 0: PlayHUDMotion({ "anim_holster_0", "anim_holster", "anm_hide_0" }, true, GetState()); break;
	case 1: PlayHUDMotion({ "anim_holster_1", "anim_holster", "anm_hide_1" }, true, GetState()); break;
	case 2: PlayHUDMotion({ "anim_holster_2", "anim_holster", "anm_hide_2" }, true, GetState()); break;
	}
}

void CWeaponBM16::PlayAnimReload()
{
	VERIFY(GetState() == eReload);
	if (m_magazine.size() == 1 || !HaveCartridgeInInventory(2))
		PlayHUDMotion({ "anim_reload_1", "anm_reload_1" }, true, GetState());
	else
		PlayHUDMotion({ "anim_reload", "anm_reload_2" }, true, GetState());
}

void CWeaponBM16::PlayAnimIdleMoving()
{
	switch (m_magazine.size())
	{
	case 0: PlayHUDMotion({ "anim_idle_moving", "anim_idle", "anm_idle_moving_0" }, true, GetState()); break;
	case 1: PlayHUDMotion({ "anim_idle_moving_1", "anim_idle_moving", "anim_idle_1", "anm_idle_moving_1" }, true, GetState()); break;
	case 2: PlayHUDMotion({ "anim_idle_moving_2", "anim_idle_moving", "anim_idle_2", "anm_idle_moving_2" }, true, GetState()); break;
	}
}

void CWeaponBM16::PlayAnimIdleMovingSlow()
{
	switch (m_magazine.size())
	{
	case 0: PlayHUDMotion({ "anm_idle_moving_slow_0", "anim_idle_moving", "anim_idle", "anm_idle_moving_0" }, true, GetState()); break;
	case 1: PlayHUDMotion({ "anm_idle_moving_slow_1", "anim_idle_moving_1", "anim_idle_moving", "anim_idle_1", "anm_idle_moving_1" }, true, GetState()); break;
	case 2: PlayHUDMotion({ "anm_idle_moving_slow_2", "anim_idle_moving_2", "anim_idle_moving", "anim_idle_2", "anm_idle_moving_2" }, true, GetState()); break;
	}
}

void CWeaponBM16::PlayAnimIdleMovingCrouch()
{
	switch (m_magazine.size())
	{
	case 0: PlayHUDMotion({ "anm_idle_moving_crouch_0", "anim_idle_moving", "anim_idle", "anm_idle_moving_0" }, true, GetState()); break;
	case 1: PlayHUDMotion({ "anm_idle_moving_crouch_1", "anim_idle_moving_1", "anim_idle_moving", "anim_idle_1", "anm_idle_moving_1" }, true, GetState()); break;
	case 2: PlayHUDMotion({ "anm_idle_moving_crouch_2", "anim_idle_moving_2", "anim_idle_moving", "anim_idle_2", "anm_idle_moving_2" }, true, GetState()); break;
	}
}

void CWeaponBM16::PlayAnimIdleMovingCrouchSlow()
{
	switch (m_magazine.size())
	{
	case 0: PlayHUDMotion({ "anm_idle_moving_crouch_slow_0", "anm_idle_moving_crouch_0", "anim_idle_moving", "anim_idle", "anm_idle_moving_0" }, true, GetState()); break;
	case 1: PlayHUDMotion({ "anm_idle_moving_crouch_slow_1", "anm_idle_moving_crouch_1", "anim_idle_moving_1", "anim_idle_moving", "anim_idle_1", "anm_idle_moving_1" }, true, GetState()); break;
	case 2: PlayHUDMotion({ "anm_idle_moving_crouch_slow_2", "anm_idle_moving_crouch_2", "anim_idle_moving_2", "anim_idle_moving", "anim_idle_2", "anm_idle_moving_2" }, true, GetState()); break;
	}
}

void CWeaponBM16::PlayAnimIdleSprint()
{
	switch (m_magazine.size())
	{
	case 0: PlayHUDMotion({ "anim_idle_sprint", "anim_idle", "anm_idle_sprint_0" }, true, GetState()); break;
	case 1: PlayHUDMotion({ "anim_idle_sprint_1", "anim_idle_sprint", "anim_idle_1", "anm_idle_sprint_1" }, true, GetState()); break;
	case 2: PlayHUDMotion({ "anim_idle_sprint_2", "anim_idle_sprint", "anim_idle_2", "anm_idle_sprint_2" }, true, GetState()); break;
	}
}

void CWeaponBM16::PlayAnimIdle()
{
	if (TryPlayAnimIdle())
		return;

	if (IsZoomed())
	{
		if (IsRotatingToZoom()) {
			string32 guns_aim_anm;
			xr_strconcat(guns_aim_anm, "anm_idle_aim_start_", std::to_string(m_magazine.size()).c_str());
			if (AnimationExist(guns_aim_anm)) {
				PlayHUDMotion(guns_aim_anm, true, GetState());
				return;
			}
		}

		if (const char* guns_aim_anm = GetAnimAimName()) {
			string64 guns_aim_anm_full;
			xr_strconcat(guns_aim_anm_full, guns_aim_anm, "_", std::to_string(m_magazine.size()).c_str());
			if (AnimationExist(guns_aim_anm_full)) {
				PlayHUDMotion(guns_aim_anm_full, true, GetState());
				return;
			}
		}

		switch (m_magazine.size())
		{
		case 0: PlayHUDMotion({ "anim_idle", "anm_idle_aim_0" }, true, GetState()); break;
		case 1: PlayHUDMotion({ "anim_zoomed_idle_1", "anm_idle_aim_1" }, true, GetState()); break;
		case 2: PlayHUDMotion({ "anim_zoomed_idle_2", "anim_zoomedidle_2", "anm_idle_aim_2" }, true, GetState()); break;
		}
	}
	else
	{
		if (IsRotatingFromZoom()) {
			string32 guns_aim_anm;
			xr_strconcat(guns_aim_anm, "anm_idle_aim_end_", std::to_string(m_magazine.size()).c_str());
			if (AnimationExist(guns_aim_anm)) {
				PlayHUDMotion(guns_aim_anm, true, GetState());
				return;
			}
		}

		switch (m_magazine.size())
		{
		case 0: PlayHUDMotion({ "anim_idle", "anm_idle_0" }, true, GetState()); break;
		case 1: PlayHUDMotion({ "anim_idle_1", "anm_idle_1" }, true, GetState()); break;
		case 2: PlayHUDMotion({ "anim_idle_2", "anm_idle_2" }, true, GetState()); break;
		}
	}
}

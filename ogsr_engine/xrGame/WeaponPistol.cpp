#include "stdafx.h"
#include "weaponpistol.h"
#include "ParticlesObject.h"
#include "actor.h"

CWeaponPistol::CWeaponPistol(LPCSTR name) : CWeaponCustomPistol(name)
{
	m_eSoundClose		= ESoundTypes(SOUND_TYPE_WEAPON_RECHARGING /*| eSoundType*/);
	m_opened = false;
	SetPending(FALSE);
}

CWeaponPistol::~CWeaponPistol(void)
{
}

void CWeaponPistol::net_Destroy()
{
	inherited::net_Destroy();

	// sounds
	HUD_SOUND::DestroySound(sndClose);
}

void CWeaponPistol::net_Relcase(CObject *object)
{
	inherited::net_Relcase(object);
}

void CWeaponPistol::OnDrawUI()
{
	inherited::OnDrawUI();
}

void CWeaponPistol::Load	(LPCSTR section)
{
	inherited::Load		(section);

	HUD_SOUND::LoadSound(section, "snd_close", sndClose, m_eSoundClose);
}

void CWeaponPistol::OnH_B_Chield		()
{
	inherited::OnH_B_Chield		();
	m_opened = false;
}

void CWeaponPistol::PlayAnimShow()
{
	VERIFY(GetState()==eShowing);
	if (iAmmoElapsed >= 1)
		m_opened = false;
	else
		m_opened = true;
		
	if (m_opened)
		PlayHUDMotion("anim_draw_empty", "anm_show_empty", FALSE, this, GetState());
	else
		inherited::PlayAnimShow();
}

void CWeaponPistol::PlayAnimBore()
{
	if (m_opened)
		PlayHUDMotion("anim_empty", "anm_bore_empty", TRUE, this, GetState());
	else
		inherited::PlayAnimBore();
}

void CWeaponPistol::PlayAnimIdleSprint()
{
	if (m_opened)
	{
		if (AnimationExist("anm_idle_sprint_empty") || AnimationExist("anm_idle_sprint"))
			PlayHUDMotion("anm_idle_sprint_empty", "anm_idle_sprint", TRUE, nullptr, GetState());
		else if (AnimationExist("anim_idle_sprint_empty") || AnimationExist("anim_idle_sprint"))
			PlayHUDMotion("anim_idle_sprint_empty", "anim_idle_sprint", TRUE, nullptr, GetState());
		else
			PlayHUDMotion("anim_empty", TRUE, nullptr, GetState());
	}
	else
		inherited::PlayAnimIdleSprint();
}

void CWeaponPistol::PlayAnimIdleMoving()
{
	if (m_opened)
		PlayHUDMotion("anim_empty", "anm_idle_moving_empty", TRUE, nullptr, GetState());
	else
		inherited::PlayAnimIdleMoving();
}


void CWeaponPistol::PlayAnimIdle()
{
	VERIFY(GetState()==eIdle);

	if (TryPlayAnimIdle()) 
		return;

	if (m_opened)
		PlayHUDMotion("anim_empty", "anm_idle_empty", TRUE, nullptr, GetState());
	else
		inherited::PlayAnimIdle();
}

void CWeaponPistol::PlayAnimAim()
{
	if (m_opened)
		PlayHUDMotion("anim_empty", "anm_idle_aim_empty", TRUE, nullptr, GetState());
	else
		inherited::PlayAnimAim();
}

void CWeaponPistol::PlayAnimReload()
{
	VERIFY(GetState() == eReload);
	if (m_opened)
	{
		PlayHUDMotion("anim_reload_empty", "anm_reload_empty", TRUE, nullptr, GetState());
	}
	else 
	{
		if (IsPartlyReloading())
		{
			if (AnimationExist("anim_reload_partly"))
				PlayHUDMotion("anim_reload_partly", TRUE, nullptr, GetState());
			else if	(AnimationExist("anm_reload_partly"))
				PlayHUDMotion("anm_reload_partly", TRUE, nullptr, GetState());
			else
				PlayHUDMotion("anim_reload", "anm_reload", TRUE, nullptr, GetState());
		}
		else
		{
			inherited::PlayAnimReload();
		}
	}
	m_opened = false;
}

void CWeaponPistol::PlayAnimHide()
{
	VERIFY(GetState() == eHiding);
	if (m_opened)
	{
		PlaySound(sndClose, get_LastFP());
		PlayHUDMotion("anim_close", "anm_hide_empty", TRUE, this, GetState());
	}
	else
		inherited::PlayAnimHide();
}

void CWeaponPistol::PlayAnimShoot()
{
	VERIFY(GetState() == eFire || GetState() == eFire2);
	if (iAmmoElapsed > 1)
	{
		PlayHUDMotion("anim_shoot", "anm_shots", FALSE, this, GetState());
		m_opened = false;
	}
	else
	{
		PlayHUDMotion("anim_shoot_last", "anm_shot_l", FALSE, this, GetState());
		m_opened = true;
	}
}

void CWeaponPistol::switch2_Reload()
{
//.	if(GetState()==eReload) return;
	inherited::switch2_Reload();
}

void CWeaponPistol::OnAnimationEnd(u32 state)
{
	if(state == eHiding && m_opened) 
	{
		m_opened = false;
//		switch2_Hiding();
	} 
	inherited::OnAnimationEnd(state);
}

/*
void CWeaponPistol::OnShot		()
{
	// Sound
	PlaySound		(*m_pSndShotCurrent,get_LastFP());

	AddShotEffector	();
	
	PlayAnimShoot	();

	// Shell Drop
	Fvector vel; 
	PHGetLinearVell(vel);
	OnShellDrop					(get_LastSP(),  vel);

	// Огонь из ствола
	
	StartFlameParticles	();
	R_ASSERT2(!m_pFlameParticles || !m_pFlameParticles->IsLooped(),
			  "can't set looped particles system for shoting with pistol");
	
	//дым из ствола
	StartSmokeParticles	(get_LastFP(), vel);
}
*/

void CWeaponPistol::UpdateSounds()
{
	inherited::UpdateSounds();

	if (sndClose.playing())
		sndClose.set_position(get_LastFP());
}

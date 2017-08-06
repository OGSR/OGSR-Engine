#include "stdafx.h"
#include "weaponpistol.h"
#include "WeaponHUD.h"
#include "ParticlesObject.h"
#include "actor.h"

CWeaponPistol::CWeaponPistol(LPCSTR name) : CWeaponCustomPistol(name)
{
	m_eSoundClose		= ESoundTypes(SOUND_TYPE_WEAPON_RECHARGING /*| eSoundType*/);
	m_opened = false;
	m_bPending = false;
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

	animGet				(mhud_pistol.mhud_empty,		pSettings->r_string(*hud_sect, "anim_empty"));
	animGet				(mhud_pistol.mhud_shot_l,		pSettings->r_string(*hud_sect, "anim_shot_last"));
	animGet				(mhud_pistol.mhud_close,		pSettings->r_string(*hud_sect, "anim_close"));
	animGet				(mhud_pistol.mhud_show_empty,	pSettings->r_string(*hud_sect, "anim_draw_empty"));
	animGet				(mhud_pistol.mhud_reload_empty,	pSettings->r_string(*hud_sect, "anim_reload_empty"));

	string128			str;
	strconcat(sizeof(str),str,pSettings->r_string(*hud_sect, "anim_empty"),"_r");
	animGet				(mhud_pistol_r.mhud_empty,		str);

	strconcat(sizeof(str),str,pSettings->r_string(*hud_sect, "anim_shot_last"),"_r");
	animGet				(mhud_pistol_r.mhud_shot_l,		str);

	strconcat(sizeof(str),str,pSettings->r_string(*hud_sect, "anim_close"),"_r");
	animGet				(mhud_pistol_r.mhud_close,		str);

	strconcat(sizeof(str),str,pSettings->r_string(*hud_sect, "anim_draw_empty"),"_r");
	animGet				(mhud_pistol_r.mhud_show_empty,	str);

	strconcat(sizeof(str),str,pSettings->r_string(*hud_sect, "anim_reload_empty"),"_r");
	animGet				(mhud_pistol_r.mhud_reload_empty,	str);



	strconcat(sizeof(str),str,pSettings->r_string(*hud_sect, "anim_idle"),"_r");
	animGet				(wm_mhud_r.mhud_idle,	str);

	strconcat(sizeof(str),str,pSettings->r_string(*hud_sect, "anim_reload"),"_r");
	animGet				(wm_mhud_r.mhud_reload,	str);

	strconcat(sizeof(str),str,pSettings->r_string(*hud_sect, "anim_draw"),"_r");
	animGet				(wm_mhud_r.mhud_show,	str);

	strconcat(sizeof(str),str,pSettings->r_string(*hud_sect, "anim_holster"),"_r");
	animGet				(wm_mhud_r.mhud_hide,	str);

	strconcat(sizeof(str),str,pSettings->r_string(*hud_sect, "anim_shoot"),"_r");
	animGet				(wm_mhud_r.mhud_shots,	str);

	if(IsZoomEnabled()){
		strconcat(sizeof(str),str,pSettings->r_string(*hud_sect, "anim_idle_aim"),"_r");
		animGet				(wm_mhud_r.mhud_idle_aim,		str);
	}

}

void CWeaponPistol::OnH_B_Chield		()
{
	inherited::OnH_B_Chield		();
	m_opened = false;
}

void CWeaponPistol::PlayAnimShow	()
{
	VERIFY(GetState()==eShowing);
	if(iAmmoElapsed >= 1)
		m_opened = false;
	else
		m_opened = true;
		
	if(m_opened){ 
		CWeaponPistol::WWPMotions& m = wwpm_current();
		m_pHUD->animPlay(random_anim(m.mhud_show_empty),FALSE, this, GetState());
	}else{ 
		CWeaponMagazined::SWMmotions& m = swm_current();
		m_pHUD->animPlay(random_anim(m.mhud_show),FALSE, this, GetState());
	}
}

void CWeaponPistol::PlayAnimIdle	()
{
	VERIFY(GetState()==eIdle);
	if(m_opened){ 
		CWeaponPistol::WWPMotions& m = wwpm_current();
		m_pHUD->animPlay(random_anim(m.mhud_empty), TRUE, NULL, GetState());
	}else{
		CActor* A = smart_cast<CActor*>(H_Parent());
		if(A && A->Holder()){
			MotionSVec* m = (IsZoomed())?&wm_mhud_r.mhud_idle_aim:&wm_mhud_r.mhud_idle;
			m_pHUD->animPlay(random_anim(*m), TRUE, NULL, GetState());
		}else
			inherited::PlayAnimIdle		();
	}
}

void CWeaponPistol::PlayAnimReload	()
{	
	VERIFY(GetState()==eReload);
	if(m_opened){ 
		CWeaponPistol::WWPMotions& m = wwpm_current();
		m_pHUD->animPlay(random_anim(m.mhud_reload_empty), TRUE, this, GetState());
	}else{
		CWeaponMagazined::SWMmotions& m = swm_current();
		m_pHUD->animPlay(random_anim(m.mhud_reload), TRUE, this, GetState());
	}
	
	m_opened = false;		
}


void CWeaponPistol::PlayAnimHide()
{
	VERIFY(GetState()==eHiding);
	if(m_opened) 
	{
		PlaySound			(sndClose,get_LastFP());
		CWeaponPistol::WWPMotions& m = wwpm_current();
		m_pHUD->animPlay	(random_anim(m.mhud_close), TRUE, this, GetState());
	} 
	else 
		inherited::PlayAnimHide();
}

void CWeaponPistol::PlayAnimShoot	()
{
	VERIFY(GetState()==eFire || GetState()==eFire2);
	if(iAmmoElapsed > 1) 
	{
		CWeaponMagazined::SWMmotions& m = swm_current();
		m_pHUD->animPlay	(random_anim(m.mhud_shots), FALSE, this, GetState());
		m_opened = false;
	}
	else 
	{
		CWeaponPistol::WWPMotions& m = wwpm_current();
		m_pHUD->animPlay	(random_anim(m.mhud_shot_l), FALSE, this, GetState()); 
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

	// ќгонь из ствола
	
	StartFlameParticles	();
	R_ASSERT2(!m_pFlameParticles || !m_pFlameParticles->IsLooped(),
			  "can't set looped particles system for shoting with pistol");
	
	//дым из ствола
	StartSmokeParticles	(get_LastFP(), vel);
}

void CWeaponPistol::UpdateSounds()
{
	inherited::UpdateSounds();

	if (sndClose.playing()) sndClose.set_position	(get_LastFP());
}

CWeaponPistol::WWPMotions&	 CWeaponPistol::wwpm_current	()
{
	CActor* A = smart_cast<CActor*>(H_Parent());
	if(A && A->Holder()){	
//		Msg("right-hand animation playing");
		return				mhud_pistol_r;
	}
//	Msg("double-hands animation playing");
	return					mhud_pistol;
}

CWeaponMagazined::SWMmotions&	 CWeaponPistol::swm_current	()
{
	CActor* A = smart_cast<CActor*>(H_Parent());
	if(A && A->Holder()){
//.		Msg("right-hand animation playing");
		return				wm_mhud_r;
	}
//.	Msg("double-hands animation playing");
	return					mhud;
}

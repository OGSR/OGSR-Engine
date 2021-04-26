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

	animGetEx( mhud_pistol.mhud_empty,        "anim_empty" );
	animGetEx( mhud_pistol.mhud_shot_l,       "anim_shot_last" );
	animGetEx( mhud_pistol.mhud_close,        "anim_close" );
	animGetEx( mhud_pistol.mhud_show_empty,   "anim_draw_empty" );
	animGetEx( mhud_pistol.mhud_reload_empty, "anim_reload_empty" );

	animGetEx( mhud_pistol.mhud_idle_sprint_empty, pSettings->line_exist( hud_sect.c_str(), "anim_idle_sprint_empty" ) ? "anim_idle_sprint_empty" : pSettings->line_exist( hud_sect.c_str(), "anim_idle_sprint" ) ? "anim_idle_sprint" : "anim_empty" );
	animGetEx( mhud_pistol.mhud_idle_moving_empty, pSettings->line_exist( hud_sect.c_str(), "anim_idle_moving_empty" ) ? "anim_idle_moving_empty" : pSettings->line_exist( hud_sect.c_str(), "anim_idle_moving" ) ? "anim_idle_moving" : "anim_empty" );

	animGetEx( mhud_pistol_r.mhud_empty,        "anim_empty", "_r" );
	animGetEx( mhud_pistol_r.mhud_shot_l,       "anim_shot_last", "_r" );
	animGetEx( mhud_pistol_r.mhud_close,        "anim_close", "_r" );
	animGetEx( mhud_pistol_r.mhud_show_empty,   "anim_draw_empty", "_r" );
	animGetEx( mhud_pistol_r.mhud_reload_empty, "anim_reload_empty", "_r" );

	animGetEx( mhud_pistol_r.mhud_idle_sprint_empty, pSettings->line_exist(hud_sect.c_str(), "anim_idle_sprint_empty") ? "anim_idle_sprint_empty" : pSettings->line_exist(hud_sect.c_str(), "anim_idle_sprint") ? "anim_idle_sprint" : "anim_empty" );
	animGetEx( mhud_pistol_r.mhud_idle_moving_empty, pSettings->line_exist(hud_sect.c_str(), "anim_idle_moving_empty") ? "anim_idle_moving_empty" : pSettings->line_exist(hud_sect.c_str(), "anim_idle_moving") ? "anim_idle_moving" : "anim_empty" );

	animGetEx( wm_mhud_r.mhud_idle,             "anim_idle", "_r" );
	animGetEx( wm_mhud_r.mhud_reload,           "anim_reload", "_r" );
	animGetEx( wm_mhud_r.mhud_show,             "anim_draw", "_r" );
	animGetEx( wm_mhud_r.mhud_hide,             "anim_holster", "_r" ); // not used ???
	animGetEx( wm_mhud_r.mhud_shots,            "anim_shoot", "_r" );

	if(IsZoomEnabled()){
		animGetEx( wm_mhud_r.mhud_idle_aim, "anim_idle_aim", "_r" );
	}

	animGetEx( wm_mhud_r.mhud_reload_partly, "anim_reload_partly", "_r", "anim_reload" );
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

void CWeaponPistol::PlayAnimIdle( u8 state = eIdle ) {
	VERIFY(GetState()==eIdle);
	if(m_opened){ 
		if (TryPlayAnimIdle(state)) return;
		CWeaponPistol::WWPMotions& m = wwpm_current();
		m_pHUD->animPlay(random_anim(m.mhud_empty), TRUE, NULL, GetState());
	}else{
		CActor* A = smart_cast<CActor*>(H_Parent());
		if(A && A->Holder()){
			MotionSVec* m = (IsZoomed())?&wm_mhud_r.mhud_idle_aim:&wm_mhud_r.mhud_idle;
			m_pHUD->animPlay(random_anim(*m), TRUE, NULL, GetState());
		}else
			inherited::PlayAnimIdle( state );
	}
}

void CWeaponPistol::PlayAnimReload() {
  VERIFY( GetState() == eReload );
  if ( m_opened ) {
    CWeaponPistol::WWPMotions& m = wwpm_current();
    m_pHUD->animPlay( random_anim( m.mhud_reload_empty ), TRUE, this, GetState() );
  }
  else {
    CWeaponMagazined::SWMmotions& m = swm_current();
    if ( IsPartlyReloading() )
      m_pHUD->animPlay( random_anim( m.mhud_reload_partly ), TRUE, this, GetState() );
    else
      m_pHUD->animPlay( random_anim( m.mhud_reload ), TRUE, this, GetState() );
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

bool CWeaponPistol::TryPlayAnimIdle(u8 state = eIdle) 
{
	VERIFY(GetState() == eIdle);
	if (m_opened)
	{
		if ( !IsZoomed() ) {
			CWeaponPistol::WWPMotions& m = wwpm_current();
			switch (state) {
			case eSubstateIdleMoving:
				m_pHUD->animPlay(random_anim(m.mhud_idle_moving_empty), TRUE, NULL, GetState());
				return true;
			case eSubstateIdleSprint:
				m_pHUD->animPlay(random_anim(m.mhud_idle_sprint_empty), TRUE, NULL, GetState());
				return true;
			default:
				return false;
			}
		}
		return false;
	}
	else 
		return inherited::TryPlayAnimIdle(state);
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

#include "stdafx.h"
#include "weaponBM16.h"
#include "WeaponHUD.h"

CWeaponBM16::~CWeaponBM16()
{
	HUD_SOUND::DestroySound(m_sndReload1);
}

void CWeaponBM16::Load	(LPCSTR section)
{
	inherited::Load		(section);

	animGetEx( mhud_reload1,      "anim_reload_1" );

	animGetEx( mhud_shot1,        "anim_shoot_1" );
	animGetEx( mhud_shot2,	pSettings->line_exist(hud_sect.c_str(), "anim_shoot_2") ? "anim_shoot_2"
		: "anim_shoot");

	animGetEx( mhud_idle1,        "anim_idle_1" );
	animGetEx( mhud_idle2,        "anim_idle_2" );

	animGetEx( mhud_zoomed_idle1, "anim_zoomed_idle_1" );

	// что бы можно было нормальные имена в конфигах юзать
	if (pSettings->line_exist(hud_sect.c_str(), "anim_zoomed_idle_2"))
		animGetEx( mhud_zoomed_idle2, "anim_zoomed_idle_2" );
	else
		animGetEx(mhud_zoomed_idle2, "anim_zoomedidle_2");

	animGetEx( mhud_idle_sprint_1, pSettings->line_exist( hud_sect.c_str(), "anim_idle_sprint_1" ) ? "anim_idle_sprint_1" : pSettings->line_exist( hud_sect.c_str(), "anim_idle_sprint" ) ? "anim_idle_sprint" : "anim_idle_1" );
	animGetEx( mhud_idle_sprint_2, pSettings->line_exist( hud_sect.c_str(), "anim_idle_sprint_2" ) ? "anim_idle_sprint_2" : pSettings->line_exist( hud_sect.c_str(), "anim_idle_sprint" ) ? "anim_idle_sprint" : "anim_idle_2" );
	animGetEx( mhud_idle_moving_1, pSettings->line_exist( hud_sect.c_str(), "anim_idle_moving_1" ) ? "anim_idle_moving_1" : pSettings->line_exist( hud_sect.c_str(), "anim_idle_moving" ) ? "anim_idle_moving" : "anim_idle_1" );
	animGetEx( mhud_idle_moving_2, pSettings->line_exist( hud_sect.c_str(), "anim_idle_moving_2" ) ? "anim_idle_moving_2" : pSettings->line_exist( hud_sect.c_str(), "anim_idle_moving" ) ? "anim_idle_moving" : "anim_idle_2" );

	animGetEx(mhud_show1, pSettings->line_exist(hud_sect.c_str(), "anim_draw_1") ? "anim_draw_1"
		: "anim_draw");
	animGetEx(mhud_show2, pSettings->line_exist(hud_sect.c_str(), "anim_draw_2") ? "anim_draw_2"
		: "anim_draw");
	animGetEx(mhud_hide1, pSettings->line_exist(hud_sect.c_str(), "anim_holster_1") ? "anim_holster_1"
		: "anim_holster");
	animGetEx(mhud_hide2, pSettings->line_exist(hud_sect.c_str(), "anim_holster_2") ? "anim_holster_2"
		: "anim_holster");

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
	if (m_magazine.size() == 2)
		m_pHUD->animPlay(random_anim(mhud_shot2), TRUE, this, GetState());
	else if(m_magazine.size()==1)
		m_pHUD->animPlay(random_anim(mhud_shot1),TRUE, this, GetState());
	else
		m_pHUD->animPlay(random_anim(mhud.mhud_shots),TRUE, this, GetState());
}

void CWeaponBM16::PlayAnimReload()
{
	VERIFY(GetState()==eReload);
	if ( m_magazine.size() == 1 || !HaveCartridgeInInventory( 2 ) )
		m_pHUD->animPlay(random_anim(mhud_reload1),TRUE,this,GetState());
	else
		m_pHUD->animPlay(random_anim(mhud.mhud_reload),TRUE,this,GetState());

}

void CWeaponBM16::PlayAnimShow()
{
	VERIFY(GetState() == eShowing);

	switch (m_magazine.size())
	{
	case 0: {
		m_pHUD->animPlay(random_anim(mhud.mhud_show), FALSE, this, GetState());
	}break;
	case 1: {
		m_pHUD->animPlay(random_anim(mhud_show1), FALSE, this, GetState());
	}break;
	case 2: {
		m_pHUD->animPlay(random_anim(mhud_show2), FALSE, this, GetState());
	}break;
	};
}

void CWeaponBM16::PlayAnimHide()
{
	VERIFY(GetState() == eHiding);

	switch (m_magazine.size())
	{
	case 0: {
		m_pHUD->animPlay(random_anim(mhud.mhud_hide), TRUE, this, GetState());
	}break;
	case 1: {
		m_pHUD->animPlay(random_anim(mhud_hide1), TRUE, this, GetState());
	}break;
	case 2: {
		m_pHUD->animPlay(random_anim(mhud_hide2), TRUE, this, GetState());
	}break;
	};
}

void CWeaponBM16::PlayAnimIdle( u8 state = eIdle ) {
	if ( TryPlayAnimIdle( state ) ) return;

	if(IsZoomed())
	{
		switch (m_magazine.size())
		{
		case 0:{
			m_pHUD->animPlay(random_anim(mhud.mhud_idle_aim), TRUE, NULL, GetState());
		}break;
		case 1:{
			m_pHUD->animPlay(random_anim(mhud_zoomed_idle1), TRUE, NULL, GetState());
		}break;
		case 2:{
			m_pHUD->animPlay(random_anim(mhud_zoomed_idle2), TRUE, NULL, GetState());
		}break;
		};
	}else{
		switch (m_magazine.size())
		{
		case 0:{
			m_pHUD->animPlay(random_anim(mhud.mhud_idle), TRUE, NULL, GetState());
		}break;
		case 1:{
			m_pHUD->animPlay(random_anim(mhud_idle1), TRUE, NULL, GetState());
		}break;
		case 2:{
			m_pHUD->animPlay(random_anim(mhud_idle2), TRUE, NULL, GetState());
		}break;
		};
	}
}

bool CWeaponBM16::TryPlayAnimIdle(u8 state = eIdle) {
	VERIFY(GetState() == eIdle);
	if (!IsZoomed()) {
		switch (state) {
			case eSubstateIdleMoving:
				switch (m_magazine.size())
				{
				case 0: {
					m_pHUD->animPlay(random_anim(mhud.mhud_idle_moving), TRUE, NULL, GetState());
				}break;
				case 1: {
					m_pHUD->animPlay(random_anim(mhud_idle_moving_1), TRUE, NULL, GetState());
				}break;
				case 2: {
					m_pHUD->animPlay(random_anim(mhud_idle_moving_2), TRUE, NULL, GetState());
				}break;
				};
				return true;
			case eSubstateIdleSprint:
				switch (m_magazine.size())
				{
				case 0: {
					m_pHUD->animPlay(random_anim(mhud.mhud_idle_sprint), TRUE, NULL, GetState());
				}break;
				case 1: {
					m_pHUD->animPlay(random_anim(mhud_idle_sprint_1), TRUE, NULL, GetState());
				}break;
				case 2: {
					m_pHUD->animPlay(random_anim(mhud_idle_sprint_2), TRUE, NULL, GetState());
				}break;
				};
				return true;
			default:
				return false;
		}
	}
	return false;
}

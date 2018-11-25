#include "stdafx.h"

#include "WeaponKnife.h"
#include "WeaponHUD.h"
#include "Entity.h"
#include "Actor.h"
#include "level.h"
#include "xr_level_controller.h"
#include "gamemtllib.h"
#include "level_bullet_manager.h"
#include "ai_sounds.h"
#include "game_cl_single.h"
#include "game_object_space.h"

#define KNIFE_MATERIAL_NAME "objects\\knife"

CWeaponKnife::CWeaponKnife() : CWeapon("KNIFE") 
{
	m_attackStart			= false;
	SetState				( eHidden );
	SetNextState			( eHidden );
	knife_material_idx		= (u16)-1;
}
CWeaponKnife::~CWeaponKnife()
{
	HUD_SOUND::DestroySound(m_sndShot);

}

void CWeaponKnife::Load	(LPCSTR section)
{
	// verify class
	inherited::Load		(section);

	fWallmarkSize = pSettings->r_float(section,"wm_size");

	// HUD :: Anims
	R_ASSERT			(m_pHUD);
	animGetEx( mhud_idle,        "anim_idle" );
	animGetEx( mhud_idle_moving, pSettings->line_exist( hud_sect.c_str(), "anim_idle_moving" ) ? "anim_idle_moving" : "anim_idle" );
	animGetEx( mhud_idle_sprint, pSettings->line_exist( hud_sect.c_str(), "anim_idle_sprint" ) ? "anim_idle_sprint" : "anim_idle" );
	animGetEx( mhud_hide,        "anim_hide" );
	animGetEx( mhud_show,        "anim_draw" );
	animGetEx( mhud_attack,      "anim_shoot1_start" );
	animGetEx( mhud_attack2,     "anim_shoot2_start" );
	animGetEx( mhud_attack_e,    "anim_shoot1_end" );
	animGetEx( mhud_attack2_e,   "anim_shoot2_end" );

	HUD_SOUND::LoadSound(section,"snd_shoot"		, m_sndShot		, ESoundTypes(SOUND_TYPE_WEAPON_SHOOTING)		);
	
	knife_material_idx =  GMLib.GetMaterialIdx(KNIFE_MATERIAL_NAME);
}

void CWeaponKnife::OnStateSwitch	(u32 S)
{
	inherited::OnStateSwitch(S);
	switch (S)
	{
	case eIdle:
		switch2_Idle	();
		break;
	case eShowing:
		switch2_Showing	();
		break;
	case eHiding:
		switch2_Hiding	();
		break;
	case eHidden:
		switch2_Hidden	();
		break;
	case eFire:
	case eFire2:
		{
			switch2_Attacking	(S);
		}break;
	}
}
	

void CWeaponKnife::KnifeStrike(u32 state, const Fvector& pos, const Fvector& dir)
{
	ALife::EHitType		cur_eHitType = ALife::eHitTypeBurn;
	float				cur_fHitImpulse = 0;
	float				cur_fHit = 0;
	bool apply = false;

	switch (state)
	{
	case eFire:
	{
		//-------------------------------------------
		cur_eHitType = m_eHitType_1;
		//fHitPower		= fHitPower_1;
		if (ParentIsActor())
		{
			cur_fHit = fvHitPower_1[g_SingleGameDifficulty];
		}
		else
		{
			cur_fHit = fvHitPower_1[egdMaster];
		}
		cur_fHitImpulse = fHitImpulse_1;
		apply = true;
		//-------------------------------------------
	}break;
	case eFire2:
	{
		//-------------------------------------------
		cur_eHitType = m_eHitType_2;
		//fHitPower		= fHitPower_2;
		if (ParentIsActor())
		{
			cur_fHit = fvHitPower_2[g_SingleGameDifficulty];
		}
		else
		{
			cur_fHit = fvHitPower_2[egdMaster];
		}
		cur_fHitImpulse = fHitImpulse_2;
		apply = true;
		//-------------------------------------------
	}break;
	}	

	if (apply)
	{
		CCartridge						cartridge;
		cartridge.m_buckShot = 1;
		cartridge.m_impair = 1;
		cartridge.m_kDisp = 1;
		cartridge.m_kHit = 1;
		cartridge.m_kImpulse = 1;
		cartridge.m_kPierce = 1;
		cartridge.m_flags.set(CCartridge::cfTracer, FALSE);
		cartridge.m_flags.set(CCartridge::cfRicochet, FALSE);
		cartridge.fWallmarkSize = fWallmarkSize;
		cartridge.bullet_material_idx = knife_material_idx;

		while (m_magazine.size() < 2)	
			m_magazine.push_back(cartridge);

		iAmmoElapsed = m_magazine.size();

		const bool send_hit = SendHitAllowed(H_Parent());

		PlaySound(m_sndShot, pos);

		Level().BulletManager().AddBullet(pos,
			dir,
			m_fStartBulletSpeed,
			cur_fHit,
			cur_fHitImpulse,
			H_Parent()->ID(),
			ID(),
			cur_eHitType,
			fireDistance,
			cartridge,
			send_hit);
	}
}


void CWeaponKnife::OnAnimationEnd(u32 state)
{
	switch (state)
	{
	case eHiding:	SwitchState(eHidden);	break;
	case eFire: 
	case eFire2: 
		{
            if(m_attackStart) 
			{
				m_attackStart = false;
				if(GetState()==eFire)
					m_pHUD->animPlay(random_anim(mhud_attack_e), TRUE, this, GetState());
				else
					m_pHUD->animPlay(random_anim(mhud_attack2_e), TRUE, this, GetState());

				Fvector	p1, d; 
				p1.set(get_LastFP()); 
				d.set(get_LastFD());

				if(H_Parent()) 
					smart_cast<CEntity*>(H_Parent())->g_fireParams(this, p1,d);
				else 
					break;

				KnifeStrike(state, p1,d);
			} 
			else 
				SwitchState(eIdle);
		}break;
	case eShowing:
	case eIdle:	
		SwitchState(eIdle);
		break;	
	}
}

void CWeaponKnife::state_Attacking	(float)
{
}

void CWeaponKnife::switch2_Attacking	(u32 state)
{
	if(m_bPending)	return;

	m_attackStart = true;
	m_bPending = true;

	if(state==eFire)
		m_pHUD->animPlay(random_anim(mhud_attack),		FALSE, this, state);
	else //eFire2
		m_pHUD->animPlay(random_anim(mhud_attack2),		FALSE, this, state);

	StateSwitchCallback(GameObject::eOnActorWeaponFire, GameObject::eOnNPCWeaponFire);
}


void CWeaponKnife::switch2_Idle() {
  PlayAnimIdle( m_idle_state );
  m_bPending = false;
}


void CWeaponKnife::switch2_Hiding	()
{
	FireEnd					();
	VERIFY(GetState()==eHiding);
	m_pHUD->animPlay		(random_anim(mhud_hide), TRUE, this, GetState());
//	m_bPending				= true;
}

void CWeaponKnife::switch2_Hidden()
{
	signal_HideComplete		();
	m_bPending = false;
}

void CWeaponKnife::switch2_Showing	()
{
	VERIFY(GetState()==eShowing);
	m_pHUD->animPlay		(random_anim(mhud_show), FALSE, this, GetState());
//	m_bPending				= true;
}


void CWeaponKnife::FireStart()
{	
	inherited::FireStart();
	SwitchState			(eFire);
}

void CWeaponKnife::Fire2Start () 
{
	inherited::Fire2Start();
	SwitchState(eFire2);

	if (ParentIsActor())
	{
		Actor()->set_state_wishful(Actor()->get_state_wishful() & (~mcSprint));
	}
}


bool CWeaponKnife::Action(s32 cmd, u32 flags) 
{
	if(inherited::Action(cmd, flags)) return true;
	switch(cmd) 
	{
		case kWPN_ZOOM : 
			if(flags&CMD_START) Fire2Start();
			else Fire2End();
			return true;
	}
	return false;
}

void CWeaponKnife::LoadFireParams(LPCSTR section, LPCSTR prefix)
{
	inherited::LoadFireParams(section, prefix);

	string256			full_name;
	string32			buffer;
	shared_str			s_sHitPower_2;
	//fHitPower_1		= fHitPower;
	fvHitPower_1		= fvHitPower;
	fHitImpulse_1		= fHitImpulse;
	m_eHitType_1		= ALife::g_tfString2HitType(pSettings->r_string(section, "hit_type"));

	//fHitPower_2			= pSettings->r_float	(section,strconcat(full_name, prefix, "hit_power_2"));
	s_sHitPower_2		= pSettings->r_string_wb	(section,strconcat(sizeof(full_name),full_name, prefix, "hit_power_2"));
	fvHitPower_2[egdMaster]	= (float)atof(_GetItem(*s_sHitPower_2,0,buffer));//первый параметр - это хит для уровня игры мастер

	fvHitPower_2[egdVeteran]	= fvHitPower_2[egdMaster];//изначально параметры для других уровней
	fvHitPower_2[egdStalker]	= fvHitPower_2[egdMaster];//сложности
	fvHitPower_2[egdNovice]		= fvHitPower_2[egdMaster];//такие же

	int num_game_diff_param=_GetItemCount(*s_sHitPower_2);//узнаём колличество параметров для хитов
	if (num_game_diff_param>1)//если задан второй параметр хита
	{
		fvHitPower_2[egdVeteran]	= (float)atof(_GetItem(*s_sHitPower_2,1,buffer));//то вычитываем его для уровня ветерана
	}
	if (num_game_diff_param>2)//если задан третий параметр хита
	{
		fvHitPower_2[egdStalker]	= (float)atof(_GetItem(*s_sHitPower_2,2,buffer));//то вычитываем его для уровня сталкера
	}
	if (num_game_diff_param>3)//если задан четвёртый параметр хита
	{
		fvHitPower_2[egdNovice]	= (float)atof(_GetItem(*s_sHitPower_2,3,buffer));//то вычитываем его для уровня новичка
	}

	fHitImpulse_2		= pSettings->r_float	(section,strconcat(sizeof(full_name),full_name, prefix, "hit_impulse_2"));
	m_eHitType_2		= ALife::g_tfString2HitType(pSettings->r_string(section, "hit_type_2"));
}

void CWeaponKnife::StartIdleAnim()
{
	m_pHUD->animDisplay(mhud_idle[Random.randI(mhud_idle.size())], TRUE);
}
void CWeaponKnife::GetBriefInfo(xr_string& str_name, xr_string& icon_sect_name, xr_string& str_count)
{
	str_name		= NameShort();
	str_count		= "";
	icon_sect_name	= *cNameSect();
}


void CWeaponKnife::PlayAnimIdle( u8 state ) {
  VERIFY( GetState() == eIdle );

  switch ( state ) {
  case eSubstateIdleMoving:
    m_pHUD->animPlay( random_anim( mhud_idle_moving ), TRUE, this, GetState() );
    break;
  case eSubstateIdleSprint:
    m_pHUD->animPlay( random_anim( mhud_idle_sprint ), TRUE, this, GetState() );
    break;
  default:
    m_pHUD->animPlay( random_anim( mhud_idle ), TRUE, this, GetState() );
  }
}


void CWeaponKnife::onMovementChanged( ACTOR_DEFS::EMoveCommand cmd ) {
  if ( cmd == ACTOR_DEFS::mcSprint && GetState() == eIdle ) {
    m_idle_state = eSubstateIdleSprint;
    PlayAnimIdle( m_idle_state );
  }
}

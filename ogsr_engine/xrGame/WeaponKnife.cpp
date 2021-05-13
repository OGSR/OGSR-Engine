#include "stdafx.h"
#include "WeaponKnife.h"
#include "Entity.h"
#include "Actor.h"
#include "level.h"
#include "xr_level_controller.h"
#include "../xr_3da/gamemtllib.h"
#include "level_bullet_manager.h"
#include "ai_sounds.h"
#include "game_cl_single.h"
#include "game_object_space.h"
#include "Level_Bullet_Manager.h"

#define KNIFE_MATERIAL_NAME "objects\\knife"

CWeaponKnife::CWeaponKnife() : CWeapon("KNIFE") 
{
	m_attackStart			= false;
	m_attackMotionMarksAvailable = false;
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

	HUD_SOUND::LoadSound(section,"snd_shoot"		, m_sndShot		, ESoundTypes(SOUND_TYPE_WEAPON_SHOOTING)		);
	
	knife_material_idx =  GMLib.GetMaterialIdx(KNIFE_MATERIAL_NAME);
}

void CWeaponKnife::OnStateSwitch(u32 S, u32 oldState)
{
	inherited::OnStateSwitch(S, oldState);
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

		if ( ParentIsActor() && !fis_zero( conditionDecreasePerShotOnHit ) && GetCondition() < 0.95f )
                  cur_fHit = cur_fHit * ( GetCondition() / 0.95f );
		SBullet& bullet = Level().BulletManager().AddBullet(pos,
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
		if ( ParentIsActor() )
		  bullet.setOnBulletHit( true );
	}
}

void CWeaponKnife::OnMotionMark(u32 state, const motion_marks& M)
{
	inherited::OnMotionMark(state, M);

	if (H_Parent())
	{
		Fvector p1, d;
		p1.set(get_LastFP());
		d.set(get_LastFD());
		smart_cast<CEntity*>(H_Parent())->g_fireParams(this, p1, d);
		KnifeStrike(state, p1, d);
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
			u32 time = 0;
            if (m_attackStart) 
			{
				m_attackStart = false;
				if (GetState() == eFire)
					time = PlayHUDMotion("anim_shoot1_end", "anm_attack_end", FALSE, this, state);
				else // eFire2
					time = PlayHUDMotion("anim_shoot2_end", "anm_attack2_end", FALSE, this, state);

				Fvector	p1, d; 
				p1.set(get_LastFP()); 
				d.set(get_LastFD());

				if (H_Parent()) 
					smart_cast<CEntity*>(H_Parent())->g_fireParams(this, p1,d);
				else 
					break;

				if (time != 0 && !m_attackMotionMarksAvailable)
					KnifeStrike(state, p1,d);
			} 
			if (time == 0)
			{
				SwitchState(eIdle);
			}
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
	if (IsPending())
		return;

	if (state == eFire)
		PlayHUDMotion("anim_shoot1_start", "anm_attack", FALSE, this, state);
	else // eFire2
		PlayHUDMotion("anim_shoot2_start", "anm_attack2", FALSE, this, state);

	m_attackMotionMarksAvailable = !m_current_motion_def->marks.empty();
	m_attackStart = true;
	SetPending(TRUE);

	StateSwitchCallback(GameObject::eOnActorWeaponFire, GameObject::eOnNPCWeaponFire);
}

void CWeaponKnife::switch2_Idle()
{
	PlayAnimIdle();
	SetPending(FALSE);
}

void CWeaponKnife::switch2_Hiding()
{
	FireEnd();
	VERIFY(GetState() == eHiding);
	PlayHUDMotion("anim_hide", "anm_hide", TRUE, this, GetState());
}

void CWeaponKnife::switch2_Hidden()
{
	signal_HideComplete();
	SetPending(FALSE);
}

void CWeaponKnife::switch2_Showing()
{
	VERIFY(GetState() == eShowing);
	PlayHUDMotion("anim_draw", "anm_show", FALSE, this, GetState());
}

void CWeaponKnife::FireStart()
{
	inherited::FireStart();
	SwitchState(eFire);
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

void CWeaponKnife::GetBriefInfo(xr_string& str_name, xr_string& icon_sect_name, xr_string& str_count)
{
	str_name		= NameShort();
	str_count		= "";
	icon_sect_name	= *cNameSect();
}

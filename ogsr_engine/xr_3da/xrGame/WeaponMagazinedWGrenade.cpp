#include "stdafx.h"
#include "weaponmagazinedwgrenade.h"
#include "WeaponHUD.h"
#include "HUDManager.h"
#include "entity.h"
#include "ParticlesObject.h"
#include "GrenadeLauncher.h"
#include "xrserver_objects_alife_items.h"
#include "ExplosiveRocket.h"
#include "Actor_Flags.h"
#include "xr_level_controller.h"
#include "level.h"
#include "../skeletoncustom.h"
#include "object_broker.h"
#include "game_base_space.h"
#include "MathUtils.h"
#include "clsid_game.h"
#ifdef DEBUG
#include "phdebug.h"
#endif

#include "pch_script.h"
#include "game_object_space.h"
#include "script_callback_ex.h"
#include "script_game_object.h"

CWeaponMagazinedWGrenade::CWeaponMagazinedWGrenade(LPCSTR name,ESoundTypes eSoundType) : CWeaponMagazined(name, eSoundType)
{
	m_ammoType2 = 0;
    m_bGrenadeMode = false;
}

CWeaponMagazinedWGrenade::~CWeaponMagazinedWGrenade(void)
{
	// sounds
	HUD_SOUND::DestroySound(sndShotG);
	HUD_SOUND::DestroySound(sndReloadG);
	HUD_SOUND::DestroySound(sndSwitch);
}

void CWeaponMagazinedWGrenade::StopHUDSounds		()
{
	HUD_SOUND::StopSound(sndShotG);
	HUD_SOUND::StopSound(sndReloadG);
	HUD_SOUND::StopSound(sndSwitch);

	inherited::StopHUDSounds();
}

void CWeaponMagazinedWGrenade::Load	(LPCSTR section)
{
	inherited::Load			(section);
	CRocketLauncher::Load	(section);
	
	
	//// Sounds
	HUD_SOUND::LoadSound(section,"snd_shoot_grenade"	, sndShotG		, m_eSoundShot);
	HUD_SOUND::LoadSound(section,"snd_reload_grenade"	, sndReloadG	, m_eSoundReload);
	HUD_SOUND::LoadSound(section,"snd_switch"			, sndSwitch		, m_eSoundReload);
	

	m_sFlameParticles2 = pSettings->r_string(section, "grenade_flame_particles");

	
	// HUD :: Anims
	R_ASSERT			(m_pHUD);

	animGet				(mhud_idle_g,	pSettings->r_string(*hud_sect, "anim_idle_g"));
	animGet				(mhud_reload_g,	pSettings->r_string(*hud_sect, "anim_reload_g"));
	animGet				(mhud_shots_g,	pSettings->r_string(*hud_sect, "anim_shoot_g"));
	animGet				(mhud_switch_g,	pSettings->r_string(*hud_sect, "anim_switch_grenade_on"));
	animGet				(mhud_switch,	pSettings->r_string(*hud_sect, "anim_switch_grenade_off"));
	animGet				(mhud_show_g,	pSettings->r_string(*hud_sect, "anim_draw_g"));
	animGet				(mhud_hide_g,	pSettings->r_string(*hud_sect, "anim_holster_g"));

	animGet				(mhud_idle_w_gl,	pSettings->r_string(*hud_sect, "anim_idle_gl"));
	animGet				(mhud_reload_w_gl,	pSettings->r_string(*hud_sect, "anim_reload_gl"));
	animGet				(mhud_show_w_gl,	pSettings->r_string(*hud_sect, "anim_draw_gl"));
	animGet				(mhud_hide_w_gl,	pSettings->r_string(*hud_sect, "anim_holster_gl"));
	animGet				(mhud_shots_w_gl,	pSettings->r_string(*hud_sect, "anim_shoot_gl"));

	if(this->IsZoomEnabled())
	{
		animGet				(mhud_idle_g_aim,		pSettings->r_string(*hud_sect, "anim_idle_g_aim"));
		animGet				(mhud_idle_w_gl_aim,	pSettings->r_string(*hud_sect, "anim_idle_gl_aim"));
	}


	if(m_eGrenadeLauncherStatus == ALife::eAddonPermanent)
	{
		CRocketLauncher::m_fLaunchSpeed = pSettings->r_float(section, "grenade_vel");
	}

	grenade_bone_name = pSettings->r_string(*hud_sect, "grenade_bone");

	// load ammo classes SECOND (grenade_class)
	m_ammoTypes2.clear	(); 
	LPCSTR				S = pSettings->r_string(section,"grenade_class");
	if (S && S[0]) 
	{
		string128		_ammoItem;
		int				count		= _GetItemCount	(S);
		for (int it=0; it<count; ++it)	
		{
			_GetItem				(S,it,_ammoItem);
			m_ammoTypes2.push_back	(_ammoItem);
		}
		m_ammoName2 = pSettings->r_string(*m_ammoTypes2[0],"inv_name_short");
	}
	else
		m_ammoName2 = 0;

	iMagazineSize2 = iMagazineSize;
}

void CWeaponMagazinedWGrenade::net_Destroy()
{
	inherited::net_Destroy();
}

void CWeaponMagazinedWGrenade::net_Relcase(CObject *object)
{
	inherited::net_Relcase(object);
}

void CWeaponMagazinedWGrenade::OnDrawUI()
{
	inherited::OnDrawUI();
}


BOOL CWeaponMagazinedWGrenade::net_Spawn(CSE_Abstract* DC) 
{
	BOOL l_res = inherited::net_Spawn(DC);
	 
	UpdateGrenadeVisibility(!!iAmmoElapsed);
	m_bPending = false;

	m_DefaultCartridge2.Load(*m_ammoTypes2[m_ammoType2], u8(m_ammoType2));

	if (GameID() != GAME_SINGLE)
	{
		if (!m_bGrenadeMode && IsGrenadeLauncherAttached() && !getRocketCount())
		{
			m_magazine2.push_back(m_DefaultCartridge2);

			shared_str grenade_name = m_DefaultCartridge2.m_ammoSect;
			shared_str fake_grenade_name = pSettings->r_string(grenade_name, "fake_grenade_name");

			CRocketLauncher::SpawnRocket(*fake_grenade_name, this);
		}
	};
	
	xr_vector<CCartridge>* pM = NULL;
	bool b_if_grenade_mode	= (m_bGrenadeMode && iAmmoElapsed && !getRocketCount());
	if(b_if_grenade_mode)
		pM = &m_magazine;
		
	bool b_if_simple_mode	= (!m_bGrenadeMode && m_magazine2.size() && !getRocketCount());
	if(b_if_simple_mode)
		pM = &m_magazine2;

	if(b_if_grenade_mode || b_if_simple_mode) 
	{
		shared_str fake_grenade_name = pSettings->r_string(pM->back().m_ammoSect, "fake_grenade_name");
		
		CRocketLauncher::SpawnRocket(*fake_grenade_name, this);
	}

	return l_res;
}

void CWeaponMagazinedWGrenade::switch2_Idle() 
{
	inherited::switch2_Idle();
}

void CWeaponMagazinedWGrenade::switch2_Reload()
{
	VERIFY(GetState()==eReload);
	if(m_bGrenadeMode) 
	{
		PlaySound(sndReloadG,get_LastFP2());

		m_pHUD->animPlay(random_anim(mhud_reload_g),FALSE,this,GetState());
		m_bPending = true;
	}
	else 
	     inherited::switch2_Reload();
}

void CWeaponMagazinedWGrenade::OnShot		()
{
	if(m_bGrenadeMode)
	{
		PlaySound(sndShotG, get_LastFP2());
		
		AddShotEffector		();
		
		//партиклы огня вылета гранаты из подствольника
		StartFlameParticles2();
	} 
	else inherited::OnShot();
}
//переход в режим подствольника или выход из него
//если мы в режиме стрельбы очередями, переключиться
//на одиночные, а уже потом на подствольник
bool CWeaponMagazinedWGrenade::SwitchMode() 
{
	bool bUsefulStateToSwitch = ((eIdle==GetState())||(eHidden==GetState())||(eMisfire==GetState())||(eMagEmpty==GetState())) && (!IsPending());

	if(!bUsefulStateToSwitch)
		return false;

	if(!IsGrenadeLauncherAttached()) 
		return false;

	m_bPending				= true;

	PerformSwitchGL			();
	
	PlaySound				(sndSwitch,get_LastFP());

	PlayAnimModeSwitch		();

	m_dwAmmoCurrentCalcFrame = 0;

	return					true;
}

void  CWeaponMagazinedWGrenade::PerformSwitchGL()
{
	m_bGrenadeMode		= !m_bGrenadeMode;

	iMagazineSize		= m_bGrenadeMode?1:iMagazineSize2;

	m_ammoTypes.swap	(m_ammoTypes2);

	swap				(m_ammoType,m_ammoType2);
	swap				(m_ammoName,m_ammoName2);
	
	swap				(m_DefaultCartridge, m_DefaultCartridge2);

	xr_vector<CCartridge> l_magazine;
	while(m_magazine.size()) { l_magazine.push_back(m_magazine.back()); m_magazine.pop_back(); }
	while(m_magazine2.size()) { m_magazine.push_back(m_magazine2.back()); m_magazine2.pop_back(); }
	while(l_magazine.size()) { m_magazine2.push_back(l_magazine.back()); l_magazine.pop_back(); }
	iAmmoElapsed = (int)m_magazine.size();

	if(m_bZoomEnabled && m_pHUD)
	{
		if(m_bGrenadeMode)
			LoadZoomOffset(*hud_sect, "grenade_");
		else 
		{
			if(GrenadeLauncherAttachable())
				LoadZoomOffset(*hud_sect, "grenade_normal_");
			else
				LoadZoomOffset(*hud_sect, "");
		}
	}
}

bool CWeaponMagazinedWGrenade::Action(s32 cmd, u32 flags) 
{
	if(inherited::Action(cmd, flags)) return true;
	
	switch(cmd) 
	{
	case kWPN_ZOOM: 
	case kWPN_FUNC: 
			{
                if(flags&CMD_START) 
					SwitchState(eSwitch);
				return true;
			}
	}
	return false;
}

#include "inventory.h"
#include "inventoryOwner.h"
void CWeaponMagazinedWGrenade::state_Fire(float dt) 
{
	VERIFY(fTimeToFire>0.f);

	//режим стрельбы подствольника
	if(m_bGrenadeMode)
	{
		fTime					-=dt;
		Fvector					p1, d; 
		p1.set	(get_LastFP2()); 
		d.set	(get_LastFD());
		
		if(H_Parent())
		{ 
			CInventoryOwner* io		= smart_cast<CInventoryOwner*>(H_Parent());
			if(NULL == io->inventory().ActiveItem())
			{
			Log("current_state", GetState() );
			Log("next_state", GetNextState());
			Log("state_time", m_dwStateTime);
			Log("item_sect", cNameSect().c_str());
			Log("H_Parent", H_Parent()->cNameSect().c_str());
			}

			smart_cast<CEntity*>	(H_Parent())->g_fireParams	(this, p1,d);
		}else 
			return;
		
		while (fTime<=0 && (iAmmoElapsed>0) && (IsWorking() || m_bFireSingleShot))
		{

			fTime			+=	fTimeToFire;

			++m_iShotNum;
			OnShot			();
			
			// Ammo
			if(Local()) 
			{
				VERIFY(m_magazine.size());
				m_magazine.pop_back	();
				--iAmmoElapsed;
			
				VERIFY((u32)iAmmoElapsed == m_magazine.size());

				if(!iAmmoElapsed) 
					OnMagazineEmpty();
			}
		}
		UpdateSounds			();
		if(m_iShotNum == m_iQueueSize) FireEnd();
	} 
	//режим стрельбы очередями
	else inherited::state_Fire(dt);
}

void CWeaponMagazinedWGrenade::SwitchState(u32 S) 
{
	inherited::SwitchState(S);
	
	//стрельнуть из подствольника
	if(m_bGrenadeMode && GetState() == eIdle && S == eFire && getRocketCount() ) 
	{
		Fvector						p1, d; 
		p1.set						(get_LastFP2());
		d.set						(get_LastFD());
		CEntity*					E = smart_cast<CEntity*>(H_Parent());

		if (E){
			CInventoryOwner* io		= smart_cast<CInventoryOwner*>(H_Parent());
			if(NULL == io->inventory().ActiveItem())
			{
			Log("current_state", GetState() );
			Log("next_state", GetNextState());
			Log("state_time", m_dwStateTime);
			Log("item_sect", cNameSect().c_str());
			Log("H_Parent", H_Parent()->cNameSect().c_str());
			}
			E->g_fireParams		(this, p1,d);
		}
		if (IsGameTypeSingle())
			p1.set						(get_LastFP2());
		
		Fmatrix launch_matrix;
		launch_matrix.identity();
		launch_matrix.k.set(d);
		Fvector::generate_orthonormal_basis(launch_matrix.k,
											launch_matrix.j, launch_matrix.i);
		launch_matrix.c.set(p1);

		if (IsZoomed() && H_Parent()->CLS_ID == CLSID_OBJECT_ACTOR)
		{
			H_Parent()->setEnabled(FALSE);
			setEnabled(FALSE);

			collide::rq_result RQ;
			BOOL HasPick = Level().ObjectSpace.RayPick(p1, d, 300.0f, collide::rqtStatic, RQ, this);

			setEnabled(TRUE);
			H_Parent()->setEnabled(TRUE);

			if (HasPick)
			{
				Fvector Transference;
				Transference.mul(d, RQ.range);
				Fvector res[2];
#ifdef		DEBUG
//.				DBG_OpenCashedDraw();
//.				DBG_DrawLine(p1,Fvector().add(p1,d),D3DCOLOR_XRGB(255,0,0));
#endif
				u8 canfire0 = TransferenceAndThrowVelToThrowDir(Transference, CRocketLauncher::m_fLaunchSpeed, EffectiveGravity(), res);
#ifdef DEBUG
//.				if(canfire0>0)DBG_DrawLine(p1,Fvector().add(p1,res[0]),D3DCOLOR_XRGB(0,255,0));
//.				if(canfire0>1)DBG_DrawLine(p1,Fvector().add(p1,res[1]),D3DCOLOR_XRGB(0,0,255));
//.				DBG_ClosedCashedDraw(30000);
#endif
				
				if (canfire0 != 0)
				{
					d = res[0];
				};
			}
		};
		
		d.normalize();
		d.mul(CRocketLauncher::m_fLaunchSpeed);
		VERIFY2(_valid(launch_matrix),"CWeaponMagazinedWGrenade::SwitchState. Invalid launch_matrix!");
		CRocketLauncher::LaunchRocket(launch_matrix, d, zero_vel);

		CExplosiveRocket* pGrenade = smart_cast<CExplosiveRocket*>(getCurrentRocket()/*m_pRocket*/);
		VERIFY(pGrenade);
		pGrenade->SetInitiator(H_Parent()->ID());

		
		if (Local() && OnServer())
		{
			NET_Packet P;
			u_EventGen(P,GE_LAUNCH_ROCKET,ID());
			P.w_u16(getCurrentRocket()->ID());
			u_EventSend(P);
		};

	}
}

void CWeaponMagazinedWGrenade::OnEvent(NET_Packet& P, u16 type) 
{
	inherited::OnEvent(P,type);
	u16 id;
	switch (type) 
	{
		case GE_OWNERSHIP_TAKE: 
			{
				P.r_u16(id);
				CRocketLauncher::AttachRocket(id, this);
			}
			break;
		case GE_OWNERSHIP_REJECT :
		case GE_LAUNCH_ROCKET : 
			{
				bool bLaunch = (type==GE_LAUNCH_ROCKET);
				P.r_u16(id);
				CRocketLauncher::DetachRocket(id, bLaunch);
				break;
			}
	}
}

void CWeaponMagazinedWGrenade::ReloadMagazine() 
{
	inherited::ReloadMagazine();

	//перезарядка подствольного гранатомета
	if(iAmmoElapsed && !getRocketCount() && m_bGrenadeMode) 
	{
//.		shared_str fake_grenade_name = pSettings->r_string(*m_pAmmo->cNameSect(), "fake_grenade_name");
		shared_str fake_grenade_name = pSettings->r_string(*m_ammoTypes[m_ammoType], "fake_grenade_name");
		
		CRocketLauncher::SpawnRocket(*fake_grenade_name, this);
	}
}


void CWeaponMagazinedWGrenade::OnStateSwitch(u32 S) 
{

	switch (S)
	{
	case eSwitch:
		{
			if( !SwitchMode() ){
				SwitchState(eIdle);
				return;
			}
		}break;
	}
	
	inherited::OnStateSwitch(S);
	UpdateGrenadeVisibility(!!iAmmoElapsed || S == eReload);
}


void CWeaponMagazinedWGrenade::OnAnimationEnd(u32 state)
{
	switch (state)
	{
	case eSwitch:
		{
			SwitchState(eIdle);
		}break;
	}
	inherited::OnAnimationEnd(state);
}


void CWeaponMagazinedWGrenade::OnH_B_Independent(bool just_before_destroy)
{
	inherited::OnH_B_Independent(just_before_destroy);

	m_bPending		= false;
	if (m_bGrenadeMode) {
		SetState		( eIdle );
//.		SwitchMode	();
		m_bPending	= false;
	}
}

bool CWeaponMagazinedWGrenade::CanAttach(PIItem pIItem)
{
	CGrenadeLauncher* pGrenadeLauncher = smart_cast<CGrenadeLauncher*>(pIItem);
	
	if(pGrenadeLauncher &&
	   CSE_ALifeItemWeapon::eAddonAttachable == m_eGrenadeLauncherStatus &&
	   0 == (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher) &&
	   !xr_strcmp(*m_sGrenadeLauncherName, pIItem->object().cNameSect()))
       return true;
	else
		return inherited::CanAttach(pIItem);
}

bool CWeaponMagazinedWGrenade::CanDetach(const char* item_section_name)
{
	if(CSE_ALifeItemWeapon::eAddonAttachable == m_eGrenadeLauncherStatus &&
	   0 != (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher) &&
	   !xr_strcmp(*m_sGrenadeLauncherName, item_section_name))
	   return true;
	else
	   return inherited::CanDetach(item_section_name);
}

bool CWeaponMagazinedWGrenade::Attach(PIItem pIItem, bool b_send_event)
{
	CGrenadeLauncher* pGrenadeLauncher = smart_cast<CGrenadeLauncher*>(pIItem);
	
	if(pGrenadeLauncher &&
	   CSE_ALifeItemWeapon::eAddonAttachable == m_eGrenadeLauncherStatus &&
	   0 == (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher) &&
	   !xr_strcmp(*m_sGrenadeLauncherName, pIItem->object().cNameSect()))
	{
		m_flagsAddOnState |= CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher;

		CRocketLauncher::m_fLaunchSpeed = pGrenadeLauncher->GetGrenadeVel();

 		//уничтожить подствольник из инвентаря
		if(b_send_event)
		{
//.			pIItem->Drop();
			if (OnServer()) 
				pIItem->object().DestroyObject	();
		}
		InitAddons				();
		UpdateAddonsVisibility	();
		return					true;
	}
	else
        return inherited::Attach(pIItem, b_send_event);
}

bool CWeaponMagazinedWGrenade::Detach(const char* item_section_name, bool b_spawn_item)
{
	if (CSE_ALifeItemWeapon::eAddonAttachable == m_eGrenadeLauncherStatus &&
	   0 != (m_flagsAddOnState&CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher) &&
	   !xr_strcmp(*m_sGrenadeLauncherName, item_section_name))
	{
		m_flagsAddOnState &= ~CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher;
		if(m_bGrenadeMode)
		{
			UnloadMagazine();
			PerformSwitchGL();
		}

		UpdateAddonsVisibility();
		return CInventoryItemObject::Detach(item_section_name, b_spawn_item);
	}
	else
		return inherited::Detach(item_section_name, b_spawn_item);
}




void CWeaponMagazinedWGrenade::InitAddons()
{	
	inherited::InitAddons();

	if(GrenadeLauncherAttachable())
	{
		if(IsGrenadeLauncherAttached())
		{
			CRocketLauncher::m_fLaunchSpeed = pSettings->r_float(*m_sGrenadeLauncherName,"grenade_vel");
		}

		if(m_bZoomEnabled && m_pHUD)
		{
			if(m_bGrenadeMode)
				LoadZoomOffset(*hud_sect, "grenade_");
			else 
			{
				if(IsGrenadeLauncherAttached())
					LoadZoomOffset(*hud_sect, "grenade_normal_");
				else
					LoadZoomOffset(*hud_sect, "");
			}
		}
	}

	callback(GameObject::eOnAddonInit)(2);
}

bool	CWeaponMagazinedWGrenade::UseScopeTexture()
{
	if ((GetAddonsState() & CSE_ALifeItemWeapon::eForcedNotexScope) != 0) return false;
	if (IsGrenadeLauncherAttached() && m_bGrenadeMode) return false;
	return true;
};
//float default_fov = 67.5f;
float	CWeaponMagazinedWGrenade::CurrentZoomFactor	()
{
	float zf = 1.0f;
	if (IsGrenadeLauncherAttached() && m_bGrenadeMode) 
		zf = m_fIronSightZoomFactor;
	else
		zf = inherited::CurrentZoomFactor();

//	zf /= (g_fov / default_fov);
	return zf;
}

//виртуальные функции для проигрывания анимации HUD
void CWeaponMagazinedWGrenade::PlayAnimShow()
{
	VERIFY(GetState()==eShowing);
	if(IsGrenadeLauncherAttached())
	{
		if(!m_bGrenadeMode)
			m_pHUD->animPlay(random_anim(mhud_show_w_gl),FALSE,this, GetState());
		else
			m_pHUD->animPlay(random_anim(mhud_show_g),FALSE,this, GetState());
	}	
	else
		m_pHUD->animPlay(random_anim(mhud.mhud_show),FALSE,this, GetState());
}

void CWeaponMagazinedWGrenade::PlayAnimHide()
{
	VERIFY(GetState()==eHiding);
	
	if(IsGrenadeLauncherAttached())
		m_pHUD->animPlay(random_anim(mhud_hide_w_gl),TRUE,this, GetState());
	else
		m_pHUD->animPlay (random_anim(mhud.mhud_hide),TRUE,this, GetState());
}

void CWeaponMagazinedWGrenade::PlayAnimReload()
{
	VERIFY(GetState()==eReload);

	if(IsGrenadeLauncherAttached())
		m_pHUD->animPlay(random_anim(mhud_reload_w_gl),TRUE,this, GetState());
	else
		inherited::PlayAnimReload();
}

void CWeaponMagazinedWGrenade::PlayAnimIdle()
{
	if(TryPlayAnimIdle())	return;
	VERIFY(GetState()==eIdle);
	if(IsGrenadeLauncherAttached())
	{
		if(m_bGrenadeMode)
		{
			if(IsZoomed())
				m_pHUD->animPlay(random_anim(mhud_idle_g_aim), FALSE, NULL, GetState());
			else
				m_pHUD->animPlay(random_anim(mhud_idle_g), FALSE, NULL, GetState());
		}
		else
		{
			if(IsZoomed())
				m_pHUD->animPlay(random_anim(mhud_idle_w_gl_aim), TRUE, NULL, GetState());
			else
				m_pHUD->animPlay(random_anim(mhud_idle_w_gl), TRUE, NULL, GetState());
				
		}
	}
	else
		inherited::PlayAnimIdle();
}
void CWeaponMagazinedWGrenade::PlayAnimShoot()
{
	VERIFY(GetState()==eFire || GetState()==eFire2);
	if(this->m_bGrenadeMode)
	{
		//анимация стрельбы из подствольника
		m_pHUD->animPlay(random_anim(mhud_shots_g),TRUE,this, GetState());
	}
	else
	{
		if(IsGrenadeLauncherAttached())
			m_pHUD->animPlay(random_anim(mhud_shots_w_gl),TRUE,this, GetState());
		else
			inherited::PlayAnimShoot();
	}
}

void  CWeaponMagazinedWGrenade::PlayAnimModeSwitch()
{
	if(m_bGrenadeMode)
		m_pHUD->animPlay(random_anim(mhud_switch_g), FALSE, this, eSwitch); //fake
	else 
		m_pHUD->animPlay(random_anim(mhud_switch), FALSE, this, eSwitch); //fake
}


void CWeaponMagazinedWGrenade::UpdateSounds	()
{
	inherited::UpdateSounds			();

	if (sndShotG.playing			())	sndShotG.set_position		(get_LastFP());
	if (sndReloadG.playing			())	sndReloadG.set_position		(get_LastFP());
	if (sndSwitch.playing			())	sndSwitch.set_position		(get_LastFP());
}

void CWeaponMagazinedWGrenade::UpdateGrenadeVisibility(bool visibility)
{
	if (H_Parent() != Level().CurrentEntity())	return;
	CKinematics* pHudVisual						= smart_cast<CKinematics*>(m_pHUD->Visual());
	VERIFY										(pHudVisual);
	pHudVisual->LL_SetBoneVisible				(pHudVisual->LL_BoneID(*grenade_bone_name),visibility,TRUE);
	pHudVisual->CalculateBones_Invalidate		();
	pHudVisual->CalculateBones					();
}

void CWeaponMagazinedWGrenade::save(NET_Packet &output_packet)
{
	inherited::save								(output_packet);
	save_data									(m_bGrenadeMode, output_packet);
	save_data									(m_magazine2.size(), output_packet);

}

void CWeaponMagazinedWGrenade::load(IReader &input_packet)
{
	inherited::load				(input_packet);
	bool b;
	load_data					(b, input_packet);
	if(b!=m_bGrenadeMode)		
		SwitchMode				();

	u32 sz;
	load_data					(sz, input_packet);

	CCartridge					l_cartridge; 
	l_cartridge.Load			(*m_ammoTypes2[m_ammoType2], u8(m_ammoType2));

	while (sz > m_magazine2.size())
		m_magazine2.push_back(l_cartridge);
}

void CWeaponMagazinedWGrenade::net_Export	(NET_Packet& P)
{
	P.w_u8						(m_bGrenadeMode ? 1 : 0);

	inherited::net_Export		(P);
}

void CWeaponMagazinedWGrenade::net_Import	(NET_Packet& P)
{
	bool NewMode				= FALSE;
	NewMode						= !!P.r_u8();	
	if (NewMode != m_bGrenadeMode)
		SwitchMode				();

	inherited::net_Import		(P);
}

bool CWeaponMagazinedWGrenade::IsNecessaryItem	    (const shared_str& item_sect)
{
	return (	std::find(m_ammoTypes.begin(), m_ammoTypes.end(), item_sect) != m_ammoTypes.end() ||
				std::find(m_ammoTypes2.begin(), m_ammoTypes2.end(), item_sect) != m_ammoTypes2.end() 
			);
}

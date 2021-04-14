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
#include "../Include/xrRender/Kinematics.h"
#include "object_broker.h"
#include "game_base_space.h"
#include "MathUtils.h"
#include "clsid_game.h"
#ifdef DEBUG
#include "phdebug.h"
#endif

#include "game_object_space.h"
#include "script_callback_ex.h"
#include "script_game_object.h"
#include "alife_registry_wrappers.h"
#include "alife_simulator_header.h"

CWeaponMagazinedWGrenade::CWeaponMagazinedWGrenade(LPCSTR name,ESoundTypes eSoundType) : CWeaponMagazined(name, eSoundType)
{
	m_ammoType2 = 0;
	iAmmoElapsed2  = 0;
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

	animGetEx( mhud_idle_g,        "anim_idle_g" );
	animGetEx( mhud_idle_moving_g, pSettings->line_exist( hud_sect.c_str(), "anim_idle_moving_g" ) ? "anim_idle_moving_g" : "anim_idle_g" );
	animGetEx( mhud_idle_sprint_g, pSettings->line_exist( hud_sect.c_str(), "anim_idle_sprint_g" ) ? "anim_idle_sprint_g" : pSettings->line_exist( hud_sect.c_str(), "anim_idle_sprint" ) ? "anim_idle_sprint" : "anim_idle_g" );
	animGetEx( mhud_reload_g,      "anim_reload_g" );
	animGetEx( mhud_shots_g,       "anim_shoot_g" );
	animGetEx( mhud_switch_g,      "anim_switch_grenade_on" );
	animGetEx( mhud_switch,        "anim_switch_grenade_off" );
	animGetEx( mhud_show_g,	       "anim_draw_g" );
	animGetEx( mhud_hide_g,        "anim_holster_g" );

	animGetEx( mhud_idle_w_gl,      "anim_idle_gl" );
	animGetEx( mhud_idle_moving_gl, pSettings->line_exist( hud_sect.c_str(), "anim_idle_moving_gl" ) ? "anim_idle_moving_gl" : "anim_idle_gl" );
	animGetEx( mhud_idle_sprint_gl, pSettings->line_exist( hud_sect.c_str(), "anim_idle_sprint_gl" ) ? "anim_idle_sprint_gl" : pSettings->line_exist( hud_sect.c_str(), "anim_idle_sprint" ) ? "anim_idle_sprint" : "anim_idle_gl" );
	animGetEx( mhud_reload_w_gl,    "anim_reload_gl" );
	animGetEx( mhud_show_w_gl,      "anim_draw_gl" );
	animGetEx( mhud_hide_w_gl,      "anim_holster_gl" );
	animGetEx( mhud_shots_w_gl,     "anim_shoot_gl" );

	if(this->IsZoomEnabled())
	{
		animGetEx( mhud_idle_g_aim,    "anim_idle_g_aim" );
		animGetEx( mhud_idle_w_gl_aim, "anim_idle_gl_aim" );
	}

	animGetEx( mhud_reload_w_gl_partly, "anim_reload_gl_partly", nullptr, "anim_reload_gl" );


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
	}

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

	const auto wgl = smart_cast<CSE_ALifeItemWeaponMagazinedWGL*>( DC );
	m_ammoType2   = m_ammoType2   > 0 ? m_ammoType2   : wgl->ammo_type2;
	iAmmoElapsed2 = iAmmoElapsed2 > 0 ? iAmmoElapsed2 : wgl->a_elapsed2;

	//Msg("~~[%s][%s] net_Spawn", __FUNCTION__, this->Name());

	if (wgl->m_bGrenadeMode) // m_bGrenadeMode enabled
	{
		m_bGrenadeMode = true;

		m_fZoomFactor = this->CurrentZoomFactor();

		iMagazineSize = 1;

		m_ammoTypes.swap(m_ammoTypes2);

		UpdateZoomOffset();

		StateSwitchCallback( GameObject::eOnActorWeaponSwitchGL, GameObject::eOnNPCWeaponSwitchGL );

		// reloading
		m_DefaultCartridge.Load(*m_ammoTypes[m_ammoType], u8(m_ammoType));
		u32 mag_sz = m_magazine.size();
		m_magazine.clear();
		while (mag_sz--) 
			m_magazine.push_back(m_DefaultCartridge);

		m_DefaultCartridge2.Load(*m_ammoTypes2[m_ammoType2], u8(m_ammoType2));
		//mag_sz = m_magazine2.size();
		m_magazine2.clear();
		while ((u32)iAmmoElapsed2 > m_magazine2.size()) //(mag_sz--)
			m_magazine2.push_back(m_DefaultCartridge2);
	}
	else
	{
		ASSERT_FMT( m_ammoType2 < m_ammoTypes2.size(), "Ammo type [%u] not found in weapon [%s]. Something strange...", m_ammoType2, this->cName().c_str() );

		m_DefaultCartridge2.Load(m_ammoTypes2.at(m_ammoType2).c_str(), u8(m_ammoType2));
		while ((u32)iAmmoElapsed2 > m_magazine2.size())
			m_magazine2.push_back(m_DefaultCartridge2);
	}

	if (!getRocketCount())
	{
		if (m_magazine.size() && pSettings->line_exist(m_magazine.back().m_ammoSect, "fake_grenade_name"))
		{
			shared_str fake_grenade_name = pSettings->r_string(m_magazine.back().m_ammoSect, "fake_grenade_name");
			CRocketLauncher::SpawnRocket(*fake_grenade_name, this);
		}
		else if (m_magazine2.size() && pSettings->line_exist(m_magazine2.back().m_ammoSect, "fake_grenade_name"))
		{
			shared_str fake_grenade_name = pSettings->r_string(m_magazine2.back().m_ammoSect, "fake_grenade_name");
			CRocketLauncher::SpawnRocket(*fake_grenade_name, this);
		}
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
		PlaySound( sndShotG, get_LastFP2(), true );
		
		AddShotEffector		();
		
		PlayAnimShoot();

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
	bool bUsefulStateToSwitch = ((eIdle==GetState())||(eHidden==GetState())||(eMisfire==GetState())||(eMagEmpty==GetState()));

	if(!bUsefulStateToSwitch)
		return false;

	if(!IsGrenadeLauncherAttached()) 
		return false;

	m_bPending				= true;

	PerformSwitchGL			();
	StateSwitchCallback( GameObject::eOnActorWeaponSwitchGL, GameObject::eOnNPCWeaponSwitchGL );
	
	PlaySound				(sndSwitch,get_LastFP());

	PlayAnimModeSwitch		();

	m_dwAmmoCurrentCalcFrame = 0;

	return					true;
}

void  CWeaponMagazinedWGrenade::PerformSwitchGL()
{
	m_bGrenadeMode		= !m_bGrenadeMode;

	m_fZoomFactor = this->CurrentZoomFactor();

	iMagazineSize		= m_bGrenadeMode?1:iMagazineSize2;

	m_ammoTypes.swap	(m_ammoTypes2);

	swap				(m_ammoType,m_ammoType2);
	
	swap				(m_DefaultCartridge, m_DefaultCartridge2);

	m_magazine.swap(m_magazine2); // https://github.com/revolucas/CoC-Xray/pull/5/commits/4a396eb30137c5625c5b0dd934e63eaa5b62cbc5

	iAmmoElapsed  = (int)m_magazine.size();
	iAmmoElapsed2 = (int)m_magazine2.size();

	UpdateZoomOffset();
}

bool CWeaponMagazinedWGrenade::Action(s32 cmd, u32 flags) 
{
	if (m_bGrenadeMode && (cmd == kWPN_FIREMODE_PREV || cmd == kWPN_FIREMODE_NEXT))
		return false;

	if(inherited::Action(cmd, flags))
		return true;
	
	switch(cmd) 
	{
	// case kWPN_ZOOM:  ??? 
	case kWPN_FUNC: 
	{
		if (!IsZoomed())
		{
			if (flags&CMD_START)
				SwitchState(eSwitch);
			return true;
		}
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
#ifdef DEBUG
			CInventoryOwner* io		= smart_cast<CInventoryOwner*>(H_Parent());
			if(NULL == io->inventory().ActiveItem())
			{
			Log("current_state", GetState() );
			Log("next_state", GetNextState());
			Log("state_time", m_dwStateTime);
			Log("item_sect", cNameSect().c_str());
			Log("H_Parent", H_Parent()->cNameSect().c_str());
			}
#endif

			smart_cast<CEntity*>	(H_Parent())->g_fireParams	(this, p1,d);
		}else 
			return;
		
		while (fTime<=0 && (iAmmoElapsed>0) && (IsWorking() || m_bFireSingleShot))
		{

			fTime			+=	fTimeToFire;

			++m_iShotNum;
			OnShot			();
			StateSwitchCallback(GameObject::eOnActorWeaponFire, GameObject::eOnNPCWeaponFire);
			
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
#ifdef DEBUG
			CInventoryOwner* io		= smart_cast<CInventoryOwner*>(H_Parent());
			if(NULL == io->inventory().ActiveItem())
			{
			Log("current_state", GetState() );
			Log("next_state", GetNextState());
			Log("state_time", m_dwStateTime);
			Log("item_sect", cNameSect().c_str());
			Log("H_Parent", H_Parent()->cNameSect().c_str());
			}
#endif
			E->g_fireParams		(this, p1,d);
		}

		p1.set(get_LastFP2());
		
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
		pGrenade->SetRealGrenadeName( m_ammoTypes[ m_ammoType ] );

		
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
		if (!IsPending())
		{
			if (!SwitchMode()) {
				SwitchState(eIdle);
				return;
			}
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
		// https://github.com/revolucas/CoC-Xray/pull/5/commits/9ca73da34a58ceb48713b1c67608198c6af26db2
		// Now we need to unload GL's magazine
		if (!m_bGrenadeMode)
		{
			PerformSwitchGL();
		}
		UnloadMagazine();
		PerformSwitchGL();

		m_flagsAddOnState &= ~CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher;

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
	}

	callback(GameObject::eOnAddonInit)(2);
}

bool CWeaponMagazinedWGrenade::UseScopeTexture()
{
	if (IsGrenadeLauncherAttached() && m_bGrenadeMode)
		return false;
	return inherited::UseScopeTexture();
}

float CWeaponMagazinedWGrenade::CurrentZoomFactor()
{
	if (IsGrenadeLauncherAttached() && m_bGrenadeMode) 
		return m_fIronSightZoomFactor;
	else
		return inherited::CurrentZoomFactor();
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
	
	if (IsGrenadeLauncherAttached())
	{
		if (!m_bGrenadeMode)
			m_pHUD->animPlay(random_anim(mhud_hide_w_gl), TRUE, this, GetState());
		else
			m_pHUD->animPlay(random_anim(mhud_hide_g), TRUE, this, GetState());
	}
	else
		m_pHUD->animPlay(random_anim(mhud.mhud_hide), TRUE, this, GetState());
}

void CWeaponMagazinedWGrenade::PlayAnimReload() {
  VERIFY( GetState() == eReload );

  if ( IsGrenadeLauncherAttached() ) {
    if ( IsPartlyReloading() )
      m_pHUD->animPlay( random_anim( mhud_reload_w_gl_partly ),TRUE, this, GetState() );
    else
      m_pHUD->animPlay( random_anim( mhud_reload_w_gl ),TRUE, this, GetState() );
  }
  else
    inherited::PlayAnimReload();
}


bool CWeaponMagazinedWGrenade::TryPlayAnimIdle( u8 state = eIdle ) {
  VERIFY( GetState() == eIdle );
  if( IsGrenadeLauncherAttached() && !IsZoomed() ) {
    switch ( state ) {
    case eSubstateIdleMoving:
      m_pHUD->animPlay( random_anim( m_bGrenadeMode ? mhud_idle_moving_g : mhud_idle_moving_gl ), TRUE, NULL, GetState() );
      return true;
    case eSubstateIdleSprint:
      m_pHUD->animPlay( random_anim( m_bGrenadeMode ? mhud_idle_sprint_g : mhud_idle_sprint_gl ), TRUE, NULL, GetState() );
      return true;
    default:
      return false;
    }
  }
  return inherited::TryPlayAnimIdle( state );
}


void CWeaponMagazinedWGrenade::PlayAnimIdle( u8 state )
{
	if ( TryPlayAnimIdle( state ) ) return;
	VERIFY(GetState()==eIdle);
	if(IsGrenadeLauncherAttached())
	{
		if(m_bGrenadeMode)
		{
			if(IsZoomed())
				m_pHUD->animPlay(random_anim(mhud_idle_g_aim), TRUE, NULL, GetState());
			else
				m_pHUD->animPlay(random_anim(mhud_idle_g), TRUE, NULL, GetState());
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
		inherited::PlayAnimIdle( state );
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

	if (sndShotG.playing			())	sndShotG.set_position		(get_LastFP2());
	if (sndReloadG.playing			())	sndReloadG.set_position		(get_LastFP2());
	if (sndSwitch.playing			())	sndSwitch.set_position		(get_LastFP());
}

void CWeaponMagazinedWGrenade::UpdateGrenadeVisibility(bool visibility)
{
	if (H_Parent() != Level().CurrentEntity())	return;
	IKinematics* pHudVisual						= smart_cast<IKinematics*>(m_pHUD->Visual());
	VERIFY										(pHudVisual);
	pHudVisual->LL_SetBoneVisible				(pHudVisual->LL_BoneID(*grenade_bone_name),visibility,TRUE);
	pHudVisual->CalculateBones_Invalidate		();
	pHudVisual->CalculateBones					();
}

void CWeaponMagazinedWGrenade::save(NET_Packet &output_packet)
{
	inherited::save(output_packet);
	save_data(m_bGrenadeMode, output_packet);
	save_data(m_magazine2.size(), output_packet);
	save_data(m_ammoType2, output_packet);
	//Msg( "~~[%s][%s] saved: m_bGrenadeMode: [%d], m_magazine2.size(): [%u], m_ammoType2: [%u]", __FUNCTION__, this->Name(), m_bGrenadeMode, m_magazine2.size(), m_ammoType2 );
}

void CWeaponMagazinedWGrenade::load(IReader &input_packet)
{
	inherited::load(input_packet);

	load_data(m_bGrenadeMode, input_packet);
	load_data(iAmmoElapsed2, input_packet);
	if ( ai().get_alife()->header().version() >= 5 )
	  load_data( m_ammoType2, input_packet );
	//Msg( "~~[%s][%s] loaded: m_bGrenadeMode: [%d], iAmmoElapsed2: [%d], m_ammoType2: [%u]", __FUNCTION__, this->Name(), m_bGrenadeMode, iAmmoElapsed2, m_ammoType2 );
}

void CWeaponMagazinedWGrenade::net_Export(NET_Packet& P)
{
	//Msg( "~~[%s][%s] net_export: m_bGrenadeMode: [%d], iAmmoElapsed2: [%d], m_ammoType2: [%u]", __FUNCTION__, this->Name(), m_bGrenadeMode, m_magazine2.size(), m_ammoType2 );
	P.w_u8( m_bGrenadeMode ? 1 : 0 );

	inherited::net_Export(P);

	P.w_u8( (u8)m_ammoType2 );
	P.w_u16( (u16)m_magazine2.size() );
}

void CWeaponMagazinedWGrenade::net_Import(NET_Packet& P) //Этот и все подобные методы вообще не вызываются в в синглплеере, походу.
{
	u8 _data = P.r_u8();
	bool NewMode = !!(_data & 0x1);

	inherited::net_Import(P);

	m_ammoType2   = P.r_u8();
	iAmmoElapsed2 = P.r_u16();

	if (NewMode != m_bGrenadeMode)
		SwitchMode();
	//Msg( "~~[%s][%s] net_import: m_bGrenadeMode: [%d], iAmmoElapsed2: [%d], m_ammoType2: [%u]", __FUNCTION__, this->Name(), NewMode, iAmmoElapsed2, m_ammoType2 );
}

bool CWeaponMagazinedWGrenade::IsNecessaryItem	    (const shared_str& item_sect)
{
	return (	std::find(m_ammoTypes.begin(), m_ammoTypes.end(), item_sect) != m_ammoTypes.end() ||
				std::find(m_ammoTypes2.begin(), m_ammoTypes2.end(), item_sect) != m_ammoTypes2.end() 
			);
}


float CWeaponMagazinedWGrenade::Weight() const {
  return inherited::Weight() + GetMagazineWeight( m_magazine2 );
}

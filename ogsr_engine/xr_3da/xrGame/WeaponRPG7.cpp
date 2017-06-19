#include "stdafx.h"
#include "weaponrpg7.h"
#include "WeaponHUD.h"
#include "xrserver_objects_alife_items.h"
#include "explosiverocket.h"
#include "entity.h"
#include "level.h"
#include "../skeletoncustom.h"

CWeaponRPG7::CWeaponRPG7(void) : CWeaponCustomPistol("RPG7") 
{
}

CWeaponRPG7::~CWeaponRPG7(void) 
{
}

void CWeaponRPG7::Load	(LPCSTR section)
{
	inherited::Load			(section);
	CRocketLauncher::Load	(section);

	m_fScopeZoomFactor		= pSettings->r_float	(section,"max_zoom_factor");

	m_sGrenadeBoneName		= pSettings->r_string	(section,"grenade_bone");
	m_sHudGrenadeBoneName	= pSettings->r_string	(hud_sect,"grenade_bone");

	m_sRocketSection		= pSettings->r_string	(section,"rocket_class");
}


void CWeaponRPG7::UpdateMissileVisibility()
{
	bool vis_hud,vis_weap;
	vis_hud		= (!!iAmmoElapsed || GetState() == eReload);
	vis_weap	= !!iAmmoElapsed;

	CKinematics* pHudVisual = smart_cast<CKinematics*>(m_pHUD->Visual());
	VERIFY(pHudVisual);
	if (H_Parent() != Level().CurrentEntity()) pHudVisual = NULL;
	CKinematics* pWeaponVisual = smart_cast<CKinematics*>(Visual()); 
	VERIFY(pWeaponVisual);

	if (pHudVisual) pHudVisual->LL_SetBoneVisible(pHudVisual->LL_BoneID(*m_sHudGrenadeBoneName),vis_hud,TRUE);
	pWeaponVisual->LL_SetBoneVisible(pWeaponVisual->LL_BoneID(*m_sGrenadeBoneName),vis_weap,TRUE);
	pWeaponVisual->CalculateBones_Invalidate();
	pWeaponVisual->CalculateBones();
}


BOOL CWeaponRPG7::net_Spawn(CSE_Abstract* DC) 
{
	BOOL l_res = inherited::net_Spawn(DC);

	UpdateMissileVisibility();
	if(iAmmoElapsed && !getCurrentRocket())
	{
		CRocketLauncher::SpawnRocket(*m_sRocketSection, this);
	}

	return l_res;
}

void CWeaponRPG7::OnStateSwitch(u32 S) 
{
	inherited::OnStateSwitch(S);
	UpdateMissileVisibility();
}

void CWeaponRPG7::UnloadMagazine(bool spawn_ammo)
{
	inherited::UnloadMagazine	(spawn_ammo);
	UpdateMissileVisibility		();
}

void CWeaponRPG7::ReloadMagazine() 
{
	inherited::ReloadMagazine();

	if(iAmmoElapsed && !getRocketCount()) 
	{
		CRocketLauncher::SpawnRocket(*m_sRocketSection, this);
	}
}
void CWeaponRPG7::SwitchState(u32 S) 
{
	inherited::SwitchState(S);
}

void CWeaponRPG7::FireStart()
{
	inherited::FireStart();
}

#include "inventory.h"
#include "inventoryOwner.h"
void CWeaponRPG7::switch2_Fire	()
{
	m_iShotNum = 0;
	m_bFireSingleShot = true;
	bWorking = false;

	if(GetState() == eFire	&& getRocketCount()) 
	{
		Fvector p1, d; 
		p1.set								(get_LastFP()); 
		d.set								(get_LastFD());

		CEntity* E = smart_cast<CEntity*>	(H_Parent());
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
			E->g_fireParams				(this, p1,d);
		}

		Fmatrix								launch_matrix;
		launch_matrix.identity				();
		launch_matrix.k.set					(d);
		Fvector::generate_orthonormal_basis(launch_matrix.k,
											launch_matrix.j, launch_matrix.i);
		launch_matrix.c.set					(p1);

		d.normalize							();
		d.mul								(m_fLaunchSpeed);

		CRocketLauncher::LaunchRocket		(launch_matrix, d, zero_vel);

		CExplosiveRocket* pGrenade			= smart_cast<CExplosiveRocket*>(getCurrentRocket());
		VERIFY								(pGrenade);
		pGrenade->SetInitiator				(H_Parent()->ID());

		if (OnServer())
		{
			NET_Packet						P;
			u_EventGen						(P,GE_LAUNCH_ROCKET,ID());
			P.w_u16							(u16(getCurrentRocket()->ID()));
			u_EventSend						(P);
		}
	}
}

void CWeaponRPG7::OnEvent(NET_Packet& P, u16 type) 
{
	inherited::OnEvent(P,type);
	u16 id;
	switch (type) {
		case GE_OWNERSHIP_TAKE : {
			P.r_u16(id);
			CRocketLauncher::AttachRocket(id, this);
		} break;
		case GE_OWNERSHIP_REJECT:
		case GE_LAUNCH_ROCKET	: 
			{
			bool bLaunch = (type==GE_LAUNCH_ROCKET);
			P.r_u16(id);
			CRocketLauncher::DetachRocket(id, bLaunch);
		} break;
	}
}

void CWeaponRPG7::net_Import( NET_Packet& P)
{
	inherited::net_Import		(P);
	UpdateMissileVisibility		();
}

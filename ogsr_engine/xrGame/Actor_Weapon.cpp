// Actor_Weapon.cpp:	 для работы с оружием
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "actor.h"
#include "actoreffector.h"
#include "Missile.h"
#include "inventory.h"
#include "weapon.h"
#include "map_manager.h"
#include "level.h"
#include "CharacterPhysicsSupport.h"
#include "EffectorShot.h"
#include "WeaponMagazined.h"
#include "Grenade.h"
#include "game_base_space.h"
#include "Artifact.h"

static const float VEL_MAX		= 10.f;
static const float VEL_A_MAX	= 10.f;

#define GetWeaponParam(pWeapon, func_name, def_value)	((pWeapon) ? (pWeapon->func_name) : def_value)

//возвращает текуший разброс стрельбы (в радианах)с учетом движения
float CActor::GetWeaponAccuracy() const
{
	CWeapon* W	= smart_cast<CWeapon*>(inventory().ActiveItem());
	

	if(m_bZoomAimingMode&&W&&!GetWeaponParam(W, IsRotatingToZoom(), false))
		return m_fDispAim;

	float dispersion = m_fDispBase*GetWeaponParam(W, Get_PDM_Base(), 1.0f);

	CEntity::SEntityState state;
	if (g_State(state))
	{
		// angular factor
		dispersion *= (1.f + (state.fAVelocity/VEL_A_MAX)*m_fDispVelFactor*GetWeaponParam(W, Get_PDM_Vel_F(), 1.0f));
//		Msg("--- base=[%f] angular disp=[%f]",m_fDispBase, dispersion);
		// linear movement factor
		bool bAccelerated = isActorAccelerated(mstate_real, IsZoomAimingMode());
		if( bAccelerated )
			dispersion *= (1.f + (state.fVelocity/VEL_MAX)*m_fDispVelFactor*GetWeaponParam(W, Get_PDM_Vel_F(), 1.0f)*(1.f + m_fDispAccelFactor*GetWeaponParam(W, Get_PDM_Accel_F(), 1.0f)));
		else
			dispersion *= (1.f + (state.fVelocity/VEL_MAX)*m_fDispVelFactor*GetWeaponParam(W, Get_PDM_Vel_F(), 1.0f));

		if (state.bCrouch){	
			dispersion *= (1.f + m_fDispCrouchFactor*GetWeaponParam(W, Get_PDM_Crouch(), 1.0f));

			if(!bAccelerated )
				dispersion *= (1.f + m_fDispCrouchNoAccelFactor*GetWeaponParam(W, Get_PDM_Crouch_NA(), 1.0f));
		}
	}

	return dispersion;
}


void CActor::g_fireParams	(const CHudItem* pHudItem, Fvector &fire_pos, Fvector &fire_dir)
{
//	VERIFY			(inventory().ActiveItem());

	fire_pos		= Cameras().Pos();
	fire_dir		= Cameras().Dir();

	const CMissile	*pMissile = smart_cast <const CMissile*> (pHudItem);
	if (pMissile)
	{
		Fvector offset;
		XFORM().transform_dir(offset, m_vMissileOffset);
		fire_pos.add(offset);
	}
}

void CActor::g_WeaponBones	(int &L, int &R1, int &R2)
{
	R1				= m_r_hand;
	R2				= m_r_finger2;
	L				= m_l_finger1;
}

BOOL CActor::g_State (SEntityState& state) const
{
	state.bJump			= !!(mstate_real&mcJump);
	state.bCrouch		= !!(mstate_real&mcCrouch);
	state.bFall			= !!(mstate_real&mcFall);
	state.bSprint		= !!(mstate_real&mcSprint);
	state.fVelocity		= character_physics_support()->movement()->GetVelocityActual();
	state.fAVelocity	= fCurAVelocity;
	return TRUE;
}

void CActor::SetWeaponHideState (u32 State, bool bSet)
{
	if (g_Alive() && this == Level().CurrentControlEntity())
	{
		NET_Packet	P;
		u_EventGen	(P, GEG_PLAYER_WEAPON_HIDE_STATE, ID());
		P.w_u32		(State);
		P.w_u8		(u8(bSet));
		u_EventSend	(P);
	};
}
static	u16 BestWeaponSlots [] = {
	SECOND_WEAPON_SLOT		,		// 2
	FIRST_WEAPON_SLOT		,		// 1
	GRENADE_SLOT	,		// 3
	KNIFE_SLOT		,		// 0
};
void CActor::SelectBestWeapon	(CObject* O)
{
	if (!O) return;
	if ( IsGameTypeSingle() ) return;
	if (OnClient()) return;
	//-------------------------------------------------
	CWeapon* pWeapon = smart_cast<CWeapon*>(O);
	CGrenade* pGrenade = smart_cast<CGrenade*>(O);
	CArtefact* pArtefact = smart_cast<CArtefact*>(O);
	CInventoryItem*	pIItem	= smart_cast<CInventoryItem*> (O);
	bool NeedToSelectBestWeapon = false;
	if ((pWeapon || pGrenade || pArtefact) && pIItem)
	{
		NeedToSelectBestWeapon = true;
		if (GameID() == GAME_ARTEFACTHUNT)
		{
			if (pIItem->GetSlot() == FIRST_WEAPON_SLOT || pIItem->GetSlot() == SECOND_WEAPON_SLOT)
			{
				CInventoryItem* pIItemInSlot = inventory().ItemFromSlot(pIItem->GetSlot());
				if (pIItemInSlot != NULL && pIItemInSlot != pIItem)				
					NeedToSelectBestWeapon = false;
			}
		}
	}
	if (!NeedToSelectBestWeapon) return;
	//-------------------------------------------------
	for (int i=0; i<4; i++)
	{
		if (inventory().m_slots[BestWeaponSlots[i]].m_pIItem)
		{
			if (inventory().GetActiveSlot() != BestWeaponSlots[i])
			{
				inventory().Activate(BestWeaponSlots[i]);
			}
			return;
		};
	};
}

#define ENEMY_HIT_SPOT	"mp_hit_sector_location"
BOOL	g_bShowHitSectors	= TRUE;

void	CActor::HitSector(CObject* who, CObject* weapon)
{
	if (!g_bShowHitSectors) return;
	if (!g_Alive()) return;

	bool bShowHitSector = true;
	
	CEntityAlive* pEntityAlive = smart_cast<CEntityAlive*>(who);

	if (!pEntityAlive || this == who) bShowHitSector = false;

	if (weapon)
	{
		CWeapon* pWeapon = smart_cast<CWeapon*> (weapon);
		if (pWeapon)
		{
			if (pWeapon->IsSilencerAttached())
			{
				bShowHitSector = false;
				if (pWeapon->IsGrenadeLauncherAttached())
				{
				}
			}
		}
	}

	if (!bShowHitSector) return;	
		Level().MapManager().AddMapLocation(ENEMY_HIT_SPOT, who->ID());
}

void CActor::on_weapon_shot_start		(CWeapon *weapon)
{	
	CWeaponMagazined* pWM = smart_cast<CWeaponMagazined*> (weapon);
	//*
	CCameraShotEffector				*effector = smart_cast<CCameraShotEffector*>	(Cameras().GetCamEffector(eCEShot)); 
	if (!effector) {
		effector					= 
			(CCameraShotEffector*)Cameras().AddCamEffector(
			xr_new<CCameraShotEffector>(weapon->camMaxAngle,
			weapon->camRelaxSpeed,
			weapon->camMaxAngleHorz,
			weapon->camStepAngleHorz,
			weapon->camDispertionFrac)	);
	}
	R_ASSERT						(effector);

	if (pWM)
	{
		if (effector->IsSingleShot())
			update_camera(effector);

		if (pWM->GetCurrentFireMode() == 1)
		{
			effector->SetSingleShoot(TRUE);
		}
		else
		{
			effector->SetSingleShoot(FALSE);
		}
	};

	effector->SetRndSeed			(GetShotRndSeed());
	effector->SetActor				(this);
	effector->Shot					(weapon->camDispersion + weapon->camDispersionInc*float(weapon->ShotsFired()));

	if (pWM)
	{
		if (pWM->GetCurrentFireMode() != 1)
		{
			effector->SetActive(FALSE);
			update_camera(effector);
		}		
	}
}

void CActor::on_weapon_shot_stop		(CWeapon *weapon)
{
	//---------------------------------------------
	CCameraShotEffector				*effector = smart_cast<CCameraShotEffector*>(Cameras().GetCamEffector(eCEShot)); 
	if (effector && effector->IsActive())
	{
		if (effector->IsSingleShot())
			update_camera(effector);
	}
	//---------------------------------------------
	Cameras().RemoveCamEffector(eCEShot);
}

void CActor::on_weapon_hide				(CWeapon *weapon)
{
	CCameraShotEffector				*effector = smart_cast<CCameraShotEffector*>(Cameras().GetCamEffector(eCEShot)); 
	if (effector && !effector->IsActive())
		effector->Clear				();
}

Fvector CActor::weapon_recoil_delta_angle	()
{
	CCameraShotEffector				*effector = smart_cast<CCameraShotEffector*>(Cameras().GetCamEffector(eCEShot));
	Fvector							result = {0.f,0.f,0.f};

	if (effector)
		effector->GetDeltaAngle		(result);

	return							(result);
}

Fvector CActor::weapon_recoil_last_delta()
{
	CCameraShotEffector				*effector = smart_cast<CCameraShotEffector*>(Cameras().GetCamEffector(eCEShot));
	Fvector							result = {0.f,0.f,0.f};

	if (effector)
		effector->GetLastDelta		(result);

	return							(result);
}
//////////////////////////////////////////////////////////////////////////

void	CActor::SpawnAmmoForWeapon	(CInventoryItem *pIItem)
{
	if (OnClient()) return;
	if (!pIItem) return;

	CWeaponMagazined* pWM = smart_cast<CWeaponMagazined*> (pIItem);
	if (!pWM || !pWM->AutoSpawnAmmo()) return;

	pWM->SpawnAmmo(0xffffffff, NULL, ID());
};

void	CActor::RemoveAmmoForWeapon	(CInventoryItem *pIItem)
{
	if (OnClient()) return;
	if (!pIItem) return;

	CWeaponMagazined* pWM = smart_cast<CWeaponMagazined*> (pIItem);
	if (!pWM || !pWM->AutoSpawnAmmo()) return;

	CWeaponAmmo* pAmmo = smart_cast<CWeaponAmmo*>(inventory().GetAny(*(pWM->m_ammoTypes[0]) ));
	if (!pAmmo) return;
	pAmmo->DestroyObject();
};

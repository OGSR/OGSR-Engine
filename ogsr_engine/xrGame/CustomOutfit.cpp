#include "stdafx.h"

#include "customoutfit.h"
#include "PhysicsShell.h"
#include "inventory_space.h"
#include "Inventory.h"
#include "Actor.h"
#include "game_cl_base.h"
#include "Level.h"
#include "BoneProtections.h"


CCustomOutfit::CCustomOutfit()
{
	SetSlot( OUTFIT_SLOT );

	m_flags.set(FUsingCondition, TRUE);

	m_HitTypeProtection.resize(ALife::eHitTypeMax);
	for(int i=0; i<ALife::eHitTypeMax; i++)
		m_HitTypeProtection[i] = 1.0f;

	m_boneProtection = xr_new<SBoneProtections>();

	m_fBleedingRestoreSpeed  = 0.f;
	m_fHealthRestoreSpeed    = 0.f;
	m_fPowerRestoreSpeed     = 0.f;
	m_fSatietyRestoreSpeed   = 0.f;
}

CCustomOutfit::~CCustomOutfit() 
{
	xr_delete(m_boneProtection);
}

void CCustomOutfit::net_Export(NET_Packet& P)
{
	inherited::net_Export	(P);
	P.w_float_q8			(m_fCondition,0.0f,1.0f);
}

void CCustomOutfit::net_Import(NET_Packet& P)
{
	inherited::net_Import	(P);
	P.r_float_q8			(m_fCondition,0.0f,1.0f);
}

void CCustomOutfit::Load(LPCSTR section) 
{
	inherited::Load(section);

	m_HitTypeProtection[ALife::eHitTypeBurn]		= pSettings->r_float(section,"burn_protection");
	m_HitTypeProtection[ALife::eHitTypeStrike]		= pSettings->r_float(section,"strike_protection");
	m_HitTypeProtection[ALife::eHitTypeShock]		= pSettings->r_float(section,"shock_protection");
	m_HitTypeProtection[ALife::eHitTypeWound]		= pSettings->r_float(section,"wound_protection");
	m_HitTypeProtection[ALife::eHitTypeRadiation]	= pSettings->r_float(section,"radiation_protection");
	m_HitTypeProtection[ALife::eHitTypeTelepatic]	= pSettings->r_float(section,"telepatic_protection");
	m_HitTypeProtection[ALife::eHitTypeChemicalBurn]= pSettings->r_float(section,"chemical_burn_protection");
	m_HitTypeProtection[ALife::eHitTypeExplosion]	= pSettings->r_float(section,"explosion_protection");
	m_HitTypeProtection[ALife::eHitTypeFireWound]	= pSettings->r_float(section,"fire_wound_protection");
	m_HitTypeProtection[ALife::eHitTypePhysicStrike]= READ_IF_EXISTS(pSettings, r_float, section, "physic_strike_protection", 0.0f);

	if (pSettings->line_exist(section, "actor_visual"))
		m_ActorVisual = pSettings->r_string(section, "actor_visual");
	else
		m_ActorVisual = NULL;

	m_ef_equipment_type		= pSettings->r_u32(section,"ef_equipment_type");
	if (pSettings->line_exist(section, "power_loss"))
		m_fPowerLoss = pSettings->r_float(section, "power_loss");
	else
		m_fPowerLoss = 1.0f;	

	m_additional_weight				= pSettings->r_float(section,"additional_inventory_weight");
	m_additional_weight2			= pSettings->r_float(section,"additional_inventory_weight2");

	if (pSettings->line_exist(section, "nightvision_sect"))
		m_NightVisionSect = pSettings->r_string(section, "nightvision_sect");
	else
		m_NightVisionSect = NULL;

	m_full_icon_name								= pSettings->r_string(section,"full_icon_name");

	m_fBleedingRestoreSpeed  = READ_IF_EXISTS( pSettings, r_float, section, "bleeding_restore_speed", 0.f );
	m_fHealthRestoreSpeed    = READ_IF_EXISTS( pSettings, r_float, section, "health_restore_speed", 0.f );
	m_fPowerRestoreSpeed     = READ_IF_EXISTS( pSettings, r_float, section, "power_restore_speed", 0.f );
	m_fSatietyRestoreSpeed   = READ_IF_EXISTS( pSettings, r_float, section, "satiety_restore_speed", 0.f );
}

void CCustomOutfit::Hit(float hit_power, ALife::EHitType hit_type)
{
	hit_power *= m_HitTypeK[hit_type];
	if (hit_power > 0)
	{
		ChangeCondition(-hit_power);
	}
}

float CCustomOutfit::GetDefHitTypeProtection(ALife::EHitType hit_type)
{
	return 1.0f - m_HitTypeProtection[hit_type]*GetCondition();
}

float CCustomOutfit::GetHitTypeProtection(ALife::EHitType hit_type, s16 element)
{
	float fBase = m_HitTypeProtection[hit_type]*GetCondition();
	float bone = m_boneProtection->getBoneProtection(element);
	return 1.0f - fBase*bone;
}

float	CCustomOutfit::HitThruArmour(float hit_power, s16 element, float AP)
{
	float BoneArmour = m_boneProtection->getBoneArmour(element)*GetCondition()*(1-AP);	
	float NewHitPower = hit_power - BoneArmour;
	if (NewHitPower < hit_power*m_boneProtection->m_fHitFrac) return hit_power*m_boneProtection->m_fHitFrac;
	return NewHitPower;
};

BOOL	CCustomOutfit::BonePassBullet					(int boneID)
{
	return m_boneProtection->getBonePassBullet(s16(boneID));
};

#include "torch.h"
void	CCustomOutfit::OnMoveToSlot		()
{
	inherited::OnMoveToSlot();
	if (m_pCurrentInventory)
	{
		CActor* pActor = smart_cast<CActor*> (m_pCurrentInventory->GetOwner());
		if (pActor)
		{
			if (m_ActorVisual.size())
			{
				pActor->ChangeVisual(m_ActorVisual);
			}
			if(pSettings->line_exist(cNameSect(),"bones_koeff_protection")){
				m_boneProtection->reload( pSettings->r_string(cNameSect(),"bones_koeff_protection"), smart_cast<CKinematics*>(pActor->Visual()) );
			};
		}
	}
};

void	CCustomOutfit::OnMoveToRuck		()
{
	inherited::OnMoveToRuck();
	if ( m_pCurrentInventory && !Level().is_removing_objects() )
	{
		CActor* pActor = smart_cast<CActor*> (m_pCurrentInventory->GetOwner());
		if (pActor)
		{
			// т.к. родительский инвентарь у предмета меняется раньше вызова этой функции, не будем ничего ломать, а просто проверим, есть ли в слоте бронька. Если есть - уходим.
			CCustomOutfit* pOutfit = smart_cast<CCustomOutfit*>(pActor->inventory().ItemFromSlot(OUTFIT_SLOT));
			if (pOutfit)
				return;

			CTorch* pTorch = smart_cast<CTorch*>(pActor->inventory().ItemFromSlot(TORCH_SLOT));
			if(pTorch)
			{
				pTorch->SwitchNightVision(false);
			}
			if (m_ActorVisual.size())
			{
				shared_str DefVisual = pActor->GetDefaultVisualOutfit();
				if (DefVisual.size())
				{
					pActor->ChangeVisual(DefVisual);
				};
			}
		}
	}
};

u32	CCustomOutfit::ef_equipment_type	() const
{
	return		(m_ef_equipment_type);
}

float CCustomOutfit::GetPowerLoss() 
{
	if (m_fPowerLoss<1 && GetCondition() <= 0)
	{
		return 1.0f;			
	};
	return m_fPowerLoss;
};

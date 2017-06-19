#include "stdafx.h"
#include "CustomZone.h"
#include "../SkeletonAnimated.h"
#include "ZoneVisual.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "../SkeletonAnimated.h"
CVisualZone::CVisualZone						()
{
}
CVisualZone::~CVisualZone						()
{

}

BOOL CVisualZone::	net_Spawn						(CSE_Abstract* DC)
{
	BOOL ret					=	inherited::net_Spawn(DC);
	CSE_Abstract				*e = (CSE_Abstract*)(DC);
	CSE_ALifeZoneVisual		*Z = smart_cast<CSE_ALifeZoneVisual*>(e);
	CKinematicsAnimated		*SA= smart_cast<CKinematicsAnimated*>(Visual());
	m_attack_animation		=SA->ID_Cycle(Z->attack_animation);
	m_idle_animation		=SA->ID_Cycle(Z->startup_animation);
	SA->PlayCycle(m_idle_animation);
	setVisible(TRUE);
	return ret;
}
void CVisualZone::net_Destroy()
{
	inherited::net_Destroy();

}
void CVisualZone:: AffectObjects					()		
{
	inherited::AffectObjects					();
//	smart_cast<CKinematicsAnimated*>(Visual())->PlayCycle(*m_attack_animation);
}
void CVisualZone::SwitchZoneState(EZoneState new_state)
{
	if(m_eZoneState==eZoneStateBlowout && new_state != eZoneStateBlowout)
	{
	//	CKinematicsAnimated*	SA=smart_cast<CKinematicsAnimated*>(Visual());
		smart_cast<CKinematicsAnimated*>(Visual())->PlayCycle(m_idle_animation);
	}

	inherited::SwitchZoneState(new_state);

}
void CVisualZone::Load(LPCSTR section)
{
	inherited::Load(section);
	m_dwAttackAnimaionStart		=pSettings->r_u32(section,"attack_animation_start");
	m_dwAttackAnimaionEnd		=pSettings->r_u32(section,"attack_animation_end");
	VERIFY2(m_dwAttackAnimaionStart<m_dwAttackAnimaionEnd,"attack_animation_start must be less then attack_animation_end");
}

void CVisualZone::UpdateBlowout()
{
	inherited::UpdateBlowout();
	if(m_dwAttackAnimaionStart >=(u32)m_iPreviousStateTime && 
		m_dwAttackAnimaionStart	<(u32)m_iStateTime)
				smart_cast<CKinematicsAnimated*>(Visual())->PlayCycle(m_attack_animation);
		
	if(m_dwAttackAnimaionEnd >=(u32)m_iPreviousStateTime && 
		m_dwAttackAnimaionEnd	<(u32)m_iStateTime)
				smart_cast<CKinematicsAnimated*>(Visual())->PlayCycle(m_idle_animation);
}
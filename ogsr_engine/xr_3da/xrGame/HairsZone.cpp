#include "pch_script.h"
#include "HairsZone.h"
#include "hudmanager.h"
#include "level.h"
#include "PhysicsShellHolder.h"
#include "entity_alive.h"
#include "PHMovementControl.h"
#include "CharacterPhysicsSupport.h"

bool CHairsZone::BlowoutState()
{
	bool result = inherited::BlowoutState();
	if(!result) UpdateBlowout();

	return result;
}

void CHairsZone::CheckForAwaking()
{
	for(OBJECT_INFO_VEC_IT it = m_ObjectInfoMap.begin(); 
		m_ObjectInfoMap.end() != it; ++it) 
	{
		CObject* pObject = (*it).object;
		if (!pObject) continue;

		CEntityAlive* pEnt = smart_cast<CEntityAlive*>(pObject);
		if(pEnt){
			float sp = pEnt->character_physics_support()->movement()->GetVelocityActual();
			if(sp>m_min_speed_to_react){
				SwitchZoneState				(eZoneStateAwaking);
				return;
			}
		}
/*
		u32 cnt = pObject->ps_Size();
		if(cnt>2){
			CObject::SavedPosition p0 = pObject->ps_Element(cnt-1);
			CObject::SavedPosition p1 = pObject->ps_Element(cnt-2);

			float dist	= p0.vPosition.distance_to(p1.vPosition);
			float tm	= (p0.dwTime-p1.dwTime)/1000.0f;
			float sp	= dist/tm;
			if(sp>m_min_speed_to_react){
				SwitchZoneState				(eZoneStateAwaking);
				return;
			}
		}
*/
	}
}

void CHairsZone::Load(LPCSTR section) 
{
	inherited::Load				(section);
	m_min_speed_to_react		= pSettings->r_float(section,			"min_speed_to_react");
}

void CHairsZone::Affect(SZoneObjectInfo* O) 
{
	CPhysicsShellHolder *pGameObject = smart_cast<CPhysicsShellHolder*>(O->object);
	if(!pGameObject) return;

	if(O->zone_ignore) return;

	Fvector P; 
	XFORM().transform_tiny(P,CFORM()->getSphere().P);

#ifdef DEBUG
	if(bDebug){
		char l_pow[255]; 
		sprintf_s(l_pow, "zone hit. %.1f", Power(pGameObject->Position().distance_to(P)));
		Msg("%s %s",*pGameObject->cName(), l_pow);
	}
#endif

	Fvector hit_dir; 
	hit_dir.set(::Random.randF(-.5f,.5f), 
		::Random.randF(.0f,1.f), 
		::Random.randF(-.5f,.5f)); 
	hit_dir.normalize();


	Fvector position_in_bone_space;

	P.y=0.f;
	float power = Power(pGameObject->Position().distance_to(P));
	float impulse = m_fHitImpulseScale*power*pGameObject->GetMass();

	//статистика по объекту
	O->total_damage += power;
	O->hit_num++;

	if(power > 0.01f) 
	{
		m_dwDeltaTime = 0;
		position_in_bone_space.set(0.f,0.f,0.f);

		CreateHit(pGameObject->ID(),ID(),hit_dir,power,0,position_in_bone_space,impulse,m_eHitTypeBlowout);

		PlayHitParticles(pGameObject);
	}
}

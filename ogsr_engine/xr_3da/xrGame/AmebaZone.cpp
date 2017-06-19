#include "stdafx.h"
#include "CustomZone.h"
#include "../SkeletonAnimated.h"
#include "ZoneVisual.h"
#include "PHObject.h"
#include "PHMovementControl.h"
#include "AmebaZone.h"
#include "hudmanager.h"
#include "level.h"
#include "entity_alive.h"
#include "CharacterPhysicsSupport.h"
CAmebaZone::CAmebaZone()
{
	m_fVelocityLimit=1.f;
}

CAmebaZone::~CAmebaZone()
{

}
void CAmebaZone::Load(LPCSTR section)
{
	inherited::Load(section);
	m_fVelocityLimit= pSettings->r_float(section,		"max_velocity_in_zone");
}
bool CAmebaZone::BlowoutState()
{
	bool result = inherited::BlowoutState();
	if(!result) UpdateBlowout();

	for(OBJECT_INFO_VEC_IT it = m_ObjectInfoMap.begin(); m_ObjectInfoMap.end() != it; ++it) 
		Affect(&(*it));

	return result;
}

void  CAmebaZone::Affect(SZoneObjectInfo* O) 
{
	CPhysicsShellHolder *pGameObject = smart_cast<CPhysicsShellHolder*>(O->object);
	if(!pGameObject) return;

	if(O->zone_ignore) return;

#ifdef DEBUG
	char l_pow[255]; 
	sprintf_s(l_pow, "zone hit. %.1f", Power(distance_to_center(O->object)));
	if(bDebug) Msg("%s %s",*pGameObject->cName(), l_pow);
#endif
	Fvector hit_dir; 
	hit_dir.set(::Random.randF(-.5f,.5f), 
		::Random.randF(.0f,1.f), 
		::Random.randF(-.5f,.5f)); 
	hit_dir.normalize();


	Fvector position_in_bone_space;

	float power = Power(distance_to_center(O->object));
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

void CAmebaZone::PhTune(dReal step)
{
	OBJECT_INFO_VEC_IT it;
	for(it = m_ObjectInfoMap.begin(); m_ObjectInfoMap.end() != it; ++it) 
	{
		CEntityAlive	*EA=smart_cast<CEntityAlive*>((*it).object);
		if(EA)
		{
			CPHMovementControl* mc=EA->character_physics_support()->movement();
			if(mc)
			{
				//Fvector vel;
				//mc->GetCharacterVelocity(vel);
				//vel.invert();
				//vel.mul(mc->GetMass());
				if(distance_to_center(EA)<effective_radius())
								mc->SetVelocityLimit(m_fVelocityLimit);
			}
		}
		
	}
}

void CAmebaZone::SwitchZoneState(EZoneState new_state)
{
	if(new_state==eZoneStateBlowout&&m_eZoneState!=eZoneStateBlowout)
	{
		CPHUpdateObject::Activate();
	}

	if(new_state!=eZoneStateBlowout&&m_eZoneState==eZoneStateBlowout)
	{
		CPHUpdateObject::Deactivate();
	}
	inherited::SwitchZoneState(new_state);
}

float CAmebaZone::distance_to_center(CObject* O)
{
	Fvector P; 
	XFORM().transform_tiny(P,CFORM()->getSphere().P);
	Fvector OP;OP.set(O->Position());
	return _sqrt((P.x-OP.x)*(P.x-OP.x)+(P.x-OP.x)*(P.x-OP.x));
}

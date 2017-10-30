////////////////////////////////////////////////////////////////////////////
//	Module 		: script_actor.h
//	Created 	: 12.08.2014
//  Modified 	: 12.08.2014
//	Author		: Alexander Petrov
//	Description : Script Actor (params)
////////////////////////////////////////////////////////////////////////////



#pragma once

#include "Actor.h"
#include "ActorCondition.h"
#include "script_export_space.h"

class CActorObject:
	public CActor
{

};

class CActorConditionObject :
	public CActorCondition
{
	CActorConditionObject(CActor *pActor): CActorCondition(pActor) { };
};


class CScriptActor
{	
public:
	typedef CEntityCondition::SConditionChangeV SConditionChangeV;

	IC static HitImmunity::HitTypeSVec &immunities (CActorCondition *C) { return C->m_HitTypeK; };
	IC static CActorCondition::SConditionChangeV &sccv (CActorCondition *C) { return C->m_change_v; };
	IC static float					   &jump_speed(CActor *A) { return A->m_fJumpSpeed;  }

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CScriptActor)
#undef script_type_list
#define script_type_list save_type_list(CScriptActor)

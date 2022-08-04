////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_impl.h
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Stalker" (inline functions implementation)
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../../effectorshot.h"

IC Fvector CAI_Stalker::weapon_shot_effector_direction(const Fvector& current) const
{
    VERIFY(weapon_shot_effector().IsActive());
    Fvector result;
    weapon_shot_effector().GetDeltaAngle(result);

    float y, p;
    current.getHP(y, p);

    result.setHP(-result.y + y, -result.x + p);

    return (result);
}

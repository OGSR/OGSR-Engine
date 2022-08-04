////////////////////////////////////////////////////////////////////////////
//	Module 		: script_actor.cpp
//	Created 	: 12.08.2014
//  Modified 	: 23.08.2014
//	Author		: Alexander Petrov
//	Description : Script Actor (params)
////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "base_client_classes.h"
#include "script_game_object.h"
#include "CharacterPhysicsSupport.h"
#include "script_actor.h"
#include "PHSimpleCharacter.h"
#include "Inventory.h"
#include "Wound.h"

//#include "../lua_tools.h"

using namespace luabind;

CPHMovementControl* get_movement(CActor* pActor) { return pActor->character_physics_support()->movement(); }

#pragma optimize("s", on)

typedef CScriptActor::SConditionChangeV SConditionChangeV;
typedef float SConditionChangeV::*SConditionChangeVField;

template <SConditionChangeVField field>
float get_change_v(CActorCondition* C)
{
    return CScriptActor::sccv(C).*field;
}

template <SConditionChangeVField field>
void set_change_v(CActorCondition* C, float v)
{
    CScriptActor::sccv(C).*field = v;
}

void set_health(CActorCondition* C, float h) { C->health() = h; }
void set_max_health(CActorCondition* C, float h) { C->max_health() = h; }

float get_wound_size(CActorCondition* C, u32 bone, u32 hit_type)
{
    if (C->wounds().size() <= bone)
        return 0;
    return C->wounds().at(bone)->TypeSize((ALife::EHitType)hit_type);
}

float get_wound_total_size(CActorCondition* C, u32 bone)
{
    if (C->wounds().size() <= bone)
        return 0;
    return C->wounds().at(bone)->TotalSize();
}

float get_jump_up_velocity(CPHMovementControl* M)
{
    CPHSimpleCharacter* sp = smart_cast<CPHSimpleCharacter*>(M->PHCharacter());
    if (sp)
        return sp->get_jump_up_velocity();
    return 0;
}

float get_jump_speed(CActor* pActor) { return CScriptActor::jump_speed(pActor); }
void set_jump_speed(CActor* pActor, float speed)
{
    CScriptActor::jump_speed(pActor) = speed;
    get_movement(pActor)->SetJumpUpVelocity(speed);
}

CHitImmunity* get_immunities(CActor* pActor) { return pActor->conditions().cast_hit_immunities(); }

CEntity::SEntityState* get_actor_state(CActor* pActor)
{
    static CEntity::SEntityState state;
    pActor->g_State(state);
    return &state;
}

CActorConditionObject* get_actor_condition(CActor* pActor) { return (CActorConditionObject*)(&pActor->conditions()); }

SRotation& get_actor_orientation(CActor* pActor) { return pActor->Orientation(); }

// extern LPCSTR get_lua_class_name(luabind::object O);

bool IsLimping(CActorCondition* C) { return C->m_condition_flags.test(CActorCondition::eLimping); }

bool IsCantWalk(CActorCondition* C) { return C->m_condition_flags.test(CActorCondition::eCantWalk); }

bool IsCantSprint(CActorCondition* C) { return C->m_condition_flags.test(CActorCondition::eCantSprint); }

void CScriptActor::script_register(lua_State* L)
{
    module(
        L)[class_<CActorCondition>("CActorConditionBase")
               .property("health", &CActorCondition::GetHealth, &set_health)
               .property("health_max", &CActorCondition::GetMaxHealth, &set_max_health)
               .def_readwrite("alcohol_health", &CActorCondition::m_fAlcohol)
               .def_readwrite("alcohol_v", &CActorCondition::m_fV_Alcohol)
               .def_readwrite("power_v", &CActorCondition::m_fV_Power)
               .def_readwrite("satiety", &CActorCondition::m_fSatiety)
               .def_readwrite("satiety_v", &CActorCondition::m_fV_Satiety)
               .def_readwrite("satiety_health_v", &CActorCondition::m_fV_SatietyHealth)
               .def_readwrite("satiety_power_v", &CActorCondition::m_fV_SatietyPower)

               .def_readwrite("thirst", &CActorCondition::m_fThirst)
               .def_readwrite("thirst_v", &CActorCondition::m_fV_Thirst)
               .def_readwrite("thirst_health_v", &CActorCondition::m_fV_ThirstHealth)
               .def_readwrite("thirst_power_v", &CActorCondition::m_fV_ThirstPower)

               .def_readwrite("max_power_leak_speed", &CActorCondition::m_fPowerLeakSpeed)
               .def_readwrite("jump_power", &CActorCondition::m_fJumpPower)
               .def_readwrite("stand_power", &CActorCondition::m_fStandPower)
               .def_readwrite("walk_power", &CActorCondition::m_fWalkPower)
               .def_readwrite("jump_weight_power", &CActorCondition::m_fJumpWeightPower)
               .def_readwrite("walk_weight_power", &CActorCondition::m_fWalkWeightPower)
               .def_readwrite("overweight_walk_k", &CActorCondition::m_fOverweightWalkK)
               .def_readwrite("overweight_jump_k", &CActorCondition::m_fOverweightJumpK)
               .def_readwrite("accel_k", &CActorCondition::m_fAccelK)
               .def_readwrite("sprint_k", &CActorCondition::m_fSprintK)
               .def_readwrite("max_walk_weight", &CActorCondition::m_MaxWalkWeight)

               //.def_readwrite("health_hit_part",			&CActorCondition::m_fHealthHitPart)
               .def_readwrite("power_hit_part", &CActorCondition::m_fPowerHitPart)

               .def_readwrite("limping_power_begin", &CActorCondition::m_fLimpingPowerBegin)
               .def_readwrite("limping_power_end", &CActorCondition::m_fLimpingPowerEnd)
               .def_readwrite("cant_walk_power_begin", &CActorCondition::m_fCantWalkPowerBegin)
               .def_readwrite("cant_walk_power_end", &CActorCondition::m_fCantWalkPowerEnd)
               .def_readwrite("cant_spint_power_begin", &CActorCondition::m_fCantSprintPowerBegin)
               .def_readwrite("cant_spint_power_end", &CActorCondition::m_fCantSprintPowerEnd)
               .def_readwrite("limping_health_begin", &CActorCondition::m_fLimpingHealthBegin)
               .def_readwrite("limping_health_end", &CActorCondition::m_fLimpingHealthEnd)
               .property("limping", &IsLimping)
               .property("cant_walk", &IsCantWalk)
               .property("cant_sprint", &IsCantSprint)
               .property("radiation_v", &get_change_v<&SConditionChangeV::m_fV_Radiation>, &set_change_v<&SConditionChangeV::m_fV_Radiation>)
               .property("psy_health_v", &get_change_v<&SConditionChangeV::m_fV_PsyHealth>, &set_change_v<&SConditionChangeV::m_fV_PsyHealth>)
               .property("morale_v", &get_change_v<&SConditionChangeV::m_fV_EntityMorale>, &set_change_v<&SConditionChangeV::m_fV_EntityMorale>)
               .property("radiation_health_v", &get_change_v<&SConditionChangeV::m_fV_RadiationHealth>, &set_change_v<&SConditionChangeV::m_fV_RadiationHealth>)
               .property("bleeding_v", &get_change_v<&SConditionChangeV::m_fV_Bleeding>, &set_change_v<&SConditionChangeV::m_fV_Bleeding>)
               .property("wound_incarnation_v", &get_change_v<&SConditionChangeV::m_fV_WoundIncarnation>, &set_change_v<&SConditionChangeV::m_fV_WoundIncarnation>)
               .property("health_restore_v", &get_change_v<&SConditionChangeV::m_fV_HealthRestore>, &set_change_v<&SConditionChangeV::m_fV_HealthRestore>)
               .def("get_wound_size", &get_wound_size)
               .def("get_wound_total_size", &get_wound_total_size)
           //.property("class_name",						&get_lua_class_name)
           ,
           class_<CActorConditionObject, bases<CActorCondition, CEntityCondition>>("CActorCondition") // нормальное наследование свойств происходит через Ж (
           ,
           class_<CPHMovementControl>("CPHMovementControl")
               .def_readwrite("ph_mass", &CPHMovementControl::fMass)
               .def_readwrite("crash_speed_max", &CPHMovementControl::fMaxCrashSpeed)
               .def_readwrite("crash_speed_min", &CPHMovementControl::fMinCrashSpeed)
               .def_readwrite("collision_damage_factor", &CPHMovementControl::fCollisionDamageFactor)
               .def_readwrite("air_control_param", &CPHMovementControl::fAirControlParam)
               .property("jump_up_velocity", &get_jump_up_velocity, &CPHMovementControl::SetJumpUpVelocity)
           //.property("class_name",						&get_lua_class_name)
           ,
           class_<CActor, bases<CInventoryOwner, CGameObject>>("CActorBase")
               .property("condition", &get_actor_condition)
               .property("immunities", &get_immunities)
               .def_readwrite("hit_slowmo", &CActor::hit_slowmo)
               .def_readwrite("hit_probability", &CActor::hit_probability)
               .def_readwrite("walk_accel", &CActor::m_fWalkAccel)

               .def_readwrite("run_coef", &CActor::m_fRunFactor)
               .def_readwrite("run_back_coef", &CActor::m_fRunBackFactor)
               .def_readwrite("walk_back_coef", &CActor::m_fWalkBackFactor)
               .def_readwrite("crouch_coef", &CActor::m_fCrouchFactor)
               .def_readwrite("climb_coef", &CActor::m_fClimbFactor)
               .def_readwrite("sprint_koef", &CActor::m_fSprintFactor)
               .def_readwrite("walk_strafe_coef", &CActor::m_fWalk_StrafeFactor)
               .def_readwrite("run_strafe_coef", &CActor::m_fRun_StrafeFactor)
               .def_readwrite("disp_base", &CActor::m_fDispBase)
               .def_readwrite("disp_aim", &CActor::m_fDispAim)
               .def_readwrite("disp_vel_factor", &CActor::m_fDispVelFactor)
               .def_readwrite("disp_accel_factor", &CActor::m_fDispAccelFactor)
               .def_readwrite("disp_crouch_factor", &CActor::m_fDispCrouchFactor)
               .def_readwrite("disp_crouch_no_acc_factor", &CActor::m_fDispCrouchNoAccelFactor)

               .property("movement", &get_movement)
               .property("jump_speed", &get_jump_speed, &set_jump_speed)
               .property("state", &get_actor_state)
               .property("orientation", &get_actor_orientation)

               // Real Wolf. Start. 14.10.2014.
               .def("block_action", &CActor::block_action)
               .def("unblock_action", &CActor::unblock_action)
               .def("press_action", &CActor::IR_OnKeyboardPress)
               .def("hold_action", &CActor::IR_OnKeyboardHold)
               .def("release_action", &CActor::IR_OnKeyboardRelease)
               .def("is_zoom_aiming_mode", &CActor::IsZoomAimingMode)
               // Real Wolf. End. 14.10.2014.

               .def("get_body_state", &CActor::get_state)
               .def("is_actor_normal", &CActor::is_actor_normal)
               .def("is_actor_crouch", &CActor::is_actor_crouch)
               .def("is_actor_creep", &CActor::is_actor_creep)
               .def("is_actor_climb", &CActor::is_actor_climb)
               .def("is_actor_walking", &CActor::is_actor_walking)
               .def("is_actor_running", &CActor::is_actor_running)
               .def("is_actor_sprinting", &CActor::is_actor_sprinting)
               .def("is_actor_crouching", &CActor::is_actor_crouching)
               .def("is_actor_creeping", &CActor::is_actor_creeping)
               .def("is_actor_climbing", &CActor::is_actor_climbing)
               .def("is_actor_moving", &CActor::is_actor_moving)
               .def("UpdateArtefactsOnBelt", &CActor::UpdateArtefactsOnBelt)
               .def("IsDetectorActive", &CActor::IsDetectorActive),
           class_<CActorObject, bases<CActor, CEntityAlive>>("CActor") // хак с наследованием нужен для переопределения свойств. Luabind не поддерживает property getters override

    ];
}

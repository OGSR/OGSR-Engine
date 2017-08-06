#pragma once

#include "controller_state_attack_hide.h"
#include "controller_state_attack_hide_lite.h"
#include "controller_state_attack_moveout.h"
#include "controller_state_attack_camp.h"
#include "controller_state_attack_fire.h"
#include "controller_tube.h"

#define CONTROL_FIRE_PERC 80
#define CONTROL_TUBE_PERC 20


#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateControllerAttackAbstract CStateControllerAttack<_Object>

TEMPLATE_SPECIALIZATION
CStateControllerAttackAbstract::CStateControllerAttack(_Object *obj, state_ptr state_run, state_ptr state_melee) : inherited(obj)
{
	add_state	(eStateAttack_HideInCover,		xr_new<CStateControlHide<_Object> >	(obj));
	add_state	(eStateAttack_HideInCoverLite,	xr_new<CStateControlHideLite<_Object> >	(obj));
	add_state	(eStateAttack_MoveOut,			xr_new<CStateControlMoveOut<_Object> >	(obj));
	add_state	(eStateAttack_CampInCover,		xr_new<CStateControlCamp<_Object> >		(obj));
	add_state	(eStateAttack_ControlFire,		xr_new<CStateControlFire<_Object> >		(obj));

	add_state	(eStateAttack_ControlTube,		xr_new<CStateControllerTube<_Object> >	(obj));
	
}

TEMPLATE_SPECIALIZATION
void CStateControllerAttackAbstract::execute()
{
	EMonsterState state_id = eStateUnknown;

	// Prev == None
	if (prev_substate == u32(-1))
		state_id = eStateAttack_HideInCoverLite;

	// Prev == Hide Cover Lite
	if (state_id == eStateUnknown) {
		if (prev_substate == eStateAttack_HideInCoverLite) {
			if (!get_state_current()->check_completion())
				state_id = eStateAttack_HideInCoverLite;
			else if (get_state(eStateAttack_MoveOut)->check_start_conditions())
				state_id =eStateAttack_MoveOut;
		}
	}

	// Prev == Move Out
	if (state_id == eStateUnknown) {
		if (prev_substate == eStateAttack_MoveOut) {
			if (get_state(eStateAttack_ControlFire)->check_start_conditions())
				state_id =eStateAttack_ControlFire;
			else {
				if (!get_state_current()->check_completion())
					state_id = eStateAttack_MoveOut;
				else 
					state_id = eStateAttack_HideInCover;
			}
			
			//// eStateAttack_ControlTube
			//bool control_fire_can = get_state(eStateAttack_ControlFire)->check_start_conditions();
			//bool control_tube_can = get_state(eStateAttack_ControlTube)->check_start_conditions();

			//if (control_fire_can && !control_tube_can) {
			//	state_id = eStateAttack_ControlFire;
			//} else if (!control_fire_can && control_tube_can) {
			//	state_id = eStateAttack_ControlTube;
			//} else if (control_fire_can && control_tube_can) {
			//	s32 rand = Random.randI(CONTROL_FIRE_PERC + CONTROL_TUBE_PERC);
			//	
			//	if (rand < CONTROL_FIRE_PERC)
			//		state_id = eStateAttack_ControlFire;
			//	else 
			//		state_id = eStateAttack_ControlTube;
			//} else {
			//	if (!get_state_current()->check_completion())
			//		state_id = eStateAttack_MoveOut;
			//	else 
			//		state_id = eStateAttack_HideInCover;
			//}
		}
	}

	// Prev == Control Fire
	if (state_id == eStateUnknown) {
		if (prev_substate == eStateAttack_ControlFire) {
			if (!get_state_current()->check_completion())
				state_id = eStateAttack_ControlFire;
			else 
				state_id = eStateAttack_HideInCover;

		}
	}

	// Prev == Hide Cover
	if (state_id == eStateUnknown) {
		if (prev_substate == eStateAttack_HideInCover) {
			if (!get_state_current()->check_completion())
				state_id = eStateAttack_HideInCover;
			else 
				state_id = eStateAttack_CampInCover;

		}
	}

	// Prev == Camp In Cover
	if (state_id == eStateUnknown) {
		if (prev_substate == eStateAttack_CampInCover) {
			if (!get_state_current()->check_completion())
				state_id = eStateAttack_CampInCover;
			else {
				if (get_state(eStateAttack_MoveOut)->check_start_conditions())
					state_id = eStateAttack_MoveOut;	
				else 
					state_id = eStateAttack_HideInCoverLite;
			}
		}
	}



	if (state_id == eStateUnknown) {
		state_id = eStateAttack_HideInCover;
	}

	select_state		(state_id);
	get_state_current()->execute();
	prev_substate		= current_substate;
}

TEMPLATE_SPECIALIZATION
void CStateControllerAttackAbstract::setup_substates()
{
	//state_ptr state = get_state_current();

	//if (current_substate == eStateFaceEnemy) {
	//	SStateDataLookToPoint data;

	//	data.point				= object->EnemyMan.get_enemy_position();
	//	data.action.action		= ACT_STAND_IDLE;

	//	state->fill_data_with(&data, sizeof(SStateDataLookToPoint));
	//
	//	object->sound().play(MonsterSound::eMonsterSoundAggressive, 0,0,object->db().m_dwAttackSndDelay);
	//	return;
	//}
}

TEMPLATE_SPECIALIZATION
void CStateControllerAttackAbstract::check_force_state() 
{
	//if (current_substate == eStateRun) {
	//	float dist_to_enemy = object->Position().distance_to(object->EnemyMan.get_enemy_position());
	//	if (dist_to_enemy > 10.f) {
	//		get_state_current()->critical_finalize();
	//		current_substate = u32(-1);
	//	}
	//}
}

TEMPLATE_SPECIALIZATION
void CStateControllerAttackAbstract::initialize()
{
	inherited::initialize();
	object->set_mental_state(CController::eStateDanger);
}

TEMPLATE_SPECIALIZATION
void CStateControllerAttackAbstract::finalize()
{
	inherited::finalize();
	object->set_mental_state(CController::eStateIdle);
}

TEMPLATE_SPECIALIZATION
void CStateControllerAttackAbstract::critical_finalize()
{
	inherited::critical_finalize();
	object->set_mental_state(CController::eStateIdle);
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateControllerAttackAbstract
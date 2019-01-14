//#pragma once
//
////#include "bloodsucker_attack_state_hide.h"
//#include "../states/state_move_to_point.h"
//
//#define TEMPLATE_SPECIALIZATION template <\
//	typename _Object\
//>
//
//#define CBloodsuckerStateAttackAbstract CBloodsuckerStateAttack<_Object>
//
//TEMPLATE_SPECIALIZATION
//CBloodsuckerStateAttackAbstract::CBloodsuckerStateAttack(_Object *obj) : inherited_attack(obj)
//{
//	add_state	(eStateAttack_Hide,	xr_new<CStateMonsterBackstubEnemy<_Object> >(obj));
//	add_state	(eStateVampire_Execute,	xr_new<CStateBloodsuckerVampireExecute<_Object> >(obj));
//}
//
//TEMPLATE_SPECIALIZATION
//CBloodsuckerStateAttackAbstract::~CBloodsuckerStateAttack()
//{
//}
//
//TEMPLATE_SPECIALIZATION
//void CBloodsuckerStateAttackAbstract::initialize()
//{
//	inherited::initialize	();
//	m_time_stop_invis		= 0;
//	m_last_health           = object->conditions().GetHealth();
//}
//
//TEMPLATE_SPECIALIZATION
//void CBloodsuckerStateAttackAbstract::finalize()
//{
//	inherited::finalize();
//	object->start_invisible_predator();
//}
//
//TEMPLATE_SPECIALIZATION
//void CBloodsuckerStateAttackAbstract::critical_finalize()
//{
//	inherited::critical_finalize();
//	object->start_invisible_predator();
//}
//
//namespace detail
//{
//
//namespace bloodsucker
//{
//	const u32   encircle_time               = 3000;
//	const float loose_health_diff           = 0.15f;
//	const u32   change_behaviour_time       = 1000;
//
//} // namespace bloodsucker
//
//} // namespace detail
//
//TEMPLATE_SPECIALIZATION
//void CBloodsuckerStateAttackAbstract::execute()
//{
//	if ( check_home_point() )				select_state(eStateAttack_MoveToHomePoint);
//	else if ( check_vampire() )				select_state(eStateVampire_Execute);
//	else if ( check_steal_state() )			select_state(eStateAttack_Steal);
//	else if ( check_camp_state() )			select_state(eStateAttackCamp);
//	else if ( check_find_enemy_state() )	select_state(eStateAttack_FindEnemy);
//	else if ( check_hiding() )				select_state(eStateAttack_Hide);
//	else if ( check_run_attack_state() )	select_state(eStateAttack_RunAttack);
//	else
//	{
//		// определить тип атаки
//		bool b_melee = false; 
//
//		if ( prev_substate == eStateAttack_Melee )
//		{
//			if ( !get_state_current()->check_completion() )
//			{
//				b_melee = true;
//			}
//		} 
//		else if ( get_state(eStateAttack_Melee)->check_start_conditions() )
//		{
//			b_melee = true;
//		}
//		
// 		if ( !b_melee && (prev_substate == eStateAttack_Melee) )
// 		{
// 			select_state(eStateAttack_Hide);
// 		}
// 		else
//		// установить целевое состояние
//		if ( b_melee ) 
//		{  
//			// check if enemy is behind me for a long time
//			// [TODO] make specific state and replace run_away state (to avoid ratation jumps)
//			//if (check_behinder()) 
//			//	select_state(eStateAttack_RunAway);
//			//else 
//			select_state(eStateAttack_Melee);
//		}
//		else 
//		{
//			select_state(eStateAttack_Run);
//		}
//	}
//
//	// clear behinder var if not melee state selected
//	if ( current_substate != eStateAttack_Melee )
//	{
//		m_time_start_check_behinder = 0;
//	}
//	else
//	{
//		object->clear_runaway_invisible();
//	}
//	
//	get_state_current()->execute();
//	prev_substate = current_substate;
//
//	// Notify squad	
//	CMonsterSquad* squad = monster_squad().get_squad(object);
//	if ( squad )
//	{
//		SMemberGoal goal;
//
//		goal.type	= MG_AttackEnemy;
//		goal.entity	= const_cast<CEntityAlive*>(object->EnemyMan.get_enemy());
//
//		squad->UpdateGoal(object, goal);
//	}
//}
//
//TEMPLATE_SPECIALIZATION
//bool CBloodsuckerStateAttackAbstract::check_vampire()
//{
//	if ( prev_substate != eStateVampire_Execute )
//	{
//		if (get_state(eStateVampire_Execute)->check_start_conditions())	return true;
//	} 
//	else
//	{
//		if (!get_state(eStateVampire_Execute)->check_completion())		return true;
//	}
//	return false;
//}
//
//TEMPLATE_SPECIALIZATION
//bool CBloodsuckerStateAttackAbstract::check_hiding()
//{
//	const bool health_step_lost = object->conditions().GetHealth() < 
//		                          m_last_health-detail::bloodsucker::loose_health_diff;
//
//	if ( health_step_lost )
//	{
//		object->start_runaway_invisible();
//		m_last_health = object->conditions().GetHealth();
//		m_start_with_encircle = true;
//		return true;
//	}
//
//	// if we get here before 1 sec after last critical hit: 
//	u32 last_critical_hit_tick = object->get_last_critical_hit_tick();
//	if ( last_critical_hit_tick && time() < last_critical_hit_tick + 1000 )
//	{
//		object->clear_last_critical_hit_tick();
//		m_start_with_encircle = true;
//		return true;
//	}
//
//	if ( current_substate == eStateAttack_Hide )
//	{
//		return !get_state_current()->check_completion();
//	}
//
//	m_start_with_encircle = false;
//	return get_state(eStateAttack_Hide)->check_start_conditions();
//}
//
//TEMPLATE_SPECIALIZATION
//void CBloodsuckerStateAttackAbstract::setup_substates()
//{
//	state_ptr state = get_state_current();
//
//	if ( current_substate == eStateAttack_Hide ) 
//	{
//		CStateMonsterBackstubEnemy<_Object>::StateParams data;
//		
//		data.action.action		= ACT_RUN;
//		data.action.time_out	= 0;
//		data.completion_dist	= 1.f;		// get exactly to the point
//		data.time_to_rebuild	= 200;		
//		data.accelerated		= true;
//		data.braking			= false;
//		data.accel_type 		= eAT_Aggressive;
//		data.action.sound_type	= MonsterSound::eMonsterSoundIdle;
//		data.action.sound_delay = object->db().m_dwIdleSndDelay;
//		data.start_with_encircle = m_start_with_encircle;
//
//		state->fill_data_with(&data, sizeof(CStateMonsterBackstubEnemy<_Object>::StateParams));
//		return;
//	}
//}
//
//#undef TEMPLATE_SPECIALIZATION
//#undef CBloodsuckerStateAttackAbstract
//

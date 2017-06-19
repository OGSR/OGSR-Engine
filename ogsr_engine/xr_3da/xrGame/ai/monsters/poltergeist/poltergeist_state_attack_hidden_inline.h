#pragma once

#include "../../../sound_player.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStatePoltergeistAttackHiddenAbstract CStatePoltergeistAttackHidden<_Object>

#define FLY_AROUND_DIST		15.f
#define DIST_TO_PATH_END	1.5f


TEMPLATE_SPECIALIZATION
void CStatePoltergeistAttackHiddenAbstract::initialize()
{
	inherited::initialize();
	select_target_point();
	object->path().prepare_builder();
}

TEMPLATE_SPECIALIZATION
void CStatePoltergeistAttackHiddenAbstract::execute()
{
	// проверить на завершение пути
	if (object->control().path_builder().detail().time_path_built() > time_state_started) {
		if (object->control().path_builder().is_path_end(DIST_TO_PATH_END)) select_target_point();
	}
	
	object->path().set_target_point		(m_target.point, m_target.node);
	object->path().set_rebuild_time		(5000);
	object->path().set_distance_to_end	(3.f);
	object->path().set_use_covers		(false);

	object->anim().m_tAction			= ACT_RUN;
	object->anim().accel_activate	(eAT_Aggressive);
	object->anim().accel_set_braking (false);
	object->sound().play				(MonsterSound::eMonsterSoundAggressive, 0,0,object->db().m_dwAttackSndDelay);
}

TEMPLATE_SPECIALIZATION
void CStatePoltergeistAttackHiddenAbstract::select_target_point()
{
	float dist = object->Position().distance_to(object->EnemyMan.get_enemy_position());
	if (dist > FLY_AROUND_DIST) {
		m_target.point	= object->EnemyMan.get_enemy_position();
		m_target.node	= object->EnemyMan.get_enemy_vertex();
	} else {
		m_target.point	= random_position(object->EnemyMan.get_enemy_position(), FLY_AROUND_DIST / 2);
		m_target.node	= u32(-1);
	}
}

#undef TEMPLATE_SPECIALIZATION
#undef CStatePoltergeistAttackHiddenAbstract
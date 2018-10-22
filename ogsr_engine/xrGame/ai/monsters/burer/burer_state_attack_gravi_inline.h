#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateBurerAttackGraviAbstract CStateBurerAttackGravi<_Object>

#define GOOD_DISTANCE_FOR_GRAVI 6.f

TEMPLATE_SPECIALIZATION
CStateBurerAttackGraviAbstract::CStateBurerAttackGravi(_Object *obj) : inherited(obj)
{
}

TEMPLATE_SPECIALIZATION
void CStateBurerAttackGraviAbstract::initialize()
{
	inherited::initialize			();

	m_action						= ACTION_GRAVI_STARTED;

	time_gravi_started				= 0;

	object->set_script_capture		(false);
}

TEMPLATE_SPECIALIZATION
void CStateBurerAttackGraviAbstract::execute()
{
	switch (m_action) {
		/************************/
		case ACTION_GRAVI_STARTED:
			/************************/

			ExecuteGraviStart();
			m_action = ACTION_GRAVI_CONTINUE;

			break;
			/************************/
		case ACTION_GRAVI_CONTINUE:
			/************************/

			ExecuteGraviContinue();

			break;

			/************************/
		case ACTION_GRAVI_FIRE:
			/************************/

			ExecuteGraviFire();
			m_action = ACTION_WAIT_TRIPLE_END;

			break;
			/***************************/
		case ACTION_WAIT_TRIPLE_END:
			/***************************/
			if (!object->com_man().ta_is_active()) {
				m_action = ACTION_COMPLETED; 
			}

			/*********************/
		case ACTION_COMPLETED:
			/*********************/

			break;
	}

	object->anim().m_tAction	= ACT_STAND_IDLE;	
	object->dir().face_target	(object->EnemyMan.get_enemy(), 500);
}
TEMPLATE_SPECIALIZATION
void CStateBurerAttackGraviAbstract::finalize()
{
	inherited::finalize();

	object->com_man().ta_pointbreak	();
	object->DeactivateShield		();
	object->set_script_capture		(true);
}

TEMPLATE_SPECIALIZATION
void CStateBurerAttackGraviAbstract::critical_finalize()
{
	inherited::critical_finalize();
	
	object->com_man().ta_pointbreak	();
	object->DeactivateShield		();
	object->StopGraviPrepare		();
	object->set_script_capture		(false);
}

TEMPLATE_SPECIALIZATION
bool CStateBurerAttackGraviAbstract::check_start_conditions()
{
	// обработать объекты
	float dist = object->Position().distance_to(object->EnemyMan.get_enemy()->Position());
	if (dist < GOOD_DISTANCE_FOR_GRAVI) return false;
	if (!object->EnemyMan.see_enemy_now()) return false; 
	if (!object->control().direction().is_face_target(object->EnemyMan.get_enemy(), deg(45))) return false;
	if (object->com_man().ta_is_active()) return false;

	// всё ок, можно начать грави атаку
	return true;
}

TEMPLATE_SPECIALIZATION
bool CStateBurerAttackGraviAbstract::check_completion()
{
	return (m_action == ACTION_COMPLETED);
}

//////////////////////////////////////////////////////////////////////////

TEMPLATE_SPECIALIZATION
void CStateBurerAttackGraviAbstract::ExecuteGraviStart()
{
	object->com_man().ta_activate(object->anim_triple_gravi);

	time_gravi_started			= Device.dwTimeGlobal;

	object->StartGraviPrepare();
	object->ActivateShield();
}

TEMPLATE_SPECIALIZATION
void CStateBurerAttackGraviAbstract::ExecuteGraviContinue()
{
	// проверить на грави удар

	float dist = object->Position().distance_to(object->EnemyMan.get_enemy()->Position());
	float time_to_hold = (abs(dist - GOOD_DISTANCE_FOR_GRAVI)/GOOD_DISTANCE_FOR_GRAVI);
	clamp(time_to_hold, 0.f, 1.f);
	time_to_hold *= float(object->m_gravi_time_to_hold);

	if (time_gravi_started + u32(time_to_hold) < Device.dwTimeGlobal) {
		m_action = ACTION_GRAVI_FIRE;
	}
}

TEMPLATE_SPECIALIZATION
void CStateBurerAttackGraviAbstract::ExecuteGraviFire()
{
	object->com_man().ta_pointbreak();

	Fvector from_pos;
	Fvector target_pos;
	from_pos	= object->Position();	from_pos.y		+= 0.5f;
	target_pos	= object->EnemyMan.get_enemy()->Position();	target_pos.y	+= 0.5f;

	object->m_gravi_object.activate(object->EnemyMan.get_enemy(), from_pos, target_pos);

	object->StopGraviPrepare	();
	object->sound().play		(CBurer::eMonsterSoundGraviAttack);
	object->DeactivateShield	();
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateBurerAttackGraviAbstract
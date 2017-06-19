////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_property_evaluators.cpp
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker property evaluators classes
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "stalker_property_evaluators.h"
#include "ai/stalker/ai_stalker.h"
#include "stalker_decision_space.h"
#include "script_game_object.h"
#include "ai/ai_monsters_misc.h"
#include "inventory.h"
#include "alife_simulator.h"
#include "alife_object_registry.h"
#include "memory_manager.h"
#include "visual_memory_manager.h"
#include "item_manager.h"
#include "enemy_manager.h"
#include "danger_manager.h"
#include "ai_space.h"
#include "ai/stalker/ai_stalker.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "alife_human_brain.h"
#include "actor.h"
#include "actor_memory.h"
#include "stalker_movement_manager.h"
#include "agent_manager.h"
#include "agent_enemy_manager.h"

using namespace StalkerDecisionSpace;

typedef CStalkerPropertyEvaluator::_value_type _value_type;

const float wounded_enemy_reached_distance = 3.f;

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorALife
//////////////////////////////////////////////////////////////////////////

CStalkerPropertyEvaluatorALife::CStalkerPropertyEvaluatorALife	(CAI_Stalker *object, LPCSTR evaluator_name) :
	inherited		(object ? object->lua_game_object() : 0,evaluator_name)
{
}

_value_type CStalkerPropertyEvaluatorALife::evaluate	()
{
	return			(!!ai().get_alife());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorAlive
//////////////////////////////////////////////////////////////////////////

CStalkerPropertyEvaluatorAlive::CStalkerPropertyEvaluatorAlive	(CAI_Stalker *object, LPCSTR evaluator_name) :
	inherited		(object ? object->lua_game_object() : 0,evaluator_name)
{
}

_value_type CStalkerPropertyEvaluatorAlive::evaluate	()
{
	return			(!!object().g_Alive());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorItems
//////////////////////////////////////////////////////////////////////////

CStalkerPropertyEvaluatorItems::CStalkerPropertyEvaluatorItems	(CAI_Stalker *object, LPCSTR evaluator_name) :
	inherited		(object ? object->lua_game_object() : 0,evaluator_name)
{
}

_value_type CStalkerPropertyEvaluatorItems::evaluate	()
{
	return			(!!m_object->memory().item().selected());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorEnemies
//////////////////////////////////////////////////////////////////////////

CStalkerPropertyEvaluatorEnemies::CStalkerPropertyEvaluatorEnemies	(
	CAI_Stalker *object, 
	LPCSTR evaluator_name, 
	u32 time_to_wait,
	const bool *dont_wait
) : 
	inherited	(object ? object->lua_game_object() : 0,evaluator_name)
{
	m_time_to_wait	= time_to_wait;
	m_dont_wait		= dont_wait;
}

_value_type CStalkerPropertyEvaluatorEnemies::evaluate	()
{
	if (m_object->memory().enemy().selected())
		return			(true);

	if (m_dont_wait && *m_dont_wait)
		return			(false);

	if (Device.dwTimeGlobal < m_object->memory().enemy().last_enemy_time() + m_time_to_wait)
		return			(true);

	return				(false);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorSeeEnemy
//////////////////////////////////////////////////////////////////////////

CStalkerPropertyEvaluatorSeeEnemy::CStalkerPropertyEvaluatorSeeEnemy	(CAI_Stalker *object, LPCSTR evaluator_name) :
	inherited		(object ? object->lua_game_object() : 0,evaluator_name)
{
}

_value_type CStalkerPropertyEvaluatorSeeEnemy::evaluate	()
{
	return				(m_object->memory().enemy().selected() ? m_object->memory().visual().visible_now(m_object->memory().enemy().selected()) : false);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorEnemySeeMe
//////////////////////////////////////////////////////////////////////////

CStalkerPropertyEvaluatorEnemySeeMe::CStalkerPropertyEvaluatorEnemySeeMe	(CAI_Stalker *object, LPCSTR evaluator_name) :
	inherited		(object ? object->lua_game_object() : 0,evaluator_name)
{
}

_value_type CStalkerPropertyEvaluatorEnemySeeMe::evaluate	()
{
	const CEntityAlive		*enemy = m_object->memory().enemy().selected();
	if (!enemy)
		return				(false);

	const CCustomMonster	*enemy_monster = smart_cast<const CCustomMonster*>(enemy);
	if (enemy_monster)
		return				(enemy_monster->memory().visual().visible_now(m_object));

	const CActor			*actor = smart_cast<const CActor*>(enemy);
	if (actor)
		return				(actor->memory().visual().visible_now(m_object));

	return					(false);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorItemToKill
//////////////////////////////////////////////////////////////////////////

CStalkerPropertyEvaluatorItemToKill::CStalkerPropertyEvaluatorItemToKill	(CAI_Stalker *object, LPCSTR evaluator_name) :
	inherited		(object ? object->lua_game_object() : 0,evaluator_name)
{
}

_value_type CStalkerPropertyEvaluatorItemToKill::evaluate	()
{
	return				(!!m_object->item_to_kill());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorItemCanKill
//////////////////////////////////////////////////////////////////////////

CStalkerPropertyEvaluatorItemCanKill::CStalkerPropertyEvaluatorItemCanKill	(CAI_Stalker *object, LPCSTR evaluator_name) :
	inherited		(object ? object->lua_game_object() : 0,evaluator_name)
{
}

_value_type CStalkerPropertyEvaluatorItemCanKill::evaluate	()
{
	return				(m_object->item_can_kill());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorFoundItemToKill
//////////////////////////////////////////////////////////////////////////

CStalkerPropertyEvaluatorFoundItemToKill::CStalkerPropertyEvaluatorFoundItemToKill	(CAI_Stalker *object, LPCSTR evaluator_name) :
	inherited		(object ? object->lua_game_object() : 0,evaluator_name)
{
}

_value_type CStalkerPropertyEvaluatorFoundItemToKill::evaluate	()
{
	return				(m_object->remember_item_to_kill());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorFoundAmmo
//////////////////////////////////////////////////////////////////////////

CStalkerPropertyEvaluatorFoundAmmo::CStalkerPropertyEvaluatorFoundAmmo	(CAI_Stalker *object, LPCSTR evaluator_name) :
	inherited		(object ? object->lua_game_object() : 0,evaluator_name)
{
}

_value_type CStalkerPropertyEvaluatorFoundAmmo::evaluate	()
{
	return				(m_object->remember_ammo());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorReadyToKill
//////////////////////////////////////////////////////////////////////////

CStalkerPropertyEvaluatorReadyToKill::CStalkerPropertyEvaluatorReadyToKill	(CAI_Stalker *object, LPCSTR evaluator_name) :
	inherited		(object ? object->lua_game_object() : 0,evaluator_name)
{
}

_value_type CStalkerPropertyEvaluatorReadyToKill::evaluate	()
{
	return				(m_object->ready_to_kill());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorReadyToDetour
//////////////////////////////////////////////////////////////////////////

CStalkerPropertyEvaluatorReadyToDetour::CStalkerPropertyEvaluatorReadyToDetour	(CAI_Stalker *object, LPCSTR evaluator_name) :
	inherited		(object ? object->lua_game_object() : 0,evaluator_name)
{
}

_value_type CStalkerPropertyEvaluatorReadyToDetour::evaluate	()
{
	return				(m_object->ready_to_detour());
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorAnomaly
//////////////////////////////////////////////////////////////////////////

CStalkerPropertyEvaluatorAnomaly::CStalkerPropertyEvaluatorAnomaly	(CAI_Stalker *object, LPCSTR evaluator_name) :
	inherited		(object ? object->lua_game_object() : 0,evaluator_name)
{
}

_value_type CStalkerPropertyEvaluatorAnomaly::evaluate	()
{
	if (!m_object->undetected_anomaly())
		return			(false);

	if (!m_object->memory().enemy().selected())
		return			(true);

	u32					result = dwfChooseAction(2000,m_object->panic_threshold(),0.f,0.f,0.f,m_object->g_Team(),m_object->g_Squad(),m_object->g_Group(),0,1,2,3,4,m_object,300.f);
	return				(!result);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorInsideAnomaly
//////////////////////////////////////////////////////////////////////////

CStalkerPropertyEvaluatorInsideAnomaly::CStalkerPropertyEvaluatorInsideAnomaly	(CAI_Stalker *object, LPCSTR evaluator_name) :
	inherited		(object ? object->lua_game_object() : 0,evaluator_name)
{
}

_value_type CStalkerPropertyEvaluatorInsideAnomaly::evaluate	()
{
	if (!m_object->inside_anomaly())
		return			(false);

	if (!m_object->memory().enemy().selected())
		return			(true);

	u32					result = dwfChooseAction(2000,m_object->panic_threshold(),0.f,0.f,0.f,m_object->g_Team(),m_object->g_Squad(),m_object->g_Group(),0,1,2,3,4,m_object,300.f);
	return				(!result);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorPanic
//////////////////////////////////////////////////////////////////////////

CStalkerPropertyEvaluatorPanic::CStalkerPropertyEvaluatorPanic	(CAI_Stalker *object, LPCSTR evaluator_name) :
	inherited		(object ? object->lua_game_object() : 0,evaluator_name)
{
}

_value_type CStalkerPropertyEvaluatorPanic::evaluate	()
{
	u32					result = dwfChooseAction(2000,m_object->panic_threshold(),0.f,0.f,0.f,m_object->g_Team(),m_object->g_Squad(),m_object->g_Group(),0,1,2,3,4,m_object,300.f);
	return				(!!result);
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorSmartTerrainTask
//////////////////////////////////////////////////////////////////////////

CStalkerPropertyEvaluatorSmartTerrainTask::CStalkerPropertyEvaluatorSmartTerrainTask	(CAI_Stalker *object, LPCSTR evaluator_name) :
	inherited		(object ? object->lua_game_object() : 0,evaluator_name)
{
}

_value_type CStalkerPropertyEvaluatorSmartTerrainTask::evaluate	()
{
	if (!ai().get_alife())
		return			(false);

	CSE_ALifeHumanAbstract		*stalker = smart_cast<CSE_ALifeHumanAbstract*>(ai().alife().objects().object(m_object->ID()));
	VERIFY						(stalker);
	stalker->brain().select_task();
	return						(stalker->m_smart_terrain_id != 0xffff);
}


//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorEnemyReached
//////////////////////////////////////////////////////////////////////////

CStalkerPropertyEvaluatorEnemyReached::CStalkerPropertyEvaluatorEnemyReached	(CAI_Stalker *object, LPCSTR evaluator_name) :
	inherited		(object ? object->lua_game_object() : 0,evaluator_name)
{
}

_value_type CStalkerPropertyEvaluatorEnemyReached::evaluate	()
{
	const CEntityAlive			*enemy = object().memory().enemy().selected();
	if (!enemy)
		return					(false);

	ALife::_OBJECT_ID			processor_id = object().agent_manager().enemy().wounded_processor(enemy);
	if (processor_id != object().ID())
		return					(false);

	return						((object().Position().distance_to_sqr(enemy->Position()) <= _sqr(wounded_enemy_reached_distance)));
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorPlayerOnThePath
//////////////////////////////////////////////////////////////////////////

CStalkerPropertyEvaluatorPlayerOnThePath::CStalkerPropertyEvaluatorPlayerOnThePath	(CAI_Stalker *object, LPCSTR evaluator_name) :
	inherited		(object ? object->lua_game_object() : 0,evaluator_name)
{
}

_value_type CStalkerPropertyEvaluatorPlayerOnThePath::evaluate	()
{
	const CEntityAlive			*enemy = object().memory().enemy().selected();
	if (!enemy)
		return					(false);

	if (!object().is_relation_enemy(Actor()))
		return					(false);

	if (!m_object->memory().visual().visible_now(Actor()))
		return					(false);

	return						(object().movement().is_object_on_the_way(Actor(),2.f));
}

//////////////////////////////////////////////////////////////////////////
// CStalkerPropertyEvaluatorEnemyCriticallyWounded
//////////////////////////////////////////////////////////////////////////

CStalkerPropertyEvaluatorEnemyCriticallyWounded::CStalkerPropertyEvaluatorEnemyCriticallyWounded	(CAI_Stalker *object, LPCSTR evaluator_name) :
	inherited		(object ? object->lua_game_object() : 0,evaluator_name)
{
}

_value_type CStalkerPropertyEvaluatorEnemyCriticallyWounded::evaluate	()
{
	const CEntityAlive			*enemy = object().memory().enemy().selected();
	if (!enemy)
		return					(false);

	const CAI_Stalker			*enemy_stalker = smart_cast<const CAI_Stalker*>(enemy);
	if (!enemy_stalker)
		return					(false);

	return						(const_cast<CAI_Stalker*>(enemy_stalker)->critically_wounded());
}

////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_cover.cpp
//	Created 	: 25.04.2006
//  Modified 	: 25.04.2006
//	Author		: Dmitriy Iassenev
//	Description : 
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "../../cover_point.h"
#include "../../cover_evaluators.h"
#include "../../ai_space.h"
#include "../../cover_manager.h"
#include "../../stalker_movement_restriction.h"
#include "../../level_graph.h"

extern const float MIN_SUITABLE_ENEMY_DISTANCE = 3.f;

#ifdef _DEBUG
static int g_advance_search_count		= 0;
static int g_near_cover_search_count	= 0;
static int g_far_cover_search_count		= 0;
#endif

void CAI_Stalker::subscribe_on_best_cover_changed	(const on_best_cover_changed_delegate &delegate)
{
	VERIFY								(m_cover_delegates.end() == std::find(m_cover_delegates.begin(),m_cover_delegates.end(),delegate));
	m_cover_delegates.push_back			(delegate);
}

void CAI_Stalker::unsubscribe_on_best_cover_changed	(const on_best_cover_changed_delegate &delegate)
{
	cover_delegates::iterator			I = std::find(m_cover_delegates.begin(),m_cover_delegates.end(),delegate);
	VERIFY								(I != m_cover_delegates.end());
	m_cover_delegates.erase				(I);
}

void CAI_Stalker::on_best_cover_changed				(const CCoverPoint *new_cover, const CCoverPoint *old_cover)
{
	cover_delegates::const_iterator		I = m_cover_delegates.begin();
	cover_delegates::const_iterator		E = m_cover_delegates.end();
	for ( ; I != E; ++I)
		(*I)							(new_cover,old_cover);
}

const CCoverPoint *CAI_Stalker::find_best_cover		(const Fvector &position_to_cover_from)
{
#ifdef _DEBUG
//	Msg									("* [%6d][%s] search for new cover performed",Device.dwTimeGlobal,*cName());
#endif
#ifdef _DEBUG
	++g_near_cover_search_count;
#endif
	m_ce_best->setup					(position_to_cover_from,MIN_SUITABLE_ENEMY_DISTANCE,170.f,MIN_SUITABLE_ENEMY_DISTANCE);
	const CCoverPoint					*point = ai().cover_manager().best_cover(Position(),10.f,*m_ce_best,CStalkerMovementRestrictor(this,true));
	if (point)
		return							(point);

#ifdef _DEBUG
	++g_far_cover_search_count;
#endif
	m_ce_best->setup					(position_to_cover_from,10.f,170.f,10.f);
	point								= ai().cover_manager().best_cover(Position(),30.f,*m_ce_best,CStalkerMovementRestrictor(this,true));
	return								(point);
}

float CAI_Stalker::best_cover_value					(const Fvector &position_to_cover_from)
{
	m_ce_best->setup					(position_to_cover_from,MIN_SUITABLE_ENEMY_DISTANCE,170.f,MIN_SUITABLE_ENEMY_DISTANCE);
	m_ce_best->initialize				(Position(),true);
	m_ce_best->evaluate					(m_best_cover,CStalkerMovementRestrictor(this,true).weight(m_best_cover));
	return								(m_ce_best->best_value());
}

void CAI_Stalker::best_cover_can_try_advance		()
{
	if (!m_best_cover_actual)
		return;

	if (m_best_cover_advance_cover == m_best_cover)
		return;

	m_best_cover_can_try_advance		= true;
}

void CAI_Stalker::update_best_cover_actuality		(const Fvector &position_to_cover_from)
{
	if (!m_best_cover_actual)
		return;

	if (!m_best_cover)
		return;

	if (m_best_cover->position().distance_to_sqr(position_to_cover_from) < _sqr(MIN_SUITABLE_ENEMY_DISTANCE)) {
		m_best_cover_actual				= false;
#if 0//def _DEBUG
		Msg								("* [%6d][%s] enemy too close",Device.dwTimeGlobal,*cName());
#endif
		return;
	}

	float								cover_value = best_cover_value(position_to_cover_from);
	if (cover_value >= m_best_cover_value + 1.f) {
		m_best_cover_actual				= false;
#if 0//def _DEBUG
		Msg								("* [%6d][%s] cover became too bad",Device.dwTimeGlobal,*cName());
#endif
		return;
	}

//	if (cover_value >= 1.5f*m_best_cover_value) {
//		m_best_cover_actual				= false;
//		Msg								("* [%6d][%s] cover became too bad2",Device.dwTimeGlobal,*cName());
//		return;
//	}

	if (!m_best_cover_can_try_advance)
		return;

	if (m_best_cover_advance_cover == m_best_cover)
		return;

	m_best_cover_advance_cover			= m_best_cover;
	m_best_cover_can_try_advance		= false;

#ifdef _DEBUG
//	Msg									("* [%6d][%s] advance search performed",Device.dwTimeGlobal,*cName());
#endif
#ifdef _DEBUG
	++g_advance_search_count;
#endif
	m_ce_best->setup					(position_to_cover_from,MIN_SUITABLE_ENEMY_DISTANCE,170.f,MIN_SUITABLE_ENEMY_DISTANCE);
	m_best_cover						= ai().cover_manager().best_cover(Position(),10.f,*m_ce_best,CStalkerMovementRestrictor(this,true));
}

const CCoverPoint *CAI_Stalker::best_cover			(const Fvector &position_to_cover_from)
{
	update_best_cover_actuality			(position_to_cover_from);

	if (m_best_cover_actual)
		return							(m_best_cover);

	m_best_cover_actual					= true;

	const CCoverPoint					*best_cover = find_best_cover(position_to_cover_from);
	if (best_cover != m_best_cover) {
		on_best_cover_changed			(best_cover,m_best_cover);
		m_best_cover					= best_cover;
		m_best_cover_advance_cover		= 0;
		m_best_cover_can_try_advance	= false;
	}
	m_best_cover_value					= m_best_cover ? best_cover_value(position_to_cover_from) : flt_max;

	return								(m_best_cover);
}

void CAI_Stalker::on_restrictions_change			()
{
	inherited::on_restrictions_change	();
	m_best_cover_actual					= false;
#ifdef _DEBUG
	Msg									("* [%6d][%s] on_restrictions_change",Device.dwTimeGlobal,*cName());
#endif
}

void CAI_Stalker::on_enemy_change					(const CEntityAlive *enemy)
{
	inherited::on_enemy_change			(enemy);
	m_item_actuality					= false;
	m_best_cover_actual					= false;
#ifdef _DEBUG
//	Msg									("* [%6d][%s] on_enemy_change",Device.dwTimeGlobal,*cName());
#endif
}

void CAI_Stalker::on_danger_location_add			(const CDangerLocation &location)
{
	if (!m_best_cover)
		return;

	if (m_best_cover->position().distance_to_sqr(location.position()) <= _sqr(location.m_radius)) {
#ifdef _DEBUG
//		Msg								("* [%6d][%s] on_danger_add",Device.dwTimeGlobal,*cName());
#endif
		m_best_cover_actual				= false;
	}
}

void CAI_Stalker::on_danger_location_remove			(const CDangerLocation &location)
{
	if (!m_best_cover) {
		if (Position().distance_to_sqr(location.position()) <= _sqr(location.m_radius)) {
#ifdef _DEBUG
//			Msg							("* [%6d][%s] on_danger_remove",Device.dwTimeGlobal,*cName());
#endif
			m_best_cover_actual			= false;
		}

		return;
	}

	if (m_best_cover->position().distance_to_sqr(location.position()) <= _sqr(location.m_radius)) {
#ifdef _DEBUG
//		Msg								("* [%6d][%s] on_danger_remove",Device.dwTimeGlobal,*cName());
#endif
		m_best_cover_actual				= false;
	}
}

void CAI_Stalker::on_cover_blocked					(const CCoverPoint *cover)
{
#ifdef _DEBUG
//	Msg									("* [%6d][%s] cover is blocked",Device.dwTimeGlobal,*cName());
#endif
	m_best_cover_actual					= false;
}

#include "stdafx.h"
#include "monster_home.h"
#include "BaseMonster/base_monster.h"
#include "../../ai_space.h"
#include "../../patrol_path_storage.h"
#include "../../patrol_path.h"
#include "../../level_graph.h"
#include "../../cover_point.h"
#include "monster_cover_manager.h"
#include "../../ai_object_location.h"
#include "../../restricted_object.h"

#ifdef DEBUG
#	include "../../game_graph.h"
void check_path	(const CBaseMonster *monster, const CPatrolPath *path)
{
	VERIFY2			(
		ai().game_graph().vertex(
			path->vertices().begin()->second->data().game_vertex_id()
		)->level_id()
		==
		ai().level_graph().level_id(),
		make_string(
			"invalid patrol path [%s] as home specified for monster [%s]\nmonster is on level %s\npatrol path is on level %s",
			*path->m_name,
			*monster->cName(),
			*ai().game_graph().header().level(
				ai().game_graph().vertex(
					monster->ai_location().game_vertex_id()
				)->level_id()
			).name(),
			*ai().game_graph().header().level(
				ai().game_graph().vertex(
					path->vertices().begin()->second->data().game_vertex_id()
				)->level_id()
			).name()
		)
	);
}
#else // DEBUG
#	define check_path(a,b)
#endif // DEBUG

void CMonsterHome::load(LPCSTR line)
{
	m_path			= 0;
	m_radius_min	= 20.f;
	m_radius_max	= 40.f;

	if (m_object->spawn_ini() && m_object->spawn_ini()->section_exist(line)) {
		m_path			= ai().patrol_paths().path(m_object->spawn_ini()->r_string(line,"path"));
		check_path		(m_object,m_path);
		if (m_object->spawn_ini()->line_exist(line,"radius_min"))
			m_radius_min	= m_object->spawn_ini()->r_float(line,"radius_min");
		if (m_object->spawn_ini()->line_exist(line,"radius_max"))
			m_radius_max	= m_object->spawn_ini()->r_float(line,"radius_max");

		VERIFY3(m_radius_max > m_radius_min, "Error: Wrong home point radius specified for monster ", *m_object->cName());
	}

	m_aggressive = false;
}

void CMonsterHome::setup(LPCSTR path_name, float min_radius, float max_radius, bool aggressive)
{
	m_path			= ai().patrol_paths().path(path_name);
	check_path		(m_object,m_path);
	m_radius_min	= min_radius;
	m_radius_max	= max_radius;

	m_aggressive	= aggressive;
}

u32	CMonsterHome::get_place()
{
	VERIFY	(m_path);
	u32		result = u32(-1);

	//get_random_point
	const CPatrolPath::CVertex *vertex = m_path->vertex(Random.randI(m_path->vertex_count()));
	
	//get_random node
	m_object->control().path_builder().get_node_in_radius(vertex->data().level_vertex_id(), m_radius_min, m_radius_min + (m_radius_max - m_radius_min)/2, 5, result);
	
	// if node was not found - return vertex selected
	if (result == u32(-1)) {
		
		// TODO: find more acceptable decision, than return its level_vertex_id, if !accessible
		if (m_object->control().path_builder().accessible(vertex->data().level_vertex_id()))		
			return vertex->data().level_vertex_id();
		else
			return m_object->ai_location().level_vertex_id();
	}

	return result;
}

u32	CMonsterHome::get_place_in_cover()
{
	VERIFY	(m_path);

	//get_random_point
	const CPatrolPath::CVertex *vertex = m_path->vertex(Random.randI(m_path->vertex_count()));

	// find cover
	const CCoverPoint *point = m_object->CoverMan->find_cover(vertex->data().position(), vertex->data().position(), m_radius_min, m_radius_min + (m_radius_max - m_radius_min)/2);
	if (point) return point->level_vertex_id();

	return u32(-1);
}


bool CMonsterHome::at_home()
{
	return at_home(m_object->Position());
}

bool CMonsterHome::at_home(const Fvector &pos)
{
	if (!m_path) return true;

	// check every point and distance to it
	for (u32 i=0; i<m_path->vertex_count(); i++) {
		const CPatrolPath::CVertex *vertex = m_path->vertex(i);
		float dist = pos.distance_to(ai().level_graph().vertex_position(vertex->data().level_vertex_id()));

		if (dist < m_radius_max) return true;
	}

	return false;
}

void CMonsterHome::remove_home()
{
	m_path			= 0;
	m_aggressive	= false;
}


////////////////////////////////////////////////////////////////////////////
//	Module 		: cover_evaluators_inline.h
//	Created 	: 24.04.2004
//  Modified 	: 24.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Cover evaluators inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorBase
//////////////////////////////////////////////////////////////////////////
IC	CCoverEvaluatorBase::CCoverEvaluatorBase	(CRestrictedObject *object)
{
	m_inertia_time			= 0;
	m_last_update			= 0;
	m_inertia_time			= 0;
	m_best_value			= flt_max;
	m_initialized			= false;
	m_start_position.set	(flt_max,flt_max,flt_max);
	m_selected				= 0;
	m_object				= object;
	m_actuality				= true;
	m_last_radius			= flt_max;
}

IC	const CCoverPoint *CCoverEvaluatorBase::selected	() const
{
	return					(m_selected);
}

IC	void CCoverEvaluatorBase::set_inertia				(u32 inertia_time)
{
	m_inertia_time			= inertia_time;
}

bool CCoverEvaluatorBase::inertia						(float radius)
{
//	m_actuality				= m_actuality && fsimilar(m_last_radius,radius);
//	m_actuality				= m_actuality && ((m_last_radius + EPS_L) >= radius);
	bool					radius_criteria = ((m_last_radius + EPS_L) >= radius);
	bool					time_criteria = (Device.dwTimeGlobal < m_last_update + m_inertia_time);

	m_last_radius			= radius;

	return					(time_criteria && radius_criteria);// && (radius_criteria || m_selected));
}

IC	void CCoverEvaluatorBase::setup						()
{
	m_initialized			= true;
}

IC	void CCoverEvaluatorBase::initialize				(const Fvector &start_position, bool fake_call)
{
	VERIFY					(initialized());
	m_start_position		= start_position;
	m_selected				= 0;
	m_best_value			= 1000.f;
	if (!fake_call)
		m_last_update		= Device.dwTimeGlobal;
}

IC	void CCoverEvaluatorBase::finalize					()
{
	m_initialized			= false;
	m_actuality				= true;
}

IC	bool CCoverEvaluatorBase::initialized				() const
{
	return					(m_initialized);
}

IC	bool CCoverEvaluatorBase::accessible				(const Fvector &position)
{
	return					(m_object ? object().accessible(position) : true);
}

IC	CRestrictedObject &CCoverEvaluatorBase::object		() const
{
	VERIFY					(m_object);
	return					(*m_object);
}

IC	bool CCoverEvaluatorBase::actual					() const
{
	return					(m_actuality);
}

IC	void CCoverEvaluatorBase::invalidate				()
{
	m_last_update			= 0;
}

IC	float CCoverEvaluatorBase::best_value				() const
{
	return					(m_best_value);
}

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorCloseToEnemy
//////////////////////////////////////////////////////////////////////////

IC	CCoverEvaluatorCloseToEnemy::CCoverEvaluatorCloseToEnemy	(CRestrictedObject *object) : inherited(object)
{
	m_enemy_position.set	(flt_max,flt_max,flt_max);
	m_deviation				= flt_max;
	m_min_distance			= flt_max;
	m_max_distance			= flt_max;
	m_best_distance			= flt_max;
	m_current_distance		= flt_max;
}

IC	void CCoverEvaluatorCloseToEnemy::initialize(const Fvector &start_position, bool fake_call)
{
	inherited::initialize	(start_position,fake_call);
	m_current_distance		= m_start_position.distance_to(m_enemy_position);
}

IC	void CCoverEvaluatorCloseToEnemy::setup		(const Fvector &enemy_position, float min_enemy_distance, float	max_enemy_distance, float deviation)
{
	inherited::setup		();
	
//	m_actuality				= m_actuality && m_enemy_position.similar(enemy_position,10.f);
	m_enemy_position		= enemy_position;

	m_actuality				= m_actuality && fsimilar(m_deviation,deviation);
	m_deviation				= deviation;
	
	m_actuality				= m_actuality && fsimilar(m_min_distance,min_enemy_distance);
	m_min_distance			= min_enemy_distance;

	m_actuality				= m_actuality && fsimilar(m_max_distance,max_enemy_distance);
	m_max_distance			= max_enemy_distance;
}

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorAngle
//////////////////////////////////////////////////////////////////////////

IC	CCoverEvaluatorAngle::CCoverEvaluatorAngle	(CRestrictedObject *object) : inherited(object)
{
	m_direction.set		(flt_max,flt_max,flt_max);
	m_best_direction.set(flt_max,flt_max,flt_max);
	m_best_alpha		= flt_max;;
	m_level_vertex_id	= u32(-1);
}

IC	void CCoverEvaluatorAngle::setup		(const Fvector &enemy_position, float min_enemy_distance, float	max_enemy_distance, u32 level_vertex_id)
{
	inherited::setup		(enemy_position,min_enemy_distance,max_enemy_distance);
	m_actuality				= m_actuality && (m_level_vertex_id == level_vertex_id);
	m_level_vertex_id		= level_vertex_id;
}

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorFarFromEnemy
//////////////////////////////////////////////////////////////////////////

IC	CCoverEvaluatorFarFromEnemy::CCoverEvaluatorFarFromEnemy	(CRestrictedObject *object) : inherited(object)
{
}

IC	CCoverEvaluatorBest::CCoverEvaluatorBest					(CRestrictedObject *object) : inherited(object)
{
}

IC	CCoverEvaluatorBestByTime::CCoverEvaluatorBestByTime		(CRestrictedObject *object) : inherited(object)
{
}

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorSafe
//////////////////////////////////////////////////////////////////////////

IC	CCoverEvaluatorSafe::CCoverEvaluatorSafe	(CRestrictedObject *object) : inherited(object)
{
	m_min_distance			= flt_max;
}

IC	void CCoverEvaluatorSafe::setup		(float min_distance)
{
	inherited::setup		();
	m_actuality				= m_actuality && fsimilar(m_min_distance,min_distance);
	m_min_distance			= min_distance;
}

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorRandomGame
//////////////////////////////////////////////////////////////////////////

IC	CCoverEvaluatorRandomGame::CCoverEvaluatorRandomGame	(CRestrictedObject *object) : inherited(object)
{
	m_game_vertex_id		= GameGraph::_GRAPH_ID(-1);
	m_start_position.set	(flt_max,flt_max,flt_max);
	m_max_distance_sqr		= flt_max;
}

//////////////////////////////////////////////////////////////////////////
// CCoverEvaluatorAmbush
//////////////////////////////////////////////////////////////////////////

IC	CCoverEvaluatorAmbush::CCoverEvaluatorAmbush			(CRestrictedObject *object) :
	inherited	(object)
{
	m_my_position.set		(flt_max,flt_max,flt_max);
	m_enemy_position.set	(flt_max,flt_max,flt_max);
	m_min_enemy_distance	= flt_max;;
}

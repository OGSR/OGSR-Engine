////////////////////////////////////////////////////////////////////////////
//	Module 		: cover_manager.cpp
//	Created 	: 24.03.2004
//  Modified 	: 24.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Cover manager class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "level_graph.h"
#include "cover_manager.h"
#include "ai_space.h"
#include "cover_point.h"
#include "object_broker.h"

#define MIN_COVER_VALUE 16

CCoverManager::CCoverManager				()
{
	m_covers				= 0;
}

CCoverManager::~CCoverManager				()
{
	clear					();
	xr_delete				(m_covers);
}

IC	bool CCoverManager::edge_vertex		(u32 index)
{
	CLevelGraph::CVertex	*v = ai().level_graph().vertex(index);
	return					(
		(!ai().level_graph().valid_vertex_id(v->link(0)) && (v->cover(0) < MIN_COVER_VALUE)) ||
		(!ai().level_graph().valid_vertex_id(v->link(1)) && (v->cover(1) < MIN_COVER_VALUE)) ||
		(!ai().level_graph().valid_vertex_id(v->link(2)) && (v->cover(2) < MIN_COVER_VALUE)) ||
		(!ai().level_graph().valid_vertex_id(v->link(3)) && (v->cover(3) < MIN_COVER_VALUE))
	);
}

IC	bool CCoverManager::cover			(CLevelGraph::CVertex *v, u32 index0, u32 index1)
{
	return					(
		ai().level_graph().valid_vertex_id(v->link(index0)) &&
		ai().level_graph().valid_vertex_id(ai().level_graph().vertex(v->link(index0))->link(index1)) &&
		m_temp[ai().level_graph().vertex(v->link(index0))->link(index1)]
	);
}

IC	bool CCoverManager::critical_point	(CLevelGraph::CVertex *v, u32 index, u32 index0, u32 index1)
{
	return					(
		!ai().level_graph().valid_vertex_id(v->link(index)) &&
		(
			!ai().level_graph().valid_vertex_id(v->link(index0)) || 
			!ai().level_graph().valid_vertex_id(v->link(index1)) ||
			cover(v,index0,index) || 
			cover(v,index1,index)
		)
	);
}

IC	bool CCoverManager::critical_cover	(u32 index)
{
	CLevelGraph::CVertex	*v = ai().level_graph().vertex(index);
	return					(
		critical_point(v,0,1,3) || 
		critical_point(v,2,1,3) || 
		critical_point(v,1,0,2) || 
		critical_point(v,3,0,2)
	);
}

void CCoverManager::compute_static_cover	()
{
	clear					();
	xr_delete				(m_covers);
	m_covers				= xr_new<CPointQuadTree>(ai().level_graph().header().box(),ai().level_graph().header().cell_size()*.5f,4*65536,2*65536);
	m_temp.resize			(ai().level_graph().header().vertex_count());

	for (u32 i=0, n = ai().level_graph().header().vertex_count(); i<n; ++i)
		if (ai().level_graph().vertex(i)->cover(0) + ai().level_graph().vertex(i)->cover(1) + ai().level_graph().vertex(i)->cover(2) + ai().level_graph().vertex(i)->cover(3))
			m_temp[i]		= edge_vertex(i);
		else
			m_temp[i]		= false;

	for (u32 i=0; i<n; ++i)
		if (m_temp[i] && critical_cover(i))
			m_covers->insert(xr_new<CCoverPoint>(ai().level_graph().vertex_position(ai().level_graph().vertex(i)),i));
}

void CCoverManager::clear					()
{
	if (!get_covers())
		return;

	covers().all			(m_nearest);
	delete_data				(m_nearest);
	m_covers->clear			();
}

////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_object_location.h
//	Created 	: 27.11.2003
//  Modified 	: 27.11.2003
//	Author		: Dmitriy Iassenev
//	Description : AI object location
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "game_graph_space.h"
#include "ai_object_location.h"
#include "ai_space.h"
#include "game_graph.h"
#include "level_graph.h"

namespace LevelGraph {
	class CVertex;
};

class CAI_ObjectLocation {
private:
	u32										m_level_vertex_id;
	GameGraph::_GRAPH_ID					m_game_vertex_id;

public:
	IC										CAI_ObjectLocation	();
	IC			void						init				()
	{
		if (ai().get_level_graph())
			ai().level_graph().set_invalid_vertex(m_level_vertex_id);
		else
			m_level_vertex_id = u32(-1);

		if (ai().get_game_graph())
			ai().game_graph().set_invalid_vertex(m_game_vertex_id);
		else
			m_game_vertex_id = GameGraph::_GRAPH_ID(-1);
	}
	IC	virtual	void						reinit				();
	IC			void						game_vertex			(const GameGraph::CVertex	*game_vertex)
	{
		VERIFY(ai().game_graph().valid_vertex_id(ai().game_graph().vertex_id(game_vertex)));
		m_game_vertex_id = ai().game_graph().vertex_id(game_vertex);
	}
	IC			void						game_vertex			(const GameGraph::_GRAPH_ID	game_vertex_id)
	{
		VERIFY(ai().game_graph().valid_vertex_id(game_vertex_id));
		m_game_vertex_id = game_vertex_id;
	}
	IC			const GameGraph::CVertex	*game_vertex		() const
	{
		VERIFY(ai().game_graph().valid_vertex_id(m_game_vertex_id));
		return				(ai().game_graph().vertex(m_game_vertex_id));
	}
	IC			const GameGraph::_GRAPH_ID	game_vertex_id		() const;
	IC			void						level_vertex		(const LevelGraph::CVertex	*level_vertex)
	{
		VERIFY(ai().level_graph().valid_vertex_id(ai().level_graph().vertex_id(level_vertex)));
		m_level_vertex_id = ai().level_graph().vertex_id(level_vertex);
	}
	IC			void						level_vertex		(const u32					level_vertex_id)
	{
		VERIFY(ai().level_graph().valid_vertex_id(level_vertex_id));
		m_level_vertex_id = level_vertex_id;
	}
	IC			const LevelGraph::CVertex	*level_vertex		() const
	{
		VERIFY(ai().level_graph().valid_vertex_id(m_level_vertex_id));
		return				(ai().level_graph().vertex(m_level_vertex_id));
	}

	IC			const u32					level_vertex_id		() const;
};

#include "ai_object_location_inline.h"
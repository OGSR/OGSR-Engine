////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_object_location.h
//	Created 	: 27.11.2003
//  Modified 	: 27.11.2003
//	Author		: Dmitriy Iassenev
//	Description : AI object location
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "game_graph_space.h"

namespace LevelGraph {
	class CVertex;
};

class CAI_ObjectLocation {
private:
	u32										m_level_vertex_id;
	GameGraph::_GRAPH_ID					m_game_vertex_id;

public:
	IC										CAI_ObjectLocation	();
	IC			void						init				();
	IC	virtual	void						reinit				();
	IC			void						game_vertex			(const GameGraph::CVertex	*game_vertex);
	IC			void						game_vertex			(const GameGraph::_GRAPH_ID	game_vertex_id);
	IC			const GameGraph::CVertex	*game_vertex		() const;
	IC			const GameGraph::_GRAPH_ID	game_vertex_id		() const;
	IC			void						level_vertex		(const LevelGraph::CVertex	*level_vertex);
	IC			void						level_vertex		(const u32					level_vertex_id);
	IC			const LevelGraph::CVertex	*level_vertex		() const;
	IC			const u32					level_vertex_id		() const;
};

#include "ai_object_location_inline.h"
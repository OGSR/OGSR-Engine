////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_space_inline.h
//	Created 	: 12.11.2003
//  Modified 	: 25.11.2003
//	Author		: Dmitriy Iassenev
//	Description : AI space class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CGameGraph					&CAI_Space::game_graph				() const
{
	VERIFY					(m_game_graph);
	return					(*m_game_graph);
}

IC	CGameGraph					*CAI_Space::get_game_graph			() const
{
	return					(m_game_graph);
}

IC	CLevelGraph		&CAI_Space::level_graph							() const
{
	VERIFY					(m_level_graph);
	return					(*m_level_graph);
}

IC	const CLevelGraph	*CAI_Space::get_level_graph					() const
{
	return					(m_level_graph);
}

#ifndef PRIQUEL
IC	const CGameLevelCrossTable	&CAI_Space::cross_table				() const
{
	VERIFY					(m_cross_table);
	return					(*m_cross_table);
}

IC	const CGameLevelCrossTable	*CAI_Space::get_cross_table			() const
{
	return					(m_cross_table);
}
#endif // PRIQUEL

IC	CEF_Storage					&CAI_Space::ef_storage				() const
{
	VERIFY					(m_ef_storage);
	return					(*m_ef_storage);
}

IC	CGraphEngine				&CAI_Space::graph_engine			() const
{
	VERIFY					(m_graph_engine);
	return					(*m_graph_engine);
}

IC	const CALifeSimulator		&CAI_Space::alife					() const
{
	VERIFY					(m_alife_simulator);
	return					(*m_alife_simulator);
}

IC	const CALifeSimulator		*CAI_Space::get_alife				() const
{
	return					(m_alife_simulator);
}

IC	const CCoverManager			&CAI_Space::cover_manager			() const
{
	VERIFY					(m_cover_manager);
	return					(*m_cover_manager);
}

IC	CScriptEngine				&CAI_Space::script_engine			() const
{
	VERIFY					(m_script_engine);
	return					(*m_script_engine);
}

IC	const CPatrolPathStorage &CAI_Space::patrol_paths				() const
{
	VERIFY					(m_patrol_path_storage);
	return					(*m_patrol_path_storage);
}

IC	CAI_Space &ai													()
{
	if (!g_ai_space) {
		g_ai_space			= xr_new<CAI_Space>();
		g_ai_space->init	();
	}
	return					(*g_ai_space);
}

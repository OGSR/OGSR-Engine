////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_engine_inline.h
//	Created 	: 21.03.2002
//  Modified 	: 03.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Graph engine inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC CGraphEngine::CGraphEngine(u32 max_vertex_count)
{
    m_algorithm = xr_new<CAlgorithm>(max_vertex_count);
    m_algorithm->data_storage().set_min_bucket_value(static_cast<GraphEngineSpace::_dist_type>(0));
    m_algorithm->data_storage().set_max_bucket_value(static_cast<GraphEngineSpace::_dist_type>(2000));
}

IC CGraphEngine::~CGraphEngine()
{
    xr_delete(m_algorithm);
}

// level_graph

template <typename _Graph, typename _Parameters>
bool CGraphEngine::search(const _Graph& graph, const GraphEngineSpace::_index_type& start_node, const GraphEngineSpace::_index_type& dest_node, xr_vector<GraphEngineSpace::_index_type>* node_path, const _Parameters& parameters)
{
    Device.Statistic->AI_Path.Begin();
    START_PROFILE("graph_engine")
    START_PROFILE("graph_engine/level_graph")

    using CPathManagerGeneric = CPathManager<_Graph, CAlgorithm::CDataStorage, _Parameters, GraphEngineSpace::_dist_type, GraphEngineSpace::_index_type, GraphEngineSpace::_iteration_type>;

    CPathManagerGeneric path_manager;

    path_manager.setup(&graph, &m_algorithm->data_storage(), node_path, start_node, dest_node, parameters);

    const bool successfull = m_algorithm->find(path_manager);

    Device.Statistic->AI_Path.End();

    return (successfull);
    STOP_PROFILE
    STOP_PROFILE
}

// game_graph search

template <typename _Graph, typename _Parameters>
bool CGraphEngine::search(const _Graph& graph, const GraphEngineSpace::_index_type& start_node, const GraphEngineSpace::_index_type& dest_node, xr_vector<GraphEngineSpace::_index_type>* node_path, _Parameters& parameters)
{
    Device.Statistic->AI_Path.Begin();
    START_PROFILE("graph_engine")
    START_PROFILE("graph_engine/game_graph")

    using CPathManagerGeneric = CPathManager<_Graph, CAlgorithm::CDataStorage, _Parameters, GraphEngineSpace::_dist_type, GraphEngineSpace::_index_type, GraphEngineSpace::_iteration_type>;

    CPathManagerGeneric path_manager;

    path_manager.setup(&graph, &m_algorithm->data_storage(), node_path, start_node, dest_node, parameters);

    const bool successfull = m_algorithm->find(path_manager);

    Device.Statistic->AI_Path.End();

    return (successfull);
    STOP_PROFILE
    STOP_PROFILE
}

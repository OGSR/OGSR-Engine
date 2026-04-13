////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_engine.h
//	Created 	: 21.03.2002
//  Modified 	: 26.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Graph engine
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "a_star.h"
#include "edge_path.h"
#include "vertex_manager_fixed.h"
#include "vertex_allocator_fixed.h"
#include "data_storage_bucket_list.h"
#include "path_manager.h"
#include "graph_engine_space.h"
#include "vertex_manager_hash_fixed.h"
#include "data_storage_binary_heap.h"

#include "problem_solver.h"
#include "operator_condition.h"
#include "condition_state.h"
#include "operator_abstract.h"

namespace hash_fixed_vertex_manager
{
inline u32 to_u32(const GraphEngineSpace::CWorldState& other) { return other.hash_value(); }
} // namespace hash_fixed_vertex_manager

class CGraphEngine
{
public:
    // common algorithm
    using CPriorityQueue = CDataStorageBucketList<u32, u32, 8 * 1024, false>;
    using CVertexManager = CVertexManagerFixed<u32, u32, 8>;
    static const size_t VertexAllocatorReserve = 64 * 1024;
    using CVertexAllocator = CVertexAllocatorFixed<VertexAllocatorReserve>;
    using AlgorithmStorage = CVertexPath<true>;
    using CAlgorithm = CAStar<GraphEngineSpace::_dist_type, CPriorityQueue, CVertexManager, CVertexAllocator, true, AlgorithmStorage>;

    CAlgorithm* m_algorithm;

public:
    IC CGraphEngine(u32 max_vertex_count);
    virtual ~CGraphEngine();

    template <typename _Graph, typename _Parameters>
    bool search(const _Graph& graph, const GraphEngineSpace::_index_type& start_node, const GraphEngineSpace::_index_type& dest_node, xr_vector<GraphEngineSpace::_index_type>* node_path, const _Parameters& parameters);

    template <typename _Graph, typename _Parameters>
    bool search(const _Graph& graph, const GraphEngineSpace::_index_type& start_node, const GraphEngineSpace::_index_type& dest_node, xr_vector<GraphEngineSpace::_index_type>* node_path, _Parameters& parameters);
};

#include "graph_engine_inline.h"
////////////////////////////////////////////////////////////////////////////
//	Module 		: a_star.h
//	Created 	: 21.03.2002
//  Modified 	: 02.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Implementation of the A* (a-star) algorithm
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "vertex_path.h"
#include "data_storage_constructor.h"
#include "dijkstra.h"

template <typename TDistance, typename TVertexData>
struct AStarVertexData
{
    template <typename TCompoundVertex>
    struct VertexData : TVertexData::template VertexData<TCompoundVertex>
    {
        using Distance = TDistance;

        Distance _g;
        Distance _h;

        Distance& g() { return _g; }
        Distance& h() { return _h; }
    };
};

template <typename TDistance, typename TPriorityQueue, typename TVertexManager, typename TVertexAllocator, bool EuclidianHeuristics = true,
          typename TPathBuilder = CVertexPath<EuclidianHeuristics>, typename TIteration = u32, typename TVertexData = EmptyVertexData>
class CAStar : public CDijkstra<TDistance, TPriorityQueue, TVertexManager, TVertexAllocator, EuclidianHeuristics, TPathBuilder, TIteration, AStarVertexData<TDistance, TVertexData>>
{
protected:
    using Inherited =
        CDijkstra<TDistance, TPriorityQueue, TVertexManager, TVertexAllocator, EuclidianHeuristics, TPathBuilder, TIteration, AStarVertexData<TDistance, TVertexData>>;
    using Vertex = Inherited::Vertex;
    using Distance = Vertex::Distance;
    using Index = Vertex::Index;

protected:
    template <typename TPathManager>
    void initialize(TPathManager& path_manager);
    template <typename TPathManager>
    bool step(TPathManager& path_manager);

public:
    CAStar(u32 max_vertex_count);
    ~CAStar() override;
    template <typename TPathManager>
    bool find(TPathManager& path_manager);
};

#include "a_star_inline.h"

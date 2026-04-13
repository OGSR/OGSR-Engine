////////////////////////////////////////////////////////////////////////////
//	Module 		: vertex_path.h
//	Created 	: 21.03.2002
//  Modified 	: 02.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Vertex path class
////////////////////////////////////////////////////////////////////////////

#pragma once

template <bool EuclidianHeuristics = true>
struct CVertexPath
{
#pragma pack(push, 1)
    template <typename TCompoundVertex>
    struct VertexData
    {};
#pragma pack(pop)

    template <typename TCompoundVertex>
    class CDataStorage
    {
    public:
        using Vertex = TCompoundVertex;
        using Index = Vertex::Index;

    public:
        CDataStorage(u32 vertex_count);
        virtual ~CDataStorage();
        void init();
        void assign_parent(Vertex& neighbour, Vertex* parent);
        template <typename T>
        void assign_parent(Vertex& neighbour, Vertex* parent, const T&);
        void update_successors(Vertex& neighbour);
        void get_node_path(xr_vector<Index>& path, Vertex* best);
    };
};

#include "vertex_path_inline.h"

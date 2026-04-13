////////////////////////////////////////////////////////////////////////////
//	Module 		: data_storage_binary_heap.h
//	Created 	: 21.03.2002
//  Modified 	: 26.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Binary heap data storage
////////////////////////////////////////////////////////////////////////////

#pragma once

struct CDataStorageBinaryHeap
{
    template <typename TCompoundVertex>
    struct VertexData
    {};

    template <typename TManagerDataStorage>
    class CDataStorage : public TManagerDataStorage
    {
    public:
        using Inherited = TManagerDataStorage;
        using Vertex = Inherited::Vertex;
        using Distance = Vertex::Distance;
        using Index = Vertex::Index;

        struct VertexPredicate
        {
            bool operator()(Vertex* a, Vertex* b) { return a->f() > b->f(); }
        };

    protected:
        Vertex** m_heap;
        Vertex** m_heap_head{};
        Vertex** m_heap_tail{};

    public:
        CDataStorage(u32 vertex_count);
        virtual ~CDataStorage();
        void init();
        bool is_opened_empty() const;
        void add_opened(Vertex& vertex);
        void decrease_opened(Vertex& vertex, Distance value);
        void remove_best_opened();
        void add_best_closed();
        Vertex& get_best() const;
    };
};

#include "data_storage_binary_heap_inline.h"

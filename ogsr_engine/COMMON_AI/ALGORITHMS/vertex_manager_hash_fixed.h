////////////////////////////////////////////////////////////////////////////
//  Module      : vertex_manager_hash_fixed.h
//  Created     : 21.03.2002
//  Modified    : 05.03.2004
//  Author      : Dmitriy Iassenev
//  Description : Hash fixed vertex manager
////////////////////////////////////////////////////////////////////////////

#pragma once

template <typename TPathId, typename TIndex, u32 HashSize, u32 FixSize>
struct CVertexManagerHashFixed
{
    template <typename TCompoundVertex>
    struct VertexData
    {
        using Index = TIndex;

        Index _index;
        bool _opened;

        const Index& index() const { return _index; }
        Index& index() { return _index; }
        bool& opened() { return _opened; }
        bool opened() const { return _opened; }
    };

    template <typename TPathBuilder, typename TVertexAllocator, typename TCompoundVertex>
    class CDataStorage : public TPathBuilder::template CDataStorage<TCompoundVertex>, public TVertexAllocator::template CDataStorage<TCompoundVertex>
    {
    public:
        using CDataStorageBase = TPathBuilder::template CDataStorage<TCompoundVertex>;
        using CDataStorageAllocator = TVertexAllocator::template CDataStorage<TCompoundVertex>;
        using Vertex = TCompoundVertex;
        using Index = TIndex;
        using PathId = TPathId;

#pragma pack(push, 1)
        struct IndexVertex
        {
            Vertex* m_vertex;
            IndexVertex* m_next;
            IndexVertex* m_prev;
            u32 m_hash;
            PathId m_path_id;
        };
#pragma pack(pop)

    protected:
        PathId m_current_path_id;
        IndexVertex* m_vertices;
        IndexVertex** m_hash;
        u32 m_vertex_count{};

        static constexpr auto IndexVertexHashSize = HashSize * sizeof(IndexVertex*);
        static constexpr auto IndexVertexFixSize = FixSize * sizeof(IndexVertex);

    public:
        CDataStorage(u32 vertex_count);
        virtual ~CDataStorage();
        void init();
        bool is_opened(const Vertex& vertex) const;
        bool is_visited(const Index& vertex_id) const;
        bool is_closed(const Vertex& vertex) const;
        Vertex& get_node(const Index& vertex_id) const;
        Vertex& create_vertex(Vertex& vertex, const Index& vertex_id);
        void add_opened(Vertex& vertex);
        void add_closed(Vertex& vertex);
        PathId current_path_id() const;
        u32 hash_index(const Index& vertex_id) const;
    };
};

#include "vertex_manager_hash_fixed_inline.h"

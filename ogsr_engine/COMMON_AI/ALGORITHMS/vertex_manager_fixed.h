////////////////////////////////////////////////////////////////////////////
//  Module      : vertex_manager_fixed.h
//  Created     : 21.03.2002
//  Modified    : 01.03.2004
//  Author      : Dmitriy Iassenev
//  Description : Fixed vertex manager
////////////////////////////////////////////////////////////////////////////

#pragma once

template <typename TPathId, typename TIndex, u8 Mask>
struct CVertexManagerFixed
{
    template <typename TCompoundVertex>
    struct VertexData
    {
        using Index = TIndex;
        Index _index;
        bool _opened;

        Index index() const { return _index; }
        Index opened() const { return _opened; }
    };

    template <typename TPathBuilder, typename TVertexAllocator, typename TCompoundVertex>
    class CDataStorage : public TPathBuilder::template CDataStorage<TCompoundVertex>, public TVertexAllocator::template CDataStorage<TCompoundVertex>
    {
    public:
        using CDataStorageBase = TPathBuilder::template CDataStorage<TCompoundVertex>;
        using CDataStorageAllocator = TVertexAllocator::template CDataStorage<TCompoundVertex>;

#pragma pack(push, 1)
        struct IndexVertex
        {
            TPathId m_path_id;
            TCompoundVertex* m_vertex;
        };
#pragma pack(pop)

    protected:
        TPathId m_current_path_id;
        u32 m_max_node_count;
        IndexVertex* m_indexes;

    public:
        CDataStorage(u32 vertex_count);
        virtual ~CDataStorage();
        void init();
        bool is_opened(const TCompoundVertex& vertex) const;
        bool is_visited(const TIndex& vertex_id) const;
        bool is_closed(const TCompoundVertex& vertex) const;
        TCompoundVertex& get_node(const TIndex& vertex_id) const;
        TCompoundVertex& create_vertex(TCompoundVertex& vertex, const TIndex& vertex_id);
        void add_opened(TCompoundVertex& vertex);
        void add_closed(TCompoundVertex& vertex);
        TPathId current_path_id() const;
    };
};

#include "vertex_manager_fixed_inline.h"

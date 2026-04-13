////////////////////////////////////////////////////////////////////////////
//	Module 		: data_storage_bucket_list.h
//	Created 	: 21.03.2002
//  Modified 	: 26.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Bucket list data storage
////////////////////////////////////////////////////////////////////////////

#pragma once

template <typename TPathId, typename TBucketId, u32 BucketCount, bool ClearBuckets>
struct CDataStorageBucketList
{
    template <typename TCompoundVertex>
    struct VertexData
    {
        TCompoundVertex* _next;
        TCompoundVertex* _prev;
        TPathId m_path_id;
        TBucketId m_bucket_id;
        TCompoundVertex*& next() { return _next; }
        TCompoundVertex*& prev() { return _prev; }
    };

    template <typename TManagerDataStorage>
    class CDataStorage : public TManagerDataStorage
    {
    protected:
        TManagerDataStorage::Vertex::Distance m_max_distance;
        TManagerDataStorage::Vertex m_list_data[2];
        TManagerDataStorage::Vertex* m_list_head{};
        TManagerDataStorage::Vertex* m_list_tail{};
        TManagerDataStorage::Vertex::Distance m_min_bucket_value;
        TManagerDataStorage::Vertex::Distance m_max_bucket_value;
        TManagerDataStorage::Vertex* m_buckets[BucketCount];
        u32 m_min_bucket_id;

        static constexpr auto BucketSize = BucketCount * sizeof(typename  TManagerDataStorage::Vertex*);

    public:
        CDataStorage(u32 vertex_count);
        virtual ~CDataStorage();
        void init();
        void add_best_closed();
        bool is_opened_empty();
        u32 compute_bucket_id(TManagerDataStorage::Vertex& vertex) const;
        void verify_buckets() const;
        void add_to_bucket(TManagerDataStorage::Vertex& vertex, u32 bucket_id);
        void add_opened(TManagerDataStorage::Vertex& vertex);
        void decrease_opened(TManagerDataStorage::Vertex& vertex, TManagerDataStorage::Vertex::Distance value);
        void remove_best_opened();
        TManagerDataStorage::Vertex& get_best();
        void set_min_bucket_value(TManagerDataStorage::Vertex::Distance min_bucket_value);
        void set_max_bucket_value(TManagerDataStorage::Vertex::Distance max_bucket_value);
    };
};

#include "data_storage_bucket_list_inline.h"

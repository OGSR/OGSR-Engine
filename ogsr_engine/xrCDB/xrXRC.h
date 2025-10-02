// xrXRC.h: interface for the xrXRC class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "xrCDB.h"

extern XRCDB_API CStatTimer* cdb_clRAY; // total: ray-testing
extern XRCDB_API CStatTimer* cdb_clBOX; // total: box query
extern XRCDB_API CStatTimer* cdb_clFRUSTUM; // total: frustum query

class XRCDB_API xrXRC
{
public:
    CDB::COLLIDER* collider();

public:
    IC void ray_query(u32 options, const CDB::MODEL* m_def, const Fvector& r_start, const Fvector& r_dir, float r_range = 10000.f)
    {
        cdb_clRAY->Begin();
        collider()->ray_query(options, m_def, r_start, r_dir, r_range);
        cdb_clRAY->End();
    }

    IC void box_query(u32 options, const CDB::MODEL* m_def, const Fvector& b_center, const Fvector& b_dim)
    {
        cdb_clBOX->Begin();
        collider()->box_query(options, m_def, b_center, b_dim);
        cdb_clBOX->End();
    }

    IC void frustum_query(u32 options, const CDB::MODEL* m_def, const CFrustum& F)
    {
        cdb_clFRUSTUM->Begin();
        collider()->frustum_query(options, m_def, F);
        cdb_clFRUSTUM->End();
    }

    IC CDB::RESULT* r_begin() { return collider()->r_begin(); }
    IC CDB::RESULT* r_end() { return collider()->r_end(); }
    IC void r_free() { collider()->r_free(); }
    IC size_t r_count() { return collider()->r_count(); }
    IC void r_clear() { collider()->r_clear(); }
    IC void r_clear_compact() { collider()->r_clear_compact(); }

    xrXRC();
    ~xrXRC();
};
XRCDB_API extern xrXRC XRC;

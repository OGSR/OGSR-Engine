#include "stdafx.h"
#include "xr_area.h"
#include "xr_collide_form.h"
#include "xr_object.h"
#include "../xrCDB/cl_intersect.h"

#include "igame_level.h"
#include "x_ray.h"
#include "GameFont.h"

#ifdef DEBUG
#include <d3d9.h>
#endif

using namespace collide;

//--------------------------------------------------------------------------------
// RayTest - Occluded/No
//--------------------------------------------------------------------------------
BOOL CObjectSpace::RayTest(const Fvector& start, const Fvector& dir, float range, collide::rq_target tgt, collide::ray_cache* cache, const CObject* ignore_object)
{
    BOOL _ret = _RayTest(start, dir, range, tgt, cache, ignore_object);
    return _ret;
}
BOOL CObjectSpace::_RayTest(const Fvector& start, const Fvector& dir, float range, collide::rq_target tgt, collide::ray_cache* cache, const CObject* ignore_object)
{
    ZoneScoped;

    VERIFY(_abs(dir.magnitude() - 1) < EPS);

    const collide::ray_defs Q(start, dir, range, CDB::OPT_ONLYFIRST, tgt);

    // dynamic test
    if (tgt & rqtDyn)
    {
        collide::rq_results r_temp;
        xr_vector<ISpatial*> r_spatial;
        u32 d_flags = STYPE_COLLIDEABLE | ((tgt & rqtObstacle) ? STYPE_OBSTACLE : 0) | ((tgt & rqtShape) ? STYPE_SHAPE : 0);
        // traverse object database
        g_SpatialSpace->q_ray(r_spatial, 0, d_flags, start, dir, range);
        // Determine visibility for dynamic part of scene
        for (const auto spatial : r_spatial)
        {
            const CObject* collidable = spatial->dcast_CObject();
            if (collidable && collidable != ignore_object && collidable->collidable.model)
            {
                ECollisionFormType tp = collidable->collidable.model->Type();
                if ((tgt & (rqtObject | rqtObstacle)) && (tp == cftObject) && collidable->collidable.model->_RayQuery(Q, r_temp))
                    return TRUE;
                if ((tgt & rqtShape) && (tp == cftShape) && collidable->collidable.model->_RayQuery(Q, r_temp))
                    return TRUE;
            }
        }
    }
    // static test
    if (tgt & rqtStatic)
    {
        xrXRC xrc;

        // If we get here - test static model
        if (cache)
        {
            // 0. similar query???
            if (cache->similar(start, dir, range))
            {
                return cache->result;
            }

            // 1. Check cached polygon
            float _u, _v, _range;
            if (CDB::TestRayTri(start, dir, cache->verts, _u, _v, _range, false))
            {
                if (_range > 0 && _range < range)
                    return TRUE;
            }

            // 2. Polygon doesn't pick - real database query
            xrc.ray_query(CDB::OPT_ONLYFIRST, &Static, start, dir, range);
            if (0 == xrc.r_count())
            {
                cache->set(start, dir, range, FALSE);
                return FALSE;
            }

            // cache polygon
            cache->set(start, dir, range, TRUE);
            CDB::RESULT* R = xrc.r_begin();
            CDB::TRI& T = Static.get_tris()[R->id];
            Fvector* V = Static.get_verts();
            cache->verts[0].set(V[T.verts[0]]);
            cache->verts[1].set(V[T.verts[1]]);
            cache->verts[2].set(V[T.verts[2]]);
            return TRUE;
        }

        xrc.ray_query(CDB::OPT_ONLYFIRST, &Static, start, dir, range);
        return xrc.r_count();
    }
    return FALSE;
}

//--------------------------------------------------------------------------------
// RayPick
//--------------------------------------------------------------------------------
BOOL CObjectSpace::RayPick(const Fvector& start, const Fvector& dir, float range, rq_target tgt, rq_result& R, const CObject* ignore_object) const
{
    BOOL _res = _RayPick(start, dir, range, tgt, R, ignore_object);
    return _res;
}
BOOL CObjectSpace::_RayPick(const Fvector& start, const Fvector& dir, float range, rq_target tgt, rq_result& R, const CObject* ignore_object) const
{
    ZoneScoped;

    R.O = nullptr;
    R.range = range;
    R.element = -1;
    // static test
    if (tgt & rqtStatic)
    {
        xrXRC xrc;
        xrc.ray_query(CDB::OPT_ONLYNEAREST | CDB::OPT_CULL, &Static, start, dir, range);
        if (xrc.r_count())
            R.set_if_less(xrc.r_begin());
    }
    // dynamic test
    if (tgt & rqtDyn)
    {
        xr_vector<ISpatial*> r_spatial;
        collide::rq_results r_temp;
        collide::ray_defs Q(start, dir, R.range, CDB::OPT_ONLYNEAREST | CDB::OPT_CULL, tgt);
        // traverse object database
        u32 d_flags = STYPE_COLLIDEABLE | ((tgt & rqtObstacle) ? STYPE_OBSTACLE : 0) | ((tgt & rqtShape) ? STYPE_SHAPE : 0);
        g_SpatialSpace->q_ray(r_spatial, 0, d_flags, start, dir, range);
        // Determine visibility for dynamic part of scene
        for (const auto spatial : r_spatial)
        {
            const CObject* collidable = spatial->dcast_CObject();
            if (!collidable || collidable == ignore_object || !collidable->collidable.model)
                continue;

            ECollisionFormType tp = collidable->collidable.model->Type();
            if (((tgt & (rqtObject | rqtObstacle)) && (tp == cftObject)) || ((tgt & rqtShape) && (tp == cftShape)))
            {
                Q.range = R.range;
                if (collidable->collidable.model->_RayQuery(Q, r_temp))
                {
                    R.set_if_less(r_temp.r_begin());
                }
            }
        }
    }
    return (R.element >= 0);
}

//--------------------------------------------------------------------------------
// RayQuery
//--------------------------------------------------------------------------------
BOOL CObjectSpace::RayQuery(collide::rq_results& dest, const collide::ray_defs& R, collide::rq_callback* CB, LPVOID user_data, collide::test_callback* tb, const CObject* ignore_object) const
{
    BOOL _res = _RayQuery(dest, R, CB, user_data, tb, ignore_object);
    return (_res);
}
BOOL CObjectSpace::_RayQuery(collide::rq_results& r_dest, const collide::ray_defs& R, collide::rq_callback* CB, LPVOID user_data, collide::test_callback* tb, const CObject* ignore_object) const
{
    ZoneScoped;

    ZoneValue(R.range);

    // initialize query
    r_dest.r_clear();
    collide::rq_results r_temp;

    rq_target s_mask = rqtStatic;
    rq_target d_mask = rq_target(((R.tgt & rqtObject) ? rqtObject : rqtNone) | ((R.tgt & rqtObstacle) ? rqtObstacle : rqtNone) | ((R.tgt & rqtShape) ? rqtShape : rqtNone));

    // Test static
    if (R.tgt & s_mask)
    {
        xrXRC xrc;
        xrc.ray_query(R.flags, &Static, R.start, R.dir, R.range);
        if (xrc.r_count())
        {
            CDB::RESULT* _I = xrc.r_begin();
            CDB::RESULT* _E = xrc.r_end();
            for (; _I != _E; _I++)
                r_temp.append_result(rq_result().set(nullptr, _I->range, _I->id));
        }
    }
    // Test dynamic
    if (R.tgt & d_mask)
    {
        xr_vector<ISpatial*> r_spatial;
        // Traverse object database
        const u32 d_flags = STYPE_COLLIDEABLE | ((R.tgt & rqtObstacle) ? STYPE_OBSTACLE : 0) | ((R.tgt & rqtShape) ? STYPE_SHAPE : 0);
        g_SpatialSpace->q_ray(r_spatial, 0, d_flags, R.start, R.dir, R.range);
        for (const auto& o_it : r_spatial)
        {
            CObject* collidable = o_it->dcast_CObject();
            if (!collidable || collidable == ignore_object || !collidable->collidable.model)
                continue;

            ICollisionForm* cform = collidable->collidable.model;
            ECollisionFormType tp = collidable->collidable.model->Type();
            if (((R.tgt & (rqtObject | rqtObstacle)) && (tp == cftObject)) || ((R.tgt & rqtShape) && (tp == cftShape)))
            {
                if (tb && !tb(R, collidable, user_data))
                    continue;
                cform->_RayQuery(R, r_temp);
            }
        }
    }

    if (r_temp.r_count())
    {
        r_temp.r_sort();
        collide::rq_result* I = r_temp.r_begin();
        const collide::rq_result* E = r_temp.r_end();
        for (; I != E; I++)
        {
            r_dest.append_result(*I);
            if (!(CB ? CB(*I, user_data) : TRUE))
                return r_dest.r_count();
            if (R.flags & (CDB::OPT_ONLYNEAREST | CDB::OPT_ONLYFIRST))
                return r_dest.r_count();
        }
    }
    return r_dest.r_count();
}

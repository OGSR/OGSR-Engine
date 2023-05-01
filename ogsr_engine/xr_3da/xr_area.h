#ifndef __XR_AREA_H__
#define __XR_AREA_H__

#include "xr_collide_form.h"
#include "..\xrCDB\xr_collide_defs.h"

// refs
class ENGINE_API ISpatial;
class ENGINE_API ICollisionForm;
class ENGINE_API CObject;

//-----------------------------------------------------------------------------------------------------------
// Space Area
//-----------------------------------------------------------------------------------------------------------
class ENGINE_API CObjectSpace
{
private:
    // Debug
    CDB::MODEL Static;
    Fbox m_BoundingVolume;
public:
#ifdef DEBUG
    ref_shader sh_debug;
    clQueryCollision q_debug; // MT: dangerous
    xr_vector<std::pair<Fsphere, u32>> dbg_S; // MT: dangerous
#endif

private:
    BOOL _RayTest(const Fvector& start, const Fvector& dir, float range, collide::rq_target tgt, collide::ray_cache* cache, CObject* ignore_object);
    BOOL _RayPick(const Fvector& start, const Fvector& dir, float range, collide::rq_target tgt, collide::rq_result& R, CObject* ignore_object);
    BOOL _RayQuery(collide::rq_results& dest, const collide::ray_defs& rq, collide::rq_callback* cb, LPVOID user_data, collide::test_callback* tb, CObject* ignore_object);
    BOOL _RayQuery2(collide::rq_results& dest, const collide::ray_defs& rq, collide::rq_callback* cb, LPVOID user_data, collide::test_callback* tb, CObject* ignore_object);

public:
    CObjectSpace();
    ~CObjectSpace();

    void Load();

    // Occluded/No
    BOOL RayTest(const Fvector& start, const Fvector& dir, float range, collide::rq_target tgt, collide::ray_cache* cache, CObject* ignore_object);

    // Game raypick (nearest) - returns object and addititional params
    BOOL RayPick(const Fvector& start, const Fvector& dir, float range, collide::rq_target tgt, collide::rq_result& R, CObject* ignore_object);

    // General collision query
    BOOL RayQuery(collide::rq_results& dest, const collide::ray_defs& rq, collide::rq_callback* cb, LPVOID user_data, collide::test_callback* tb, CObject* ignore_object);
    BOOL RayQuery(collide::rq_results& dest, ICollisionForm* target, const collide::ray_defs& rq);
    bool BoxQuery(Fvector const& box_center, Fvector const& box_z_axis, Fvector const& box_y_axis, Fvector const& box_sizes, xr_vector<Fvector>* out_tris);

    int GetNearest(xr_vector<CObject*>& q_nearest, ICollisionForm* obj, float range);
    int GetNearest(xr_vector<CObject*>& q_nearest, const Fvector& point, float range, CObject* ignore_object);
    int GetNearest(xr_vector<ISpatial*>& q_spatial, xr_vector<CObject*>& q_nearest, const Fvector& point, float range, CObject* ignore_object);

    CDB::TRI* GetStaticTris() { return Static.get_tris(); }
    Fvector* GetStaticVerts() { return Static.get_verts(); }
    CDB::MODEL* GetStaticModel() { return &Static; }

    const Fbox& GetBoundingVolume() { return m_BoundingVolume; }

    // Debugging
#ifdef DEBUG
    void dbgRender();
    ref_shader dbgGetShader() { return sh_debug; }
#endif
};

#endif //__XR_AREA_H__

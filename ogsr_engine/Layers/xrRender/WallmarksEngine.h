// WallmarksEngine.h: interface for the CWallmarksEngine class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

namespace WallmarksEngine
{
struct wallmark_static_queue_item;
struct wm_slot;
}

class CSkeletonWallmark;
class CKinematics;

class CWallmarksEngine
{
public:
    typedef WallmarksEngine::wm_slot wm_slot;

public:
    struct static_wallmark
    {
        Fsphere bounds;
        xr_vector<FVF::LIT> verts;
        float ttl;
    };
    DEFINE_VECTOR(static_wallmark*, StaticWMVec, StaticWMVecIt);
    DEFINE_VECTOR(wm_slot*, WMSlotVec, WMSlotVecIt);

private:
    xrCriticalSection lock;
    ref_geom hGeom;
    WMSlotVec marks;


    StaticWMVec static_pool;

    Fvector sml_normal;
    CFrustum sml_clipper;
    sPoly sml_poly_dest;
    sPoly sml_poly_src;

    CDB::Collector sml_collector;
    xr_vector<u32> sml_adjacency;

    xr_vector<WallmarksEngine::wallmark_static_queue_item> static_items_to_add;

private:
    wm_slot* FindSlot(const ref_shader& shader);
    wm_slot* AppendSlot(const ref_shader& shader);

private:
    void BuildMatrix(Fmatrix& dest, float invsz, const Fvector& from) const;
    void RecurseTri(u32 T, Fmatrix& mView, static_wallmark& W);

    static_wallmark* static_wm_allocate();
    void static_wm_destroy(static_wallmark* W);

    void static_wm_render(const static_wallmark* W, FVF::LIT*& V);

    void add_static_wallmark_internal(const WallmarksEngine::wallmark_static_queue_item& q);

public:
    CWallmarksEngine();
    ~CWallmarksEngine();

    void AddStaticWallmark(CDB::TRI* pTri, const Fvector* pVerts, const Fvector& contact_point, const ref_shader& sh, float wm_size);

    void AddSkeletonWallmark(Fmatrix* xf, CKinematics* obj, const ref_shader& sh, const Fvector& start, const Fvector& dir, float size);
    void AppendSkeletonWallmark(intrusive_ptr<CSkeletonWallmark> wm);

    void add_static_wallmarks_async();

    // render
    void Render();

    void Clear();
};

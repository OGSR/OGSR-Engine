// WallmarksEngine.cpp: implementation of the CWallmarksEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WallmarksEngine.h"
#include "SkeletonCustom.h"

namespace WallmarksEngine
{
struct wm_slot
{
    typedef CWallmarksEngine::StaticWMVec StaticWMVec;

    ref_shader shader;
    StaticWMVec static_items;
    xr_vector<intrusive_ptr<CSkeletonWallmark>> skeleton_items;

    wm_slot(ref_shader sh)
    {
        shader = sh;
        static_items.reserve(256);
        skeleton_items.reserve(256);
    }
};

struct wallmark_static_queue_item
{
    CDB::TRI* pTri{};
    const Fvector* pVerts{};
    Fvector contact_point{};
    ref_shader shader{};
    float size{};
};
} // namespace WallmarksEngine

// #include "xr_effsun.h"

const float W_DIST_FADE = 15.f;
const float W_DIST_FADE_SQR = W_DIST_FADE * W_DIST_FADE;
const float I_DIST_FADE_SQR = 1.f / W_DIST_FADE_SQR;
const int MAX_TRIS = 1024 * 16;

IC bool operator==(const CWallmarksEngine::wm_slot* slot, const ref_shader& shader) { return slot->shader == shader; }

CWallmarksEngine::wm_slot* CWallmarksEngine::FindSlot(const ref_shader& shader)
{
    const WMSlotVecIt it = std::find(marks.begin(), marks.end(), shader);
    return (it != marks.end()) ? *it : nullptr;
}
CWallmarksEngine::wm_slot* CWallmarksEngine::AppendSlot(const ref_shader& shader)
{
    marks.push_back(xr_new<wm_slot>(shader));
    return marks.back();
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWallmarksEngine::CWallmarksEngine()
{
    static_pool.reserve(256);
    marks.reserve(256);
    hGeom.create(FVF::F_LIT, RImplementation.Vertex.Buffer(), nullptr);
}

CWallmarksEngine::~CWallmarksEngine()
{
    Device.remove_from_seq_parallel(fastdelegate::MakeDelegate(this, &CWallmarksEngine::add_static_wallmarks_async));
    static_items_to_add.clear();

    Clear();
    hGeom.destroy();
}

void CWallmarksEngine::Clear()
{
    {
        for (auto& mark : marks)
        {
            for (StaticWMVecIt m_it = mark->static_items.begin(); m_it != mark->static_items.end(); ++m_it)
                static_wm_destroy(*m_it);

            xr_delete(mark);
        }
        marks.clear();
    }

    {
        for (auto& it : static_pool)
            xr_delete(it);
        static_pool.clear();
    }
}

// allocate
CWallmarksEngine::static_wallmark* CWallmarksEngine::static_wm_allocate()
{
    static_wallmark* W = nullptr;
    if (static_pool.empty())
        W = xr_new<static_wallmark>();
    else
    {
        W = static_pool.back();
        static_pool.pop_back();
    }

    W->ttl = ps_r__WallmarkTTL;
    W->verts.clear();
    return W;
}

// destroy
void CWallmarksEngine::static_wm_destroy(CWallmarksEngine::static_wallmark* W) { static_pool.push_back(W); }
// render
void CWallmarksEngine::static_wm_render(const CWallmarksEngine::static_wallmark* W, FVF::LIT*& V)
{
    const float a = 1 - (W->ttl / ps_r__WallmarkTTL);
    int aC = iFloor(a * 255.f);
    clamp(aC, 0, 255);
    const u32 C = color_rgba(128, 128, 128, aC);
    for (const auto& el : W->verts)
    {
        V->p.set(el.p);
        V->color = C;
        V->t.set(el.t);
        V++;
    }
}

//--------------------------------------------------------------------------------
void CWallmarksEngine::RecurseTri(const u32 t, Fmatrix& mView, CWallmarksEngine::static_wallmark& W)
{
    CDB::TRI* T = sml_collector.getT() + t;
    if (T->dummy)
        return;
    T->dummy = 0xffffffff;

    // Some vars
    const u32* v_ids = T->verts;
    const Fvector* v_data = sml_collector.getV();
    sml_poly_src.clear();
    sml_poly_src.push_back(v_data[v_ids[0]]);
    sml_poly_src.push_back(v_data[v_ids[1]]);
    sml_poly_src.push_back(v_data[v_ids[2]]);
    sml_poly_dest.clear();

    sPoly* P = sml_clipper.ClipPoly(sml_poly_src, sml_poly_dest);

    //. todo
    // uv_gen = mView * []
    // UV = pos*uv_gen

    if (P)
    {
        // Create vertices and triangulate poly (tri-fan style triangulation)
        FVF::LIT V0, V1, V2;
        Fvector UV;

        mView.transform_tiny(UV, (*P)[0]);
        V0.set((*P)[0], 0, (1 + UV.x) * .5f, (1 - UV.y) * .5f);
        mView.transform_tiny(UV, (*P)[1]);
        V1.set((*P)[1], 0, (1 + UV.x) * .5f, (1 - UV.y) * .5f);

        for (u32 i = 2; i < P->size(); i++)
        {
            mView.transform_tiny(UV, (*P)[i]);
            V2.set((*P)[i], 0, (1 + UV.x) * .5f, (1 - UV.y) * .5f);
            W.verts.push_back(V0);
            W.verts.push_back(V1);
            W.verts.push_back(V2);
            V1 = V2;
        }

        // recurse
        for (u32 i = 0; i < 3; i++)
        {
            const u32 adj = sml_adjacency[3 * t + i];
            if (0xffffffff == adj)
                continue;
            CDB::TRI* SML = sml_collector.getT() + adj;
            v_ids = SML->verts;

            Fvector test_normal;
            test_normal.mknormal(v_data[v_ids[0]], v_data[v_ids[1]], v_data[v_ids[2]]);
            const float cosa = test_normal.dotproduct(sml_normal);
            if (cosa < 0.034899f)
                continue; // cos(88)
            RecurseTri(adj, mView, W);
        }
    }
}

void CWallmarksEngine::BuildMatrix(Fmatrix& mView, const float invsz, const Fvector& from) const
{
    // build projection
    Fmatrix mScale;
    Fvector at, up, right, y;
    at.sub(from, sml_normal);
    y.set(0, 1, 0);
    if (_abs(sml_normal.y) > .99f)
        y.set(1, 0, 0);
    right.crossproduct(y, sml_normal);
    up.crossproduct(sml_normal, right);
    mView.build_camera(from, at, up);
    mScale.scale(invsz, invsz, invsz);
    mView.mulA_43(mScale);
}

void CWallmarksEngine::add_static_wallmarks_async()
{
    if (!static_items_to_add.empty())
    {
        // Physics may add wallmarks in parallel with rendering
        lock.Enter();

        for (const auto& items_to_add : static_items_to_add)
        {
            add_static_wallmark_internal(items_to_add);
        }

        lock.Leave();

        static_items_to_add.clear();
    }
}

void CWallmarksEngine::add_static_wallmark_internal(const WallmarksEngine::wallmark_static_queue_item& q)
{
    CDB::TRI* pTri = q.pTri;
    const Fvector* pVerts = q.pVerts;

    ZoneScoped;

    // query for polygons in bounding box
    // calculate adjacency

    Fbox bb_query;
    Fvector bbc, bbd;
    bb_query.set(q.contact_point, q.contact_point);
    bb_query.grow(q.size * 2.5f);
    bb_query.get_CD(bbc, bbd);

    xrXRC xrc;
    xrc.box_query(CDB::OPT_FULL_TEST, g_pGameLevel->ObjectSpace.GetStaticModel(), bbc, bbd);
    u32 triCount = xrc.r_count();
    if (0 == triCount)
        return;

    CDB::TRI* tris = g_pGameLevel->ObjectSpace.GetStaticTris();
    sml_collector.clear();
    sml_collector.add_face_packed_D(pVerts[pTri->verts[0]], pVerts[pTri->verts[1]], pVerts[pTri->verts[2]], 0);
    for (u32 t = 0; t < triCount; t++)
    {
        CDB::TRI* T = tris + xrc.r_begin()[t].id;
        if (T == pTri)
            continue;
        sml_collector.add_face_packed_D(pVerts[T->verts[0]], pVerts[T->verts[1]], pVerts[T->verts[2]], 0);
    }
    sml_collector.calc_adjacency(sml_adjacency);

    // calc face normal
    Fvector N;
    N.mknormal(pVerts[pTri->verts[0]], pVerts[pTri->verts[1]], pVerts[pTri->verts[2]]);
    sml_normal.set(N);

    // build 3D ortho-frustum
    Fmatrix mView, mRot;
    BuildMatrix(mView, 1 / q.size, q.contact_point);
    mRot.rotateZ(::Random.randF(deg2rad(-20.f), deg2rad(20.f)));
    mView.mulA_43(mRot);
    sml_clipper.CreateFromMatrix(mView, FRUSTUM_P_LRTB);

    // create wallmark
    static_wallmark* W = static_wm_allocate();

    {
        ZoneScopedN("RecurseTri");

        RecurseTri(0, mView, *W);
    }

    // calc sphere
    if (W->verts.size() < 3)
    {
        static_wm_destroy(W);
        return;
    }

    Fbox bb;
    bb.invalidate();
    for (const auto& el : W->verts)
        bb.modify(el.p);
    bb.getsphere(W->bounds.P, W->bounds.R);

    // append to slot

    //if (W->bounds.R < 1.f)
    {
        // search if similar wallmark exists
        wm_slot* slot = FindSlot(q.shader);
        if (slot)
        {
            StaticWMVecIt it = slot->static_items.begin();
            StaticWMVecIt end = slot->static_items.end();
            for (; it != end; ++it)
            {
                static_wallmark* wm = *it;
                if (wm->bounds.P.similar(W->bounds.P, 0.02f))
                {
                    // replace
                    static_wm_destroy(wm);
                    *it = W;
                    return;
                }
            }
        }
        else
        {
            slot = AppendSlot(q.shader);
        }

        // no similar - register _new_
        slot->static_items.push_back(W);
    }
    //else
    //{
    //	static_wm_destroy(W);
    //}
}

void CWallmarksEngine::AddStaticWallmark(CDB::TRI* pTri, const Fvector* pVerts, const Fvector& contact_point, const ref_shader& sh, const float wm_size)
{
    // optimization cheat: don't allow wallmarks more than 100 m from viewer/actor
    if (contact_point.distance_to_sqr(Device.vCameraPosition) > _sqr(100.f))
        return;

    static_items_to_add.emplace_back(pTri, pVerts, contact_point, sh, wm_size);

    // make sure to submit for next parallel call
    Device.add_to_seq_parallel(fastdelegate::MakeDelegate(this, &CWallmarksEngine::add_static_wallmarks_async));
}

void CWallmarksEngine::AddSkeletonWallmark(Fmatrix* xf, CKinematics* obj, const ref_shader& sh, const Fvector& start, const Fvector& dir, const float size)
{
    // optimization cheat: don't allow wallmarks more than 50 m from viewer/actor
    if (xf->c.distance_to_sqr(Device.vCameraPosition) > _sqr(50.f))
        return;

    obj->AddWallmark(xf, start, dir, sh, size);
}

void CWallmarksEngine::AppendSkeletonWallmark(intrusive_ptr<CSkeletonWallmark> wm)
{
    lock.Enter();
    // search if similar wallmark exists
    wm_slot* slot = FindSlot(wm->Shader());
    if (!slot)
        slot = AppendSlot(wm->Shader());
    // no similar - register _new_
    slot->skeleton_items.push_back(wm);
#ifdef DEBUG
        wm->used_in_render = Device.dwFrame;
#endif
    lock.Leave();
}

extern float r_ssaDISCARD;

ICF void BeginStream(ref_geom hGeom, u32& w_offset, FVF::LIT*& w_verts, FVF::LIT*& w_start)
{
    w_offset = 0;
    w_verts = static_cast<FVF::LIT*>(RImplementation.Vertex.Lock(MAX_TRIS * 3, hGeom->vb_stride, w_offset));
    w_start = w_verts;
}

ICF void FlushStream(CBackend& cmd_list, ref_geom hGeom, ref_shader shader, const u32& w_offset, FVF::LIT*& w_verts, FVF::LIT*& w_start, const BOOL bSuppressCull)
{
    const u32 w_count = static_cast<u32>(w_verts - w_start);
    RImplementation.Vertex.Unlock(w_count, hGeom->vb_stride);
    if (w_count)
    {
        cmd_list.set_Shader(shader);
        cmd_list.set_Geometry(hGeom);
        if (bSuppressCull)
            cmd_list.set_CullMode(CULL_NONE);
        cmd_list.Render(D3DPT_TRIANGLELIST, w_offset, w_count / 3);
        if (bSuppressCull)
            cmd_list.set_CullMode(CULL_CCW);
        Device.Statistic->RenderDUMP_WMT_Count += w_count / 3;
    }
}

void CWallmarksEngine::Render()
{
    ZoneScoped;

    auto& dsgraph = RImplementation.get_imm_context();
    auto& cmd_list = dsgraph.cmd_list;

    // Projection and xform
    Fmatrix proj = Device.mProject;
    proj._43 -= ps_r__WallmarkSHIFT;
    cmd_list.set_xform_world(Fidentity);
    cmd_list.set_xform_project(proj);

    Fvector mViewPos;
    mViewPos.mad(Device.vCameraPosition, Device.vCameraDirection, ps_r__WallmarkSHIFT_V);
    Fmatrix view;
    view.build_camera_dir(mViewPos, Device.vCameraDirection, Device.vCameraTop);
    cmd_list.set_xform_view(view);

    Device.Statistic->RenderDUMP_WM.Begin();
    Device.Statistic->RenderDUMP_WMS_Count = 0;
    Device.Statistic->RenderDUMP_WMD_Count = 0;
    Device.Statistic->RenderDUMP_WMT_Count = 0;

    float ssaCLIP = r_ssaDISCARD / 4;

    lock.Enter(); // Physics may add wallmarks in parallel with rendering

    for (const auto& slot : marks)
    {
        u32 w_offset;
        FVF::LIT *w_verts, *w_start;
        BeginStream(hGeom, w_offset, w_verts, w_start);
        // static wallmarks
        for (auto w_it = slot->static_items.begin(); w_it != slot->static_items.end();)
        {
            static_wallmark* W = *w_it;
            if (RImplementation.ViewBase.testSphere_dirty(W->bounds.P, W->bounds.R))
            {
                Device.Statistic->RenderDUMP_WMS_Count++;
                float dst = Device.vCameraPosition.distance_to_sqr(W->bounds.P);
                float ssa = W->bounds.R * W->bounds.R / dst;
                if (ssa >= ssaCLIP)
                {
                    u32 w_count = static_cast<u32>(w_verts - w_start);
                    if ((w_count + W->verts.size()) >= (MAX_TRIS * 3))
                    {
                        FlushStream(cmd_list, hGeom, slot->shader, w_offset, w_verts, w_start, FALSE);
                        BeginStream(hGeom, w_offset, w_verts, w_start);
                    }
                    static_wm_render(W, w_verts);
                }
                W->ttl -= 0.1f * Device.fTimeDelta; // visible wallmarks fade much slower
            }
            else
            {
                W->ttl -= Device.fTimeDelta;
            }
            if (W->ttl <= EPS)
            {
                static_wm_destroy(W);
                w_it = slot->static_items.erase(w_it);
            }
            else
            {
                ++w_it;
            }
        }

        // Flush stream
        FlushStream(cmd_list, hGeom, slot->shader, w_offset, w_verts, w_start, FALSE); //. remove line if !(suppress cull needed)
        BeginStream(hGeom, w_offset, w_verts, w_start);

        // dynamic wallmarks
        for (xr_vector<intrusive_ptr<CSkeletonWallmark>>::iterator w_it = slot->skeleton_items.begin(); w_it != slot->skeleton_items.end(); ++w_it)
        {
            intrusive_ptr<CSkeletonWallmark> W = *w_it;
            if (!W)
            {
                continue;
            }

#ifdef DEBUG
            if (W->used_in_render != Device.dwFrame)
            {
                Log("W->used_in_render", W->used_in_render);
                Log("Device.dwFrame", Device.dwFrame);
                VERIFY(W->used_in_render == Device.dwFrame);
            }
#endif

            float dst = Device.vCameraPosition.distance_to_sqr(W->m_Bounds.P);
            float ssa = W->m_Bounds.R * W->m_Bounds.R / dst;
            if (ssa >= ssaCLIP)
            {
                Device.Statistic->RenderDUMP_WMD_Count++;
                u32 w_count = static_cast<u32>(w_verts - w_start);
                if ((w_count + W->VCount()) >= (MAX_TRIS * 3))
                {
                    FlushStream(cmd_list, hGeom, slot->shader, w_offset, w_verts, w_start, TRUE);
                    BeginStream(hGeom, w_offset, w_verts, w_start);
                }

                //FVF::LIT* w_save = w_verts;
                //try
                {
                    W->Parent()->RenderWallmark(W, w_verts);
                }
                /*catch (...)
                {
                    Msg("! Failed to render dynamic wallmark");
                    w_verts = w_save;
                }*/
            }
#ifdef DEBUG
            W->used_in_render = u32(-1);
#endif
        }
        slot->skeleton_items.clear();
        // Flush stream
        FlushStream(cmd_list, hGeom, slot->shader, w_offset, w_verts, w_start, TRUE);
    }

    lock.Leave(); // Physics may add wallmarks in parallel with rendering

    // Level-wmarks
    dsgraph.r_dsgraph_render_wmarks();
    Device.Statistic->RenderDUMP_WM.End();

    cmd_list.set_xform_view(Device.mView);
    cmd_list.set_xform_project(Device.mProject);
}

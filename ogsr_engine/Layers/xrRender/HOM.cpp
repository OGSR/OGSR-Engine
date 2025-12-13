// HOM.cpp: implementation of the CHOM class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HOM.h"

#include "dxDebugRender.h"
#include "occRasterizer.h"
#include "../../xr_3da/GameFont.h"

#include "dxRenderDeviceRender.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHOM::CHOM()
{
    m_pModel = nullptr;
    m_pTris = nullptr;

    Device.seqRender.Add(this, REG_PRIORITY_LOW - 1000);
}

CHOM::~CHOM()
{
    Device.seqRender.Remove(this);
}

#pragma pack(push, 4)
struct HOM_poly
{
    Fvector v1, v2, v3;
    u32 flags;
};
#pragma pack(pop)

static float area(const Fvector& v0, const Fvector& v1, const Fvector& v2)
{
    const float e1 = v0.distance_to(v1);
    const float e2 = v0.distance_to(v2);
    const float e3 = v1.distance_to(v2);

    const float p = (e1 + e2 + e3) / 2.f;
    return _sqrt(p * (p - e1) * (p - e2) * (p - e3));
}

void CHOM::Load()
{
    // Find and open file
    string_path fName;
    FS.update_path(fName, fsgame::level, fsgame::level_files::level_hom);
    if (!FS.exist(fName))
    {
        Msg(" WARNING: Occlusion map '%s' not found.", fName);
        return;
    }
    Msg("* Loading HOM: %s", fName);

    IReader* fs = FS.r_open(fName);

    // Load tris and merge them
    CDB::Collector CL;

    {
        IReader* S = fs->open_chunk(1);
        while (!S->eof())
        {
            HOM_poly P;
            S->r(&P, sizeof(P));
            CL.add_face_packed_D(P.v1, P.v2, P.v3, P.flags, 0.01f);
        }
        S->close();
    }

    FS.r_close(fs);

    // Determine adjacency
    xr_vector<u32> adjacency;
    CL.calc_adjacency(adjacency);

    // Create RASTER-triangles
    m_pTris = xr_alloc<occTri>(CL.getTS());

    for (size_t it{}; it < CL.getTS(); it++)
    {
        const CDB::TRI& clT = CL.getT()[it];
        occTri& rT = m_pTris[it];
        const Fvector& v0 = CL.getV()[clT.verts[0]];
        const Fvector& v1 = CL.getV()[clT.verts[1]];
        const Fvector& v2 = CL.getV()[clT.verts[2]];
        rT.adjacent[0] = (0xffffffff == adjacency[3 * it + 0]) ? ((occTri*)(-1)) : (m_pTris + adjacency[3 * it + 0]);
        rT.adjacent[1] = (0xffffffff == adjacency[3 * it + 1]) ? ((occTri*)(-1)) : (m_pTris + adjacency[3 * it + 1]);
        rT.adjacent[2] = (0xffffffff == adjacency[3 * it + 2]) ? ((occTri*)(-1)) : (m_pTris + adjacency[3 * it + 2]);
        rT.flags = static_cast<u32>(clT.dummy);
        rT.area = area(v0, v1, v2);
        if (rT.area < EPS_L)
        {
            Msg("! Invalid HOM triangle (%f,%f,%f)-(%f,%f,%f)-(%f,%f,%f)", VPUSH(v0), VPUSH(v1), VPUSH(v2));
        }
        rT.plane.build(v0, v1, v2);
        rT.skip = 0;
        rT.center.add(v0, v1).add(v2).div(3.f);
    }

    // Create AABB-tree
    m_pModel = xr_new<CDB::MODEL>();
    m_pModel->build(CL.getV(), CL.getVS(), CL.getT(), CL.getTS());
}

void CHOM::Unload()
{
    xr_delete(m_pModel);
    xr_free(m_pTris);
}

class pred_fb
{
public:
    occTri* m_pTris;
    Fvector camera;

public:
    pred_fb(occTri* _t) : m_pTris(_t) {}
    pred_fb(occTri* _t, Fvector& _c) : m_pTris(_t), camera(_c) {}
    ICF bool operator()(const CDB::RESULT& _1, const CDB::RESULT& _2) const
    {
        const occTri& t0 = m_pTris[_1.id];
        const occTri& t1 = m_pTris[_2.id];
        return camera.distance_to_sqr(t0.center) < camera.distance_to_sqr(t1.center);
    }
    ICF bool operator()(const CDB::RESULT& _1) const
    {
        const occTri& T = m_pTris[_1.id];
        return T.skip > Device.dwFrame;
    }
};

void CHOM::Render_DB(CFrustum& base)
{
    ZoneScoped;

    // Update projection matrices on every frame to ensure valid HOM culling
    float view_dim = occ_dim_0;
    Fmatrix m_viewport = {view_dim / 2.f, 0.0f, 0.0f, 0.0f, 0.0f, -view_dim / 2.f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, view_dim / 2.f + 0 + 0, view_dim / 2.f + 0 + 0, 0.0f, 1.0f};
    Fmatrix m_viewport_01 = {1.f / 2.f, 0.0f, 0.0f, 0.0f, 0.0f, -1.f / 2.f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.f / 2.f + 0 + 0, 1.f / 2.f + 0 + 0, 0.0f, 1.0f};
    m_xform.mul(m_viewport, Device.mFullTransform);
    m_xform_01.mul(m_viewport_01, Device.mFullTransform);

    // Query DB
    xrc.frustum_query(0, m_pModel, base);
    if (0 == xrc.r_count())
        return;

    // Prepare
    CDB::RESULT* it = xrc.r_begin();
    CDB::RESULT* end = xrc.r_end();

    Fvector COP = Device.vCameraPosition;
    end = std::remove_if(it, end, pred_fb(m_pTris));
    std::sort(it, end, pred_fb(m_pTris, COP));

    // Build frustum with near plane only
    CFrustum clip;
    clip.CreateFromMatrix(Device.mFullTransform, FRUSTUM_P_NEAR);
    sPoly src, dst;
    u32 _frame = Device.dwFrame;

    FrustumTriangleCount = xrc.r_count();
    VisibleTriangleCount = 0;

    // Perfrom selection, sorting, culling
    for (; it != end; it++)
    {
        // Control skipping
        occTri& T = m_pTris[it->id];
        u32 next = _frame + ::Random.randI(3, 10);

        // Test for good occluder - should be improved :)
        if (!(T.flags || (T.plane.classify(COP) > 0)))
        {
            T.skip = next;
            continue;
        }

        // Access to triangle vertices
        CDB::TRI& t = m_pModel->get_tris()[it->id];
        const Fvector* v = m_pModel->get_verts();
        src.clear();
        dst.clear();
        src.push_back(v[t.verts[0]]);
        src.push_back(v[t.verts[1]]);
        src.push_back(v[t.verts[2]]);
        sPoly* P = clip.ClipPoly(src, dst);
        if (nullptr == P)
        {
            T.skip = next;
            continue;
        }

        // XForm and Rasterize
        VisibleTriangleCount++;
        u32 pixels = 0;
        int limit = int(P->size()) - 1;
        for (int v = 1; v < limit; v++)
        {
            m_xform.transform(T.raster[0], (*P)[0]);
            m_xform.transform(T.raster[1], (*P)[v + 0]);
            m_xform.transform(T.raster[2], (*P)[v + 1]);
            pixels += Raster.rasterize(&T);
        }
        if (0 == pixels)
        {
            T.skip = next;
            continue;
        }
    }
}

void CHOM::DispatchRender()
{
    VisibleTriangleCount = 0;
    FrustumTriangleCount = 0;
    CulledOutCount = 0;

    if (!Allowed())
        return;

    if (g_pGameLevel && !IsMainMenuActive())
    {
        ZoneScoped;

        CFrustum ViewBase;
        ViewBase.CreateFromMatrix(Device.mFullTransform, FRUSTUM_P_LRTB + FRUSTUM_P_FAR);

        Raster.clear();
        Render_DB(ViewBase);
        Raster.propagade();
    }
}

namespace
{
ICF BOOL xform_b0(Fvector2& min, Fvector2& max, float& minz, const Fmatrix& X, const float _x, const float _y, const float _z)
{
    const float z = _x * X._13 + _y * X._23 + _z * X._33 + X._43;
    if (z < EPS)
        return TRUE;
    const float iw = 1.f / (_x * X._14 + _y * X._24 + _z * X._34 + X._44);
    min.x = max.x = (_x * X._11 + _y * X._21 + _z * X._31 + X._41) * iw;
    min.y = max.y = (_x * X._12 + _y * X._22 + _z * X._32 + X._42) * iw;
    minz = 0.f + z * iw;
    return FALSE;
}
ICF BOOL xform_b1(Fvector2& min, Fvector2& max, float& minz, Fmatrix& X, float _x, float _y, float _z)
{
    float t;
    const float z = _x * X._13 + _y * X._23 + _z * X._33 + X._43;
    if (z < EPS)
        return TRUE;
    const float iw = 1.f / (_x * X._14 + _y * X._24 + _z * X._34 + X._44);
    t = (_x * X._11 + _y * X._21 + _z * X._31 + X._41) * iw;
    if (t < min.x)
        min.x = t;
    else if (t > max.x)
        max.x = t;
    t = (_x * X._12 + _y * X._22 + _z * X._32 + X._42) * iw;
    if (t < min.y)
        min.y = t;
    else if (t > max.y)
        max.y = t;
    t = 0.f + z * iw;
    if (t < minz)
        minz = t;
    return FALSE;
}
IC BOOL _visible(Fbox& B, Fmatrix& m_xform_01)
{
    // Find min/max points of xformed-box
    Fvector2 min, max;
    float z;
    if (xform_b0(min, max, z, m_xform_01, B.min.x, B.min.y, B.min.z))
        return TRUE;
    if (xform_b1(min, max, z, m_xform_01, B.min.x, B.min.y, B.max.z))
        return TRUE;
    if (xform_b1(min, max, z, m_xform_01, B.max.x, B.min.y, B.max.z))
        return TRUE;
    if (xform_b1(min, max, z, m_xform_01, B.max.x, B.min.y, B.min.z))
        return TRUE;
    if (xform_b1(min, max, z, m_xform_01, B.min.x, B.max.y, B.min.z))
        return TRUE;
    if (xform_b1(min, max, z, m_xform_01, B.min.x, B.max.y, B.max.z))
        return TRUE;
    if (xform_b1(min, max, z, m_xform_01, B.max.x, B.max.y, B.max.z))
        return TRUE;
    if (xform_b1(min, max, z, m_xform_01, B.max.x, B.max.y, B.min.z))
        return TRUE;
    return Raster.test(min.x, min.y, max.x, max.y, z);
}
} // namespace

BOOL CHOM::visible(vis_data& vis)
{
    if (!Allowed())
        return TRUE; // disabled - everything visible

    if (vis.hom_tested == Device.dwFrame)
        return vis.hom_frame > vis.hom_tested; // tested on this frame - visible if hom_frame > that current

    if (Device.dwFrame < vis.hom_frame)
        return TRUE; // visible for cached frames

    // Now, the test time comes
    // 0. The object was hidden, and we must prove that each frame - test | frame-old, tested-new, hom_res = false;
    // 1. The object was visible, but we must to re-check it - test | frame-new, tested-???, hom_res = true;
    // 2. New object slides into view - delay test| frame-old, tested-old, hom_res = ???;

    const u32 frame_current = Device.dwFrame;

    Device.Statistic->RenderCALC_HOM.Begin();
    const BOOL result = _visible(vis.box, m_xform_01);
    Device.Statistic->RenderCALC_HOM.End();

    if (result)
        // visible - delay next test
        vis.hom_frame = frame_current + ::Random.randI(5 * 2, 5 * 5);
    else
        // hidden - schedule to next frame
        vis.hom_frame = frame_current;

    vis.hom_tested = frame_current;

    if (!result)
        CulledOutCount++;

    return result;
}

BOOL CHOM::visible(sPoly& P) // special case for traverse_sector
{
    if (!Allowed())
        return TRUE;

    // Find min/max points of xformed-box
    Fvector2 min, max;
    float z;

    if (xform_b0(min, max, z, m_xform_01, P.front().x, P.front().y, P.front().z))
        return TRUE;
    for (u32 it = 1; it < P.size(); it++)
        if (xform_b1(min, max, z, m_xform_01, P[it].x, P[it].y, P[it].z))
            return TRUE;
    return Raster.test(min.x, min.y, max.x, max.y, z);
}

void CHOM::OnRender()
{
    if (psDeviceFlags.is(rsOcclusionDraw))
    {
        Raster.on_dbg_render();

        if (m_pModel)
        {
            xr_vector<u16> pairs;
            pairs.resize(m_pModel->get_tris_count() * 6);
            for (size_t i = 0; i < m_pModel->get_tris_count(); i++)
            {
                const CDB::TRI* T = m_pModel->get_tris() + i;

                pairs[(i * 6) + 0] = static_cast<u16>(T->verts[0]);
                pairs[(i * 6) + 1] = static_cast<u16>(T->verts[1]);
                pairs[(i * 6) + 2] = static_cast<u16>(T->verts[1]);
                pairs[(i * 6) + 3] = static_cast<u16>(T->verts[2]);
                pairs[(i * 6) + 4] = static_cast<u16>(T->verts[2]);
                pairs[(i * 6) + 5] = static_cast<u16>(T->verts[0]);
            }

            DebugRenderImpl.add_lines(
				m_pModel->get_verts(), m_pModel->get_verts_count(),
				pairs.data(), (u32)pairs.size() / 2, 0xFFFFFFFF
			);
        }
    }
}

void CHOM::stats()
{
    if (m_pModel)
    {
        CGameFont& F = *Device.Statistic->Font();
        F.OutNext("***** HOM *****");
        F.OutNext("   visible:  %2d", VisibleTriangleCount);
        F.OutNext("   frustum:  %2d", FrustumTriangleCount);
        F.OutNext("     total:  %2d", m_pModel->get_tris_count());
        F.OutNext("culled cnt:  %2d", CulledOutCount);
    }
}

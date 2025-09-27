#include "stdafx.h"

#include "dxRenderDeviceRender.h"
#include "../../xr_3da/igame_persistent.h"
#include "../../xr_3da/environment.h"
#include "fvf.h"

CPortalTraverser::CPortalTraverser() { i_marker = 0xffffffff; }

#ifdef DEBUG
xr_vector<IRender_Sector*> dbg_sectors;
#endif

void CPortalTraverser::traverse(IRender_Sector* start, CFrustum& F, const Fvector& vBase, const Fmatrix& mXFORM, const u32 options)
{
    if (options & VQ_FADE)
    {
        f_portals.clear();
        f_portals.reserve(16);
    }

    R_ASSERT(start);

    i_start = smart_cast<CSector*>(start);
    i_vBase = vBase;
    i_mXFORM = mXFORM;
    i_options = options;

    i_marker++;
    device_frame = Device.dwFrame;

    r_sectors.clear();

    {
        ZoneScoped;

        _scissor scissor;
        scissor.set(0, 0, 1, 1);
        scissor.depth = 0;
        traverse_sector(i_start, F, scissor);
    }
}

void CPortalTraverser::fade_portal(CPortal* _p, float ssa) { f_portals.emplace_back(_p, ssa); }

extern float r_ssaDISCARD;
extern float r_ssaLOD_A, r_ssaLOD_B;

void CPortalTraverser::fade_render(CBackend& cmd_list)
{
    if (f_portals.empty())
        return;

    // re-sort, back to front
    std::sort(f_portals.begin(), f_portals.end(), [this](const auto& p1, const auto& p2) {
        const float d1 = i_vBase.distance_to_sqr(p1.first->S.P);
        const float d2 = i_vBase.distance_to_sqr(p2.first->S.P);
        return d2 > d1; // descending, back to front
    });

    // calc poly-count
    u32 _pcount = 0;
    for (const auto& f_portal : f_portals)
        _pcount += f_portal.first->getPoly().size() - 2;

    // fill buffers
    u32 _offset = 0;
    FVF::L* _v = (FVF::L*)RImplementation.Vertex.Lock(_pcount * 3, dxRenderDeviceRender::Instance().m_PortalFadeGeom.stride(), _offset);
    const float ssaRange = r_ssaLOD_A - r_ssaLOD_B;
    const Fvector _ambient_f = g_pGamePersistent->Environment().CurrentEnv->ambient;
    const u32 _ambient = color_rgba_f(_ambient_f.x, _ambient_f.y, _ambient_f.z, 0);
    for (const auto& fp : f_portals)
    {
        CPortal* _P = fp.first;
        const float _ssa = fp.second;
        const float ssaDiff = _ssa - r_ssaLOD_B;
        const float ssaScale = ssaDiff / ssaRange;
        int iA = iFloor((1 - ssaScale) * 255.5f);
        clamp(iA, 0, 255);
        const u32 _clr = subst_alpha(_ambient, u32(iA));

        // fill polys
        const u32 _polys = _P->getPoly().size() - 2;
        for (u32 _pit = 0; _pit < _polys; _pit++)
        {
            _v->set(_P->getPoly()[0], _clr);
            _v++;
            _v->set(_P->getPoly()[_pit + 1], _clr);
            _v++;
            _v->set(_P->getPoly()[_pit + 2], _clr);
            _v++;
        }
    }
    RImplementation.Vertex.Unlock(_pcount * 3, dxRenderDeviceRender::Instance().m_PortalFadeGeom.stride());

    // render
    cmd_list.set_xform_world(Fidentity);
    cmd_list.set_Shader(dxRenderDeviceRender::Instance().m_PortalFadeShader);
    cmd_list.set_Geometry(dxRenderDeviceRender::Instance().m_PortalFadeGeom);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.Render(D3DPT_TRIANGLELIST, _offset, _pcount);
    cmd_list.set_CullMode(CULL_CCW);

    // cleanup
    f_portals.clear();
}

void CPortalTraverser::traverse_sector(CSector* sector, CFrustum& F, _scissor& R_scissor)
{
    // Register traversal process
    if (sector->r_marker != i_marker)
    {
        sector->r_marker = i_marker;
        r_sectors.push_back(sector);
        sector->r_frustums.clear();
        sector->r_scissors.clear();
    }
    sector->r_frustums.push_back(F);
    sector->r_scissors.push_back(R_scissor);

    // Search visible portals and go through them
    sPoly S, D;
    for (u32 I = 0; I < sector->m_portals.size(); I++)
    {
        if (sector->m_portals[I]->r_marker == i_marker)
            continue;

        CPortal* PORTAL = sector->m_portals[I];
        CSector* pSector;

        // Select sector (allow intersecting portals to be finely classified)
        if (PORTAL->bDualRender)
        {
            pSector = PORTAL->getSector(sector);
        }
        else
        {
            pSector = PORTAL->getSectorBack(i_vBase);
            if (pSector == sector)
                continue;
            if (pSector == i_start)
                continue;
        }

        // Early-out sphere
        if (!F.testSphere_dirty(PORTAL->S.P, PORTAL->S.R))
            continue;

        // SSA	(if required)
        if (i_options & CPortalTraverser::VQ_SSA)
        {
            Fvector dir2portal;
            dir2portal.sub(PORTAL->S.P, i_vBase);
            float R = PORTAL->S.R;
            float distSQ = dir2portal.square_magnitude();
            float ssa = R * R / distSQ;
            dir2portal.div(_sqrt(distSQ));
            ssa *= _abs(PORTAL->P.n.dotproduct(dir2portal));
            if (ssa < r_ssaDISCARD)
                continue;

            if (i_options & CPortalTraverser::VQ_FADE)
            {
                if (ssa < r_ssaLOD_A)
                    fade_portal(PORTAL, ssa);
                if (ssa < r_ssaLOD_B)
                    continue;
            }
        }

        // Clip by frustum
        auto& POLY = PORTAL->getPoly();
        S.assign(&*POLY.begin(), POLY.size());
        D.clear();
        sPoly* P = F.ClipPoly(S, D);
        if (nullptr == P)
            continue;

        // Scissor and optimized HOM-testing
        _scissor scissor = R_scissor;

        // Cull by HOM (slower algo)
        if ((i_options & CPortalTraverser::VQ_HOM) && (!RImplementation.HOM.visible(*P)))
            continue;

        // Create _new_ frustum and recurse
        CFrustum Clip;
        Clip.CreateFromPortal(P, PORTAL->P.n, i_vBase, i_mXFORM);
        PORTAL->r_marker = i_marker;
        PORTAL->bDualRender = FALSE;
        traverse_sector(pSector, Clip, scissor);
    }
}


#ifdef DEBUG
void CPortalTraverser::dbg_draw()
{
    cmd_list.OnFrameEnd();
    cmd_list.set_xform_world(Fidentity);
    cmd_list.set_xform_view(Fidentity);
    cmd_list.set_xform_project(Fidentity);
    cmd_list.set_Shader(dxRenderDeviceRender::Instance().m_WireShader);
    cmd_list.set_c("tfactor", 1.f, 1.f, 1.f, 1.f);

    for (u32 s = 0; s < dbg_sectors.size(); s++)
    {
        CSector* S = (CSector*)dbg_sectors[s];
        FVF::L verts[5];
        Fbox2 bb = S->r_scissor_merged;
        bb.min.x = bb.min.x * 2 - 1;
        bb.max.x = bb.max.x * 2 - 1;
        bb.min.y = (1 - bb.min.y) * 2 - 1;
        bb.max.y = (1 - bb.max.y) * 2 - 1;

        verts[0].set(bb.min.x, bb.min.y, EPS, 0xffffffff);
        verts[1].set(bb.max.x, bb.min.y, EPS, 0xffffffff);
        verts[2].set(bb.max.x, bb.max.y, EPS, 0xffffffff);
        verts[3].set(bb.min.x, bb.max.y, EPS, 0xffffffff);
        verts[4].set(bb.min.x, bb.min.y, EPS, 0xffffffff);
        cmd_list.dbg_Draw(D3DPT_LINESTRIP, verts, 4);
    }
}
#endif

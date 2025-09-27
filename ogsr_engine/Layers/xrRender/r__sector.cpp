
// Portal.cpp: implementation of the CPortal class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "r__sector.h"
#include "../../xr_3da/xrLevel.h"
#include "../../xr_3da/xr_object.h"
#include "fbasicvisual.h"
#include "../../xr_3da/IGame_Persistent.h"
#include "dxRenderDeviceRender.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPortal::CPortal()
{
#ifdef DEBUG
    Device.seqRender.Add(this, REG_PRIORITY_LOW - 1000);
#endif
}

CPortal::~CPortal()
{
#ifdef DEBUG
    Device.seqRender.Remove(this);
#endif
}

#ifdef DEBUG
void CPortal::OnRender()
{
    if (psDeviceFlags.is(rsOcclusionDraw))
    {
        VERIFY(poly.size());
        // draw rect
        DEFINE_VECTOR(FVF::L, LVec, LVecIt);
        static LVec V;
        V.resize(poly.size() + 2);
        constexpr u32 portalColor = 0x800000FF;
        Fvector C{};
        for (u32 k = 0; k < poly.size(); k++)
        {
            C.add(poly[k]);
            V[k + 1].set(poly[k], portalColor);
        }
        V.back().set(poly[0], portalColor);
        C.div((float)poly.size());
        V[0].set(C, portalColor);

        cmd_list.set_xform_world(Fidentity);
        // draw solid
        cmd_list.set_Shader(dxRenderDeviceRender::Instance().m_SelectionShader);

        cmd_list.set_c("tfactor", float(color_get_R(portalColor)) / 255.f, float(color_get_G(portalColor)) / 255.f, float(color_get_B(portalColor)) / 255.f,
                     float(color_get_A(portalColor)) / 255.f);

        cmd_list.dbg_Draw(D3DPT_TRIANGLEFAN, &*V.begin(), V.size() - 2);

        // draw wire
        if (bDebug)
        {
            RImplementation.rmNear();
        }
        else
        {
            Device.SetNearer(TRUE);
        }
        cmd_list.set_Shader(dxRenderDeviceRender::Instance().m_WireShader);

        cmd_list.set_c("tfactor", float(color_get_R(portalColor)) / 255.f, float(color_get_G(portalColor)) / 255.f, float(color_get_B(portalColor)) / 255.f,
                     float(color_get_A(portalColor)) / 255.f);

        cmd_list.dbg_Draw(D3DPT_LINESTRIP, &*(V.begin() + 1), V.size() - 2);
        if (bDebug)
        {
            RImplementation.rmNormal();
        }
        else
        {
            Device.SetNearer(FALSE);
        }
    }
}
#endif
//
void CPortal::setup(const level_portal_data_t& data, const xr_vector<CSector*>& sectors)
{
    const auto* V = data.vertices.begin();
    const auto vcnt = data.vertices.size();
    CSector* face = sectors[data.sector_front];
    CSector* back = sectors[data.sector_back];

    // calc sphere
    Fbox BB;
    BB.invalidate();
    for (u32 v = 0; v < vcnt; v++)
        BB.modify(V[v]);
    BB.getsphere(S.P, S.R);

    //
    poly.assign(V, vcnt);
    pFace = face;
    pBack = back;
    r_marker = 0xffffffff;

    Fvector N, T;
    N.set(0, 0, 0);

    u32 _cnt = 0;
    for (u32 i = 2; i < vcnt; i++)
    {
        T.mknormal_non_normalized(poly[0], poly[i - 1], poly[i]);
        const float m = T.magnitude();
        if (m > EPS_S)
        {
            N.add(T.div(m));
            _cnt++;
        }
    }
    R_ASSERT(_cnt, "Invalid portal detected");
    N.div(float(_cnt));
    P.build(poly[0], N);

    /*
    if (_abs(1-P.n.magnitude())<EPS)
    xrDebug::Fatal      (DEBUG_INFO,"Degenerated portal found at {%3.2f,%3.2f,%3.2f}.",VPUSH(poly[0]));
    */
}

void CSector::setup(const level_sector_data_t& data, const xr_vector<CPortal*>& portals)
{
    r_marker = 0xffffffff;

    // Assign portal polygons
    const auto num_portals = data.portals_id.size();
    m_portals.resize(num_portals);
    for (int idx = 0; idx < num_portals; ++idx)
    {
        const auto ID = data.portals_id[idx];
        m_portals[idx] = portals[ID];
    }

    // Assign visual
    m_root = smart_cast<dxRender_Visual*>(RImplementation.getVisual(data.root_id));
}
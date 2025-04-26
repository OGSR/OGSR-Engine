#include "stdafx.h"
#include "dxStatsRender.h"
#include "../../xr_3da/GameFont.h"
#include "dxRenderDeviceRender.h"

void dxStatsRender::Copy(IStatsRender& _in) { *this = *((dxStatsRender*)&_in); }

void dxStatsRender::OutData1(CGameFont& F)
{
    const auto& rcstats = RCache.stat;

    F.OutNext("Vertices:     %d/%d", rcstats.verts, rcstats.calls ? rcstats.verts / rcstats.calls : 0);
    F.OutNext("Polygons:     %d/%d", rcstats.polys, rcstats.calls ? rcstats.polys / rcstats.calls : 0);
    F.OutNext("DIP/DP:       %d", rcstats.calls);
}

void dxStatsRender::OutData2(CGameFont& F)
{
    const auto& rcstats = RCache.stat;

    F.OutNext("S/T/C:      %d/%d/%d", rcstats.states, rcstats.textures, rcstats.constants);
    F.OutNext("RT/ZB:     %d/%d", rcstats.target_rt, rcstats.target_zb);
    F.OutNext("PS/VS/GS:     %d/%d/%d", rcstats.ps, rcstats.vs, rcstats.gs);
    F.OutNext("HS/DS/CS:     %d/%d/%d", rcstats.hs, rcstats.ds, rcstats.cs);
    F.OutNext("DECL/VB/IB:   %d/%d/%d", rcstats.decl, rcstats.vb, rcstats.ib);
}

void dxStatsRender::OutData3(CGameFont& F)
{
    F.OutNext("xforms:      %d", RCache.stat.xforms);
}

void dxStatsRender::OutData4(CGameFont& F)
{
    const auto& rcstats = RCache.stat;

    F.OutNext("Static:       %3.1f verts/[%d] calls", rcstats.r.s_static.verts / 1024.f, rcstats.r.s_static.draw_calls);
    F.OutNext("Lods:         %3.1f verts/[%d] calls", rcstats.r.s_flora_lods.verts / 1024.f, rcstats.r.s_flora_lods.draw_calls);

    F.OutNext("Dynamic:      %3.1f verts/[%d] calls", rcstats.r.s_dynamic.verts / 1024.f, rcstats.r.s_dynamic.draw_calls);
    F.OutNext("- soft:       %3.1f verts/[%d] calls", rcstats.r.s_dynamic_sw.verts / 1024.f, rcstats.r.s_dynamic_sw.draw_calls);
    F.OutNext("- index:      %3.1f verts/[%d] calls", rcstats.r.s_dynamic_inst.verts / 1024.f, rcstats.r.s_dynamic_inst.draw_calls);
    F.OutNext("- 1B:         %3.1f verts/[%d] calls", rcstats.r.s_dynamic_1B.verts / 1024.f, rcstats.r.s_dynamic_1B.draw_calls);
    F.OutNext("- 2B:         %3.1f verts/[%d] calls", rcstats.r.s_dynamic_2B.verts / 1024.f, rcstats.r.s_dynamic_2B.draw_calls);
    F.OutNext("- 3B:         %3.1f verts/[%d] calls", rcstats.r.s_dynamic_3B.verts / 1024.f, rcstats.r.s_dynamic_3B.draw_calls);
    F.OutNext("- 4B:         %3.1f verts/[%d] calls", rcstats.r.s_dynamic_4B.verts / 1024.f, rcstats.r.s_dynamic_4B.draw_calls);

    F.OutNext("Flora:        [%u] instances, [%u] draw calls", rcstats.r.s_flora.instances_count, rcstats.r.s_flora.draw_calls_count);
    F.OutNext("Details:      [%u] instances, [%u] draw calls", rcstats.r.s_details.instances_count, rcstats.r.s_details.draw_calls_count);
}

void dxStatsRender::GuardVerts(CGameFont& F)
{
    if (RCache.stat.verts > 500000)
        F.OutNext("Verts     > 500k: %d", RCache.stat.verts);
}

void dxStatsRender::GuardDrawCalls(CGameFont& F)
{
    if (RCache.stat.calls > 1000)
        F.OutNext("DIP/DP    > 1k:   %d", RCache.stat.calls);
}

void dxStatsRender::SetDrawParams(IRenderDeviceRender* pRender)
{
    dxRenderDeviceRender* pR = (dxRenderDeviceRender*)pRender;

    RCache.set_xform_world(Fidentity);
    RCache.set_Shader(pR->m_SelectionShader);
    RCache.set_c("tfactor", 1, 1, 1, 1);
}
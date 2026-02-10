#include "stdafx.h"
#include "dxStatsRender.h"
#include "../../xr_3da/GameFont.h"
#include "dxRenderDeviceRender.h"

void dxStatsRender::Copy(IStatsRender& _in) { *this = *smart_cast<dxStatsRender*>(&_in); }

void dxStatsRender::OutData(CGameFont& F)
{
    const auto& main_stat = RCache.stat;

    F.OutNext("Total verts:  %d/~ per call %d", main_stat.verts, main_stat.calls ? main_stat.verts / main_stat.calls : 0);
    F.OutNext("Total polys:  %d/~ per call %d", main_stat.polys, main_stat.calls ? main_stat.polys / main_stat.calls : 0);
    F.OutNext("Render calls:    %d", main_stat.calls);

    F.OutSkip();

    F.OutNext("S/T/C:        %d/%d/%d", main_stat.states, main_stat.textures, main_stat.constants);
    F.OutNext("RT/ZB:        %d/%d", main_stat.target_rt, main_stat.target_zb);
    F.OutNext("PS/VS/GS:     %d/%d/%d", main_stat.ps, main_stat.vs, main_stat.gs);
    F.OutNext("HS/DS/CS:     %d/%d/%d", main_stat.hs, main_stat.ds, main_stat.cs);
    F.OutNext("DECL/VB/IB:   %d/%d/%d", main_stat.decl, main_stat.vb, main_stat.ib);

    F.OutSkip();
    
    F.OutNext("xforms:      %d", main_stat.xforms);

    F.OutSkip();

    F.OutNext("Static:       %3.1f verts/[%d] calls", main_stat.r.s_static.verts / 1024.f, main_stat.r.s_static.draw_calls);
    F.OutNext("Lods:         %3.1f verts/[%d] calls", main_stat.r.s_flora_lods.verts / 1024.f, main_stat.r.s_flora_lods.draw_calls);
    F.OutNext("Trees:        [%u] instances, [%3.1f] verts, [%u] draw calls", main_stat.r.s_flora.instances_count, main_stat.r.s_flora.total_verts  / 1024.f, main_stat.r.s_flora.draw_calls_count);
    F.OutNext("Details:      [%u] instances, [%3.1f] verts, [%u] draw calls", main_stat.r.s_details.instances_count, main_stat.r.s_details.total_verts / 1024.f,main_stat.r.s_details.draw_calls_count);

    F.OutSkip();

    F.OutNext("Dynamic:      %3.1f verts/[%d] calls", main_stat.r.s_dynamic.verts / 1024.f, main_stat.r.s_dynamic.draw_calls);
    F.OutNext("- index:      %3.1f verts/[%d] calls", main_stat.r.s_dynamic_inst.verts / 1024.f, main_stat.r.s_dynamic_inst.draw_calls);
    F.OutNext("- 1B:         %3.1f verts/[%d] calls", main_stat.r.s_dynamic_1B.verts / 1024.f, main_stat.r.s_dynamic_1B.draw_calls);
    F.OutNext("- 2B:         %3.1f verts/[%d] calls", main_stat.r.s_dynamic_2B.verts / 1024.f, main_stat.r.s_dynamic_2B.draw_calls);
    F.OutNext("- 3B:         %3.1f verts/[%d] calls", main_stat.r.s_dynamic_3B.verts / 1024.f, main_stat.r.s_dynamic_3B.draw_calls);
    F.OutNext("- 4B:         %3.1f verts/[%d] calls", main_stat.r.s_dynamic_4B.verts / 1024.f, main_stat.r.s_dynamic_4B.draw_calls);

    for (u32 i = 0;i< 3;i++)
    {
        F.OutSkip();

        F.OutNext("Cascade:       %d", i);
        
        const auto& cas_stat = RImplementation.r_sun.stats[i];

        F.OutSkip();
        F.OutNext("Total verts:  %d/~ per call %d", cas_stat.verts, cas_stat.calls ? cas_stat.verts / cas_stat.calls : 0);
        F.OutNext("Total polys:  %d/~ per call %d", cas_stat.polys, cas_stat.calls ? cas_stat.polys / cas_stat.calls : 0);
        F.OutNext("Render calls:    %d", cas_stat.calls);

        F.OutSkip();
        F.OutNext("Static:       %3.1f verts/[%d] calls", cas_stat.r.s_static.verts / 1024.f, cas_stat.r.s_static.draw_calls);
        F.OutNext("Lods:         %3.1f verts/[%d] calls", cas_stat.r.s_flora_lods.verts / 1024.f, cas_stat.r.s_flora_lods.draw_calls);
        F.OutNext("Trees:        [%u] instances, [%3.1f] verts, [%u] draw calls", cas_stat.r.s_flora.instances_count,cas_stat.r.s_flora.total_verts  / 1024.f, cas_stat.r.s_flora.draw_calls_count);
        F.OutNext("Details:      [%u] instances, [%3.1f] verts, [%u] draw calls", cas_stat.r.s_details.instances_count, cas_stat.r.s_details.total_verts / 1024.f,cas_stat.r.s_details.draw_calls_count);
    }
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
    dxRenderDeviceRender* pR = smart_cast<dxRenderDeviceRender*>(pRender);

    RCache.set_xform_world(Fidentity);
    RCache.set_Shader(pR->m_SelectionShader);
    RCache.set_c("tfactor", 1, 1, 1, 1);
}
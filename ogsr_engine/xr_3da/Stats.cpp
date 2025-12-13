#include "stdafx.h"
#include "cpuid.h"
#include "GameFont.h"
#include "../xrcdb/ISpatial.h"
#include "IGame_Persistent.h"
#include "render.h"
#include "xr_object.h"
#include <psapi.h>
#include <mmsystem.h>

#include "../Include/xrRender/DrawUtils.h"

enum DebugTextColor : DWORD
{
    DTC_RED = 0xFFF0672B,
    DTC_YELLOW = 0xFFF6D434,
    DTC_GREEN = 0xFF67F92E,
};

int g_ErrorLineCount = 15;
Flags32 g_stats_flags = {0};

// stats
void rp_Stats(void* p) { static_cast<pureStats*>(p)->OnStats(); };

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BOOL g_bDisableRedText = FALSE;

CStats::CStats()
{
    fFPS = 30.f;
    fRFPS = 30.f;
    fTPS = 0;
    pFont = nullptr;
    RenderDUMP_DT_Count = 0;
    Device.seqRender.Add(this, 4);
}

CStats::~CStats()
{
    Device.seqRender.Remove(this);
    xr_delete(pFont);
}

static void _draw_cam_pos(CGameFont* pFont)
{
    float sz = pFont->GetHeight();
    pFont->SetHeightI(0.02f);
    pFont->SetColor(0xffffffff);
    pFont->Out(10, 600, "CAMERA POSITION:  [%3.2f,%3.2f,%3.2f]", VPUSH(Device.vCameraPosition));
    pFont->SetHeight(sz);
    pFont->OnRender();
}

void CStats::Show()
{
    if (psDeviceFlags.test(rsCameraPos) || psDeviceFlags.test(rsStatistic) || !errors.empty())
    {
        // Stop timers
        {
            EngineTOTAL.FrameEnd();
            Sheduler.FrameEnd();
            UpdateClient.FrameEnd();
            Physics.FrameEnd();
            ph_collision.FrameEnd();
            ph_core.FrameEnd();
            Animation.FrameEnd();
            AI_Think.FrameEnd();
            AI_Range.FrameEnd();
            AI_Path.FrameEnd();
            AI_Node.FrameEnd();
            AI_Vis.FrameEnd();
            AI_Vis_Query.FrameEnd();
            AI_Vis_RayTests.FrameEnd();

            RenderTOTAL.FrameEnd();
            RenderCALC.FrameEnd();
            RenderCALC_HOM.FrameEnd();
            RenderDUMP.FrameEnd();
            RenderDUMP_Wait.FrameEnd();
            RenderDUMP_WM.FrameEnd();
            RenderDUMP_DT_VIS.FrameEnd();
            RenderDUMP_DT_Render.FrameEnd();
            RenderDUMP_DT_Cache.FrameEnd();

            Sound.FrameEnd();
            Input.FrameEnd();
            clRAY.FrameEnd();
            clBOX.FrameEnd();
            clFRUSTUM.FrameEnd();

            BulletManager.FrameEnd();
            cam_Update.FrameEnd();
            dxRainRender.FrameEnd();

            g_SpatialSpace->stat_insert.FrameEnd();
            g_SpatialSpace->stat_remove.FrameEnd();
            g_SpatialSpacePhysic->stat_insert.FrameEnd();
            g_SpatialSpacePhysic->stat_remove.FrameEnd();
        }

        // calc FPS & TPS
        if (Device.fTimeDeltaReal > EPS_S)
        {
            float fps = 1.f / Device.fTimeDeltaReal;

            float fOne = 0.3f;
            float fInv = 1.f - fOne;

            fFPS = fInv * fFPS + fOne * fps;

            if (RenderTOTAL.result > EPS_S)
            {
                u32 rendered_polies = ::Render->GetCacheStatPolys();
                fTPS = fInv * fTPS + fOne * float(rendered_polies) / (RenderTOTAL.result * 1000.f);
                fRFPS = fInv * fRFPS + fOne * 1000.f / RenderTOTAL.result;
            }
        }

        CGameFont& F = *pFont;
        float f_base_size = 0.01f;
        F.SetHeightI(f_base_size);

        // Show them
        if (psDeviceFlags.test(rsStatistic))
        {
            static float r_ps = 0;
            static float b_ps = 0;
            r_ps = .99f * r_ps + .01f * (clRAY.count / clRAY.result);
            b_ps = .99f * b_ps + .01f * (clBOX.count / clBOX.result);

            CSound_stats snd_stat;
            ::Sound->statistic(&snd_stat, nullptr);
            F.SetColor(0xFFFFFFFF);

            F.OutSet(0, 0);

#define PPP(a) (100.f * float(a) / float(EngineTOTAL.result))
            F.OutNext("*** ENGINE:  %2.2fms", EngineTOTAL.result);
            F.OutNext("FPS/RFPS:    %3.1f/%3.1f", fFPS, fRFPS);
            F.OutNext("TPS:         %2.2f M", fTPS);

            F.OutSkip();
            F.OutNext("Clients:     %2.2fms, %2.1f%%", UpdateClient.result, PPP(UpdateClient.result));
            F.OutNext("  crow(%d)/active(%d)/total(%d)", UpdateClient_crows, UpdateClient_active, UpdateClient_total);

            F.OutSkip();
            F.OutNext("Object Scheduler:");
            F.OutNext(" - update:   %2.2fms, %2.1f%%", Sheduler.result, PPP(Sheduler.result));
            F.OutNext(" - load:     %2.2fms", fShedulerLoad);

            if (g_pGamePersistent)
            {
                F.OutSkip();
                F.OutNext("Particles:   start[%d] active[%d] destroy[%d]", Particles_starting, Particles_active, Particles_destroy);

                F.OutNext("spInsert:    o[%.2fms, %2.1f%%], p[%.2fms, %2.1f%%]", 
                            g_SpatialSpace->stat_insert.result, PPP(g_SpatialSpace->stat_insert.result),
                            g_SpatialSpacePhysic->stat_insert.result, PPP(g_SpatialSpacePhysic->stat_insert.result));
                F.OutNext("spRemove:    o[%.2fms, %2.1f%%], p[%.2fms, %2.1f%%]", 
                            g_SpatialSpace->stat_remove.result, PPP(g_SpatialSpace->stat_remove.result),
                            g_SpatialSpacePhysic->stat_remove.result, PPP(g_SpatialSpacePhysic->stat_remove.result));

                F.OutSkip();
                F.OutNext("clRAY:       %2.2fms, %d, %2.0fK", clRAY.result, clRAY.count, r_ps);
                F.OutNext("clBOX:       %2.2fms, %d, %2.0fK", clBOX.result, clBOX.count, b_ps);
                F.OutNext("clFRUSTUM:   %2.2fms, %d", clFRUSTUM.result, clFRUSTUM.count);
            }

            F.OutSkip();
            F.OutNext("Physics:     %2.2fms, %2.1f%%", Physics.result, PPP(Physics.result));
            F.OutNext("  collider:  %2.2fms", ph_collision.result);
            F.OutNext("  solver:    %2.2fms, %d", ph_core.result, ph_core.count);
            F.OutSkip();
            F.OutNext("aiThink:     %2.2fms, %d", AI_Think.result, AI_Think.count);
            F.OutNext("  aiRange:   %2.2fms, %d", AI_Range.result, AI_Range.count);
            F.OutNext("  aiPath:    %2.2fms, %d", AI_Path.result, AI_Path.count);
            F.OutNext("  aiNode:    %2.2fms, %d", AI_Node.result, AI_Node.count);
            F.OutSkip();
            F.OutNext("aiVision:    %2.2fms, %d", AI_Vis.result, AI_Vis.count);
            F.OutNext("  Query:     %2.2fms", AI_Vis_Query.result);
            F.OutNext("  RayCast:   %2.2fms", AI_Vis_RayTests.result);

            F.OutSkip();
            F.OutNext("SOUND:       %2.2fms", Sound.result);
            F.OutNext("  RENDERED/SIMULATED/EVENTS: %d/%d/%d", snd_stat._rendered, snd_stat._simulated, snd_stat._events);
            F.OutSkip();
            F.OutNext("Input:       %2.2fms", Input.result);
            F.OutSkip();
            F.OutNext("BulletManager:   %2.2fms, %d", BulletManager.result, BulletManager.count);
            F.OutNext("cam_Update:      %2.2fms, %d", cam_Update.result, cam_Update.count);
            F.OutNext("dxRainRender:    %2.2fms, %d", dxRainRender.result, dxRainRender.count);
#undef PPP

            //////////////////////////////////////////////////////////////////////////
            // Renderer specific
            F.SetHeightI(f_base_size);
            F.OutSet(450, 0);

            auto renderTotal = RenderTOTAL.result;
#define PPP(a) (100.f * float(a) / renderTotal)
    
            F.OutNext("*** RENDER:  %2.2fms", renderTotal);
            F.OutNext("Calc:        %2.2fms, %2.1f%%", RenderCALC.result, PPP(RenderCALC.result));
            F.OutNext("Skeletons:   %2.2fms, %d", Animation.result, Animation.count);
            F.OutNext("Primitives:  %2.2fms, %2.1f%%", RenderDUMP.result, PPP(RenderDUMP.result));
            F.OutNext("Wait-L:      %2.2fms", RenderDUMP_Wait.result);
            F.OutNext("DT_Vis/Cnt:  %2.2fms/%d", RenderDUMP_DT_VIS.result, RenderDUMP_DT_Count);
            F.OutNext("DT_Render:   %2.2fms", RenderDUMP_DT_Render.result);
            F.OutNext("DT_Cache:    %2.2fms", RenderDUMP_DT_Cache.result);
            F.OutNext("Wallmarks:   %2.2fms, %d/%d - %d", RenderDUMP_WM.result, RenderDUMP_WMS_Count, RenderDUMP_WMD_Count, RenderDUMP_WMT_Count);
            F.OutNext("HOM:         %2.2fms, %d", RenderCALC_HOM.result, RenderCALC_HOM.count);
#undef PPP

            F.OutSkip();            
            m_pRender->OutData1(F);
            F.OutSkip();
            m_pRender->OutData2(F);
            F.OutSkip();
            m_pRender->OutData3(F);
            F.OutSkip();
            m_pRender->OutData4(F);

            F.OutSkip();
            Render->Statistics(&F);

            //////////////////////////////////////////////////////////////////////////
            // Game specific
            F.SetHeightI(f_base_size);
            F.OutSet(600, 0);
            g_pGamePersistent->Statistics(&F);

            //////////////////////////////////////////////////////////////////////////
            // process PURE STATS
            F.SetHeightI(f_base_size);
            seqStats.Process(rp_Stats);
            pFont->OnRender();
        }

        if (/*psDeviceFlags.test(rsStatistic) ||*/ psDeviceFlags.test(rsCameraPos))
        {
            _draw_cam_pos(pFont);

            pFont->OnRender();
        }

#ifdef DEBUG
        //////////////////////////////////////////////////////////////////////////
        // PERF ALERT
        if (!g_bDisableRedText)
        {
            CGameFont& F = *((CGameFont*)pFont);
            F.SetColor(color_rgba(255, 16, 16, 255));
            F.OutSet(300, 300);
            F.SetHeightI(f_base_size * 2);
            if (fFPS < 30)
                F.OutNext("FPS       < 30:   %3.1f", fFPS);

            m_pRender->GuardVerts(F);

            if (psDeviceFlags.test(rsStatistic))
            {
                m_pRender->GuardDrawCalls(F);
                if (RenderDUMP_DT_Count > 1000)
                    F.OutNext("DT_count  > 1000: %u", RenderDUMP_DT_Count);
                F.OutSkip();

                if (Sheduler.result > 3.f)
                    F.OutNext("Update     > 3ms:	%3.1f", Sheduler.result);
                if (UpdateClient.result > 3.f)
                    F.OutNext("UpdateCL   > 3ms: %3.1f", UpdateClient.result);
                if (Physics.result > 5.f)
                    F.OutNext("Physics    > 5ms: %3.1f", Physics.result);
            }
        }

        //////////////////////////////////////////////////////////////////////////
        // Show errors
        if (!g_bDisableRedText && errors.size())
        {
            CGameFont& F = *((CGameFont*)pFont);
            F.SetColor(color_rgba(255, 16, 16, 191));
            F.OutSet(200, 0);
            F.SetHeightI(f_base_size);
#if 0
		for (u32 it=0; it<errors.size(); it++)
			F.OutNext("%s",errors[it].c_str());
#else
            for (u32 it = (u32)_max(int(0), (int)errors.size() - g_ErrorLineCount); it < errors.size(); it++)
                F.OutNext("%s", errors[it].c_str());
#endif
            F.OnRender();
        }
#endif

        {
            EngineTOTAL.FrameStart();
            Sheduler.FrameStart();
            UpdateClient.FrameStart();
            Physics.FrameStart();
            ph_collision.FrameStart();
            ph_core.FrameStart();
            Animation.FrameStart();
            AI_Think.FrameStart();
            AI_Range.FrameStart();
            AI_Path.FrameStart();
            AI_Node.FrameStart();
            AI_Vis.FrameStart();
            AI_Vis_Query.FrameStart();
            AI_Vis_RayTests.FrameStart();

            RenderTOTAL.FrameStart();
            RenderCALC.FrameStart();
            RenderCALC_HOM.FrameStart();
            RenderDUMP.FrameStart();
            RenderDUMP_Wait.FrameStart();
            RenderDUMP_WM.FrameStart();
            RenderDUMP_DT_VIS.FrameStart();
            RenderDUMP_DT_Render.FrameStart();
            RenderDUMP_DT_Cache.FrameStart();

            Sound.FrameStart();
            Input.FrameStart();
            clRAY.FrameStart();
            clBOX.FrameStart();
            clFRUSTUM.FrameStart();

            BulletManager.FrameStart();
            cam_Update.FrameStart();
            dxRainRender.FrameStart();

            g_SpatialSpace->stat_insert.FrameStart();
            g_SpatialSpace->stat_remove.FrameStart();

            g_SpatialSpacePhysic->stat_insert.FrameStart();
            g_SpatialSpacePhysic->stat_remove.FrameStart();
        }
        dwSND_Played = dwSND_Allocated = 0;
        Particles_starting = Particles_active = Particles_destroy = 0;
    }
}

void CStats::Show_HW()
{
    if (psDeviceFlags.test(rsHWInfo))
    {
        static DWORD dwLastFrameTime = 0;
        DWORD dwCurrentTime = timeGetTime();
        if (dwCurrentTime - dwLastFrameTime > 500) //Апдейт раз в полсекунды
        {
            dwLastFrameTime = dwCurrentTime;

            MEMORYSTATUSEX mem;
            mem.dwLength = sizeof(mem);
            GlobalMemoryStatusEx(&mem);

            AvailableMem = (float)mem.ullAvailPhys; // how much phys mem available
            AvailableMem /= (1024 * 1024);

            if (mem.ullTotalPageFile - mem.ullTotalPhys > 0.f)
            {
                AvailablePageFileMem = (float)mem.ullAvailPageFile - (float)mem.ullAvailPhys; // how much pagefile mem available
                AvailablePageFileMem /= (1024 * 1024);
            }

            PhysMemoryUsedPercent = (float)mem.dwMemoryLoad;

            // Getting info by request
            PROCESS_MEMORY_COUNTERS pc;
            std::memset(&pc, 0, sizeof(PROCESS_MEMORY_COUNTERS));
            pc.cb = sizeof(pc);
            if (GetProcessMemoryInfo(GetCurrentProcess(), &pc, sizeof(pc)))
            {
                PagefileUsage = (float)pc.PagefileUsage;
                PagefileUsage /= (1024 * 1024);

                WorkingSetSize = (float)pc.WorkingSetSize;
                WorkingSetSize /= (1024 * 1024);
            }

            // Counting CPU load
            CPU::ID.getCPULoad(cpuLoad);
            CPU::ID.MTCPULoad();
        }

        pFontHW->SetHeightI(0.018f);

        if (AvailableMem < 512 || AvailablePageFileMem < 1596)
            pFontHW->SetColor(DebugTextColor::DTC_RED);
        else if (AvailableMem < 768 || AvailablePageFileMem < 2048)
            pFontHW->SetColor(DebugTextColor::DTC_YELLOW);
        else
            pFontHW->SetColor(DebugTextColor::DTC_GREEN);

        float y = 10.f;
        float dy = 15.f;

        // Draw all your stuff
        pFontHW->Out(10, y += dy, "PHYS MEM AVAILABLE: %0.0fMB", AvailableMem); // Physical memory available
        pFontHW->Out(10, y += dy, "PAGE MEM AVAILABLE: %0.0fMB", AvailablePageFileMem); // Pagefile memory available

        pFontHW->Out(10, y += dy, "Engine Working Set: %0.0fMB", WorkingSetSize); // Physical memory used by app
        pFontHW->Out(10, y += dy, "Engine Commit Charge: %0.0fMB", PagefileUsage); // Physical memory used by app

        if (PhysMemoryUsedPercent > 80.0f)
            pFontHW->SetColor(DebugTextColor::DTC_RED);
        else if (PhysMemoryUsedPercent > 60.0f)
            pFontHW->SetColor(DebugTextColor::DTC_YELLOW);
        else
            pFontHW->SetColor(DebugTextColor::DTC_GREEN);

        pFontHW->Out(10, y += dy, "MEM USED: %0.0f%%", PhysMemoryUsedPercent); // Total Phys. memory load (%)

        if (cpuLoad > 80.0)
            pFontHW->SetColor(DebugTextColor::DTC_RED);
        else if (cpuLoad > 60.0)
            pFontHW->SetColor(DebugTextColor::DTC_YELLOW);
        else
            pFontHW->SetColor(DebugTextColor::DTC_GREEN);

        pFontHW->Out(10, y += dy, "CPU LOAD: %0.0f%%", cpuLoad); // CPU load

        // get MT Load
        float dwScale = y + dy;
        for (size_t i = 0; i < CPU::ID.m_dwNumberOfProcessors; i++)
        {
            pFontHW->Out(10, dwScale, "CPU%u: %0.0f%%", i, CPU::ID.fUsage[i]);
            dwScale += dy;
        }

        pFontHW->OnRender();
    }
}

void CStats::OnDeviceCreate()
{
    pFont = xr_new<CGameFont>("stat_font", CGameFont::fsDeviceIndependent);
    pFontHW = xr_new<CGameFont>("stat_font", CGameFont::fsDeviceIndependent);
}

void CStats::OnDeviceDestroy()
{
    xr_delete(pFont);
    xr_delete(pFontHW);
}

void CStats::OnRender()
{
    if (g_stats_flags.is(st_sound))
    {
        CSound_stats_ext snd_stat_ext;
        ::Sound->statistic(0, &snd_stat_ext);
        CSound_stats_ext::item_vec_it _I = snd_stat_ext.items.begin();
        CSound_stats_ext::item_vec_it _E = snd_stat_ext.items.end();
        for (; _I != _E; ++_I)
        {
            const CSound_stats_ext::SItem& item = *_I;
            if (item._3D)
            {
                m_pRender->SetDrawParams(Device.m_pRender);

                DU->DrawCross(item.params.position, 0.5f, 0xFF0000FF, true);
                if (g_stats_flags.is(st_sound_min_dist))
                    DU->DrawSphere(Fidentity, item.params.position, item.params.min_distance, 0x400000FF, 0xFF0000FF, true, true);
                if (g_stats_flags.is(st_sound_max_dist))
                    DU->DrawSphere(Fidentity, item.params.position, item.params.max_distance, 0x4000FF00, 0xFF008000, true, true);

                xr_string out_txt = g_stats_flags.is(st_sound_info_name) ? item.name.c_str() : "";

                if (item.game_object)
                {
                    if (g_stats_flags.is(st_sound_ai_dist))
                        DU->DrawSphere(Fidentity, item.params.position, item.params.max_ai_distance, 0x80FF0000, 0xFF800000, true, true);
                    if (g_stats_flags.is(st_sound_info_object))
                    {
                        out_txt += "  (";
                        out_txt += item.game_object->cNameSect().c_str();
                        out_txt += ")";
                    }
                }

                if (g_stats_flags.is_any(st_sound_info_name | st_sound_info_object))
                    DU->OutText(item.params.position, out_txt.c_str(), 0xFFFFFFFF, 0xFF000000);
            }
        }
    }

    Show();
    Show_HW();
}

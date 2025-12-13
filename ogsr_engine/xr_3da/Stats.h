// Stats.h: interface for the CStats class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class ENGINE_API CGameFont;

#include "../Include/xrRender/FactoryPtr.h"
#include "../Include/xrRender/StatsRender.h"

extern RP_FUNC rp_Stats;
class pureStats
{
public:
    virtual void OnStats(void) = 0;
};

class ENGINE_API CStatsPhysics
{
public:
    CStatTimer ph_collision; // collision
    CStatTimer ph_core; // integrate
    CStatTimer Physics; // movement+collision
};

class ENGINE_API CStats : public pureRender, public CStatsPhysics
{
    CGameFont* pFont;
    CGameFont* pFontHW;

public:
    float fFPS, fRFPS, fTPS; // FPS, RenderFPS, TPS
    u32 dwMem_calls;
    u32 dwSND_Played, dwSND_Allocated; // Play/Alloc
    float fShedulerLoad;

    CStatTimer EngineTOTAL; //
    CStatTimer Sheduler; //
    CStatTimer UpdateClient; //

    u32 UpdateClient_updated; //
    u32 UpdateClient_crows; //
    u32 UpdateClient_active; //
    u32 UpdateClient_total; //
    u32 Particles_starting; // starting
    u32 Particles_active; // active
    u32 Particles_destroy; // destroying

    CStatTimer AI_Think; // thinking
    CStatTimer AI_Range; // query: range
    CStatTimer AI_Path; // query: path
    CStatTimer AI_Node; // query: node
    CStatTimer AI_Vis; // visibility detection - total
    CStatTimer AI_Vis_Query; // visibility detection - portal traversal and frustum culling
    CStatTimer AI_Vis_RayTests; // visibility detection - ray casting

    CStatTimer RenderTOTAL; //
    CStatTimer RenderTOTAL_Real;
    CStatTimer RenderCALC; // portal traversal, frustum culling, entities "renderable_Render"
    CStatTimer RenderCALC_HOM; // HOM rendering
    CStatTimer Animation; // skeleton calculation
    CStatTimer RenderDUMP; // actual primitive rendering
    CStatTimer RenderDUMP_Wait; // ...waiting something back (queries results, etc.)
    CStatTimer RenderDUMP_WM; // ...wallmark sorting, rendering
    u32 RenderDUMP_WMS_Count; // ...number of static wallmark
    u32 RenderDUMP_WMD_Count; // ...number of dynamic wallmark
    u32 RenderDUMP_WMT_Count; // ...number of wallmark tri
    CStatTimer RenderDUMP_DT_VIS; // ...details visibility detection
    CStatTimer RenderDUMP_DT_Render; // ...details rendering
    CStatTimer RenderDUMP_DT_Cache; // ...details slot cache access
    u32 RenderDUMP_DT_Count; // ...number of DT-elements

    CStatTimer Sound; // total time taken by sound subsystem (accurate only in single-threaded mode)
    CStatTimer Input; // total time taken by input subsystem (accurate only in single-threaded mode)
    CStatTimer clRAY; // total: ray-testing
    CStatTimer clBOX; // total: box query
    CStatTimer clFRUSTUM; // total: frustum query

    CStatTimer BulletManager; // debug counter
    CStatTimer cam_Update; // debug counter
    CStatTimer dxRainRender; // debug counter

private:
    float PhysMemoryUsedPercent;

    float AvailableMem;
    float AvailablePageFileMem;

    float WorkingSetSize;
    float PagefileUsage;

    double cpuLoad{};

public:
    void Show();
    void Show_HW();

    virtual void OnRender();
    void OnDeviceCreate(void);
    void OnDeviceDestroy(void);

public:
    xr_vector<shared_str> errors;
    CRegistrator<pureStats> seqStats;

public:
    CStats();
    ~CStats();

    IC CGameFont* Font() { return pFont; }

private:
    FactoryPtr<IStatsRender> m_pRender;
};

enum
{
    st_sound = (1 << 0),
    st_sound_min_dist = (1 << 1),
    st_sound_max_dist = (1 << 2),
    st_sound_ai_dist = (1 << 3),
    st_sound_info_name = (1 << 4),
    st_sound_info_object = (1 << 5),
};

extern Flags32 g_stats_flags;

#include "StdAfx.h"
#include "r__occlusion.h"
#include "QueryHelper.h"

R_occlusion::R_occlusion() : enabled(!strstr(Core.Params, "-no_occq")), last_frame(Device.dwFrame) {}

R_occlusion::~R_occlusion() { occq_destroy(); }

void R_occlusion::occq_destroy()
{
    Msg("* [%s]: fids[%u] used[%u] pool[%u]", __FUNCTION__, fids.size(), used.size(), pool.size());
    size_t p_cnt = pool.size(), u_cnt = 0;
    for (const auto& it : used)
        if (it.Q)
            u_cnt++;

    used.clear();
    pool.clear();
    fids.clear();

    Msg("* [%s]: released [%u] used and [%u] pool queries", __FUNCTION__, u_cnt, p_cnt);
}

void R_occlusion::cleanup_lost()
{
    u32 cnt = 0;
    for (u32 ID = 0; ID < used.size(); ID++)
    {
        if (used[ID].Q && used[ID].ttl && used[ID].ttl < Device.dwFrame)
        {
            occq_free(ID);
            cnt++;
        }
    }
    if (cnt > 0)
        Msg("! [%s]: cleanup %u lost queries", __FUNCTION__, cnt);
}

u32 R_occlusion::occq_begin(u32& ID)
{
    if (!enabled)
    {
        ID = iInvalidHandle;
        return 0;
    }

    if (last_frame != Device.dwFrame)
    {
        cleanup_lost();
        last_frame = Device.dwFrame;
    }

    RImplementation.stats.o_queries++;
    if (fids.empty())
    {
        ID = u32(used.size());
        _Q q{};
        q.order = ID;
        if (FAILED(CreateQuery(q.Q.GetAddressOf(), D3DQUERYTYPE_OCCLUSION)))
        {
            if (Device.dwFrame % 100 == 0)
                Msg("RENDER [Warning]: Too many occlusion queries were issued: %u !!!", used.size());
            ID = iInvalidHandle;
            return 0;
        }
        used.push_back(std::move(q));
    }
    else
    {
        VERIFY(pool.size() == fids.size());
        ID = fids.back();
        fids.pop_back();
        used[ID].Q = pool.back().Q;
        pool.pop_back();
    }
    used[ID].ttl = Device.dwFrame + 1;
    CHK_DX(BeginQuery(used[ID].Q.Get()));
    return used[ID].order;
}

void R_occlusion::occq_end(u32& ID)
{
    if (!enabled || ID == iInvalidHandle || !used[ID].Q)
        return;
    CHK_DX(EndQuery(used[ID].Q.Get()));
    used[ID].ttl = Device.dwFrame + 1;
}

R_occlusion::occq_result R_occlusion::occq_get(u32& ID)
{
    if (!enabled || ID == iInvalidHandle || !used[ID].Q)
        return 0xffffffff;

    occq_result fragments = 0;
    HRESULT hr;
    CTimer T;
    T.Start();
    Device.Statistic->RenderDUMP_Wait.Begin();
    VERIFY2(ID < used.size(), make_string("_Pos = %d, size() = %d", ID, used.size()));
    // здесь нужно дождаться результата, т.к. отладка показывает, что
    // очень редко когда он готов немедленно
    while ((hr = GetData(used[ID].Q.Get(), &fragments, sizeof(fragments))) == S_FALSE)
    {
        if (!SwitchToThread())
            Sleep(ps_r2_wait_sleep);
        if (T.GetElapsed_ms() > 500)
        {
            fragments = (occq_result)-1; // 0xffffffff;
            break;
        }
    }
    Device.Statistic->RenderDUMP_Wait.End();
    if (hr == D3DERR_DEVICELOST)
        fragments = 0xffffffff;

    if (fragments == 0)
        RImplementation.stats.o_culled++;

    // remove from used and shrink as nesessary
    occq_free(ID);
    ID = 0;

    return fragments;
}

void R_occlusion::occq_free(u32 ID)
{
    if (used[ID].Q)
    {
        pool.push_back(used[ID]);
        used[ID].Q.Reset();
        fids.push_back(std::move(ID));
    }
}

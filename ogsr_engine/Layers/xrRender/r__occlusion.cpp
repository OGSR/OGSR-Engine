#include "StdAfx.h"
#include "r__occlusion.h"
#include "QueryHelper.h"

R_occlusion::R_occlusion() : last_frame(Device.dwFrame)
{
    enabled = true;
}

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

void R_occlusion::cleanup_lost(const bool full_cleanup)
{
    u32 cnt = 0;
    for (u32 ID = 0; ID < used.size(); ID++)
    {
        if (used[ID].Q && used[ID].ttl && (full_cleanup || used[ID].ttl < Device.dwFrame))
        {
            occq_free(ID, true);
            cnt++;
        }
    }
    if (cnt > 0
#if !defined(_DEBUG) && !defined(OGSR_TOTAL_DBG)
        && full_cleanup
#endif
    )
    {
        Msg("! [%s]: %s cleanup [%u] queries", __FUNCTION__, full_cleanup ? "FULL" : "", cnt);
    }
}

void R_occlusion::occq_begin(u32& ID, const u32 context_id)
{
    if (!enabled)
    {
        ID = iInvalidHandle;
        return;
    }

    std::scoped_lock slock(lock);

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
        const auto hr = CreateQuery(q.Q.GetAddressOf(), D3DQUERYTYPE_OCCLUSION);
        if (FAILED(hr))
        {
            Msg("!![%s] CreateQuery returns error: [%s]; used [%u] queries.", __FUNCTION__, Debug.DXerror2string(hr), used.size());
            ID = iInvalidHandle;
            return;
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

    used[ID].ttl = Device.dwFrame + 100;
    R_CHK(BeginQuery(used[ID].Q.Get(), context_id));
}

void R_occlusion::occq_end(const u32& ID, u32 context_id)
{
    if (!enabled || ID == iInvalidHandle)
        return;

    std::scoped_lock slock(lock);

    if (!used.at(ID).Q)
        return;

    R_CHK(EndQuery(used.at(ID).Q.Get(), context_id));
    used.at(ID).ttl = Device.dwFrame + 100;
}

R_occlusion::occq_result R_occlusion::occq_get(u32& ID, const bool for_smapvis)
{
    if (!enabled || ID == iInvalidHandle)
        return OCC_NOT_AVAIL;

    std::scoped_lock slock(lock);

    if (ID >= used.size() || !used.at(ID).Q)
        return OCC_NOT_AVAIL;

    ZoneScoped;

    Device.Statistic->RenderDUMP_Wait.Begin();

    occq_result fragments{OCC_NOT_AVAIL};
    HRESULT hr{};

    if (!for_smapvis)
    {
        ZoneScopedN("occq_get/wait for light_vis");

        hr = GetData(used.at(ID).Q.Get(), &fragments, sizeof(fragments), D3D11_ASYNC_GETDATA_DONOTFLUSH);
        if (hr == S_FALSE)
        {
            fragments = OCC_CONTINUE_WAIT;
        }
    }
    else
    {
        ZoneScopedN("occq_get/wait for light_smapvis");

        while ((hr = GetData(used.at(ID).Q.Get(), &fragments, sizeof(fragments))) == S_FALSE)
        {
            YieldProcessor();
        }
    }

    Device.Statistic->RenderDUMP_Wait.End();

    if (hr != S_FALSE && hr != S_OK)
    {
        Msg("!![%s] GetData returns error: [%s]", __FUNCTION__, Debug.DXerror2string(hr));
        fragments = OCC_NOT_AVAIL;
    }

    if (fragments == 0)
        RImplementation.stats.o_culled++;

    if (fragments != OCC_CONTINUE_WAIT)
    {
        // remove from used and shrink as nesessary
        occq_free(ID);
        ID = 0;
    }

    return fragments;
}

void R_occlusion::occq_free(const u32 ID, const bool get_data)
{
    if (ID == iInvalidHandle)
        return;

    std::scoped_lock slock(lock);

    if (ID < used.size() && used.at(ID).Q)
    {
        if (get_data)
        {
            // Попробуем здесь всегда ждать результат
            HRESULT hr{};
            occq_result fragments{};
            while ((hr = GetData(used.at(ID).Q.Get(), &fragments, sizeof(fragments))) == S_FALSE)
            {
                YieldProcessor();
            }
            if (hr != S_FALSE && hr != S_OK)
            {
                Msg("!![%s] GetData returns error: [%s]", __FUNCTION__, Debug.DXerror2string(hr));
            }
        }

        pool.push_back(used.at(ID));
        used.at(ID).Q.Reset();
        fids.push_back(ID);
    }
}

void Cleanup_R_occlusion() { RImplementation.HWOCC.cleanup_lost(true); }
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

void R_occlusion::cleanup_lost()
{
    u32 cnt = 0;
    for (u32 ID = 0; ID < used.size(); ID++)
    {
        if (used[ID].Q && used[ID].ttl && used[ID].ttl < Device.dwFrame)
        {
            occq_free(ID, true);
            cnt++;
        }
    }
    if (cnt > 0)
        MsgDbg("! [%s]: cleanup %u lost queries", __FUNCTION__, cnt);
}

u32 R_occlusion::occq_begin(u32& ID, u32 context_id)
{
    if (!enabled)
    {
        ID = iInvalidHandle;
        return 0;
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
        q.order = ID;
        if (FAILED(CreateQuery(q.Q.GetAddressOf(), D3DQUERYTYPE_OCCLUSION)))
        {
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
    CHK_DX(BeginQuery(used[ID].Q.Get(), context_id));
    return used[ID].order;
}

void R_occlusion::occq_end(const u32& ID, u32 context_id)
{
    if (!enabled || ID == iInvalidHandle)
        return;

    std::scoped_lock slock(lock);

    if (!used[ID].Q)
        return;

    CHK_DX(EndQuery(used[ID].Q.Get(), context_id));
    used[ID].ttl = Device.dwFrame + 1;
}

#define OCC_NOT_AVAIL R_occlusion::occq_result(-1)

R_occlusion::occq_result R_occlusion::occq_get(u32& ID, float max_wait_occ)
{
    if (!enabled || ID == iInvalidHandle)
        return OCC_NOT_AVAIL;

    std::scoped_lock slock(lock);

    if (!used[ID].Q)
        return OCC_NOT_AVAIL;

    ZoneScoped;

    HRESULT hr{};

    Device.Statistic->RenderDUMP_Wait.Begin();
    VERIFY(ID < used.size(), make_string("_Pos = %d, size() = %d", ID, used.size()));

    occq_result fragments = OCC_NOT_AVAIL;

    {
        CTimer T;
        T.Start();

        ZoneScopedN("occq_get/wait");

        //u32 tries{0};

        // здесь нужно дождаться результата, т.к. отладка показывает, что
        // очень редко когда он готов немедленно
        while ((hr = GetData(used[ID].Q.Get(), &fragments, sizeof(fragments))) == S_FALSE)
        {
            YieldProcessor();
            /*
            if (T.GetElapsed_ms_total() > max_wait_occ || (max_wait_occ <= 1.f && tries++ > 512))
            {
                //Msg("local skip occq_get due to timeout!");

                fragments = OCC_NOT_AVAIL;
                break;
            }
            */
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

    // remove from used and shrink as nesessary
    occq_free(ID);
    ID = 0;

    return fragments;
}

void R_occlusion::occq_free(const u32 ID, const bool get_data)
{
    if (!enabled || ID == iInvalidHandle)
        return;

    std::scoped_lock slock(lock);

    if (used[ID].Q)
    {
        if (get_data)
        {
            // Попробуем здесь всегда ждать результат
            HRESULT hr{};
            occq_result fragments{};
            while ((hr = GetData(used[ID].Q.Get(), &fragments, sizeof(fragments))) == S_FALSE)
            {
                YieldProcessor();
            }
            if (hr != S_FALSE && hr != S_OK)
            {
                Msg("!![%s] GetData returns error: [%s]", __FUNCTION__, Debug.DXerror2string(hr));
            }
        }

        pool.push_back(used[ID]);
        used[ID].Q.Reset();
        fids.push_back(ID);
    }
}

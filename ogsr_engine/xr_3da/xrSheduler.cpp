#include "stdafx.h"
#include "xrSheduler.h"
#include "xr_object.h"

float psShedulerCurrent = 10.f;
float psShedulerTarget = 10.f;
float psShedulerMax = 10.f;
constexpr float psShedulerReaction = 1.f; // 0.1f;

BOOL g_bSheduleInProgress = FALSE;

//-------------------------------------------------------------------------------------
void CSheduler::Initialize() { m_processing_now = false; }

void CSheduler::Destroy()
{
    internal_Registration();

    ItemsRT.clear();
    Items.clear();
    Registration.clear();
}

void CSheduler::internal_Registration()
{
    for (u32 it = 0; it < Registration.size(); it++)
    {
        ItemReg& R = Registration[it];
        if (R.OP)
        {
            // register
            // search for paired "unregister"

            BOOL bFoundAndErased = FALSE;
            for (u32 pair = it + 1; pair < Registration.size(); pair++)
            {
                ItemReg& R_pair = Registration[pair];
                if ((!R_pair.OP) && (R_pair.Object == R.Object))
                {
                    bFoundAndErased = TRUE;
                    Registration.erase(Registration.begin() + pair);
                    break;
                }
            }

            // register if non-paired
            if (!bFoundAndErased)
            {
                //Msg("SCHEDULER: internal register [%s][%x][%s]", *R.Object->shedule_Name(), R.Object, R.RT ? "true" : "false");
                internal_Register(R.Object, R.RT);
            }
            //else
            //    Msg("SCHEDULER: internal register skipped, because unregister found [%s][%x][%s]", "unknown", R.Object, R.RT ? "true" : "false");
        }
        else
        {
            // unregister
            internal_Unregister(R.Object, R.RT);
        }
    }

    Registration.clear();
}

void CSheduler::internal_Register(ISheduled* object, BOOL RT)
{
    VERIFY(!O->shedule.b_locked);

    if (RT)
    {
        // Fill item structure
        auto& TNext = ItemsRT.emplace_back();
        TNext.dwTimeForExecute = Device.dwTimeGlobal;
        TNext.dwTimeOfLastExecute = Device.dwTimeGlobal;
        TNext.Object = object;
        TNext.scheduled_name = object->shedule_Name();
        object->shedule.b_RT = TRUE;
    }
    else
    {
        // Fill item structure && Insert into priority Queue
        auto& TNext = Items.emplace_back();
        TNext.dwTimeForExecute = Device.dwTimeGlobal;
        TNext.dwTimeOfLastExecute = Device.dwTimeGlobal;
        TNext.Object = object;
        TNext.scheduled_name = object->shedule_Name();
        object->shedule.b_RT = FALSE;
    }
}

bool CSheduler::internal_Unregister(const ISheduled* object, BOOL RT)
{
    if (RT)
    {
        for (u32 i = 0; i < ItemsRT.size(); i++)
        {
            if (ItemsRT[i].Object == object)
            {
                ItemsRT.erase(ItemsRT.begin() + i);
                return true;
            }
        }
    }
    else
    {
        for (auto& Item : Items)
        {
            if (Item.Object == object)
            {
                Item.Object = nullptr;
                return true;
            }
        }
    }

    if (m_current_step_obj == object)
    {
        m_current_step_obj = nullptr;
        return true;
    }

    return false;
}

#ifdef DEBUG
bool CSheduler::Registered(ISheduled* object) const
{
    u32 count = 0;
    typedef xr_vector<Item> ITEMS;

    {
        ITEMS::const_iterator I = ItemsRT.begin();
        ITEMS::const_iterator E = ItemsRT.end();
        for (; I != E; ++I)
            if ((*I).Object == object)
            {
                //				Msg				("0x%8x found in RT",object);
                count = 1;
                break;
            }
    }
    {
        ITEMS::const_iterator I = Items.begin();
        ITEMS::const_iterator E = Items.end();
        for (; I != E; ++I)
            if ((*I).Object == object)
            {
                //				Msg				("0x%8x found in non-RT",object);
                VERIFY(!count);
                count = 1;
                break;
            }
    }

    {
        ITEMS::const_iterator I = ItemsProcessed.begin();
        ITEMS::const_iterator E = ItemsProcessed.end();
        for (; I != E; ++I)
            if ((*I).Object == object)
            {
                //				Msg				("0x%8x found in process items",object);
                VERIFY(!count);
                count = 1;
                break;
            }
    }

    typedef xr_vector<ItemReg> ITEMS_REG;
    ITEMS_REG::const_iterator I = Registration.begin();
    ITEMS_REG::const_iterator E = Registration.end();
    for (; I != E; ++I)
    {
        if ((*I).Object == object)
        {
            if ((*I).OP)
            {
                //				Msg				("0x%8x found in registration on register",object);
                VERIFY(!count);
                ++count;
            }
            else
            {
                //				Msg				("0x%8x found in registration on UNregister",object);
                VERIFY(count == 1);
                --count;
            }
        }
    }

    VERIFY(!count || (count == 1));
    return (count == 1);
}
#endif // DEBUG

void CSheduler::Register(ISheduled* A, BOOL RT)
{
    VERIFY(!Registered(A));

    auto& R = Registration.emplace_back();
    R.OP = TRUE;
    R.RT = RT;
    R.Object = A;
    R.Object->shedule.b_RT = RT;

    //Msg("SCHEDULER: register [%s][%x]", *A->shedule_Name(), A);
}

void CSheduler::Unregister(ISheduled* A, bool force)
{
    VERIFY(Registered(A));

    if (m_processing_now || force)
    {
        if (internal_Unregister(A, A->shedule.b_RT))
            return;
    }

    auto& R = Registration.emplace_back();
    R.OP = FALSE;
    R.RT = A->shedule.b_RT;
    R.Object = A;
}

void CSheduler::ProcessStep()
{
    ZoneScoped;

    // Normal priority
    u32 dwTime = Device.dwTimeGlobal;

    const bool prefetch = Device.dwPrecacheFrame > 0;
    decltype(Items) ItemsProcessed;
    bool stopped{};
    //size_t cnt{};
    CTimer t_total;
    t_total.Start();

    for (size_t it{}; it < Items.size();)
    {
        Item curr = Items.at(it++);
        bool skip{curr.Object == nullptr}, shed_need{true};

        if (curr.dwTimeForExecute >= dwTime)
        {
            continue;
        }

        if (!skip)
        {
            __try
            {
                shed_need = curr.Object->shedule_Needed();
            }
            __except (ExceptStackTrace("[CSheduler::ProcessStep] stack trace:\n"))
            {
                Msg("Scheduler tried to update object %s", *curr.scheduled_name);
                skip = true;
            }
        }

        Items.erase(Items.begin() + (--it));

        if (skip || !shed_need)
        {
            continue;
        }

        __try
        {
            // Calc next update interval
            const u32 dwMin = std::max(30u, curr.Object->shedule.t_min);
            const u32 dwMax = (1000u + curr.Object->shedule.t_max) / 2;

            const float scale = curr.Object->shedule_Scale();

            u32 dwUpdate = dwMin + iFloor(float(dwMax - dwMin) * scale);
            clamp(dwUpdate, std::max(dwMin, 20u), dwMax);

            const u32 elapsed = dwTime - curr.dwTimeOfLastExecute;

            m_current_step_obj = curr.Object;

            // if (!Core.DebugFlags.test(xrCore::dbg_DisableObjectsScheduler))
            curr.Object->shedule_Update(std::clamp(elapsed, 1u, std::max(curr.Object->shedule.t_max, 1000u)));

            if (!m_current_step_obj)
            {
                continue;
            }

            m_current_step_obj = nullptr;

            // Fill item structure
            auto& next = ItemsProcessed.emplace_back();
            next.dwTimeForExecute = dwTime + dwUpdate;
            next.dwTimeOfLastExecute = dwTime;
            next.Object = curr.Object;
            next.scheduled_name = curr.Object->shedule_Name();

            //cnt++;
        }
        __except (ExceptStackTrace("[CSheduler::ProcessStep2] stack trace:\n"))
        {
            Msg("Scheduler tried to update object %s", *curr.scheduled_name);
            curr.Object = nullptr;
            continue;
        }

        if (!prefetch && t_total.GetElapsed_ms() > static_cast<u32>(std::floor(psShedulerCurrent)))
        {
            // we have maxed out the load - increase heap
            psShedulerTarget += (psShedulerReaction * 3);

            // if (Core.DebugFlags.test(xrCore::dbg_TraceScheduler))
            {
                // Msg("Break ProcessStep. Processed: [%u], left in queue: [%u]", ItemsProcessed.size(), Items.size());
                if (ItemsProcessed.size() == 1) // кто то жрет все время на кадре
                    Msg("! Single item [%s] took whole update frame!!!", ItemsProcessed.front().scheduled_name.c_str());
            }

            stopped = true;
            break;
        }
    }

    //if (/*Core.DebugFlags.test(xrCore::dbg_TraceScheduler) &&*/ !prefetch && t_total.GetElapsed_ms() > 20)
    //    Msg("Long ProcessStep !!! duration [%u]ms. updated: [%u] objects!", t_total.GetElapsed_ms(), cnt);

    //if (prefetch)
    //    Msg("Prefetch frame, updated: [%u] objects!", cnt);

    // Push "processed" back
    Items.insert(Items.end(), std::make_move_iterator(ItemsProcessed.begin()), std::make_move_iterator(ItemsProcessed.end()));

    if (!stopped)
    {
        // always try to decrease target
        psShedulerTarget -= psShedulerReaction;
    }
}

void CSheduler::Update()
{
    ZoneScoped;

    // Initialize
    Device.Statistic->Sheduler.Begin();

    internal_Registration();

    g_bSheduleInProgress = TRUE;

    m_processing_now = true;

    u32 dwTime = Device.dwTimeGlobal;

    {
        ZoneScopedN("ItemsRT");

        // Realtime priority
        for (auto& curr : ItemsRT)
        {
            R_ASSERT(curr.Object);

            if (!curr.Object->shedule_Needed())
            {
                curr.dwTimeOfLastExecute = dwTime;
                continue;
            }

            const u32 elapsed = dwTime - curr.dwTimeOfLastExecute;
            curr.Object->shedule_Update(elapsed);
            curr.dwTimeOfLastExecute = dwTime;
        }
    }

    // Normal (sheduled)
    ProcessStep();

    clamp(psShedulerTarget, 3.f, psShedulerMax);

    psShedulerCurrent = 0.9f * psShedulerCurrent + 0.1f * psShedulerTarget;
    Device.Statistic->fShedulerLoad = psShedulerCurrent;

    m_processing_now = false;

    // Finalize
    g_bSheduleInProgress = FALSE;

    internal_Registration();

    Device.Statistic->Sheduler.End();
}

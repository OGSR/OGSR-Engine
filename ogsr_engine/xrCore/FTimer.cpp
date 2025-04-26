#include "stdafx.h"

XRCORE_API BOOL g_bEnableStatGather = FALSE;

void CStatTimer::FrameStart()
{
    accum = Duration();
    count = 0;
}

void CStatTimer::FrameEnd() { result = static_cast<float>(GetElapsed_ms()); }

pauseMngr* g_pauseMngr;

pauseMngr& g_pauseMngr_get()
{
    static pauseMngr manager;
    g_pauseMngr = &manager;

    return manager;
}

pauseMngr::pauseMngr() : paused(false) { m_timers.reserve(3); }

void pauseMngr::Pause(const bool b)
{
    if (paused == b)
        return;

    for (auto& timer : m_timers)
    {
        timer->Pause(b);
    }

    paused = b;
}

void pauseMngr::Register(CTimer_paused& t) { m_timers.push_back(&t); }

void pauseMngr::UnRegister(CTimer_paused& t)
{
    const auto it = std::find(m_timers.cbegin(), m_timers.cend(), &t);
    if (it != m_timers.end())
        m_timers.erase(it);
}

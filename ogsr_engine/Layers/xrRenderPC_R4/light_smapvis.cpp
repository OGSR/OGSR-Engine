#include "StdAfx.h"
#include "../xrRender/light.h"
#include "../xrRender/FBasicVisual.h"

smapvis::smapvis()
{
    invalidate();
    frame_sleep = 0;
}

smapvis::~smapvis()
{
    flushoccq();
    invalidate();
}

void smapvis::invalidate()
{
    state = state_counting;
    testQ_V = nullptr;
    frame_sleep = Device.dwFrame + ps_r__LightSleepFrames;
    invisible.clear();
}

void smapvis::begin()
{
    auto& dsgraph = RImplementation.get_context(context_id);
    dsgraph.clear_Counters();
    switch (state)
    {
    case state_counting:
        // do nothing -> we just prepare for testing process
        break;
    case state_working:
        // mark already known to be invisible visuals, set breakpoint
        testQ_V = nullptr;
        testQ_id = 0;
        mark();
        dsgraph.set_Feedback(this, test_current);
        break;
    case state_usingTC:
        // just mark
        mark();
        break;
    }
}
void smapvis::end()
{
    auto& dsgraph = RImplementation.get_context(context_id);

    // Gather stats
    u32 ts;
    dsgraph.get_Counters(ts);
    RImplementation.stats.ic_total += ts;
    dsgraph.set_Feedback(nullptr, 0);

    switch (state)
    {
    case state_counting:
        // switch to 'working'
        if (sleep())
        {
            test_count = ts;
            test_current = 0;
            state = state_working;
        }
        break;
    case state_working:
        // feedback should be called at this time -> clear feedback
        // issue query
        if (testQ_V)
        {
            RImplementation.occq_begin(testQ_id, dsgraph.cmd_list.context_id);

            dsgraph.marker++;
            dsgraph.r_dsgraph_insert_static(testQ_V);
            dsgraph.r_dsgraph_render_graph(0);

            RImplementation.occq_end(testQ_id, dsgraph.cmd_list.context_id);
            testQ_frame = Device.dwFrame + 1; // get result on next frame
        }
        break;
    case state_usingTC:
        // nothing to do
        break;
    }
}

void smapvis::flushoccq()
{
    // the tough part
    if (testQ_frame != Device.dwFrame)
        return;

    if (state != state_working || !testQ_V)
        return;

    ZoneScoped;

    const u64 fragments = RImplementation.occq_get(testQ_id, 6.f);
    if (fragments == 0)
    {
        // this is invisible shadow-caster, register it
        // next time we will not get this caster, so 'test_current' remains the same
        invisible.push_back(testQ_V);
        test_count--;
    }
    else
    {
        // this is visible shadow-caster, advance testing
        test_current++;
    }

    testQ_V = nullptr;

    if (test_current == test_count)
    {
        // we are at the end of list
        if (state == state_working)
            state = state_usingTC;
    }
}

void smapvis::resetoccq()
{
    if (testQ_frame == (Device.dwFrame + 1))
        testQ_frame--;
    flushoccq();
}

void smapvis::mark() const
{
    if (invisible.empty())
        return;

    RImplementation.stats.ic_culled += invisible.size();

    const auto& dsgraph = RImplementation.get_context(context_id);
    const u32 marker = dsgraph.marker + 1; // we are called before marker increment
    for (auto& it : invisible)
        it->getVisData().marker[context_id] = marker; // this effectively disables processing
}

void smapvis::rfeedback_static(dxRender_Visual* V)
{
    testQ_V = V;
    auto& dsgraph = RImplementation.get_context(context_id);
    dsgraph.set_Feedback(nullptr, 0);
}

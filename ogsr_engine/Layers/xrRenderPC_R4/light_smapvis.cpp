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
    finish();
}

void smapvis::invalidate()
{
    state = state_sleeping;
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
    case state_sleeping:
        // do nothing -> we just prepare for testing process
        return;
    case state_working:
        // mark already known to be invisible visuals, set breakpoint
        testQ_V = nullptr;
        dsgraph.set_Feedback(this, test_current);
        break;
    case state_using:
        // using current visibility results,
        break;
    case state_waiting:
        // wating for occ query result, do nothing
        break;
    }

    // mark
    mark_invisible();
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
    case state_sleeping:
        // switch to 'working'
        if (ready_to_work())
        {
            test_total = ts;
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

            state = state_waiting;
        }
        break;
    case state_using:
        // nothing to do
        break;
    case state_waiting:
        // wating for occ query result, do nothing
        break;
    }
}

void smapvis::flush()
{
    if (state != state_waiting)
        return;

    ZoneScoped;

    const auto fragments = RImplementation.occq_get(testQ_id);
    if (fragments != R_occlusion::OCC_CONTINUE_WAIT && fragments != R_occlusion::OCC_NOT_AVAIL)
    {
        handle_occ_result(fragments);
    }
}

void smapvis::finish()
{
    if (state == state_waiting)
    {
        ZoneScoped;

        const auto fragments = RImplementation.occq_free(testQ_id, true);
        handle_occ_result(fragments);
    }
    invalidate();
}

void smapvis::handle_occ_result(const R_occlusion::occq_result fragments)
{
    state = state_working; // back to 'working' state, we are ready for next test

    if (fragments == 0)
    {
        // this is invisible shadow-caster, register it
        // next time we will not get this caster, so 'test_current' remains the same
        invisible.push_back(testQ_V);

        test_total--;
    }
    else
    {
        // this is visible shadow-caster, advance testing
        test_current++;
    }

    testQ_V = nullptr;

    if (test_current == test_total) // we tested all casters, switch to 'using' state
    {
        // we are at the end of list
        if (state == state_working)
            state = state_using;
    }
}

void smapvis::mark_invisible() const
{
    if (invisible.empty())
        return;

    RImplementation.stats.ic_culled += invisible.size();

    const auto& dsgraph = RImplementation.get_context(context_id);
    const u32 marker = dsgraph.marker + 1; // we are called before marker increment
    for (auto& it : invisible)
        it->getVisData().marker[context_id] = marker; // this effectively disables processing
}

void smapvis::feedback_callback(dxRender_Visual* V)
{
    testQ_V = V;
    auto& dsgraph = RImplementation.get_context(context_id);
    dsgraph.set_Feedback(nullptr, 0);
}

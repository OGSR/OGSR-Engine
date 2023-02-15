#include "stdafx.h"
#include "PHCommander.h"

CPHCall::CPHCall(CPHCondition* condition, CPHAction* action)
{
    m_action = action;
    m_condition = condition;
    paused = 0;
    removed = false;
}

CPHCall::~CPHCall()
{
    xr_delete(m_action);
    xr_delete(m_condition);
}
bool CPHCall::obsolete() { return m_action->obsolete() || m_condition->obsolete(); }

void CPHCall::check()
{
    if (m_condition->is_true() && m_action)
        m_action->run();
}

bool CPHCall::equal(CPHReqComparerV* cmp_condition, CPHReqComparerV* cmp_action) { return m_action->compare(cmp_action) && m_condition->compare(cmp_condition); }
bool CPHCall::is_any(CPHReqComparerV* v) { return m_action->compare(v) || m_condition->compare(v); }

void CPHCall::setPause(u32 ms) { paused = Device.dwTimeGlobal + ms; }

bool CPHCall::isPaused() { return paused > Device.dwTimeGlobal; }

void CPHCall::removeLater() { removed = true; }

bool CPHCall::isNeedRemove() { return removed; }

/////////////////////////////////////////////////////////////////////////////////
CPHCommander::~CPHCommander() {}

void CPHCommander::clear()
{
    m_calls.clear();
}

void CPHCommander::update()
{
    for (size_t it{}; it < m_calls.size(); ++it)
    {
        auto& call = m_calls.at(it);
        if (!call->isNeedRemove() && !call->isPaused())
            call->check();
    }

    m_calls.erase(std::remove_if(m_calls.begin(), m_calls.end(), [](auto& call) { return call->isNeedRemove() || call->obsolete(); }), m_calls.end());
}

CPHCall* CPHCommander::add_call(CPHCondition* condition, CPHAction* action) { return m_calls.emplace_back(std::make_unique<CPHCall>(condition, action)).get(); }

PHCALL_I CPHCommander::find_call(CPHReqComparerV* cmp_condition, CPHReqComparerV* cmp_action)
{
    return std::find_if(m_calls.begin(), m_calls.end(), [&](auto& call) { return !call->isNeedRemove() && call->equal(cmp_condition, cmp_action); });
}

bool CPHCommander::has_call(CPHReqComparerV* cmp_condition, CPHReqComparerV* cmp_action) { return find_call(cmp_condition, cmp_action) != m_calls.end(); }

void CPHCommander::remove_call(CPHReqComparerV* cmp_condition, CPHReqComparerV* cmp_action)
{
    auto it = find_call(cmp_condition, cmp_action);
    if (it != m_calls.end())
    {
        auto call = it->get();
        call->removeLater();
    }
}

CPHCall* CPHCommander::add_call_unique(CPHCondition* condition, CPHReqComparerV* cmp_condition, CPHAction* action, CPHReqComparerV* cmp_action)
{
    auto it = find_call(cmp_condition, cmp_action);
    if (it == m_calls.end())
        return add_call(condition, action);
    return it->get();
}

void CPHCommander::remove_calls(CPHReqComparerV* cmp_object)
{
    for (const auto& call : m_calls)
        if (!call->isNeedRemove() && call->is_any(cmp_object))
            call->removeLater();
}

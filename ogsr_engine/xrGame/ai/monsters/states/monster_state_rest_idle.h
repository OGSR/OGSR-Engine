#pragma once

#include "../state.h"

template <typename _Object>
class CStateMonsterRestIdle : public CState<_Object>
{
    typedef CState<_Object> inherited;
    typedef CState<_Object>* state_ptr;
    using inherited::add_state;
    using inherited::current_substate;
    using inherited::get_state;
    using inherited::get_state_current;
    using inherited::object;
    using inherited::prev_substate;
    using inherited::select_state;

    u32 m_target_node{};

public:
    CStateMonsterRestIdle(_Object* obj);
    virtual void initialize();
    virtual void finalize();
    virtual void critical_finalize();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }

    virtual void reselect_state();
    virtual void setup_substates();
};

#include "monster_state_rest_idle_inline.h"

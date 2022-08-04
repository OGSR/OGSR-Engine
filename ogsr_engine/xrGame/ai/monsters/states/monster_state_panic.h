#pragma once

#include "../state.h"

template <typename _Object>
class CStateMonsterPanic : public CState<_Object>
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

public:
    CStateMonsterPanic(_Object* obj);
    virtual ~CStateMonsterPanic();

    virtual void initialize();
    virtual void reselect_state();
    virtual void check_force_state();
    virtual void setup_substates();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};

#include "monster_state_panic_inline.h"

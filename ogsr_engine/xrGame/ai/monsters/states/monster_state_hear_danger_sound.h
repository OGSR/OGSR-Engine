#pragma once

#include "../state.h"

template <typename _Object>
class CStateMonsterHearDangerousSound : public CState<_Object>
{
protected:
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
    CStateMonsterHearDangerousSound(_Object* obj);
    virtual ~CStateMonsterHearDangerousSound() {}

    virtual void reselect_state();
    virtual void setup_substates();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};

#include "monster_state_hear_danger_sound_inline.h"
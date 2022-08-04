#pragma once

#include "../state.h"

template <typename _Object>
class CStateCustomGroup : public CState<_Object>
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
    CStateCustomGroup(_Object* obj);
    virtual ~CStateCustomGroup();

    virtual void execute();
    virtual void setup_substates();
    virtual bool check_completion() { return (object->b_state_end); }
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};

#include "group_state_custom_inline.h"
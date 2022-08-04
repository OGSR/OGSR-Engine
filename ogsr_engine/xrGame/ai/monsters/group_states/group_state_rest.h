#pragma once

#include "../state.h"
#include "../../../entitycondition.h"
#include "../states/state_data.h"
template <typename _Object>
class CStateGroupRest : public CState<_Object>
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

    u32 time_for_life;
    u32 time_for_sleep;

public:
    CStateGroupRest(_Object* obj);
    virtual ~CStateGroupRest();

    virtual void initialize();
    virtual void execute();
    virtual void finalize();
    virtual void critical_finalize();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};

#include "group_state_rest_inline.h"
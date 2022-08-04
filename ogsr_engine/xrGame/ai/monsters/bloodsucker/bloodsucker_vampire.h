#pragma once
#include "../state.h"

template <typename _Object>
class CStateBloodsuckerVampire : public CState<_Object>
{
    typedef CState<_Object> inherited;
    typedef CState<_Object>* state_ptr;
    using inherited::current_substate;
    using inherited::get_state;
    using inherited::get_state_current;
    using inherited::object;
    using inherited::prev_substate;
    using inherited::select_state;

    const CEntityAlive* enemy;

public:
    CStateBloodsuckerVampire(_Object* obj);

    virtual void reinit();

    virtual void initialize();
    virtual void reselect_state();
    virtual void finalize();
    virtual void critical_finalize();
    virtual bool check_start_conditions();
    virtual bool check_completion();

    virtual void setup_substates();
    virtual void check_force_state();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};

#include "bloodsucker_vampire_inline.h"

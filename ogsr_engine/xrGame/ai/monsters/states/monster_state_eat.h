#pragma once

#include "../state.h"

template <typename _Object>
class CStateMonsterEat : public CState<_Object>
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

    const CEntityAlive* corpse;

    u32 m_time_last_eat{};

public:
    CStateMonsterEat(_Object* obj);
    virtual ~CStateMonsterEat();

    virtual void reinit();
    virtual void initialize();
    virtual void finalize();
    virtual void critical_finalize();
    virtual void remove_links(CObject* object);

    virtual void reselect_state();
    virtual void setup_substates();
    virtual bool check_completion();
    virtual bool check_start_conditions();

private:
    bool hungry();
};

#include "monster_state_eat_inline.h"

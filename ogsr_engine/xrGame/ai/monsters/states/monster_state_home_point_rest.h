#pragma once
#include "../state.h"

template <typename _Object>
class CStateMonsterRestMoveToHomePoint : public CStateMove<_Object>
{
    typedef CStateMove<_Object> inherited;
    typedef CStateMove<_Object>* state_ptr;
    using inherited::inherited::object;

    u32 m_target_node{};

public:
    CStateMonsterRestMoveToHomePoint(_Object* obj) : inherited(obj) {}
    virtual void initialize();
    virtual void execute();
    virtual bool check_start_conditions();
    virtual bool check_completion();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }
};

#include "monster_state_home_point_rest_inline.h"

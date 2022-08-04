#pragma once
#include "../state.h"
#include "state_data.h"

template <typename _Object>
class CStateMonsterLookToPoint : public CState<_Object>
{
    typedef CState<_Object> inherited;
    using inherited::object;
    using inherited::time_state_started;

    SStateDataLookToPoint data;

public:
    CStateMonsterLookToPoint(_Object* obj);
    virtual ~CStateMonsterLookToPoint();

    virtual void initialize();
    virtual void execute();
    virtual void remove_links(CObject* object) { inherited::remove_links(object); }

    virtual bool check_completion();
};

#include "state_look_point_inline.h"
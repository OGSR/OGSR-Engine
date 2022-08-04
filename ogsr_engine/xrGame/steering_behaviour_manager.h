////////////////////////////////////////////////////////////////////////////
//	Module 		: steering_behaviour_manager.h
//	Created 	: 07.11.2007
//  Modified 	: 07.11.2007
//	Author		: Dmitriy Iassenev
//	Description : steering behaviour manager class
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "smart_cover_detail.h"

class CAI_Rat;
private
debug::make_final<manager>
{
public:
    manager(CAI_Rat const* object);
    manager(const& manager) = delete;
    const& operator=(const manager&) = delete;
    ~manager();
    void add(base * behaviour, float const& factor);
    void remove(base * behaviour);
    Fvector new_position(float const& time_delta);

private:
    void clear();

private:
    typedef xr_map<base*, float> Behaviours;

private:
    Behaviours m_behaviours;
};
} // namespace steering_behaviour

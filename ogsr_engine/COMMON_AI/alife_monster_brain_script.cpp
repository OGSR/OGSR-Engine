////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_monster_brain_script.cpp
//	Created 	: 02.11.2005
//  Modified 	: 02.11.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife monster brain class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_monster_brain.h"
#include "alife_monster_movement_manager.h"

using namespace luabind;

CALifeMonsterMovementManager* get_movement(const CALifeMonsterBrain* brain) { return (&brain->movement()); }

#pragma optimize("s", on)
void CALifeMonsterBrain::script_register(lua_State* L)
{
    module(L)[class_<CALifeMonsterBrain>("CALifeMonsterBrain")
                  .def("movement", &get_movement)
                  .def("update", &CALifeMonsterBrain::update_script)
                  .def("can_choose_alife_tasks", (void(CALifeMonsterBrain::*)(bool)) & CALifeMonsterBrain::can_choose_alife_tasks)
                  .def("can_choose_alife_tasks", (bool(CALifeMonsterBrain::*)() const) & CALifeMonsterBrain::can_choose_alife_tasks)
                  .def_readwrite("m_time_interval", &CALifeMonsterBrain::m_time_interval)
                  .def_readwrite("m_last_search_time", &CALifeMonsterBrain::m_last_search_time)];
}

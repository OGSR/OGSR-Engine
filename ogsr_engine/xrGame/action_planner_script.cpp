////////////////////////////////////////////////////////////////////////////
//	Module 		: action_planner_script.cpp
//	Created 	: 28.01.2004
//  Modified 	: 10.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Action planner script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_action_planner_wrapper.h"
#include "script_game_object.h"

using namespace luabind;

void set_goal_world_state(CScriptActionPlanner* action_planner, CScriptActionPlanner::CState* world_state) { action_planner->set_target_state(*world_state); }

bool get_actual(const CScriptActionPlanner* action_planner) { return (action_planner->actual()); }

#pragma optimize("s", on)
void CActionPlanner<CScriptGameObject>::script_register(lua_State* L)
{
    module(L)[class_<CScriptActionPlanner, CScriptActionPlannerWrapper>("action_planner")
                  .def_readonly("object", &CScriptActionPlanner::m_object)
                  .def_readonly("storage", &CScriptActionPlanner::m_storage)
                  .def(constructor<>())
                  .def("actual", &get_actual)
                  .def("setup", &CScriptActionPlanner::setup, &CScriptActionPlannerWrapper::setup_static)
                  .def("update", &CScriptActionPlanner::update, &CScriptActionPlannerWrapper::update_static)
#ifdef LUABIND_09
                  .def("add_action", &CScriptActionPlanner::add_operator, adopt(_3))
#else
                  .def("add_action", &CScriptActionPlanner::add_operator, adopt<3>())
#endif
                  .def("remove_action", (void(CScriptActionPlanner::*)(const CScriptActionPlanner::_edge_type&))(&CScriptActionPlanner::remove_operator))
                  .def("action", &CScriptActionPlanner::action)
#ifdef LUABIND_09
                  .def("add_evaluator", &CScriptActionPlanner::add_evaluator, adopt(_3))
#else
                  .def("add_evaluator", &CScriptActionPlanner::add_evaluator, adopt<3>())
#endif
                  .def("remove_evaluator", (void(CScriptActionPlanner::*)(const CScriptActionPlanner::_condition_type&))(&CScriptActionPlanner::remove_evaluator))
                  .def("evaluator", &CScriptActionPlanner::evaluator)
                  .def("current_action_id", &CScriptActionPlanner::current_action_id)
                  .def("current_action", &CScriptActionPlanner::current_action)
                  .def("initialized", &CScriptActionPlanner::initialized)
                  .def("set_goal_world_state", &set_goal_world_state)
    // KRodin: Это теперь не нужно, т.к. планировщик НПС теперь принудительно останавливается при их смерти.
    //.def("clear",						&CScriptActionPlanner::clear)
#ifdef LOG_ACTION
                  .def("show", &CScriptActionPlanner::show)
#endif
    ];
}

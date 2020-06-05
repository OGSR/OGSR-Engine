////////////////////////////////////////////////////////////////////////////
//	Module 		: action_planner_action_inline.h
//	Created 	: 28.01.2004
//  Modified 	: 10.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Action planner action inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <typename _object_type>
#define CPlanner				CActionPlannerAction<_object_type>

TEMPLATE_SPECIALIZATION
IC	CPlanner::CActionPlannerAction	(_object_type *object, LPCSTR action_name) :
	inherited_action					(object,action_name)
{
}

TEMPLATE_SPECIALIZATION
CPlanner::~CActionPlannerAction		()
{
}

TEMPLATE_SPECIALIZATION
void CPlanner::setup				(_object_type *object, CPropertyStorage *storage)
{
	inherited_planner::setup			(object);
	inherited_action::setup				(object,storage);
	set_target_state					(effects());
}

TEMPLATE_SPECIALIZATION
void CPlanner::initialize			()
{
	inherited_action::initialize		();
}

TEMPLATE_SPECIALIZATION
void CPlanner::finalize				()
{
	if (current_action_id() != _action_id_type(-1))
		current_action().finalize();

	inherited_action::finalize			();
	m_initialized						= false;
}

TEMPLATE_SPECIALIZATION
bool CPlanner::completed			() const
{
	return								(inherited_action::completed());
}

TEMPLATE_SPECIALIZATION
IC	void CPlanner::set_use_log		(bool value)
{
	inherited_action::set_use_log		(value);
	inherited_planner::set_use_log		(value);
}

TEMPLATE_SPECIALIZATION
void CPlanner::execute				()
{
	inherited_action::execute			();
	update								();
}

TEMPLATE_SPECIALIZATION
IC	void CPlanner::add_condition	(_world_operator *action, _condition_type condition_id, _value_type condition_value)
{
	inherited_planner::add_condition	(action,condition_id,condition_value);
}

TEMPLATE_SPECIALIZATION
IC	void CPlanner::add_effect		(_world_operator *action, _condition_type condition_id, _value_type condition_value)
{
	inherited_planner::add_effect		(action,condition_id,condition_value);
}

TEMPLATE_SPECIALIZATION
IC	void CPlanner::show				(LPCSTR offset)
{
	inherited_action::show	(offset);
	inherited_planner::show	(offset);
}

#undef TEMPLATE_SPECIALIZATION
#undef CPlanner
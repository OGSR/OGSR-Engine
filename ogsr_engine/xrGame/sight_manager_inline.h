////////////////////////////////////////////////////////////////////////////
//	Module 		: sight_manager_inline.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Sight manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	bool CSightManager::use_torso_look	() const
{
	if (!m_actions.empty())
		return				(current_action().use_torso_look());
	else
		return				(true);
}

IC	bool CSightManager::turning_in_place() const
{
	return					(m_turning_in_place);
}

IC	bool CSightManager::enabled			() const
{
	return					(m_enabled);
}

IC	void CSightManager::enable			(bool value)
{
	m_enabled				= value;
}

template <typename T1, typename T2, typename T3>
IC	void CSightManager::setup			(T1 _1, T2 _2, T3 _3)
{
	setup(CSightAction(_1,_2,_3));
}

template <typename T1, typename T2>
IC	void CSightManager::setup			(T1 _1, T2 _2)
{
	setup(CSightAction(_1,_2));
}

template <typename T1>
IC	void CSightManager::setup			(T1 _1)
{
	setup(CSightAction(_1));
}


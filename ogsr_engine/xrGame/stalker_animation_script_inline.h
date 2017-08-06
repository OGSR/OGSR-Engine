////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_animation_script_inline.h
//	Created 	: 25.02.2003
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker animation script inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CStalkerAnimationScript::CStalkerAnimationScript				(
		const MotionID &animation,
		bool hand_usage,
		bool use_movement_controller
	) :
	m_animation					(animation),
	m_hand_usage				(hand_usage),
	m_use_movement_controller	(use_movement_controller)
{
}

IC	const MotionID &CStalkerAnimationScript::animation				() const
{
	return						(m_animation);
}

IC	const bool &CStalkerAnimationScript::hand_usage					() const
{
	return						(m_hand_usage);
}

IC	const bool &CStalkerAnimationScript::use_movement_controller	() const
{
	return						(m_use_movement_controller);
}

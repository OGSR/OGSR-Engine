////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_animation_manager_inline.h
//	Created 	: 25.02.2003
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker animation manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	const CStalkerAnimationManager::SCRIPT_ANIMATIONS &CStalkerAnimationManager::script_animations	() const
{
	return							(m_script_animations);
}

IC	CStalkerAnimationPair &CStalkerAnimationManager::global	()
{
	return							(m_global);
}

IC	CStalkerAnimationPair &CStalkerAnimationManager::head	()
{
	return							(m_head);
}

IC	CStalkerAnimationPair &CStalkerAnimationManager::torso	()
{
	return							(m_torso);
}

IC	CStalkerAnimationPair &CStalkerAnimationManager::legs	()
{
	return							(m_legs);
}

IC	CStalkerAnimationPair &CStalkerAnimationManager::script	()
{
	return							(m_script);
}

IC	CAI_Stalker	&CStalkerAnimationManager::object			() const
{
	VERIFY							(m_object);
	return							(*m_object);
}

IC	void CStalkerAnimationManager::pop_script_animation		()
{
	VERIFY							(!script_animations().empty());
	m_script_animations.pop_front	();
	script().reset					();
}

IC	void CStalkerAnimationManager::clear_script_animations	()
{
	m_script_animations.clear		();
	script().reset					();
}

IC	bool CStalkerAnimationManager::non_script_need_update	() const
{
	return							(
		m_global.need_update() ||
		m_head.need_update() ||
		m_torso.need_update() ||
		m_legs.need_update()
	);
}

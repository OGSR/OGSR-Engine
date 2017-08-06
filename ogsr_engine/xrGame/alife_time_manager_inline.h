////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_time_manager_inline.h
//	Created 	: 05.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife time manager class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	void			CALifeTimeManager::set_time_factor		(float time_factor)
{
	m_game_time					= game_time();
	m_start_time				= Device.dwTimeGlobal;
	m_time_factor				= time_factor;
};

IC	void			CALifeTimeManager::advance_game_time(u32 dt_ms)
{
	m_game_time += dt_ms;
	m_start_time = Device.dwTimeGlobal;
};

IC	ALife::_TIME_ID	CALifeTimeManager::game_time			() const
{
	return						(m_game_time + iFloor(m_time_factor*float(Device.dwTimeGlobal - m_start_time)));
};

IC	float			CALifeTimeManager::time_factor	() const
{
	return						(m_time_factor);
}

IC	float			CALifeTimeManager::normal_time_factor	() const
{
	return						(m_normal_time_factor);
}

////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_smart_terrain_task_inline.h
//	Created 	: 20.09.2005
//  Modified 	: 20.09.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife smart terrain task inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CALifeSmartTerrainTask::CALifeSmartTerrainTask				(LPCSTR patrol_path_name)
{
	init					(patrol_path_name,0);
}

IC	CALifeSmartTerrainTask::CALifeSmartTerrainTask				(LPCSTR patrol_path_name, const u32 &patrol_point_index)
{
	init					(patrol_path_name,patrol_point_index);
}

IC	CALifeSmartTerrainTask::CALifeSmartTerrainTask				(const shared_str &patrol_path_name)
{
	init					(patrol_path_name,0);
}

IC	CALifeSmartTerrainTask::CALifeSmartTerrainTask				(const shared_str &patrol_path_name, const u32 &patrol_point_index)
{
	init					(patrol_path_name,patrol_point_index);
}

IC	void CALifeSmartTerrainTask::init							(const shared_str &patrol_path_name, const u32 &patrol_point_index)
{
#ifdef DEBUG
	m_patrol_path_name		= patrol_path_name;
	m_patrol_point_index	= patrol_point_index;
#endif
	m_patrol_point			= 0;
	setup_patrol_point		(patrol_path_name,patrol_point_index);
}

#ifdef DEBUG
IC	const shared_str &CALifeSmartTerrainTask::patrol_path_name	() const
{
	return					(m_patrol_path_name);
}

IC	const u32 &CALifeSmartTerrainTask::patrol_point_index		() const
{
	return					(m_patrol_point_index);
}
#endif

IC	const CPatrolPoint &CALifeSmartTerrainTask::patrol_point	() const
{
	VERIFY					(m_patrol_point);
	return					(*m_patrol_point);
}

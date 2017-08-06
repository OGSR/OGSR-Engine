////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_movement_params_inline.h
//	Created 	: 23.12.2005
//  Modified 	: 23.12.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker movement parameters class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	bool CStalkerMovementParams::operator==	(const CStalkerMovementParams &params) const
{
	if (m_use_desired_position != params.m_use_desired_position)
		return				(false);

	if (m_use_desired_direction != params.m_use_desired_direction)
		return				(false);

	if (m_detail_path_type != params.m_detail_path_type)
		return				(false);

	if (m_path_type != params.m_path_type)
		return				(false);

	if (m_mental_state != params.m_mental_state)
		return				(false);

	if (m_movement_type != params.m_movement_type)
		return				(false);

	if (m_body_state != params.m_body_state)
		return				(false);

	if (m_use_desired_direction && !m_desired_direction.similar(params.m_desired_direction))
		return				(false);

	if (m_use_desired_position && !m_desired_position.similar(params.m_desired_position))
		return				(false);

	return					(true);
}

IC	bool CStalkerMovementParams::operator!=	(const CStalkerMovementParams &params) const
{
	return					(!(*this == params));
}

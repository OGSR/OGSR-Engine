////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_movement_manager_inline.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Stalker movement manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC const MonsterSpace::SBoneRotation &CStalkerMovementManager::head_orientation() const
{
	return				(m_head);
}

IC void CStalkerMovementManager::set_head_orientation(const MonsterSpace::SBoneRotation &orientation)
{
	m_head							= orientation;
}

IC	void CStalkerMovementManager::set_desired_direction(const Fvector *desired_direction)
{
	if (desired_direction) {
		m_target.m_use_desired_direction	= true;
		m_target.m_desired_direction		= *desired_direction;
	}
	else {
		m_target.m_use_desired_direction	= false;
#ifdef DEBUG
		m_target.m_desired_direction		= Fvector().set(flt_max,flt_max,flt_max);
#endif
	}
}

IC	void CStalkerMovementManager::add_velocity		(int mask, float linear, float compute_angular)
{
	add_velocity				(mask,linear,compute_angular,compute_angular);
}

IC	bool CStalkerMovementManager::turn_in_place			() const
{
	return						(!path_completed() && fis_zero(speed()) && (angle_difference(body_orientation().current.yaw,body_orientation().target.yaw) > EPS_L));
}

IC	void CStalkerMovementManager::set_body_state(EBodyState body_state)
{
	THROW						((body_state != eBodyStateCrouch) || (m_target.m_mental_state != eMentalStateFree));
	m_target.m_body_state		= body_state;
}

IC	void CStalkerMovementManager::set_movement_type(EMovementType movement_type)
{
	m_target.m_movement_type	= movement_type;
}

IC	void CStalkerMovementManager::set_mental_state(EMentalState mental_state)
{
	THROW						((m_target.m_body_state != eBodyStateCrouch) || (mental_state != eMentalStateFree));
	m_target.m_mental_state		= mental_state;
#pragma todo("Dima to Dima: this is correct, commented just because of the October presentation, no time right now to fix it correctly, should be fixed sometimes later")
//.	m_path_actuality			= m_path_actuality && (m_target.m_mental_state == m_current.m_mental_state);
}

IC	void CStalkerMovementManager::set_path_type(EPathType path_type)
{
	m_target.m_path_type		= path_type;
}

IC	void CStalkerMovementManager::set_detail_path_type(EDetailPathType detail_path_type)
{
	m_target.m_detail_path_type	= detail_path_type;
}

IC	const MonsterSpace::EBodyState CStalkerMovementManager::body_state() const
{
	return						(m_current.m_body_state);
}

IC	const MonsterSpace::EBodyState CStalkerMovementManager::target_body_state() const
{
	return						(m_target.m_body_state);
}

IC const MonsterSpace::EMovementType CStalkerMovementManager::movement_type() const
{
	return						(m_current.m_movement_type);
}

IC const MonsterSpace::EMentalState CStalkerMovementManager::mental_state() const
{
	return						(m_current.m_mental_state);
}

IC const MonsterSpace::EMentalState CStalkerMovementManager::target_mental_state() const
{
	return						(m_target.m_mental_state);
}

IC	const MovementManager::EPathType CStalkerMovementManager::path_type	() const
{
	return						(m_current.m_path_type);
}

IC	const DetailPathManager::EDetailPathType	CStalkerMovementManager::detail_path_type	() const
{
	return						(m_current.m_detail_path_type);
}

IC	const Fvector &CStalkerMovementManager::desired_position			() const
{
	VERIFY						(use_desired_position());
	return						(m_current.m_desired_position);
}

IC	const Fvector &CStalkerMovementManager::desired_direction			() const
{
	VERIFY						(use_desired_direction());
	return						(m_current.m_desired_direction);
}

IC	bool CStalkerMovementManager::use_desired_position					() const
{
	return						(m_current.m_use_desired_position);
}

IC	bool CStalkerMovementManager::use_desired_direction					() const
{
	return						(m_current.m_use_desired_direction);
}

IC	CAI_Stalker &CStalkerMovementManager::object						() const
{
	VERIFY						(m_object);
	return						(*m_object);
}

IC	const MonsterSpace::EMovementType CStalkerMovementManager::target_movement_type	() const
{
	return						(m_target.m_movement_type);
}

IC	void CStalkerMovementManager::danger_head_speed						(const float &speed)
{
	m_danger_head_speed			= speed;
}

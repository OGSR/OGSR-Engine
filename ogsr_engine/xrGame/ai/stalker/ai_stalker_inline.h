////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_inline.h
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Stalker" (inline functions)
////////////////////////////////////////////////////////////////////////////

#pragma once

IC BOOL CAI_Stalker::UsedAI_Locations() { return (TRUE); }

IC CStalkerAnimationManager& CAI_Stalker::animation() const
{
    VERIFY(m_animation_manager);
    return (*m_animation_manager);
}

IC CStalkerPlanner& CAI_Stalker::brain() const
{
    VERIFY(m_brain);
    return (*m_brain);
}

IC CSightManager& CAI_Stalker::sight() const
{
    VERIFY(m_sight_manager);
    return (*m_sight_manager);
}

IC LPCSTR CAI_Stalker::Name() const { return (CInventoryOwner::Name()); }

IC CStalkerMovementManager& CAI_Stalker::movement() const
{
    VERIFY(m_movement_manager);
    return (*m_movement_manager);
}

IC bool CAI_Stalker::frame_check(u32& frame)
{
    if (Device.dwFrame == frame)
        return (false);

    frame = Device.dwFrame;
    return (true);
}

IC bool CAI_Stalker::group_behaviour() const { return (m_group_behaviour); }

IC CWeaponShotEffector& CAI_Stalker::weapon_shot_effector() const
{
    VERIFY(m_weapon_shot_effector);
    return (*m_weapon_shot_effector);
}

IC u32 CAI_Stalker::min_queue_size_far() const { return (m_min_queue_size_far); }

IC u32 CAI_Stalker::max_queue_size_far() const { return (m_max_queue_size_far); }

IC u32 CAI_Stalker::min_queue_interval_far() const { return (m_min_queue_interval_far); }

IC u32 CAI_Stalker::max_queue_interval_far() const { return (m_max_queue_interval_far); }

IC u32 CAI_Stalker::min_queue_size_medium() const { return (m_min_queue_size_medium); }

IC u32 CAI_Stalker::max_queue_size_medium() const { return (m_max_queue_size_medium); }

IC u32 CAI_Stalker::min_queue_interval_medium() const { return (m_min_queue_interval_medium); }

IC u32 CAI_Stalker::max_queue_interval_medium() const { return (m_max_queue_interval_medium); }

IC u32 CAI_Stalker::min_queue_size_close() const { return (m_min_queue_size_close); }

IC u32 CAI_Stalker::max_queue_size_close() const { return (m_max_queue_size_close); }

IC u32 CAI_Stalker::min_queue_interval_close() const { return (m_min_queue_interval_close); }

IC u32 CAI_Stalker::max_queue_interval_close() const { return (m_max_queue_interval_close); }

IC bool CAI_Stalker::wounded() const { return (m_wounded); }

IC const CAI_Stalker::CRITICAL_WOUND_WEIGHTS& CAI_Stalker::critical_wound_weights() const
{
    VERIFY(!m_critical_wound_weights.empty());
    return (m_critical_wound_weights);
}

IC const bool& CAI_Stalker::throw_enabled()
{
    if (!m_throw_actual)
        update_throw_params();

    return (m_throw_enabled);
}

IC const u32& CAI_Stalker::last_throw_time() const { return (m_last_throw_time); }

IC const bool& CAI_Stalker::can_throw_grenades() const { return (m_can_throw_grenades); }

IC void CAI_Stalker::can_throw_grenades(const bool& value) { m_can_throw_grenades = value; }

IC const u32& CAI_Stalker::throw_time_interval() const { return (m_throw_time_interval); }

IC void CAI_Stalker::throw_time_interval(const u32& value) { m_throw_time_interval = value; }

IC const Fvector& CAI_Stalker::throw_target() const { return (m_throw_target_position); }

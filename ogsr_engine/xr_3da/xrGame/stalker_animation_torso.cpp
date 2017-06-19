////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_torso_animation.cpp
//	Created 	: 19.11.2004
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Torso animations for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_animation_manager.h"
#include "ai/stalker/ai_stalker.h"
#include "inventory.h"
#include "weapon.h"
#include "missile.h"
#include "clsid_game.h"
#include "object_handler_space.h"
#include "object_handler_planner.h"
#include "stalker_movement_manager.h"
#include "entitycondition.h"
#include "stalker_animation_data.h"
#include "stalker_animation_manager_impl.h"

const u32	need_look_back_time_interval= 2000;

void CStalkerAnimationManager::torso_play_callback	(CBlend *blend)
{
	CAI_Stalker						*object = (CAI_Stalker*)blend->CallbackParam;
	VERIFY							(object);

	CStalkerAnimationManager		&animation = object->animation();
	CStalkerAnimationPair			&pair = animation.torso();
	pair.on_animation_end			();

	if (animation.m_looking_back) {
		animation.m_change_direction_time	= Device.dwTimeGlobal + need_look_back_time_interval;
		animation.m_looking_back	= 0;
	}
}

MotionID CStalkerAnimationManager::no_object_animation(const EBodyState &body_state) const
{
	const CAI_Stalker				&stalker = object();
	const CStalkerMovementManager	&movement = stalker.movement();
	const xr_vector<CAniVector>		&animation = m_data_storage->m_part_animations.A[body_state].m_torso.A[0].A;

	if (eMentalStateFree == movement.mental_state()) {
		VERIFY3						(
			eBodyStateStand == movement.body_state(),
			"Cannot run FREE animations, when body state is not stand!",
			*stalker.cName()
		);

		if (standing())
			return					(animation[9].A[1]);

		return						(animation[7 + movement.movement_type()].A[1]);
	}
		
	if (standing())
		return						(animation[6].A[0]);

	if (eMovementTypeWalk == movement.movement_type())
		return						(animation[6].A[2]);

	VERIFY							(eMovementTypeRun == movement.movement_type());
	return							(animation[6].A[3]);
}

MotionID CStalkerAnimationManager::unknown_object_animation(u32 slot, const EBodyState &body_state) const
{
	// animation shortcuts
	typedef CStalkerAnimationState	STATE;
	const xr_vector<STATE>			&part_animations = m_data_storage->m_part_animations.A;
	const xr_vector<CAniVector>		&animation = part_animations[body_state].m_torso.A[slot].A;
	const xr_vector<CAniVector>		&animation_stand = part_animations[eBodyStateStand].m_torso.A[slot].A;
	
	// stalker shortcuts
	const CAI_Stalker				&stalker = object();
	const CStalkerMovementManager	&movement = stalker.movement();
	u32								id = stalker.CObjectHandler::planner().current_action_state_id();

	switch (id) {
		case ObjectHandlerSpace::eWorldOperatorFire1:
		case ObjectHandlerSpace::eWorldOperatorFire2:
		case ObjectHandlerSpace::eWorldOperatorAim1:
		case ObjectHandlerSpace::eWorldOperatorAim2:
		case ObjectHandlerSpace::eWorldOperatorAimingReady1:
		case ObjectHandlerSpace::eWorldOperatorAimingReady2:
		case ObjectHandlerSpace::eWorldOperatorQueueWait1:
		case ObjectHandlerSpace::eWorldOperatorQueueWait2:	{
			if (standing())
				return				(animation[6].A[0]);

			if (eMovementTypeWalk == movement.movement_type()) {
				if ((body_state == eBodyStateStand) && (slot == 2) && need_look_back())
					return			(animation[13 + m_looking_back - 1].A[1]);
				else
					return			(animation[6].A[0]);
			}

			if ((body_state == eBodyStateStand) && (slot == 2) && need_look_back())
				return				(animation[13 + m_looking_back - 1].A[0]);

			VERIFY					(eMovementTypeRun == movement.movement_type());
			return					(animation[6].A[3]);
		}

		case ObjectHandlerSpace::eWorldOperatorStrapping		:
			return					(animation_stand[11].A[0]);
		case ObjectHandlerSpace::eWorldOperatorUnstrapping		:
			return					(animation_stand[12].A[0]);
		case ObjectHandlerSpace::eWorldOperatorStrapping2Idle	:
			return					(animation_stand[11].A[1]);
		case ObjectHandlerSpace::eWorldOperatorUnstrapping2Idle	:
			return					(animation_stand[12].A[1]);
	}

	if (eMentalStateFree == movement.mental_state()) {
		VERIFY3								(
			eBodyStateStand == movement.body_state(),
			"Cannot run FREE animation when body state is not stand!",
			*object().cName()
		);

		if (standing())
			return					(animation[9].A[1]);

		return						(animation[7 + movement.movement_type()].A[1]);
	}

	if (standing())
		return						(animation[6].A[0]);

	if (eMovementTypeWalk == movement.movement_type())
		return						(animation[6].A[2]);

	VERIFY							(eMovementTypeRun == movement.movement_type());

	if (eBodyStateStand == movement.body_state())
		return						(animation[6].A[3]);

	return							(animation[6].A[3]);
}

MotionID CStalkerAnimationManager::weapon_animation	(u32 slot, const EBodyState &body_state)
{
	const xr_vector<CAniVector>		&animation = m_data_storage->m_part_animations.A[body_state].m_torso.A[slot].A;

	switch (m_weapon->GetState()) {
		case CWeapon::eReload : {
			switch (m_weapon->GetReloadState()){
				case CWeapon::eSubstateReloadBegin:
					return			(animation[4].A[0]);
				case CWeapon::eSubstateReloadInProcess:
					return			(animation[4].A[1]);
				case CWeapon::eSubstateReloadEnd:
					return			(animation[4].A[2]);

				default:			NODEFAULT;
			}
#ifdef DEBUG
			return					(animation[4].A[0]);
#endif
		}
		case CWeapon::eShowing :
			return					(torso().select(animation[0].A));
		case CWeapon::eHiding :
			return					(torso().select(animation[3].A));
		case CWeapon::eHidden :
			return					(no_object_animation(body_state));
		case CWeapon::eFire:
		case CWeapon::eFire2 : {
			CAI_Stalker				&stalker = object();
			CStalkerMovementManager	&movement = stalker.movement();
			if (standing())
				return				(animation[1].A[0]);

			if (eMovementTypeWalk == movement.movement_type()) {
				if ((body_state == eBodyStateStand) && (slot == 2) && need_look_back())
					return			(animation[13 + m_looking_back - 1].A[1/**1/**/]);
				else
					return			(animation[1].A[0/**2/**/]);
			}

			if ((body_state == eBodyStateStand) && (slot == 2) && need_look_back())
				return				(animation[13 + m_looking_back - 1].A[0]);

			VERIFY					(eMovementTypeRun == movement.movement_type());
			return					(animation[1].A[3]);
		}
	}

	return							(unknown_object_animation(slot,body_state));
}

MotionID CStalkerAnimationManager::missile_animation	(u32 slot, const EBodyState &body_state)
{
	VERIFY							(m_missile);

	if (body_state == eBodyStateCrouch)
		slot						= 0;

	const xr_vector<CAniVector>		&animation = m_data_storage->m_part_animations.A[body_state].m_torso.A[slot].A;

	switch (m_missile->GetState()) {
		case MS_SHOWING	 :
			return					(torso().select(animation[0].A));
		case MS_HIDING	 :
			return					(torso().select(animation[3].A));
		case MS_THREATEN :
			return					(animation[1].A[0]);
		case MS_READY	 :
			return					(animation[1].A[1]);
		case MS_THROW	 :
			return					(animation[1].A[2]);
		case MS_END		 :
			return					(animation[1].A[2]);
		case MS_PLAYING	 :
			return					(animation[1].A[2]);
		case MS_IDLE	 :
		case MS_HIDDEN	 :
		case MS_EMPTY	 :
		default			 :
			return					(torso().select(animation[6].A));
	}
}

MotionID CStalkerAnimationManager::assign_torso_animation	()
{
	EBodyState						body_state	= this->body_state();

	if (!object().inventory().ActiveItem())
		return						(no_object_animation(body_state));

	fill_object_info				();

	if (m_weapon) {
		if (!strapped())
			return					(weapon_animation(object_slot(),body_state));

		return						(no_object_animation(body_state));
	}

	if (m_missile)
		return						(missile_animation(object_slot(),body_state));

	return							(unknown_object_animation(object_slot(),body_state));
}

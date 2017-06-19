////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_animation_pair.h
//	Created 	: 25.02.2003
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker animation pair
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../skeletonanimated.h"
#include "ai/ai_monsters_anims.h"

class CBlend;
class CAI_Stalker;

#define USE_HEAD_BONE_PART_FAKE

class CStalkerAnimationPair {
#ifdef USE_HEAD_BONE_PART_FAKE
public:
	enum {
		all_bone_parts = u16(0xf),
	};
#endif // USE_HEAD_BONE_PART_FAKE

public:
	typedef xr_vector<float>						ANIMATION_WEIGHTS;
	typedef std::pair<LPCSTR,LPCSTR>				BLEND_ID;

public:
	typedef fastdelegate::FastDelegate0<>			CALLBACK_ID;
	typedef xr_vector<CALLBACK_ID>					CALLBACKS;

private:
	MotionID				m_animation;
	CBlend					*m_blend;
	bool					m_actual;
	bool					m_step_dependence;
	bool					m_global_animation;
	const ANIM_VECTOR		*m_array;
	MotionID				m_array_animation;
	CALLBACKS				m_callbacks;

#ifdef DEBUG
private:
	LPCSTR					m_object_name;
	LPCSTR					m_animation_type_name;

public:
	bool					m_just_started;
#endif // DEBUG

private:
			void			select_animation		(const ANIM_VECTOR &array, const ANIMATION_WEIGHTS *weights);
#ifndef USE_HEAD_BONE_PART_FAKE
			void			play_global_animation	(CKinematicsAnimated *skeleton_animated, PlayCallback callback, CAI_Stalker *object, const bool &use_animation_movement_control);
#else // USE_HEAD_BONE_PART_FAKE
			void			play_global_animation	(CKinematicsAnimated *skeleton_animated, PlayCallback callback, CAI_Stalker *object, const u32 &bone_part, const bool &use_animation_movement_control);
#endif // USE_HEAD_BONE_PART_FAKE

public:
	IC						CStalkerAnimationPair	();
	IC		void			reset					();
			void			synchronize				(CKinematicsAnimated *skeleton_animated, const CStalkerAnimationPair &stalker_animation_pair) const;
			MotionID		select					(const ANIM_VECTOR &array, const ANIMATION_WEIGHTS *weights = 0);
	IC		bool			actual					() const;
	IC		bool			animation				(const MotionID &animation);
	IC		const MotionID	&animation				() const;
	IC		CBlend			*blend					() const;
	IC		void			step_dependence			(bool value);
	IC		bool			step_dependence			() const;
	IC		void			global_animation		(bool global_animation);
	IC		bool			global_animation		() const;
	IC		void			make_inactual			();

public:
#ifndef USE_HEAD_BONE_PART_FAKE
			void			play					(CKinematicsAnimated *skeleton_animated, PlayCallback callback, CAI_Stalker *object, const bool &use_animation_movement_control, bool continue_interrupted_animation = true);
#else // USE_HEAD_BONE_PART_FAKE
			void			play					(CKinematicsAnimated *skeleton_animated, PlayCallback callback, CAI_Stalker *object, const bool &use_animation_movement_control, bool continue_interrupted_animation = true, const u32 &bone_part = all_bone_parts);
#endif // USE_HEAD_BONE_PART_FAKE

#ifdef DEBUG
public:
	IC		void			set_dbg_info			(LPCSTR object_name, LPCSTR animation_type_name);
			BLEND_ID		*blend_id				(CKinematicsAnimated *skeleton_animated, BLEND_ID &result) const;
#endif // DEBUG

public:
	IC	const CALLBACK_ID	*callback				(const CALLBACK_ID &callback) const;
	IC		void			add_callback			(const CALLBACK_ID &callback);
	IC		void			remove_callback			(const CALLBACK_ID &callback);
			void			on_animation_end		();
	IC		bool			need_update				() const;
};

#include "stalker_animation_pair_inline.h"
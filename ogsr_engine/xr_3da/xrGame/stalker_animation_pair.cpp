////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_animation_pair.cpp
//	Created 	: 25.02.2003
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker animation pair
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_animation_pair.h"
#include "../motion.h"
#include "ai_debug.h"
#include "ai/stalker/ai_stalker.h"
#include "ai/ai_monsters_anims.h"

#pragma warning(push)
#pragma warning(disable:4995)
#include <malloc.h>
#pragma warning(pop)

void CStalkerAnimationPair::synchronize		(CKinematicsAnimated *skeleton_animated, const CStalkerAnimationPair &stalker_animation) const
{
	if (!blend())
		return;

	CMotionDef				*motion0 = skeleton_animated->LL_GetMotionDef(animation());
	VERIFY					(motion0);
	if (!(motion0->flags & esmSyncPart))
		return;

	if (!stalker_animation.blend())
		return;

	CMotionDef				*motion1 = skeleton_animated->LL_GetMotionDef(stalker_animation.animation());
	VERIFY					(motion1);
	if (!(motion1->flags & esmSyncPart))
		return;

	blend()->timeCurrent	= stalker_animation.blend()->timeCurrent;
}

#ifndef USE_HEAD_BONE_PART_FAKE
void CStalkerAnimationPair::play_global_animation	(CKinematicsAnimated *skeleton_animated, PlayCallback callback, CAI_Stalker *object, const bool &use_animation_movement_control)
#else
void CStalkerAnimationPair::play_global_animation	(CKinematicsAnimated *skeleton_animated, PlayCallback callback, CAI_Stalker *object, const u32 &bone_part, const bool &use_animation_movement_control)
#endif
{
	m_blend				= 0;
	for (u16 i=0; i<MAX_PARTS; ++i) {
#ifdef USE_HEAD_BONE_PART_FAKE
		if (!(bone_part & (1 << i)))
			continue;
#endif

		CBlend			*blend = 0;
		if (!m_blend) {
			blend		= skeleton_animated->LL_PlayCycle(i,animation(),TRUE,callback,object);
			if (use_animation_movement_control)
				object->create_anim_mov_ctrl	(blend);
		}
		else
			skeleton_animated->LL_PlayCycle(i,animation(),TRUE,0,0);

		if (blend && !m_blend)
			m_blend		= blend;
	}
}

#ifndef USE_HEAD_BONE_PART_FAKE
void CStalkerAnimationPair::play			(CKinematicsAnimated *skeleton_animated, PlayCallback callback, CAI_Stalker *object, const bool &use_animation_movement_control, bool continue_interrupted_animation)
#else
void CStalkerAnimationPair::play			(CKinematicsAnimated *skeleton_animated, PlayCallback callback, CAI_Stalker *object, const bool &use_animation_movement_control, bool continue_interrupted_animation, const u32 &bone_part)
#endif
{
	VERIFY					(animation());
	if (actual()) {
#if 0
#	ifdef DEBUG
		if (psAI_Flags.is(aiAnimation) && blend())
			Msg				("%6d [%s][%s][%s][%f]",Device.dwTimeGlobal,m_object_name,m_animation_type_name,*animation()->name(),blend()->timeCurrent);
#	endif
#endif

#ifdef DEBUG
		m_just_started		= false;
#endif // DEBUG
		return;
	}

	if (animation() != m_array_animation) {
		m_array_animation.invalidate	();
		m_array							= 0;
	}

#ifdef DEBUG
	m_just_started			= true;
#endif // DEBUG

	if (!global_animation()) {

		float				pos = 0.f;
		if (m_step_dependence && continue_interrupted_animation) {
			VERIFY			(!m_blend || !fis_zero(m_blend->timeTotal));
			if (m_step_dependence && m_blend)
				pos			= fmod(m_blend->timeCurrent,m_blend->timeTotal)/m_blend->timeTotal;
		}

		m_blend				= skeleton_animated->PlayCycle(animation(),TRUE,callback,object);

		if (m_step_dependence && continue_interrupted_animation) {
//			if (we were standing and now we are moving)
//				pos						= 0.5f*Random.randI(2);
			if (m_blend)
				m_blend->timeCurrent = m_blend->timeTotal*pos;
		}
	}
	else
#ifndef USE_HEAD_BONE_PART_FAKE
		play_global_animation	(skeleton_animated,callback,object,use_animation_movement_control);
#else
		play_global_animation	(skeleton_animated,callback,object,bone_part,use_animation_movement_control);
#endif
	m_actual				= true;

	if (m_step_dependence)
		object->CStepManager::on_animation_start(animation(),blend());

#ifdef DEBUG
	if (psAI_Flags.is(aiAnimation)) {
		CMotionDef			*motion = skeleton_animated->LL_GetMotionDef(animation());
		VERIFY				(motion);
		LPCSTR				name = skeleton_animated->LL_MotionDefName_dbg(animation()).first;
		Msg					("%6d [%s][%s][%s][%d] - LOOPED: %d",Device.dwTimeGlobal,m_object_name,m_animation_type_name,name,motion->bone_or_part,! (motion->flags & esmStopAtEnd));
	}
#endif
}

#ifdef DEBUG
std::pair<LPCSTR,LPCSTR> *CStalkerAnimationPair::blend_id	(CKinematicsAnimated *skeleton_animated, std::pair<LPCSTR,LPCSTR> &result) const
{
	if (!blend())
		return				(0);

	u32						bone_part_id = 0;
	if (!global_animation())
		bone_part_id		= blend()->bone_or_part;

	const BlendSVec			&blends = skeleton_animated->blend_cycle(bone_part_id);
	if (blends.size() < 2)
		return				(0);

#if 0
	VERIFY2					(
		blends[blends.size() - 2]->motionID != animation(),
		make_string(
			"animation is blending with itself (%s)",
			skeleton_animated->LL_MotionDefName_dbg(animation()).first
		)
	);
#endif
	result					= skeleton_animated->LL_MotionDefName_dbg(blends[blends.size() - 2]->motionID);
	return					(&result);
}
#endif // DEBUG

void CStalkerAnimationPair::select_animation(const ANIM_VECTOR &array, const ANIMATION_WEIGHTS *weights)
{
	if (!weights) {
		m_array_animation	= array[::Random.randI(array.size())];
		VERIFY				(m_array_animation);
		return;
	}

	float					accumulator = 0.f;
	ANIMATION_WEIGHTS::const_iterator	I = weights->begin(), B = I;
	ANIMATION_WEIGHTS::const_iterator	E = weights->end();
	
	u32						array_size = array.size();
	if (array_size < weights->size())
		E					= B + array_size;

	for ( ; I != E; ++I)
		accumulator			+= *I;

	float					chosen = ::Random.randF()*accumulator;
	accumulator				= 0.f;
	for (I = B; I != E; ++I) {
		if ((accumulator + *I) >= chosen)
			break;

		accumulator			+= *I;
		continue;
	}

	VERIFY					(I != E);
	VERIFY					(u32(I - B) < array.size());
	m_array_animation		= array[I - B];
	VERIFY					(m_array_animation);
}

MotionID CStalkerAnimationPair::select	(const ANIM_VECTOR &array, const ANIMATION_WEIGHTS *weights)
{
	VERIFY					(!array.empty());

	if (m_array == &array) {
		VERIFY				(animation());
		return				(animation());
	}

	m_array					= &array;
	select_animation		(array,weights);
	return					(m_array_animation);
}


void CStalkerAnimationPair::on_animation_end	()
{
	make_inactual			();

	if (m_callbacks.empty())
		return;

	u32						callback_count = m_callbacks.size();
	CALLBACK_ID				*callbacks = (CALLBACK_ID*)_alloca(callback_count*sizeof(CALLBACK_ID));
	CALLBACK_ID				*I = callbacks;
	CALLBACK_ID				*E = callbacks + callback_count;
	CALLBACKS::iterator		i = m_callbacks.begin();
	for ( ; I != E; ++I, ++i)
		new (I) CALLBACK_ID	(*i);

	for (I = callbacks; I != E; ++I)
		(*I)				();

	for (I = callbacks; I != E; ++I)
		I->~CALLBACK_ID		();
}

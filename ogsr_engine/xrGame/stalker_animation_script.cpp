////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_animation_script.cpp
//	Created 	: 25.02.2003
//  Modified 	: 19.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker animation manager : script animations
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_animation_manager.h"
#include "ai/stalker/ai_stalker.h"
#include "script_engine.h"
#include "game_object_space.h"
#include "script_callback_ex.h"
#include "ai_space.h"

void CStalkerAnimationManager::script_play_callback(CBlend *blend)
{
	CAI_Stalker					*object = (CAI_Stalker*)blend->CallbackParam;
	VERIFY						(object);
	
	CStalkerAnimationManager	&animation_manager = object->animation();
	CStalkerAnimationPair		&pair = animation_manager.script();
	const SCRIPT_ANIMATIONS		&animations = animation_manager.script_animations();

#if 0
	Msg							(
		"%6d Script callback [%s]",
		Device.dwTimeGlobal,
		animations.empty()
		?
		"unknown"
		:
		animation_manager.m_skeleton_animated->LL_MotionDefName_dbg(animations.front().animation())
	);
#endif

	if	(
			pair.animation() && 
			!animations.empty() && 
			(pair.animation() == animations.front().animation())
		)
		animation_manager.pop_script_animation();

	animation_manager.m_call_script_callback	= true;

	pair.on_animation_end		();
}

void CStalkerAnimationManager::add_script_animation	(LPCSTR animation, bool hand_usage, bool use_movement_controller)
{
	const MotionID					&motion = m_skeleton_animated->ID_Cycle_Safe(animation);
	if (!motion) {
		ai().script_engine().script_log(eLuaMessageTypeError,"There is no animation %s (object %s)!",animation,*object().cName());
		return;
	}

	m_script_animations.emplace_back(motion, hand_usage, use_movement_controller);
}

const CStalkerAnimationScript &CStalkerAnimationManager::assign_script_animation	() const
{
	VERIFY							(!script_animations().empty());
	return							(script_animations().front());
}

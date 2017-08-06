////////////////////////////////////////////////////////////////////////////
//	Module 		: script_game_object_script2.cpp
//	Created 	: 25.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script game object script export
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "script_game_object.h"
#include "alife_space.h"
#include "script_entity_space.h"
#include "movement_manager_space.h"
#include "pda_space.h"
#include "memory_space.h"
#include "cover_point.h"
#include "script_hit.h"
#include "script_binder_object.h"
#include "script_ini_file.h"
#include "script_sound_info.h"
#include "script_monster_hit_info.h"
#include "script_entity_action.h"
#include "action_planner.h"
#include "PhysicsShell.h"
#include "helicopter.h"
#include "script_zone.h"
#include "relation_registry.h"
#include "danger_object.h"
#include "Torch.h"
#include "searchlight.h"

using namespace luabind;

extern CScriptActionPlanner *script_action_planner(CScriptGameObject *obj);

class_<CScriptGameObject> &script_register_game_object1(class_<CScriptGameObject> &instance)
{
	instance
		.enum_("relation")
		[
			value("friend",					int(ALife::eRelationTypeFriend)),
			value("neutral",				int(ALife::eRelationTypeNeutral)),
			value("enemy",					int(ALife::eRelationTypeEnemy)),
			value("dummy",					int(ALife::eRelationTypeDummy))
		]
		.enum_("action_types")
		[
			value("movement",				int(ScriptEntity::eActionTypeMovement)),
			value("watch",					int(ScriptEntity::eActionTypeWatch)),
			value("animation",				int(ScriptEntity::eActionTypeAnimation)),
			value("sound",					int(ScriptEntity::eActionTypeSound)),
			value("particle",				int(ScriptEntity::eActionTypeParticle)),
			value("object",					int(ScriptEntity::eActionTypeObject)),
			value("action_type_count",		int(ScriptEntity::eActionTypeCount))
		]
		.enum_("EPathType")
		[
			value("game_path",				int(MovementManager::ePathTypeGamePath)),
			value("level_path",				int(MovementManager::ePathTypeLevelPath)),
			value("patrol_path",			int(MovementManager::ePathTypePatrolPath)),
			value("no_path",				int(MovementManager::ePathTypeNoPath))
		]
		
//		.property("visible",				&CScriptGameObject::getVisible,			&CScriptGameObject::setVisible)
//		.property("enabled",				&CScriptGameObject::getEnabled,			&CScriptGameObject::setEnabled)

//		.def_readonly("health",				&CScriptGameObject::GetHealth,			&CScriptGameObject::SetHealth)
		.property("health",					&CScriptGameObject::GetHealth,			&CScriptGameObject::SetHealth)
		.property("psy_health",				&CScriptGameObject::GetPsyHealth,		&CScriptGameObject::SetPsyHealth)
		.property("power",					&CScriptGameObject::GetPower,			&CScriptGameObject::SetPower)
		.property("satiety",				&CScriptGameObject::GetSatiety,			&CScriptGameObject::SetSatiety)
		.property("radiation",				&CScriptGameObject::GetRadiation,		&CScriptGameObject::SetRadiation)
		.property("morale",					&CScriptGameObject::GetMorale,			&CScriptGameObject::SetMorale)
// KD
		.property("alcohol",				&CScriptGameObject::GetAlcohol,			&CScriptGameObject::SetAlcohol)
		.property("max_power",				&CScriptGameObject::GetMaxPower,		&CScriptGameObject::SetMaxPower)

			// Actor State
		.property("jump_speed",				&CScriptGameObject::GetActorJumpSpeed,	&CScriptGameObject::SetActorJumpSpeed)
		.property("walk_accel",				&CScriptGameObject::GetActorWalkAccel,	&CScriptGameObject::SetActorWalkAccel)
		.property("exo_factor",				&CScriptGameObject::GetActorExoFactor,	&CScriptGameObject::SetActorExoFactor)

			// Other
		.def("get_bleeding",				&CScriptGameObject::GetBleeding)
		.def("center",						&CScriptGameObject::Center)
		.def("position",					&CScriptGameObject::Position)
		.def("direction",					&CScriptGameObject::Direction)
		.def("clsid",						&CScriptGameObject::clsid)
		.def("id",							&CScriptGameObject::ID)
		.def("story_id",					&CScriptGameObject::story_id)
		.def("section",						&CScriptGameObject::Section)
		.def("name",						&CScriptGameObject::Name)
		.def("parent",						&CScriptGameObject::Parent)
		.def("mass",						&CScriptGameObject::Mass)
		.def("cost",						&CScriptGameObject::Cost)
		.def("condition",					&CScriptGameObject::GetCondition)
		.def("set_condition",				&CScriptGameObject::SetCondition)
		.def("death_time",					&CScriptGameObject::DeathTime)
//		.def("armor",						&CScriptGameObject::Armor)
		.def("max_health",					&CScriptGameObject::MaxHealth)
		.def("accuracy",					&CScriptGameObject::Accuracy)
		.def("alive",						&CScriptGameObject::Alive)
		.def("team",						&CScriptGameObject::Team)
		.def("squad",						&CScriptGameObject::Squad)
		.def("group",						&CScriptGameObject::Group)
		.def("change_team",					(void (CScriptGameObject::*)(u8,u8,u8))(&CScriptGameObject::ChangeTeam))
		.def("kill",						&CScriptGameObject::Kill)
		.def("hit",							&CScriptGameObject::Hit)
		.def("play_cycle",					(void (CScriptGameObject::*)(LPCSTR))(&CScriptGameObject::play_cycle))
		.def("play_cycle",					(void (CScriptGameObject::*)(LPCSTR,bool))(&CScriptGameObject::play_cycle))
		.def("fov",							&CScriptGameObject::GetFOV)
		.def("range",						&CScriptGameObject::GetRange)
		.def("relation",					&CScriptGameObject::GetRelationType)
		.def("script",						&CScriptGameObject::SetScriptControl)
		.def("get_script",					&CScriptGameObject::GetScriptControl)
		.def("get_script_name",				&CScriptGameObject::GetScriptControlName)
		.def("reset_action_queue",			&CScriptGameObject::ResetActionQueue)
		.def("see",							&CScriptGameObject::CheckObjectVisibility)
		.def("see",							&CScriptGameObject::CheckTypeVisibility)

		.def("who_hit_name",				&CScriptGameObject::WhoHitName)
		.def("who_hit_section_name",		&CScriptGameObject::WhoHitSectionName)
		
		.def("rank",						&CScriptGameObject::GetRank)
		.def("command",						&CScriptGameObject::AddAction)
		.def("action",						&CScriptGameObject::GetCurrentAction, adopt(result))
		.def("object_count",				&CScriptGameObject::GetInventoryObjectCount)
		.def("object",						(CScriptGameObject *(CScriptGameObject::*)(LPCSTR))(&CScriptGameObject::GetObjectByName))
		.def("object",						(CScriptGameObject *(CScriptGameObject::*)(int))(&CScriptGameObject::GetObjectByIndex))
		.def("active_item",					&CScriptGameObject::GetActiveItem)
		
		.def("set_callback",				(void (CScriptGameObject::*)(GameObject::ECallbackType, const luabind::functor<void> &))(&CScriptGameObject::SetCallback))
		.def("set_callback",				(void (CScriptGameObject::*)(GameObject::ECallbackType, const luabind::functor<void> &, const luabind::object &))(&CScriptGameObject::SetCallback))
		.def("set_callback",				(void (CScriptGameObject::*)(GameObject::ECallbackType))(&CScriptGameObject::SetCallback))

		.def("set_patrol_extrapolate_callback",	(void (CScriptGameObject::*)())(&CScriptGameObject::set_patrol_extrapolate_callback))
		.def("set_patrol_extrapolate_callback",	(void (CScriptGameObject::*)(const luabind::functor<bool> &))(&CScriptGameObject::set_patrol_extrapolate_callback))
		.def("set_patrol_extrapolate_callback",	(void (CScriptGameObject::*)(const luabind::functor<bool> &, const luabind::object &))(&CScriptGameObject::set_patrol_extrapolate_callback))

		.def("set_enemy_callback",			(void (CScriptGameObject::*)())(&CScriptGameObject::set_enemy_callback))
		.def("set_enemy_callback",			(void (CScriptGameObject::*)(const luabind::functor<bool> &))(&CScriptGameObject::set_enemy_callback))
		.def("set_enemy_callback",			(void (CScriptGameObject::*)(const luabind::functor<bool> &, const luabind::object &))(&CScriptGameObject::set_enemy_callback))

		.def("patrol",						&CScriptGameObject::GetPatrolPathName)

		.def("get_ammo_in_magazine",		&CScriptGameObject::GetAmmoElapsed)
		.def("get_ammo_total",				&CScriptGameObject::GetAmmoCurrent)
		.def("set_ammo_elapsed",			&CScriptGameObject::SetAmmoElapsed)
		.def("set_queue_size",				&CScriptGameObject::SetQueueSize)
//		.def("best_hit",					&CScriptGameObject::GetBestHit)
//		.def("best_sound",					&CScriptGameObject::GetBestSound)
		.def("best_danger",					&CScriptGameObject::GetBestDanger)
		.def("best_enemy",					&CScriptGameObject::GetBestEnemy)
		.def("best_item",					&CScriptGameObject::GetBestItem)
		.def("action_count",				&CScriptGameObject::GetActionCount)
		.def("action_by_index",				&CScriptGameObject::GetActionByIndex)
		
		//.def("set_hear_callback",			(void (CScriptGameObject::*)(const luabind::object &, LPCSTR))(&CScriptGameObject::SetSoundCallback))
		//.def("set_hear_callback",			(void (CScriptGameObject::*)(const luabind::functor<void> &))(&CScriptGameObject::SetSoundCallback))
		//.def("clear_hear_callback",		&CScriptGameObject::ClearSoundCallback)
		
		.def("memory_time",					&CScriptGameObject::memory_time)
		.def("memory_position",				&CScriptGameObject::memory_position)
		.def("best_weapon",					&CScriptGameObject::best_weapon)
		.def("explode",						&CScriptGameObject::explode)
		.def("get_enemy",					&CScriptGameObject::GetEnemy)
		.def("get_corpse",					&CScriptGameObject::GetCorpse)
		.def("get_enemy_strength",			&CScriptGameObject::GetEnemyStrength)
		.def("get_sound_info",				&CScriptGameObject::GetSoundInfo)
		.def("get_monster_hit_info",		&CScriptGameObject::GetMonsterHitInfo)
		.def("bind_object",					&CScriptGameObject::bind_object,adopt(_2))
		.def("motivation_action_manager",	&script_action_planner)

		// bloodsucker
		.def("set_invisible",				&CScriptGameObject::set_invisible)
		.def("set_manual_invisibility",		&CScriptGameObject::set_manual_invisibility)
		.def("set_alien_control",			&CScriptGameObject::set_alien_control)

		// zombie
		.def("fake_death_fall_down",		&CScriptGameObject::fake_death_fall_down)
		.def("fake_death_stand_up",			&CScriptGameObject::fake_death_stand_up)

		// base monster
		.def("skip_transfer_enemy",			&CScriptGameObject::skip_transfer_enemy)
		.def("set_home",					&CScriptGameObject::set_home)
		.def("remove_home",					&CScriptGameObject::remove_home)
		.def("berserk",						&CScriptGameObject::berserk)
		.def("can_script_capture",			&CScriptGameObject::can_script_capture)
		.def("set_custom_panic_threshold",	&CScriptGameObject::set_custom_panic_threshold)
		.def("set_default_panic_threshold",	&CScriptGameObject::set_default_panic_threshold)

		// inventory owner
		.def("get_current_outfit",			&CScriptGameObject::GetCurrentOutfit)
		.def("get_current_outfit_protection",&CScriptGameObject::GetCurrentOutfitProtection)

		// searchlight
		.def("get_current_direction",		&CScriptGameObject::GetCurrentDirection)
		.def("get_projector",				&CScriptGameObject::GetProjector)
		.def("projector_switch",			&CScriptGameObject::SwitchProjector)

		// movement manager
		.def("set_body_state",				&CScriptGameObject::set_body_state			)
		.def("set_movement_type",			&CScriptGameObject::set_movement_type		)
		.def("set_mental_state",			&CScriptGameObject::set_mental_state		)
		.def("set_path_type",				&CScriptGameObject::set_path_type			)
		.def("set_detail_path_type",		&CScriptGameObject::set_detail_path_type	)

		.def("body_state",					&CScriptGameObject::body_state				)
		.def("target_body_state",			&CScriptGameObject::target_body_state		)
		.def("movement_type",				&CScriptGameObject::movement_type			)
		.def("target_movement_type",		&CScriptGameObject::target_movement_type	)
		.def("mental_state",				&CScriptGameObject::mental_state			)
		.def("target_mental_state",			&CScriptGameObject::target_mental_state		)
		.def("path_type",					&CScriptGameObject::path_type				)
		.def("detail_path_type",			&CScriptGameObject::detail_path_type		)

		//
		.def("set_desired_position",		(void (CScriptGameObject::*)())(&CScriptGameObject::set_desired_position))
		.def("set_desired_position",		(void (CScriptGameObject::*)(const Fvector *))(&CScriptGameObject::set_desired_position))
		.def("set_desired_direction",		(void (CScriptGameObject::*)())(&CScriptGameObject::set_desired_direction))
		.def("set_desired_direction",		(void (CScriptGameObject::*)(const Fvector *))(&CScriptGameObject::set_desired_direction))
		.def("set_patrol_path",				&CScriptGameObject::set_patrol_path)
		.def("set_dest_level_vertex_id",	&CScriptGameObject::set_dest_level_vertex_id)
		.def("level_vertex_id",				&CScriptGameObject::level_vertex_id)
		.def("level_vertex_light",			&CScriptGameObject::level_vertex_light)
		.def("game_vertex_id",				&CScriptGameObject::game_vertex_id)
		.def("add_animation",				(void (CScriptGameObject::*)(LPCSTR, bool, bool))(&CScriptGameObject::add_animation))
		.def("clear_animations",			&CScriptGameObject::clear_animations)
		.def("animation_count",				&CScriptGameObject::animation_count)
		.def("animation_slot",				&CScriptGameObject::animation_slot)

		.def("ignore_monster_threshold",				&CScriptGameObject::set_ignore_monster_threshold)
		.def("restore_ignore_monster_threshold",		&CScriptGameObject::restore_ignore_monster_threshold)
		.def("ignore_monster_threshold",				&CScriptGameObject::ignore_monster_threshold)
		.def("max_ignore_monster_distance",				&CScriptGameObject::set_max_ignore_monster_distance)
		.def("restore_max_ignore_monster_distance",		&CScriptGameObject::restore_max_ignore_monster_distance)
		.def("max_ignore_monster_distance",				&CScriptGameObject::max_ignore_monster_distance)

		.def("eat",							&CScriptGameObject::eat)

		.def("extrapolate_length",			(float (CScriptGameObject::*)() const)(&CScriptGameObject::extrapolate_length))
		.def("extrapolate_length",			(void (CScriptGameObject::*)(float))(&CScriptGameObject::extrapolate_length))

		.def("set_fov",						&CScriptGameObject::set_fov)
		.def("set_range",					&CScriptGameObject::set_range)

		.def("head_orientation",			&CScriptGameObject::head_orientation)

		.def("set_actor_position",			&CScriptGameObject::SetActorPosition)
		.def("set_actor_direction",			&CScriptGameObject::SetActorDirection)

		.def("vertex_in_direction",			&CScriptGameObject::vertex_in_direction)

		.def("item_in_slot",				&CScriptGameObject::item_in_slot)
		.def("active_slot",					&CScriptGameObject::active_slot)
		.def("activate_slot",				&CScriptGameObject::activate_slot)

		.def("switch_torch",				&CScriptGameObject::SwitchTorch)

		.def("get_xform",					&CScriptGameObject::GetXForm)

#ifdef DEBUG
		.def("debug_planner",				&CScriptGameObject::debug_planner)
#endif // DEBUG
		.def("invulnerable",				(bool (CScriptGameObject::*)() const)&CScriptGameObject::invulnerable)
		.def("invulnerable",				(void (CScriptGameObject::*)(bool))&CScriptGameObject::invulnerable)

	;return	(instance);
}
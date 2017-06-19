////////////////////////////////////////////////////////////////////////////
//	Module 		: object_handler.cpp
//	Created 	: 11.03.2004
//  Modified 	: 11.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object handler
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "object_handler.h"
#include "object_handler_space.h"
#include "object_handler_planner.h"
#include "ai_monster_space.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "weaponmagazined.h"
#include "ef_storage.h"
#include "ef_pattern.h"
#include "ai/stalker/ai_stalker.h"
#include "inventory.h"
#include "torch.h"
#include "../skeletoncustom.h"
#include "memory_manager.h"
#include "enemy_manager.h"
#include "ai_object_location.h"
#include "profiler.h"
#include "stalker_animation_manager.h"
#include "object_handler_planner_impl.h"
#include "effectorshot.h"

CObjectHandler::CObjectHandler		()
{
	m_planner					= xr_new<CObjectHandlerPlanner>();
	m_inventory_actual			= false;
//	m_last_enemy_for_best_weapon= 0;
}

CObjectHandler::~CObjectHandler		()
{
	xr_delete					(m_planner);
}

void CObjectHandler::Load			(LPCSTR section)
{
	inherited::Load				(section);
}

void CObjectHandler::reinit			(CAI_Stalker *object)
{
	inherited::reinit			();
	m_hammer_is_clutched		= false;
	planner().setup				(object);
	CKinematics					*kinematics = smart_cast<CKinematics*>(planner().m_object->Visual());
	m_r_hand					= kinematics->LL_BoneID(pSettings->r_string(*planner().m_object->cNameSect(),"weapon_bone0"));
	m_l_finger1					= kinematics->LL_BoneID(pSettings->r_string(*planner().m_object->cNameSect(),"weapon_bone1"));
	m_r_finger2					= kinematics->LL_BoneID(pSettings->r_string(*planner().m_object->cNameSect(),"weapon_bone2"));
	m_strap_object_id			= ALife::_OBJECT_ID(-1);
	m_strap_bone0				= -1;
	m_strap_bone1				= -1;
	m_clutched_hammer_enabled	= false;
}

void CObjectHandler::reload			(LPCSTR section)
{
	inherited::reload			(section);
}

BOOL CObjectHandler::net_Spawn		(CSE_Abstract* DC)
{
	if (!inherited::net_Spawn(DC))
		return					(FALSE);

	CSE_Abstract				*abstract = static_cast<CSE_Abstract*>(DC);
	CSE_ALifeTraderAbstract		*trader = smart_cast<CSE_ALifeTraderAbstract*>(abstract);
	VERIFY						(trader);

	m_infinite_ammo				= !!trader->m_trader_flags.test(CSE_ALifeTraderAbstract::eTraderFlagInfiniteAmmo);
	return						(TRUE);
}

void CObjectHandler::OnItemTake		(CInventoryItem *inventory_item)
{
	inherited::OnItemTake		(inventory_item);

	m_inventory_actual			= false;

	planner().add_item			(inventory_item);

	if (planner().object().g_Alive())
		switch_torch			(inventory_item,true);

	if (inventory_item->useful_for_NPC() && (inventory_item->object().cNameSect() == m_item_to_spawn)) {
		m_item_to_spawn			= shared_str();
		m_ammo_in_box_to_spawn	= 0;
	}

	CWeapon						*weapon = smart_cast<CWeapon*>(inventory_item);
	if (weapon)
		planner().object().weapon_shot_effector().Initialize(
			weapon->camMaxAngle,
			weapon->camRelaxSpeed_AI,
			weapon->camMaxAngleHorz,
			weapon->camStepAngleHorz,
			weapon->camDispertionFrac
		);
}

void CObjectHandler::OnItemDrop		(CInventoryItem *inventory_item)
{
	inherited::OnItemDrop	(inventory_item);

	m_inventory_actual		= false;
	
	if (m_infinite_ammo && planner().object().g_Alive() && !inventory_item->useful_for_NPC()) {
		CWeaponAmmo				*weapon_ammo = smart_cast<CWeaponAmmo*>(inventory_item);
		if (weapon_ammo) {
			Level().spawn_item		(*weapon_ammo->cNameSect(),planner().object().Position(),planner().object().ai_location().level_vertex_id(),planner().object().ID());
			m_item_to_spawn			= weapon_ammo->cNameSect();
			m_ammo_in_box_to_spawn	= weapon_ammo->m_boxSize;
		}
	}

	planner().remove_item		(inventory_item);

	switch_torch				(inventory_item,false);
}

CInventoryItem *CObjectHandler::best_weapon() const
{
	if (!planner().object().g_Alive())
		return									(0);

	planner().object().update_best_item_info	();
	return										(planner().object().m_best_item_to_kill);
}

void CObjectHandler::update		()
{
	START_PROFILE("Object Handler")
	planner().update		();
	STOP_PROFILE
}

void CObjectHandler::set_goal	(MonsterSpace::EObjectAction object_action, CGameObject *game_object, u32 min_queue_size, u32 max_queue_size, u32 min_queue_interval, u32 max_queue_interval)
{
	planner().set_goal(object_action,game_object,min_queue_size,max_queue_size,min_queue_interval,max_queue_interval);
}

void CObjectHandler::set_goal	(MonsterSpace::EObjectAction object_action, CInventoryItem *inventory_item, u32 min_queue_size, u32 max_queue_size, u32 min_queue_interval, u32 max_queue_interval)
{
	set_goal(object_action,inventory_item ? &inventory_item->object() : 0,min_queue_size,max_queue_size,min_queue_interval,max_queue_interval);
}

bool CObjectHandler::goal_reached	()
{
	return					(planner().solution().size() < 2);
}

void CObjectHandler::weapon_bones	(int &b0, int &b1, int &b2) const
{
	CWeapon						*weapon = smart_cast<CWeapon*>(inventory().ActiveItem());
	if (!weapon || !planner().m_storage.property(ObjectHandlerSpace::eWorldPropertyStrapped)) {
		if (weapon)
			weapon->strapped_mode	(false);
		b0						= m_r_hand;
		b1						= m_r_finger2;
		b2						= m_l_finger1;
		return;
	}

	THROW3						(weapon->can_be_strapped(),"Cannot strap weapon",*weapon->cName());

	if (weapon->ID() != m_strap_object_id) {
		CKinematics				*kinematics = smart_cast<CKinematics*>(planner().m_object->Visual());
		m_strap_bone0			= kinematics->LL_BoneID(weapon->strap_bone0());
		m_strap_bone1			= kinematics->LL_BoneID(weapon->strap_bone1());
		m_strap_object_id		= weapon->ID();
	}

	weapon->strapped_mode		(true);
	b0							= m_strap_bone0;
	b1							= m_strap_bone1;
	b2							= b1;
}

bool CObjectHandler::weapon_strapped	() const
{
	CWeapon						*weapon = smart_cast<CWeapon*>(inventory().ActiveItem());
	if (!weapon)
		return					(false);

	return						(weapon_strapped(weapon));
}

void CObjectHandler::actualize_strap_mode	(CWeapon *weapon) const
{
	VERIFY						(weapon);

	if (!planner().m_storage.property(ObjectHandlerSpace::eWorldPropertyStrapped)) {
		weapon->strapped_mode	(false);
		return;
	}

	THROW3						(weapon->can_be_strapped(),"Cannot strap weapon",*weapon->cName());
	weapon->strapped_mode		(true);
}

bool CObjectHandler::weapon_strapped	(CWeapon *weapon) const
{
	VERIFY						(weapon);

	if (!weapon->can_be_strapped())
		return					(false);

	if (
		(planner().current_action_state_id() == ObjectHandlerSpace::eWorldOperatorStrapping2Idle) ||
		(planner().current_action_state_id() == ObjectHandlerSpace::eWorldOperatorStrapping) ||
		(planner().current_action_state_id() == ObjectHandlerSpace::eWorldOperatorUnstrapping2Idle) ||
		(planner().current_action_state_id() == ObjectHandlerSpace::eWorldOperatorUnstrapping)
	) {
		return					(false);
	}

	actualize_strap_mode		(weapon);

	return						(weapon->strapped_mode());
}

bool CObjectHandler::weapon_unstrapped	() const
{
	CWeapon						*weapon = smart_cast<CWeapon*>(inventory().ActiveItem());
	if (!weapon)
		return					(true);

	return						(weapon_unstrapped(weapon));
}

bool CObjectHandler::weapon_unstrapped	(CWeapon *weapon) const
{
	VERIFY						(weapon);

	if (!weapon->can_be_strapped())
		return					(true);

	switch (planner().current_action_state_id()) {
		case ObjectHandlerSpace::eWorldOperatorStrapping2Idle:
		case ObjectHandlerSpace::eWorldOperatorStrapping:
		case ObjectHandlerSpace::eWorldOperatorUnstrapping2Idle:
		case ObjectHandlerSpace::eWorldOperatorUnstrapping:
			return				(false);
	}

	actualize_strap_mode		(weapon);

	VERIFY						(
		(planner().current_action_state_id() != ObjectHandlerSpace::eWorldOperatorStrapped) ||
		weapon->strapped_mode()
	);

	return						(!weapon->strapped_mode());
}

IC	void CObjectHandler::switch_torch	(CInventoryItem *inventory_item, bool value)
{
	CTorch						*torch = smart_cast<CTorch*>(inventory_item);
	if (torch && attached(torch) && planner().object().g_Alive())
		torch->Switch			(value);
}

void CObjectHandler::attach				(CInventoryItem *inventory_item)
{
	inherited::attach			(inventory_item);
	switch_torch				(inventory_item,true);
}

void CObjectHandler::detach				(CInventoryItem *inventory_item)
{
	switch_torch				(inventory_item,false);
	inherited::detach			(inventory_item);
}

extern Flags32 g_uCommonFlags;

bool CObjectHandler::can_use_dynamic_lights	()
{	
	// flAiUseTorchDynamicLights == 1
	return						(!!g_uCommonFlags.test(1));
}

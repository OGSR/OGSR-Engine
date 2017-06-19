////////////////////////////////////////////////////////////////////////////
//	Module 		: object_actions.h
//	Created 	: 12.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object actions
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_actions.h"
#include "inventory.h"
#include "ai/stalker/ai_stalker.h"
#include "xr_level_controller.h"
#include "xrmessages.h"
#include "fooditem.h"
#include "weapon.h"
#include "weaponmagazined.h"
#include "object_handler_space.h"
#include "stalker_animation_manager.h"

#define ALLOW_STRANGE_BEHAVIOUR

//////////////////////////////////////////////////////////////////////////
// CObjectActionCommand
//////////////////////////////////////////////////////////////////////////

CObjectActionCommand::CObjectActionCommand(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, u32 command, LPCSTR action_name) :
	inherited			(item,owner,storage,action_name),
	m_command			(command)
{
}

void CObjectActionCommand::initialize	()
{
	inherited::initialize();
	object().inventory().Action(m_command,CMD_START);
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionShow
//////////////////////////////////////////////////////////////////////////

CObjectActionShow::CObjectActionShow	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, LPCSTR action_name) :
	inherited		(item,owner,storage,action_name)
{
	m_weapon						= smart_cast<CWeapon*>(item);
}

void CObjectActionShow::initialize		()
{
	inherited::initialize			();
	
	VERIFY							(m_item);
	if (object().inventory().m_slots[m_item->GetSlot()].m_pIItem)
		object().inventory().Ruck	(object().inventory().m_slots[m_item->GetSlot()].m_pIItem);

//.	object().inventory().SetActiveSlot(NO_ACTIVE_SLOT);
	object().inventory().Slot		(m_item);
/*
	bool							result = object().inventory().Activate	(m_item->GetSlot());
	VERIFY							(result);
*/
	if (!m_weapon)
		return;
}

void CObjectActionShow::execute		()
{
	inherited::execute				();
	VERIFY							(m_item);
	if (!object().inventory().ActiveItem() || (object().inventory().ActiveItem()->object().ID() != m_item->object().ID())) {
		CHudItem					*hud_item = smart_cast<CHudItem*>(object().inventory().ActiveItem());
		if (!hud_item){
			object().inventory().Slot		(m_item);
			return;
		}
		if (!hud_item->IsPending()) {
			if (object().inventory().m_slots[m_item->GetSlot()].m_pIItem)
				object().inventory().Ruck	(object().inventory().m_slots[m_item->GetSlot()].m_pIItem);
//.			object().inventory().SetActiveSlot(NO_ACTIVE_SLOT);
			object().inventory().Slot		(m_item);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionHide
//////////////////////////////////////////////////////////////////////////

CObjectActionHide::CObjectActionHide	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, LPCSTR action_name) :
	inherited		(item,owner,storage,action_name)
{
}

void CObjectActionHide::execute		()
{
	inherited::execute				();
	VERIFY							(m_item);
	object().inventory().Activate	(NO_ACTIVE_SLOT);
	set_property					(ObjectHandlerSpace::eWorldPropertyUseEnough,false);
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionReload
//////////////////////////////////////////////////////////////////////////

CObjectActionReload::CObjectActionReload	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, _condition_type type, LPCSTR action_name) :
	inherited		(item,owner,storage,action_name),
	m_type			(type)
{
}

void CObjectActionReload::initialize		()
{
	inherited::initialize		();
	VERIFY						(m_item);
	VERIFY						(object().inventory().ActiveItem());
	VERIFY						(object().inventory().ActiveItem()->object().ID() == m_item->object().ID());
	object().inventory().Action	(kWPN_RELOAD,	CMD_START);
}

void CObjectActionReload::execute			()
{
	inherited::execute			();
	
	VERIFY						(m_item);
	VERIFY						(object().inventory().ActiveItem());
	VERIFY						(object().inventory().ActiveItem()->object().ID() == m_item->object().ID());
	
	CWeapon						*weapon = smart_cast<CWeapon*>(object().inventory().ActiveItem());
	VERIFY						(weapon);
	if (weapon->IsPending())
		return;
	
	if (weapon->GetAmmoElapsed())
		return;

	object().inventory().Action	(kWPN_RELOAD,CMD_START);
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionFire
//////////////////////////////////////////////////////////////////////////

CObjectActionFire::CObjectActionFire	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, _condition_type type, LPCSTR action_name) :
	inherited		(item,owner,storage,action_name),
	m_type			(type)
{
}

void CObjectActionFire::initialize		()
{
	inherited::inherited::initialize	();

	VERIFY						(m_item);
	VERIFY						(object().inventory().ActiveItem());
	VERIFY						(object().inventory().ActiveItem()->object().ID() == m_item->object().ID());

	if (!m_object->can_kill_member())
		object().inventory().Action	(kWPN_FIRE,	CMD_START);
	else
		object().inventory().Action	(kWPN_FIRE,	CMD_STOP);
}

void CObjectActionFire::execute			()
{
	inherited::execute					();

	VERIFY						(m_item);
	VERIFY						(object().inventory().ActiveItem());
	VERIFY						(object().inventory().ActiveItem()->object().ID() == m_item->object().ID());

	if (!m_object->can_kill_member()) {
		CWeapon					*weapon = smart_cast<CWeapon*>(object().inventory().ActiveItem());
		if (!weapon || (weapon->GetState() != CWeapon::eFire))
			object().inventory().Action	(kWPN_FIRE,	CMD_START);
	}
	else
		object().inventory().Action	(kWPN_FIRE,	CMD_STOP);
}

void CObjectActionFire::finalize		()
{
	inherited::finalize					();
	object().inventory().Action		(kWPN_FIRE,	CMD_STOP);
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionStrapping
//////////////////////////////////////////////////////////////////////////

CObjectActionStrapping::CObjectActionStrapping	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, LPCSTR action_name) :
	inherited		(item,owner,storage,action_name)
{
	m_callback_removed			= true;
}

CObjectActionStrapping::~CObjectActionStrapping	()
{
	if (m_callback_removed) {
		VERIFY					(
			!object().animation().torso().callback(
				CStalkerAnimationPair::CALLBACK_ID(
					this,
					&CObjectActionStrapping::on_animation_end
				)
			)
		);
		return;
	}

	object().animation().torso().remove_callback(
		CStalkerAnimationPair::CALLBACK_ID(
			this,
			&CObjectActionStrapping::on_animation_end
		)
	);
}

void CObjectActionStrapping::on_animation_end	()
{
	VERIFY						(!m_callback_removed);

	m_storage->set_property		(ObjectHandlerSpace::eWorldPropertyStrapped,true);

	object().animation().torso().remove_callback(
		CStalkerAnimationPair::CALLBACK_ID(
			this,
			&CObjectActionStrapping::on_animation_end
		)
	);

	m_callback_removed			= true;

//	Msg							("[[%6d][%s]][%s] removing callback on callabck CObjectActionStrapping::on_animation_end",Device.dwTimeGlobal,*object().cName());
}

void CObjectActionStrapping::initialize			()
{
	inherited::initialize				();

	VERIFY						(m_item);
	VERIFY						(object().inventory().ActiveItem());
	VERIFY						(object().inventory().ActiveItem()->object().ID() == m_item->object().ID());

	m_callback_removed			= false;

	m_storage->set_property		(ObjectHandlerSpace::eWorldPropertyStrapped2Idle,true);
	
	object().animation().torso().add_callback	(
		CStalkerAnimationPair::CALLBACK_ID(
			this,
			&CObjectActionStrapping::on_animation_end
		)
	);

//	Msg							("[%6d][%s] adding callback CObjectActionStrapping::on_animation_end",Device.dwTimeGlobal,*object().cName());
}

void CObjectActionStrapping::execute			()
{
	inherited::execute();

	VERIFY						(m_item);
	VERIFY						(object().inventory().ActiveItem());
	VERIFY						(object().inventory().ActiveItem()->object().ID() == m_item->object().ID());
}

void CObjectActionStrapping::finalize		()
{
	inherited::finalize					();

	if (!m_callback_removed) {
//		Msg						("[%6d][%s] removing callback from ::finalize CObjectActionStrapping::on_animation_end",Device.dwTimeGlobal,*object().cName());

		object().animation().torso().remove_callback	(
			CStalkerAnimationPair::CALLBACK_ID(
				this,
				&CObjectActionStrapping::on_animation_end
			)
		);

		m_callback_removed		= true;
	}
	else {
		VERIFY					(
			!object().animation().torso().callback(
				CStalkerAnimationPair::CALLBACK_ID(
					this,
					&CObjectActionStrapping::on_animation_end
				)
			)
		);
//		Msg						("[%6d][%s] callback is already removed, do nothing on ::finalize CObjectActionStrapping::on_animation_end",Device.dwTimeGlobal,*object().cName());
	}
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionStrappingToIdle
//////////////////////////////////////////////////////////////////////////

CObjectActionStrappingToIdle::CObjectActionStrappingToIdle	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, LPCSTR action_name) :
	inherited		(item,owner,storage,action_name)
{
	m_callback_removed			= true;
}

CObjectActionStrappingToIdle::~CObjectActionStrappingToIdle	()
{
	if (m_callback_removed) {
		VERIFY					(
			!object().animation().torso().callback(
				CStalkerAnimationPair::CALLBACK_ID(
					this,
					&CObjectActionStrappingToIdle::on_animation_end
				)
			)
		);
		return;
	}

	object().animation().torso().remove_callback(
		CStalkerAnimationPair::CALLBACK_ID(
			this,
			&CObjectActionStrappingToIdle::on_animation_end
		)
	);
}

void CObjectActionStrappingToIdle::on_animation_end	()
{
	VERIFY						(!m_callback_removed);

	m_storage->set_property		(ObjectHandlerSpace::eWorldPropertyStrapped2Idle,false);

	object().animation().torso().remove_callback(
		CStalkerAnimationPair::CALLBACK_ID(
			this,
			&CObjectActionStrappingToIdle::on_animation_end
		)
	);

	m_callback_removed			= true;

//	Msg							("[%6d][%s] removing callback on callabck CObjectActionStrappingToIdle::on_animation_end",Device.dwTimeGlobal,*object().cName());
}

void CObjectActionStrappingToIdle::initialize		()
{
	inherited::initialize				();

	VERIFY						(m_item);
	VERIFY						(object().inventory().ActiveItem());
	VERIFY						(object().inventory().ActiveItem()->object().ID() == m_item->object().ID());

	m_callback_removed			= false;

	object().animation().torso().add_callback	(
		CStalkerAnimationPair::CALLBACK_ID(
			this,
			&CObjectActionStrappingToIdle::on_animation_end
		)
	);

//	Msg							("[%6d][%s] adding callback CObjectActionStrappingToIdle::on_animation_end",Device.dwTimeGlobal,*object().cName());
}

void CObjectActionStrappingToIdle::execute			()
{
	inherited::execute();

	VERIFY						(m_item);
	VERIFY						(object().inventory().ActiveItem());
	VERIFY						(object().inventory().ActiveItem()->object().ID() == m_item->object().ID());
}

void CObjectActionStrappingToIdle::finalize		()
{
	inherited::finalize					();

	if (!m_callback_removed) {
		object().animation().torso().remove_callback	(
			CStalkerAnimationPair::CALLBACK_ID(
				this,
				&CObjectActionStrappingToIdle::on_animation_end
			)
		);

		m_callback_removed		= true;
//		Msg						("[%6d][%s] removing callback from ::finalize CObjectActionStrappingToIdle::on_animation_end",Device.dwTimeGlobal,*object().cName());
	}
	else {
		VERIFY					(
			!object().animation().torso().callback(
				CStalkerAnimationPair::CALLBACK_ID(
					this,
					&CObjectActionStrappingToIdle::on_animation_end
				)
			)
		);
//		Msg						("[%6d][%s] callback is already removed, do nothing on ::finalize CObjectActionStrappingToIdle::on_animation_end",Device.dwTimeGlobal,*object().cName());
	}
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionUnstrapping
//////////////////////////////////////////////////////////////////////////

CObjectActionUnstrapping::CObjectActionUnstrapping	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, LPCSTR action_name) :
	inherited		(item,owner,storage,action_name)
{
	m_callback_removed			= true;
}

CObjectActionUnstrapping::~CObjectActionUnstrapping	()
{
	if (m_callback_removed) {
		VERIFY					(
			!object().animation().torso().callback(
				CStalkerAnimationPair::CALLBACK_ID(
					this,
					&CObjectActionUnstrapping::on_animation_end
				)
			)
		);
		return;
	}

	object().animation().torso().remove_callback(
		CStalkerAnimationPair::CALLBACK_ID(
			this,
			&CObjectActionUnstrapping::on_animation_end
		)
	);
}

void CObjectActionUnstrapping::on_animation_end	()
{
	VERIFY						(!m_callback_removed);

	m_storage->set_property		(ObjectHandlerSpace::eWorldPropertyStrapped,false);

	object().animation().torso().remove_callback(
		CStalkerAnimationPair::CALLBACK_ID(
			this,
			&CObjectActionUnstrapping::on_animation_end
		)
	);

	m_callback_removed			= true;

//	Msg							("[%6d][%s] removing callback on callabck CObjectActionUnstrapping::on_animation_end",Device.dwTimeGlobal,*object().cName());
}

void CObjectActionUnstrapping::initialize		()
{
	inherited::initialize();

	VERIFY						(m_item);
	VERIFY						(object().inventory().ActiveItem());
	VERIFY						(object().inventory().ActiveItem()->object().ID() == m_item->object().ID());

	m_callback_removed			= false;

	m_storage->set_property(ObjectHandlerSpace::eWorldPropertyStrapped2Idle,true);

	object().animation().torso().add_callback	(
		CStalkerAnimationPair::CALLBACK_ID(
			this,
			&CObjectActionUnstrapping::on_animation_end
		)
	);

//	Msg							("[%6d][%s] adding callback CObjectActionUnstrapping::on_animation_end",Device.dwTimeGlobal,*object().cName());
}

void CObjectActionUnstrapping::execute			()
{
	inherited::execute();

	VERIFY						(m_item);
	VERIFY						(object().inventory().ActiveItem());
	VERIFY						(object().inventory().ActiveItem()->object().ID() == m_item->object().ID());
}

void CObjectActionUnstrapping::finalize		()
{
	inherited::finalize					();

	if (!m_callback_removed) {
		object().animation().torso().remove_callback	(
			CStalkerAnimationPair::CALLBACK_ID(
				this,
				&CObjectActionUnstrapping::on_animation_end
			)
		);

		m_callback_removed		= true;
//		Msg						("[%6d][%s] removing callback from ::finalize CObjectActionUnstrapping::on_animation_end",Device.dwTimeGlobal,*object().cName());
	}
	else {
		VERIFY					(
			!object().animation().torso().callback(
				CStalkerAnimationPair::CALLBACK_ID(
					this,
					&CObjectActionUnstrapping::on_animation_end
				)
			)
		);
//		Msg						("[%6d][%s] callback is already removed, do nothing on ::finalize CObjectActionUnstrapping::on_animation_end",Device.dwTimeGlobal,*object().cName());
	}
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionUnstrappingToIdle
//////////////////////////////////////////////////////////////////////////

CObjectActionUnstrappingToIdle::CObjectActionUnstrappingToIdle	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, LPCSTR action_name) :
	inherited		(item,owner,storage,action_name)
{
	m_callback_removed			= true;
}

CObjectActionUnstrappingToIdle::~CObjectActionUnstrappingToIdle	()
{
	if (m_callback_removed) {
		VERIFY					(
			!object().animation().torso().callback(
				CStalkerAnimationPair::CALLBACK_ID(
					this,
					&CObjectActionUnstrappingToIdle::on_animation_end
				)
			)
		);
		return;
	}

	object().animation().torso().remove_callback(
		CStalkerAnimationPair::CALLBACK_ID(
			this,
			&CObjectActionUnstrappingToIdle::on_animation_end
		)
	);
}

void CObjectActionUnstrappingToIdle::on_animation_end	()
{
	VERIFY						(!m_callback_removed);

	m_storage->set_property		(ObjectHandlerSpace::eWorldPropertyStrapped2Idle,false);

	object().animation().torso().remove_callback(
		CStalkerAnimationPair::CALLBACK_ID(
			this,
			&CObjectActionUnstrappingToIdle::on_animation_end
		)
	);

	m_callback_removed			= true;

//	Msg							("[%6d][%s] removing callback on callabck CObjectActionUnstrappingToIdle::on_animation_end",Device.dwTimeGlobal,*object().cName());
}

void CObjectActionUnstrappingToIdle::initialize		()
{
	inherited::initialize				();

	VERIFY						(m_item);
	VERIFY						(object().inventory().ActiveItem());
	VERIFY						(object().inventory().ActiveItem()->object().ID() == m_item->object().ID());

	m_callback_removed			= false;

	object().animation().torso().add_callback	(
		CStalkerAnimationPair::CALLBACK_ID(
			this,
			&CObjectActionUnstrappingToIdle::on_animation_end
		)
	);

//	Msg							("[%6d][%s] adding callback CObjectActionUnstrappingToIdle::on_animation_end",Device.dwTimeGlobal,*object().cName());
}

void CObjectActionUnstrappingToIdle::execute			()
{
	inherited::execute();

	VERIFY						(m_item);
	VERIFY						(object().inventory().ActiveItem());
	VERIFY						(object().inventory().ActiveItem()->object().ID() == m_item->object().ID());
}

void CObjectActionUnstrappingToIdle::finalize		()
{
	inherited::finalize					();

	if (!m_callback_removed) {
		object().animation().torso().remove_callback	(
			CStalkerAnimationPair::CALLBACK_ID(
				this,
				&CObjectActionUnstrappingToIdle::on_animation_end
			)
		);

		m_callback_removed		= true;
//		Msg						("[%6d][%s] removing callback from ::finalize CObjectActionUnstrappingToIdle::on_animation_end",Device.dwTimeGlobal,*object().cName());
	}
	else {
		VERIFY					(
			!object().animation().torso().callback(
				CStalkerAnimationPair::CALLBACK_ID(
					this,
					&CObjectActionUnstrappingToIdle::on_animation_end
				)
			)
		);
//		Msg						("[%6d][%s] callback is already removed, do nothing on ::finalize CObjectActionUnstrappingToIdle::on_animation_end",Device.dwTimeGlobal,*object().cName());
	}
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionQueueWait
//////////////////////////////////////////////////////////////////////////

CObjectActionQueueWait::CObjectActionQueueWait	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, _condition_type type, LPCSTR action_name) :
	inherited				(item,owner,storage,action_name),
	m_type					(type)
{
	m_magazined		= smart_cast<CWeaponMagazined*>(item);
}

void CObjectActionQueueWait::initialize		()
{
	inherited::inherited::initialize	();

	VERIFY						(m_item);
	VERIFY						(object().inventory().ActiveItem());
	VERIFY						(object().inventory().ActiveItem()->object().ID() == m_item->object().ID());
}

void CObjectActionQueueWait::execute			()
{
	inherited::execute		();

	VERIFY						(m_item);
	VERIFY						(object().inventory().ActiveItem());
	VERIFY						(object().inventory().ActiveItem()->object().ID() == m_item->object().ID());

	if (completed())
		m_magazined->StopedAfterQueueFired(false);
}

void CObjectActionQueueWait::finalize		()
{
	inherited::finalize		();
	
	VERIFY						(m_item);
	VERIFY						(object().inventory().ActiveItem());
	VERIFY						(object().inventory().ActiveItem()->object().ID() == m_item->object().ID());

	if (!completed())
		m_magazined->StopedAfterQueueFired(false);
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionSwitch
//////////////////////////////////////////////////////////////////////////

CObjectActionSwitch::CObjectActionSwitch	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, _condition_type type, LPCSTR action_name) :
	inherited		(item,owner,storage,action_name),
	m_type			(type)
{
}

void CObjectActionSwitch::initialize		()
{
	inherited::initialize	();

	VERIFY						(m_item);
	VERIFY						(object().inventory().ActiveItem());
	VERIFY						(object().inventory().ActiveItem()->object().ID() == m_item->object().ID());
}

void CObjectActionSwitch::execute			()
{
	inherited::execute		();

	VERIFY						(m_item);
	VERIFY						(object().inventory().ActiveItem());
	VERIFY						(object().inventory().ActiveItem()->object().ID() == m_item->object().ID());
}

void CObjectActionSwitch::finalize		()
{
	inherited::finalize		();
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionDrop
//////////////////////////////////////////////////////////////////////////

CObjectActionDrop::CObjectActionDrop	(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, LPCSTR action_name) :
	inherited		(item,owner,storage,action_name)
{
}

void CObjectActionDrop::initialize		()
{
	inherited::initialize	();
	if (!m_item || !m_item->object().H_Parent() || (m_object->ID() != m_item->object().H_Parent()->ID()))
		return;

	NET_Packet				P;
	m_object->u_EventGen	(P,GE_OWNERSHIP_REJECT,m_object->ID());
	P.w_u16					(u16(m_item->object().ID()));
	m_object->u_EventSend	(P);
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionThreaten
//////////////////////////////////////////////////////////////////////////

CObjectActionThreaten::CObjectActionThreaten	(CAI_Stalker *item, CAI_Stalker *owner, CPropertyStorage *storage, LPCSTR action_name) :
	inherited				(item,owner,storage,action_name)
{
}

void CObjectActionThreaten::execute			()
{
	inherited::execute		();
	if (completed())
		object().inventory().Action(kWPN_FIRE,	CMD_STOP);
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionAim
//////////////////////////////////////////////////////////////////////////

CObjectActionAim::CObjectActionAim			(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, _condition_type condition_id, _value_type value, LPCSTR action_name) :
	inherited							(item,owner,storage,condition_id,value,action_name)
{
	m_weapon					= smart_cast<CWeaponMagazined*>(m_item);
//	VERIFY						(m_weapon);
}

void CObjectActionAim::initialize			()
{
	inherited::inherited::initialize	();

	VERIFY						(m_item);
	VERIFY						(object().inventory().ActiveItem());
	VERIFY						(object().inventory().ActiveItem()->object().ID() == m_item->object().ID());
}

void CObjectActionAim::execute				()
{
	inherited::execute					();

	VERIFY						(m_item);
	VERIFY						(object().inventory().ActiveItem());
	VERIFY						(object().inventory().ActiveItem()->object().ID() == m_item->object().ID());

	if (m_weapon && completed())
		m_weapon->StopedAfterQueueFired(false);
}

//////////////////////////////////////////////////////////////////////////
// CObjectActionIdle
//////////////////////////////////////////////////////////////////////////

CObjectActionIdle::CObjectActionIdle(CInventoryItem *item, CAI_Stalker *owner, CPropertyStorage *storage, LPCSTR action_name) :
	inherited			(item,owner,storage,action_name)
{
}

void CObjectActionIdle::initialize	()
{
	inherited::initialize();

	VERIFY						(m_item);
	VERIFY						(object().inventory().ActiveItem());
	VERIFY						(object().inventory().ActiveItem()->object().ID() == m_item->object().ID());

	if (m_storage->property(ObjectHandlerSpace::eWorldPropertyUseEnough))
		object().CObjectHandler::set_goal(MonsterSpace::eObjectActionActivate,object().inventory().ActiveItem());
	m_storage->set_property	(ObjectHandlerSpace::eWorldPropertyUseEnough,false);
}

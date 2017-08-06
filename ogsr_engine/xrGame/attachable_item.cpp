////////////////////////////////////////////////////////////////////////////
//	Module 		: attachable_item.cpp
//	Created 	: 11.02.2004
//  Modified 	: 11.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Attachable item
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "physicsshellholder.h"
#include "attachable_item.h"
#include "inventoryowner.h"
#include "inventory.h"

#ifdef DEBUG
	CAttachableItem*	CAttachableItem::m_dbgItem = NULL;
#endif

IC	CPhysicsShellHolder &CAttachableItem::object	() const
{
	return				(item().object());
}

DLL_Pure *CAttachableItem::_construct	()
{
	VERIFY				(!m_item);
	m_item				= smart_cast<CInventoryItem*>(this);
	VERIFY				(m_item);
	return				(&item().object());
}

CAttachableItem::~CAttachableItem		()
{
}

void CAttachableItem::reload			(LPCSTR section)
{
	if (!pSettings->line_exist(section,"attach_angle_offset"))
		return;

	Fvector							angle_offset = pSettings->r_fvector3	(section,"attach_angle_offset");
	Fvector							position_offset	= pSettings->r_fvector3	(section,"attach_position_offset");
	m_offset.setHPB					(VPUSH(angle_offset));
	m_offset.c						= position_offset;
	m_bone_name						= pSettings->r_string	(section,"attach_bone_name");
//	enable							(m_auto_attach = !!(READ_IF_EXISTS(pSettings,r_bool,section,"auto_attach",TRUE)));
	enable							(false);
#ifdef DEBUG
	m_valid							= true;
#endif
}

void CAttachableItem::OnH_A_Chield		() 
{
//	VERIFY							(m_valid);
	const CInventoryOwner			*inventory_owner = smart_cast<const CInventoryOwner*>(object().H_Parent());
	if (inventory_owner && inventory_owner->attached(&item()))
		object().setVisible			(true);
}

void CAttachableItem::renderable_Render	()
{
//	VERIFY							(m_valid);
	::Render->set_Transform			(&object().XFORM());
	::Render->add_Visual			(object().Visual());
}

void CAttachableItem::OnH_A_Independent	()
{
//	VERIFY							(m_valid);
	enable							(false/*m_auto_attach*/);
}

void CAttachableItem::enable			(bool value)
{
//	VERIFY							(m_valid);
	if (!object().H_Parent()) 
	{
		m_enabled			= value;
		return;
	}

	if (value && !enabled() && object().H_Parent()) {
		CGameObject			*game_object = smart_cast<CGameObject*>(object().H_Parent());
		CAttachmentOwner	*owner = smart_cast<CAttachmentOwner*>(game_object);
//		VERIFY				(owner);
		if (owner) {
			m_enabled			= value;
			owner->attach		(&item());
			object().setVisible	(true);
		}
	}
	
	if (!value && enabled() && object().H_Parent()) {
		CGameObject			*game_object = smart_cast<CGameObject*>(object().H_Parent());
		CAttachmentOwner	*owner = smart_cast<CAttachmentOwner*>(game_object);
//		VERIFY				(owner);
		if (owner) {
			m_enabled			= value;
			owner->detach		(&item());
			object().setVisible	(false);
		}
	}
}

bool  CAttachableItem::can_be_attached	() const
{
//	VERIFY							(m_valid);
	if (!item().m_pCurrentInventory)
		return				(false);

	if (!item().m_pCurrentInventory->IsBeltUseful())
		return				(true);

	if (item().m_eItemPlace != eItemPlaceBelt)
		return				(false);
	 
	return					(true);
}
void CAttachableItem::afterAttach		()
{
	VERIFY							(m_valid);
	object().processing_activate	();
}

void CAttachableItem::afterDetach		()
{
	VERIFY							(m_valid);
	object().processing_deactivate	();
}

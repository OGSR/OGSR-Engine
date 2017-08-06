////////////////////////////////////////////////////////////////////////////
//	Module 		: inventory_item_impl.h
//	Created 	: 18.08.2005
//  Modified 	: 18.08.2005
//	Author		: Dmitriy Iassenev
//	Description : inventory item implementation functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "inventory.h"

IC	CInventoryOwner &CInventoryItem::inventory_owner	() const
{
	VERIFY				(m_pCurrentInventory);
	VERIFY				(m_pCurrentInventory->GetOwner());
	return				(*m_pCurrentInventory->GetOwner());
}

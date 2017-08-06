////////////////////////////////////////////////////////////////////////////
//	Module 		: inventory_item_inline.h
//	Created 	: 24.03.2003
//  Modified 	: 29.01.2004
//	Author		: Victor Reutsky, Yuri Dobronravin
//	Description : Inventory item inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	bool CInventoryItem::useful_for_NPC					() const
{
	return				(Useful() && m_flags.test(Fuseful_for_NPC));
}

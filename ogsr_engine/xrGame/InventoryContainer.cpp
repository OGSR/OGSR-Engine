////////////////////////////////////////////////////////////////////////////
//	Module 		: inventory_container.cpp
//	Created 	: 12.11.2014
//  Modified 	: 12.12.2014
//	Author		: Alexander Petrov
//	Description : Mobile container class, based on inventory item 
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Level.h"
#include "InventoryContainer.h"

#pragma optimize("gyts", off)

CInventoryContainer::CInventoryContainer():
		CCustomInventoryBox<CInventoryItemObject>()
{
	open();
}

u32 CInventoryContainer::Cost() const
{
	SItemsInfo info;
	CalcItems(info);
	return info.cost + m_cost;
}

float CInventoryContainer::RadiationRestoreSpeed() const
{
	SItemsInfo info;
	CalcItems(info);
#ifdef		OBJECTS_RADIOACTIVE
	return m_fRadiationRestoreSpeed + info.info[0];
#else
	return info.info[0];
#endif
}

float CInventoryContainer::Weight() const
{
	SItemsInfo info;
	CalcItems(info);
	return info.weight + m_weight;
}


u32	CInventoryContainer::CalcItems	(SItemsInfo &info) const
{
	CObjectList &objs = Level().Objects;
	u32  result = 0;
	Memory.mem_fill(&info, 0, sizeof(info));

	for (auto it = m_items.begin(); it != m_items.end(); it++)
	{
		u16 id = *it;
		PIItem itm = smart_cast<PIItem>(objs.net_Find(id));
		if (itm)
		{
			result++;
			info.weight += itm->Weight();
			info.cost	+= itm->Cost();
#ifdef		OBJECTS_RADIOACTIVE
			CInventoryItem *obj = smart_cast<CInventoryItem*>(itm);
#else
			CArtefact  *obj  = smart_cast<CArtefact*>(itm);
#endif
			if (obj)
			{
				float rsp = obj->RadiationRestoreSpeed();
				info.info[0] += rsp > 0 ? rsp : 0; // нейтрализаторы радиации из рюкзака не работают (артефакты в т.ч.)
			}

		}
		
	}

	return result;
}

bool CInventoryContainer::CanTrade() const
{	
	if (!IsEmpty()) // продавать можно только пустым
		return false;
	return inherited::CanTrade();
}

DLL_Pure* CInventoryContainer::_construct()
{
	return inherited::_construct();
}


BOOL CInventoryContainer::net_Spawn(CSE_Abstract* DC)
{
	BOOL res = inherited::net_Spawn(DC);
	if (cNameSect() == "shadow_inventory")
	{
		close();
		inherited::set_tip_text("st_pick_rucksack");
	}
	return res;
}
void CInventoryContainer::OnEvent(NET_Packet& P, u16 type)
{
	inherited::OnEvent(P, type);
	//if (GE_OWNERSHIP_TAKE == type)
	//	Msg("CInventoryContainer %s received object", Name());
	//if (GE_OWNERSHIP_REJECT == type)
	//	Msg("CInventoryContainer %s lost object", Name());

}


void CInventoryContainer::close()
{
	m_opened = false;
	inherited::set_tip_text_default();
}


void CInventoryContainer::open()
{
	m_opened = true;
	inherited::set_tip_text	 ("container_use");
}

#include "stdafx.h"
#include "InventoryBox.h"
#include "level.h"
#include "actor.h"
#include "game_object_space.h"

#include "script_callback_ex.h"
#include "script_game_object.h"

#pragma optimize("gyts", off)

IInventoryBox::IInventoryBox() : m_items ()
{
	m_in_use = false;
	m_items.clear();
}

void IInventoryBox::ProcessEvent(CGameObject *O, NET_Packet& P, u16 type)
{
	switch (type)
	{
	case GE_OWNERSHIP_TAKE:
		{
			u16 id;
            P.r_u16(id);
			CObject* itm = Level().Objects.net_Find(id);  VERIFY(itm);
			m_items.push_back	(id);
			itm->H_SetParent	(O);
			itm->setVisible		(FALSE);
			itm->setEnabled		(FALSE);

			// Real Wolf: Коллбек для ящика на получение предмета. 02.08.2014.
			if( m_in_use )
				if (auto obj = smart_cast<CGameObject*>(itm) )
					Actor()->callback(GameObject::eInvBoxItemPlace)(O->lua_game_object(), obj->lua_game_object());

		}break;
	case GE_OWNERSHIP_REJECT:
		{
			u16 id;
            P.r_u16(id);
			CObject* itm = Level().Objects.net_Find(id);  VERIFY(itm);
			xr_vector<u16>::iterator it;
			it = std::find(m_items.begin(),m_items.end(),id); VERIFY(it!=m_items.end());
			m_items.erase		(it);
			itm->H_SetParent	(NULL,!P.r_eof() && P.r_u8());

			if( m_in_use )
			{
				CGameObject* GO		= smart_cast<CGameObject*>(itm);
				Actor()->callback(GameObject::eInvBoxItemTake)( O->lua_game_object(), GO->lua_game_object() );
			}
		}break;
	};
}

#include "inventory_item.h"
void IInventoryBox::AddAvailableItems(TIItemContainer& items_container) const
{
	xr_vector<u16>::const_iterator it = m_items.begin();
	xr_vector<u16>::const_iterator it_e = m_items.end();

	for(;it!=it_e;++it)
	{
		PIItem itm = smart_cast<PIItem>(Level().Objects.net_Find(*it));VERIFY(itm);
		items_container.push_back	(itm);
	}
}

CScriptGameObject* IInventoryBox::GetObjectByName(LPCSTR name)
{	
	const shared_str s_name(name);
	CObjectList &objects = Level().Objects;
	CObject* result = objects.FindObjectByName(name);
	if (result)
	{
		CObject *self = this->object().dcast_CObject();
		if (result->H_Parent() != self)
			 return NULL; // объект существует, но не принадлежит сему контейнеру
	} 
	else
	{		
		for (auto it = m_items.begin(); it != m_items.end(); ++it)
			if (auto obj = objects.net_Find(*it))
			{
				if (obj->cName() == s_name)		return smart_cast<CGameObject*>(obj)->lua_game_object();
				if (obj->cNameSect() == s_name) result = obj; // поиск по секции в качестве резерва
			}

	}
	return result ? smart_cast<CGameObject*>(result)->lua_game_object() : NULL;
}

CScriptGameObject* IInventoryBox::GetObjectByIndex(u32 id)
{
	if (id < m_items.size() )
	{
		u32 obj_id = u32(m_items[id]);
		if (auto obj = smart_cast<CGameObject*>(Level().Objects.net_Find(obj_id) ) )
			return obj->lua_game_object();
	}
	return NULL;
}

u32 IInventoryBox::GetSize() const
 { 
	LPCSTR t = typeid(*this).name();
	R_ASSERT(t);
	u32	result = m_items.size();
	R_ASSERT2(result < 1000, "to many items in inventory box, probably cast error");
	return result; 
}

bool IInventoryBox::IsEmpty() const
{   
	return m_items.empty(); 
}

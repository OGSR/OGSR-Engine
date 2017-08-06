#pragma once
#include "inventory_space.h"
#include "GameObject.h"

class CInventoryBox :public CGameObject
{
	typedef CGameObject									inherited;
	xr_vector<u16>										m_items;
public:
				bool	m_in_use;
						CInventoryBox					();
	virtual		void	OnEvent							(NET_Packet& P, u16 type);
	virtual		BOOL	net_Spawn						(CSE_Abstract* DC);
	virtual		void	net_Relcase						(CObject* O	);
				void	AddAvailableItems				(TIItemContainer& items_container) const;
				bool	IsEmpty							() {return m_items.empty();}
				u32		Size							() { return m_items.size(); }
				u16		Item							(u16 _i) { return m_items[_i]; }
};
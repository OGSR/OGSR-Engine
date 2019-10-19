#pragma once

#include "UIWindow.h"

class CCustomOutfit;
class CUIStatic;
class CUIXml;
struct ActorRestoreParams;


class CUIOutfitImmunity : public CUIWindow
{
public:
				CUIOutfitImmunity	();
	virtual			~CUIOutfitImmunity	();

	CUIStatic* m_name; // texture + name
	CUIStatic* m_value1; // 100%
	CUIStatic* m_value2; // 100%

	void	InitFromXml			( CUIXml& xml_doc, LPCSTR base_str, u32 hit_type );
}; // class CUIOutfitImmunity


class CUIOutfitInfo : public CUIWindow
{
public:
					CUIOutfitInfo			();
	virtual			~CUIOutfitInfo			();

			void 	Update( CCustomOutfit* outfit, bool af = true );
			void 	InitFromXml				(CUIXml& xml_doc);
protected:

	float			GetArtefactParam		(ActorRestoreParams params, u32 i);

	CUIStatic*	m_listWnd;

	enum{
		_item_start = 0,
		_item_health_restore_speed = _item_start,
		_item_radiation_restore_speed,
		_item_satiety_restore_speed,
		_item_thirst_restore_speed,
		_item_power_restore_speed,
		_item_bleeding_restore_speed,
		_item_psy_health_restore_speed,

		_max_item_index1,

		_item_burn_immunity = _max_item_index1,
		_item_strike_immunity,
		_item_shock_immunity,
		_item_wound_immunity,
		_item_radiation_immunity,
		_item_telepatic_immunity,
		_item_chemical_burn_immunity,
		_item_explosion_immunit,
		_item_fire_wound_immunity,

		_max_item_index,
	};
	CUIOutfitImmunity*		m_items[_max_item_index];
};

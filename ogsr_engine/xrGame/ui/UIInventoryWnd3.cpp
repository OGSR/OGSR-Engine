#include "stdafx.h"
#include "UIInventoryWnd.h"
#include "../actor.h"
#include "../silencer.h"
#include "../scope.h"
#include "../grenadelauncher.h"
#include "../Artifact.h"
#include "../eatable_item.h"
#include "../BottleItem.h"
#include "../WeaponMagazined.h"
#include "../WeaponMagazinedWGrenade.h"
#include "../inventory.h"
#include "../game_base.h"
#include "../game_cl_base.h"
#include "../xr_level_controller.h"
#include "UICellItem.h"
#include "UIListBoxItem.h"
#include "../CustomOutfit.h"
#include "../string_table.h"
#include <regex>

void CUIInventoryWnd::EatItem(PIItem itm)
{
	SetCurrentItem							(NULL);
	if(!itm->Useful())						return;

	SendEvent_Item_Eat						(itm);

	PlaySnd									(eInvItemUse);
}

#include "../Medkit.h"
#include "../Antirad.h"
void CUIInventoryWnd::ActivatePropertiesBox()
{
	// Флаг-признак для невлючения пункта контекстного меню: Dreess Outfit, если костюм уже надет
	bool bAlreadyDressed = false; 

	UIPropertiesBox.RemoveAll();

	CMedkit*			pMedkit				= smart_cast<CMedkit*>			(CurrentIItem());
	CAntirad*			pAntirad			= smart_cast<CAntirad*>			(CurrentIItem());
	CEatableItem*		pEatableItem		= smart_cast<CEatableItem*>		(CurrentIItem());
	CCustomOutfit*		pOutfit				= smart_cast<CCustomOutfit*>	(CurrentIItem());
	CWeapon*			pWeapon				= smart_cast<CWeapon*>			(CurrentIItem());
	CBottleItem*		pBottleItem			= smart_cast<CBottleItem*>		(CurrentIItem());
    
	bool b_show = false;

	if (!pOutfit && CurrentIItem()->GetSlot() != NO_ACTIVE_SLOT) {
		auto slots = CurrentIItem()->GetSlots();
		bool multi_slot = slots.size() > 1;
		for (u8 i = 0; i < (u8)slots.size(); ++i) {
			auto slot = slots[i];
			if (slot != NO_ACTIVE_SLOT && slot != GRENADE_SLOT) {
				if (!m_pInv->m_slots[slot].m_pIItem || m_pInv->m_slots[slot].m_pIItem != CurrentIItem()) {
					if (multi_slot && Core.Features.test(xrCore::Feature::slots_extend_menu))
					{
						string128 full_action_text;
						strconcat(sizeof(full_action_text), full_action_text, "st_move_to_slot_", std::to_string(slot).c_str());
						UIPropertiesBox.AddItem(full_action_text, (void*)(__int64)slot, INVENTORY_TO_SLOT_ACTION);
						b_show = true;
					}
					else 
					{
						UIPropertiesBox.AddItem("st_move_to_slot", NULL, INVENTORY_TO_SLOT_ACTION);
						b_show = true;
						break;
					}
				}
			}
		}
	}


	if(CurrentIItem()->Belt() && m_pInv->CanPutInBelt(CurrentIItem()))
	{
		UIPropertiesBox.AddItem("st_move_on_belt",  NULL, INVENTORY_TO_BELT_ACTION);
		b_show = true;
	}

	if(CurrentIItem()->Ruck() && m_pInv->CanPutInRuck(CurrentIItem()) && (CurrentIItem()->GetSlot() == NO_ACTIVE_SLOT || !m_pInv->m_slots[CurrentIItem()->GetSlot()].m_bPersistent) )
	{
		if(!pOutfit)
			UIPropertiesBox.AddItem("st_move_to_bag",  NULL, INVENTORY_TO_BAG_ACTION);
		else
			UIPropertiesBox.AddItem("st_undress_outfit",  NULL, INVENTORY_TO_BAG_ACTION);
		bAlreadyDressed = true;
		b_show			= true;
	}
	if(pOutfit  && !bAlreadyDressed )
	{
		UIPropertiesBox.AddItem("st_dress_outfit",  NULL, INVENTORY_TO_SLOT_ACTION);
		b_show = true;
	}
	
	//отсоединение аддонов от вещи
	if(pWeapon)
	{
		if(pWeapon->GrenadeLauncherAttachable() && pWeapon->IsGrenadeLauncherAttached())
		{
			UIPropertiesBox.AddItem("st_detach_gl",  NULL, INVENTORY_DETACH_GRENADE_LAUNCHER_ADDON);
			b_show = true;
		}
		if(pWeapon->ScopeAttachable() && pWeapon->IsScopeAttached())
		{
			UIPropertiesBox.AddItem("st_detach_scope",  NULL, INVENTORY_DETACH_SCOPE_ADDON);
			b_show = true;
		}
		if(pWeapon->SilencerAttachable() && pWeapon->IsSilencerAttached())
		{
			UIPropertiesBox.AddItem("st_detach_silencer",  NULL, INVENTORY_DETACH_SILENCER_ADDON);
			b_show = true;
		}
		if(smart_cast<CWeaponMagazined*>(pWeapon))
		{
			auto WpnMagazWgl = smart_cast<CWeaponMagazinedWGrenade*>(pWeapon);
			bool b = pWeapon->GetAmmoElapsed() > 0 || ( WpnMagazWgl && !WpnMagazWgl->m_magazine2.empty() );

			if(!b)
			{
				CUICellItem * itm = CurrentItem();
				for(u32 i=0; i<itm->ChildsCount(); ++i)
				{
					auto pWeaponChild = static_cast<CWeaponMagazined*>(itm->Child(i)->m_pData);
					auto WpnMagazWglChild = smart_cast<CWeaponMagazinedWGrenade*>(pWeaponChild);
					if (pWeaponChild->GetAmmoElapsed() > 0 || ( WpnMagazWglChild && !WpnMagazWglChild->m_magazine2.empty() ))
					{
						b = true;
						break;
					}
				}
			}

			if(b){
				UIPropertiesBox.AddItem("st_unload_magazine",  NULL, INVENTORY_UNLOAD_MAGAZINE);
				b_show = true;
			}
		}
	}
	
	//присоединение аддонов к оружиям в слотах
        static std::regex addon_re( R"(\{ADDON\})" );
        static std::regex wpn_re( R"(\{WPN\})" );
	for (u8 i = 0; i < SLOTS_TOTAL; ++i) {
		PIItem tgt = m_pInv->m_slots[i].m_pIItem;
		if (tgt && tgt->CanAttach(CurrentIItem())) {
			string128 trans_str;
			strconcat(sizeof(trans_str), trans_str, "st_attach_addon_to_wpn_in_slot_", std::to_string(i).c_str());
			string128 str = { 0 };
			// В локализации должно быть что-то типа 'Прикрепить %s к %s в таком-то слоте'
			std::snprintf(str, sizeof(str), CStringTable().translate(trans_str).c_str(), CurrentIItem()->m_nameShort.c_str(), tgt->m_nameShort.c_str());
                        std::string s( str );
                        s = std::regex_replace( s, addon_re, CurrentIItem()->m_nameShort.c_str() );
                        s = std::regex_replace( s, wpn_re,   tgt->m_nameShort.c_str() );
			UIPropertiesBox.AddItem( s.c_str(), (void*)tgt, INVENTORY_ATTACH_ADDON );
			b_show = true;
		}
	}


	LPCSTR _action = nullptr;

	if(pMedkit || pAntirad)
	{
		_action					= "st_use";
	}
	else if(pEatableItem)
	{
		if(pBottleItem)
			_action					= "st_drink";
		else
			_action					= "st_eat";
	}

	if(_action){
		UIPropertiesBox.AddItem(_action,  NULL, INVENTORY_EAT_ACTION);
		b_show			= true;
	}

	bool disallow_drop	= (pOutfit&&bAlreadyDressed);
	disallow_drop		|= !!CurrentIItem()->IsQuestItem();

	if(!disallow_drop)
	{

		UIPropertiesBox.AddItem("st_drop", NULL, INVENTORY_DROP_ACTION);
		b_show			= true;

		if(CurrentItem()->ChildsCount())
			UIPropertiesBox.AddItem("st_drop_all", (void*)33, INVENTORY_DROP_ACTION);
	}

	if(b_show)
	{
		UIPropertiesBox.AutoUpdateSize	();
		UIPropertiesBox.BringAllToTop	();

		Fvector2						cursor_pos;
		Frect							vis_rect;
		GetAbsoluteRect					(vis_rect);
		cursor_pos						= GetUICursor()->GetCursorPosition();
		cursor_pos.sub					(vis_rect.lt);
		UIPropertiesBox.Show			(vis_rect, cursor_pos);
		PlaySnd							(eInvProperties);
	}
}

void CUIInventoryWnd::ProcessPropertiesBoxClicked	()
{
	if(UIPropertiesBox.GetClickedItem())
	{
		switch(UIPropertiesBox.GetClickedItem()->GetTAG())
		{

                case INVENTORY_TO_SLOT_ACTION: {
                  auto item  = CurrentIItem();
				  // Явно указали слот в меню
				  void* d = UIPropertiesBox.GetClickedItem()->GetData();
				  if (d) 
				  {
					  auto slot = (u8)(__int64)d;
					  item->SetSlot(slot);
					  if (ToSlot(CurrentItem(), true))
						  return;
				  }
				  // Пытаемся найти свободный слот из списка разрешенных.
				  // Если его нету, то принудительно займет первый слот,
				  // указанный в списке.
                  auto slots = item->GetSlots();
                  for ( u8 i = 0; i < (u8)slots.size(); ++i ) {
                    item->SetSlot( slots[ i ] );
                    if ( ToSlot( CurrentItem(), false ) )
                      return;
                  }
                  item->SetSlot( slots.size() ? slots[ 0 ]: NO_ACTIVE_SLOT );
                  ToSlot( CurrentItem(), true );
                  break;
                }

		case INVENTORY_TO_BELT_ACTION:	
			ToBelt(CurrentItem(),false);
			break;
		case INVENTORY_TO_BAG_ACTION:	
			ToBag(CurrentItem(),false);
			break;
		case INVENTORY_DROP_ACTION:
			{
				void* d = UIPropertiesBox.GetClickedItem()->GetData();
				bool b_all = (d==(void*)33);

				DropCurrentItem(b_all);
			}break;
		case INVENTORY_EAT_ACTION:
			EatItem(CurrentIItem());
			break;
		case INVENTORY_ATTACH_ADDON:
			AttachAddon((PIItem)(UIPropertiesBox.GetClickedItem()->GetData()));
			break;
		case INVENTORY_DETACH_SCOPE_ADDON:
			DetachAddon(*(smart_cast<CWeapon*>(CurrentIItem()))->GetScopeName());
			break;
		case INVENTORY_DETACH_SILENCER_ADDON:
			DetachAddon(*(smart_cast<CWeapon*>(CurrentIItem()))->GetSilencerName());
			break;
		case INVENTORY_DETACH_GRENADE_LAUNCHER_ADDON:
			DetachAddon(*(smart_cast<CWeapon*>(CurrentIItem()))->GetGrenadeLauncherName());
			break;
		case INVENTORY_RELOAD_MAGAZINE:
			(smart_cast<CWeapon*>(CurrentIItem()))->Action(kWPN_RELOAD, CMD_START);
			break;
		case INVENTORY_UNLOAD_MAGAZINE:
			{
				auto ProcessUnload = [](void* pWpn) {
					auto WpnMagaz = static_cast<CWeaponMagazined*>(pWpn);
					WpnMagaz->UnloadMagazine();
					if (auto WpnMagazWgl = smart_cast<CWeaponMagazinedWGrenade*>(WpnMagaz))
					{
						if (WpnMagazWgl->IsGrenadeLauncherAttached())
						{
							WpnMagazWgl->PerformSwitchGL();
							WpnMagazWgl->UnloadMagazine();
							WpnMagazWgl->PerformSwitchGL();
						}
					}
				};

				auto itm = CurrentItem();
				ProcessUnload(itm->m_pData);

				for (u32 i = 0; i < itm->ChildsCount(); ++i)
				{
					auto child_itm = itm->Child(i);
					ProcessUnload(child_itm->m_pData);
				}
			}break;
		}
	}
}

bool CUIInventoryWnd::TryUseItem(PIItem itm)
{
	CBottleItem*		pBottleItem			= smart_cast<CBottleItem*>		(itm);
	CMedkit*			pMedkit				= smart_cast<CMedkit*>			(itm);
	CAntirad*			pAntirad			= smart_cast<CAntirad*>			(itm);
	CEatableItem*		pEatableItem		= smart_cast<CEatableItem*>		(itm);

	if(pMedkit || pAntirad || pEatableItem || pBottleItem)
	{
		EatItem(itm);
		return true;
	}
	return false;
}

bool CUIInventoryWnd::DropItem(PIItem itm, CUIDragDropListEx* lst)
{
	if(lst==m_pUIOutfitList)
	{
		return TryUseItem			(itm);
/*
		CCustomOutfit*		pOutfit		= smart_cast<CCustomOutfit*>	(CurrentIItem());
		if(pOutfit)
			ToSlot			(CurrentItem(), true);
		else
			EatItem				(CurrentIItem());

		return				true;
*/
	}
	CUICellItem*	_citem	= lst->ItemsCount() ? lst->GetItemIdx(0) : NULL;
	PIItem _iitem	= _citem ? (PIItem)_citem->m_pData : NULL;

	if(!_iitem)						return	false;
	if(!_iitem->CanAttach(itm))		return	false;
	AttachAddon						(_iitem);

	return							true;
}

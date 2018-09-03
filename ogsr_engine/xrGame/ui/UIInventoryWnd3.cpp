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
#include "../inventory.h"
#include "../game_base.h"
#include "../game_cl_base.h"
#include "../xr_level_controller.h"
#include "UICellItem.h"
#include "UIListBoxItem.h"
#include "../CustomOutfit.h"


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
	// ‘лаг-признак дл€ невлючени€ пункта контекстного меню: Dreess Outfit, если костюм уже надет
	bool bAlreadyDressed = false; 

		
	UIPropertiesBox.RemoveAll();

	CMedkit*			pMedkit				= smart_cast<CMedkit*>			(CurrentIItem());
	CAntirad*			pAntirad			= smart_cast<CAntirad*>			(CurrentIItem());
	CEatableItem*		pEatableItem		= smart_cast<CEatableItem*>		(CurrentIItem());
	CCustomOutfit*		pOutfit				= smart_cast<CCustomOutfit*>	(CurrentIItem());
	CWeapon*			pWeapon				= smart_cast<CWeapon*>			(CurrentIItem());
	CScope*				pScope				= smart_cast<CScope*>			(CurrentIItem());
	CSilencer*			pSilencer			= smart_cast<CSilencer*>		(CurrentIItem());
	CGrenadeLauncher*	pGrenadeLauncher	= smart_cast<CGrenadeLauncher*>	(CurrentIItem());
	CBottleItem*		pBottleItem			= smart_cast<CBottleItem*>		(CurrentIItem());
    
	bool b_show = false;
	bool extend_new_wpn_menu = false;

#ifdef NEW_WPN_SLOTS_EXTEND_MENU
	extend_new_wpn_menu = true;
#endif // NEW_WPN_SLOTS_EXTEND_MENU

#ifdef NEW_WPN_SLOTS

	if (!pOutfit && CurrentIItem()->GetSlot() != NO_ACTIVE_SLOT) {
		auto slots = CurrentIItem()->GetSlots();
		bool multi_slot = slots.size() > 1;
		for (u8 i = 0; i < (u8)slots.size(); ++i) {
			auto slot = slots[i];
			if (slot != NO_ACTIVE_SLOT && slot != GRENADE_SLOT) {
				if (!m_pInv->m_slots[slot].m_pIItem || m_pInv->m_slots[slot].m_pIItem != CurrentIItem()) {
					b_show = true;

					if (multi_slot && extend_new_wpn_menu)
					{
						string256 full_action_text;
						string16 tmp;

						strconcat(sizeof(full_action_text), full_action_text, "st_move_to_slot_", itoa(slot, tmp, 10));
						UIPropertiesBox.AddItem(full_action_text, (void*)slot, INVENTORY_TO_SLOT_ACTION);
					}
					else 
					{
						UIPropertiesBox.AddItem("st_move_to_slot", NULL, INVENTORY_TO_SLOT_ACTION);
						break;
					}
				};
			};
		};
	};

#else
	if (!pOutfit && CurrentIItem()->GetSlot() != NO_ACTIVE_SLOT )
	{
		if ((CurrentIItem()->GetSlot() == FIRST_WEAPON_SLOT) || (CurrentIItem()->GetSlot() == SECOND_WEAPON_SLOT))
		{
			if (!m_pInv->m_slots[FIRST_WEAPON_SLOT].m_bPersistent && m_pInv->CanPutInSlot(CurrentIItem(), FIRST_WEAPON_SLOT))
			{
				UIPropertiesBox.AddItem("st_move_to_slot_1", NULL, INVENTORY_TO_WEAPON_SLOT_1_ACTION);
				b_show = true;
			}
			if (!m_pInv->m_slots[SECOND_WEAPON_SLOT].m_bPersistent && m_pInv->CanPutInSlot(CurrentIItem(), SECOND_WEAPON_SLOT))
			{
				UIPropertiesBox.AddItem("st_move_to_slot_2", NULL, INVENTORY_TO_WEAPON_SLOT_2_ACTION);
				b_show = true;
			}
		}
		else if (!m_pInv->m_slots[CurrentIItem()->GetSlot()].m_bPersistent && m_pInv->CanPutInSlot(CurrentIItem()))
		{
			UIPropertiesBox.AddItem("st_move_to_slot", NULL, INVENTORY_TO_SLOT_ACTION);
			b_show = true;
		}
	}
#endif


	if(CurrentIItem()->Belt() && m_pInv->CanPutInBelt(CurrentIItem()))
	{
		UIPropertiesBox.AddItem("st_move_on_belt",  NULL, INVENTORY_TO_BELT_ACTION);
		b_show			= true;
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
		b_show			= true;
	}
	
	//отсоединение аддонов от вещи
	if(pWeapon)
	{
		if(pWeapon->GrenadeLauncherAttachable() && pWeapon->IsGrenadeLauncherAttached())
		{
			UIPropertiesBox.AddItem("st_detach_gl",  NULL, INVENTORY_DETACH_GRENADE_LAUNCHER_ADDON);
		b_show			= true;
		}
		if(pWeapon->ScopeAttachable() && pWeapon->IsScopeAttached())
		{
			UIPropertiesBox.AddItem("st_detach_scope",  NULL, INVENTORY_DETACH_SCOPE_ADDON);
		b_show			= true;
		}
		if(pWeapon->SilencerAttachable() && pWeapon->IsSilencerAttached())
		{
			UIPropertiesBox.AddItem("st_detach_silencer",  NULL, INVENTORY_DETACH_SILENCER_ADDON);
		b_show			= true;
		}
		if(smart_cast<CWeaponMagazined*>(pWeapon))
		{
			bool b = (0!=pWeapon->GetAmmoElapsed());

			if(!b)
			{
				CUICellItem * itm = CurrentItem();
				for(u32 i=0; i<itm->ChildsCount(); ++i)
				{
					pWeapon		= smart_cast<CWeaponMagazined*>((CWeapon*)itm->Child(i)->m_pData);
					if(pWeapon->GetAmmoElapsed())
					{
						b = true;
						break;
					}
				}
			}

			if(b){
				UIPropertiesBox.AddItem("st_unload_magazine",  NULL, INVENTORY_UNLOAD_MAGAZINE);
				b_show			= true;
			}
		}
	}
	
	//присоединение аддонов к активному слоту (2 или 3)
	if (pScope)
	{
#ifdef NEW_WPN_SLOTS

		for (u8 i = 0; i < SLOTS_TOTAL; ++i) {
			PIItem tgt = m_pInv->m_slots[i].m_pIItem;
			if (tgt && tgt->CanAttach(pScope)) {
				b_show = true;
				if (extend_new_wpn_menu)
				{
					string256 full_action_text;
					string16 tmp;

					strconcat(sizeof(full_action_text), full_action_text, "st_attach_scope_to_rifle_", itoa(i, tmp, 10));

					UIPropertiesBox.AddItem(full_action_text, (void*)tgt, INVENTORY_ATTACH_ADDON);
				}
				else 
				{
					UIPropertiesBox.AddItem("st_attach_scope_to_rifle", (void*)tgt, INVENTORY_ATTACH_ADDON);
					b_show = true;
					break;
				}
			}
		};

#else
		if (m_pInv->m_slots[FIRST_WEAPON_SLOT].m_pIItem != NULL &&
			m_pInv->m_slots[FIRST_WEAPON_SLOT].m_pIItem->CanAttach(pScope))
		{
			PIItem tgt = m_pInv->m_slots[FIRST_WEAPON_SLOT].m_pIItem;
			UIPropertiesBox.AddItem("st_attach_scope_to_pistol", (void*)tgt, INVENTORY_ATTACH_ADDON);
			b_show = true;
		}
		if (m_pInv->m_slots[SECOND_WEAPON_SLOT].m_pIItem != NULL &&
			m_pInv->m_slots[SECOND_WEAPON_SLOT].m_pIItem->CanAttach(pScope))
		{
			PIItem tgt = m_pInv->m_slots[SECOND_WEAPON_SLOT].m_pIItem;
			UIPropertiesBox.AddItem("st_attach_scope_to_rifle", (void*)tgt, INVENTORY_ATTACH_ADDON);
			b_show = true;
		}
#endif
		}
	else if (pSilencer)
	{
#ifdef NEW_WPN_SLOTS

		for (u8 i = 0; i < SLOTS_TOTAL; ++i) {
			PIItem tgt = m_pInv->m_slots[i].m_pIItem;
			if (tgt && tgt->CanAttach(pSilencer)) {
				b_show = true;
				if (extend_new_wpn_menu)
				{
					string256 full_action_text;
					string16 tmp;

					strconcat(sizeof(full_action_text), full_action_text, "st_attach_silencer_to_rifle_", itoa(i, tmp, 10));

					UIPropertiesBox.AddItem(full_action_text, (void*)tgt, INVENTORY_ATTACH_ADDON);
				}
				else
				{
					UIPropertiesBox.AddItem("st_attach_silencer_to_rifle", (void*)tgt, INVENTORY_ATTACH_ADDON);
					b_show = true;
					break;
				}
			}
		};

#else
		if (m_pInv->m_slots[FIRST_WEAPON_SLOT].m_pIItem != NULL &&
			m_pInv->m_slots[FIRST_WEAPON_SLOT].m_pIItem->CanAttach(pSilencer))
		{
			PIItem tgt = m_pInv->m_slots[FIRST_WEAPON_SLOT].m_pIItem;
			UIPropertiesBox.AddItem("st_attach_silencer_to_pistol", (void*)tgt, INVENTORY_ATTACH_ADDON);
			b_show = true;
		}
		if (m_pInv->m_slots[SECOND_WEAPON_SLOT].m_pIItem != NULL &&
			m_pInv->m_slots[SECOND_WEAPON_SLOT].m_pIItem->CanAttach(pSilencer))
		{
			PIItem tgt = m_pInv->m_slots[SECOND_WEAPON_SLOT].m_pIItem;
			UIPropertiesBox.AddItem("st_attach_silencer_to_rifle", (void*)tgt, INVENTORY_ATTACH_ADDON);
			b_show = true;
		}
#endif
		}
	else if (pGrenadeLauncher)
	{
#ifdef NEW_WPN_SLOTS

		for (u8 i = 0; i < SLOTS_TOTAL; ++i) {
			PIItem tgt = m_pInv->m_slots[i].m_pIItem;
			if (tgt && tgt->CanAttach(pGrenadeLauncher)) {
				b_show = true;
				if (extend_new_wpn_menu)
				{
					string256 full_action_text;
					string16 tmp;

					strconcat(sizeof(full_action_text), full_action_text, "st_attach_gl_to_rifle_", itoa(i, tmp, 10));

					UIPropertiesBox.AddItem(full_action_text, (void*)tgt, INVENTORY_ATTACH_ADDON);
				}
				else
				{
					UIPropertiesBox.AddItem("st_attach_gl_to_rifle", (void*)tgt, INVENTORY_ATTACH_ADDON);
					b_show = true;
					break;
				}
			}
		};

#else
		if (m_pInv->m_slots[SECOND_WEAPON_SLOT].m_pIItem != NULL &&
			m_pInv->m_slots[SECOND_WEAPON_SLOT].m_pIItem->CanAttach(pGrenadeLauncher))
		{
			PIItem tgt = m_pInv->m_slots[SECOND_WEAPON_SLOT].m_pIItem;
			UIPropertiesBox.AddItem("st_attach_gl_to_rifle", (void*)tgt, INVENTORY_ATTACH_ADDON);
			b_show = true;
		}

#endif
		}
	LPCSTR _action = NULL;

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
#ifdef NEW_WPN_SLOTS

                case INVENTORY_TO_SLOT_ACTION: {
                  auto item  = CurrentIItem();
				  // явно указали слот в меню
				  void* d = UIPropertiesBox.GetClickedItem()->GetData();
				  if (d) 
				  {
					  auto slot = (u8)d;
					  item->SetSlot(slot);
					  if (ToSlot(CurrentItem(), true))
						  return;
				  }
				  // ѕытаемс€ найти свободный слот из списка разрешенных.
				  // ≈сли его нету, то принудительно займет первый слот,
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

#else
		case INVENTORY_TO_WEAPON_SLOT_1_ACTION:
			ToSlot(CurrentItem(), FIRST_WEAPON_SLOT, true);
			break;
		case INVENTORY_TO_WEAPON_SLOT_2_ACTION:
			ToSlot(CurrentItem(), SECOND_WEAPON_SLOT, true);
			break;
		case INVENTORY_TO_SLOT_ACTION:	
			ToSlot(CurrentItem(), true);
			break;
#endif
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
				CUICellItem * itm = CurrentItem();
				(smart_cast<CWeaponMagazined*>((CWeapon*)itm->m_pData))->UnloadMagazine();
				for(u32 i=0; i<itm->ChildsCount(); ++i)
				{
					CUICellItem * child_itm			= itm->Child(i);
					(smart_cast<CWeaponMagazined*>((CWeapon*)child_itm->m_pData))->UnloadMagazine();
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

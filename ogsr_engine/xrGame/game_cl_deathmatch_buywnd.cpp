#include "stdafx.h"
#include "game_cl_deathmatch.h"
#include "level.h"
#include "actor.h"
#include "inventory.h"
#include "xrServer_Objects_ALife_Items.h"
#include "weapon.h"
#include "xr_level_controller.h"
#include "eatable_item_object.h"
#include "Missile.h"
#include "clsid_game.h"

static	u16 SlotsToCheck [] = {
//		APPARATUS_SLOT	,		// 4
		OUTFIT_SLOT		,		// 5
//		KNIFE_SLOT		,		// 0
//		GRENADE_SLOT	,		// 3
		FIRST_WEAPON_SLOT		,		// 1
		SECOND_WEAPON_SLOT		,		// 2
};

#define UNBUYABLESLOT		20

//s16	game_cl_Deathmatch::GetBuyMenuItemIndex		(u8 SlotID, u8 ItemID)
s16	game_cl_Deathmatch::GetBuyMenuItemIndex		(u8 Addons, u8 ItemID)
{
//	R_ASSERT2(SlotID != 0xff && ItemID != 0xff, "Bad Buy Manu Item");
//	if (SlotID == OUTFIT_SLOT) SlotID = APPARATUS_SLOT;
	s16	ID = (s16(Addons) << 0x08) | s16(ItemID);
	return ID;
};

void game_cl_Deathmatch::OnBuyMenu_Ok	()
{
	if (!m_bBuyEnabled) return;
	CObject *l_pObj = Level().CurrentEntity();

	CGameObject *l_pPlayer = smart_cast<CGameObject*>(l_pObj);
	if(!l_pPlayer) return;
	CActor* pActor = smart_cast<CActor*>(l_pObj);

	game_PlayerState* Pl = local_player;
	if (!Pl) return;

	NET_Packet		P;
	l_pPlayer->u_EventGen		(P,GE_GAME_EVENT,l_pPlayer->ID()	);
	P.w_u16(GAME_EVENT_PLAYER_BUY_FINISHED);
	//-------------------------------------------------------------------------------
//	pCurPresetItems->clear();
	PRESET_ITEMS	tmpItems;
	tmpItems.clear();

	const preset_items* _p	= &(pCurBuyMenu->GetPreset(_preset_idx_last));//_preset_idx_last : _preset_idx_origin);
	if (_p->size() == 0) 
	{
		if (!pActor || !pActor->g_Alive())
			_p	= &(pCurBuyMenu->GetPreset(_preset_idx_origin));
	}
	
	u32 ItemsCount							= _p->size();
	for (u32 i=0; i<ItemsCount; ++i)
	{
		const _preset_item& _pitem = _p[0][i];

		for(u32 idx=0; idx<_pitem.count; ++idx)
		{
			u8 SlotID		= 0;
			u8 ItemID		= 0;
			pCurBuyMenu->GetWeaponIndexByName(_pitem.sect_name, SlotID, ItemID);

			u8 Addons		= _pitem.addon_state;

			s16 ID			= GetBuyMenuItemIndex(Addons, ItemID);
//			pCurPresetItems->push_back(ID);
			tmpItems.push_back(ID);
		}
	}

	//принудительно добавляем нож
	u8 SectID, ItemID;
	pCurBuyMenu->GetWeaponIndexByName("mp_wpn_knife", SectID, ItemID);
//	pCurPresetItems->push_back(GetBuyMenuItemIndex(SectID, ItemID));
	tmpItems.push_back(GetBuyMenuItemIndex(SectID, ItemID));
	//-------------------------------------------------------------------------------
	if (pCurBuyMenu->IsIgnoreMoneyAndRank())
	{
		P.w_s32(0);
	}
	else
	{
		s32 MoneyDiff = pCurBuyMenu->GetPresetCost(_preset_idx_origin)- pCurBuyMenu->GetPresetCost(_preset_idx_last);
//		P.w_s32		(s32(pCurBuyMenu->GetMoneyAmount()) - Pl->money_for_round);
		P.w_s32(MoneyDiff);
	}
//	P.w_u8		(u8(pCurPresetItems->size()));
//	for (u8 s=0; s<pCurPresetItems->size(); s++)
//	{
//		P.w_s16((*pCurPresetItems)[s].BigID);
//	}
	
	P.w_u8		(u8(tmpItems.size()));
	for (u8 s=0; s<tmpItems.size(); s++)
	{
		P.w_s16(tmpItems[s].BigID);
	}

	//-------------------------------------------------------------------------------
	l_pPlayer->u_EventSend		(P);
	//-------------------------------------------------------------------------------
	if (m_bMenuCalledFromReady)
	{
		OnKeyboardPress(kJUMP);
	}
};

void	game_cl_Deathmatch::OnBuyMenu_DefaultItems	()
{
	//---------------------------------------------------------
/*	PRESET_ITEMS_it It = PlayerDefItems.begin();
	PRESET_ITEMS_it Et = PlayerDefItems.end();
	for ( ; It != Et; ++It) 
	{
		s16	ItemID = (*It);

		pCurBuyMenu->SectionToSlot(u8((ItemID&0xff00)>>0x08), u8(ItemID&0x00ff), false);
	};
*/	//---------------------------------------------------------
	SetBuyMenuItems(&PlayerDefItems, TRUE);
};

void game_cl_Deathmatch::SetBuyMenuItems		(PRESET_ITEMS* pItems, BOOL OnlyPreset)
{
	game_PlayerState* P = local_player;
	if (!P) return;
	if (pCurBuyMenu->IsShown()) return;

	pCurBuyMenu->ResetItems();
	//---------------------------------------------------------
	pCurBuyMenu->SetupPlayerItemsBegin();
	//---------------------------------------------------------
	CActor* pCurActor = smart_cast<CActor*> (Level().Objects.net_Find	(P->GameID));
	if (pCurActor)
	{
		//проверяем слоты
		TISlotArr::const_iterator	ISlot = pCurActor->inventory().m_slots.begin();
		TISlotArr::const_iterator	ESlot = pCurActor->inventory().m_slots.end();

		for ( ; ISlot != ESlot; ++ISlot) 
		{
			PIItem pItem = (*ISlot).m_pIItem;
			if(!pItem)																	continue;
			if (pItem->IsInvalid() || pItem->object().CLS_ID == CLSID_OBJECT_W_KNIFE)	continue;

			if(pSettings->line_exist(GetBaseCostSect(), pItem->object().cNameSect()))
			{
				u8 Addons = 0;
				CWeapon* pWeapon = smart_cast<CWeapon*> (pItem);
				{
					if (pWeapon) Addons = pWeapon->GetAddonsState();
				}
				CWeaponAmmo* pAmmo = smart_cast<CWeaponAmmo*> (pItem);
				if (pAmmo)
				{
					if (pAmmo->m_boxCurr != pAmmo->m_boxSize) continue;
				}
				pCurBuyMenu->ItemToSlot(pItem->object().cNameSect(), Addons);
			}
		};

		//проверяем пояс
		TIItemContainer::const_iterator	IBelt = pCurActor->inventory().m_belt.begin();
		TIItemContainer::const_iterator	EBelt = pCurActor->inventory().m_belt.end();

		for ( ; IBelt != EBelt; ++IBelt) 
		{
			PIItem pItem = (*IBelt);
			if (pItem->IsInvalid() || pItem->object().CLS_ID == CLSID_OBJECT_W_KNIFE) continue;
			if(pSettings->line_exist(GetBaseCostSect(), pItem->object().cNameSect()))
			{
				CWeaponAmmo* pAmmo = smart_cast<CWeaponAmmo*> (pItem);
				if (pAmmo)
				{
					if (pAmmo->m_boxCurr != pAmmo->m_boxSize) continue;
				}
				pCurBuyMenu->ItemToBelt(pItem->object().cNameSect());
			}
		};

		//проверяем ruck
		TIItemContainer::const_iterator	IRuck = pCurActor->inventory().m_ruck.begin();
		TIItemContainer::const_iterator	ERuck = pCurActor->inventory().m_ruck.end();

		for ( ; IRuck != ERuck; ++IRuck) 
		{
			PIItem pItem = *IRuck;
			if (pItem->IsInvalid() || pItem->object().CLS_ID == CLSID_OBJECT_W_KNIFE) continue;
			if(pSettings->line_exist(GetBaseCostSect(), pItem->object().cNameSect()))
			{
				u8 Addons = 0;
				CWeapon* pWeapon = smart_cast<CWeapon*> (pItem);
				{
					if (pWeapon) Addons = pWeapon->GetAddonsState();
				}
				CWeaponAmmo* pAmmo = smart_cast<CWeaponAmmo*> (pItem);
				if (pAmmo)
				{
					if (pAmmo->m_boxCurr != pAmmo->m_boxSize) continue;
				}
				pCurBuyMenu->ItemToRuck(pItem->object().cNameSect(), Addons);
			}
		};
	}
	else
	{
		//---------------------------------------------------------
		u8 KnifeSlot, KnifeIndex;
		pCurBuyMenu->GetWeaponIndexByName("mp_wpn_knife", KnifeSlot, KnifeIndex);
		//---------------------------------------------------------
		PRESET_ITEMS		TmpPresetItems;
		PRESET_ITEMS_it		It = pItems->begin();
		PRESET_ITEMS_it		Et = pItems->end();
		for ( ; It != Et; ++It) 
		{
			PresetItem PIT = *It;
			if (PIT.ItemID == KnifeIndex) continue;
			pCurBuyMenu->ItemToSlot(pCurBuyMenu->GetWeaponNameByIndex(0, PIT.ItemID), PIT.SlotID);
		};
		//---------------------------------------------------------
	};
	pCurBuyMenu->SetMoneyAmount(P->money_for_round);
	pCurBuyMenu->SetupPlayerItemsEnd();

	pCurBuyMenu->CheckBuyAvailabilityInSlots();
};

void game_cl_Deathmatch::CheckItem			(PIItem pItem, PRESET_ITEMS* pPresetItems, BOOL OnlyPreset)
{
	R_ASSERT					(pItem);
	R_ASSERT					(pPresetItems);
	if (pItem->IsInvalid())	return;

	u8 SlotID, ItemID;
	pCurBuyMenu->GetWeaponIndexByName(*pItem->object().cNameSect(), SlotID, ItemID);
	if (SlotID == 0xff || ItemID == 0xff) return;
	s16 BigID = GetBuyMenuItemIndex(SlotID, ItemID);
	CWeaponAmmo* pAmmo = smart_cast<CWeaponAmmo*> (pItem);
	if (pAmmo)
	{
		if (pAmmo->m_boxCurr != pAmmo->m_boxSize) return;
	}
	//-----------------------------------------------------	
	PRESET_ITEMS_it PresetItemIt = std::find(pPresetItems->begin(), pPresetItems->end(), BigID);
	if (OnlyPreset)
	{		
		if (PresetItemIt == pPresetItems->end()) return;
	}

	if (SlotID == FIRST_WEAPON_SLOT)
	{
		PRESET_ITEMS_it DefPistolIt = std::find(PlayerDefItems.begin(), PlayerDefItems.end(), BigID);
		if (DefPistolIt != PlayerDefItems.end() && PresetItemIt == pPresetItems->end()) return;
	}
	
	pCurBuyMenu->SectionToSlot(SlotID, ItemID, true);
	//-----------------------------------------------------	
	s16 DesiredAddons = 0;	
	if (PresetItemIt != pPresetItems->end())
	{
		DesiredAddons = (*PresetItemIt).ItemID >> 5;
		pPresetItems->erase(PresetItemIt);
	}
	//-----------------------------------------------------
	CWeapon* pWeapon = smart_cast<CWeapon*> (pItem);
	if (pWeapon)
	{
		if (pWeapon->ScopeAttachable())
		{
			pCurBuyMenu->GetWeaponIndexByName(*pWeapon->GetScopeName(), SlotID, ItemID);
			if (SlotID != 0xff && ItemID != 0xff)
			{
				if (pWeapon->IsScopeAttached())
				{
					if ((DesiredAddons & CSE_ALifeItemWeapon::eWeaponAddonScope) || !OnlyPreset)
						pCurBuyMenu->AddonToSlot(CSE_ALifeItemWeapon::eWeaponAddonScope, pWeapon->GetSlot(), true);
				}					
				else
				{
					if (DesiredAddons & CSE_ALifeItemWeapon::eWeaponAddonScope)
						pCurBuyMenu->AddonToSlot(CSE_ALifeItemWeapon::eWeaponAddonScope, pWeapon->GetSlot(), false);
				}
			}
		};

		if (pWeapon->GrenadeLauncherAttachable())
		{
			pCurBuyMenu->GetWeaponIndexByName(*pWeapon->GetGrenadeLauncherName(), SlotID, ItemID);
			if (SlotID != 0xff && ItemID != 0xff)
			{
				if (pWeapon->IsGrenadeLauncherAttached())
				{
					if ((DesiredAddons & CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher) || !OnlyPreset)
						pCurBuyMenu->AddonToSlot(CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher, pWeapon->GetSlot(), true);
				}
				else
				{
					if (DesiredAddons & CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher)
						pCurBuyMenu->AddonToSlot(CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher, pWeapon->GetSlot(), false);
				}
			}
		};

		if (pWeapon->SilencerAttachable())
		{
			pCurBuyMenu->GetWeaponIndexByName(*pWeapon->GetSilencerName(), SlotID, ItemID);
			if (SlotID != 0xff && ItemID != 0xff)
			{
				if (pWeapon->IsSilencerAttached())
				{
					if ((DesiredAddons & CSE_ALifeItemWeapon::eWeaponAddonSilencer) || !OnlyPreset)
						pCurBuyMenu->AddonToSlot(CSE_ALifeItemWeapon::eWeaponAddonSilencer, pWeapon->GetSlot(), true);
				}
				else
				{
					if (DesiredAddons & CSE_ALifeItemWeapon::eWeaponAddonSilencer)						
						pCurBuyMenu->AddonToSlot(CSE_ALifeItemWeapon::eWeaponAddonSilencer, pWeapon->GetSlot(), false);
				}
			}
		};
	};
};

void	game_cl_Deathmatch::LoadTeamDefaultPresetItems	(const shared_str& caSection, IBuyWnd* pBuyMenu, PRESET_ITEMS* pPresetItems)
{
	if (!pSettings->line_exist(caSection, "default_items")) return;
	if (!pBuyMenu) return;
	if (!pPresetItems) return;

	pPresetItems->clear();

	string256			ItemName;
	string4096			DefItems;
	// Читаем данные этого поля
	std::strcpy(DefItems, pSettings->r_string(caSection, "default_items"));
	u32 count	= _GetItemCount(DefItems);
	// теперь для каждое имя оружия, разделенные запятыми, заносим в массив
	for (u32 i = 0; i < count; ++i)
	{
		_GetItem(DefItems, i, ItemName);

		u8 SlotID, ItemID;
		pBuyMenu->GetWeaponIndexByName(ItemName, SlotID, ItemID);
		if (SlotID == 0xff || ItemID == 0xff) continue;
//		s16 ID = GetBuyMenuItemIndex(SlotID, ItemID);
		s16 ID = GetBuyMenuItemIndex(0, ItemID);
		pPresetItems->push_back(ID);
	};
};

void				game_cl_Deathmatch::LoadDefItemsForRank(IBuyWnd* pBuyMenu)
{
	if (!pBuyMenu) return;
	//---------------------------------------------------
	LoadPlayerDefItems(getTeamSection(local_player->team), pBuyMenu);
	//---------------------------------------------------
	string16 RankStr;
	string256 ItemStr;
	string256 NewItemStr;
	char tmp[5];
	for (int i=1; i<=local_player->rank; i++)
	{
		strconcat(sizeof(RankStr),RankStr,"rank_",itoa(i,tmp,10));
		if (!pSettings->section_exist(RankStr)) continue;
		for (u32 it=0; it<PlayerDefItems.size(); it++)
		{
//			s16* pItemID = &(PlayerDefItems[it]);
//			char* ItemName = pBuyMenu->GetWeaponNameByIndex(u8(((*pItemID)&0xff00)>>0x08), u8((*pItemID)&0x00ff));
			PresetItem *pDefItem = &(PlayerDefItems[it]);
			const shared_str& ItemName = pBuyMenu->GetWeaponNameByIndex(pDefItem->SlotID, pDefItem->ItemID);
			if (!ItemName.size()) continue;
			strconcat(sizeof(ItemStr),ItemStr, "def_item_repl_", ItemName.c_str() );
			if (!pSettings->line_exist(RankStr, ItemStr)) continue;

			strcpy_s(NewItemStr,sizeof(NewItemStr),pSettings->r_string(RankStr, ItemStr));

			u8 SlotID, ItemID;
			pBuyMenu->GetWeaponIndexByName(NewItemStr, SlotID, ItemID);
			if (SlotID == 0xff || ItemID == 0xff) continue;

//			s16 ID = GetBuyMenuItemIndex(SlotID, ItemID);
			s16 ID = GetBuyMenuItemIndex(0, ItemID);

//			*pItemID = ID;
			pDefItem->set(ID);			
		}
	}
	//---------------------------------------------------------
	for (u32 it=0; it<PlayerDefItems.size(); it++)
	{
//		s16* pItemID = &(PlayerDefItems[it]);
//		char* ItemName = pBuyMenu->GetWeaponNameByIndex(u8(((*pItemID)&0xff00)>>0x08), u8((*pItemID)&0x00ff));
		PresetItem *pDefItem = &(PlayerDefItems[it]);
		const shared_str& ItemName = pBuyMenu->GetWeaponNameByIndex(pDefItem->SlotID, pDefItem->ItemID);
		if ( !ItemName.size() ) continue;
		if (!xr_strcmp(*ItemName, "mp_wpn_knife")) continue;
		if (!pSettings->line_exist(ItemName, "ammo_class")) continue;
		
		string1024 wpnAmmos, BaseAmmoName;
		std::strcpy(wpnAmmos, pSettings->r_string(ItemName, "ammo_class"));
		_GetItem(wpnAmmos, 0, BaseAmmoName);

		u8 SlotID, ItemID;
		pBuyMenu->GetWeaponIndexByName(BaseAmmoName, SlotID, ItemID);
		if (SlotID == 0xff || ItemID == 0xff) continue;

//		s16 ID = GetBuyMenuItemIndex(SlotID, ItemID);

		s16 ID = GetBuyMenuItemIndex(0, ItemID);
		if (GameID() == GAME_ARTEFACTHUNT)
		{
			PlayerDefItems.push_back(ID);
			PlayerDefItems.push_back(ID);
		}		
	};
	//-------------------------------------------------------------
	if (pCurBuyMenu->IsShown()) return;
	pCurBuyMenu->ResetItems();
	pCurBuyMenu->SetupDefaultItemsBegin();
	//---------------------------------------------------------
	u8 KnifeSlot, KnifeIndex;
	pCurBuyMenu->GetWeaponIndexByName("mp_wpn_knife", KnifeSlot, KnifeIndex);
	//---------------------------------------------------------
	PRESET_ITEMS		TmpPresetItems;
	PRESET_ITEMS_it		It = PlayerDefItems.begin();
	PRESET_ITEMS_it		Et = PlayerDefItems.end();
	for ( ; It != Et; ++It) 
	{
		PresetItem PIT = *It;
		if (PIT.ItemID == KnifeIndex) continue;
		pCurBuyMenu->ItemToSlot(pCurBuyMenu->GetWeaponNameByIndex(0, PIT.ItemID), PIT.SlotID);
	};
	//---------------------------------------------------------
	pCurBuyMenu->SetupDefaultItemsEnd();
};

void				game_cl_Deathmatch::ChangeItemsCosts			(IBuyWnd* pBuyMenu)
{
	if (!pBuyMenu) return;
};

void				game_cl_Deathmatch::OnMoneyChanged				()
{	
	if (pCurBuyMenu && pCurBuyMenu->IsShown())
	{
		pCurBuyMenu->SetMoneyAmount(local_player->money_for_round);
		pCurBuyMenu->CheckBuyAvailabilityInSlots();
	}
}
#include "stdafx.h"
#include "UICarBodyWnd.h"
#include "xrUIXmlParser.h"
#include "UIXmlInit.h"
#include "../HUDManager.h"
#include "../level.h"
#include "UICharacterInfo.h"
#include "UIDragDropListEx.h"
#include "UIFrameWindow.h"
#include "UIItemInfo.h"
#include "UIPropertiesBox.h"
#include "../ai/monsters/BaseMonster/base_monster.h"
#include "../inventory.h"
#include "UIInventoryUtilities.h"
#include "UICellItem.h"
#include "UICellItemFactory.h"
#include "../WeaponMagazined.h"
#include "../WeaponMagazinedWGrenade.h"
#include "../Actor.h"
#include "../eatable_item.h"
#include "../alife_registry_wrappers.h"
#include "UI3tButton.h"
#include "UIListBoxItem.h"
#include "../InventoryBox.h"
#include "../game_object_space.h"
#include "../script_callback_ex.h"
#include "../script_game_object.h"
#include "../BottleItem.h"
#include "../xr_3da/xr_input.h"

#define				CAR_BODY_XML		"carbody_new.xml"
#define				CARBODY_ITEM_XML	"carbody_item.xml"

void move_item (u16 from_id, u16 to_id, u16 what_id);

CUICarBodyWnd::CUICarBodyWnd()
{
	m_pInventoryBox		= NULL;
	Init				();
	Hide				();
	m_b_need_update		= false;
}

CUICarBodyWnd::~CUICarBodyWnd()
{
	m_pUIOurBagList->ClearAll					(true);
	m_pUIOthersBagList->ClearAll				(true);
}

void CUICarBodyWnd::Init()
{
	CUIXml						uiXml;
	uiXml.Init					(CONFIG_PATH, UI_PATH, CAR_BODY_XML);
	
	CUIXmlInit					xml_init;

	xml_init.InitWindow			(uiXml, "main", 0, this);

	m_pUIStaticTop				= xr_new<CUIStatic>(); m_pUIStaticTop->SetAutoDelete(true);
	AttachChild					(m_pUIStaticTop);
	xml_init.InitStatic			(uiXml, "top_background", 0, m_pUIStaticTop);


	m_pUIStaticBottom			= xr_new<CUIStatic>(); m_pUIStaticBottom->SetAutoDelete(true);
	AttachChild					(m_pUIStaticBottom);
	xml_init.InitStatic			(uiXml, "bottom_background", 0, m_pUIStaticBottom);

	m_pUIOurIcon				= xr_new<CUIStatic>(); m_pUIOurIcon->SetAutoDelete(true);
	AttachChild					(m_pUIOurIcon);
	xml_init.InitStatic			(uiXml, "static_icon", 0, m_pUIOurIcon);

	m_pUIOthersIcon				= xr_new<CUIStatic>(); m_pUIOthersIcon->SetAutoDelete(true);
	AttachChild					(m_pUIOthersIcon);
	xml_init.InitStatic			(uiXml, "static_icon", 1, m_pUIOthersIcon);


	m_pUICharacterInfoLeft		= xr_new<CUICharacterInfo>(); m_pUICharacterInfoLeft->SetAutoDelete(true);
	m_pUIOurIcon->AttachChild	(m_pUICharacterInfoLeft);
	m_pUICharacterInfoLeft->Init(0,0, m_pUIOurIcon->GetWidth(), m_pUIOurIcon->GetHeight(), "trade_character.xml");


	m_pUICharacterInfoRight			= xr_new<CUICharacterInfo>(); m_pUICharacterInfoRight->SetAutoDelete(true);
	m_pUIOthersIcon->AttachChild	(m_pUICharacterInfoRight);
	m_pUICharacterInfoRight->Init	(0,0, m_pUIOthersIcon->GetWidth(), m_pUIOthersIcon->GetHeight(), "trade_character.xml");

	m_pUIOurBagWnd					= xr_new<CUIStatic>(); m_pUIOurBagWnd->SetAutoDelete(true);
	AttachChild						(m_pUIOurBagWnd);
	xml_init.InitStatic				(uiXml, "our_bag_static", 0, m_pUIOurBagWnd);


	m_pUIOthersBagWnd				= xr_new<CUIStatic>(); m_pUIOthersBagWnd->SetAutoDelete(true);
	AttachChild						(m_pUIOthersBagWnd);
	xml_init.InitStatic				(uiXml, "others_bag_static", 0, m_pUIOthersBagWnd);

	m_pUIOurBagList					= xr_new<CUIDragDropListEx>(); m_pUIOurBagList->SetAutoDelete(true);
	m_pUIOurBagWnd->AttachChild		(m_pUIOurBagList);	
	xml_init.InitDragDropListEx		(uiXml, "dragdrop_list_our", 0, m_pUIOurBagList);

	m_pUIOthersBagList				= xr_new<CUIDragDropListEx>(); m_pUIOthersBagList->SetAutoDelete(true);
	m_pUIOthersBagWnd->AttachChild	(m_pUIOthersBagList);	
	xml_init.InitDragDropListEx		(uiXml, "dragdrop_list_other", 0, m_pUIOthersBagList);


	//информация о предмете
	m_pUIDescWnd					= xr_new<CUIFrameWindow>(); m_pUIDescWnd->SetAutoDelete(true);
	AttachChild						(m_pUIDescWnd);
	xml_init.InitFrameWindow		(uiXml, "frame_window", 0, m_pUIDescWnd);

	m_pUIStaticDesc					= xr_new<CUIStatic>(); m_pUIStaticDesc->SetAutoDelete(true);
	m_pUIDescWnd->AttachChild		(m_pUIStaticDesc);
	xml_init.InitStatic				(uiXml, "descr_static", 0, m_pUIStaticDesc);

	m_pUIItemInfo					= xr_new<CUIItemInfo>(); m_pUIItemInfo->SetAutoDelete(true);
	m_pUIDescWnd->AttachChild		(m_pUIItemInfo);
	m_pUIItemInfo->Init				(0,0, m_pUIDescWnd->GetWidth(), m_pUIDescWnd->GetHeight(), CARBODY_ITEM_XML);


	xml_init.InitAutoStatic			(uiXml, "auto_static", this);

	m_pUIPropertiesBox				= xr_new<CUIPropertiesBox>(); m_pUIPropertiesBox->SetAutoDelete(true);
	AttachChild						(m_pUIPropertiesBox);
	m_pUIPropertiesBox->Init		(0,0,300,300);
	m_pUIPropertiesBox->Hide		();

	SetCurrentItem					(NULL);
	m_pUIStaticDesc->SetText		(NULL);

	m_pUITakeAll					= xr_new<CUI3tButton>(); m_pUITakeAll->SetAutoDelete(true);
	AttachChild						(m_pUITakeAll);
	xml_init.Init3tButton			(uiXml, "take_all_btn", 0, m_pUITakeAll);

	BindDragDropListEnents			(m_pUIOurBagList);
	BindDragDropListEnents			(m_pUIOthersBagList);

	//Load sounds
	if (uiXml.NavigateToNode("action_sounds", 0))
	{
		XML_NODE* stored_root = uiXml.GetLocalRoot();
		uiXml.SetLocalRoot(uiXml.NavigateToNode("action_sounds", 0));

		::Sound->create(sounds[eInvSndOpen], uiXml.Read("snd_open", 0, NULL), st_Effect, sg_SourceType);
		::Sound->create(sounds[eInvSndClose], uiXml.Read("snd_close", 0, NULL), st_Effect, sg_SourceType);
		::Sound->create(sounds[eInvProperties], uiXml.Read("snd_properties", 0, NULL), st_Effect, sg_SourceType);
		::Sound->create(sounds[eInvDropItem], uiXml.Read("snd_drop_item", 0, NULL), st_Effect, sg_SourceType);
		::Sound->create(sounds[eInvDetachAddon], uiXml.Read("snd_detach_addon", 0, NULL), st_Effect, sg_SourceType);
		::Sound->create(sounds[eInvItemUse], uiXml.Read("snd_item_use", 0, NULL), st_Effect, sg_SourceType);

		uiXml.SetLocalRoot(stored_root);
	}
}

void CUICarBodyWnd::InitCarBody(CInventoryOwner* pOur, IInventoryBox* pInvBox)
{
    m_pOurObject									= pOur;
	m_pOthersObject									= NULL;
	m_pInventoryBox									= pInvBox;
	m_pInventoryBox->m_in_use						= true;

	u16 our_id										= smart_cast<CGameObject*>(m_pOurObject)->ID();
	m_pUICharacterInfoLeft->InitCharacter			(our_id);
	m_pUIOthersIcon->Show							(false);
	m_pUICharacterInfoRight->ClearInfo				();
	m_pUIPropertiesBox->Hide						();
	EnableAll										();
	UpdateLists										();

	if (auto obj = smart_cast<CInventoryBox*>(pInvBox))
	{
		obj->callback(GameObject::eOnInvBoxOpen)();
	}
}

void CUICarBodyWnd::InitCarBody(CInventoryOwner* pOur, CInventoryOwner* pOthers)
{
    m_pOurObject									= pOur;
	m_pOthersObject									= pOthers;
	m_pInventoryBox									= NULL;
	
	u16 our_id										= smart_cast<CGameObject*>(m_pOurObject)->ID();
	u16 other_id									= smart_cast<CGameObject*>(m_pOthersObject)->ID();

	m_pUICharacterInfoLeft->InitCharacter			(our_id);
	m_pUIOthersIcon->Show							(true);
	
	CBaseMonster *monster = NULL;
	if(m_pOthersObject) {
		monster										= smart_cast<CBaseMonster *>(m_pOthersObject);
		if (monster || m_pOthersObject->use_simplified_visual() ) 
		{
			m_pUICharacterInfoRight->ClearInfo		();
			if(monster)
			{
				shared_str monster_tex_name = pSettings->r_string(monster->cNameSect(),"icon");
				m_pUICharacterInfoRight->UIIcon().InitTexture(monster_tex_name.c_str());
				m_pUICharacterInfoRight->UIIcon().SetStretchTexture(true);
			}
		}else 
		{
			m_pUICharacterInfoRight->InitCharacter	(other_id);
		}
	}

	m_pUIPropertiesBox->Hide						();
	EnableAll										();
	UpdateLists										();

	if(!monster){
		CInfoPortionWrapper	*known_info_registry	= xr_new<CInfoPortionWrapper>();
		known_info_registry->registry().init		(other_id);
		KNOWN_INFO_VECTOR& known_info				= known_info_registry->registry().objects();

		KNOWN_INFO_VECTOR_IT it = known_info.begin();
		for(int i=0;it!=known_info.end();++it,++i){
			NET_Packet		P;
			CGameObject::u_EventGen		(P,GE_INFO_TRANSFER, our_id);
			P.w_u16						(0);//not used
			P.w_stringZ					((*it).info_id);			//сообщение
			P.w_u8						(1);						//добавление сообщения
			CGameObject::u_EventSend	(P);
		}
		known_info.clear	();
		xr_delete			(known_info_registry);
	}
}  

void CUICarBodyWnd::UpdateLists_delayed()
{
		m_b_need_update = true;
}

#include "UIInventoryUtilities.h"

void CUICarBodyWnd::Hide()
{
	InventoryUtilities::SendInfoToActor			("ui_car_body_hide");
	m_pUIOurBagList->ClearAll					(true);
	m_pUIOthersBagList->ClearAll				(true);
	inherited::Hide								();
	if(m_pInventoryBox)
		m_pInventoryBox->m_in_use				= false;

	if (Core.Features.test(xrCore::Feature::more_hide_weapon))
		if ( Actor() )
			Actor()->SetWeaponHideState(INV_STATE_BLOCK_ALL, false);

	PlaySnd(eInvSndClose);
}

void CUICarBodyWnd::UpdateLists()
{
	TIItemContainer								ruck_list;
	m_pUIOurBagList->ClearAll					(true);
	m_pUIOthersBagList->ClearAll				(true);

	ruck_list.clear								();
	m_pOurObject->inventory().AddAvailableItems	(ruck_list, true);
	std::sort									(ruck_list.begin(),ruck_list.end(),InventoryUtilities::GreaterRoomInRuck);

	//Наш рюкзак
	for(const auto& inv_item : ruck_list)
	{
		CUICellItem* itm = create_cell_item(inv_item);
		if (inv_item->m_highlight_equipped)
		{
			itm->m_select_equipped = true;
			itm->SetColor(reinterpret_cast<CInventoryItem*>(itm->m_pData)->ClrEquipped);
		}
		m_pUIOurBagList->SetItem(itm);
	}


	ruck_list.clear									();
	if(m_pOthersObject)
		m_pOthersObject->inventory().AddAvailableItems	(ruck_list, false);
	else
		m_pInventoryBox->AddAvailableItems			(ruck_list);

	std::sort										(ruck_list.begin(),ruck_list.end(),InventoryUtilities::GreaterRoomInRuck);

	//Чужой рюкзак
	for (const auto& inv_item : ruck_list)
	{
		CUICellItem* itm = create_cell_item(inv_item);
		m_pUIOthersBagList->SetItem(itm);
	}

	InventoryUtilities::UpdateWeight				(*m_pUIOurBagWnd);
	m_b_need_update									= false;
}

void CUICarBodyWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if (BUTTON_CLICKED == msg)
	{
		if (m_pUITakeAll == pWnd)
		{
			TakeAll();
		}
	}
	else if(pWnd == m_pUIPropertiesBox &&	msg == PROPERTY_CLICKED)
	{
		if(m_pUIPropertiesBox->GetClickedItem())
		{
			switch(m_pUIPropertiesBox->GetClickedItem()->GetTAG())
			{
				case INVENTORY_EAT_ACTION:	//съесть объект
					EatItem();
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
				case INVENTORY_DETACH_SCOPE_ADDON:
				{
					auto wpn = smart_cast<CWeapon*>(CurrentIItem());
					wpn->Detach(wpn->GetScopeName().c_str(), true);
					PlaySnd(eInvDetachAddon);
				}break;
				case INVENTORY_DETACH_SILENCER_ADDON:
				{
					auto wpn = smart_cast<CWeapon*>(CurrentIItem());
					wpn->Detach(wpn->GetSilencerName().c_str(), true);
					PlaySnd(eInvDetachAddon);
				}break;
				case INVENTORY_DETACH_GRENADE_LAUNCHER_ADDON:
				{
					auto wpn = smart_cast<CWeapon*>(CurrentIItem());
					wpn->Detach(wpn->GetGrenadeLauncherName().c_str(), true);
					PlaySnd(eInvDetachAddon);
				}break;
				case INVENTORY_MOVE_ACTION:
				{
					void* d = m_pUIPropertiesBox->GetClickedItem()->GetData();
					bool b_all = (d == (void*)33);

					if (b_all)
						MoveItems(CurrentItem());
					else
						MoveItem(CurrentItem());
				}break;
				case INVENTORY_DROP_ACTION:
				{
					void* d = m_pUIPropertiesBox->GetClickedItem()->GetData();
					bool b_all = (d == (void*)33);

					DropItemsfromCell(b_all);
				}break;
			}

			// refresh if nessesary
			switch (m_pUIPropertiesBox->GetClickedItem()->GetTAG())
			{
				case INVENTORY_UNLOAD_MAGAZINE:
				case INVENTORY_DETACH_SCOPE_ADDON:
				case INVENTORY_DETACH_SILENCER_ADDON:
				case INVENTORY_DETACH_GRENADE_LAUNCHER_ADDON:
				{
					if (m_pInventoryBox)
					{
						UpdateLists_delayed();
					}
				}break;
			}
		}
	}

	inherited::SendMessage(pWnd, msg, pData);
}

void CUICarBodyWnd::Draw()
{
	inherited::Draw	();
}

void CUICarBodyWnd::Update()
{
	if (m_b_need_update ||
		m_pOurObject->inventory().ModifyFrame() == Device.dwFrame ||
		(m_pOthersObject&&m_pOthersObject->inventory().ModifyFrame() == Device.dwFrame))
	{
		if (m_pUIOurBagList && m_pUIOthersBagList)
		{
			int our_scroll = m_pUIOurBagList->ScrollPos();
			int other_scroll = m_pUIOthersBagList->ScrollPos();
			UpdateLists();
			m_pUIOurBagList->SetScrollPos(our_scroll);
			m_pUIOthersBagList->SetScrollPos(other_scroll);
		}
	}

	CGameObject* pOurGO = smart_cast<CGameObject*>(m_pOurObject);
	CGameObject* pOtherGO = smart_cast<CGameObject*>(m_pOthersObject);
	if (pOtherGO && pOurGO->Position().distance_to(pOtherGO->Position()) - pOtherGO->Radius() - pOurGO->Radius() > m_pOurObject->inventory().GetTakeDist() + 0.5f)
	{
		GetHolder()->StartStopMenu(this, true);
	}
	inherited::Update();
}


void CUICarBodyWnd::Show() 
{ 
	if (Core.Features.test(xrCore::Feature::more_hide_weapon))
		Actor()->SetWeaponHideState(INV_STATE_BLOCK_ALL, true);

	InventoryUtilities::SendInfoToActor		("ui_car_body");
	inherited::Show							();
	SetCurrentItem							(NULL);
	InventoryUtilities::UpdateWeight		(*m_pUIOurBagWnd);

	if (Core.Features.test(xrCore::Feature::engine_ammo_repacker) && !Core.Features.test(xrCore::Feature::hard_ammo_reload))
		if (auto pActor = Actor())
			pActor->RepackAmmo();

	PlaySnd(eInvSndOpen);
}

void CUICarBodyWnd::DisableAll()
{
	m_pUIOurBagWnd->Enable			(false);
	m_pUIOthersBagWnd->Enable		(false);
}

void CUICarBodyWnd::EnableAll()
{
	m_pUIOurBagWnd->Enable			(true);
	m_pUIOthersBagWnd->Enable		(true);
}

CUICellItem* CUICarBodyWnd::CurrentItem()
{
	return m_pCurrentCellItem;
}

PIItem CUICarBodyWnd::CurrentIItem()
{
	return	(m_pCurrentCellItem)?(PIItem)m_pCurrentCellItem->m_pData : NULL;
}

void CUICarBodyWnd::SetCurrentItem(CUICellItem* itm)
{
	if(m_pCurrentCellItem == itm) return;
	m_pCurrentCellItem		= itm;
	m_pUIItemInfo->InitItem(CurrentIItem());

	if (m_pCurrentCellItem) {
		m_pCurrentCellItem->m_select_armament = true;
		auto script_obj = CurrentIItem()->object().lua_game_object();
		g_actor->callback(GameObject::eCellItemSelect)(script_obj);
	}
}

void CUICarBodyWnd::TakeAll()
{
	u32 cnt				= m_pUIOthersBagList->ItemsCount();
	u16 tmp_id = 0;
	if(m_pInventoryBox){
		tmp_id	= (smart_cast<CGameObject*>(m_pOurObject))->ID();
	}

	for(u32 i=0; i<cnt; ++i)
	{
		CUICellItem*	ci = m_pUIOthersBagList->GetItemIdx(i);
		for(u32 j=0; j<ci->ChildsCount(); ++j)
		{
			PIItem _itm		= (PIItem)(ci->Child(j)->m_pData);
			if(m_pOthersObject)
				TransferItem	(_itm, m_pOthersObject, m_pOurObject, false);
			else{
				move_item		(m_pInventoryBox->object().ID(), tmp_id, _itm->object().ID());
//.				Actor()->callback(GameObject::eInvBoxItemTake)( m_pInventoryBox->lua_game_object(), _itm->object().lua_game_object() );
			}
		
		}
		PIItem itm		= (PIItem)(ci->m_pData);
		if(m_pOthersObject)
			TransferItem	(itm, m_pOthersObject, m_pOurObject, false);
		else{
			move_item		(m_pInventoryBox->object().ID(), tmp_id, itm->object().ID());
//.			Actor()->callback(GameObject::eInvBoxItemTake)(m_pInventoryBox->lua_game_object(), itm->object().lua_game_object() );
		}

	}
}

void CUICarBodyWnd::MoveItems(CUICellItem* itm)
{
	u16 tmp_id = 0;
	if (m_pInventoryBox) {
		tmp_id = (smart_cast<CGameObject*>(m_pOurObject))->ID();
	}

	CUIDragDropListEx* owner_list = itm->OwnerList();

	if (owner_list != m_pUIOthersBagList)
	{ // from actor to box
		for (u32 j = 0; j < itm->ChildsCount(); ++j)
		{
			PIItem _itm = (PIItem)(itm->Child(j)->m_pData);
			if (m_pOthersObject)
				TransferItem(_itm, m_pOurObject, m_pOthersObject, true);
			else
			{
				move_item(tmp_id, m_pInventoryBox->object().ID(), _itm->object().ID());
				//. Actor()->callback(GameObject::eInvBoxItemTake)( m_pInventoryBox->lua_game_object(), _itm->object().lua_game_object() );
			}
		}

		PIItem p_itm = (PIItem)(itm->m_pData);
		if (m_pOthersObject)
			TransferItem(p_itm, m_pOurObject, m_pOthersObject, true);
		else
		{
			move_item(tmp_id, m_pInventoryBox->object().ID(), p_itm->object().ID());
			//. Actor()->callback(GameObject::eInvBoxItemTake)(m_pInventoryBox->lua_game_object(), itm->object().lua_game_object() );
		}
	}
	else
	{ // from box to actor
		for (u32 j = 0; j < itm->ChildsCount(); ++j)
		{
			PIItem _itm = (PIItem)(itm->Child(j)->m_pData);
			if (m_pOthersObject)
				TransferItem(_itm, m_pOthersObject, m_pOurObject, false);
			else
			{
				move_item(m_pInventoryBox->object().ID(), tmp_id, _itm->object().ID());
				//. Actor()->callback(GameObject::eInvBoxItemTake)( m_pInventoryBox->lua_game_object(), _itm->object().lua_game_object() );
			}
		}

		PIItem p_itm = (PIItem)(itm->m_pData);
		if (m_pOthersObject)
			TransferItem(p_itm, m_pOthersObject, m_pOurObject, false);
		else
		{
			move_item(m_pInventoryBox->object().ID(), tmp_id, p_itm->object().ID());
			//. Actor()->callback(GameObject::eInvBoxItemTake)(m_pInventoryBox->lua_game_object(), itm->object().lua_game_object() );
		}
	}

	owner_list->RemoveItem(itm, true);

	SetCurrentItem(NULL);
}

void SendEvent_Item_Drop(u16 from_id, PIItem	pItem)
{
	pItem->SetDropManual(TRUE);

	NET_Packet P;
	pItem->object().u_EventGen(P, GE_OWNERSHIP_REJECT, from_id);
	P.w_u16(pItem->object().ID());
	pItem->object().u_EventSend(P);
}

void CUICarBodyWnd::DropItemsfromCell(bool b_all)
{
	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if (!pActor)
	{
		return;
	}

	CUICellItem* ci = CurrentItem();
	if (!ci) 
	{
		return;
	}

	CUIDragDropListEx* old_owner = ci->OwnerList();

	u16 from_id = 0;
	if (old_owner == m_pUIOthersBagList)
	{
		from_id = (m_pInventoryBox) ? m_pInventoryBox->object().ID() : smart_cast<CGameObject*>(m_pOthersObject)->ID();
	}
	else 
	{
		from_id = smart_cast<CGameObject*>(m_pOurObject)->ID();
	}

	if (b_all)
	{
		u32 cnt = ci->ChildsCount();

		for (u32 i = 0; i<cnt; ++i) {
			CUICellItem*	itm = ci->PopChild();

			PIItem			iitm = (PIItem)itm->m_pData;
			SendEvent_Item_Drop(from_id, iitm);
		}
	}

	PIItem	iitm = (PIItem)ci->m_pData;
	SendEvent_Item_Drop(from_id, iitm);

	old_owner->RemoveItem(ci, b_all);

	SetCurrentItem(NULL);

	InventoryUtilities::UpdateWeight(*m_pUIOurBagWnd);

	PlaySnd(eInvDropItem);
}

#include "../xr_level_controller.h"

bool CUICarBodyWnd::OnKeyboard(int dik, EUIMessages keyboard_action)
{
	if (m_b_need_update)
		return true;

	if (keyboard_action == WINDOW_KEY_PRESSED)
	{
		if (m_pUIPropertiesBox->GetVisible())
			m_pUIPropertiesBox->OnKeyboard(dik, keyboard_action);
	}

	if(keyboard_action==WINDOW_KEY_PRESSED && is_binded(kUSE, dik)) 
	{
			GetHolder()->StartStopMenu(this,true);
			return true;
	}

	if (inherited::OnKeyboard(dik, keyboard_action))return true;

	return false;
}

bool CUICarBodyWnd::OnMouse(float x, float y, EUIMessages mouse_action)
{
	if (m_b_need_update)
		return true;

	if (mouse_action == WINDOW_RBUTTON_DOWN)
	{
		if (m_pUIPropertiesBox->IsShown())
		{
			m_pUIPropertiesBox->Hide();
			return						true;
		}
	}

	if (CUIWindow::OnMouse(x, y, mouse_action))
	{
		return  true;
	}

	return false;
}

#include "../Medkit.h"
#include "../Antirad.h"
#include "../CustomOutfit.h"

void CUICarBodyWnd::ActivatePropertiesBox()
{		
	m_pUIPropertiesBox->RemoveAll();
	
	CWeapon*			pWeapon			= smart_cast<CWeapon*>			(CurrentIItem());
	CEatableItem*		pEatableItem	= smart_cast<CEatableItem*>		(CurrentIItem());
	CMedkit*			pMedkit			= smart_cast<CMedkit*>			(CurrentIItem());
	CAntirad*			pAntirad		= smart_cast<CAntirad*>			(CurrentIItem());
	CBottleItem*		pBottleItem		= smart_cast<CBottleItem*>		(CurrentIItem());

    bool b_show			= false;
	
	LPCSTR _action		= NULL;

	if (pWeapon)
	{
		if (pWeapon->GrenadeLauncherAttachable() && pWeapon->IsGrenadeLauncherAttached())
		{
			m_pUIPropertiesBox->AddItem("st_detach_gl", NULL, INVENTORY_DETACH_GRENADE_LAUNCHER_ADDON);
			b_show = true;
		}
		if (pWeapon->ScopeAttachable() && pWeapon->IsScopeAttached())
		{
			m_pUIPropertiesBox->AddItem("st_detach_scope", NULL, INVENTORY_DETACH_SCOPE_ADDON);
			b_show = true;
		}
		if (pWeapon->SilencerAttachable() && pWeapon->IsSilencerAttached())
		{
			m_pUIPropertiesBox->AddItem("st_detach_silencer", NULL, INVENTORY_DETACH_SILENCER_ADDON);
			b_show = true;
		}
		if (smart_cast<CWeaponMagazined*>(pWeapon))
		{
			auto WpnMagazWgl = smart_cast<CWeaponMagazinedWGrenade*>(pWeapon);
			bool b = pWeapon->GetAmmoElapsed() > 0 || ( WpnMagazWgl && !WpnMagazWgl->m_magazine2.empty() );

			if (!b)
			{
				CUICellItem * itm = CurrentItem();
				for (u32 i = 0; i<itm->ChildsCount(); ++i)
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

			if (b) {
				m_pUIPropertiesBox->AddItem("st_unload_magazine", NULL, INVENTORY_UNLOAD_MAGAZINE);
				b_show = true;
			}
		}
	}

	if(pMedkit || pAntirad)
	{
		_action						= "st_use";
	}
	else if(pEatableItem)
	{
		if(pBottleItem)
			_action					= "st_drink";
		else
			_action					= "st_eat";
		
	}
	if (_action) {
		m_pUIPropertiesBox->AddItem(_action, NULL, INVENTORY_EAT_ACTION);
		b_show = true;
	}

	b_show = true;

	bool hasMany = CurrentItem()->ChildsCount() > 0;

	m_pUIPropertiesBox->AddItem("st_move", NULL, INVENTORY_MOVE_ACTION);

	if (hasMany)
		m_pUIPropertiesBox->AddItem("st_move_all", (void*)33, INVENTORY_MOVE_ACTION);

	m_pUIPropertiesBox->AddItem("st_drop", NULL, INVENTORY_DROP_ACTION);

	if (hasMany)
		m_pUIPropertiesBox->AddItem("st_drop_all", (void*)33, INVENTORY_DROP_ACTION);

	if(b_show){
		m_pUIPropertiesBox->AutoUpdateSize	();
		m_pUIPropertiesBox->BringAllToTop	();

		Fvector2						cursor_pos;
		Frect							vis_rect;

		GetAbsoluteRect					(vis_rect);
		cursor_pos						= GetUICursor()->GetCursorPosition();
		cursor_pos.sub					(vis_rect.lt);
		m_pUIPropertiesBox->Show		(vis_rect, cursor_pos);

		PlaySnd(eInvProperties);
	}
}

void CUICarBodyWnd::EatItem()
{
	CActor *pActor				= smart_cast<CActor*>(Level().CurrentEntity());
	if(!pActor)					return;

	CUIDragDropListEx* owner_list		= CurrentItem()->OwnerList();
	if(owner_list==m_pUIOthersBagList)
	{
		u16 owner_id				= (m_pInventoryBox)?m_pInventoryBox->object().ID():smart_cast<CGameObject*>(m_pOthersObject)->ID();

		move_item(	owner_id, //from
					Actor()->ID(), //to
					CurrentIItem()->object().ID());
	}

	NET_Packet					P;
	CGameObject::u_EventGen		(P, GEG_PLAYER_ITEM_EAT, Actor()->ID());
	P.w_u16						(CurrentIItem()->object().ID());
	CGameObject::u_EventSend	(P);

	PlaySnd(eInvItemUse);
}

bool CUICarBodyWnd::OnItemStartDrag(CUICellItem* itm)
{
	return	false; //default behaviour
}

bool CUICarBodyWnd::OnItemDrop(CUICellItem* itm)
{
	CUIDragDropListEx*	old_owner		= itm->OwnerList();
	CUIDragDropListEx*	new_owner		= CUIDragDropListEx::m_drag_item->BackList();
	
	if(old_owner==new_owner || !old_owner || !new_owner)
		return false;

	if (Level().IR_GetKeyState(DIK_LSHIFT) || Level().IR_GetKeyState(DIK_RSHIFT)) {
		MoveItems(itm);
	}
	else {
		MoveItem(itm);
	}

	return true;
}

bool CUICarBodyWnd::OnItemDbClick(CUICellItem* itm)
{
	if (Level().IR_GetKeyState(DIK_LSHIFT) || Level().IR_GetKeyState(DIK_RSHIFT)) {
		MoveItems(itm);
	}
	else {
		MoveItem(itm);
	}
	return true;
}

void CUICarBodyWnd::MoveItem(CUICellItem* itm) 
{
	CUIDragDropListEx*	old_owner = itm->OwnerList();
	CUIDragDropListEx*	new_owner = (old_owner == m_pUIOthersBagList) ? m_pUIOurBagList : m_pUIOthersBagList;

	PIItem p_itm = (PIItem)itm->m_pData;

	bool bMoveDirection = (old_owner == m_pUIOthersBagList);

	if (m_pOthersObject)
	{
		if (TransferItem(p_itm,
			bMoveDirection ? m_pOthersObject : m_pOurObject,
			bMoveDirection ? m_pOurObject : m_pOthersObject,
			!bMoveDirection)
			)
		{
			CUICellItem* ci = old_owner->RemoveItem(CurrentItem(), false);
			new_owner->SetItem(ci);
		}
	}
	else
	{
		u16 tmp_id = (smart_cast<CGameObject*>(m_pOurObject))->ID();

		move_item(
			bMoveDirection ? m_pInventoryBox->object().ID() : tmp_id,
			bMoveDirection ? tmp_id : m_pInventoryBox->object().ID(),
			p_itm->object().ID());

		//.		Actor()->callback		(GameObject::eInvBoxItemTake)(m_pInventoryBox->lua_game_object(), CurrentIItem()->object().lua_game_object() );

		CUICellItem* ci = old_owner->RemoveItem(CurrentItem(), false);
		new_owner->SetItem(ci);

	}
	SetCurrentItem(NULL);
}

bool CUICarBodyWnd::OnItemSelected(CUICellItem* itm)
{
	SetCurrentItem(itm);
	itm->ColorizeItems( { m_pUIOurBagList, m_pUIOthersBagList } );
	return false;
}

bool CUICarBodyWnd::OnItemRButtonClick(CUICellItem* itm)
{
	SetCurrentItem				(itm);
	ActivatePropertiesBox		();
	return						false;
}

void move_item (u16 from_id, u16 to_id, u16 what_id)
{
	NET_Packet P;
	CGameObject::u_EventGen					(	P,
												GE_TRANSFER_REJECT,
												from_id
											);

	P.w_u16									(what_id);
	CGameObject::u_EventSend				(P);

	//другому инвентарю - взять вещь 
	CGameObject::u_EventGen					(	P,
												GE_TRANSFER_TAKE,
												to_id
											);
	P.w_u16									(what_id);
	CGameObject::u_EventSend				(P);

}

bool CUICarBodyWnd::TransferItem(PIItem itm, CInventoryOwner* owner_from, CInventoryOwner* owner_to, bool b_check)
{
	VERIFY									(NULL==m_pInventoryBox);
	CGameObject* go_from					= smart_cast<CGameObject*>(owner_from);
	CGameObject* go_to						= smart_cast<CGameObject*>(owner_to);

	if(smart_cast<CBaseMonster*>(go_to))	return false;
	if(b_check)
	{
		float invWeight						= owner_to->inventory().CalcTotalWeight();
		float maxWeight						= owner_to->inventory().GetMaxWeight();
		float itmWeight						= itm->Weight();
		if(invWeight+itmWeight >=maxWeight)	return false;
	}

	move_item(go_from->ID(), go_to->ID(), itm->object().ID());

	return true;
}

void CUICarBodyWnd::BindDragDropListEnents(CUIDragDropListEx* lst)
{
	lst->m_f_item_drop = fastdelegate::MakeDelegate(this, &CUICarBodyWnd::OnItemDrop);
	lst->m_f_item_start_drag = fastdelegate::MakeDelegate(this, &CUICarBodyWnd::OnItemStartDrag);
	lst->m_f_item_db_click = fastdelegate::MakeDelegate(this, &CUICarBodyWnd::OnItemDbClick);
	lst->m_f_item_selected = fastdelegate::MakeDelegate(this, &CUICarBodyWnd::OnItemSelected);
	lst->m_f_item_rbutton_click = fastdelegate::MakeDelegate(this, &CUICarBodyWnd::OnItemRButtonClick);
}

void CUICarBodyWnd::PlaySnd(eInventorySndAction a)
{
	if (sounds[a]._handle())
		sounds[a].play(NULL, sm_2D);
}
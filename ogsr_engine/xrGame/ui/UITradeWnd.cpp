#include "stdafx.h"
#include "UITradeWnd.h"

#include "xrUIXmlParser.h"
#include "UIXmlInit.h"

#include "../Entity.h"
#include "../HUDManager.h"
#include "../WeaponAmmo.h"
#include "../Actor.h"
#include "../Trade.h"
#include "../UIGameSP.h"
#include "UIInventoryUtilities.h"
#include "../inventoryowner.h"
#include "../eatable_item.h"
#include "../inventory.h"
#include "../level.h"
#include "../string_table.h"
#include "../character_info.h"
#include "UIMultiTextStatic.h"
#include "UI3tButton.h"
#include "UIItemInfo.h"

#include "UICharacterInfo.h"
#include "UIDragDropListEx.h"
#include "UICellItem.h"
#include "UICellItemFactory.h"
#include "UIPropertiesBox.h"
#include "UIListBoxItem.h"

#include "../game_object_space.h"
#include "../script_callback_ex.h"
#include "../script_game_object.h"
#include "../xr_3da/xr_input.h"


#define				TRADE_XML			"trade.xml"
#define				TRADE_CHARACTER_XML	"trade_character.xml"
#define				TRADE_ITEM_XML		"trade_item.xml"


struct CUITradeInternal{
	CUIStatic			UIStaticTop;
	CUIStatic			UIStaticBottom;

	CUIStatic			UIOurBagWnd;
	CUIStatic			UIOurMoneyStatic;
	CUIStatic			UIOthersBagWnd;
	CUIStatic			UIOtherMoneyStatic;
	CUIDragDropListEx	UIOurBagList;
	CUIDragDropListEx	UIOthersBagList;

	CUIStatic			UIOurTradeWnd;
	CUIStatic			UIOthersTradeWnd;
	CUIMultiTextStatic	UIOurPriceCaption;
	CUIMultiTextStatic	UIOthersPriceCaption;
	CUIDragDropListEx	UIOurTradeList;
	CUIDragDropListEx	UIOthersTradeList;

	//кнопки
	CUI3tButton			UIPerformTradeButton;
	CUI3tButton			UIToTalkButton;

	//информация о персонажах 
	CUIStatic			UIOurIcon;
	CUIStatic			UIOthersIcon;
	CUICharacterInfo	UICharacterInfoLeft;
	CUICharacterInfo	UICharacterInfoRight;

	//информация о перетаскиваемом предмете
	CUIStatic			UIDescWnd;
	CUIItemInfo			UIItemInfo;

	SDrawStaticStruct*	UIDealMsg;
};

bool others_zero_trade;

CUITradeWnd::CUITradeWnd()
	:	m_bDealControlsVisible	(false),
		m_pTrade(NULL),
		m_pOthersTrade(NULL),
		bStarted(false)
{
	m_uidata = xr_new<CUITradeInternal>();
	Init();
	Hide();
	SetCurrentItem			(NULL);
        others_zero_trade = !!READ_IF_EXISTS( pSettings, r_bool, "trade", "others_zero_trade", false );
}

CUITradeWnd::~CUITradeWnd()
{
	m_uidata->UIOurBagList.ClearAll		(true);
	m_uidata->UIOurTradeList.ClearAll	(true);
	m_uidata->UIOthersBagList.ClearAll	(true);
	m_uidata->UIOthersTradeList.ClearAll(true);
	xr_delete							(m_uidata);
}

void CUITradeWnd::Init()
{
	CUIXml								uiXml;
	bool xml_result						= uiXml.Init(CONFIG_PATH, UI_PATH, TRADE_XML);
	R_ASSERT3							(xml_result, "xml file not found", TRADE_XML);
	CUIXmlInit							xml_init;

	xml_init.InitWindow					(uiXml, "main", 0, this);

	//статические элементы интерфейса
	AttachChild							(&m_uidata->UIStaticTop);
	xml_init.InitStatic					(uiXml, "top_background", 0, &m_uidata->UIStaticTop);
	AttachChild							(&m_uidata->UIStaticBottom);
	xml_init.InitStatic					(uiXml, "bottom_background", 0, &m_uidata->UIStaticBottom);

	//иконки с изображение нас и партнера по торговле
	AttachChild							(&m_uidata->UIOurIcon);
	xml_init.InitStatic					(uiXml, "static_icon", 0, &m_uidata->UIOurIcon);
	AttachChild							(&m_uidata->UIOthersIcon);
	xml_init.InitStatic					(uiXml, "static_icon", 1, &m_uidata->UIOthersIcon);
	m_uidata->UIOurIcon.AttachChild		(&m_uidata->UICharacterInfoLeft);
	m_uidata->UICharacterInfoLeft.Init	(0,0, m_uidata->UIOurIcon.GetWidth(), m_uidata->UIOurIcon.GetHeight(), TRADE_CHARACTER_XML);
	m_uidata->UIOthersIcon.AttachChild	(&m_uidata->UICharacterInfoRight);
	m_uidata->UICharacterInfoRight.Init	(0,0, m_uidata->UIOthersIcon.GetWidth(), m_uidata->UIOthersIcon.GetHeight(), TRADE_CHARACTER_XML);


	//Списки торговли
	AttachChild							(&m_uidata->UIOurBagWnd);
	xml_init.InitStatic					(uiXml, "our_bag_static", 0, &m_uidata->UIOurBagWnd);
	AttachChild							(&m_uidata->UIOthersBagWnd);
	xml_init.InitStatic					(uiXml, "others_bag_static", 0, &m_uidata->UIOthersBagWnd);

	m_uidata->UIOurBagWnd.AttachChild	(&m_uidata->UIOurMoneyStatic);
	xml_init.InitStatic					(uiXml, "our_money_static", 0, &m_uidata->UIOurMoneyStatic);

	m_uidata->UIOthersBagWnd.AttachChild(&m_uidata->UIOtherMoneyStatic);
	xml_init.InitStatic					(uiXml, "other_money_static", 0, &m_uidata->UIOtherMoneyStatic);

	AttachChild							(&m_uidata->UIOurTradeWnd);
	xml_init.InitStatic					(uiXml, "static", 0, &m_uidata->UIOurTradeWnd);
	AttachChild							(&m_uidata->UIOthersTradeWnd);
	xml_init.InitStatic					(uiXml, "static", 1, &m_uidata->UIOthersTradeWnd);

	m_uidata->UIOurTradeWnd.AttachChild	(&m_uidata->UIOurPriceCaption);
	xml_init.InitMultiTextStatic		(uiXml, "price_mt_static", 0, &m_uidata->UIOurPriceCaption);

	m_uidata->UIOthersTradeWnd.AttachChild(&m_uidata->UIOthersPriceCaption);
	xml_init.InitMultiTextStatic		(uiXml, "price_mt_static", 0, &m_uidata->UIOthersPriceCaption);

	//Списки Drag&Drop
	m_uidata->UIOurBagWnd.AttachChild	(&m_uidata->UIOurBagList);	
	xml_init.InitDragDropListEx			(uiXml, "dragdrop_list", 0, &m_uidata->UIOurBagList);

	m_uidata->UIOthersBagWnd.AttachChild(&m_uidata->UIOthersBagList);	
	xml_init.InitDragDropListEx			(uiXml, "dragdrop_list", 1, &m_uidata->UIOthersBagList);

	m_uidata->UIOurTradeWnd.AttachChild	(&m_uidata->UIOurTradeList);	
	xml_init.InitDragDropListEx			(uiXml, "dragdrop_list", 2, &m_uidata->UIOurTradeList);

	m_uidata->UIOthersTradeWnd.AttachChild(&m_uidata->UIOthersTradeList);	
	xml_init.InitDragDropListEx			(uiXml, "dragdrop_list", 3, &m_uidata->UIOthersTradeList);

	
	AttachChild							(&m_uidata->UIDescWnd);
	xml_init.InitStatic					(uiXml, "desc_static", 0, &m_uidata->UIDescWnd);
	m_uidata->UIDescWnd.AttachChild		(&m_uidata->UIItemInfo);
	m_uidata->UIItemInfo.Init			(0,0, m_uidata->UIDescWnd.GetWidth(), m_uidata->UIDescWnd.GetHeight(), TRADE_ITEM_XML);


	xml_init.InitAutoStatic				(uiXml, "auto_static", this);


	AttachChild							(&m_uidata->UIPerformTradeButton);
	xml_init.Init3tButton				(uiXml, "button", 0, &m_uidata->UIPerformTradeButton);

	AttachChild							(&m_uidata->UIToTalkButton);
	xml_init.Init3tButton				(uiXml, "button", 1, &m_uidata->UIToTalkButton);

	m_pUIPropertiesBox					= xr_new<CUIPropertiesBox>(); m_pUIPropertiesBox->SetAutoDelete(true);
	AttachChild(m_pUIPropertiesBox);
	m_pUIPropertiesBox->Init(0, 0, 300, 300);
	m_pUIPropertiesBox->Hide();

	m_uidata->UIDealMsg					= NULL;

	BindDragDropListEnents				(&m_uidata->UIOurBagList);
	BindDragDropListEnents				(&m_uidata->UIOthersBagList);
	BindDragDropListEnents				(&m_uidata->UIOurTradeList);
	BindDragDropListEnents				(&m_uidata->UIOthersTradeList);
}

void CUITradeWnd::InitTrade(CInventoryOwner* pOur, CInventoryOwner* pOthers)
{
	VERIFY								(pOur);
	VERIFY								(pOthers);

	m_pInvOwner							= pOur;
	m_pOthersInvOwner					= pOthers;
	m_uidata->UIOthersPriceCaption.GetPhraseByIndex(0)->SetText(*CStringTable().translate("ui_st_opponent_items"));

	m_uidata->UICharacterInfoLeft.InitCharacter(m_pInvOwner->object_id());
	m_uidata->UICharacterInfoRight.InitCharacter(m_pOthersInvOwner->object_id());

	m_pInv								= &m_pInvOwner->inventory();
	m_pOthersInv						= pOur->GetTrade()->GetPartnerInventory();
		
	m_pTrade							= pOur->GetTrade();
	m_pOthersTrade						= pOur->GetTrade()->GetPartnerTrade();

	m_pUIPropertiesBox->Hide();
    	
	EnableAll							();

	UpdateLists							(eBoth);
}  

void CUITradeWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == &m_uidata->UIToTalkButton && msg == BUTTON_CLICKED)
	{
		SwitchToTalk();
	}
	else if(pWnd == &m_uidata->UIPerformTradeButton && msg == BUTTON_CLICKED)
	{
		PerformTrade();
	}
	else if (pWnd == m_pUIPropertiesBox && msg == PROPERTY_CLICKED)
	{
		if (m_pUIPropertiesBox->GetClickedItem())
		{
			switch (m_pUIPropertiesBox->GetClickedItem()->GetTAG())
			{			
				case INVENTORY_MOVE_ACTION:
				{
					void* d = m_pUIPropertiesBox->GetClickedItem()->GetData();
					bool b_all = (d == (void*)33);

					MoveItems(CurrentItem(), b_all);
				}break;
			}
		}
	}

	CUIWindow::SendMessage(pWnd, msg, pData);
}

void CUITradeWnd::Draw()
{
	inherited::Draw				();
	if(m_uidata->UIDealMsg)		m_uidata->UIDealMsg->Draw();

}

extern void UpdateCameraDirection(CGameObject* pTo);

void CUITradeWnd::Update()
{
	EListType et					= eNone;

	if(m_pInv->ModifyFrame()==Device.dwFrame && m_pOthersInv->ModifyFrame()==Device.dwFrame){
		et = eBoth;
	}else if(m_pInv->ModifyFrame()==Device.dwFrame){
		et = e1st;
	}else if(m_pOthersInv->ModifyFrame()==Device.dwFrame){
		et = e2nd;
	}
	if(et!=eNone)
		UpdateLists					(et);

	inherited::Update				();
	UpdateCameraDirection			(smart_cast<CGameObject*>(m_pOthersInvOwner));

	if(m_uidata->UIDealMsg){
		m_uidata->UIDealMsg->Update();
		if( !m_uidata->UIDealMsg->IsActual()){
			HUD().GetUI()->UIGame()->RemoveCustomStatic("not_enough_money_mine");
			HUD().GetUI()->UIGame()->RemoveCustomStatic("not_enough_money_other");
			m_uidata->UIDealMsg			= NULL;
		}
	}
}

#include "UIInventoryUtilities.h"
void CUITradeWnd::Show()
{
	InventoryUtilities::SendInfoToActor("ui_trade");
	inherited::Show					(true);
	inherited::Enable				(true);

	SetCurrentItem					(NULL);
	ResetAll						();
	m_uidata->UIDealMsg				= NULL;

	if (Core.Features.test(xrCore::Feature::engine_ammo_repacker) && !Core.Features.test(xrCore::Feature::hard_ammo_reload))
		if (auto pActor = Actor())
			pActor->RepackAmmo();
}

void CUITradeWnd::Hide()
{
	InventoryUtilities::SendInfoToActor("ui_trade_hide");
	inherited::Show					(false);
	inherited::Enable				(false);
	if(bStarted)
		StopTrade					();
	
	m_uidata->UIDealMsg				= NULL;

	if(HUD().GetUI()->UIGame()){
		HUD().GetUI()->UIGame()->RemoveCustomStatic("not_enough_money_mine");
		HUD().GetUI()->UIGame()->RemoveCustomStatic("not_enough_money_other");
	}

	m_uidata->UIOurBagList.ClearAll		(true);
	m_uidata->UIOurTradeList.ClearAll	(true);
	m_uidata->UIOthersBagList.ClearAll	(true);
	m_uidata->UIOthersTradeList.ClearAll(true);
}

void CUITradeWnd::StartTrade()
{
	if (m_pTrade)					m_pTrade->TradeCB(true);
	if (m_pOthersTrade)				m_pOthersTrade->TradeCB(true);
	bStarted						= true;
}

void CUITradeWnd::StopTrade()
{
	if (m_pTrade)					m_pTrade->TradeCB(false);
	if (m_pOthersTrade)				m_pOthersTrade->TradeCB(false);
	bStarted						= false;
}

#include "../trade_parameters.h"
bool CUITradeWnd::CanMoveToOther( PIItem pItem, bool our )
{
	if (pItem->m_flags.test(CInventoryItem::FIAlwaysUntradable))
		return false;
	if ( !our ) return true;

	float r1				= CalcItemsWeight(&m_uidata->UIOurTradeList);	// our
	float r2				= CalcItemsWeight(&m_uidata->UIOthersTradeList);	// other

	float itmWeight			= pItem->Weight();
	float otherInvWeight	= m_pOthersInv->CalcTotalWeight();
	float otherMaxWeight	= m_pOthersInv->GetMaxWeight();

	if (!m_pOthersInvOwner->trade_parameters().enabled(
			CTradeParameters::action_buy(0),
			pItem->object().cNameSect()
		))
		return				(false);

	if ( pItem->GetCondition() < m_pOthersInvOwner->trade_parameters().factors( CTradeParameters::action_buy( 0 ), pItem->object().cNameSect() ).min_condition() )
	  return false;

	if(otherInvWeight-r2+r1+itmWeight > otherMaxWeight)
		return				false;

	return true;
}

void move_item(CUICellItem* itm, CUIDragDropListEx* from, CUIDragDropListEx* to)
{
	CUICellItem* _itm		= from->RemoveItem	(itm, false);
	to->SetItem				(_itm);
}

bool CUITradeWnd::ToOurTrade(CUICellItem* itm)
{
	if ( !CanMoveToOther((PIItem)m_pCurrentCellItem->m_pData, true ) ) return false;

	move_item				(itm, &m_uidata->UIOurBagList, &m_uidata->UIOurTradeList);
	UpdatePrices			();
	return					true;
}

bool CUITradeWnd::ToOthersTrade(CUICellItem* itm)
{
	if ( !CanMoveToOther((PIItem)m_pCurrentCellItem->m_pData, false ) ) return false;

	move_item				(itm, &m_uidata->UIOthersBagList, &m_uidata->UIOthersTradeList);
	UpdatePrices			();

	return					true;
}

bool CUITradeWnd::ToOurBag(CUICellItem* itm)
{
	move_item				(itm, &m_uidata->UIOurTradeList, &m_uidata->UIOurBagList);
	UpdatePrices			();
	
	return					true;
}

bool CUITradeWnd::ToOthersBag(CUICellItem* itm)
{
	move_item				(itm, &m_uidata->UIOthersTradeList, &m_uidata->UIOthersBagList);
	UpdatePrices			();

	return					true;
}

float CUITradeWnd::CalcItemsWeight(CUIDragDropListEx* pList)
{
	float res = 0.0f;

	for(u32 i=0; i<pList->ItemsCount(); ++i)
	{
		CUICellItem* itm	= pList->GetItemIdx	(i);
		PIItem	iitem		= (PIItem)itm->m_pData;
		res					+= iitem->Weight();
		for(u32 j=0; j<itm->ChildsCount(); ++j){
			PIItem	jitem		= (PIItem)itm->Child(j)->m_pData;
			res					+= jitem->Weight();
		}
	}
	return res;
}

u32 CUITradeWnd::CalcItemsPrice(CUIDragDropListEx* pList, CTrade* pTrade, bool bBuying)
{
	u32 iPrice				= 0;
	
	for(u32 i=0; i<pList->ItemsCount(); ++i)
	{
		CUICellItem* itm	= pList->GetItemIdx(i);
		PIItem iitem		= (PIItem)itm->m_pData;
		iPrice				+= pTrade->GetItemPrice(iitem, bBuying);

		for(u32 j=0; j<itm->ChildsCount(); ++j){
			PIItem jitem	= (PIItem)itm->Child(j)->m_pData;
			iPrice			+= pTrade->GetItemPrice(jitem, bBuying);
		}

	}

	return					iPrice;
}

void CUITradeWnd::PerformTrade()
{

	if (m_uidata->UIOurTradeList.ItemsCount()==0 && m_uidata->UIOthersTradeList.ItemsCount()==0) 
		return;

	int our_money			= (int)m_pInvOwner->get_money();
	int others_money		= (int)m_pOthersInvOwner->get_money();

	int delta_price			= int(m_iOurTradePrice-m_iOthersTradePrice);

	our_money				+= delta_price;
	others_money			-= delta_price;

	if(our_money>=0 && others_money>=0 && (m_iOurTradePrice>=0 || m_iOthersTradePrice>0))
	{
		m_pOthersTrade->OnPerformTrade(m_iOthersTradePrice, m_iOurTradePrice);
		
		TransferItems		(&m_uidata->UIOurTradeList,		&m_uidata->UIOthersBagList, m_pOthersTrade,	true);
		TransferItems		(&m_uidata->UIOthersTradeList,	&m_uidata->UIOurBagList,	m_pOthersTrade,	false);
                if ( others_zero_trade ) {
                  m_pOthersTrade->pThis.inv_owner->set_money( others_money, true );
                  m_pOthersTrade->pPartner.inv_owner->set_money( our_money, true );
                }
	}else
	{
		if(others_money<0)
			m_uidata->UIDealMsg		= HUD().GetUI()->UIGame()->AddCustomStatic("not_enough_money_other", true);
		else
			m_uidata->UIDealMsg		= HUD().GetUI()->UIGame()->AddCustomStatic("not_enough_money_mine", true);


		m_uidata->UIDealMsg->m_endTime	= Device.fTimeGlobal+2.0f;// sec
	}
	SetCurrentItem			(NULL);
}

#include "../xr_level_controller.h"

bool CUITradeWnd::OnKeyboard(int dik, EUIMessages keyboard_action)
{
	if (m_pUIPropertiesBox->GetVisible())
		m_pUIPropertiesBox->OnKeyboard(dik, keyboard_action);

	if (inherited::OnKeyboard(dik, keyboard_action))return true;

	return false;
}

bool CUITradeWnd::OnMouse(float x, float y, EUIMessages mouse_action)
{
	if (mouse_action == WINDOW_RBUTTON_DOWN)
	{
		if (m_pUIPropertiesBox->IsShown())
		{
			m_pUIPropertiesBox->Hide();
			return						true;
		}
	}

	CUIWindow::OnMouse(x, y, mouse_action);

	return true; // always returns true, because ::StopAnyMove() == true;
}

void CUITradeWnd::ActivatePropertiesBox()
{
	m_pUIPropertiesBox->RemoveAll();

	bool hasMany = CurrentItem()->ChildsCount() > 0;

	m_pUIPropertiesBox->AddItem("st_move", NULL, INVENTORY_MOVE_ACTION);

	if (hasMany)
		m_pUIPropertiesBox->AddItem("st_move_all", (void*)33, INVENTORY_MOVE_ACTION);
	
	m_pUIPropertiesBox->AutoUpdateSize();
	m_pUIPropertiesBox->BringAllToTop();

	Fvector2						cursor_pos;
	Frect							vis_rect;

	GetAbsoluteRect(vis_rect);
	cursor_pos = GetUICursor()->GetCursorPosition();
	cursor_pos.sub(vis_rect.lt);
	m_pUIPropertiesBox->Show(vis_rect, cursor_pos);
}


void CUITradeWnd::DisableAll()
{
	m_uidata->UIOurBagWnd.Enable			(false);
	m_uidata->UIOthersBagWnd.Enable			(false);
	m_uidata->UIOurTradeWnd.Enable			(false);
	m_uidata->UIOthersTradeWnd.Enable		(false);
}

void CUITradeWnd::EnableAll()
{
	m_uidata->UIOurBagWnd.Enable			(true);
	m_uidata->UIOthersBagWnd.Enable			(true);
	m_uidata->UIOurTradeWnd.Enable			(true);
	m_uidata->UIOthersTradeWnd.Enable		(true);
}

void CUITradeWnd::UpdatePrices()
{
	m_iOurTradePrice	= CalcItemsPrice	(&m_uidata->UIOurTradeList,		m_pOthersTrade, true);
	m_iOthersTradePrice = CalcItemsPrice	(&m_uidata->UIOthersTradeList,	m_pOthersTrade, false);

	if ( !m_pOthersInvOwner->InfinitiveMoney() ) {
          u32 others_money = m_pOthersInvOwner->get_money();
          if ( others_zero_trade && m_iOurTradePrice > others_money )
            m_iOurTradePrice = others_money;
	}

	string256				buf;
	sprintf_s( buf, "%d RU", m_iOurTradePrice );
	m_uidata->UIOurPriceCaption.GetPhraseByIndex(2)->str = buf;
	sprintf_s					(buf, "%d RU", m_iOthersTradePrice);
	m_uidata->UIOthersPriceCaption.GetPhraseByIndex(2)->str = buf;

	sprintf_s					(buf, "%d RU", m_pInvOwner->get_money());
	m_uidata->UIOurMoneyStatic.SetText(buf);

	if(!m_pOthersInvOwner->InfinitiveMoney()){
          sprintf_s( buf, "%d RU", (int)m_pOthersInvOwner->get_money() );
          m_uidata->UIOtherMoneyStatic.SetText(buf);
	}else
	{
		m_uidata->UIOtherMoneyStatic.SetText("---");
	}
}

void CUITradeWnd::TransferItems(CUIDragDropListEx* pSellList,
								CUIDragDropListEx* pBuyList,
								CTrade* pTrade,
								bool bBuying)
{
	while(pSellList->ItemsCount())
	{
		CUICellItem* itm	=	pSellList->RemoveItem(pSellList->GetItemIdx(0),false);
		auto InvItm = (PIItem)itm->m_pData;
		InvItm->m_highlight_equipped = false; //Убираем подсветку после продажи предмета
		itm->m_select_equipped = false;
		pTrade->TransferItem( InvItm, bBuying, !others_zero_trade );
		pBuyList->SetItem		(itm);
	}

        if ( !others_zero_trade ) {
          pTrade->pThis.inv_owner->set_money ( pTrade->pThis.inv_owner->get_money(), true );
          pTrade->pPartner.inv_owner->set_money( pTrade->pPartner.inv_owner->get_money(), true );
        }
}

void CUITradeWnd::UpdateLists(EListType mode)
{
	if(mode==eBoth||mode==e1st){
		m_uidata->UIOurBagList.ClearAll(true);
		m_uidata->UIOurTradeList.ClearAll(true);
	}

	if(mode==eBoth||mode==e2nd){
		m_uidata->UIOthersBagList.ClearAll(true);
		m_uidata->UIOthersTradeList.ClearAll(true);
	}

	UpdatePrices						();


	if(mode==eBoth||mode==e1st){
		ruck_list.clear					();
   		m_pInv->AddAvailableItems		(ruck_list, true);
		std::sort						(ruck_list.begin(),ruck_list.end(),InventoryUtilities::GreaterRoomInRuck);
		FillList						(ruck_list, m_uidata->UIOurBagList, true);
	}

	if(mode==eBoth||mode==e2nd){
		ruck_list.clear					();
		m_pOthersInv->AddAvailableItems	(ruck_list, true);
		std::sort						(ruck_list.begin(),ruck_list.end(),InventoryUtilities::GreaterRoomInRuck);
		FillList						(ruck_list, m_uidata->UIOthersBagList, false);
	}
}

void CUITradeWnd::FillList	(TIItemContainer& cont, CUIDragDropListEx& dragDropList, bool our)
{
	TIItemContainer::iterator it	= cont.begin();
	TIItemContainer::iterator it_e	= cont.end();

	for(; it != it_e; ++it)
	{
		CInventoryItem* item = *it;
		CUICellItem* itm = create_cell_item( item );
		if (item->m_highlight_equipped)
			itm->m_select_equipped = true;
		bool canTrade = CanMoveToOther( item, our );
		ColorizeItem( itm, canTrade, itm->m_select_equipped );
		dragDropList.SetItem( itm );
	}
}

bool CUITradeWnd::OnItemStartDrag(CUICellItem* itm)
{
	return false; //default behaviour
}

bool CUITradeWnd::OnItemSelected(CUICellItem* itm)
{
	SetCurrentItem(itm);
	itm->ColorizeItems( { &m_uidata->UIOurTradeList, &m_uidata->UIOthersTradeList, &m_uidata->UIOurBagList, &m_uidata->UIOthersBagList } );
	return false;
}

bool CUITradeWnd::OnItemRButtonClick(CUICellItem* itm)
{
	SetCurrentItem				(itm);
	ActivatePropertiesBox();
	return						false;
}


bool CUITradeWnd::OnItemDrop(CUICellItem* itm)
{
	CUIDragDropListEx*	old_owner		= itm->OwnerList();
	CUIDragDropListEx*	new_owner		= CUIDragDropListEx::m_drag_item->BackList();
	if(old_owner==new_owner || !old_owner || !new_owner)
		return false;

	if (Level().IR_GetKeyState(DIK_LSHIFT)) {
		MoveItems(itm, true);
	}
	else {
		MoveItem(itm);
	}

	return true;
}

bool CUITradeWnd::OnItemDbClick(CUICellItem* itm)
{
	SetCurrentItem						(itm);

	if (Level().IR_GetKeyState(DIK_LSHIFT)) {
		MoveItems(itm, true);
	}
	else {
		MoveItem(itm);
	}

	return true;
}

void CUITradeWnd::MoveItems(CUICellItem* itm, bool b_all)
{
	if (!itm)
	{
		return;
	}

	if (b_all)
	{
		u32 cnt = itm->ChildsCount();

		//Msg("Move all items %d", cnt);

		CUIDragDropListEx* old_owner = itm->OwnerList();
		CUIDragDropListEx* to = nullptr;

		if (old_owner == &m_uidata->UIOurBagList)
		{
			if (!CanMoveToOther((PIItem)itm->m_pData, true)) return;

			to = &m_uidata->UIOurTradeList;
		}
		else if (old_owner == &m_uidata->UIOurTradeList)
			to = &m_uidata->UIOurBagList;
		else if (old_owner == &m_uidata->UIOthersBagList)
		{
			if (!CanMoveToOther((PIItem)itm->m_pData, false)) return;

			to = &m_uidata->UIOthersTradeList;
		}
		else if (old_owner == &m_uidata->UIOthersTradeList)
			to = &m_uidata->UIOthersBagList;

		R_ASSERT( to != nullptr );

		for (u32 i = 0; i < cnt; ++i) 
		{
			CUICellItem* child_itm = itm->PopChild();

			//Msg("MoveAllItems child ... %d", i);

			to->SetItem(child_itm);
		}
	}

	MoveItem(itm);
}

bool CUITradeWnd::MoveItem(CUICellItem* itm) 
{
	CUIDragDropListEx*	old_owner = itm->OwnerList();

	if (old_owner == &m_uidata->UIOurBagList)
		ToOurTrade(itm);
	else if (old_owner == &m_uidata->UIOurTradeList)
		ToOurBag(itm);
	else if (old_owner == &m_uidata->UIOthersBagList)
		ToOthersTrade(itm);
	else if (old_owner == &m_uidata->UIOthersTradeList)
		ToOthersBag(itm);

	return true;
}

CUICellItem* CUITradeWnd::CurrentItem()
{
	return m_pCurrentCellItem;
}

PIItem CUITradeWnd::CurrentIItem()
{
	return	(m_pCurrentCellItem)?(PIItem)m_pCurrentCellItem->m_pData : NULL;
}

void CUITradeWnd::SetCurrentItem(CUICellItem* itm)
{
	if(m_pCurrentCellItem == itm) return;
	m_pCurrentCellItem				= itm;
	m_uidata->UIItemInfo.InitItem	(CurrentIItem());
	
	if(!m_pCurrentCellItem)		return;

	m_pCurrentCellItem->m_select_armament = true;

	CUIDragDropListEx* owner	= itm->OwnerList();
	bool bBuying				= (owner==&m_uidata->UIOurBagList) || (owner==&m_uidata->UIOurTradeList);

	if(m_uidata->UIItemInfo.UICost){

		string256			str;

		sprintf_s				(str, "%d RU", m_pOthersTrade->GetItemPrice(CurrentIItem(), bBuying) );
		m_uidata->UIItemInfo.UICost->SetText (str);
	}

	auto script_obj = CurrentIItem()->object().lua_game_object();
	g_actor->callback(GameObject::eCellItemSelect)(script_obj);
}

void CUITradeWnd::SwitchToTalk()
{
	GetMessageTarget()->SendMessage		(this, TRADE_WND_CLOSED);
}

void CUITradeWnd::BindDragDropListEnents(CUIDragDropListEx* lst)
{
	lst->m_f_item_drop				= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CUITradeWnd::OnItemDrop);
	lst->m_f_item_start_drag		= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CUITradeWnd::OnItemStartDrag);
	lst->m_f_item_db_click			= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CUITradeWnd::OnItemDbClick);
	lst->m_f_item_selected			= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CUITradeWnd::OnItemSelected);
	lst->m_f_item_rbutton_click		= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CUITradeWnd::OnItemRButtonClick);
}

void CUITradeWnd::ColorizeItem(CUICellItem* itm, bool canTrade, bool highlighted)
{
	static const bool highlight_cop_enabled = !Core.Features.test(xrCore::Feature::colorize_untradable); //Это опция для Dsh, не убирать!

	if (!canTrade) {
		if ( highlight_cop_enabled )
			itm->m_select_untradable = true;
		itm->SetColor(reinterpret_cast<CInventoryItem*>(itm->m_pData)->ClrUntradable);
	}
	else {
		if ( highlight_cop_enabled )
			itm->m_select_untradable = false;
		if ( highlighted )
			itm->SetColor(reinterpret_cast<CInventoryItem*>(itm->m_pData)->ClrEquipped);
	}
}

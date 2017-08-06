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
	xml_init.Init3tButton					(uiXml, "button", 0, &m_uidata->UIPerformTradeButton);

	AttachChild							(&m_uidata->UIToTalkButton);
	xml_init.Init3tButton					(uiXml, "button", 1, &m_uidata->UIToTalkButton);

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
bool CUITradeWnd::CanMoveToOther(PIItem pItem)
{

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

	if(otherInvWeight-r2+r1+itmWeight > otherMaxWeight)
		return				false;

	return true;
}

void move_item(CUICellItem* itm, CUIDragDropListEx* from, CUIDragDropListEx* to)
{
	CUICellItem* _itm		= from->RemoveItem	(itm, false);
	to->SetItem				(_itm);
}

bool CUITradeWnd::ToOurTrade()
{
	if (!CanMoveToOther(CurrentIItem()))	return false;

	move_item				(CurrentItem(), &m_uidata->UIOurBagList, &m_uidata->UIOurTradeList);
	UpdatePrices			();
	return					true;
}

bool CUITradeWnd::ToOthersTrade()
{
	move_item				(CurrentItem(), &m_uidata->UIOthersBagList, &m_uidata->UIOthersTradeList);
	UpdatePrices			();

	return					true;
}

bool CUITradeWnd::ToOurBag()
{
	move_item				(CurrentItem(), &m_uidata->UIOurTradeList, &m_uidata->UIOurBagList);
	UpdatePrices			();
	
	return					true;
}

bool CUITradeWnd::ToOthersBag()
{
	move_item				(CurrentItem(), &m_uidata->UIOthersTradeList, &m_uidata->UIOthersBagList);
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


	string256				buf;
	sprintf_s					(buf, "%d RU", m_iOurTradePrice);
	m_uidata->UIOurPriceCaption.GetPhraseByIndex(2)->str = buf;
	sprintf_s					(buf, "%d RU", m_iOthersTradePrice);
	m_uidata->UIOthersPriceCaption.GetPhraseByIndex(2)->str = buf;

	sprintf_s					(buf, "%d RU", m_pInvOwner->get_money());
	m_uidata->UIOurMoneyStatic.SetText(buf);

	if(!m_pOthersInvOwner->InfinitiveMoney()){
		sprintf_s					(buf, "%d RU", m_pOthersInvOwner->get_money());
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
		pTrade->TransferItem	((PIItem)itm->m_pData, bBuying);
		pBuyList->SetItem		(itm);
	}

	pTrade->pThis.inv_owner->set_money ( pTrade->pThis.inv_owner->get_money(), true );
	pTrade->pPartner.inv_owner->set_money( pTrade->pPartner.inv_owner->get_money(), true );
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

void CUITradeWnd::FillList	(TIItemContainer& cont, CUIDragDropListEx& dragDropList, bool do_colorize)
{
	TIItemContainer::iterator it	= cont.begin();
	TIItemContainer::iterator it_e	= cont.end();

	for(; it != it_e; ++it) 
	{
		CUICellItem* itm			= create_cell_item	(*it);
		if(do_colorize)				ColorizeItem		(itm, CanMoveToOther(*it));
		dragDropList.SetItem		(itm);
	}

}

bool CUITradeWnd::OnItemStartDrag(CUICellItem* itm)
{
	return false; //default behaviour
}

bool CUITradeWnd::OnItemSelected(CUICellItem* itm)
{
	SetCurrentItem		(itm);
	return				false;
}

bool CUITradeWnd::OnItemRButtonClick(CUICellItem* itm)
{
	SetCurrentItem				(itm);
	return						false;
}


bool CUITradeWnd::OnItemDrop(CUICellItem* itm)
{
	CUIDragDropListEx*	old_owner		= itm->OwnerList();
	CUIDragDropListEx*	new_owner		= CUIDragDropListEx::m_drag_item->BackList();
	if(old_owner==new_owner || !old_owner || !new_owner)
					return false;

	if(old_owner==&m_uidata->UIOurBagList && new_owner==&m_uidata->UIOurTradeList)
		ToOurTrade				();
	else if(old_owner==&m_uidata->UIOurTradeList && new_owner==&m_uidata->UIOurBagList)
		ToOurBag				();
	else if(old_owner==&m_uidata->UIOthersBagList && new_owner==&m_uidata->UIOthersTradeList)
		ToOthersTrade			();
	else if(old_owner==&m_uidata->UIOthersTradeList && new_owner==&m_uidata->UIOthersBagList)
		ToOthersBag				();

	return true;
}

bool CUITradeWnd::OnItemDbClick(CUICellItem* itm)
{
	SetCurrentItem						(itm);
	CUIDragDropListEx*	old_owner		= itm->OwnerList();
	
	if(old_owner == &m_uidata->UIOurBagList)
		ToOurTrade				();
	else if(old_owner == &m_uidata->UIOurTradeList)
		ToOurBag				();
	else if(old_owner == &m_uidata->UIOthersBagList)
		ToOthersTrade			();
	else if(old_owner == &m_uidata->UIOthersTradeList)
		ToOthersBag				();
	else
		R_ASSERT2(false, "wrong parent for cell item");

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

	CUIDragDropListEx* owner	= itm->OwnerList();
	bool bBuying				= (owner==&m_uidata->UIOurBagList) || (owner==&m_uidata->UIOurTradeList);

	if(itm && m_uidata->UIItemInfo.UICost){

		string256			str;

		sprintf_s				(str, "%d RU", m_pOthersTrade->GetItemPrice(CurrentIItem(), bBuying) );
		m_uidata->UIItemInfo.UICost->SetText (str);
	}
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

void CUITradeWnd::ColorizeItem(CUICellItem* itm, bool b)
{
/*	if(!b)
		itm->SetColor				(color_rgba(255,100,100,255));*/
	PIItem iitem = (PIItem)itm->m_pData;
	if (!b)
		itm->SetTextureColor(color_rgba(255, 100, 100, 255));
	else if (iitem->m_eItemPlace == eItemPlaceSlot || iitem->m_eItemPlace == eItemPlaceBelt)
		itm->SetTextureColor(color_rgba(100, 255, 100, 255));
}

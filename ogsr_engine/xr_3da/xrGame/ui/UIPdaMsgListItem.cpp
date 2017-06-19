//////////////////////////////////////////////////////////////////////
// UIPdaMsgListItem.cpp: элемент окна списка в основном 
// экране для сообщений PDA
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIPdaMsgListItem.h"
#include "../Entity.h"
#include "../character_info.h"
#include "UIInventoryUtilities.h"
#include "xrUIXmlParser.h"
#include "UIXmlInit.h"
#include "uicoloranimatorwrapper.h"
#include "../object_broker.h"

#define PDA_MSG_MAINGAME_CHAR "maingame_pda_msg.xml"

using namespace InventoryUtilities;


void CUIPdaMsgListItem::SetFont(CGameFont* pFont){
	UIMsgText.SetFont(pFont);
}

void CUIPdaMsgListItem::Init(float x, float y, float width, float height)
{
	CUIStatic::Init			(x, y, width, height);

	CUIXml uiXml;
	bool xml_result			= uiXml.Init(CONFIG_PATH, UI_PATH,PDA_MSG_MAINGAME_CHAR);
	R_ASSERT2				(xml_result, "xml file not found");

	CUIXmlInit				xml_init;
	AttachChild				(&UIIcon);
	xml_init.InitStatic		(uiXml, "icon_static", 0, &UIIcon);

	AttachChild(&UIName);
	if(uiXml.NavigateToNode	("name_static",0))
		xml_init.InitStatic	(uiXml, "name_static", 0, &UIName);
	else
	{
		UIName.Show			(false);
		UIName.Enable		(false);
	}
	AttachChild				(&UIMsgText);
	xml_init.InitStatic		(uiXml, "text_static", 0, &UIMsgText);	
}

void CUIPdaMsgListItem::SetTextColor(u32 color){
	UIMsgText.SetTextColor	(color);
}

void CUIPdaMsgListItem::SetColor(u32 color){
	UIIcon.SetColor(color);
}

void CUIPdaMsgListItem::InitCharacter(CInventoryOwner* pInvOwner)
{
	VERIFY(pInvOwner);

	string256 str;
	sprintf_s(str, "name: %s", pInvOwner->Name());
	UIName.SetText			(str);

	UIIcon.InitTexture		( pInvOwner->CharacterInfo().IconName().c_str() );
/*
	UIIcon.SetShader(GetCharIconsShader());
	UIIcon.GetUIStaticItem().SetOriginalRect(
					float(pInvOwner->CharacterInfo().TradeIconX()*ICON_GRID_WIDTH),
					float(pInvOwner->CharacterInfo().TradeIconY()*ICON_GRID_HEIGHT),
					float(pInvOwner->CharacterInfo().TradeIconX()+CHAR_ICON_WIDTH*ICON_GRID_WIDTH),
					float(pInvOwner->CharacterInfo().TradeIconY()+CHAR_ICON_HEIGHT*ICON_GRID_HEIGHT));
*/
}

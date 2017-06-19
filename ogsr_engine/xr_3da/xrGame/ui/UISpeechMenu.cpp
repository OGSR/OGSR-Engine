#include "StdAfx.h"
#include "UISpeechMenu.h"
#include "UIScrollView.h"
#include "UIStatic.h"
#include "UIXmlInit.h"
#include "../game_cl_mp.h"
#include <dinput.h>
#include "../level.h"
#include "../string_table.h"
#include "../HUDManager.h"

CUISpeechMenu::CUISpeechMenu(LPCSTR section_name){
	m_pList = xr_new<CUIScrollView>();AttachChild(m_pList);m_pList->SetAutoDelete(true);
	CUIXml xml_doc;
	xml_doc.Init(CONFIG_PATH, UI_PATH, "maingame.xml");
	CUIXmlInit::InitWindow(xml_doc, "speech_menu",0,this);
	CUIXmlInit::InitScrollView(xml_doc, "speech_menu",0,m_pList);
	m_pList->SetWndPos(0,0);
	m_text_color = 0xffffffff;
	SetFont(UI()->Font()->pFontLetterica18Russian);
	CUIXmlInit::InitFont(xml_doc,"speech_menu:text",0,m_text_color,m_pFont);	
    InitList(section_name);
}

CUISpeechMenu::~CUISpeechMenu()
{
	int x=0;
	x=x;
}

void CUISpeechMenu::Init(float x, float y, float width, float height){
	CUIWindow::Init(x,y,width,height);
}

void CUISpeechMenu::InitList(LPCSTR section_name){
	R_ASSERT2(pSettings->section_exist(section_name), section_name);
	CUIStatic* pItem = NULL;

	string64 phrase;
	string256 str;
	for (int i = 0; true; i++)
	{
		CStringTable st;
		sprintf_s(phrase,"phrase_%i",i);		
		if (pSettings->line_exist(section_name, phrase))
		{
			
            LPCSTR s = pSettings->r_string(section_name, phrase);
			_GetItem(s,0,phrase);
			sprintf_s(str, "%d. %s",i+1, *st.translate(phrase));

			ADD_TEXT_TO_VIEW3(str, pItem, m_pList);
			pItem->SetFont(GetFont());
			pItem->SetTextColor(m_text_color);
		}
		else
			break;
	}
}

bool CUISpeechMenu::OnKeyboard(int dik, EUIMessages keyboard_action){
    if (dik < DIK_1 || dik > DIK_0)
		return CUIDialogWnd::OnKeyboard(dik, keyboard_action);

	game_cl_mp* game = smart_cast<game_cl_mp*>(&Game());

	Game().StartStopMenu(this,true);
	game->OnMessageSelected(this, static_cast<u8>(dik - DIK_1));

	return true;
}
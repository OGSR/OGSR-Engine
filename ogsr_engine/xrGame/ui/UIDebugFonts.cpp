// File:		UIDebugFonts.cpp
// Description:	Output list of all fonts
// Created:		22.03.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua
//
// Copyright 2005 GSC Game World

#include "StdAfx.h"
#include "UIDebugFonts.h"
#include "dinput.h"
#include "../hudmanager.h"


CUIDebugFonts::CUIDebugFonts(){
	AttachChild			(&m_background);
	Init				(0, 0, UI_BASE_WIDTH, UI_BASE_HEIGHT);
}

CUIDebugFonts::~CUIDebugFonts(){

}

void CUIDebugFonts::Init(float x, float y, float width, float height){
	CUIDialogWnd::Init(x, y, width, height);

	FillUpList();

	m_background.Init(x, y, width, height);
	m_background.InitTexture("ui\\ui_debug_font");
}

bool CUIDebugFonts::OnKeyboard(int dik, EUIMessages keyboard_action){
	if (DIK_ESCAPE == dik)
		this->GetHolder()->StartStopMenu(this, true);

	if (DIK_F12 == dik)
		return false;

    return true;
}
#include "../string_table.h"

void CUIDebugFonts::FillUpList(){
	CFontManager::FONTS_VEC& v = UI()->Font()->m_all_fonts;
	CFontManager::FONTS_VEC_IT it	= v.begin();
	CFontManager::FONTS_VEC_IT it_e = v.end();
	Fvector2 pos, sz;
	pos.set			(0,0);
	sz.set			(UI_BASE_WIDTH,UI_BASE_HEIGHT);
	string256		str;
	for(;it!=it_e;++it){
		CGameFont* F			= *(*it);
		CUIStatic* pItem		= xr_new<CUIStatic>();
		pItem->SetWndPos		(pos);
		pItem->SetWndSize		(sz);
		sprintf_s					(str, "%s:%s", *F->m_font_name, *CStringTable().translate("Test_Font_String"));
		pItem->SetFont			(F);
		pItem->SetText			(str);
		pItem->SetTextComplexMode(false);
		pItem->SetVTextAlignment(valCenter);
		pItem->SetTextAlignment	(CGameFont::alCenter);
		pItem->AdjustHeightToText();
		pos.y					+= pItem->GetHeight()+20.0f;
		pItem->SetAutoDelete	(true);
		AttachChild				(pItem);
	}

}
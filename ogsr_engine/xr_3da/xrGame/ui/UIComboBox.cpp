// File:        UIComboBox.cpp
// Description: guess :)
// Created:     10.12.2004
// Author:      Serhiy O. Vynnychenko
// Mail:        narrator@gsc-game.kiev.ua
// 
// Copyright 2004 GSC Game World
//

#include "StdAfx.h"
#include "UIComboBox.h"
#include "UITextureMaster.h"
#include "UIScrollBar.h"

#define CB_HEIGHT 23.0f
#define BTN_SIZE  23.0f

CUIComboBox::CUIComboBox()
{
	AttachChild			(&m_frameLine);
	AttachChild			(&m_text);

//.	AttachChild			(&m_btn);

	AttachChild			(&m_frameWnd);
	AttachChild			(&m_list);

	m_iListHeight		= 0;
	m_bInited			= false;
	m_eState			= LIST_FONDED;

	m_textColor[0]		= 0xff00ff00;
}

CUIComboBox::~CUIComboBox()
{}

void CUIComboBox::SetListLength(int length){
//	R_ASSERT(0 == m_iListHeight);
	m_iListHeight = length;
}

void CUIComboBox::Init(float x, float y, float width){
	m_bInited = true;
	if (0 == m_iListHeight)
		m_iListHeight = 4;

//.	width								-= BTN_SIZE;

	CUIWindow::Init						(x, y, width, CB_HEIGHT);
	// Frame Line
	m_frameLine.Init					(0, 0, width, CB_HEIGHT);
	m_frameLine.InitEnabledState		("ui_cb_linetext_e"); // horizontal by default
	m_frameLine.InitHighlightedState	("ui_cb_linetext_h");


	// Edit Box on left side of frame line
	m_text.Init							(0, 0, width, CB_HEIGHT); 
	m_text.SetTextColor					(m_textColor[0]);
	m_text.Enable						(false);
	// Button on right side of frame line
//.	m_btn.Init							("ui_cb_button", width, 0, BTN_SIZE, BTN_SIZE);


	// height of list equal to height of ONE element
	float item_height					= CUITextureMaster::GetTextureHeight("ui_cb_listline_b");
	m_list.Init							(0, CB_HEIGHT, width, item_height*m_iListHeight);
	m_list.Init							();
	m_list.SetTextColor					(m_textColor[0]);
	m_list.SetSelectionTexture			("ui_cb_listline");
	m_list.SetItemHeight				(CUITextureMaster::GetTextureHeight("ui_cb_listline_b"));
	// frame(texture) for list
	m_frameWnd.Init						(0,  CB_HEIGHT, width, m_list.GetItemHeight()*m_iListHeight);
	m_frameWnd.InitTexture				("ui_cb_listbox");

	m_list.Show							(false);
	m_frameWnd.Show						(false);
}

void CUIComboBox::Init(float x, float y, float width, float height)
{
	this->Init		(x, y, width);
}

#include "uilistboxitem.h"
CUIListBoxItem* CUIComboBox::AddItem_(LPCSTR str, int _data)
{
    R_ASSERT2			(m_bInited, "Can't add item to ComboBox before Initialization");
	CUIListBoxItem* itm = m_list.AddItem(str);
	itm->SetData		((void*)(__int64)_data);
	return				itm;
}
CUIListBoxItem* CUIComboBox::AddItem_(LPCSTR str)
{
	R_ASSERT2(m_bInited, "Can't add item to ComboBox before Initialization");
	CUIListBoxItem* itm = m_list.AddItem(str);
	return				itm;
}

void CUIComboBox::OnListItemSelect()
{
	m_text.SetText			(m_list.GetSelectedText());    
	CUIListBoxItem* itm		= m_list.GetSelectedItem();
	
	int bk_itoken_id		= m_itoken_id;
	
	m_itoken_id				= (int)(__int64)itm->GetData();
	ShowList				(false);

	if(bk_itoken_id!=m_itoken_id)
	{
		SaveValue		();
		GetMessageTarget()->SendMessage(this, LIST_ITEM_SELECT, NULL);
	}
}

#include "../string_table.h"
void CUIComboBox::SetCurrentValue()
{
	m_list.Clear		();
	xr_token* tok		= GetOptToken();

	while (tok->name)
	{		
		AddItem_(tok->name, tok->id);
		tok++;
	}

	LPCSTR cur_val		= *CStringTable().translate( GetOptTokenValue());
	m_text.SetText		( cur_val );
	m_list.SetSelectedText( cur_val );
	
	CUIListBoxItem* itm	= m_list.GetSelectedItem();
	if(itm)
		m_itoken_id			= (int)(__int64)itm->GetData();
	else
		m_itoken_id			= 1; //first
}

void CUIComboBox::SaveValue()
{
	CUIOptionsItem::SaveValue	();
	xr_token* tok				= GetOptToken();
	LPCSTR	cur_val				= get_token_name(tok, m_itoken_id);
	SaveOptTokenValue			(cur_val);
}

bool CUIComboBox::IsChanged()
{
	return				(m_backup_itoken_id != m_itoken_id);
/*
	xr_token* tok		= GetOptToken();
	LPCSTR	cur_val		= get_token_name(tok, m_itoken_id);

	bool bChanged		= (0 != xr_strcmp(GetOptTokenValue(), cur_val));

	return				bChanged;
*/
}

LPCSTR CUIComboBox::GetText()
{
	return m_text.GetText	();
}

void CUIComboBox::SetItem(int idx)
{
	m_list.SetSelectedIDX	(idx);
	CUIListBoxItem* itm		= m_list.GetSelectedItem();
	m_itoken_id				= (int)(__int64)itm->GetData();

	m_text.SetText			(m_list.GetSelectedText());
	
}
void CUIComboBox::OnBtnClicked()
{
	ShowList				(!m_list.IsShown());
}

void CUIComboBox::ShowList(bool bShow)
{
    if (bShow)
	{
		SetHeight			(m_text.GetHeight() + m_list.GetHeight());

		m_list.Show			(true);
		m_frameWnd.Show		(true);

		m_eState			= LIST_EXPANDED;

		GetParent()->SetCapture(this, true);
	}
	else
	{
		m_list.Show			(false);
		m_frameWnd.Show		(false);
		SetHeight			(m_frameLine.GetHeight());
		GetParent()->SetCapture(this, false);

		m_eState			= LIST_FONDED;
	}
}

CUIListBox* CUIComboBox::GetListWnd()
{
	return &m_list;
}
void CUIComboBox::Update()
{
	CUIWindow::Update	();
	if (!m_bIsEnabled)
	{
		SetState		(S_Disabled);
		m_text.SetTextColor(m_textColor[1]);
	}
	else
		m_text.SetTextColor(m_textColor[0]);

}

void CUIComboBox::OnFocusLost()
{
	CUIWindow::OnFocusLost();
	if (m_bIsEnabled)
        SetState(S_Enabled);

}

void CUIComboBox::OnFocusReceive()
{
	CUIWindow::OnFocusReceive();
    if (m_bIsEnabled)
        SetState(S_Highlighted);
}

bool CUIComboBox::OnMouse(float x, float y, EUIMessages mouse_action){
	if(CUIWindow::OnMouse(x, y, mouse_action)) 
		return true;

	bool bCursorOverScb = false;
//.	bCursorOverScb |= (0 <= x) && (GetWidth() >= x) && (0 <= y) && (GetHeight() >= y);

//.	Frect wndRect		= m_list.ScrollBar()->GetWndRect();
//.	bCursorOverScb		= wndRect.in(m_list.ScrollBar()->cursor_pos)
	bCursorOverScb		= m_list.ScrollBar()->CursorOverWindow();
	switch (m_eState){
		case LIST_EXPANDED:			

			if (  (!bCursorOverScb) &&  mouse_action == WINDOW_LBUTTON_DOWN)
			{
                ShowList(false);
				return true;
			}
			break;
		case LIST_FONDED:
			if(mouse_action==WINDOW_LBUTTON_DOWN)
			{
				OnBtnClicked();
				return true;			
			}break;
		default:
			break;
	}	
	 

        return false;
}

void CUIComboBox::SetState(UIState state)
{
	m_frameLine.SetState	(state);
}

void CUIComboBox::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	CUIWindow::SendMessage	(pWnd, msg, pData);

	switch (msg){
/*		case BUTTON_CLICKED:
			if (pWnd == &m_btn || pWnd == &m_list)
				OnBtnClicked();
			break;
*/
		case LIST_ITEM_CLICKED:
			if (pWnd == &m_list)
				OnListItemSelect();	
			break;
		default:
			break;
	}
}

void CUIComboBox::SeveBackUpValue()
{
	m_backup_itoken_id = m_itoken_id;
}

void CUIComboBox::Undo()
{
	SetItem				(m_backup_itoken_id);
	SaveValue			();
	SetCurrentValue		();
}


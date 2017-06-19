// File:        UIComboBox.h
// Description: guess :)
// Created:     10.12.2004
// Author:      Serhiy O. Vynnychenko
// Mail:        narrator@gsc-game.kiev.ua
// 
// Copyright 2004 GSC Game World
//

#pragma once
#include "UIEditBox.h"
//.#include "UI3tButton.h"
//#include "UIListWnd.h"
#include "UIListBox.h"
#include "UIListBoxItem.h"
#include "UIInteractiveBackground.h"
#include "UIOptionsItem.h"

class CUIListBoxItem;

class CUIComboBox : public CUIWindow, public CUIOptionsItem {
	friend class CUIXmlInit;
	typedef enum{
		LIST_EXPANDED, 
		LIST_FONDED    
	} E_COMBO_STATE;

public:
						CUIComboBox				();
	virtual				~CUIComboBox			();
	// CUIOptionsItem
	virtual void		SetCurrentValue			();
	virtual void		SaveValue				();
	virtual bool		IsChanged				();
	virtual void 		SeveBackUpValue			();
	virtual void 		Undo					();

			LPCSTR		GetText					();

	// methods
	CUIListBox*			GetListWnd				();
			void		SetListLength			(int length);
			void		SetVertScroll			(bool bVScroll = true){m_list.SetFixedScrollBar(bVScroll);};
//.	virtual void		AddItem					(LPCSTR str, bool bSelected);
	CUIListBoxItem*		AddItem_				(LPCSTR str, int _data);
	CUIListBoxItem*		AddItem_				(LPCSTR str);
	virtual void		Init					(float x, float y, float width);
			void		SetItem					(int i);

	virtual void		Init					(float x, float y, float width, float height);	
	virtual void		SendMessage				(CUIWindow *pWnd, s16 msg, void* pData = 0);
	virtual void		OnFocusLost				();
	virtual void		OnFocusReceive			();
			int			CurrentID				()	{return m_itoken_id;}
protected:
	virtual void		SetState				(UIState state);	
	virtual bool		OnMouse					(float x, float y, EUIMessages mouse_action);
	virtual void		OnBtnClicked			();
			void		ShowList				(bool bShow);
			void		OnListItemSelect		();
	virtual void		Update();

protected:
	bool				m_bInited;
	int					m_iListHeight;
	int					m_itoken_id;
	E_COMBO_STATE		m_eState;
	int					m_backup_itoken_id;

	CUI_IB_FrameLineWnd	m_frameLine;
	CUILabel			m_text;
	CUIFrameWindow		m_frameWnd;

	u32					m_textColor[2];
public:
	CUIListBox			m_list;
	void				SetTextColor			(u32 color)			{m_textColor[0] = color;};
	void				SetTextColorD			(u32 color)			{m_textColor[1] = color;};

protected:	
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUIComboBox)
#undef script_type_list
#define script_type_list save_type_list(CUIComboBox)
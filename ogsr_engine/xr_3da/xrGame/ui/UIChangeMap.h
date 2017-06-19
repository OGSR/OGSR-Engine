#pragma once

#include "UIDialogWnd.h"

class CUIStatic;
class CUI3tButton;
class CUIFrameWindow;
class CUIListBox;
class CUIXml;
class CExtraContentFilter;

class CUIChangeMap : public CUIDialogWnd 
{
public:
	using CUIDialogWnd::Init;

					CUIChangeMap			();
					~CUIChangeMap			();
			void	Init					(CUIXml& xml_doc);

	virtual bool	OnKeyboard				(int dik, EUIMessages keyboard_action);
	virtual void	SendMessage				(CUIWindow* pWnd, s16 msg, void* pData = 0);

	void 			OnBtnOk					();
	void 			OnBtnCancel				();
	void 			OnItemSelect			();

protected:
			void	FillUpList				();

	CUIStatic*		bkgrnd;
	CUIStatic*		header;
	CUIStatic*		map_pic;
	CUIStatic*		map_frame;
	CUIFrameWindow* frame;
	CUIFrameWindow* lst_back;
	CUIListBox*		lst;

	CUI3tButton*	btn_ok;
	CUI3tButton*	btn_cancel;

	CExtraContentFilter*	m_pExtraContentFilter;

	u32						m_prev_upd_time;
};
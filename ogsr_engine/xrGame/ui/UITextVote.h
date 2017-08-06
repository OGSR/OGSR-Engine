#pragma once

#include "UIDialogWnd.h"

class CUIStatic;
class CUI3tButton;
class CUIEditBoxEx;
class CUIXml;

class CUITextVote : public CUIDialogWnd 
{
public:
	using CUIDialogWnd::Init;

					CUITextVote		();

			void	Init			(CUIXml& xml_doc);

	virtual void	SendMessage		(CUIWindow* pWnd, s16 msg, void* pData = 0);

	void 			OnBtnOk			();
	void 			OnBtnCancel		();

protected:
	CUIStatic*		bkgrnd;
	CUIStatic*		header;
	CUIEditBoxEx*	edit;

	CUI3tButton*	btn_ok;
	CUI3tButton*	btn_cancel;
};
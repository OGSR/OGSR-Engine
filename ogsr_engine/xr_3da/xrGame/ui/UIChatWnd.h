#pragma once
#include "UIDialogWnd.h"
#include "UIEditBox.h"
#include "xrUIXmlParser.h"

class CUIGameLog;
class game_cl_GameState;

class CUIChatWnd: public CUIDialogWnd
{
	typedef CUIDialogWnd inherited;

public:
						CUIChatWnd			(CUIGameLog *pList);
	virtual				~CUIChatWnd			();
	virtual void		Show				();
	virtual void		Hide				();
	virtual void		SetKeyboardCapture	(CUIWindow* pChildWindow, bool capture_status);
	virtual bool		NeedCursor			() {return false;}
	void				Init				(CUIXml& uiXml);
	void				SetEditBoxPrefix	(const shared_str &prefix);
	void				TeamChat			() { sendNextMessageToTeam = true; }
	void				AllChat				() { sendNextMessageToTeam = false; }
	void				SetOwner			(game_cl_GameState *pO) { pOwner = pO; }
	virtual bool		NeedCursor			()const {return false;}

	CUIEditBox			UIEditBox;

protected:
	CUIGameLog			*pUILogList;
	CUIStatic			UIPrefix;
	bool				sendNextMessageToTeam;
	game_cl_GameState	*pOwner;
};

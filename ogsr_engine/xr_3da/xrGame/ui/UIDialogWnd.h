// UIDialogWnd.h: класс простого диалога, нужен для стандартного запуска
// разным менюшек путем вызова виртуальных Show() И Hide()
//////////////////////////////////////////////////////////////////////

#ifndef _UI_DIALOG_WND_H_
#define _UI_DIALOG_WND_H_

#pragma once

#include "uiwindow.h"
class CDialogHolder;

class CUIDialogWnd : public CUIWindow  
{
private:
	typedef CUIWindow inherited;
	CDialogHolder*	m_pHolder;
protected:
	bool			IR_process					();
public:
	bool										m_bWorkInPause;
	CUIDialogWnd								();
	virtual ~ CUIDialogWnd						();

	virtual void Show							();
	virtual void Hide							();
	virtual void Update							();

	virtual bool IR_OnKeyboardPress				(int dik);
	virtual bool IR_OnKeyboardRelease			(int dik);
	virtual bool IR_OnMouseMove					(int dx, int dy);
	virtual bool IR_OnMouseWheel				(int direction);
	virtual bool IR_OnKeyboardHold				(int dik);
	virtual bool OnKeyboard						(int dik, EUIMessages keyboard_action);
	virtual bool OnKeyboardHold					(int dik);

	CDialogHolder* GetHolder					()					{return m_pHolder;};
			void SetHolder						(CDialogHolder* h)	{m_pHolder = h;};
	virtual bool StopAnyMove					(){return true;}
	virtual bool NeedCursor						()const {return true;}
	virtual bool WorkInPause					()const {return m_bWorkInPause;}
	virtual bool Dispatch						(int cmd, int param)				{return true;}
};

#endif // _UI_DIALOG_WND_H_

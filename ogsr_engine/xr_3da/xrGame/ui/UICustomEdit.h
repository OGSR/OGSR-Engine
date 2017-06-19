#pragma once

#include "UILines.h"
#include "UIWindow.h"

enum ELang
{
	eEng = 0,
	eRus,
	eFra,
};
struct Lt
{
	CHAR plain;
	CHAR capital;
	Lt() {};
	Lt(CHAR _capital, CHAR _plain) :plain(_plain), capital(_capital) {};
};
struct SLetter
{
	Lt US, NOT_US;
	SLetter() {};
	SLetter(Lt _US, Lt _NOT_US) :US(_US), NOT_US(_NOT_US) {};
	CHAR GetChar(bool _alt_lang, bool _shift)
	{
		if (_alt_lang == false)
			return _shift ? US.capital : US.plain;
		else
			return _shift ? NOT_US.capital : NOT_US.plain;
	}
};

class CUICustomEdit : public CUIWindow, public CUILinesOwner {
	u32				m_max_symb_count;
public:
	CUICustomEdit();
	virtual ~CUICustomEdit();
	// CUILinesOwner
	virtual void			SetFont(CGameFont* pFont)					{CUILinesOwner::SetFont(pFont);}
	virtual CGameFont*		GetFont()									{return CUILinesOwner::GetFont();}
	virtual void			SetTextColor(u32 color);
	virtual void			SetTextColorD(u32 color);

	virtual void	Init			(float x, float y, float width, float height);
	virtual void	SendMessage		(CUIWindow* pWnd, s16 msg, void* pData = NULL);

	virtual bool	OnMouse			(float x, float y, EUIMessages mouse_action);
	virtual bool	OnKeyboard		(int dik, EUIMessages keyboard_action);
	virtual void	OnFocusLost		();

	virtual void	Update			();
	virtual void	Draw			();

			void	CaptureFocus	(bool bCapture) { m_bInputFocus = bCapture; }
	virtual	void	SetText			(LPCSTR str);
	virtual LPCSTR	GetText();
			void	SetMaxCharCount	(u32 cnt)			{m_max_symb_count = cnt;}
	virtual void	Enable			(bool status);
			void	SetNumbersOnly	(bool status);
			void	SetFloatNumbers	(bool status);
			void	SetPasswordMode	(bool mode = true);			
			void	SetDbClickMode	(bool mode = true)	{m_bFocusByDbClick = mode;}
			void	SetCursorColor	(u32 color)			{m_lines.SetCursorColor(color);}
			
			void	SetLightAnim			(LPCSTR lanim);

protected:

	bool KeyPressed(int dik);
	bool KeyReleased(int dik);

/*	void AddLetter(char c);
	virtual void AddChar(char c);*/

	void AddLetter(SLetter _l);
	void AddLetterNumbers(CHAR c);
	virtual void AddChar(CHAR c);

	bool m_bInputFocus;
	bool m_bShift;

	bool m_bNumbersOnly;
	bool m_bFloatNumbers;
	bool m_bFocusByDbClick;

	u32 m_textColor[2];

	//DIK клавиши, кот. нажата и удерживается, 0 если такой нет
	int m_iKeyPressAndHold;
	bool m_bHoldWaitMode;

//	u32	m_cursorColor;

	CLAItem*				m_lanim;
};

extern bool g_alternate_lang;
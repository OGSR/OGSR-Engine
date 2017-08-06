// File:		UILines.h
// Description:	Multilines Text Control
// Created:		11.03.2005
// Author:		Serge Vynnycheko
// Mail:		narrator@gsc-game.kiev.ua
//
// Copyright 2005 GSC Game World

#pragma once

#include "UILine.h"
#include "../UI.h"
#include "uiabstract.h"

class CUILines : public IUITextControl,
				 public CUISimpleWindow,
				 public CDeviceResetNotifier {
	 friend class CUICustomEdit;
	 friend class CUICDkey;
public:
	CUILines();
	virtual ~CUILines();

	// IUITextControl methods
	virtual void			SetText(LPCSTR text);
	virtual LPCSTR			GetText();
	virtual void			SetTextColor(u32 color);
	virtual u32				GetTextColor()								{return m_dwTextColor;}
			u32&			GetTextColorRef()							{return m_dwTextColor;}
	virtual void			SetFont(CGameFont* pFont);
	virtual CGameFont*		GetFont()									{return m_pFont;}
	virtual void			SetTextAlignment(ETextAlignment al)			{m_eTextAlign = al;}
	virtual ETextAlignment	GetTextAlignment()							{return m_eTextAlign;}
			void			SetVTextAlignment(EVTextAlignment al)		{m_eVTextAlign = al;}
			EVTextAlignment GetVTextAlignment()							{return m_eVTextAlign;}

	// additional
			void			SetCursorColor								(u32 color)			{m_dwCursorColor = color;}
			void			AddCharAtCursor								(TCHAR ch);
			void			DelChar										();
			void			DelLeftChar									();
			void			MoveCursorToEnd								();

			void			SetTextComplexMode							(bool mode = true);
			bool			GetTextComplexMode							() const;
			void			SetPasswordMode								(bool mode = true);
			void			SetColoringMode								(bool mode);
			void			SetCutWordsMode								(bool mode);
			void			SetUseNewLineMode							(bool mode);

    // IUISimpleWindow methods
	virtual void			Init										(float x, float y, float width, float height);
	virtual void			Draw										();
	virtual void			Draw										(float x, float y);
	virtual void			Update										();
IC			void			SetWndSize_inline							(const Fvector2& wnd_size);


    // CDeviceResetNotifier methods
	virtual void			OnDeviceReset								();

	// own methods
			void			Reset										();
			void			ParseText									();
			float			GetVisibleHeight							();

	// cursor control
			int				m_iCursorPos;
			void			IncCursorPos								();
			void			DecCursorPos								();
protected:
			Ivector2		m_cursor_pos;
			void			UpdateCursor								();
				// %c[255,255,255,255]
		u32					GetColorFromText							(const xr_string& str)							const;
		float				GetIndentByAlign							()												const;
		float				GetVIndentByAlign							();
		void				CutFirstColoredTextEntry					(xr_string& entry, u32& color,xr_string& text)	const;
	CUILine*				ParseTextToColoredLine						(const xr_string& str);

	// IUITextControl data
	typedef xr_string						Text;
	typedef xr_vector<CUILine>				LinesVector;
	typedef LinesVector::iterator			LinesVector_it;
	LinesVector				m_lines;	// parsed text
	float					m_interval; // interval

	Text					m_text;

	ETextAlignment			m_eTextAlign;
	EVTextAlignment			m_eVTextAlign;
	u32						m_dwTextColor;
	u32						m_dwCursorColor;

	CGameFont*				m_pFont;

	enum {
		flNeedReparse		= (1<<0),
		flComplexMode		= (1<<1),
		flPasswordMode		= (1<<2),
		flColoringMode		= (1<<3),
		flCutWordsMode		= (1<<4),
		flRecognizeNewLine	= (1<<5)
	};	
private:
	Flags8					uFlags;
	float					m_oldWidth;
};

class CUILinesOwner : public IUITextControl {
public:
	virtual					~CUILinesOwner() {}

	// IUIFontControl{
	virtual void			SetTextColor					(u32 color)						{m_lines.SetTextColor(color);}
	virtual u32				GetTextColor					()								{return m_lines.GetTextColor();}
	virtual void			SetFont							(CGameFont* pFont)				{m_lines.SetFont(pFont);}
	virtual CGameFont*		GetFont							()								{return m_lines.GetFont();}
	virtual void			SetTextAlignment				(ETextAlignment alignment)		{m_lines.SetTextAlignment(alignment);}
	virtual ETextAlignment	GetTextAlignment				()								{return m_lines.GetTextAlignment();}

	// IUITextControl : public IUIFontControl{
	virtual void			SetText							(LPCSTR text)					{m_lines.SetText(text);}
	virtual LPCSTR			GetText							()								{return m_lines.GetText();}

	// own
	virtual void			SetTextPosX						(float x)						{m_textPos.x = x;}
	virtual void			SetTextPosY						(float y)						{m_textPos.y = y;}

protected:
	Fvector2				m_textPos;
	CUILines				m_lines;
};

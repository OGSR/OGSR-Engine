#pragma once

#include "UIFrameLineWnd.h"
#include "UILines.h"
class CLAItem;

class CUILabel : public CUIFrameLineWnd, public CUILinesOwner //IUITextControl 
{
	CLAItem*				m_lanim;
	float					m_lainm_start_time;

public:
	//IUISimpleWindow
	virtual void SetWidth(float width);
	virtual void SetHeight(float height);
	virtual void SetFont(CGameFont* pFont);
    // CUIFrameLineWnd
	virtual void			Init(float x, float y, float width, float height);
	virtual void			Draw();
	virtual void			Update			();

	// own
	CUILabel();
			void			SetLightAnim			(LPCSTR lanim);

protected:
//	Fvector2 m_textPos;
};
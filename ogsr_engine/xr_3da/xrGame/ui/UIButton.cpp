// UIButton.cpp: класс нажимаемой кнопки
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIButton.h"
#include "../HUDManager.h"
#include "UILines.h"
#include "UIBtnHint.h"

#define PUSH_OFFSET_RIGHT 1
#define PUSH_OFFSET_DOWN  1



CUIButton:: CUIButton()
{
	m_eButtonState				= BUTTON_NORMAL;
	m_ePressMode				= NORMAL_PRESS;
	m_bButtonClicked			= false;
	m_bAvailableTexture			= false;
	m_bIsSwitch					= false;

	m_PushOffset.set			(PUSH_OFFSET_RIGHT, PUSH_OFFSET_DOWN);

	m_HighlightColor			= 0xFFFFFFFF;
	m_uAccelerator[0]				= 0;
	m_uAccelerator[1]				= 0;

	m_bEnableTextHighlighting	= true;

	m_ShadowOffset.set			(0.0f,0.0f);

	SetTextComplexMode			(false);
	SetTextAlignment			(CGameFont::alCenter); // this will create class instance for m_pLines
	SetVTextAlignment			(valCenter);
	m_bClickable				= true;
}

 CUIButton::~ CUIButton()
{
}

void CUIButton::Reset()
{
	m_eButtonState				= BUTTON_NORMAL;
	m_bButtonClicked			= false;
	m_bCursorOverWindow			= false;
	inherited::Reset			();
}



void CUIButton::Enable(bool status){
	CUIStatic::Enable			(status);

	if (!status)
		m_bCursorOverWindow		= false;
}

bool  CUIButton::OnMouse(float x, float y, EUIMessages mouse_action)
{
	m_bButtonClicked			= false;

	if( inherited::OnMouse(x, y, mouse_action) ) return true;

	if ( (	WINDOW_LBUTTON_DOWN==mouse_action	||
			WINDOW_LBUTTON_UP==mouse_action		||
			WINDOW_RBUTTON_DOWN==mouse_action	||
			WINDOW_RBUTTON_UP==mouse_action)	&& 
			HasChildMouseHandler())
		return false;

	switch (m_ePressMode)
	{
	case NORMAL_PRESS:
		if(m_eButtonState == BUTTON_NORMAL)
		{
			if(mouse_action == WINDOW_LBUTTON_DOWN || mouse_action == WINDOW_LBUTTON_DB_CLICK)
			{
				m_eButtonState = BUTTON_PUSHED;
				GetMessageTarget()->SendMessage(this, BUTTON_DOWN, NULL);
			}
		}
		else if(m_eButtonState == BUTTON_PUSHED)
		{
			if(mouse_action == WINDOW_LBUTTON_UP)
			{
				if(m_bCursorOverWindow)
					OnClick();
			
				if (!m_bIsSwitch)
					m_eButtonState = BUTTON_NORMAL;
			}
			else if(mouse_action == WINDOW_MOUSE_MOVE)
			{
				if(!m_bCursorOverWindow && !m_bIsSwitch)
					m_eButtonState = BUTTON_UP;
			}
		}
		else if(m_eButtonState == BUTTON_UP)
		{
			if(mouse_action == WINDOW_MOUSE_MOVE)
			{
				if(m_bCursorOverWindow)
					m_eButtonState = BUTTON_PUSHED;
			}
			else if(mouse_action == WINDOW_LBUTTON_UP)
			{
				m_eButtonState = BUTTON_NORMAL;
			}
		}
		break;

	case DOWN_PRESS:
        if(mouse_action == WINDOW_MOUSE_MOVE)
		{
			if(m_bCursorOverWindow)
			{
				m_eButtonState = BUTTON_PUSHED;
			}
			else
			{
				m_eButtonState = BUTTON_NORMAL;
			}
		}
		else if(mouse_action == WINDOW_LBUTTON_DOWN || mouse_action == WINDOW_LBUTTON_DB_CLICK)
			if(m_bCursorOverWindow){
				OnClick();
				return true;
			}

		break;
	}
	return false;
}

void CUIButton::OnClick(){
	GetMessageTarget()->SendMessage(this, BUTTON_CLICKED);
	m_bButtonClicked = true;
}

void CUIButton::DrawTexture()
{
	Frect rect; 
	GetAbsoluteRect		(rect);

	if(m_bAvailableTexture && m_bTextureEnable)
	{
		if(m_eButtonState == BUTTON_UP || m_eButtonState == BUTTON_NORMAL)
			m_UIStaticItem.SetPos(rect.left + m_TextureOffset.x, rect.top + m_TextureOffset.y);
		else
			m_UIStaticItem.SetPos(rect.left + m_PushOffset.x + m_TextureOffset.x, rect.top + m_PushOffset.y + m_TextureOffset.y);

		if(m_bStretchTexture)
			m_UIStaticItem.SetRect(0, 0, rect.width(), rect.height());
		else
		{
			Frect r={0,0,
				m_UIStaticItem.GetOriginalRectScaled().width(),
				m_UIStaticItem.GetOriginalRectScaled().height()};
			m_UIStaticItem.SetRect(r);
		}

		if( Heading() )
			m_UIStaticItem.Render( GetHeading() );
		else
			m_UIStaticItem.Render();		
	}
}

void CUIButton::DrawHighlightedText(){
	float right_offset;
	float down_offset;

	if(m_eButtonState == BUTTON_UP || m_eButtonState == BUTTON_NORMAL)
	{
		right_offset = 0.0f;
		down_offset = 0.0f;
	}
	else
	{
		right_offset	= m_PushOffset.x;
		down_offset		= m_PushOffset.y;
	}

	Frect					rect;
	GetAbsoluteRect			(rect);
	u32 def_col = m_pLines->GetTextColor();
	m_pLines->SetTextColor(m_HighlightColor);

	m_pLines->Draw(	rect.left + right_offset + 0 +m_TextOffset.x + m_ShadowOffset.x, 
					rect.top + down_offset   - 0 +m_TextOffset.y + m_ShadowOffset.y);

	m_pLines->SetTextColor(def_col);

}

void CUIButton::DrawText()
{
	float right_offset;
	float down_offset;

	if(m_eButtonState == BUTTON_UP || m_eButtonState == BUTTON_NORMAL)
	{
		right_offset	= 0;
		down_offset		= 0;
	}
	else
	{
		right_offset	= m_PushOffset.x;
		down_offset		= m_PushOffset.y;
	}

	CUIStatic::DrawText();
	if(g_btnHint->Owner()==this)
		g_btnHint->Draw_();
}


bool is_in2(const Frect& b1, const Frect& b2){
	return (b1.x1<b2.x1)&&(b1.x2>b2.x2)&&(b1.y1<b2.y1)&&(b1.y2>b2.y2);
}

void  CUIButton::Update()
{
	CUIStatic::Update();

	if(CursorOverWindow() && m_hint_text.size() && !g_btnHint->Owner() && Device.dwTimeGlobal>m_dwFocusReceiveTime+500)
	{
		g_btnHint->SetHintText	(this,*m_hint_text);

		Fvector2 c_pos			= GetUICursor()->GetCursorPosition();
		Frect vis_rect;
		vis_rect.set			(0,0,UI_BASE_WIDTH, UI_BASE_HEIGHT);

		//select appropriate position
		Frect r;
		r.set					(0.0f, 0.0f, g_btnHint->GetWidth(), g_btnHint->GetHeight());
		r.add					(c_pos.x, c_pos.y);

		r.sub					(0.0f,r.height());
		if (false==is_in2(vis_rect,r))
			r.sub				(r.width(),0.0f);
		if (false==is_in2(vis_rect,r))
			r.add				(0.0f,r.height());

		if (false==is_in2(vis_rect,r))
			r.add				(r.width(), 45.0f);

		g_btnHint->SetWndPos(r.lt);
	}
}

void CUIButton::OnFocusLost()
{
	inherited::OnFocusLost();
	if(g_btnHint->Owner()==this)
		g_btnHint->Discard	();
}

bool CUIButton::OnKeyboard(int dik, EUIMessages keyboard_action)
{
	if (WINDOW_KEY_PRESSED == keyboard_action)
	{
		if(IsAccelerator(dik) )
		{
			OnClick		();
			return		true;
		}
	}
	return inherited::OnKeyboard(dik, keyboard_action);
}
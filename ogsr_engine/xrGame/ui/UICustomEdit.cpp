#include "stdafx.h"
#include "../../xr_3da/xr_input.h"
#include "UICustomEdit.h"
#include "../../xr_3da/LightAnimLibrary.h"

CUICustomEdit::CUICustomEdit()
{
	m_max_symb_count		= u32(-1);

	m_bInputFocus = false;

	m_iKeyPressAndHold = 0;
	m_bHoldWaitMode = false;
   
	m_lines.SetVTextAlignment(valCenter);
	m_lines.SetColoringMode(false);
	m_lines.SetCutWordsMode(true);
	m_lines.SetUseNewLineMode(false);
	SetText("");
	m_textPos.set(3,0);
	m_bNumbersOnly = false;
	m_bFloatNumbers = false;
	m_bFocusByDbClick = false;

	m_textColor[0]=color_argb(255,235,219,185);
	m_textColor[1]=color_argb(255,100,100,100);
}

void CUICustomEdit::SetTextColor(u32 color){
	m_textColor[0] = color;
}

void CUICustomEdit::SetTextColorD(u32 color){
	m_textColor[1] = color;
}

void CUICustomEdit::Init(float x, float y, float width, float height){
	CUIWindow::Init(x,y,width,height);
	m_lines.SetWidth(width - m_textPos.x);
	m_lines.SetHeight(height - m_textPos.y);
}

void CUICustomEdit::SetLightAnim(LPCSTR lanim)
{
	if(lanim&&xr_strlen(lanim))
		m_lanim	= LALib.FindItem(lanim);
	else
		m_lanim	= NULL;
}

void CUICustomEdit::SetPasswordMode(bool mode){
	m_lines.SetPasswordMode(mode);
}

void CUICustomEdit::OnFocusLost(){
	CUIWindow::OnFocusLost();
}

void CUICustomEdit::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
//	if(pWnd == GetParent())
//	{
		//кто-то другой захватил клавиатуру
		if(msg == WINDOW_KEYBOARD_CAPTURE_LOST)
		{
			m_bInputFocus = false;
			m_iKeyPressAndHold = 0;
		}
//	}
}


bool CUICustomEdit::OnMouse(float x, float y, EUIMessages mouse_action)
{
	if (m_bFocusByDbClick)
	{
		if(mouse_action == WINDOW_LBUTTON_DB_CLICK && !m_bInputFocus)
		{
			GetParent()->SetKeyboardCapture(this, true);
			m_bInputFocus = true;
			m_iKeyPressAndHold = 0;

			m_lines.MoveCursorToEnd();
		}
	}

	if(mouse_action == WINDOW_LBUTTON_DOWN && !m_bInputFocus)
	{
		GetParent()->SetKeyboardCapture(this, true);
		m_bInputFocus = true;
		m_iKeyPressAndHold = 0;

		m_lines.MoveCursorToEnd();
	}
	return false;
}

bool CUICustomEdit::OnKeyboardHold(int dik)
{
	return true;
}

bool CUICustomEdit::KeyReleased(int dik)
{
	return true;
}

bool CUICustomEdit::OnKeyboard(int dik, EUIMessages keyboard_action)
{
	if (!m_bInputFocus)
		return false;

	if (keyboard_action == WINDOW_KEY_PRESSED)
	{
		m_iKeyPressAndHold = dik;
		m_bHoldWaitMode = true;

		if (KeyPressed(dik))
			return true;
	}
	else if (keyboard_action == WINDOW_KEY_RELEASED)
	{
		if (m_iKeyPressAndHold == dik)
		{
			m_iKeyPressAndHold = 0;
			m_bHoldWaitMode = false;
		}
		if (KeyReleased(dik))
			return true;
	}
	return false;
}


bool CUICustomEdit::KeyPressed(int dik)
{
	char out_me = 0;
	bool bChanged = false;

	switch (dik)
	{
	case DIK_LEFT:
	case DIKEYBOARD_LEFT:
		m_lines.DecCursorPos();
		break;
	case DIK_RIGHT:
	case DIKEYBOARD_RIGHT:
		m_lines.IncCursorPos();
		break;
	case DIK_ESCAPE:
		if (strlen(GetText()))
		{
			SetText("");
			bChanged = true;
		}
		else
		{
			GetParent()->SetKeyboardCapture(this, false);
			m_bInputFocus = false;
			m_iKeyPressAndHold = 0;
		}
		break;
	case DIK_RETURN:
	case DIK_NUMPADENTER:
		GetParent()->SetKeyboardCapture(this, false);
		m_bInputFocus = false;
		m_iKeyPressAndHold = 0;
		GetMessageTarget()->SendMessage(this, EDIT_TEXT_COMMIT, NULL);
		break;
	case DIK_BACKSPACE:
		m_lines.DelLeftChar();
		bChanged = true;
		break;
	case DIK_DELETE:
	case DIKEYBOARD_DELETE:
		m_lines.DelChar();
		bChanged = true;
		break;
	default:
		out_me = pInput->DikToChar(dik);
	}

	if (out_me)
	{
		if (!m_bNumbersOnly || (out_me >= '0' && out_me <= '9') || (m_bFloatNumbers && out_me == '.' && !strchr(m_lines.GetText(), '.')))
		{
			AddChar(out_me);
			bChanged = true;
		}
	}

	if (bChanged)
		GetMessageTarget()->SendMessage(this, EDIT_TEXT_CHANGED, NULL);

	return true;
}

void CUICustomEdit::AddChar(char c)
{
	if(xr_strlen(m_lines.GetText()) >= m_max_symb_count)					return;

	float text_length	= m_lines.GetFont()->SizeOf_(m_lines.GetText()) + m_lines.GetFont()->SizeOf_(c) + m_textPos.x;
	UI()->ClientToScreenScaledWidth		(text_length);

	if (!m_lines.GetTextComplexMode() && (text_length > m_lines.GetWidth() - 1))	return;

	m_lines.AddCharAtCursor(c);
	m_lines.ParseText();
	if (m_lines.GetTextComplexMode())
		if (m_lines.GetVisibleHeight() > GetHeight())
			m_lines.DelLeftChar();
}


//время для обеспечивания печатания
//символа при удерживаемой кнопке
#define HOLD_WAIT_TIME 300
#define HOLD_REPEAT_TIME 50

void CUICustomEdit::Update()
{
	if(m_bInputFocus)
	{	
    static u32 last_time;

    u32 cur_time = GetTickCount();

		if(m_iKeyPressAndHold)
		{
			if(m_bHoldWaitMode)
			{
				if(cur_time - last_time>HOLD_WAIT_TIME)
				{
					m_bHoldWaitMode = false;
					last_time = cur_time;
				}
			}
			else
			{
				if(cur_time - last_time>HOLD_REPEAT_TIME)
				{
					last_time = cur_time;
					KeyPressed(m_iKeyPressAndHold);
				}
			}
		}
		else
			last_time = cur_time;
	}

	m_lines.SetTextColor(m_textColor[IsEnabled()?0:1]);

	CUIWindow::Update();
}

void  CUICustomEdit::Draw()
{
	CUIWindow::Draw			();
	Fvector2				pos;
	GetAbsolutePos			(pos);

	m_lines.m_bDrawCursor = m_bInputFocus;
	m_lines.Draw			(pos.x + m_textPos.x, pos.y + m_textPos.y);
}

void CUICustomEdit::SetText(const char* str)
{
	CUILinesOwner::SetText(str);
}

const char* CUICustomEdit::GetText() {
	return CUILinesOwner::GetText();
}

void CUICustomEdit::Enable(bool status){
	CUIWindow::Enable(status);
	if (!status)
		SendMessage(this,WINDOW_KEYBOARD_CAPTURE_LOST);
}

void CUICustomEdit::SetNumbersOnly(bool status){
	m_bNumbersOnly = status;
}

void CUICustomEdit::SetFloatNumbers(bool status){
	m_bFloatNumbers = status;
}

void CUICustomEdit::SetTextPosX(float x) {
	CUILinesOwner::SetTextPosX(x);
	m_lines.SetWidth(GetWidth() - m_textPos.x);
}

void CUICustomEdit::SetTextPosY(float y) {
	CUILinesOwner::SetTextPosY(y);
	m_lines.SetHeight(GetHeight() - m_textPos.y);
}

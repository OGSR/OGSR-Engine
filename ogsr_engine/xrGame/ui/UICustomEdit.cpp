#include "stdafx.h"
#include <dinput.h>
#include "../HUDManager.h"
#include "UICustomEdit.h"
#include "../../xr_3da/LightAnimLibrary.h"
#include "../string_table.h"

bool g_alternate_lang = false;

static constexpr u32 DILetters[] = { DIK_A, DIK_B, DIK_C, DIK_D, DIK_E, 
DIK_F, DIK_G, DIK_H, DIK_I, DIK_J, 
DIK_K, DIK_L, DIK_M, DIK_N, DIK_O, 
DIK_P, DIK_Q, DIK_R, DIK_S, DIK_T, 
DIK_U, DIK_V, DIK_W, DIK_X, DIK_Y, DIK_Z, 
DIK_LBRACKET, DIK_RBRACKET, DIK_SEMICOLON, DIK_APOSTROPHE,
DIK_BACKSLASH, DIK_SLASH, DIK_COMMA, DIK_PERIOD, DIK_GRAVE,
DIK_0, DIK_1, DIK_2, DIK_3, DIK_4, DIK_5, DIK_6, DIK_7,
DIK_8, DIK_9};

static constexpr u32 LETTERS_SIZE = std::size(DILetters);

static constexpr char EngLetters[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u',
'v', 'w', 'x', 'y', 'z', '[', ']', ';', '\'', '\\', '/', ',', '.', '`',
'0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

static constexpr char EngLettersCap[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U',
'V', 'W', 'X', 'Y', 'Z', '{', '}', ':', '"', '|', '?', '<', '>', '~',
')', '!', '@', '#', '$', '%', '^', '&', '*', '('};

static constexpr char RusLetters[] = { 'ф', 'и', 'с', 'в', 'у', 'а', 'п', 'р',
'ш', 'о', 'л', 'д', 'ь', 'т', 'щ', 'з', 'й', 'к', 'ы', 'е', 'г',
'м', 'ц', 'ч', 'н', 'я', 'х', 'ъ', 'ж', 'э', '\\', '.', 'б', 'ю', 'ё',
'0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

static constexpr char RusLettersCap[] = { 'Ф', 'И', 'С', 'В', 'У', 'А', 'П', 'Р',
'Ш', 'О', 'Л', 'Д', 'Ь', 'Т', 'Щ', 'З', 'Й', 'К', 'Ы', 'Е', 'Г',
'М', 'Ц', 'Ч', 'Н', 'Я', 'Х', 'Ъ', 'Ж', 'Э', '/', '\'', 'Б', 'Ю', 'Ё',
')', '!', '"', '№', ';', '%', ':', '?', '*', '(' };

static constexpr char FraLetters[] = { 'q', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
'i', 'j', 'k', 'l', ',', 'n', 'o', 'p', 'a', 'r', 's', 't', 'u',
'v', 'z', 'x', 'y', 'w', '^', '$', 'm', '<', '!', '/', ',', '.', '`',
'0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

static constexpr char FraLettersCap[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U',
'V', 'W', 'X', 'Y', 'Z', '{', '}', ':', '"', '|', '?', '<', '>', '~',
')', '!', '@', '#', '$', '%', '^', '&', '*', '(' };

static xr_map<u32, SLetter> gs_DIK2CHR;

CUICustomEdit::CUICustomEdit()
{
	m_max_symb_count		= u32(-1);

	shared_str lang = CStringTable().GetLanguage();

	for (u32 i = 0; i < LETTERS_SIZE; ++i)
	{
		gs_DIK2CHR[DILetters[i]] = SLetter(Lt(EngLettersCap[i], EngLetters[i]), (lang=="rus") ? Lt(RusLettersCap[i], RusLetters[i]) : Lt(FraLettersCap[i], FraLetters[i]));
	}

	m_bShift = false;
	m_bAlt = false;

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
	m_lines.SetWndSize(m_wndSize);
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


bool CUICustomEdit::OnKeyboard(int dik, EUIMessages keyboard_action)
{	
	if(!m_bInputFocus) 
		return false;
	if(keyboard_action == WINDOW_KEY_PRESSED)	
	{
		m_iKeyPressAndHold = dik;
		m_bHoldWaitMode = true;

		if(KeyPressed(dik))	return true;
	}
	else if(keyboard_action == WINDOW_KEY_RELEASED)	
	{
		if(m_iKeyPressAndHold == dik)
		{
			m_iKeyPressAndHold = 0;
			m_bHoldWaitMode = false;
		}
		if(KeyReleased(dik)) return true;
	}
	return false;
}

bool CUICustomEdit::OnKeyboardHold(int dik)
{
  return true;
}

bool CUICustomEdit::KeyPressed(int dik)
{
	char out_me = 0;
	bool bChanged = false;
	switch(dik)
	{
	case DIK_LEFT:
	case DIKEYBOARD_LEFT:
		m_lines.DecCursorPos();		
		break;
	case DIK_RIGHT:
	case DIKEYBOARD_RIGHT:
		m_lines.IncCursorPos();		
		break;
	case DIK_LSHIFT:
	case DIK_RSHIFT:
		m_bShift = true;
		if (m_bAlt)
			g_alternate_lang = !g_alternate_lang; //Переключили язык
		break;
	case DIK_ESCAPE:
		if (xr_strlen(GetText()) != 0)
		{
			SetText("");
			bChanged = true;
		}
		else
		{
			GetParent()->SetKeyboardCapture(this, false);
			m_bInputFocus = false;
			m_iKeyPressAndHold = 0;
		};
		break;
	case DIK_RETURN:
	case DIK_NUMPADENTER:
		GetParent()->SetKeyboardCapture(this, false);
		m_bInputFocus = false;
		m_iKeyPressAndHold = 0;
		GetMessageTarget()->SendMessage(this,EDIT_TEXT_COMMIT,NULL);
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
	case DIK_SPACE:
		out_me = ' ';					break;
	case DIK_MINUS:
		out_me = m_bShift ? '_' : '-';	break;
	case DIK_EQUALS:
		out_me = m_bShift ? '+' : '=';	break;
	case DIK_SUBTRACT:
		out_me = '-';	break;
	case DIK_ADD:
		out_me = '+';	break;
	case DIK_NUMPAD1: out_me = '1'; break;
	case DIK_NUMPAD2: out_me = '2'; break;
	case DIK_NUMPAD3: out_me = '3'; break;
	case DIK_NUMPAD4: out_me = '4'; break;
	case DIK_NUMPAD5: out_me = '5'; break;
	case DIK_NUMPAD6: out_me = '6'; break;
	case DIK_NUMPAD7: out_me = '7'; break;
	case DIK_NUMPAD8: out_me = '8'; break;
	case DIK_NUMPAD9: out_me = '9'; break;
	case DIK_NUMPAD0: out_me = '0'; break;
	case DIK_NUMPADSTAR: out_me = '*'; break;
	case DIK_NUMPADSLASH: out_me = '/'; break;
	case DIK_NUMPADPERIOD: out_me = '.'; break;
	case DIK_LALT:
	case DIK_RALT:
		m_bAlt = true;
		break;
	default:
		auto it = gs_DIK2CHR.find(dik);
		if (it != gs_DIK2CHR.end())
			out_me = (*it).second.GetChar(g_alternate_lang, m_bShift);
		break;
	}

	if (out_me)
		if (!m_bNumbersOnly || (out_me >= '0' && out_me <= '9') || (m_bFloatNumbers && out_me == '.' && !strstr(m_lines.GetText(), ".")))
		{
			AddChar(out_me);
			bChanged = true;
		}

	if(bChanged)
		GetMessageTarget()->SendMessage(this,EDIT_TEXT_CHANGED,NULL);

	return true;
}

bool CUICustomEdit::KeyReleased(int dik)
{
	switch(dik)
	{
	case DIK_LSHIFT:
	case DIK_RSHIFT:
		m_bShift = false;
		break;
	case DIK_LALT:
	case DIK_RALT:
		m_bAlt = false;
		break;
	}

	return true;
}



void CUICustomEdit::AddChar(CHAR c)
{
	if(xr_strlen(m_lines.GetText()) >= m_max_symb_count)					return;

	float text_length	= m_lines.GetFont()->SizeOf_(m_lines.GetText());
	UI()->ClientToScreenScaledWidth		(text_length);

	if (!m_lines.GetTextComplexMode() && (text_length > GetWidth() - 1))	return;

	m_lines.AddCharAtCursor(c);
	m_lines.ParseText();
	if (m_lines.GetTextComplexMode())
	{
		if (m_lines.GetVisibleHeight() > GetHeight())
			m_lines.DelLeftChar();
	}
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
	m_lines.Draw			(pos.x + m_textPos.x, pos.y + m_textPos.y);
	
	if(m_bInputFocus)
	{ //draw cursor here
		Fvector2							outXY;
		
		outXY.x								= 0.0f;
		float _h				= m_lines.m_pFont->CurrentHeight_();
		UI()->ClientToScreenScaledHeight(_h);
		outXY.y								= pos.y + (GetWndSize().y - _h)/2.0f;

		float								_w_tmp;
		int i								= m_lines.m_iCursorPos;
		string256							buff;
		strncpy								(buff,m_lines.m_text.c_str(),i);
		buff[i]								= 0;
		_w_tmp								= m_lines.m_pFont->SizeOf_(buff);
		UI()->ClientToScreenScaledWidth		(_w_tmp);
		outXY.x								= pos.x + _w_tmp;
		
		_w_tmp								= m_lines.m_pFont->SizeOf_("-");
		UI()->ClientToScreenScaledWidth		(_w_tmp);
		UI()->ClientToScreenScaled			(outXY);

		m_lines.m_pFont->Out				(outXY.x, outXY.y, "_");
	}
}

void CUICustomEdit::SetText(LPCTSTR str)
{
	CUILinesOwner::SetText(str);
}

LPCTSTR CUICustomEdit::GetText() {
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
//////////////////////////////////////////////////////////////////////
// UIRadioButton.cpp: класс кнопки, имеющей 2 состояния
// и работающей в группе с такими же кнопками
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\uiradiobutton.h"
#include "UILines.h"

CUIRadioButton::CUIRadioButton(void) { SetTextAlignment(CGameFont::alLeft); }

CUIRadioButton::~CUIRadioButton(void) {}

void CUIRadioButton::Init(float x, float y, float width, float height)
{
    CUI3tButton::Init(x, y, width, height);
    InitTexture("ui_radio");
}

void CUIRadioButton::InitTexture(LPCSTR tex_name)
{
    CUI3tButton::InitTexture(tex_name);
    Frect r = m_background.GetE()->GetStaticItem()->GetRect();
    CUI3tButton::SetTextX(r.width());
    CUI3tButton::Init(GetWndPos().x, GetWndPos().y, GetWidth(), r.height() - 5);
    m_pLines->Init(GetWndPos().x, GetWndPos().y, GetWidth(), m_background.GetE()->GetStaticItem()->GetRect().height());
}

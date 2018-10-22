// UICheckButton.cpp: класс кнопки, имеющей 2 состояния:
// с галочкой и без
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include ".\uicheckbutton.h"
#include "../HUDManager.h"
#include "UILines.h"

CUICheckButton::CUICheckButton(void)
{	
	SetTextAlignment(CGameFont::alLeft);
	m_bCheckMode = true;
	m_pDependControl = NULL;
}

CUICheckButton::~CUICheckButton(void)
{
}

void CUICheckButton::SetDependControl(CUIWindow* pWnd){
	m_pDependControl = pWnd;
}

void CUICheckButton::Update(){
	CUI3tButton::Update();

	if (m_pDependControl)
		m_pDependControl->Enable(GetCheck());
}


void CUICheckButton::SetCurrentValue(){
	SetCheck(GetOptBoolValue());
}

void CUICheckButton::SaveValue(){
	CUIOptionsItem::SaveValue();
	SaveOptBoolValue(GetCheck());
}

bool CUICheckButton::IsChanged(){
	return b_backup_val != GetCheck();
}

void CUICheckButton::SeveBackUpValue()
{
	b_backup_val = GetCheck();
}

void CUICheckButton::Undo()
{
	SetCheck(b_backup_val);
	SaveValue();
}

void CUICheckButton::Init(float x, float y, float width, float height){
	CUI3tButton::Init(x, y, width, height);
	InitTexture("ui_checker");
}

void CUICheckButton::InitTexture(LPCSTR tex_name) {
	CUI3tButton::InitTexture(tex_name);
	Frect r = m_background.GetE()->GetStaticItem()->GetOriginalRect();
	CUI3tButton::SetTextX(r.width());
	CUI3tButton::Init(GetWndPos().x, GetWndPos().y, GetWidth(), r.height() - 5);
	m_pLines->Init(GetWndPos().x, GetWndPos().y, GetWidth(), m_background.GetE()->GetStaticItem()->GetRect().height());
}
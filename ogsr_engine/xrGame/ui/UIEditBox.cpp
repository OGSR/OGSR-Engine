// CUIEditBox.cpp: ввод строки с клавиатуры
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <dinput.h>
#include "uieditbox.h"
#include "../HUDManager.h"
#include "UIColorAnimatorWrapper.h"


CUIEditBox::CUIEditBox()
{
	AttachChild(&m_frameLine);
	m_lines.SetTextComplexMode(false);
}

CUIEditBox::~CUIEditBox(void)
{
}	

void CUIEditBox::Init(float x, float y, float width, float height){
	m_frameLine.Init(0,0,width,height);
	CUICustomEdit::Init(x,y,width,height);
}

void CUIEditBox::InitTexture(const char* texture){
	m_frameLine.InitTexture(texture);
}

void CUIEditBox::SetCurrentValue(){
	SetText(GetOptStringValue());
}

void CUIEditBox::SaveValue(){
	CUIOptionsItem::SaveValue();
	SaveOptStringValue(GetText());
}

bool CUIEditBox::IsChanged(){
	return 0 != xr_strcmp(GetOptStringValue(),GetText());
}




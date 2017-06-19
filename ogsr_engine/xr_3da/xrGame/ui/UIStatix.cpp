#include "stdafx.h"

#include "UIStatix.h"

CUIStatix::CUIStatix(){
	m_bSelected		= false;
}

CUIStatix::~CUIStatix()
{}

void CUIStatix::start_anim()
{
	SetClrLightAnim	("ui_slow_blinking", true, true, true, true);
	ResetClrAnimation	();
}

void CUIStatix::stop_anim()
{
	SetClrLightAnim	(NULL, true, true, true, true);
}

void CUIStatix::Update(){

	SetColor(0xffffffff);

	if (m_bCursorOverWindow)
	{
		SetColor(0xff349F06);
	}

	if (!IsEnabled())
	{
		SetColor(0x80ffffff);
	};
	
	CUIStatic::Update();
}

void CUIStatix::OnFocusLost()
{
	CUIStatic::OnFocusLost	();
	SetColor				(0xffffffff);
	if (!IsEnabled())
	{
		SetColor(0x80ffffff);
	};
}

void CUIStatix::OnFocusReceive()
{
	CUIStatic::OnFocusReceive	();
	ResetClrAnimation				();
}

bool CUIStatix::OnMouseDown(int mouse_btn)
{
	GetMessageTarget()->SendMessage(this, BUTTON_CLICKED);
	return true;
}

void CUIStatix::SetSelectedState(bool state)
{
	bool b		= m_bSelected;
	m_bSelected = state;

	if(	b==m_bSelected )		return;
	
	if (!state)
		OnFocusLost();

	if(state)
		start_anim();
	else
		stop_anim();
}

bool CUIStatix::GetSelectedState(){
	return m_bSelected;
}

#include "StdAfx.h"

#include "UILabel.h"
#include "../../LightAnimLibrary.h"

CUILabel::CUILabel()
{
	m_textPos.set				(0,0);
	m_lines.SetVTextAlignment	(valCenter);
	m_lainm_start_time			= -1.0f;
	m_lanim						= NULL;	
}

void CUILabel::Init(float x, float y, float width, float height){
	CUIFrameLineWnd::Init(x,y,width,height);
	m_lines.Init(0,0,width, height);
}

void CUILabel::SetFont(CGameFont* pFont){
	CUILinesOwner::SetFont(pFont);
}

void CUILabel::Draw(){
	CUIFrameLineWnd::Draw();
	Fvector2 p;
	GetAbsolutePos(p);
	m_lines.Draw(p.x + m_textPos.x, p.y + m_textPos.y);
}

void CUILabel::SetWidth(float width){
	m_lines.SetWidth(width);
	CUIFrameLineWnd::SetWidth(width);
}

void CUILabel::SetHeight(float height){
	m_lines.SetHeight(height);
	CUIFrameLineWnd::SetHeight(height);
}

void CUILabel::SetLightAnim(LPCSTR lanim)
{
	if(lanim&&xr_strlen(lanim))
		m_lanim	= LALib.FindItem(lanim);
	else
		m_lanim	= NULL;
}

void CUILabel::Update()
{
	CUIFrameLineWnd::Update();
	if (m_lanim)
	{
		if(m_lainm_start_time<0.0f) m_lainm_start_time = Device.fTimeGlobal; 
		int frame;
		u32 clr					= m_lanim->CalculateRGB(Device.fTimeGlobal-m_lainm_start_time,frame);
		SetColor				(clr);
		SetTextColor			(clr);
	}
}
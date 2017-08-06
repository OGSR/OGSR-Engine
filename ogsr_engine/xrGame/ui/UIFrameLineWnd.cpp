#include "stdafx.h"
#include "UIFrameLineWnd.h"

CUIFrameLineWnd::CUIFrameLineWnd()
	:	bHorizontal(true),
		m_bTextureAvailable(false)
{
	AttachChild(&UITitleText);
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLineWnd::Init(float x, float y, float width, float height){
	inherited::Init(x,y, width, height);
	UITitleText.Init(0,0, width, 50);
	inherited::Init(x,y, width, height);
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLineWnd::Init(LPCSTR base_name, float x, float y, float width, float height, bool horizontal)
{
	Init(x,y,width,height);
	InitTexture(base_name, horizontal);
	if (horizontal)
		UITitleText.Init(0,0, width, 50);
	else
		UITitleText.Init(0,0, 50, height);	
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLineWnd::InitTexture(LPCSTR tex_name, bool horizontal){

	Frect			rect;
	GetAbsoluteRect	(rect);

	bHorizontal = horizontal;

	if (horizontal)
	{
		UIFrameLine.Init(tex_name, rect.left, rect.top, rect.right - rect.left, horizontal, alNone);
		UITitleText.Init(0,0, rect.right - rect.left, 50);
	}
	else
	{
		UIFrameLine.Init(tex_name, rect.left, rect.top, rect.bottom - rect.top, horizontal, alNone);
		UITitleText.Init(0,0, 50, rect.bottom - rect.top);
	}

	m_bTextureAvailable = true;
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLineWnd::Draw()
{
	if (m_bTextureAvailable)
	{
		Fvector2 p;
		GetAbsolutePos		(p);
		UIFrameLine.SetPos	(p.x, p.y);
		UIFrameLine.Render	();

		inherited::Draw();
	}	
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLineWnd::SetWidth(float width)
{
	inherited::SetWidth(width);
	if (bHorizontal)
		UIFrameLine.SetSize(width);
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLineWnd::SetHeight(float height)
{
	inherited::SetHeight(height);
	if (!bHorizontal)
		UIFrameLine.SetSize(height);
}

float CUIFrameLineWnd::GetTextureHeight(){
	return UIFrameLine.elements[0].GetRect().height();
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLineWnd::SetOrientation(bool horizontal)
{
	UIFrameLine.SetOrientation(horizontal);
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLineWnd::SetColor(u32 cl)
{
	UIFrameLine.SetColor(cl);
}
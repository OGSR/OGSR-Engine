#include "stdafx.h"
#include "UIFrameWindow.h"
#include "../HUDManager.h"
#include "UITextureMaster.h"
#include "UIXmlInit.h"
#include "UIStatic.h"

CUIFrameWindow::CUIFrameWindow()
{
	UITitleText					= xr_new<CUIStatic>();
	UITitleText->SetAutoDelete	(true);
	AttachChild					(UITitleText);
}

void CUIFrameWindow::Init(LPCSTR base_name, float x, float y, float width, float height)
{
	Init				(x,y,width,height);
	InitTexture			(base_name);	
}

void CUIFrameWindow::Init(float x, float y, float width, float height)
{
	CUIWindow::Init		(x,y,width,height);
	m_UIWndFrame.Init	(x,y,width,height);
	UITitleText->Init	(0,0, width, 50);
}

void CUIFrameWindow::Init(LPCSTR base_name, Frect* pRect)
{
	Init(base_name, pRect->left, pRect->top, 
				pRect->right - pRect->left, 
				pRect->bottom - pRect->top);
}

void CUIFrameWindow::InitTexture(const char* texture){
	m_UIWndFrame.InitTexture	(texture);
	m_bTextureVisible			= true;
}

void CUIFrameWindow::Draw()
{
	if (m_bTextureVisible)
	{
		Frect			rect;
		GetAbsoluteRect	(rect);
		Fvector2 v		= rect.lt;
		m_UIWndFrame.SetWndPos(v);
		m_UIWndFrame.Draw();
	}
	inherited::Draw();
}

void CUIFrameWindow::Update(){
	CUIWindow::Update();
	m_UIWndFrame.Update();
}

void CUIFrameWindow::SetWidth(float width)
{
	inherited::SetWidth(width);
	m_UIWndFrame.SetWidth(width);
}

void CUIFrameWindow::SetHeight(float height)
{
	inherited::SetHeight(height);
	m_UIWndFrame.SetHeight(height);
}

void CUIFrameWindow::SetColor(u32 cl)
{
	m_UIWndFrame.SetTextureColor(cl);
}

void CUIFrameWindow::FrameClip(const Frect parentAbsR)
{
	using std::min;
	using std::max;
	VERIFY(g_bRendering);

	// Если нет границ клиппанья, то скипаем
	if (!GetParent()) return;

	Frect		ourAbsR;
	GetAbsoluteRect(ourAbsR);
	CTexture	*T;
	Fvector2	ts;
	int			tile_x, tile_y;
	float		rem_x, rem_y;
	float			size_x, size_y;
	Frect		r, null;
	null.set(0.0f, 0.0f, 0.0f, 0.0f);

	m_UIWndFrame.UpdateSize();

	// Проверяем на видимость фрейма
	if (   max(ourAbsR.right, parentAbsR.left) == parentAbsR.left
		|| min(ourAbsR.left, parentAbsR.right) == parentAbsR.right
		|| min(ourAbsR.top, parentAbsR.bottom) == parentAbsR.bottom
		|| max(ourAbsR.bottom, parentAbsR.top) == parentAbsR.top)
	{
		m_UIWndFrame.frame[CUIFrameRect::fmRT].SetTile		(0, 0, 0, 0);
		m_UIWndFrame.frame[CUIFrameRect::fmR].SetTile		(0, 0, 0, 0);
		m_UIWndFrame.frame[CUIFrameRect::fmRB].SetTile		(0, 0, 0, 0);
		m_UIWndFrame.frame[CUIFrameRect::fmB].SetTile		(0, 0, 0, 0);
		m_UIWndFrame.frame[CUIFrameRect::fmLB].SetTile		(0, 0, 0, 0);
		m_UIWndFrame.frame[CUIFrameRect::fmL].SetTile		(0, 0, 0, 0);
		m_UIWndFrame.frame[CUIFrameRect::fmLT].SetTile		(0, 0, 0, 0);
		m_UIWndFrame.frame[CUIFrameRect::fmT].SetTile		(0, 0, 0, 0);
		m_UIWndFrame.frame[CUIFrameRect::fmBK].SetTile		(0, 0, 0, 0);
		return;
	}

	// fmRT
	r.x1 = max(m_UIWndFrame.frame[CUIFrameRect::fmRT].GetPosX(), parentAbsR.left) - m_UIWndFrame.frame[CUIFrameRect::fmRT].GetPosX();
	r.y1 = max(m_UIWndFrame.frame[CUIFrameRect::fmRT].GetPosY(), parentAbsR.top) - m_UIWndFrame.frame[CUIFrameRect::fmRT].GetPosY() ;
	r.x2 = min(ourAbsR.right, parentAbsR.right) -
		   max(m_UIWndFrame.frame[CUIFrameRect::fmRT].GetPosX(), parentAbsR.left) + r.x1;
	r.y2 = min(m_UIWndFrame.frame[CUIFrameRect::fmR].GetPosY(), parentAbsR.bottom) -
		   max(m_UIWndFrame.frame[CUIFrameRect::fmRT].GetPosY(), parentAbsR.top) + r.y1;

	ClampMax_Zero(r);
	m_UIWndFrame.frame[CUIFrameRect::fmRT].SetRect(r);
	m_UIWndFrame.frame[CUIFrameRect::fmRT].SetTile(1, 1, 0, 0);

	// fmR
	RCache.set_Shader(m_UIWndFrame.frame[CUIFrameRect::fmR].GetShader());
	T			= RCache.get_ActiveTexture(0);
	ts.set		((int)T->get_Width(),(int)T->get_Height());
	size_y		= min(m_UIWndFrame.frame[CUIFrameRect::fmRB].GetPosY(), parentAbsR.bottom) -
				  max(m_UIWndFrame.frame[CUIFrameRect::fmR].GetPosY(), parentAbsR.top);
	size_x		= min(ourAbsR.right, parentAbsR.right) -
				  max(m_UIWndFrame.frame[CUIFrameRect::fmR].GetPosX(), parentAbsR.left);
	rem_y		= fmod(size_y, ts.y);
	rem_x		= fmod(size_x, ts.x);
	tile_y		= iFloor(size_y / ts.y);
	tile_x		= iFloor(size_x / ts.x);

	if (tile_y < 0) tile_y = 0;
	if (tile_x < 0) tile_x = 0;
	set_positive(rem_x);
	set_positive(rem_y);

	m_UIWndFrame.frame[CUIFrameRect::fmR].SetTile(tile_x, tile_y, rem_x, rem_y);

	m_UIWndFrame.frame[CUIFrameRect::fmR].SetPos(max(m_UIWndFrame.frame[CUIFrameRect::fmR].GetPosX(), parentAbsR.left),
			max(m_UIWndFrame.frame[CUIFrameRect::fmR].GetPosY(), parentAbsR.top));

	// fmRB
	r.x1 = max(m_UIWndFrame.frame[CUIFrameRect::fmRB].GetPosX(), parentAbsR.left) - m_UIWndFrame.frame[CUIFrameRect::fmRB].GetPosX();
	r.y1 = max(m_UIWndFrame.frame[CUIFrameRect::fmRB].GetPosY(), parentAbsR.top) - m_UIWndFrame.frame[CUIFrameRect::fmRB].GetPosY();
	r.x2 = min(ourAbsR.right, parentAbsR.right) -
		   max(m_UIWndFrame.frame[CUIFrameRect::fmRB].GetPosX(), parentAbsR.left) + r.x1;
	r.y2 = min(ourAbsR.bottom, parentAbsR.bottom) -
		   max(m_UIWndFrame.frame[CUIFrameRect::fmRB].GetPosY(), parentAbsR.top) + r.y1;

	ClampMax_Zero(r);
	m_UIWndFrame.frame[CUIFrameRect::fmRB].SetRect(r);
	m_UIWndFrame.frame[CUIFrameRect::fmRB].SetTile(1, 1, 0, 0);

	// fmB
	RCache.set_Shader(m_UIWndFrame.frame[CUIFrameRect::fmB].GetShader());
	T			= RCache.get_ActiveTexture(0);
	ts.set		((int)T->get_Width(),(int)T->get_Height());
	size_x		= min(m_UIWndFrame.frame[CUIFrameRect::fmRB].GetPosX(), parentAbsR.right) -
				  max(m_UIWndFrame.frame[CUIFrameRect::fmB].GetPosX(), parentAbsR.left);
	size_y		= min(ourAbsR.bottom, parentAbsR.bottom) -
				  max(m_UIWndFrame.frame[CUIFrameRect::fmRB].GetPosY(), parentAbsR.top);
	rem_x		= fmod(size_x, ts.x);
	rem_y		= fmod(size_y, ts.y);
	tile_x		= iFloor(float(size_x) / ts.x);
	tile_y		= iFloor(float(size_y) / ts.y);

	ClampMax_Zero(r);
	if (tile_x < 0) tile_x = 0;
	if (tile_y < 0) tile_y = 0;
	set_positive(rem_x);
	set_positive(rem_y);

	m_UIWndFrame.frame[CUIFrameRect::fmB].SetTile	(tile_x, tile_y, rem_x, rem_y);

	m_UIWndFrame.frame[CUIFrameRect::fmB].SetPos(max(m_UIWndFrame.frame[CUIFrameRect::fmB].GetPosX(), parentAbsR.left),
		max(m_UIWndFrame.frame[CUIFrameRect::fmB].GetPosY(), parentAbsR.top));

	// fmLB
	r.x1 = max(m_UIWndFrame.frame[CUIFrameRect::fmLB].GetPosX(), parentAbsR.left) - m_UIWndFrame.frame[CUIFrameRect::fmLB].GetPosX();
	r.y1 = max(m_UIWndFrame.frame[CUIFrameRect::fmLB].GetPosY(), parentAbsR.top) - m_UIWndFrame.frame[CUIFrameRect::fmLB].GetPosY();
	r.x2 = min(m_UIWndFrame.frame[CUIFrameRect::fmB].GetPosX(), parentAbsR.right) -
		   max(m_UIWndFrame.frame[CUIFrameRect::fmLB].GetPosX(), parentAbsR.left) + r.x1;
	r.y2 = min(ourAbsR.bottom, parentAbsR.bottom) -
		   max(m_UIWndFrame.frame[CUIFrameRect::fmRB].GetPosY(), parentAbsR.top) + r.y1;

	ClampMax_Zero(r);
	m_UIWndFrame.frame[CUIFrameRect::fmLB].SetRect(r);
	m_UIWndFrame.frame[CUIFrameRect::fmLB].SetTile(1, 1, 0, 0);

	// fmL
	RCache.set_Shader(m_UIWndFrame.frame[CUIFrameRect::fmL].GetShader());
	T			= RCache.get_ActiveTexture(0);
	ts.set		((int)T->get_Width(),(int)T->get_Height());
	size_y		= min(m_UIWndFrame.frame[CUIFrameRect::fmLB].GetPosY(), parentAbsR.bottom) -
				  max(m_UIWndFrame.frame[CUIFrameRect::fmL].GetPosY(), parentAbsR.top);
	size_x		= min(m_UIWndFrame.frame[CUIFrameRect::fmB].GetPosX(), parentAbsR.right) -
				  max(m_UIWndFrame.frame[CUIFrameRect::fmL].GetPosX(), parentAbsR.left);
	rem_x		= fmod(size_x, ts.x);
	rem_y		= fmod(size_y, ts.y);
	tile_y		= iFloor(float(size_y) / ts.y);
	tile_x		= iFloor(float(size_x) / ts.y);

	if (tile_y < 0) tile_y = 0;
	if (tile_x < 0) tile_x = 0;
	set_positive(rem_x);
	set_positive(rem_y);

	m_UIWndFrame.frame[CUIFrameRect::fmL].SetTile	(tile_x, tile_y, rem_x, rem_y);

	m_UIWndFrame.frame[CUIFrameRect::fmL].SetPos(max(m_UIWndFrame.frame[CUIFrameRect::fmL].GetPosX(), parentAbsR.left),
		max(m_UIWndFrame.frame[CUIFrameRect::fmL].GetPosY(), parentAbsR.top));

	// fmLT
	r.x1 = max(m_UIWndFrame.frame[CUIFrameRect::fmLT].GetPosX(), parentAbsR.left) - m_UIWndFrame.frame[CUIFrameRect::fmLT].GetPosX();
	r.y1 = max(m_UIWndFrame.frame[CUIFrameRect::fmLT].GetPosY(), parentAbsR.top) - m_UIWndFrame.frame[CUIFrameRect::fmLT].GetPosY();
	r.x2 = min(m_UIWndFrame.frame[CUIFrameRect::fmBK].GetPosX(), parentAbsR.right) -
		   max(ourAbsR.left, parentAbsR.left) + r.x1;
	r.y2 = min(m_UIWndFrame.frame[CUIFrameRect::fmL].GetPosY(), parentAbsR.bottom) -
		   max(m_UIWndFrame.frame[CUIFrameRect::fmLT].GetPosY(), parentAbsR.top) + r.y1;

	ClampMax_Zero(r);
	m_UIWndFrame.frame[CUIFrameRect::fmLT].SetRect(r);
	m_UIWndFrame.frame[CUIFrameRect::fmLT].SetTile(1, 1, 0, 0);

	// fmT
	RCache.set_Shader(m_UIWndFrame.frame[CUIFrameRect::fmT].GetShader());
	T			= RCache.get_ActiveTexture(0);
	ts.set		((int)T->get_Width(),(int)T->get_Height());
	size_x		= min(m_UIWndFrame.frame[CUIFrameRect::fmRT].GetPosX(), parentAbsR.right) -
				  max(m_UIWndFrame.frame[CUIFrameRect::fmT].GetPosX(), parentAbsR.left);
	size_y		= min(m_UIWndFrame.frame[CUIFrameRect::fmBK].GetPosY(), parentAbsR.bottom) -
				  max(m_UIWndFrame.frame[CUIFrameRect::fmT].GetPosY(), parentAbsR.top);
	rem_x		= fmod(size_x, ts.x);
	rem_y		= fmod(size_y, ts.y);
	tile_x		= iFloor(float(size_x) / ts.x);
	tile_y		= iFloor(float(size_y) / ts.y);

	if (tile_x < 0 || tile_y < 0)
	{
		rem_x = rem_y = 0.0f;
		tile_x = tile_y = 0;
	}

	m_UIWndFrame.frame[CUIFrameRect::fmT].SetTile	(tile_x, tile_y, rem_x, rem_y);

	m_UIWndFrame.frame[CUIFrameRect::fmT].SetPos(max(m_UIWndFrame.frame[CUIFrameRect::fmT].GetPosX(), parentAbsR.left),
		max(m_UIWndFrame.frame[CUIFrameRect::fmT].GetPosY(), parentAbsR.top));

	// back
	RCache.set_Shader(m_UIWndFrame.frame[CUIFrameRect::fmBK].GetShader());
	T			= RCache.get_ActiveTexture(0);
	ts.set		((int)T->get_Width(),(int)T->get_Height());
	size_x		= min(m_UIWndFrame.frame[CUIFrameRect::fmR].GetPosX(), parentAbsR.right) -
				  max(m_UIWndFrame.frame[CUIFrameRect::fmBK].GetPosX(), parentAbsR.left);
	size_y		= min(m_UIWndFrame.frame[CUIFrameRect::fmB].GetPosY(), parentAbsR.bottom) -
				  max(m_UIWndFrame.frame[CUIFrameRect::fmBK].GetPosY(), parentAbsR.top);
	rem_x		= fmod(size_x, ts.x);
	rem_y		= fmod(size_y, ts.y);
	tile_x		= iFloor(float(size_x) / ts.x);
	tile_y		= iFloor(float(size_y) / ts.y);

	if (tile_y < 0 || tile_x < 0)
	{
		m_UIWndFrame.frame[CUIFrameRect::fmBK].SetRect(null);
		m_UIWndFrame.frame[CUIFrameRect::fmBK].SetTile(0, 0, 0, 0);
		m_UIWndFrame.frame[CUIFrameRect::fmBK].SetPos(0, 0);
	}
	else
	{
		m_UIWndFrame.frame[CUIFrameRect::fmBK].SetRect(0, 0, ts.x, ts.y);
		m_UIWndFrame.frame[CUIFrameRect::fmBK].SetTile(tile_x,tile_y,rem_x,rem_y);

		m_UIWndFrame.frame[CUIFrameRect::fmBK].SetPos(max(m_UIWndFrame.frame[CUIFrameRect::fmBK].GetPosX(), parentAbsR.left),
			max(m_UIWndFrame.frame[CUIFrameRect::fmBK].GetPosY(), parentAbsR.top));
	}
}

inline void CUIFrameWindow::ClampMax_Zero(Frect &r)
{
	clamp(r.x1, 0.0f, abs(r.x1));
	clamp(r.x2, 0.0f, abs(r.x2));
	clamp(r.y1, 0.0f, abs(r.y1));
	clamp(r.y2, 0.0f, abs(r.y2));
}
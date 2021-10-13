#include "stdafx.h"
#include "uicursor.h"
#include "ui/UIStatic.h"


CUICursor::CUICursor()
{
	InitInternal();
	Device.seqRender.Add(this, -3/*2*/);
}

CUICursor::~CUICursor()
{
	Device.seqRender.Remove(this);
}


void CUICursor::InitInternal()
{
	m_static = std::make_unique<CUIStatic>();
	m_static->InitTextureEx("ui\\ui_ani_cursor", "hud\\cursor");
	constexpr Frect rect{ 0.0f, 0.0f, 40.0f, 40.0f };
	m_static->SetOriginalRect(rect);
	Fvector2 sz{ rect.rb };
	sz.x *= UI()->get_current_kx();

	m_static->SetWndSize(sz);
	m_static->SetStretchTexture(true);
}

void CUICursor::OnRender()
{
	if (!IsVisible()) return;

#ifdef DEBUG
	static u32 last_render_frame = 0;
	VERIFY(last_render_frame != Device.dwFrame);
	last_render_frame = Device.dwFrame;

	if (bDebug)
	{
		CGameFont* F = UI()->Font()->pFontDI;
		F->SetAligment(CGameFont::alCenter);
		F->SetHeightI(0.02f);
		F->OutSetI(0.f, -0.9f);
		F->SetColor(0xffffffff);
		Fvector2 pt = GetCursorPosition();
		F->OutNext("%f-%f", pt.x, pt.y);
	}
#endif

	m_static->SetWndPos(vPos);
	m_static->Update();
	m_static->Draw();
}

Fvector2 CUICursor::GetCursorPosition() const
{
	return vPos;
}

Fvector2 CUICursor::GetCursorPositionDelta() const
{
	return Fvector2{ vPos.x - vPrevPos.x , vPos.y - vPrevPos.y };
}

void CUICursor::UpdateCursorPosition(const int _dx, const int _dy)
{
	vPrevPos = vPos;

	const u32 screen_size_x = GetSystemMetrics(SM_CXSCREEN);
	const u32 screen_size_y = GetSystemMetrics(SM_CYSCREEN);
	const bool m_b_use_win_cursor = (screen_size_y >= Device.dwHeight && screen_size_x >= Device.dwWidth);

	if (m_b_use_win_cursor)
	{
		Ivector2 pti;
		IInputReceiver::IR_GetMousePosReal(pti);

		vPos.x = (float)pti.x * (UI_BASE_WIDTH / (float)Device.dwWidth);
		vPos.y = (float)pti.y * (UI_BASE_HEIGHT / (float)Device.dwHeight);
	}
	else
	{
		constexpr float sens = 1.0f;
		vPos.x += _dx * sens;
		vPos.y += _dy * sens;
	}

	clamp(vPos.x, 0.f, UI_BASE_WIDTH);
	clamp(vPos.y, 0.f, UI_BASE_HEIGHT);
}

void CUICursor::SetUICursorPosition(const Fvector2& pos)
{
	vPos = pos;

	const int x = iFloor(vPos.x / (UI_BASE_WIDTH / (float)Device.dwWidth));
	const int y = iFloor(vPos.y / (UI_BASE_HEIGHT / (float)Device.dwHeight));

	SetCursorPos(x, y);
}

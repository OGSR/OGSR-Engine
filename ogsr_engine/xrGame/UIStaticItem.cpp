#include "stdafx.h"
#include "uistaticitem.h"
#include "hudmanager.h"

void CreateUIGeom() { UIRender->CreateUIGeom(); }

void DestroyUIGeom() { UIRender->DestroyUIGeom(); }

CUIStaticItem::CUIStaticItem()
{
    dwColor = 0xffffffff;
    iTileX = 1;
    iTileY = 1;
    iRemX = 0.0f;
    iRemY = 0.0f;
    alpha_ref = -1;
#ifdef DEBUG
    dbg_tex_name = NULL;
#endif
}

CUIStaticItem::~CUIStaticItem() {}

void CUIStaticItem::CreateShader(LPCSTR tex, LPCSTR sh)
{
    hShader->create(sh, tex, !!uFlags.test(flNoShaderCache));

#ifdef DEBUG
    dbg_tex_name = tex;
#endif
    uFlags.set(flValidRect, FALSE);
    //uFlags.set(flValidOriginalRect, FALSE); // вызывать через ResetOriginalRect
}

void CUIStaticItem::SetShader(const ui_shader& sh) { hShader = sh; }

void CUIStaticItem::Init(LPCSTR tex, LPCSTR sh, float left, float top, u32 align)
{
    uFlags.set(flValidRect, FALSE);
    //uFlags.set(flValidOriginalRect, FALSE); // вызывать через ResetOriginalRect

    CreateShader(tex, sh);
    SetPos(left, top);
    SetAlign(align);
}

void CUIStaticItem::Render()
{
    VERIFY(g_bRendering);

    UIRender->SetShader(*hShader);
    if (alpha_ref != -1)
        UIRender->SetAlphaRef(alpha_ref);

    // convert&set pos
    Fvector2 bp;
    UI()->ClientToScreenScaled(bp, float(iPos.x), float(iPos.y));
    //bp.x = (float)iFloor(bp.x);
    //bp.y = (float)iFloor(bp.y);

    // actual rendering
    Fvector2 pos;
    Fvector2 f_len;
    UI()->ClientToScreenScaled(f_len, iVisRect.x2, iVisRect.y2);

    int tile_x = fis_zero(iRemX) ? iTileX : iTileX + 1;
    int tile_y = fis_zero(iRemY) ? iTileY : iTileY + 1;
    if (!(tile_x && tile_y))
        return;

    // render
    UIRender->StartPrimitive(8 * tile_x * tile_y, IUIRender::ptTriList, UI()->m_currentPointType);
    for (int x = 0; x < tile_x; ++x)
    {
        for (int y = 0; y < tile_y; ++y)
        {
            pos.set(bp.x + f_len.x * x, bp.y + f_len.y * y);
            inherited::Render(pos, dwColor);
        }
    }

    // set scissor
    Frect clip_rect = {iPos.x, iPos.y, iPos.x + iVisRect.x2 * iTileX + iRemX, iPos.y + iVisRect.y2 * iTileY + iRemY};
    UI()->PushScissor(clip_rect);

    UIRender->FlushPrimitive();

    UI()->PopScissor();

    if (alpha_ref != -1)
        UIRender->SetAlphaRef(0);
}

void CUIStaticItem::Render(float angle)
{
    VERIFY(g_bRendering);

    UIRender->SetShader(*hShader);
    if (alpha_ref != -1)
        UIRender->SetAlphaRef(alpha_ref);

    // convert&set pos
    Fvector2 bp_ns;
    bp_ns.set(iPos);

    // actual rendering
    //.	UIRender->StartTriFan(32);
    UIRender->StartPrimitive(32, IUIRender::ptTriList, UI()->m_currentPointType);
    inherited::Render(bp_ns, dwColor, angle);

    //.	UIRender->FlushTriFan();
    UIRender->FlushPrimitive();

    if (alpha_ref != -1)
        UIRender->SetAlphaRef(alpha_ref);
}

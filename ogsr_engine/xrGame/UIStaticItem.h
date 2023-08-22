#pragma once

#include "ui/uiabstract.h"
#include "uicustomitem.h"

void CreateUIGeom();
void DestroyUIGeom();

class IUIShader;

class CUIStaticItem : public IUISimpleTextureControl, public CUICustomItem
{
    ui_shader hShader;

    Fvector2 iPos;
    u32 dwColor;
    int iTileX;
    int iTileY;
    float iRemX;
    float iRemY;
    int alpha_ref;

protected:
    typedef CUICustomItem inherited;

public:
    using CUICustomItem::SetOriginalRect;

#ifdef DEBUG
    shared_str dbg_tex_name;
#endif

    CUIStaticItem();
    virtual ~CUIStaticItem();

    void SetAlphaRef(int val) { alpha_ref = val; };

    virtual void CreateShader(const char* tex, const char* sh = "hud\\default");

    virtual void SetTextureColor(u32 color) { SetColor(color); }
    virtual u32 GetTextureColor() const { return GetColor(); }

    virtual void SetOriginalRect(const Frect& r)
    {
        iOriginalRect = r;
        uFlags.set(flValidOriginalRect, TRUE);
    }

    virtual void SetOriginalRectEx(const Frect& r)
    {
        iOriginalRect = r;
        uFlags.set(flValidOriginalRect, TRUE);
        SetRect(0, 0, r.width(), r.height());
    }

    void ResetOriginalRect() { uFlags.set(flValidOriginalRect, FALSE); }

    void SetNoShaderCache(const bool v) { uFlags.set(flNoShaderCache, v); }

    void Init(LPCSTR tex, LPCSTR sh, float left, float top, u32 align);

    void Render();
    void Render(float angle);

    IC void SetTile(int tile_x, int tile_y, float rem_x, float rem_y)
    {
        iTileX = tile_x;
        iTileY = tile_y;
        iRemX = rem_x;
        iRemY = rem_y;
    }
    IC void SetPos(float left, float top) { iPos.set(left, top); }
    IC void SetPosX(float left) { iPos.x = left; }
    IC void SetPosY(float top) { iPos.y = top; }

    IC float GetPosX() { return iPos.x; }
    IC float GetPosY() { return iPos.y; }

    IC void SetColor(u32 clr) { dwColor = clr; }
    IC void SetColor(Fcolor clr) { dwColor = clr.get(); }
    IC u32 GetColor() const { return dwColor; }
    IC u32& GetColorRef() { return dwColor; }

    virtual void SetShader(const ui_shader& sh);
    ui_shader& GetShader() { return hShader; }
};

extern ENGINE_API BOOL g_bRendering;

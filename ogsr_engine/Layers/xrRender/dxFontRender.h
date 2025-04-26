#pragma once

#include "../../Include/xrRender/FontRender.h"

class dxFontRender : public IFontRender
{
public:
    dxFontRender();
    virtual ~dxFontRender();

    virtual void Initialize(LPCSTR cShader, LPCSTR cTexture);
    virtual void OnRender(CGameFont& owner);

private:
    ref_shader pShader;
    ref_geom pGeom;

    void RenderFragment(CGameFont& owner, u32& i, bool shadow_mode, float dX, float dY, u32 length, u32 last);
};

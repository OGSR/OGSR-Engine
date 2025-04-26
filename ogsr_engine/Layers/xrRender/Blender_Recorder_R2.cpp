#include "stdafx.h"

#include "ResourceManager.h"
#include "blenders/Blender_Recorder.h"
#include "blenders/Blender.h"

#include "dxRenderDeviceRender.h"

void CBlender_Compile::r_Pass(LPCSTR _vs, LPCSTR _ps, bool bFog, BOOL bZtest, BOOL bZwrite, BOOL bABlend, D3DBLEND abSRC, D3DBLEND abDST, BOOL aTest, u32 aRef)
{
    RS.Invalidate();
    ctable.clear();
    passTextures.clear();
    passMatrices.clear();
    passConstants.clear();
    dwStage = 0;

    // Setup FF-units (Z-buffer, blender)
    PassSET_ZB(bZtest, bZwrite);
    PassSET_Blend(bABlend, abSRC, abDST, aTest, aRef);
    PassSET_LightFog(FALSE, bFog);

    // Create shaders
    SPS* ps = DEV->_CreatePS(_ps);
    SVS* vs = DEV->_CreateVS(_vs);
    SGS* gs = DEV->_CreateGS("null");
    dest.ps = ps;
    dest.vs = vs;
    dest.gs = gs;
    dest.hs = DEV->_CreateHS("null");
    dest.ds = DEV->_CreateDS("null");
    dest.cs = DEV->_CreateCS("null");

    ctable.merge(&ps->constants);
    ctable.merge(&vs->constants);

    // Last Stage - disable
    if (0 == stricmp(_ps, "null"))
    {
        RS.SetTSS(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
        RS.SetTSS(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    }
}

void CBlender_Compile::r_Constant(LPCSTR name, R_constant_setup* s) const
{
    R_ASSERT(s);
    const ref_constant C = ctable.get(name);
    if (C)
        C->handler = s;
}

void CBlender_Compile::r_ColorWriteEnable(bool cR, bool cG, bool cB, bool cA)
{
    BYTE Mask = 0;
    Mask |= cR ? D3DCOLORWRITEENABLE_RED : 0;
    Mask |= cG ? D3DCOLORWRITEENABLE_GREEN : 0;
    Mask |= cB ? D3DCOLORWRITEENABLE_BLUE : 0;
    Mask |= cA ? D3DCOLORWRITEENABLE_ALPHA : 0;

    RS.SetRS(D3DRS_COLORWRITEENABLE, Mask);
    RS.SetRS(D3DRS_COLORWRITEENABLE1, Mask);
    RS.SetRS(D3DRS_COLORWRITEENABLE2, Mask);
    RS.SetRS(D3DRS_COLORWRITEENABLE3, Mask);
}

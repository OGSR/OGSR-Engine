// Blender_Recorder.h: interface for the CBlender_Recorder class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../tss.h"

#pragma pack(push, 4)

class CBlender_Compile
{
public:
    sh_list L_textures;
    sh_list L_constants;
    sh_list L_matrices;

    LPCSTR detail_texture;

    BOOL bEditor;
    BOOL bDetail;
    BOOL bDetail_Diffuse;
    BOOL bDetail_Bump;
    BOOL bUseSteepParallax;

    int iElement;
    bool HudElement{};

public:
    CSimulator RS;
    IBlender* BT;
    ShaderElement* SH;

    enum
    {
        NO_TESS = 0,
        TESS_PN = 1,
        TESS_HM = 2,
        TESS_PN_HM = 3
    };
    u32 TessMethod;


private:
    SPass dest;
    R_constant_table ctable;

    STextureList passTextures;
    SMatrixList passMatrices;
    SConstantList passConstants;
    u32 dwStage;

    string128 pass_vs;
    string128 pass_ps;
    string128 pass_gs;
    string128 pass_hs;
    string128 pass_ds;
    string128 pass_cs;

    u32 BC(BOOL v) { return v ? 0x01 : 0; }

public:
    CSimulator& R() { return RS; }

    void SetParams(int iPriority, bool bStrictB2F) const;
    void SetMapping() const;

    u32 Pass() const { return SH->passes.size(); }

    // R1-compiler
    void PassBegin();
    void PassSET_ZB(BOOL bZTest, BOOL bZWrite, BOOL bInvertZTest = FALSE);
    void PassSET_ablend_mode(BOOL bABlend, u32 abSRC, u32 abDST);
    void PassSET_ablend_aref(BOOL aTest, u32 aRef);
    void PassSET_Blend(BOOL bABlend, u32 abSRC, u32 abDST, BOOL aTest, u32 aRef);
    void PassSET_Blend_BLEND(BOOL bAref = FALSE, u32 ref = 0) { PassSET_Blend(TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, bAref, ref); }
    void PassSET_Blend_SET(BOOL bAref = FALSE, u32 ref = 0) { PassSET_Blend(FALSE, D3DBLEND_ONE, D3DBLEND_ZERO, bAref, ref); }
    void PassSET_Blend_ADD(BOOL bAref = FALSE, u32 ref = 0) { PassSET_Blend(TRUE, D3DBLEND_ONE, D3DBLEND_ONE, bAref, ref); }
    void PassSET_Blend_MUL(BOOL bAref = FALSE, u32 ref = 0) { PassSET_Blend(TRUE, D3DBLEND_DESTCOLOR, D3DBLEND_ZERO, bAref, ref); }
    void PassSET_Blend_MUL2X(BOOL bAref = FALSE, u32 ref = 0) { PassSET_Blend(TRUE, D3DBLEND_DESTCOLOR, D3DBLEND_SRCCOLOR, bAref, ref); }
    void PassSET_LightFog(BOOL bLight, BOOL bFog);
    void PassSET_PS(LPCSTR name);
    void PassSET_VS(LPCSTR name);

    u32 Stage() const { return dwStage; }

    void StageBegin();
    void StageSET_Address(u32 adr);
    void StageSET_Color(u32 a1, u32 op, u32 a2);
    void StageSET_Color3(u32 a1, u32 op, u32 a2, u32 a3);
    void StageSET_Alpha(u32 a1, u32 op, u32 a2);
    void StageEnd();

    // R1/R2-compiler	[programmable]
    void i_dx10Address(u32 s, u32 address);
    void i_dx10Filter_Min(u32 s, u32 f);
    void i_dx10Filter_Mip(u32 s, u32 f);
    void i_dx10Filter_Mag(u32 s, u32 f);
    void i_dx10FilterAnizo(u32 s, BOOL value);
    void i_dx10Filter(u32 s, u32 _min, u32 _mip, u32 _mag);
    void i_dx10BorderColor(u32 s, u32 color);

    void r_Pass(LPCSTR vs, LPCSTR ps, bool bFog, BOOL bZtest = TRUE, BOOL bZwrite = TRUE, BOOL bABlend = FALSE, D3DBLEND abSRC = D3DBLEND_ONE, D3DBLEND abDST = D3DBLEND_ZERO, BOOL aTest = FALSE, u32 aRef = 0);
    void r_Constant(LPCSTR name, R_constant_setup* s) const;

    void r_Pass(LPCSTR vs, LPCSTR gs, LPCSTR ps, bool bFog, BOOL bZtest = TRUE, BOOL bZwrite = TRUE, BOOL bABlend = FALSE, D3DBLEND abSRC = D3DBLEND_ONE, D3DBLEND abDST = D3DBLEND_ZERO, BOOL aTest = FALSE, u32 aRef = 0);

    void r_TessPass(LPCSTR vs, LPCSTR hs, LPCSTR ds, LPCSTR gs, LPCSTR ps, bool bFog, BOOL bZtest = TRUE, BOOL bZwrite = TRUE, BOOL bABlend = FALSE, D3DBLEND abSRC = D3DBLEND_ONE, D3DBLEND abDST = D3DBLEND_ZERO, BOOL aTest = FALSE, u32 aRef = 0);
    void r_ComputePass(LPCSTR cs);

    void r_Stencil(BOOL Enable, u32 Func = D3DCMP_ALWAYS, u32 Mask = 0x00, u32 WriteMask = 0x00, u32 Fail = D3DSTENCILOP_KEEP, u32 Pass = D3DSTENCILOP_KEEP, u32 ZFail = D3DSTENCILOP_KEEP);
    void r_StencilRef(u32 Ref);
    void r_CullMode(D3DCULL Mode);

    void r_dx10Texture(LPCSTR ResourceName, LPCSTR texture);
    void r_dx10Texture(LPCSTR ResourceName, const shared_str& texture) { return r_dx10Texture(ResourceName, texture.c_str()); }

    u32 r_dx10Sampler(LPCSTR ResourceName);

    void r_ColorWriteEnable(bool cR = true, bool cG = true, bool cB = true, bool cA = true);
    void r_End();

    CBlender_Compile();
    ~CBlender_Compile();

    void _cpp_Compile(ShaderElement* _SH);
    ShaderElement* _lua_Compile(LPCSTR namesp, LPCSTR name);
};
#pragma pack(pop)

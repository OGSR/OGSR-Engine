#include "stdafx.h"


#include "Blender_Screen_SET.h"

#define VER_2_oBlendCount 7
#define VER_4_oBlendCount 9
#define VER_5_oBlendCount 10

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Screen_SET::CBlender_Screen_SET()
{
    description.CLS = B_SCREEN_SET;
    description.version = 4;
    oBlend.Count = VER_4_oBlendCount;
    oBlend.IDselected = 0;
    oAREF.value = 32;
    oAREF.min = 0;
    oAREF.max = 255;
    oZTest.value = FALSE;
    oZWrite.value = FALSE;
    oLighting.value = FALSE;
    oFog.value = FALSE;
    oClamp.value = TRUE;
}

CBlender_Screen_SET::~CBlender_Screen_SET() {}

void CBlender_Screen_SET::Save(IWriter& fs)
{
    IBlenderXr::Save(fs);

    // Blend mode
    xrPWRITE_PROP(fs, "Blending", xrPID_TOKEN, oBlend);

    xrP_TOKEN::Item I;
    I.ID = 0;
    xr_strcpy(I.str, "SET");
    fs.w(&I, sizeof(I));
    I.ID = 1;
    xr_strcpy(I.str, "BLEND");
    fs.w(&I, sizeof(I));
    I.ID = 2;
    xr_strcpy(I.str, "ADD");
    fs.w(&I, sizeof(I));
    I.ID = 3;
    xr_strcpy(I.str, "MUL");
    fs.w(&I, sizeof(I));
    I.ID = 4;
    xr_strcpy(I.str, "MUL_2X");
    fs.w(&I, sizeof(I));
    I.ID = 5;
    xr_strcpy(I.str, "ALPHA-ADD");
    fs.w(&I, sizeof(I));
    I.ID = 6;
    xr_strcpy(I.str, "MUL_2X (B^D)");
    fs.w(&I, sizeof(I));
    I.ID = 7;
    xr_strcpy(I.str, "SET (2r)");
    fs.w(&I, sizeof(I));
    I.ID = 8;
    xr_strcpy(I.str, "BLEND (2r)");
    fs.w(&I, sizeof(I));
    I.ID = 9;
    xr_strcpy(I.str, "BLEND (4r)");
    fs.w(&I, sizeof(I));

    // Params
    xrPWRITE_PROP(fs, "Texture clamp", xrPID_BOOL, oClamp);
    xrPWRITE_PROP(fs, "Alpha ref", xrPID_INTEGER, oAREF);
    xrPWRITE_PROP(fs, "Z-test", xrPID_BOOL, oZTest);
    xrPWRITE_PROP(fs, "Z-write", xrPID_BOOL, oZWrite);
    xrPWRITE_PROP(fs, "Lighting", xrPID_BOOL, oLighting);
    xrPWRITE_PROP(fs, "Fog", xrPID_BOOL, oFog);
}

void CBlender_Screen_SET::Load(IReader& fs, u16 version)
{
    IBlenderXr::Load(fs, version);

    switch (version)
    {
    case 2:
        xrPREAD_PROP(fs, xrPID_TOKEN, oBlend);
        oBlend.Count = VER_5_oBlendCount;
        xrPREAD_PROP(fs, xrPID_INTEGER, oAREF);
        xrPREAD_PROP(fs, xrPID_BOOL, oZTest);
        xrPREAD_PROP(fs, xrPID_BOOL, oZWrite);
        xrPREAD_PROP(fs, xrPID_BOOL, oLighting);
        xrPREAD_PROP(fs, xrPID_BOOL, oFog);
        break;
    case 3:
        xrPREAD_PROP(fs, xrPID_TOKEN, oBlend);
        oBlend.Count = VER_5_oBlendCount;
        xrPREAD_PROP(fs, xrPID_BOOL, oClamp);
        xrPREAD_PROP(fs, xrPID_INTEGER, oAREF);
        xrPREAD_PROP(fs, xrPID_BOOL, oZTest);
        xrPREAD_PROP(fs, xrPID_BOOL, oZWrite);
        xrPREAD_PROP(fs, xrPID_BOOL, oLighting);
        xrPREAD_PROP(fs, xrPID_BOOL, oFog);
        break;
    default:
        xrPREAD_PROP(fs, xrPID_TOKEN, oBlend);
        oBlend.Count = VER_5_oBlendCount;
        xrPREAD_PROP(fs, xrPID_BOOL, oClamp);
        xrPREAD_PROP(fs, xrPID_INTEGER, oAREF);
        xrPREAD_PROP(fs, xrPID_BOOL, oZTest);
        xrPREAD_PROP(fs, xrPID_BOOL, oZWrite);
        xrPREAD_PROP(fs, xrPID_BOOL, oLighting);
        xrPREAD_PROP(fs, xrPID_BOOL, oFog);
        break;
    }
}

void CBlender_Screen_SET::SaveIni(CInifile* ini_file, LPCSTR section)
{
    IBlenderXr::SaveIni(ini_file, section);

    WriteToken(ini_file, section, "blending", oBlend);

    WriteBool(ini_file, section, "texture_clamp", oClamp);
    WriteInteger(ini_file, section, "alpha_ref", oAREF);
    WriteBool(ini_file, section, "z_test", oZTest);
    WriteBool(ini_file, section, "z_write", oZWrite);
    WriteBool(ini_file, section, "lighting", oLighting);
    WriteBool(ini_file, section, "fog", oFog);
}

void CBlender_Screen_SET::LoadIni(CInifile* ini_file, LPCSTR section)
{
    IBlenderXr::LoadIni(ini_file, section);

    ReadToken(ini_file, section, "blending", oBlend);

    ReadBool(ini_file, section, "texture_clamp", oClamp);
    ReadInteger(ini_file, section, "alpha_ref", oAREF);
    ReadBool(ini_file, section, "z_test", oZTest);
    ReadBool(ini_file, section, "z_write", oZWrite);
    ReadBool(ini_file, section, "lighting", oLighting);
    ReadBool(ini_file, section, "fog", oFog);
}

void CBlender_Screen_SET::Compile(CBlender_Compile& C)
{
    IBlender::Compile(C);

    if (oBlend.IDselected == 6)
    {
        // Usually for wallmarks
        C.r_Pass("stub_notransform_t", "stub_default_ma", false);
    }
    else if (9 == oBlend.IDselected)
    {
        // 4x R
        C.r_Pass("stub_notransform_t_m4", "stub_default", false);
    }
    else if ((7 == oBlend.IDselected) || (8 == oBlend.IDselected))
    {
        // 2x R
        C.r_Pass("stub_notransform_t_m2", "stub_default", false);
    }
    else
    {
        // 1x R
        C.r_Pass("stub_notransform_t", "stub_default", false);
    }    

    VERIFY(C.L_textures.size() > 0);
    C.r_dx10Texture("s_base", C.L_textures[0]);
    const int iSmp = C.r_dx10Sampler("smp_base");
    if ((oClamp.value) && (iSmp != u32(-1)))
        C.i_dx10Address(iSmp, D3DTADDRESS_CLAMP);

    C.PassSET_ZB(oZTest.value, oZWrite.value);

    switch (oBlend.IDselected)
    {
    case 0: // SET
        C.PassSET_Blend(FALSE, D3DBLEND_ONE, D3DBLEND_ZERO, FALSE, 0);
        break;
    case 1: // BLEND
        C.PassSET_Blend(TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, TRUE, oAREF.value);
        break;
    case 2: // ADD
        C.PassSET_Blend(TRUE, D3DBLEND_ONE, D3DBLEND_ONE, FALSE, oAREF.value);
        break;
    case 3: // MUL
        C.PassSET_Blend(TRUE, D3DBLEND_DESTCOLOR, D3DBLEND_ZERO, FALSE, oAREF.value);
        break;
    case 4: // MUL_2X
        C.PassSET_Blend(TRUE, D3DBLEND_DESTCOLOR, D3DBLEND_SRCCOLOR, FALSE, oAREF.value);
        break;
    case 5: // ALPHA-ADD
        C.PassSET_Blend(TRUE, D3DBLEND_SRCALPHA, D3DBLEND_ONE, TRUE, oAREF.value);
        break;
    case 6: // MUL_2X + A-test
        C.PassSET_Blend(TRUE, D3DBLEND_DESTCOLOR, D3DBLEND_SRCCOLOR, FALSE, oAREF.value);
        break;
    case 7: // SET (2r)
        C.PassSET_Blend(TRUE, D3DBLEND_ONE, D3DBLEND_ZERO, TRUE, 0);
        break;
    case 8: // BLEND (2r)
        C.PassSET_Blend(TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, TRUE, oAREF.value);
        break;
    case 9: // BLEND (2r)
        C.PassSET_Blend(TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, TRUE, oAREF.value);
        break;
    }

    C.PassSET_LightFog(oLighting.value, oFog.value);

    C.r_End();
}


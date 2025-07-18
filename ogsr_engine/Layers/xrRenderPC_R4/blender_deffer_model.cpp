#include "stdafx.h"


#include "../xrRender/uber_deffer.h"
#include "Blender_deffer_model.h"

CBlender_deffer_model::CBlender_deffer_model()
{
    description.CLS = B_MODEL;
    description.version = 2;
    oTessellation.Count = 4;
    oTessellation.IDselected = 0;
    oAREF.value = 32;
    oAREF.min = 0;
    oAREF.max = 255;
    oBlend.value = FALSE;
}
CBlender_deffer_model::~CBlender_deffer_model() {}

void CBlender_deffer_model::Save(IWriter& fs)
{
    IBlenderXr::Save(fs);

    xrPWRITE_PROP(fs, "Use alpha-channel", xrPID_BOOL, oBlend);
    xrPWRITE_PROP(fs, "Alpha ref", xrPID_INTEGER, oAREF);
    xrPWRITE_PROP(fs, "Tessellation", xrPID_TOKEN, oTessellation);

    xrP_TOKEN::Item I;
    I.ID = 0;
    xr_strcpy(I.str, "NO_TESS");
    fs.w(&I, sizeof(I));
    I.ID = 1;
    xr_strcpy(I.str, "TESS_PN");
    fs.w(&I, sizeof(I));
    I.ID = 2;
    xr_strcpy(I.str, "TESS_HM");
    fs.w(&I, sizeof(I));
    I.ID = 3;
    xr_strcpy(I.str, "TESS_PN+HM");
    fs.w(&I, sizeof(I));
}

void CBlender_deffer_model::Load(IReader& fs, u16 version)
{
    IBlenderXr::Load(fs, version);

    switch (version)
    {
    case 0:
        oAREF.value = 32;
        oAREF.min = 0;
        oAREF.max = 255;
        oBlend.value = FALSE;
        break;
    case 1:
    default:
        xrPREAD_PROP(fs, xrPID_BOOL, oBlend);
        xrPREAD_PROP(fs, xrPID_INTEGER, oAREF);
        break;
    }
    if (version > 1)
    {
        xrPREAD_PROP(fs, xrPID_TOKEN, oTessellation);
    }
}

void CBlender_deffer_model::SaveIni(CInifile* ini_file, LPCSTR section)
{
    IBlenderXr::SaveIni(ini_file, section);

    WriteBool(ini_file, section, "alpha_channel", oBlend);
    WriteInteger(ini_file, section, "alpha_ref", oAREF);
    WriteToken(ini_file, section, "tessellation", oTessellation);
}

void CBlender_deffer_model::LoadIni(CInifile* ini_file, LPCSTR section)
{
    IBlenderXr::LoadIni(ini_file, section);

    ReadBool(ini_file, section, "alpha_channel", oBlend);
    ReadInteger(ini_file, section, "alpha_ref", oAREF);
    ReadToken(ini_file, section, "tessellation", oTessellation);
}

void CBlender_deffer_model::Compile(CBlender_Compile& C)
{
    IBlender::Compile(C);

    BOOL bForward = FALSE;
    if (oBlend.value && oAREF.value < 16)
        bForward = TRUE;
    if (oStrictSorting.value)
        bForward = TRUE;

    if (bForward)
    {
        // forward rendering
        LPCSTR vsname, psname;
        switch (C.iElement)
        {
        case 0: //
        case 1: //
            vsname = psname = "model_def_lq";
            C.r_Pass(vsname, psname, TRUE, TRUE, FALSE, TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, TRUE, oAREF.value);
            // C.r_Sampler			("s_base",	C.L_textures[0]);
            C.r_dx10Texture("s_base", C.L_textures[0]);
            C.r_dx10Sampler("smp_base");
            C.r_End();
            break;
        default: break;
        }
    }
    else
    {
        const BOOL bAref = oBlend.value;
        // deferred rendering
        // codepath is the same, only the shaders differ

        C.TessMethod = oTessellation.IDselected;
        switch (C.iElement)
        {
        case SE_R2_NORMAL_HQ: // deffer

            if (C.HudElement)
            {
                //Msg("--[%s] Detected hud element: [%s]", __FUNCTION__, C.L_textures[0].c_str());
                uber_deffer(C, true, "model_hud", "base_hud", bAref, nullptr, true);
                C.r_dx10Texture("s_hud_rain", "fx\\hud_rain");
            }
            else
            {
                uber_deffer(C, true, "model", "base", bAref, nullptr, true);
            }
            C.r_Stencil(TRUE, D3DCMP_ALWAYS, 0xff, 0x7f, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
            C.r_StencilRef(0x01);
            C.r_End();
            break;
        case SE_R2_NORMAL_LQ: // deffer

            uber_deffer(C, false, "model", "base", bAref, nullptr, true);
            C.r_Stencil(TRUE, D3DCMP_ALWAYS, 0xff, 0x7f, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
            C.r_StencilRef(0x01);
            C.r_End();
            break;
        case SE_R2_SHADOW: // smap
            if (bAref)
            {
                C.r_Pass("shadow_direct_model_aref", "shadow_direct_base_aref", FALSE, TRUE, TRUE, FALSE, D3DBLEND_ZERO, D3DBLEND_ONE, TRUE, 220);
                C.r_dx10Texture("s_base", C.L_textures[0]);
                C.r_dx10Sampler("smp_base");
                C.r_dx10Sampler("smp_linear");
                C.r_ColorWriteEnable(false, false, false, false);
                C.r_End();
                break;
            }
            else
            {
                C.r_Pass("shadow_direct_model", "dumb", FALSE, TRUE, TRUE, FALSE);
                C.r_dx10Texture("s_base", C.L_textures[0]);
                C.r_dx10Sampler("smp_base");
                C.r_dx10Sampler("smp_linear");
                C.r_ColorWriteEnable(false, false, false, false);
                C.r_End();
                break;
            }
        }
    }
}

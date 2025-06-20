// BlenderDefault.cpp: implementation of the CBlender_Model_EbB class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"


#include "blender_Model_EbB.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Model_EbB::CBlender_Model_EbB()
{
    description.CLS = B_MODEL_EbB;
    description.version = 0x1;
    xr_strcpy(oT2_Name, "$null");
    oBlend.value = FALSE;
}

CBlender_Model_EbB::~CBlender_Model_EbB() {}

void CBlender_Model_EbB::Save(IWriter& fs)
{
    //description.version = 0x1;
    IBlenderXr::Save(fs);
    xrPWRITE_MARKER(fs, "Environment map");
    xrPWRITE_PROP(fs, "Name", xrPID_TEXTURE, oT2_Name);
    string64 oT2_xform; // xform for secondary texture
    xrPWRITE_PROP(fs, "Transform", xrPID_MATRIX, oT2_xform);
    xrPWRITE_PROP(fs, "Alpha-Blend", xrPID_BOOL, oBlend);
}

void CBlender_Model_EbB::Load(IReader& fs, u16 version)
{
    IBlenderXr::Load(fs, version);
    xrPREAD_MARKER(fs);
    xrPREAD_PROP(fs, xrPID_TEXTURE, oT2_Name);
    string64 oT2_xform; // xform for secondary texture
    xrPREAD_PROP(fs, xrPID_MATRIX, oT2_xform);
    if (version >= 0x1)
    {
        xrPREAD_PROP(fs, xrPID_BOOL, oBlend);
    }
}

void CBlender_Model_EbB::SaveIni(CInifile* ini_file, LPCSTR section)
{
    IBlenderXr::SaveIni(ini_file, section);

    ini_file->w_string(section, "detail_name", oT2_Name);

    WriteBool(ini_file, section, "alpha_blend", oBlend);
}

void CBlender_Model_EbB::LoadIni(CInifile* ini_file, LPCSTR section)
{
    IBlenderXr::LoadIni(ini_file, section);

    strcpy_s(oT2_Name, ini_file->r_string(section, "detail_name"));

    ReadBool(ini_file, section, "alpha_blend", oBlend);
}

#include "uber_deffer.h"

void CBlender_Model_EbB::Compile(CBlender_Compile& C)
{
    IBlender::Compile(C);

    if (oBlend.value)
    {
        // forward
        LPCSTR vsname = nullptr;
        LPCSTR psname = nullptr;
        switch (C.iElement)
        {
        case 0:
        case 1:
            vsname = psname = "model_env_lq";
            C.r_Pass(vsname, psname, TRUE, TRUE, FALSE, TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, TRUE, 0);
            // C.r_Sampler			("s_base",	C.L_textures[0]);
            // C.r_Sampler			("s_env",	oT2_Name,false,D3DTADDRESS_CLAMP);
            C.r_dx10Texture("s_base", C.L_textures[0]);
            C.r_dx10Texture("s_env", oT2_Name);

            C.r_dx10Sampler("smp_base");
            C.r_dx10Sampler("smp_rtlinear");
            C.r_End();
            break;
        }
    }
    else
    {
        // deferred
        switch (C.iElement)
        {
        case SE_R2_NORMAL_HQ: // deffer
            if (C.HudElement)
            {
                //Msg("--[%s] Detected hud element: [%s]", __FUNCTION__, C.L_textures[0].c_str());
                uber_deffer(C, true, "model_hud", "base_hud", false, nullptr, true);
                C.r_dx10Texture("s_hud_rain", "fx\\hud_rain");
            }
            else
            {
                uber_deffer(C, true, "model", "base", false, nullptr, true);
            }
            C.r_Stencil(TRUE, D3DCMP_ALWAYS, 0xff, 0x7f, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
            C.r_StencilRef(0x01);
            C.r_End();
            break;
        case SE_R2_NORMAL_LQ: // deffer
            uber_deffer(C, false, "model", "base", false, nullptr, true);
            C.r_Stencil(TRUE, D3DCMP_ALWAYS, 0xff, 0x7f, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
            C.r_StencilRef(0x01);
            C.r_End();
            break;
        case SE_R2_SHADOW: // smap
            C.r_Pass("shadow_direct_model", "dumb", FALSE, TRUE, TRUE, FALSE);
            // C.r_Sampler		("s_base",C.L_textures[0]);
            C.r_dx10Texture("s_base", C.L_textures[0]);
            C.r_dx10Sampler("smp_base");
            C.r_dx10Sampler("smp_linear");
            C.r_ColorWriteEnable(false, false, false, false);
            C.r_End();
            break;
        }
    }
}
// Blender_Vertex_aref.cpp: implementation of the CBlender_Tree class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Blender_tree.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Tree::CBlender_Tree()
{
    description.CLS = B_TREE;
    description.version = 1;
    oBlend.value = FALSE;
    oNotAnTree.value = FALSE;
}

CBlender_Tree::~CBlender_Tree() {}

void CBlender_Tree::Save(IWriter& fs)
{
    IBlenderXr::Save(fs);
    xrPWRITE_PROP(fs, "Alpha-blend", xrPID_BOOL, oBlend);
    xrPWRITE_PROP(fs, "Object LOD", xrPID_BOOL, oNotAnTree);
}

void CBlender_Tree::Load(IReader& fs, u16 version)
{
    IBlenderXr::Load(fs, version);
    xrPREAD_PROP(fs, xrPID_BOOL, oBlend);
    if (version >= 1)
    {
        xrPREAD_PROP(fs, xrPID_BOOL, oNotAnTree);
    }
}

void CBlender_Tree::SaveIni(CInifile* ini_file, LPCSTR section)
{
    IBlenderXr::SaveIni(ini_file, section);

    WriteBool(ini_file, section, "alpha_blend", oBlend);
    WriteBool(ini_file, section, "object_lod", oNotAnTree);
}

void CBlender_Tree::LoadIni(CInifile* ini_file, LPCSTR section)
{
    IBlenderXr::LoadIni(ini_file, section);

    ReadBool(ini_file, section, "alpha_blend", oBlend);
    ReadBool(ini_file, section, "object_lod", oNotAnTree);
}


//////////////////////////////////////////////////////////////////////////
// R3
//////////////////////////////////////////////////////////////////////////
#include "uber_deffer.h"
void CBlender_Tree::Compile(CBlender_Compile& C)
{
    IBlender::Compile(C);

    //*************** codepath is the same, only shaders differ
    LPCSTR tvs;
    LPCSTR tvs_s;
    if (oNotAnTree.value)
    {
        tvs = "tree_s";
        if (oBlend.value)
            tvs_s = "shadow_direct_tree_s_aref";
        else
            tvs_s = "shadow_direct_tree_s";
    }
    else
    {
        tvs = "tree";
        if (oBlend.value)
            tvs_s = "shadow_direct_tree_aref";
        else
            tvs_s = "shadow_direct_tree";
    }

    switch (C.iElement)
    {
    case SE_R2_NORMAL_HQ: // deffer

        // Is a branch/bush. Use a different VS
        if (oBlend.value && !oNotAnTree.value)
        {
            tvs = "tree_branch";
            /*RImplementation.addShaderOption("USE_HAT", "1");
            Render->HAT = true;*/
        }

        uber_deffer(C, true, tvs, "base", oBlend.value, nullptr, true);
        C.r_Stencil(TRUE, D3DCMP_ALWAYS, 0xff, 0x7f, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
        C.r_StencilRef(0x01);
        C.r_dx10Texture("s_waves", "fx\\wind_wave");
        C.r_dx10Sampler("smp_linear2");
        C.r_End();

        break;
    case SE_R2_NORMAL_LQ: // deffer

        uber_deffer(C, false, tvs, "base", oBlend.value, nullptr, true);
        C.r_Stencil(TRUE, D3DCMP_ALWAYS, 0xff, 0x7f, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
        C.r_StencilRef(0x01);
        //	Need only for ATOC to emulate stencil test
        C.r_End();
        break;
    case SE_R2_SHADOW: // smap-spot
        if (oBlend.value)
        {
            /*RImplementation.addShaderOption("USE_HAT", "1");
            Render->HAT = true;*/
            C.r_Pass(tvs_s, "shadow_direct_base_aref", FALSE, TRUE, TRUE, TRUE, D3DBLEND_ZERO, D3DBLEND_ONE, TRUE, 200);
        }
        else
        {
            C.r_Pass(tvs_s, "shadow_direct_base", FALSE);
        }
        C.r_dx10Texture("s_base", C.L_textures[0]);
        C.r_dx10Sampler("smp_base");
        C.r_dx10Sampler("smp_linear");
        C.r_dx10Texture("s_waves", "fx\\wind_wave");
        C.r_dx10Sampler("smp_linear2");
        C.r_ColorWriteEnable(false, false, false, false);
        C.r_End();
        break;
    }

    //RImplementation.clearAllShaderOptions();
    //Render->HAT = false;
}
// Blender_Vertex_aref.cpp: implementation of the CBlender_Detail_Still class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "Blender_Detail_still.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Detail_Still::CBlender_Detail_Still()
{
    description.CLS = B_DETAIL;
    description.version = 0;
}

CBlender_Detail_Still::~CBlender_Detail_Still() {}

void CBlender_Detail_Still::Save(IWriter& fs)
{
    IBlender::Save(fs);
    xrPWRITE_PROP(fs, "Alpha-blend", xrPID_BOOL, oBlend);
}

void CBlender_Detail_Still::Load(IReader& fs, u16 version)
{
    IBlender::Load(fs, version);
    xrPREAD_PROP(fs, xrPID_BOOL, oBlend);
}


//////////////////////////////////////////////////////////////////////////
// R3
//////////////////////////////////////////////////////////////////////////
#include "uber_deffer.h"
void CBlender_Detail_Still::Compile(CBlender_Compile& C)
{
    IBlender::Compile(C);

    bool bUseATOC = (RImplementation.o.dx10_msaa_alphatest == CRender::MSAA_ATEST_DX10_0_ATOC);

    switch (C.iElement)
    {
    case SE_R2_NORMAL_HQ: // deffer wave
        if (bUseATOC)
        {
            uber_deffer(C, false, "detail_w", "base_atoc", true, 0, true);
            C.r_Stencil(TRUE, D3DCMP_ALWAYS, 0xff, 0x7f, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
            C.r_StencilRef(0x01);
            C.r_ColorWriteEnable(false, false, false, false);
            C.r_CullMode(D3DCULL_NONE);
            //	Alpha to coverage.
            C.RS.SetRS(XRDX10RS_ALPHATOCOVERAGE, TRUE);
            C.r_End();
        }

        uber_deffer(C, false, "detail_w", "base", true, 0, true);
        C.r_Stencil(TRUE, D3DCMP_ALWAYS, 0xff, 0x7f, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
        C.r_StencilRef(0x01);
        C.r_CullMode(D3DCULL_NONE);
        if (bUseATOC)
            C.RS.SetRS(D3DRS_ZFUNC, D3DCMP_EQUAL);
        C.r_End();
        break;
    case SE_R2_NORMAL_LQ: // deffer still
        if (bUseATOC)
        {
            uber_deffer(C, false, "detail_s", "base_atoc", true, 0, true);
            C.r_Stencil(TRUE, D3DCMP_ALWAYS, 0xff, 0x7f, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
            C.r_StencilRef(0x01);
            C.r_CullMode(D3DCULL_NONE);
            C.r_ColorWriteEnable(false, false, false, false);
            //	Alpha to coverage.
            C.RS.SetRS(XRDX10RS_ALPHATOCOVERAGE, TRUE);
            C.r_End();
        }

        uber_deffer(C, false, "detail_s", "base", true, 0, true);
        C.r_Stencil(TRUE, D3DCMP_ALWAYS, 0xff, 0x7f, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
        C.r_StencilRef(0x01);
        C.r_CullMode(D3DCULL_NONE);
        //	Need this for ATOC
        if (bUseATOC)
            C.RS.SetRS(D3DRS_ZFUNC, D3DCMP_EQUAL);
        C.r_End();
        break;
    }
}

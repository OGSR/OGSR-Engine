#include "stdafx.h"


#include "Blender_light_mask.h"

CBlender_accum_direct_mask::CBlender_accum_direct_mask() { description.CLS = 0; }
CBlender_accum_direct_mask::~CBlender_accum_direct_mask() {}

//	TODO: DX10:	implement CBlender_accum_direct_mask::Compile
void CBlender_accum_direct_mask::Compile(CBlender_Compile& C)
{
    IBlender::Compile(C);

    switch (C.iElement)
    {
    case SE_MASK_SPOT: // spot or omni-part
        C.r_Pass("accum_mask", "dumb", false, TRUE, FALSE);
        // C.r_Sampler_rtf		("s_position",		r2_RT_P);	//	???
        // C.r_dx10Texture		("s_position",		r2_RT_P);	//	???
        // C.r_dx10Sampler		("smp_nofilter");	//	???
        C.r_ColorWriteEnable(false, false, false, false);
        C.r_End();
        break;
    case SE_MASK_POINT: // point
        C.r_Pass("accum_mask", "dumb", false, TRUE, FALSE);
        // C.r_Sampler_rtf		("s_position",		r2_RT_P);	//	???
        // C.r_dx10Texture		("s_position",		r2_RT_P);	//	???
        // C.r_dx10Sampler		("smp_nofilter");	//	???
        C.r_ColorWriteEnable(false, false, false, false);
        C.r_End();
        break;
    case SE_MASK_DIRECT: // stencil mask for directional light
        //	FVF::F_TL
        // C.r_Pass			("null",			"accum_sun_mask",	false,	FALSE,FALSE,TRUE,D3DBLEND_ZERO,D3DBLEND_ONE,TRUE,1);
        // C.r_Pass			("stub_notransform","accum_sun_mask",	false,	FALSE,FALSE,TRUE,D3DBLEND_ZERO,D3DBLEND_ONE,TRUE,1);
        C.r_Pass("stub_notransform_t", "accum_sun_mask_nomsaa", false, FALSE, FALSE, TRUE, D3DBLEND_ZERO, D3DBLEND_ONE, TRUE, 1);
        C.r_dx10Texture("s_position", r2_RT_P);
        C.r_dx10Texture("s_diffuse", r2_RT_albedo);
        C.r_dx10Sampler("smp_nofilter");
        C.r_ColorWriteEnable(false, false, false, false);
        C.r_End();
        break;
    }
}

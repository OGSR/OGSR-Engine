#include "stdafx.h"
#pragma hdrstop

#include "blender_aa.h"

CBlender_aa::CBlender_aa	()	{	description.CLS		= 0;	}
CBlender_aa::~CBlender_aa	()	{	}
 
void	CBlender_aa::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	switch (C.iElement)
	{
	case 0:	// FXAA
		C.r_Pass			("null",			"ogse_fxaa",		FALSE,	FALSE,	FALSE);
		C.r_Sampler_clf		("s_image",			r2_RT_generic0);
		C.r_End				();
		break;
	case 1:	// luma calculating for FXAA
		C.r_Pass			("null",			"ogse_calc_luma",		FALSE,	FALSE,	FALSE);
		C.r_Sampler_clf		("s_image",			r2_RT_generic0);
		C.r_End				();
		break;
	case 2:	// SMAA edge detection
		C.r_Pass			("null",			"ogse_smaa_edge_detect",		FALSE,	FALSE,	FALSE);
		C.r_Sampler			("s_image",			r2_RT_generic0, false, D3DTADDRESS_CLAMP, D3DTEXF_LINEAR, D3DTEXF_POINT, D3DTEXF_LINEAR);
		C.r_End				();
		break;
	case 3:	// SMAA blending weight calculation
		C.r_Pass			("null",			"ogse_smaa_bweight_calc",		FALSE,	FALSE,	FALSE);
		C.r_Sampler			("s_edgetex",		r2_RT_smaa_edgetex, false, D3DTADDRESS_CLAMP);
		C.r_Sampler			("s_areatex",		"ogse\\antialiasing\\AreaTexDX9", false, D3DTADDRESS_CLAMP);		// bilinear filtering
		C.r_Sampler			("s_searchtex",		"ogse\\antialiasing\\SearchTex", false, D3DTADDRESS_CLAMP, D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_POINT);		// point filtering
		C.r_End				();
		break;
	case 4:	// SMAA neighourhood blending
		C.r_Pass			("null",			"ogse_smaa_neighbour_blend",		FALSE,	FALSE,	FALSE);
		C.r_Sampler_clf		("s_image",			r2_RT_generic0);
		C.r_Sampler			("s_blendtex",		r2_RT_smaa_blendtex, false, D3DTADDRESS_CLAMP);
		C.r_End				();
		break;
	case 5:	// Temporal Reprojection
		C.r_Pass			("null",			"ogse_temporal_reprojection",		FALSE,	FALSE,	FALSE);
		C.r_Sampler_clf		("s_image_curr",	r2_RT_generic0);
		C.r_Sampler_clf		("s_image_prev",	r2_RT_prev_frame0);
		C.r_Sampler_rtf		("s_position",		r2_RT_P);
		C.r_End				();
		break;
	}
}
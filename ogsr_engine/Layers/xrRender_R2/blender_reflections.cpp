#include "stdafx.h"
#pragma hdrstop

#include "blender_reflections.h"

CBlender_reflections::CBlender_reflections	()	{	description.CLS		= 0;	}
CBlender_reflections::~CBlender_reflections	()	{	}
 
void	CBlender_reflections::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	switch (C.iElement)
	{
	case 0:	
		C.r_Pass			("null",			"ogse_wet_reflections",		FALSE,	FALSE,	FALSE);
		C.r_Sampler_rtf		("s_position",		r2_RT_P			);
		C.r_Sampler_rtf		("s_normal",		r2_RT_N			);
		C.r_Sampler_clf		("s_image",			r2_RT_generic0	);
		C.r_Sampler_clf		("s_env0",			"$user$sky0");
		C.r_Sampler_clf		("s_env1",			"$user$sky1");
		C.r_End				();
		break;
	case 1:
		C.r_Pass			("null",			"ogse_apply_postprocess",		FALSE,	FALSE,	FALSE, TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
		C.r_Sampler_clf		("s_pp",			r2_RT_reflections);
		C.r_End				();
		break;
	case 2:
		C.r_Pass			("null",			"ogse_apply_postprocess",		FALSE,	FALSE,	FALSE, TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
		C.r_Sampler_clf		("s_pp",			r2_RT_rain);
		C.r_End				();
		break;
	case 3:	// vertical blur
		C.r_Pass			("null",			"ogse_vert_blur",		FALSE,	FALSE,	FALSE);
		C.r_Sampler_clf		("s_image",			r2_RT_rain);
		C.r_End				();
		break;
	case 4:	// horizontal blur
		C.r_Pass			("null",			"ogse_horiz_blur",		FALSE,	FALSE,	FALSE);
		C.r_Sampler_clf		("s_blur_temp",		r2_RT_blur_temp);
		C.r_End				();
		break;
/*	case 5:	// preprocess for reflections
		C.r_Pass			("null",			"ogse_reflections_preprocess",		FALSE,	FALSE,	FALSE);
		C.r_Sampler_rtf		("s_position",		r2_RT_P			);
		C.r_End				();
		break;*/
	}
}
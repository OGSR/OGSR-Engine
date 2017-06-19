#include "stdafx.h"
#pragma hdrstop

#include "blender_dof.h"

CBlender_dof::CBlender_dof	()	{	description.CLS		= 0;	}
CBlender_dof::~CBlender_dof	()	{	}
 
void	CBlender_dof::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	switch (C.iElement)
	{
	case 0:	// dof generation
		C.r_Pass			("null",			"ogse_dof",		FALSE,	FALSE,	FALSE);
		C.r_Sampler_rtf		("s_position",		r2_RT_P			);
		C.r_Sampler_clf		("s_image",			r2_RT_generic0);
		C.r_Sampler_clf		("s_blur",			r2_RT_blur);
		C.r_End				();
		break;
	case 1:
		C.r_Pass			("null",			"ogse_apply_postprocess",		FALSE,	FALSE,	FALSE, TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
		C.r_Sampler_clf		("s_pp",			r2_RT_dof);
		C.r_End				();
		break;
	}
}
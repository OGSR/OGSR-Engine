#pragma once
#include "stdafx.h"

#include "blender_blur.h"

CBlender_blur::CBlender_blur	()	{	description.CLS		= 0;	}
CBlender_blur::~CBlender_blur	()	{	}
 
void	CBlender_blur::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	switch (C.iElement)
	{
	case 0:	// vertical blur
		C.r_Pass			("null",			"ogse_vert_blur",		FALSE,	FALSE,	FALSE);
		C.r_Sampler_clf		("s_image",			r2_RT_generic0);
		C.r_End				();
		break;
	case 1:	// horizontal blur
		C.r_Pass			("null",			"ogse_horiz_blur",		FALSE,	FALSE,	FALSE);
		C.r_Sampler_clf		("s_blur_temp",		r2_RT_blur_temp);
		C.r_End				();
		break;
	}
}
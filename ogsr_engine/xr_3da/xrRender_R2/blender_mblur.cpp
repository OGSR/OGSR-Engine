#include "stdafx.h"
#pragma hdrstop

#include "blender_mblur.h"

CBlender_mblur::CBlender_mblur	()	{	description.CLS		= 0;	}
CBlender_mblur::~CBlender_mblur	()	{	}
 
void	CBlender_mblur::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	switch (C.iElement)
	{
	case 0:	// mblur
		C.r_Pass			("null",			"ogse_motion_blur",		FALSE,	FALSE,	FALSE);
		C.r_Sampler_rtf		("s_position",		r2_RT_P			);
		C.r_Sampler_clf		("s_current",		r2_RT_generic0);
	//	C.r_Sampler_clf		("s_previous",		r2_RT_prev_frame0);
		C.r_End				();
		break;
	case 1:	// save_last frame
/*		C.r_Pass			("null",			"ogse_copy_frame",		FALSE,	FALSE,	FALSE);
		C.r_Sampler_clf		("s_image",			r2_RT_generic0);
		C.r_End				();
		break;*/
	case 2:
	case 3:
	case 4:
	case 5:
		break;
	}
}
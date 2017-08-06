#include "stdafx.h"
#pragma hdrstop

#include "blender_rmap.h"

CBlender_rmap::CBlender_rmap	()	{	description.CLS		= 0;	}
CBlender_rmap::~CBlender_rmap	()	{	}

void	CBlender_rmap::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	BOOL	b_HW_smap		= RImplementation.o.HW_smap;
	BOOL	b_HW_PCF		= RImplementation.o.HW_smap_PCF;
	BOOL		blend		= RImplementation.o.fp16_blend;
	D3DBLEND	dest		= blend?D3DBLEND_ONE:D3DBLEND_ZERO;

	switch (C.iElement)
	{
	
	case 0:		// near pass - enable Z-test to perform depth-clipping
		C.r_Pass			("null",			"ogse_rain_map_near",	false,	TRUE,	FALSE,blend,D3DBLEND_ONE,dest);
		C.PassSET_ZB		(TRUE,FALSE,TRUE	);	// force inverted Z-Buffer
		C.r_Sampler_rtf		("s_position",		r2_RT_P			);
		C.r_Sampler_rtf		("s_normal",		r2_RT_N			);
//		C.r_Sampler_rtf		("s_accumulator",	r2_RT_rmap		);
		if (b_HW_smap)		{
			if (b_HW_PCF)	C.r_Sampler_clf		("s_rmap",r2_RT_rmap_depth	);
			else			{
				C.r_Sampler_rtf		("s_rmap",r2_RT_rmap_depth	);
			}
		}
		else				C.r_Sampler_rtf		("s_rmap",r2_RT_rmap_surf	);
		jitter				(C);
		C.r_End				();
		break;
	case 1:		// far pass, only stencil clipping performed
		C.r_Pass			("null",			"ogse_rain_map_far",	false,	TRUE,	FALSE,blend,D3DBLEND_ONE,dest);
		C.r_Sampler_rtf		("s_position",		r2_RT_P			);
		C.r_Sampler_rtf		("s_accumulator",	r2_RT_rmap		);
		if (b_HW_smap)		{
			if (b_HW_PCF)	C.r_Sampler_clf		("s_rmap",r2_RT_rmap_depth	);
			else			C.r_Sampler_rtf		("s_rmap",r2_RT_rmap_depth	);
		}
		else				C.r_Sampler_rtf		("s_rmap",r2_RT_rmap_surf	);
		jitter				(C);
		C.r_End				();
		break;
	case 2:		// stencil mask for directional light
		C.r_Pass			("null",			"ogse_rain_map_mask",	false,	FALSE,FALSE,TRUE,D3DBLEND_ZERO,D3DBLEND_ONE,TRUE,1);
		C.r_Sampler_rtf		("s_normal",		r2_RT_N);
		C.r_End				();
		break;
	case 3:	// copy accumulator (temp -> real), 2D (usually after sun-blend)
		C.r_Pass			("null",			"copy",				false,	FALSE,FALSE);
		C.r_Sampler_rtf		("s_base",			r2_RT_rmap_temp	);
		C.r_End				();
		break;
	}
}

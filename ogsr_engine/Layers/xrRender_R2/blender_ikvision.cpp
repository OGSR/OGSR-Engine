#include "stdafx.h"
#pragma hdrstop

#include "blender_ikvision.h"

CBlender_ikvision::CBlender_ikvision	()	{	description.CLS		= 0;	}
CBlender_ikvision::~CBlender_ikvision	()	{	}
 
void	CBlender_ikvision::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	C.r_Pass			("null",		"ogse_ikvision",		FALSE,	FALSE,	FALSE);	//. MRT-blend?
	C.r_Sampler_rtf		("s_position",		r2_RT_P				);
	C.r_Sampler_clf		("s_distort",		r2_RT_generic1		);
	C.r_Sampler_clf		("s_image",			r2_RT_generic0		);
	C.r_Sampler			("s_jitter_0",		JITTER(0)			, FALSE, D3DTADDRESS_WRAP, D3DTEXF_POINT, D3DTEXF_NONE, D3DTEXF_POINT);
	C.r_Sampler			("s_jitter_1",		JITTER(1)			, FALSE, D3DTADDRESS_WRAP, D3DTEXF_POINT, D3DTEXF_NONE, D3DTEXF_POINT);
	C.r_Sampler			("s_jitter_5",		JITTER(5)			, FALSE, D3DTADDRESS_WRAP, D3DTEXF_POINT, D3DTEXF_NONE, D3DTEXF_POINT);
	C.r_End				();
}
#include "stdafx.h"
#pragma hdrstop

#include "blender_fog_volume.h"

CBlender_fog_volume::CBlender_fog_volume	()	{	description.CLS		= 0;	}
CBlender_fog_volume::~CBlender_fog_volume	()	{	}
 
void	CBlender_fog_volume::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	switch (C.iElement)
	{
	case 0:	// render back faces
		C.r_Pass			("ogse_fv_draw_faces",		"ogse_fv_draw_back_faces",		FALSE,	FALSE,	FALSE);
/*		C.r_Sampler			("s_jitter5",		JITTER(5), true, D3DTADDRESS_WRAP, D3DTEXF_POINT, D3DTEXF_NONE, D3DTEXF_POINT);
		C.r_Sampler			("s_jitter1",		JITTER(1), true, D3DTADDRESS_WRAP, D3DTEXF_POINT, D3DTEXF_NONE, D3DTEXF_POINT);
		C.r_Sampler			("s_jitter2",		JITTER(2), true, D3DTADDRESS_WRAP, D3DTEXF_POINT, D3DTEXF_NONE, D3DTEXF_POINT);*/
		C.r_End				();
		break;
	case 1:	// render front faces
		C.r_Pass			("ogse_fv_draw_faces",		"ogse_fv_draw_front_faces",		FALSE,	FALSE,	FALSE, TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
		C.r_End				();
		break;
	case 2:	// draw volume
		C.r_Pass			("null",			"ogse_fv_draw_volume",		FALSE,	FALSE,	FALSE, TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
		C.r_Sampler_rtf		("s_position",		r2_RT_P				);
		C.r_Sampler_clf		("s_image",			r2_RT_generic0		);
		C.r_Sampler_rtf		("s_faces",			r2_RT_FV_faces		);
	/*	C.r_Sampler_clf		("s_color",			r2_RT_FV_color		);
		C.r_Sampler_rtf		("s_tc",			r2_RT_FV_tc			);*/
		C.r_Sampler			("s_noise",			"ogse\\effects\\Noise3D"		);
		C.r_End				();
		break;
	case 3:
	case 4:
	case 5:
		break;
	}
}
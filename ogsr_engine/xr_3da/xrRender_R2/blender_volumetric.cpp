#include "stdafx.h"
#pragma hdrstop

#include "blender_volumetric.h"

CBlender_volumetric::CBlender_volumetric	()	{	description.CLS		= 0;	}
CBlender_volumetric::~CBlender_volumetric	()	{	}

void	CBlender_volumetric::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	BOOL		b_HW_smap	= RImplementation.o.HW_smap;
	BOOL		b_HW_PCF	= RImplementation.o.HW_smap_PCF;
	LPCSTR smap = r2_RT_smap_depth;
	if (C.iElement == SE_SUN_NEAR)
		smap = r2_RT_smap_depth_near;
	else if (C.iElement == SE_SUN_FAR)
		smap = r2_RT_smap_depth_far;

	switch (C.iElement)
	{
	case 0:			// near volume
		C.r_Pass			("accum_volumetric",			"accum_volumetric_sun_near",	false,	FALSE,	FALSE,TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
		C.r_Sampler_rtf		("s_position",	"$user$position");
		C.r_Sampler			("jitter0",		"$user$jitter_0", false, D3DTADDRESS_WRAP, D3DTEXF_NONE, D3DTEXF_NONE, D3DTEXF_NONE);
		if (b_HW_smap)		{
			if (b_HW_PCF)	C.r_Sampler_clf		("s_smap",smap	);
			else			C.r_Sampler_rtf		("s_smap",smap	);
		}
		else				C.r_Sampler_rtf		("s_smap",r2_RT_smap_surf	);
		C.r_End				();
		break;
	case 1:			// far volume
		C.r_Pass			("accum_volumetric",			"accum_volumetric_sun_far",	false,	FALSE,	FALSE,TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
		C.r_Sampler_rtf		("s_position",	"$user$position");
		C.r_Sampler			("jitter0",		"$user$jitter_0", false, D3DTADDRESS_WRAP, D3DTEXF_NONE, D3DTEXF_NONE, D3DTEXF_NONE);
		if (b_HW_smap)		{
			if (b_HW_PCF)	C.r_Sampler_clf		("s_smap",smap	);
			else			C.r_Sampler_rtf		("s_smap",smap	);
		}
		else				C.r_Sampler_rtf		("s_smap",r2_RT_smap_surf	);
		C.r_End				();
		break;
	case 2:			// volumetric lights
		C.r_Pass			("accum_volumetric",			"accum_volumetric",	false,	TRUE,	FALSE,TRUE, D3DBLEND_ONE, D3DBLEND_ONE);
		C.r_Sampler			("s_lmap",			C.L_textures[0],false,D3DTADDRESS_CLAMP);
		C.r_Sampler			("s_noise",		"fx\\fx_noise");
		if (b_HW_smap)		{
			if (b_HW_PCF)	C.r_Sampler_clf		("s_smap",r2_RT_smap_depth	);
			else			C.r_Sampler_rtf		("s_smap",r2_RT_smap_depth	);
		}
		else				C.r_Sampler_rtf		("s_smap",r2_RT_smap_surf	);
		C.r_End				();
		break;
	}
}
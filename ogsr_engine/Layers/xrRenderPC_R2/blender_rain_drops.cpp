#include "stdafx.h"

#include "blender_rain_drops.h"

CBlender_rain_drops::CBlender_rain_drops	()	{	description.CLS		= 0;	}
CBlender_rain_drops::~CBlender_rain_drops	()	{	}
 
void	CBlender_rain_drops::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	C.r_Pass			("null",			"ogse_rain_drops",		FALSE,	FALSE,	FALSE);
	C.r_Sampler_clf		("s_image",			r2_RT_generic0);
	C.r_End				();
}
#include "stdafx.h"
#pragma hdrstop

#include "blender_rain_drops.h"

CBlender_rain_drops::CBlender_rain_drops	()	{	description.CLS		= 0;	}
CBlender_rain_drops::~CBlender_rain_drops	()	{	}
 
void	CBlender_rain_drops::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	C.r_Pass			("null",			"ogse_rain_drops",		FALSE,	FALSE,	FALSE);
	C.r_Sampler_clf		("s_image",			r2_RT_generic0);
	C.r_Sampler_clf		("s_rain_drops",			"ogse\\effects\\rain_drops");
	C.r_Sampler_clf		("s_rain_drops_streaks",	"ogse\\effects\\rain_drops_streaks");
	C.r_Sampler_clf		("s_rain_drops_scroll",		"ogse\\effects\\rain_drops_scroll");
	C.r_End				();
}
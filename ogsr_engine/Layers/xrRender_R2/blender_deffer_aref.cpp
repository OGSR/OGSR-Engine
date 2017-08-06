#include "stdafx.h"
#pragma hdrstop

#include "..\xrRender\uber_deffer.h"
#include "Blender_deffer_aref.h"

CBlender_deffer_aref::CBlender_deffer_aref	(bool _lmapped) : lmapped(_lmapped)	{	
	description.CLS		= B_DEFAULT_AREF;
	oAREF.value			= 200;
	oAREF.min			= 0;
	oAREF.max			= 255;
	oBlend.value		= FALSE;
}
CBlender_deffer_aref::~CBlender_deffer_aref	()	{	}

void	CBlender_deffer_aref::Save	(	IWriter& fs )
{
	IBlender::Save	(fs);
	xrPWRITE_PROP	(fs,"Alpha ref",	xrPID_INTEGER,	oAREF);
	xrPWRITE_PROP	(fs,"Alpha-blend",	xrPID_BOOL,		oBlend);
}
void	CBlender_deffer_aref::Load	(	IReader& fs, u16 version )
{
	IBlender::Load	(fs,version);
	if (1==version)	{
		xrPREAD_PROP	(fs,xrPID_INTEGER,	oAREF);
		xrPREAD_PROP	(fs,xrPID_BOOL,		oBlend);
	}
}

void	CBlender_deffer_aref::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	// oBlend.value	= FALSE	;

	if (oBlend.value)	{
		switch(C.iElement) 
		{
		case SE_R2_NORMAL_HQ:
			C.SetParams(3, true);
			uber_forward(C, true, "base", "base", oAREF.value, false, 0, true);
			C.r_Sampler("s_lmap", r2_sunmask);
			C.r_Sampler_clf("s_smap_near", r2_RT_smap_depth_near);
			C.r_Sampler_clf("s_smap_far", r2_RT_smap_depth_far);
			C.r_Sampler_clf("env_s0", r2_T_envs0);
			C.r_Sampler_clf("env_s1", r2_T_envs1);
			C.r_Sampler_clf("s_sky", r2_RT_rain);
			C.r_Sampler_clw("s_material", r2_material);
			C.r_End();
			break;
		case SE_R2_NORMAL_LQ:
			C.SetParams(3, true);
			uber_forward(C, false, "base", "base", oAREF.value, false, 0, true);
			C.r_Sampler("s_lmap", r2_sunmask);
			C.r_Sampler_clf("s_smap_near", r2_RT_smap_depth_near);
			C.r_Sampler_clf("s_smap_far", r2_RT_smap_depth_far);
			C.r_Sampler_clf("env_s0", r2_T_envs0);
			C.r_Sampler_clf("env_s1", r2_T_envs1);
			C.r_Sampler_clf("s_sky", r2_RT_rain);
			C.r_Sampler_clw("s_material", r2_material);
			C.r_End();
			break;
		case SE_R2_SHADOW:		// smap
			C.SetParams(1, false);
			if (RImplementation.o.HW_smap)	C.r_Pass	("shadow_direct_base_aref","shadow_direct_base_aref",FALSE,TRUE,TRUE,FALSE,D3DBLEND_ZERO,D3DBLEND_ONE,TRUE,220);
			else							C.r_Pass	("shadow_direct_base_aref","shadow_direct_base_aref",FALSE);
			C.r_Sampler		("s_base",C.L_textures[0]);
			C.r_End			();
			break;
		default:
			break;
		}
	} else {
		C.SetParams				(1,false);	//.

		// codepath is the same, only the shaders differ
		// ***only pixel shaders differ***
		switch(C.iElement) 
		{
		case SE_R2_NORMAL_HQ: 	// deffer
			uber_deffer		(C,true,"base","base",true);
			break;
		case SE_R2_NORMAL_LQ: 	// deffer
			uber_deffer		(C,false,"base","base",true);
			break;
		case SE_R2_SHADOW:		// smap
			if (RImplementation.o.HW_smap)	C.r_Pass	("shadow_direct_base_aref","shadow_direct_base_aref",FALSE,TRUE,TRUE,FALSE,D3DBLEND_ZERO,D3DBLEND_ONE,TRUE,220);
			else							C.r_Pass	("shadow_direct_base_aref","shadow_direct_base_aref",FALSE);
			C.r_Sampler		("s_base",C.L_textures[0]);
			C.r_End			();
			break;
		}
	}
}

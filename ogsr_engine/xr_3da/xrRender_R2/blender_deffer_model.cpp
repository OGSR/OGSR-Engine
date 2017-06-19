#include "stdafx.h"
#pragma hdrstop

#include "..\xrRender\uber_deffer.h"
#include "Blender_deffer_model.h"

CBlender_deffer_model::CBlender_deffer_model	()	{	
	description.CLS		= B_MODEL;	
	description.version	= 1;
	oAREF.value			= 32;
	oAREF.min			= 0;
	oAREF.max			= 255;
	oBlend.value		= FALSE;
}
CBlender_deffer_model::~CBlender_deffer_model	()	{	}

void	CBlender_deffer_model::Save	(	IWriter& fs )
{
	IBlender::Save		(fs);
	xrPWRITE_PROP		(fs,"Use alpha-channel",	xrPID_BOOL,		oBlend);
	xrPWRITE_PROP		(fs,"Alpha ref",			xrPID_INTEGER,	oAREF);
}
void	CBlender_deffer_model::Load	(	IReader& fs, u16 version )
{
	IBlender::Load		(fs,version);

	switch (version)	
	{
	case 0: 
		oAREF.value			= 32;
		oAREF.min			= 0;
		oAREF.max			= 255;
		oBlend.value		= FALSE;
		break;
	case 1:
	default:
		xrPREAD_PROP	(fs,xrPID_BOOL,		oBlend);
		xrPREAD_PROP	(fs,xrPID_INTEGER,	oAREF);
		break;
	}
}

void	CBlender_deffer_model::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	BOOL	bForward = FALSE;
	if (oBlend.value && oAREF.value<16)	bForward = TRUE;
	if (oStrictSorting.value)			bForward = TRUE;

	if (bForward) {
		// forward rendering
		switch (C.iElement)
		{
		case SE_R2_NORMAL_HQ:
			C.SetParams(3, true);
			uber_forward(C, true, "model", "base", oAREF.value, false, 0, true);
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
			uber_forward(C, false, "model", "base", oAREF.value, false, 0, true);
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
			if (oAREF.value) {
				if (RImplementation.o.HW_smap)	C.r_Pass("shadow_direct_model_aref", "shadow_direct_base_aref", FALSE, TRUE, TRUE, FALSE, D3DBLEND_ZERO, D3DBLEND_ONE, TRUE, 200);
				else							C.r_Pass("shadow_direct_model_aref", "shadow_direct_base_aref", FALSE);
				C.r_Sampler("s_base", C.L_textures[0]);
				C.r_End();
				break;
			}
			else {
				if (RImplementation.o.HW_smap)	C.r_Pass("shadow_direct_model", "shadow_direct_base", FALSE, TRUE, TRUE, FALSE);
				else							C.r_Pass("shadow_direct_model", "shadow_direct_base", FALSE);
				C.r_Sampler("s_base", C.L_textures[0]);
				C.r_End();
				break;
			}
		default:
			break;
		}
	}
	else {
		BOOL	bAref = oBlend.value;
		// deferred rendering
		// codepath is the same, only the shaders differ
		switch (C.iElement)
		{
		case SE_R2_NORMAL_HQ: 			// deffer
			uber_deffer(C, true, "model", "base", bAref);
			break;
		case SE_R2_NORMAL_LQ: 			// deffer
			uber_deffer(C, false, "model", "base", bAref);
			break;
		case SE_R2_SHADOW:				// smap
			if (bAref) {
				if (RImplementation.o.HW_smap)	C.r_Pass("shadow_direct_model_aref", "shadow_direct_base_aref", FALSE, TRUE, TRUE, FALSE, D3DBLEND_ZERO, D3DBLEND_ONE, TRUE, 200);
				else							C.r_Pass("shadow_direct_model_aref", "shadow_direct_base_aref", FALSE);
				C.r_Sampler("s_base", C.L_textures[0]);
				C.r_End();
				break;
			}
			else {
				if (RImplementation.o.HW_smap)	C.r_Pass("shadow_direct_model", "shadow_direct_base", FALSE, TRUE, TRUE, FALSE);
				else							C.r_Pass("shadow_direct_model", "shadow_direct_base", FALSE);
				C.r_Sampler("s_base", C.L_textures[0]);
				C.r_End();
				break;
			}
		}
	}
}

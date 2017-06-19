// BlenderDefault.cpp: implementation of the CBlender_Model_EbB class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "blender_Model_EbB.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Model_EbB::CBlender_Model_EbB	()
{
	description.CLS		= B_MODEL_EbB;
	description.version	= 0x1;
	strcpy				(oT2_Name,	"$null");
	strcpy				(oT2_xform,	"$null");
	oBlend.value		= FALSE;
}

CBlender_Model_EbB::~CBlender_Model_EbB	()
{
	
}

void	CBlender_Model_EbB::Save(	IWriter& fs )
{
	description.version	= 0x1;
	IBlender::Save	(fs);
	xrPWRITE_MARKER	(fs,"Environment map");
	xrPWRITE_PROP	(fs,"Name",				xrPID_TEXTURE,	oT2_Name);
	xrPWRITE_PROP	(fs,"Transform",		xrPID_MATRIX,	oT2_xform);
	xrPWRITE_PROP	(fs,"Alpha-Blend",		xrPID_BOOL,		oBlend);
}

void	CBlender_Model_EbB::Load(	IReader& fs, u16 version )
{
	IBlender::Load	(fs,version);
	xrPREAD_MARKER	(fs);
	xrPREAD_PROP	(fs,xrPID_TEXTURE,	oT2_Name);
	xrPREAD_PROP	(fs,xrPID_MATRIX,	oT2_xform);
	if (version>=0x1)	{
		xrPREAD_PROP	(fs,xrPID_BOOL,	oBlend);
	}
}

#if RENDER==R_R1
void	CBlender_Model_EbB::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);
	if (C.bEditor)	{
		C.PassBegin		();
		{
			if (oBlend.value)	{ C.PassSET_ZB		(TRUE,FALSE);	C.PassSET_Blend_BLEND	(); }
			else				{ C.PassSET_ZB		(TRUE,TRUE);	C.PassSET_Blend_SET		(); }
			C.PassSET_LightFog	(TRUE,TRUE);
			
			// Stage1 - Env texture
			C.StageBegin		();
			C.StageSET_Address	(D3DTADDRESS_CLAMP);
			C.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_DIFFUSE);
			C.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_DIFFUSE);
			C.StageSET_TMC		(oT2_Name, oT2_xform, "$null", 0);
			C.StageEnd			();
			
			// Stage2 - Base texture
			C.StageBegin		();
			C.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_BLENDTEXTUREALPHA,	D3DTA_CURRENT);
			C.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,		D3DTA_CURRENT);
			C.StageSET_TMC		(oT_Name, oT_xform, "$null", 0);
			C.StageEnd			();

			// Stage3 - Lighting - should work on all 2tex hardware
			C.StageBegin		();
			C.StageSET_Color	(D3DTA_DIFFUSE,	  D3DTOP_MODULATE,			D3DTA_CURRENT);
			C.StageSET_Alpha	(D3DTA_DIFFUSE,	  D3DTOP_SELECTARG2,		D3DTA_CURRENT);
			C.Stage_Texture		("$null"	);
			C.Stage_Matrix		("$null",	0);
			C.Stage_Constant	("$null"	);
			C.StageEnd			();
		}
		C.PassEnd			();
	} else {
		LPCSTR	vsname			= 0;
		LPCSTR	psname			= 0;
		switch (C.iElement)
		{
		case SE_R1_NORMAL_HQ:	
			vsname = psname =	"model_env_hq"; 
			if (oBlend.value)	C.r_Pass	(vsname,psname,TRUE,TRUE,FALSE,TRUE,D3DBLEND_SRCALPHA,	D3DBLEND_INVSRCALPHA,	TRUE,0);
			else				C.r_Pass	(vsname,psname,TRUE);
			C.r_Sampler			("s_base",	C.L_textures[0]);
			C.r_Sampler			("s_env",	oT2_Name,false,D3DTADDRESS_CLAMP);
			C.r_Sampler_clf		("s_lmap",	"$user$projector",true);
			C.r_End				();
			break;
		case SE_R1_NORMAL_LQ:
			vsname = psname =	"model_env_lq"; 
			if (oBlend.value)	C.r_Pass	(vsname,psname,TRUE,TRUE,FALSE,TRUE,D3DBLEND_SRCALPHA,	D3DBLEND_INVSRCALPHA,	TRUE,0);
			else				C.r_Pass	(vsname,psname,TRUE);
			C.r_Sampler			("s_base",	C.L_textures[0]);
			C.r_Sampler			("s_env",	oT2_Name,false,D3DTADDRESS_CLAMP);
			C.r_End				();
			break;
		case SE_R1_LPOINT:
			vsname				= "model_def_point";
			psname				= "add_point";
			C.r_Pass			(vsname,psname,FALSE,TRUE,FALSE,TRUE,D3DBLEND_ONE, D3DBLEND_ONE,TRUE);
			C.r_Sampler			("s_base",	C.L_textures[0]);
			C.r_Sampler_clf		("s_lmap",	TEX_POINT_ATT);
			C.r_Sampler_clf		("s_att",	TEX_POINT_ATT);
			C.r_End				();
			break;
		case SE_R1_LSPOT:
			vsname				= "model_def_spot";
			psname				= "add_spot";
			C.r_Pass			(vsname,psname,FALSE,TRUE,FALSE,TRUE,D3DBLEND_ONE, D3DBLEND_ONE,TRUE);
			C.r_Sampler			("s_base",	C.L_textures[0]);
			C.r_Sampler_clf		("s_lmap",	"internal\\internal_light_att",		true);
			C.r_Sampler_clf		("s_att",	TEX_SPOT_ATT);
			C.r_End				();
			break;
		case SE_R1_LMODELS:
			vsname				= "model_def_shadow";
			psname				= "model_shadow";
			C.r_Pass			(vsname,psname,FALSE,FALSE,FALSE,TRUE,D3DBLEND_ZERO,D3DBLEND_SRCCOLOR,FALSE,0);
			C.r_End				();
			break;
		}
	}
}
#else
#include "uber_deffer.h"
void	CBlender_Model_EbB::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);

	if (oBlend.value) {
		switch (C.iElement)
		{
		case SE_R2_NORMAL_HQ:
			C.SetParams(3, true);
			uber_forward(C, true, "model", "base", 0, true, 0, true);
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
			uber_forward(C, false, "model", "base", 0, true, 0, true);
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
			if (RImplementation.o.HW_smap)	C.r_Pass("shadow_direct_base_aref", "shadow_direct_base_aref", FALSE, TRUE, TRUE, FALSE, D3DBLEND_ZERO, D3DBLEND_ONE, TRUE, 220);
			else							C.r_Pass("shadow_direct_base_aref", "shadow_direct_base_aref", FALSE);
			C.r_Sampler("s_base", C.L_textures[0]);
			C.r_End();
			break;
		default:
			break;
		}
	}
	else {
		// deferred
		switch (C.iElement)
		{
		case SE_R2_NORMAL_HQ: 	// deffer
			uber_deffer(C, true, "model", "base", false);
			break;
		case SE_R2_NORMAL_LQ: 	// deffer
			uber_deffer(C, false, "model", "base", false);
			break;
		case SE_R2_SHADOW:		// smap
			if (RImplementation.o.HW_smap)	C.r_Pass("shadow_direct_model", "shadow_direct_base", FALSE, TRUE, TRUE, FALSE);
			else							C.r_Pass("shadow_direct_model", "shadow_direct_base", FALSE);
			C.r_Sampler("s_base", C.L_textures[0]);
			C.r_End();
			break;
		}
	}
}
#endif

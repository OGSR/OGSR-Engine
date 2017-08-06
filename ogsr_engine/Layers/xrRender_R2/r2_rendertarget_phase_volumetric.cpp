#include "stdafx.h"
#include "..\xrRender\xrRender_console.h"

extern int reset_frame;

void	CRenderTarget::phase_accumulator_volumetric ()
{
	if (!rt_volumetric) return;
	if (dwVolumetricClearMark == Device.dwFrame)	{
		u_setrt	(rt_volumetric,		NULL,NULL,HW.pBaseZB);
	} else {
		u_setrt	(rt_volumetric,		NULL,NULL,HW.pBaseZB);
		u32		clr4clear					= color_rgba(0,0,0,0);	// 0x00
		CHK_DX	(HW.pDevice->Clear			( 0L, NULL, D3DCLEAR_TARGET, clr4clear, 1.0f, 0L));
		dwVolumetricClearMark = Device.dwFrame;
	}
	RCache.set_Stencil			(TRUE,D3DCMP_ALWAYS,dwLightMarkerID,0xff,0x00);
	RCache.set_CullMode			(CULL_NONE);
	RCache.set_ColorWriteEnable	(15);
/*	u_setrt	(rt_Accumulator,		NULL,NULL,HW.pBaseZB);
	RCache.set_Stencil			(TRUE,D3DCMP_ALWAYS,dwLightMarkerID,0xff,0x00);
	RCache.set_CullMode			(CULL_NONE);
	RCache.set_ColorWriteEnable	(D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);*/
}

void	CRenderTarget::accum_direct_volumetric (u32 sub_phase, Fmatrix& m_shadow)
{
	if (!rt_volumetric) return;
//	return;		// blocked
//	Device.Statistic->TEST2.Begin	();
//	if (reset_frame==Device.dwFrame || reset_frame==Device.dwFrame - 1)		return;
	if (g_pGamePersistent && g_pGamePersistent->m_pMainMenu && g_pGamePersistent->m_pMainMenu->IsActive())		return;
	CEnvDescriptorMixer *env = &(g_pGamePersistent->Environment().CurrentEnv);
	if (ps_r2_pp_flags.test(R2PP_FLAG_SUNSHAFTS) && (ps_sunshafts_mode == R2SS_VOLUMETRIC))
	{
		phase_accumulator_volumetric();
		if (env->sun_shafts > 0.001)
		{
			

			// *** assume accumulator setted up ***
			light*			fuckingsun			= (light*)RImplementation.Lights.sun_adapted._get()	;

			// Common calc for quad-rendering
			u32		Offset;
			u32		C					= color_rgba	(255,255,255,255);
			float	_w					= float			(Device.dwWidth);
			float	_h					= float			(Device.dwHeight);
			Fvector2					p0,p1;
			p0.set						(.5f/_w, .5f/_h);
			p1.set						((_w+.5f)/_w, (_h+.5f)/_h );
			float	d_Z	= EPS_S;

			// Common constants (light-related)
			Fvector		L_dir,L_clr;	float L_spec;
			L_clr.set					(fuckingsun->color.r,fuckingsun->color.g,fuckingsun->color.b);
			L_spec						= u_diffuse2s	(L_clr);
			Device.mView.transform_dir	(L_dir,fuckingsun->direction);
			L_dir.normalize				();

			// recalculate d_Z, to perform depth-clipping
			Fvector	center_pt;			center_pt.mad	(Device.vCameraPosition,Device.vCameraDirection,ps_r2_sun_near);
			Device.mFullTransform.transform(center_pt)	;
			d_Z							= center_pt.z	;

			// Perform lighting
			{
				// Make jitter texture
				Fvector2					j0,j1;
				float	scale_X				= float(Device.dwWidth)	/ float(TEX_jitter);
				float	offset				= (.5f / float(TEX_jitter));
				j0.set						(offset,offset);
				j1.set						(scale_X,scale_X).add(offset);

				// Fill vertex buffer
				FVF::TL2uv* pv				= (FVF::TL2uv*) RCache.Vertex.Lock	(4,g_combine_2UV->vb_stride,Offset);
				pv->set						(EPS,			float(_h+EPS),	d_Z,	1.f, C, p0.x, p1.y, j0.x, j1.y);	pv++;
				pv->set						(EPS,			EPS,			d_Z,	1.f, C, p0.x, p0.y, j0.x, j0.y);	pv++;
				pv->set						(float(_w+EPS),	float(_h+EPS),	d_Z,	1.f, C, p1.x, p1.y, j1.x, j1.y);	pv++;
				pv->set						(float(_w+EPS),	EPS,			d_Z,	1.f, C, p1.x, p0.y, j1.x, j0.y);	pv++;
				RCache.Vertex.Unlock		(4,g_combine_2UV->vb_stride);
				RCache.set_Geometry			(g_combine_2UV);

				// setup
				Fvector4 params = {0,0,0,0};
				params.x = env->sun_shafts;
				params.y = env->sun_shafts_length;
				params.z = ps_r2_sun_near;
	//			params.w = float(ps_sunshafts_quality);


		//		RCache.set_Shader			(s_volumetric);
				RCache.set_Element			(s_accum_volumetric->E[sub_phase]);
				RCache.set_c				("c_sunshafts",	params);
				RCache.set_c				("Ldynamic_dir",		L_dir.x,L_dir.y,L_dir.z, ps_r2_sun_near		);
				RCache.set_c				("Ldynamic_color",		L_clr.x,L_clr.y,L_clr.z, L_spec	);
				RCache.set_c				("m_shadow",			m_shadow						);
	/*			if (SE_SUN_NEAR==sub_phase)	{
					RCache.set_c				("debug_color",			1.0,0.0,0.0,1.0);
				} else {
					RCache.set_c				("debug_color",			0.0,0.0,1.0,1.0);
				}*/

				if (SE_SUN_FAR==sub_phase)	
				{
					// nv-DBT
					float zMin,zMax;
	//				extern float	OLES_SUN_LIMIT_27_01_07;
	/*				if (SE_SUN_NEAR==sub_phase)	{
						zMin = 0.01;
						zMax = ps_r2_sun_near;
					} else {*/
						zMin = ps_r2_sun_near;
						zMax = OLES_SUN_LIMIT_27_01_07;
	//				}
					center_pt.mad(Device.vCameraPosition,Device.vCameraDirection,zMin);	Device.mFullTransform.transform	(center_pt);
					zMin = center_pt.z	;

					center_pt.mad(Device.vCameraPosition,Device.vCameraDirection,zMax);	Device.mFullTransform.transform	(center_pt);
					zMax = center_pt.z	;

					if (u_DBT_enable(zMin,zMax))	{
						// z-test always
						HW.pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
						HW.pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
					}
				}

				// Fetch4 : enable
				if (RImplementation.o.HW_smap_FETCH4)	{
					//. we hacked the shader to force smap on S0
		#			define FOURCC_GET4  MAKEFOURCC('G','E','T','4') 
					HW.pDevice->SetSamplerState	( 0, D3DSAMP_MIPMAPLODBIAS, FOURCC_GET4 );
				}

				// setup stencil
				RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);

				// Fetch4 : disable
				if (RImplementation.o.HW_smap_FETCH4)	{
					//. we hacked the shader to force smap on S0
		#			define FOURCC_GET1  MAKEFOURCC('G','E','T','1') 
					HW.pDevice->SetSamplerState	( 0, D3DSAMP_MIPMAPLODBIAS, FOURCC_GET1 );
				}

				if (SE_SUN_FAR==sub_phase)	
					// disable depth bounds
					u_DBT_disable	();
			}
		}
	}
//	Device.Statistic->TEST2.End	();
}
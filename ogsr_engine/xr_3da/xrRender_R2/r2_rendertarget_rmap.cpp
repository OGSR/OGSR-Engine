#include "stdafx.h"
#include "..\xrRender\xrRender_console.h"

void	CRenderTarget::phase_rmap_direct		(u32 sub_phase)
{
	// Targets
/*	u_setrt	(rt_rain_map_surf, NULL, NULL, rt_rain_map_depth->pRT);

	// Clear
	if (SE_SUN_NEAR==sub_phase)			{
		D3DRECT		R;
		R.x1		= rain_caster.D.minX;
		R.x2		= rain_caster.D.maxX;
		R.y1		= rain_caster.D.minY;
		R.y2		= rain_caster.D.maxY;
		CHK_DX							(HW.pDevice->Clear( 1L, &R,	  D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,	0xFFFFFFFF, 1.0f, 0L));
	} else {
		// full-clear
		CHK_DX							(HW.pDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER,	0xFFFFFFFF, 1.0f, 0L));
	}

	// Stencil	- disable
	RCache.set_Stencil					( FALSE );

	// Misc		- draw only front/back-faces
	if (SE_SUN_NEAR==sub_phase)			RCache.set_CullMode			( CULL_CCW	);	// near
	else								RCache.set_CullMode			( CULL_CW	);	// far, reversed
	RCache.set_ColorWriteEnable	( FALSE		);*/
}

void	CRenderTarget::phase_rmap_direct_tsh	()
{
/*	VERIFY								(RImplementation.o.Tshadows);
	u32		_clr						= 0xffffffff;	//color_rgba(127,127,12,12);
	RCache.set_ColorWriteEnable			();
	CHK_DX								(HW.pDevice->Clear( 0L, NULL, D3DCLEAR_TARGET,	_clr,	1.0f, 0L));*/
}
void	CRenderTarget::phase_rmap_accumulator()
{
	// Targets
/*	if (dwRainTargetClearMark==Device.dwFrame)	{
		// normal operation - setup
	/*	if (RImplementation.o.fp16_blend)	u_setrt	(rt_rmap,		NULL,NULL,HW.pBaseZB);
		else								u_setrt	(rt_rmap_temp,	NULL,NULL,HW.pBaseZB);*//*
		u_setrt	(rt_rmap,		NULL,NULL,HW.pBaseZB);
	} else {
		// initial setup
		dwRainTargetClearMark				= Device.dwFrame;

		// clear
		u_setrt								(rt_rmap,		NULL,NULL,HW.pBaseZB);
		dwRainMarkerID						= 5;					// start from 5, increment in 2 units
		u32		clr4clear					= color_rgba(0,0,0,0);	// 0x00
		CHK_DX	(HW.pDevice->Clear			( 0L, NULL, D3DCLEAR_TARGET, clr4clear, 1.0f, 0L));

		// Render emissive geometry, stencil - write 0x0 at pixel pos
		RCache.set_xform_project					(Device.mProject); 
		RCache.set_xform_view						(Device.mView);
		// Stencil - write 0x1 at pixel pos - 
		RCache.set_Stencil							( TRUE,D3DCMP_ALWAYS,0x01,0xff,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE,D3DSTENCILOP_KEEP);
//		//RCache.set_Stencil						(TRUE,D3DCMP_ALWAYS,0x00,0xff,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE,D3DSTENCILOP_KEEP);
		RCache.set_CullMode							(CULL_CCW);
		RCache.set_ColorWriteEnable					();
		RImplementation.r_dsgraph_render_emissive	();

		// Stencil	- draw only where stencil >= 0x1
		RCache.set_Stencil					(TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0x00);
		RCache.set_CullMode					(CULL_NONE);
		RCache.set_ColorWriteEnable			();
	}*/
}

void	CRenderTarget::apply_rmap		(u32 sub_phase)
{
/*	// Choose normal code-path or filtered
	phase_rmap_accumulator					();

	// Common calc for quad-rendering
	u32		Offset;
	u32		C					= color_rgba	(255,255,255,255);
	float	_w					= float			(Device.dwWidth);
	float	_h					= float			(Device.dwHeight);
	Fvector2					p0,p1;
	p0.set						(.5f/_w, .5f/_h);
	p1.set						((_w+.5f)/_w, (_h+.5f)/_h );
	float	d_Z	= EPS_S, d_W = 1.f;

	// Common constants (light-related)
	Fvector		L_dir;
	Device.mView.transform_dir	(L_dir,rain_caster.direction);
	L_dir.normalize				();

	// Perform masking (only once - on the first/near phase)
	RCache.set_CullMode			(CULL_NONE	);
	if (SE_SUN_NEAR==sub_phase)	//.
	{
		// Fill vertex buffer
		FVF::TL* pv					= (FVF::TL*)	RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
		pv->set						(EPS,			float(_h+EPS),	d_Z,	d_W, C, p0.x, p1.y);	pv++;
		pv->set						(EPS,			EPS,			d_Z,	d_W, C, p0.x, p0.y);	pv++;
		pv->set						(float(_w+EPS),	float(_h+EPS),	d_Z,	d_W, C, p1.x, p1.y);	pv++;
		pv->set						(float(_w+EPS),	EPS,			d_Z,	d_W, C, p1.x, p0.y);	pv++;
		RCache.Vertex.Unlock		(4,g_combine->vb_stride);
		RCache.set_Geometry			(g_combine);

		// setup
		Fvector	dir					= L_dir;
					dir.normalize().mul	(- _sqrt((1.f+EPS)));
		RCache.set_Element			(s_rmap->E[2]);		// masker
		RCache.set_c				("Ldynamic_dir",		dir.x,dir.y,dir.z,0		);

		// if (stencil>=1 && aref_pass)	stencil = light_id
		RCache.set_ColorWriteEnable	(FALSE		);
		RCache.set_Stencil			(TRUE,D3DCMP_LESSEQUAL,dwRainMarkerID,0x01,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE,D3DSTENCILOP_KEEP);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	}

	// recalculate d_Z, to perform depth-clipping
	Fvector	center_pt;			center_pt.mad	(Device.vCameraPosition,Device.vCameraDirection,ps_r2_rmap_near);
	Device.mFullTransform.transform(center_pt)	;
	d_Z							= center_pt.z	;

	// Perform lighting
//	{
		phase_rmap_accumulator					()	;
		RCache.set_CullMode					(CULL_NONE);
		RCache.set_ColorWriteEnable			()	;

		// texture adjustment matrix
		float			fTexelOffs			= (.5f / float(RImplementation.o.smapsize));
		float			fRange				= (SE_SUN_NEAR==sub_phase)?ps_r2_rmap_depth_near_scale:ps_r2_rmap_depth_far_scale;
		float			fBias				= (SE_SUN_NEAR==sub_phase)?ps_r2_rmap_depth_near_bias:ps_r2_rmap_depth_far_bias;
		Fmatrix			m_TexelAdjust		= 
		{
			0.5f,				0.0f,				0.0f,			0.0f,
			0.0f,				-0.5f,				0.0f,			0.0f,
			0.0f,				0.0f,				fRange,			0.0f,
			0.5f + fTexelOffs,	0.5f + fTexelOffs,	fBias,			1.0f
		};

		// compute xforms
		FPU::m64r			();
		Fmatrix				xf_invview;		xf_invview.invert	(Device.mView)	;

		// shadow xform
		Fmatrix				m_shadow;
		{
			Fmatrix			xf_project;		xf_project.mul		(m_TexelAdjust,rain_caster.D.combine);
			m_shadow.mul	(xf_project,	xf_invview);

			// tsm-bias
			if ( (SE_SUN_FAR == sub_phase) && (RImplementation.o.HW_smap) )
			{
				Fvector		bias;	bias.mul		(L_dir,ps_r2_sun_tsm_bias);
				Fmatrix		bias_t;	bias_t.translate(bias);
				m_shadow.mulB_44	(bias_t);
			}
			FPU::m24r		();
		}

		// Make jitter texture
		Fvector2					j0,j1;
		float	scale_X				= float(Device.dwWidth)	/ float(TEX_jitter);
		//float	scale_Y				= float(Device.dwHeight)/ float(TEX_jitter);
		float	offset				= (.5f / float(TEX_jitter));
		j0.set						(offset,offset);
		j1.set						(scale_X,scale_X).add(offset);

		// Fill vertex buffer
		FVF::TL2uv* pv				= (FVF::TL2uv*) RCache.Vertex.Lock	(4,g_combine_2UV->vb_stride,Offset);
		pv->set						(EPS,			float(_h+EPS),	d_Z,	d_W, C, p0.x, p1.y, j0.x, j1.y);	pv++;
		pv->set						(EPS,			EPS,			d_Z,	d_W, C, p0.x, p0.y, j0.x, j0.y);	pv++;
		pv->set						(float(_w+EPS),	float(_h+EPS),	d_Z,	d_W, C, p1.x, p1.y, j1.x, j1.y);	pv++;
		pv->set						(float(_w+EPS),	EPS,			d_Z,	d_W, C, p1.x, p0.y, j1.x, j0.y);	pv++;
		RCache.Vertex.Unlock		(4,g_combine_2UV->vb_stride);
		RCache.set_Geometry			(g_combine_2UV);

		// setup
		RCache.set_Element			(s_rmap->E[sub_phase]);
		RCache.set_c				("Ldynamic_dir",		L_dir.x,L_dir.y,L_dir.z,0		);
		RCache.set_c				("m_rain",			m_shadow						);
		
		// nv-DBT
		float zMin,zMax;
		if (SE_SUN_NEAR==sub_phase)	{
			zMin = 0.01;
			zMax = ps_r2_rmap_near;
		} else {
//			extern float	OLES_SUN_LIMIT_27_01_07;
			zMin = ps_r2_rmap_near;
			zMax = OLES_SUN_LIMIT_27_01_07;
		}
		center_pt.mad(Device.vCameraPosition,Device.vCameraDirection,zMin);	Device.mFullTransform.transform	(center_pt);
		zMin = center_pt.z	;

		center_pt.mad(Device.vCameraPosition,Device.vCameraDirection,zMax);	Device.mFullTransform.transform	(center_pt);
		zMax = center_pt.z	;

		if (u_DBT_enable(zMin,zMax))	{
			// z-test always
			HW.pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
			HW.pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		}

		// Fetch4 : enable
		if (RImplementation.o.HW_smap_FETCH4)	{
			//. we hacked the shader to force smap on S0
#			define FOURCC_GET4  MAKEFOURCC('G','E','T','4') 
			HW.pDevice->SetSamplerState	( 0, D3DSAMP_MIPMAPLODBIAS, FOURCC_GET4 );
		}

		// setup stencil
		RCache.set_Stencil			(TRUE,D3DCMP_LESSEQUAL,dwRainMarkerID,0xff,0x00);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);

		// Fetch4 : disable
		if (RImplementation.o.HW_smap_FETCH4)	{
			//. we hacked the shader to force smap on S0
#			define FOURCC_GET1  MAKEFOURCC('G','E','T','1') 
			HW.pDevice->SetSamplerState	( 0, D3DSAMP_MIPMAPLODBIAS, FOURCC_GET1 );
		}

		// disable depth bounds
		u_DBT_disable	();
//	}*/
}

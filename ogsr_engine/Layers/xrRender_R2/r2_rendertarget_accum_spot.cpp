#include "stdafx.h"
#include "..\xrRender\xrRender_console.h"

extern Fvector du_cone_vertices[DU_CONE_NUMVERTEX];

extern int reset_frame;

void CRenderTarget::accum_spot(light* L)
{
	//	if (IsMainMenuActive())		return;
	accum_plane_spot(L);
}

void CRenderTarget::accum_plane_spot(light* L)
{
	phase_accumulator				();
	if (!is_torch_processed)
	{
		if (L->is_actor_torch())
		{
			actor_torch_enabled = true;
			is_torch_processed = true;
		}
		else
			actor_torch_enabled = false;
	}
	RImplementation.stats.l_visible	++;

	// *** assume accumulator setted up ***
	// *****************************	Mask by stencil		*************************************
	ref_shader			shader;
	if (IRender_Light::OMNIPART == L->flags.type)	{
			shader		= L->s_point;
		if (!shader)	shader		= s_accum_point;
	} else {
			shader		= L->s_spot;
		if (!shader)	shader		= s_accum_spot;
	}

	BOOL	bIntersect			= FALSE; //enable_scissor(L);
	{
		// setup xform
		L->xform_calc					();
		RCache.set_xform_world			(L->m_xform			);
		RCache.set_xform_view			(Device.mView		);
		RCache.set_xform_project		(Device.mProject	);
		bIntersect						= enable_scissor	(L);
		enable_dbt_bounds				(L);

		// *** similar to "Carmack's reverse", but assumes convex, non intersecting objects,
		// *** thus can cope without stencil clear with 127 lights
		// *** in practice, 'cause we "clear" it back to 0x1 it usually allows us to > 200 lights :)
		RCache.set_ColorWriteEnable		(FALSE);
		RCache.set_Element				(s_accum_mask->E[SE_MASK_SPOT]);		// masker

		// backfaces: if (stencil>=1 && zfail)			stencil = light_id
		RCache.set_CullMode				(CULL_CW);
		RCache.set_Stencil				(TRUE,D3DCMP_LESSEQUAL,dwLightMarkerID,0x01,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE);
		draw_volume						(L);

		// frontfaces: if (stencil>=light_id && zfail)	stencil = 0x1
		RCache.set_CullMode				(CULL_CCW);
		RCache.set_Stencil				(TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE);
		draw_volume						(L);
	}

	// nv-stencil recompression
	if (RImplementation.o.nvstencil)	u_stencil_optimize();

	// *****************************	Minimize overdraw	*************************************
	// Select shader (front or back-faces), *** back, if intersect near plane
	RCache.set_ColorWriteEnable		();
	RCache.set_CullMode				(CULL_CW);		// back

	// 2D texgens 
	Fmatrix			m_Texgen;			u_compute_texgen_screen	(m_Texgen	);
	Fmatrix			m_Texgen_J;			u_compute_texgen_jitter	(m_Texgen_J	);

	// Shadow xform (+texture adjustment matrix)
	Fmatrix			m_Shadow,m_Lmap;
	{
		float			smapsize			= float(RImplementation.o.smapsize);
		float			fTexelOffs			= (.5f / smapsize);
		float			view_dim			= float(L->X.S.size-2)/smapsize;
		float			view_sx				= float(L->X.S.posX+1)/smapsize;
		float			view_sy				= float(L->X.S.posY+1)/smapsize;
		float			fRange				= float(1.f)*ps_r2_ls_depth_scale;
		float			fBias				= ps_r2_ls_depth_bias;
		Fmatrix			m_TexelAdjust		= {
			view_dim/2.f,							0.0f,									0.0f,		0.0f,
			0.0f,									-view_dim/2.f,							0.0f,		0.0f,
			0.0f,									0.0f,									fRange,		0.0f,
			view_dim/2.f + view_sx + fTexelOffs,	view_dim/2.f + view_sy + fTexelOffs,	fBias,		1.0f
		};

		// compute xforms
		Fmatrix			xf_world;		xf_world.invert	(Device.mView);
		Fmatrix			xf_view			= L->X.S.view;
		Fmatrix			xf_project;		xf_project.mul	(m_TexelAdjust,L->X.S.project);
		m_Shadow.mul					(xf_view, xf_world);
		m_Shadow.mulA_44				(xf_project	);

		// lmap
						view_dim			= 1.f;
						view_sx				= 0.f;
						view_sy				= 0.f;
		Fmatrix			m_TexelAdjust2		= {
			view_dim/2.f,							0.0f,									0.0f,		0.0f,
			0.0f,									-view_dim/2.f,							0.0f,		0.0f,
			0.0f,									0.0f,									fRange,		0.0f,
			view_dim/2.f + view_sx + fTexelOffs,	view_dim/2.f + view_sy + fTexelOffs,	fBias,		1.0f
		};

		// compute xforms
		xf_project.mul		(m_TexelAdjust2,L->X.S.project);
		m_Lmap.mul			(xf_view, xf_world);
		m_Lmap.mulA_44		(xf_project	);
	}

	// Common constants
	Fvector		L_dir,L_clr,L_pos;	float L_spec;
	L_clr.set					(L->color.r,L->color.g,L->color.b);
	L_clr.mul					(L->get_LOD());
	L_spec						= u_diffuse2s	(L_clr);
	Device.mView.transform_tiny	(L_pos,L->position);
	Device.mView.transform_dir	(L_dir,L->direction);
	L_dir.normalize				();

	// Draw volume with projective texgen
	{
		// Select shader
		u32		_id					= 0;
		if (L->flags.bShadow)		{
			bool	bFullSize			= (L->X.S.size == RImplementation.o.smapsize);
			if (L->X.S.transluent)	_id	= SE_L_TRANSLUENT;
			else if		(bFullSize)	_id	= SE_L_FULLSIZE;
			else					_id	= SE_L_NORMAL;
		} else {
			_id						= SE_L_UNSHADOWED;
			m_Shadow				= m_Lmap;
		}
		RCache.set_Element			(shader->E[ _id ]	);

		// Constants
		float	att_R				= L->range*.95f;
		float	att_factor			= 1.f/(att_R*att_R);
		RCache.set_c				("Ldynamic_pos",	L_pos.x,L_pos.y,L_pos.z,att_factor);
		RCache.set_c				("Ldynamic_color",	L_clr.x,L_clr.y,L_clr.z,L_spec);
		RCache.set_c				("m_texgen",		m_Texgen	);
		RCache.set_c				("m_texgen_J",		m_Texgen_J	);
		RCache.set_c				("m_shadow",		m_Shadow	);
		RCache.set_ca				("m_lmap",		0,	m_Lmap._11, m_Lmap._21, m_Lmap._31, m_Lmap._41	);
		RCache.set_ca				("m_lmap",		1,	m_Lmap._12, m_Lmap._22, m_Lmap._32, m_Lmap._42	);
		RCache.set_c("lsf_params", L->LSF.speed, L->LSF.amount, L->LSF.smap_jitter, 0.0);
		if (ps_r2_test_flags.test(R2FLAG_PCSS))
		{
/*			RCache.set_c("m_v2lv", m_ViewToLightView);
			RCache.set_c("pcss_params", L->smap_near, L->range + EPS_S, /*L->smap_near*//*5 / (float)L->X.S.size, /*L->smap_near*//*5 / (float)L->X.S.size);*/
		}

		// Fetch4 : enable
		if (RImplementation.o.HW_smap_FETCH4)	{
			//. we hacked the shader to force smap on S0
#			define FOURCC_GET4  MAKEFOURCC('G','E','T','4') 
			HW.pDevice->SetSamplerState	( 0, D3DSAMP_MIPMAPLODBIAS, FOURCC_GET4 );
		}

		RCache.set_Stencil			(TRUE,D3DCMP_LESSEQUAL,dwLightMarkerID,0xff,0x00);
		draw_volume					(L);

		// Fetch4 : disable
		if (RImplementation.o.HW_smap_FETCH4)	{
			//. we hacked the shader to force smap on S0
#			define FOURCC_GET1  MAKEFOURCC('G','E','T','1') 
			HW.pDevice->SetSamplerState	( 0, D3DSAMP_MIPMAPLODBIAS, FOURCC_GET1 );
		}
	}

	// blend-copy
	if (!RImplementation.o.fp16_blend)	{
		u_setrt						(rt_Accumulator,NULL,NULL,HW.pBaseZB);
		RCache.set_Element			(s_accum_mask->E[SE_MASK_ACCUM_VOL]	);
		RCache.set_c				("m_texgen",		m_Texgen);
		RCache.set_c				("m_texgen_J",		m_Texgen_J	);
		draw_volume					(L);
	}

	CHK_DX		(HW.pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE));

	u_DBT_disable();

/*	if (ps_r2_ls_flags.test(R2FLAG_VOLLIGHT) && L->is_volumetric())
		accum_volumetric_spot(L);*/
	if ((ps_r2_ls_flags.test(R2FLAG_LENS_FLARES) && L->is_require_flare()) || ps_r2_ls_flags.test(R2FLAG_LENS_DIRT))
		render_flare(L);

	dwLightMarkerID += 2;	// keep lowest bit always setted up

}

#define	VOLUMETRIC_SLICES	100
void CRenderTarget::accum_volumetric_spot(light* L)
{
	if (!rt_volumetric) return;
	if (!ps_r2_ls_flags.test(R2FLAG_VOLLIGHT) || !(L->is_volumetric())) return;
	//	if (reset_frame==Device.dwFrame || reset_frame==Device.dwFrame - 1)		return;
	phase_accumulator_volumetric();
	//	RImplementation.stats.l_visible	++;

	//	L->xform_calc();
	RCache.set_xform_world(L->m_xform);
	RCache.set_xform_view(Device.mView);
	RCache.set_xform_project(Device.mProject);

	// 2D texgens 
	Fmatrix			m_Texgen;			u_compute_texgen_screen(m_Texgen);
	Fmatrix			m_Texgen_J;			u_compute_texgen_jitter(m_Texgen_J);

	// Shadow xform (+texture adjustment matrix)
	Fmatrix			m_Shadow, m_Lmap, m_Frustum;
	{
		float			smapsize = float(RImplementation.o.smapsize);
		float			fTexelOffs = (.5f / smapsize);
		float			view_dim = float(L->X.S.size - 2) / smapsize;
		float			view_sx = float(L->X.S.posX + 1) / smapsize;
		float			view_sy = float(L->X.S.posY + 1) / smapsize;
		float			fRange = float(1.f)*ps_r2_ls_depth_scale;
		float			fBias = ps_r2_ls_depth_bias;
		Fmatrix			m_TexelAdjust = {
			view_dim / 2.f,							0.0f,									0.0f,		0.0f,
			0.0f,									-view_dim / 2.f,							0.0f,		0.0f,
			0.0f,									0.0f,									fRange,		0.0f,
			view_dim / 2.f + view_sx + fTexelOffs,	view_dim / 2.f + view_sy + fTexelOffs,	fBias,		1.0f
		};

		// compute xforms
		Fmatrix		xf_world;		xf_world.invert(Device.mView);
		Fmatrix			xf_view = L->X.S.view;
		Fmatrix			xf_project;		xf_project.mul(m_TexelAdjust, L->X.S.project);
		m_Shadow.mul(xf_view, xf_world);
		m_Shadow.mulA_44(xf_project);

		// lmap
		view_dim = 1.f;
		view_sx = 0.f;
		view_sy = 0.f;
		Fmatrix			m_TexelAdjust2 = {
			view_dim / 2.f,							0.0f,									0.0f,		0.0f,
			0.0f,									-view_dim / 2.f,							0.0f,		0.0f,
			0.0f,									0.0f,									fRange,		0.0f,
			view_dim / 2.f + view_sx + fTexelOffs,	view_dim / 2.f + view_sy + fTexelOffs,	fBias,		1.0f
		};

		// compute xforms
		xf_project.mul(m_TexelAdjust2, L->X.S.project);
		m_Lmap.mul(xf_view, xf_world);
		m_Lmap.mulA_44(xf_project);

		// frustum matrix
		m_Frustum.mul(L->X.S.project, xf_view);
	}

	// Common constants
	Fvector		L_dir, L_clr, L_pos;	float L_spec;
	L_clr.set(L->color.r, L->color.g, L->color.b);
	L_clr.mul(L->get_LOD());
	L_spec = u_diffuse2s(L_clr);
	Device.mView.transform_tiny(L_pos, L->position);
	Device.mView.transform_dir(L_dir, L->direction);
	L_dir.normalize();

	Fvector vSpatial, vMinBounds, vMaxBounds;
	Device.mView.transform(vSpatial, L->spatial.sphere.P);
	vMinBounds.sub(vSpatial, L->spatial.sphere.R);
	vMaxBounds.add(vSpatial, L->spatial.sphere.R);

	// Draw volume with projective texgen
	{
		// Select shader
		/*		u32		_id					= 0;
		if (L->flags.bShadow)		{
		bool	bFullSize			= (L->X.S.size == RImplementation.o.smapsize);
		if (L->X.S.transluent || bFullSize)	_id	= 4;
		else					_id	= 3;
		} else {
		_id						= 2;
		m_Shadow				= m_Lmap;
		}*/
		RCache.set_Element(s_accum_volumetric->E[2]);

		// Constants
		float	att_R = L->range*.95f;
		float	att_factor = 1.f / (att_R*att_R);
		RCache.set_c("Ldynamic_pos", L_pos.x, L_pos.y, L_pos.z, att_factor);
		RCache.set_c("Ldynamic_color", L_clr.x, L_clr.y, L_clr.z, L_spec);
		RCache.set_c("m_texgen", m_Texgen);
		RCache.set_c("m_texgen_J", m_Texgen_J);
		RCache.set_c("m_shadow", m_Shadow);
		RCache.set_ca("m_lmap", 0, m_Lmap._11, m_Lmap._21, m_Lmap._31, m_Lmap._41);
		RCache.set_ca("m_lmap", 1, m_Lmap._12, m_Lmap._22, m_Lmap._32, m_Lmap._42);
		RCache.set_c("vMinBounds", vMinBounds.x, vMinBounds.y, vMinBounds.z, 1.0);
		RCache.set_c("vMaxBounds", vMaxBounds.x, vMaxBounds.y, vMaxBounds.z, 1.0);

		// clip planes
		CFrustum	F;
		F.CreateFromMatrix(m_Frustum, 0x3F);

		D3DXMATRIX			worldToClipMatrixIT;
		D3DXMatrixTranspose(&worldToClipMatrixIT, (D3DXMATRIX*)&Device.mInvFullTransform);
		CHK_DX(HW.pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, 0x3F));
		for (u32 it = 0; it<6; it++) {
			Fplane&		P = F.planes[it];
			D3DXPLANE	planeWorld(-P.n.x, -P.n.y, -P.n.z, -P.d), planeClip;
			D3DXPlaneNormalize(&planeWorld, &planeWorld);
			D3DXPlaneTransform(&planeClip, &planeWorld, &worldToClipMatrixIT);
			CHK_DX(HW.pDevice->SetClipPlane(it, planeClip));
		}

		// Fetch4 : enable
		if (RImplementation.o.HW_smap_FETCH4) {
			//. we hacked the shader to force smap on S0
#			define FOURCC_GET4  MAKEFOURCC('G','E','T','4') 
			HW.pDevice->SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS, FOURCC_GET4);
		}

		RCache.set_ColorWriteEnable(7);
		draw_volume(L, true);
		RCache.set_ColorWriteEnable(15);

		// Fetch4 : disable
		if (RImplementation.o.HW_smap_FETCH4) {
			//. we hacked the shader to force smap on S0
#			define FOURCC_GET1  MAKEFOURCC('G','E','T','1') 
			HW.pDevice->SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS, FOURCC_GET1);
		}
	}
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, FALSE));
}
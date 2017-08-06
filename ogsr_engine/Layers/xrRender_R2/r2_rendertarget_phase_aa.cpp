#include "stdafx.h"
#include "..\xrRender\xrRender_console.h"

extern u32 SMAA_MAX_SEARCH_STEPS;// = (1 << (ps_aa_quality+1));

void CRenderTarget::process_fxaa()
{
	render_simple_quad			(rt_Generic_0,		s_aa->E[1], 1.0);
	render_simple_quad			(rt_Generic_0,		s_aa->E[0], 1.0);
};

void CRenderTarget::process_smaa()
{
	u32 Offset;
	Fvector2 p0,p1;

	struct v_simple	{
		Fvector4	p;
		Fvector2	uv0;
		Fvector4	uv1;
		Fvector4	uv2;
		Fvector4	uv3;
		Fvector4	uv4;
	};

	float	_w					= float(Device.dwWidth);
	float	_h					= float(Device.dwHeight);
	float	ddw					= 1.f/_w;
	float	ddh					= 1.f/_h;
	p0.set						(.5f/_w, .5f/_h);
	p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

// edge detection

	u_setrt(rt_smaa_edgetex,0,0,HW.pBaseZB);
	RCache.set_CullMode		( CULL_NONE )	;
	RCache.set_Stencil		( TRUE, D3DCMP_ALWAYS, 1, 0, 0, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP)	;

	CHK_DX	( HW.pDevice->Clear	( 0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0L) );

	// Fill vertex buffer
	v_simple* pv					= (v_simple*) RCache.Vertex.Lock	(4,g_Test_AA->vb_stride,Offset);
		pv->p.set(EPS,			float(_h+EPS),	EPS,1.f); pv->uv0.set(p0.x, p1.y);pv->uv1.set(p0.x-ddw, p1.y, p0.x, p1.y-ddh);pv->uv2.set(p0.x+ddw, p1.y, p0.x, p1.y+ddh);pv->uv3.set(p0.x-2*ddw, p1.y, p0.x, p1.y-2*ddh);pv->uv4.set(p0.x+2*ddw, p1.y, p0.x, p1.y+2*ddh);pv++;
		pv->p.set(EPS,			EPS,			EPS,1.f); pv->uv0.set(p0.x, p0.y);pv->uv1.set(p0.x-ddw, p0.y, p0.x, p0.y-ddh);pv->uv2.set(p0.x+ddw, p0.y, p0.x, p0.y+ddh);pv->uv3.set(p0.x-2*ddw, p0.y, p0.x, p0.y-2*ddh);pv->uv4.set(p0.x+2*ddw, p0.y, p0.x, p0.y+2*ddh);pv++;
		pv->p.set(float(_w+EPS),float(_h+EPS),	EPS,1.f); pv->uv0.set(p1.x, p1.y);pv->uv1.set(p1.x-ddw, p1.y, p1.x, p1.y-ddh);pv->uv2.set(p1.x+ddw, p1.y, p1.x, p1.y+ddh);pv->uv3.set(p1.x-2*ddw, p1.y, p1.x, p1.y-2*ddh);pv->uv4.set(p1.x+2*ddw, p1.y, p1.x, p1.y+2*ddh);pv++;
		pv->p.set(float(_w+EPS),EPS,			EPS,1.f); pv->uv0.set(p1.x, p0.y);pv->uv1.set(p1.x-ddw, p0.y, p1.x, p0.y-ddh);pv->uv2.set(p1.x+ddw, p0.y, p1.x, p0.y+ddh);pv->uv3.set(p1.x-2*ddw, p0.y, p1.x, p0.y-2*ddh);pv->uv4.set(p1.x+2*ddw, p0.y, p1.x, p0.y+2*ddh);pv++;	
	RCache.Vertex.Unlock		(4,g_Test_AA->vb_stride);

	// Draw COLOR
	RCache.set_Element			(s_aa->E[2]);
	RCache.set_Geometry			(g_Test_AA);
	RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);

// blend weights calculation

	u_setrt(rt_smaa_blendtex,0,0,HW.pBaseZB);
	CHK_DX	( HW.pDevice->Clear	( 0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0L) );
	RCache.set_CullMode		( CULL_NONE )	;
	RCache.set_Stencil		( TRUE, D3DCMP_EQUAL, 1, 0, 0, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP)	;

	// Fill vertex buffer
	pv					= (v_simple*) RCache.Vertex.Lock	(4,g_Test_AA->vb_stride,Offset);
		pv->p.set(EPS,			float(_h+EPS),	EPS,1.f); pv->uv0.set(p0.x, p1.y);pv->uv1.set(p0.x/ddw, p1.y/ddh, p0.x/ddw, p1.y/ddh);pv->uv2.set(p0.x-0.25f*ddw, p1.y-0.125f*ddh, p0.x+1.25f*ddw, p1.y-0.125f*ddh);pv->uv3.set(p0.x-0.125f*ddw, p1.y-0.25f*ddh, p0.x-0.125f*ddw, p1.y+1.25f*ddh);pv->uv4.set(p0.x-(0.25f+2*SMAA_MAX_SEARCH_STEPS)*ddw, p0.x+(1.25f+2*SMAA_MAX_SEARCH_STEPS)*ddw, p1.y-(0.25f+2*SMAA_MAX_SEARCH_STEPS)*ddh, p1.y+(1.25f+2*SMAA_MAX_SEARCH_STEPS)*ddh);pv++;
		pv->p.set(EPS,			EPS,			EPS,1.f); pv->uv0.set(p0.x, p0.y);pv->uv1.set(p0.x/ddw, p0.y/ddh, p0.x/ddw, p0.y/ddh);pv->uv2.set(p0.x-0.25f*ddw, p0.y-0.125f*ddh, p0.x+1.25f*ddw, p0.y-0.125f*ddh);pv->uv3.set(p0.x-0.125f*ddw, p0.y-0.25f*ddh, p0.x-0.125f*ddw, p0.y+1.25f*ddh);pv->uv4.set(p0.x-(0.25f+2*SMAA_MAX_SEARCH_STEPS)*ddw, p0.x+(1.25f+2*SMAA_MAX_SEARCH_STEPS)*ddw, p0.y-(0.25f+2*SMAA_MAX_SEARCH_STEPS)*ddh, p0.y+(1.25f+2*SMAA_MAX_SEARCH_STEPS)*ddh);pv++;
		pv->p.set(float(_w+EPS),float(_h+EPS),	EPS,1.f); pv->uv0.set(p1.x, p1.y);pv->uv1.set(p1.x/ddw, p1.y/ddh, p1.x/ddw, p1.y/ddh);pv->uv2.set(p1.x-0.25f*ddw, p1.y-0.125f*ddh, p1.x+1.25f*ddw, p1.y-0.125f*ddh);pv->uv3.set(p1.x-0.125f*ddw, p1.y-0.25f*ddh, p1.x-0.125f*ddw, p1.y+1.25f*ddh);pv->uv4.set(p1.x-(0.25f+2*SMAA_MAX_SEARCH_STEPS)*ddw, p1.x+(1.25f+2*SMAA_MAX_SEARCH_STEPS)*ddw, p1.y-(0.25f+2*SMAA_MAX_SEARCH_STEPS)*ddh, p1.y+(1.25f+2*SMAA_MAX_SEARCH_STEPS)*ddh);pv++;
		pv->p.set(float(_w+EPS),EPS,			EPS,1.f); pv->uv0.set(p1.x, p0.y);pv->uv1.set(p1.x/ddw, p0.y/ddh, p1.x/ddw, p0.y/ddh);pv->uv2.set(p1.x-0.25f*ddw, p0.y-0.125f*ddh, p1.x+1.25f*ddw, p0.y-0.125f*ddh);pv->uv3.set(p1.x-0.125f*ddw, p0.y-0.25f*ddh, p1.x-0.125f*ddw, p0.y+1.25f*ddh);pv->uv4.set(p1.x-(0.25f+2*SMAA_MAX_SEARCH_STEPS)*ddw, p1.x+(1.25f+2*SMAA_MAX_SEARCH_STEPS)*ddw, p0.y-(0.25f+2*SMAA_MAX_SEARCH_STEPS)*ddh, p0.y+(1.25f+2*SMAA_MAX_SEARCH_STEPS)*ddh);pv++;	
	RCache.Vertex.Unlock		(4,g_Test_AA->vb_stride);

	// Draw COLOR
	RCache.set_Element			(s_aa->E[3]);
	RCache.set_Geometry			(g_Test_AA);
	RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);

// neighbour blend

	u_setrt(rt_Generic_0,0,0,HW.pBaseZB);
	RCache.set_CullMode		( CULL_NONE )	;
	RCache.set_Stencil		( FALSE		)	;

	// Fill vertex buffer
	pv					= (v_simple*) RCache.Vertex.Lock	(4,g_Test_AA->vb_stride,Offset);
		pv->p.set(EPS,			float(_h+EPS),	EPS,1.f); pv->uv0.set(p0.x, p1.y);pv->uv1.set(p0.x-ddw, p1.y, p0.x, p1.y-ddh);pv->uv2.set(p0.x+ddw, p1.y, p0.x, p1.y+ddh);pv->uv3.set(p0.x-2*ddw, p1.y, p0.x, p1.y-2*ddh);pv->uv4.set(p0.x+2*ddw, p1.y, p0.x, p1.y+2*ddh);pv++;
		pv->p.set(EPS,			EPS,			EPS,1.f); pv->uv0.set(p0.x, p0.y);pv->uv1.set(p0.x-ddw, p0.y, p0.x, p0.y-ddh);pv->uv2.set(p0.x+ddw, p0.y, p0.x, p0.y+ddh);pv->uv3.set(p0.x-2*ddw, p0.y, p0.x, p0.y-2*ddh);pv->uv4.set(p0.x+2*ddw, p0.y, p0.x, p0.y+2*ddh);pv++;
		pv->p.set(float(_w+EPS),float(_h+EPS),	EPS,1.f); pv->uv0.set(p1.x, p1.y);pv->uv1.set(p1.x-ddw, p1.y, p1.x, p1.y-ddh);pv->uv2.set(p1.x+ddw, p1.y, p1.x, p1.y+ddh);pv->uv3.set(p1.x-2*ddw, p1.y, p1.x, p1.y-2*ddh);pv->uv4.set(p1.x+2*ddw, p1.y, p1.x, p1.y+2*ddh);pv++;
		pv->p.set(float(_w+EPS),EPS,			EPS,1.f); pv->uv0.set(p1.x, p0.y);pv->uv1.set(p1.x-ddw, p0.y, p1.x, p0.y-ddh);pv->uv2.set(p1.x+ddw, p0.y, p1.x, p0.y+ddh);pv->uv3.set(p1.x-2*ddw, p0.y, p1.x, p0.y-2*ddh);pv->uv4.set(p1.x+2*ddw, p0.y, p1.x, p0.y+2*ddh);pv++;	
	RCache.Vertex.Unlock		(4,g_Test_AA->vb_stride);

	// Draw COLOR
	RCache.set_Element			(s_aa->E[4]);
	RCache.set_Geometry			(g_Test_AA);
	RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
};

void CRenderTarget::process_taa()
{
	process_smaa();
	
	u32 Offset;
	Fvector2 p0,p1;

	struct v_aa	{
		Fvector4	p;
		Fvector2	uv0;
	};

	// calc matrices
	Fmatrix		m_previous, m_current;
	{
		static Fmatrix		m_saved_viewproj;
		
		// (new-camera) -> (world) -> (old_viewproj)
		Fmatrix	m_invview;	m_invview.invert	(Device.mView);
		m_previous.mul		(m_saved_viewproj,m_invview);
		m_current.set		(Device.mProject)		;
		m_saved_viewproj.set(Device.mFullTransform)	;
	}

	float	_w					= float(Device.dwWidth);
	float	_h					= float(Device.dwHeight);
	p0.set						(.5f/_w, .5f/_h);
	p1.set						((_w+.5f)/_w, (_h+.5f)/_h );


	u_setrt(rt_Generic_0,0,0,HW.pBaseZB);
	RCache.set_CullMode		( CULL_NONE )	;
	RCache.set_Stencil		( FALSE		)	;

	// Fill vertex buffer
	v_aa* pv					= (v_aa*) RCache.Vertex.Lock	(4,g_KD->vb_stride,Offset);
		pv->p.set(EPS,			float(_h+EPS),	EPS,1.f); pv->uv0.set(p0.x, p1.y);pv++;
		pv->p.set(EPS,			EPS,			EPS,1.f); pv->uv0.set(p0.x, p0.y);pv++;
		pv->p.set(float(_w+EPS),float(_h+EPS),	EPS,1.f); pv->uv0.set(p1.x, p1.y);pv++;
		pv->p.set(float(_w+EPS),EPS,			EPS,1.f); pv->uv0.set(p1.x, p0.y);pv++;	
	RCache.Vertex.Unlock		(4,g_KD->vb_stride);

	// Draw COLOR
	RCache.set_Element			(s_aa->E[5]);
	RCache.set_Geometry			(g_KD);
	RCache.set_c				("m_current",	m_current);
	RCache.set_c				("m_previous",	m_previous);
	RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
};
void CRenderTarget::phase_aa()
{
	if (ps_aa_mode == R2AA_FXAA) 
		process_fxaa();
	else if (ps_aa_mode == R2AA_SMAA_1X)
		process_smaa();
	else if (ps_aa_mode == R2AA_SMAA_T2X)
	{
/*		process_taa();
		save_previous_frame();
			// switch id for frame jittering
		if (0==frame)
			frame = 1;
		else
			frame = 0;*/
	}
};
void CRenderTarget::save_previous_frame()
{
	u32 Offset;
	Fvector2 p0,p1;

	struct v_aa	{
		Fvector4	p;
		Fvector2	uv0;
	};

	float	_w					= float(Device.dwWidth);
	float	_h					= float(Device.dwHeight);
	p0.set						(.5f/_w, .5f/_h);
	p1.set						((_w+.5f)/_w, (_h+.5f)/_h );


	u_setrt(rt_prev_frame0,0,0,HW.pBaseZB);
	RCache.set_CullMode		( CULL_NONE )	;
	RCache.set_Stencil		( FALSE		)	;

	// Fill vertex buffer
	v_aa* pv					= (v_aa*) RCache.Vertex.Lock	(4,g_KD->vb_stride,Offset);
		pv->p.set(EPS,			float(_h+EPS),	EPS,1.f); pv->uv0.set(p0.x, p1.y);pv++;
		pv->p.set(EPS,			EPS,			EPS,1.f); pv->uv0.set(p0.x, p0.y);pv++;
		pv->p.set(float(_w+EPS),float(_h+EPS),	EPS,1.f); pv->uv0.set(p1.x, p1.y);pv++;
		pv->p.set(float(_w+EPS),EPS,			EPS,1.f); pv->uv0.set(p1.x, p0.y);pv++;	
	RCache.Vertex.Unlock		(4,g_KD->vb_stride);

	// Draw COLOR
	RCache.set_Element			(s_combine->E[1]);
	RCache.set_Geometry			(g_KD);
	RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
};
#include "stdafx.h"
#include "..\xrRender\xrRender_console.h"

void CRenderTarget::phase_motion_blur()
{
	u32 Offset;
	Fvector2 p0,p1;

	// calc m-blur matrices
	Fmatrix		m_previous, m_current;
	Fvector2	m_blur_scale;
	{
		static Fmatrix		m_saved_viewproj;
		
		// (new-camera) -> (world) -> (old_viewproj)
		Fmatrix	m_invview;	m_invview.invert	(Device.mView);
		m_previous.mul		(m_saved_viewproj,m_invview);
		m_current.set		(Device.mProject)		;
		m_saved_viewproj.set(Device.mFullTransform)	;
		float	scale		= (ps_r2_mblur)/2.f;
		m_blur_scale.set	(scale,-scale).div(12.f);
	}

	struct v_aa	{
		Fvector4	p;
		Fvector2	uv0;
		Fvector2	uv1;
		Fvector2	uv2;
		Fvector2	uv3;
		Fvector2	uv4;
		Fvector4	uv5;
		Fvector4	uv6;
	};

	float	_w					= float(Device.dwWidth);
	float	_h					= float(Device.dwHeight);
	float	ddw					= 1.f/_w;
	float	ddh					= 1.f/_h;
	p0.set						(.5f/_w, .5f/_h);
	p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

	// Set RT's
	u_setrt(rt_Generic_0,0,0,HW.pBaseZB);
	RCache.set_CullMode		( CULL_NONE )	;
	RCache.set_Stencil		( FALSE		)	;

	// Fill vertex buffer
	v_aa* pv					= (v_aa*) RCache.Vertex.Lock	(4,g_aa_AA->vb_stride,Offset);
		pv->p.set(EPS,			float(_h+EPS),	EPS,1.f); pv->uv0.set(p0.x, p1.y);pv->uv1.set(p0.x-ddw,p1.y-ddh);pv->uv2.set(p0.x+ddw,p1.y+ddh);pv->uv3.set(p0.x+ddw,p1.y-ddh);pv->uv4.set(p0.x-ddw,p1.y+ddh);pv->uv5.set(p0.x-ddw,p1.y,p1.y,p0.x+ddw);pv->uv6.set(p0.x,p1.y-ddh,p1.y+ddh,p0.x);pv++;
		pv->p.set(EPS,			EPS,			EPS,1.f); pv->uv0.set(p0.x, p0.y);pv->uv1.set(p0.x-ddw,p0.y-ddh);pv->uv2.set(p0.x+ddw,p0.y+ddh);pv->uv3.set(p0.x+ddw,p0.y-ddh);pv->uv4.set(p0.x-ddw,p0.y+ddh);pv->uv5.set(p0.x-ddw,p0.y,p0.y,p0.x+ddw);pv->uv6.set(p0.x,p0.y-ddh,p0.y+ddh,p0.x);pv++;
		pv->p.set(float(_w+EPS),float(_h+EPS),	EPS,1.f); pv->uv0.set(p1.x, p1.y);pv->uv1.set(p1.x-ddw,p1.y-ddh);pv->uv2.set(p1.x+ddw,p1.y+ddh);pv->uv3.set(p1.x+ddw,p1.y-ddh);pv->uv4.set(p1.x-ddw,p1.y+ddh);pv->uv5.set(p1.x-ddw,p1.y,p1.y,p1.x+ddw);pv->uv6.set(p1.x,p1.y-ddh,p1.y+ddh,p1.x);pv++;
		pv->p.set(float(_w+EPS),EPS,			EPS,1.f); pv->uv0.set(p1.x, p0.y);pv->uv1.set(p1.x-ddw,p0.y-ddh);pv->uv2.set(p1.x+ddw,p0.y+ddh);pv->uv3.set(p1.x+ddw,p0.y-ddh);pv->uv4.set(p1.x-ddw,p0.y+ddh);pv->uv5.set(p1.x-ddw,p0.y,p0.y,p1.x+ddw);pv->uv6.set(p1.x,p0.y-ddh,p0.y+ddh,p1.x);pv++;
	RCache.Vertex.Unlock		(4,g_aa_AA->vb_stride);

	// Draw COLOR
	RCache.set_Element			(s_mblur->E[0]);
	RCache.set_Geometry			(g_aa_AA);
	RCache.set_c				("m_current",	m_current);
	RCache.set_c				("m_previous",	m_previous);
	RCache.set_c				("m_blur",		m_blur_scale.x,m_blur_scale.y, 0,0);
	RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
};
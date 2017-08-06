#include "stdafx.h"

void CRenderTarget::prepare_simple_quad(ref_rt& DEST_RT, ref_selement& SHADER, u32& Offset, u32 w, u32 h, float downscale)
{
//	u32 Offset;
	Fvector2 p0,p1;

	// common 
	RCache.set_CullMode		( CULL_NONE )	;
	RCache.set_Stencil		( FALSE		)	;

	struct v_simple	{
		Fvector4	p;
		Fvector2	uv0;
	};

	float	_w					= float(w)*downscale;
	float	_h					= float(h)*downscale;
	float	ddw					= 1.f/_w;
	float	ddh					= 1.f/_h;
	p0.set						(.5f/_w, .5f/_h);
	p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

	// Set RT's
	u_setrt(DEST_RT,0,0,HW.pBaseZB);

	// Fill vertex buffer
	v_simple* pv					= (v_simple*) RCache.Vertex.Lock	(4,g_KD->vb_stride,Offset);
		pv->p.set(EPS,			float(_h+EPS),	EPS,1.f); pv->uv0.set(p0.x, p1.y);pv++;
		pv->p.set(EPS,			EPS,			EPS,1.f); pv->uv0.set(p0.x, p0.y);pv++;
		pv->p.set(float(_w+EPS),float(_h+EPS),	EPS,1.f); pv->uv0.set(p1.x, p1.y);pv++;
		pv->p.set(float(_w+EPS),EPS,			EPS,1.f); pv->uv0.set(p1.x, p0.y);pv++;
	RCache.Vertex.Unlock		(4,g_KD->vb_stride);

	// Draw COLOR
	RCache.set_Element			(SHADER);
	RCache.set_Geometry			(g_KD);
};
void CRenderTarget::prepare_simple_quad(ref_rt& DEST_RT, ref_selement& SHADER, u32& Offset, float downscale)
{	
	prepare_simple_quad			(DEST_RT, SHADER, Offset, Device.dwWidth, Device.dwHeight, downscale);
}
void CRenderTarget::render_simple_quad(ref_rt& DEST_RT, ref_selement& SHADER, float downscale)
{	
	u32 Offset;
	prepare_simple_quad			(DEST_RT, SHADER, Offset, downscale);
	RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
}
void CRenderTarget::render_simple_quad(ref_rt& DEST_RT, ref_selement& SHADER, u32 w, u32 h, float downscale)
{	
	u32 Offset;
	prepare_simple_quad			(DEST_RT, SHADER, Offset, w, h, downscale);
	RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
}
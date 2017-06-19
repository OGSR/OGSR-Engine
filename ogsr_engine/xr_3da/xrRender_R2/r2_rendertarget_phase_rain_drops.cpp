#include "stdafx.h"
#include "..\xrRender\xrRender_console.h"
#include "..\Rain.h"

/*rain_timer_params *rain_timers_raycheck = nullptr;
rain_timer_params *rain_timers = nullptr;*/

void CRenderTarget::phase_rain_drops()
{
	u32 Offset;
	Fvector2 p0,p1;

	// common 
	RCache.set_CullMode		( CULL_NONE )	;
	RCache.set_Stencil		( FALSE		)	;

	struct v_simple	{
		Fvector4	p;
		Fvector2	uv0;
	};

	float	_w					= float(Device.dwWidth);
	float	_h					= float(Device.dwHeight);
	p0.set						(.5f/_w, .5f/_h);
	p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

	// Set RT's
	u_setrt(rt_Generic_0,0,0,HW.pBaseZB);

	// Fill vertex buffer
	v_simple* pv					= (v_simple*) RCache.Vertex.Lock	(4,g_KD->vb_stride,Offset);
		pv->p.set(EPS,			float(_h+EPS),	EPS,1.f); pv->uv0.set(p0.x, p1.y);pv++;
		pv->p.set(EPS,			EPS,			EPS,1.f); pv->uv0.set(p0.x, p0.y);pv++;
		pv->p.set(float(_w+EPS),float(_h+EPS),	EPS,1.f); pv->uv0.set(p1.x, p1.y);pv++;
		pv->p.set(float(_w+EPS),EPS,			EPS,1.f); pv->uv0.set(p1.x, p0.y);pv++;
	RCache.Vertex.Unlock		(4,g_KD->vb_stride);

	// Draw COLOR
	RCache.set_Element			(s_rain_drops->E[0]);
	RCache.set_c				("c_timers",	rain_timers_raycheck->timer.x, rain_timers_raycheck->timer.y, rain_timers_raycheck->timer.z, 0.f);
	RCache.set_Geometry			(g_KD);
	RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	RCache.set_Stencil		(FALSE);
};
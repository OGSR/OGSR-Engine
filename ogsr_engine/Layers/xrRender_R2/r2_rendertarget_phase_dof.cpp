#include "stdafx.h"
#include "..\xrRender\xrRender_console.h"
extern Fvector4 w_states;
void CRenderTarget::phase_dof()
{
//	BEGIN_PERF_EVENT(D3DCOLOR_XRGB(0,255,255), L"DOF");
	u32 Offset;
	prepare_simple_quad			(rt_dof,		s_dof->E[0], Offset, 0.5);
	RCache.set_c				("c_timers", w_timers.x, w_timers.y, w_timers.z, (ps_r2_pp_flags.test(R2PP_FLAG_DOF_CONTROL))?1.f:0.f);
	RCache.set_c				("c_states", float(w_states.x), float(w_states.y), float(w_states.z), float(w_states.w));
	RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	render_simple_quad			(rt_Generic_0,	s_dof->E[1], 1.0);
//	END_PERF_EVENT;
};
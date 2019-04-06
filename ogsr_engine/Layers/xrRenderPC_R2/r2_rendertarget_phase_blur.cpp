#include "stdafx.h"
#include "../xrRender/xrRender_console.h"

void CRenderTarget::phase_blur()
{
	u32 Offset;
	prepare_simple_quad		(rt_blur_temp,	s_blur->E[0], Offset, 1.0f);
	RCache.set_c			("blur_params", ps_r2_blur_params.x, ps_r2_blur_params.y + 0.01f, ps_r2_blur_params.z/ps_r2_blur_params.y, (ps_r2_blur_params.x - 0.1f)/ps_r2_blur_params.y);
	RCache.Render			(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
	prepare_simple_quad		(rt_blur,		s_blur->E[1], Offset, 1.0f);
	RCache.set_c			("blur_params", ps_r2_blur_params.x, ps_r2_blur_params.y + 0.01f, ps_r2_blur_params.z/ps_r2_blur_params.y, (ps_r2_blur_params.x - 0.1f)/ps_r2_blur_params.y);
	RCache.Render			(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
}
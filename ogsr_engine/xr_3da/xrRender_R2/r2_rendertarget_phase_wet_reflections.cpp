#include "stdafx.h"
#include "..\xrRender\xrRender_console.h"
#include "..\Rain.h"

//Fvector4 *rain_params = nullptr;

void	CRenderTarget::phase_wet_reflections	()
{
	if (!g_pGameLevel)			return;
//	phase_rain	();
/*	float	factor				= g_pGamePersistent->Environment().CurrentEnv.rain_density;
	if (factor<EPS_L)			return;*/
	if (rain_params->x<EPS_L)			return;
	if (ps_r2_ls_flags.test(R2FLAG_WET_SURFACES))
	{
		render_simple_quad		(rt_reflections,	s_reflections->E[0], 1.0);
		render_simple_quad		(rt_Generic_0,		s_reflections->E[1], 1.0);
	/*	RCache.set_c			("blur_params", ps_r2_blur_params.x, ps_r2_blur_params.y + 0.01, ps_r2_blur_params.z/ps_r2_blur_params.y, (ps_r2_blur_params.x - 0.1)/ps_r2_blur_params.y);
		render_quad_simple		(rt_blur_temp,	s_reflections->E[3], 1.0);
		RCache.set_c			("blur_params", ps_r2_blur_params.x, ps_r2_blur_params.y + 0.01, ps_r2_blur_params.z/ps_r2_blur_params.y, (ps_r2_blur_params.x - 0.1)/ps_r2_blur_params.y);
		render_quad_simple		(rt_rain,		s_reflections->E[4], 1.0);*/
	}
	render_simple_quad		(rt_Generic_0,		s_reflections->E[2], 1.0);
}
/*
void	CRenderTarget::phase_reflections_preprocess	()
{
	if (!g_pGameLevel)			return;
	float	factor				= g_pGamePersistent->Environment().CurrentEnv.rain_density;
	if (factor<EPS_L)			return;
	if (ps_r2_ls_flags.test(R2FLAG_WET_SURFACES))
	{
		render_simple_quad		(rt_reflections,	s_reflections->E[0], 1.0);
		render_simple_quad		(rt_Generic_0,		s_reflections->E[1], 1.0);
	/*	RCache.set_c			("blur_params", ps_r2_blur_params.x, ps_r2_blur_params.y + 0.01, ps_r2_blur_params.z/ps_r2_blur_params.y, (ps_r2_blur_params.x - 0.1)/ps_r2_blur_params.y);
		render_quad_simple		(rt_blur_temp,	s_reflections->E[3], 1.0);
		RCache.set_c			("blur_params", ps_r2_blur_params.x, ps_r2_blur_params.y + 0.01, ps_r2_blur_params.z/ps_r2_blur_params.y, (ps_r2_blur_params.x - 0.1)/ps_r2_blur_params.y);
		render_quad_simple		(rt_rain,		s_reflections->E[4], 1.0);*//*
	}
	render_simple_quad		(rt_Generic_0,		s_reflections->E[2], 1.0);
}
*/
void	CRenderTarget::phase_rain	()
{
	u_setrt(rt_rain,0,0,HW.pBaseZB);
	HW.pDevice->Clear(0L, NULL, D3DCLEAR_TARGET, color_rgba(0,0,0,0), 1.0f, 0L);
}
#include "stdafx.h"
#include "..\xrRender\xrRender_console.h"
#include "..\..\xr_3da\Rain.h"

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

	// Функция рассчитывает интенсивность эффекта капель на худе. В шейдере нормально рассчитать слишком муторно, проще посчитать здесь и получить в шейдере через c_timers.w
	static float rain_drops_factor = 0.f;
	auto update_rain_drops_factor = [](bool act_on_rain)
	{
		static u32 steps_finished = 0;
		float factor = g_pGamePersistent->Environment().CurrentEnv.rain_density;
		if (!fis_zero(factor))
		{
			// В данном варианте настроек - при выходе из укрытия в шторм, капли заработают на полную мощность за 20 секунд. При заходе в укрытие - эффект отключится так же через 20 секунд.
			constexpr u32 change_step = 200; //Интервал в миллисекундах между ступенями изменения rain_drops_factor
			constexpr u32 steps_count = 100; //Кол-во ступеней. Чем меньше интервал - тем больше ступеней должно быть.
			constexpr float step_rain_factor_change = 1.f / float(steps_count);

			static bool saved_rain_flag = act_on_rain;
			if (saved_rain_flag != act_on_rain) {
				saved_rain_flag = act_on_rain;
				steps_finished = 0;
			}

			if (steps_finished < steps_count) {
				static u32 last_update = Device.dwTimeGlobal;
				if (Device.dwTimeGlobal > (last_update + change_step)) {
					last_update = Device.dwTimeGlobal;
					steps_finished++;
					if (act_on_rain) { //плавное повышение интенсивности капель.
						rain_drops_factor += step_rain_factor_change;
					}
					else { //плавное понижение интенсивности капель.
						rain_drops_factor -= step_rain_factor_change;
					}
					rain_drops_factor = std::clamp(rain_drops_factor, 0.f, factor);
				}
			}
		}
		else {
			steps_finished = 0;
			rain_drops_factor = 0.f;
		}
	};

	//Msg("[%s] rain_timers_raycheck: %f,%f,%f", __FUNCTION__, rain_timers_raycheck->timer.x, rain_timers_raycheck->timer.y, rain_timers_raycheck->timer.z);
	update_rain_drops_factor(fis_zero(rain_timers_raycheck->timer.y)); //Когда актор в укрытии - y больше нуля. По этому признаку и будем определять. Да, по уму надо переделать но мне лень.
	//Msg("[%s] rain_drops_factor: [%f]", __FUNCTION__, rain_drops_factor);
	RCache.set_c("c_timers", rain_timers_raycheck->timer.x, rain_timers_raycheck->timer.y, rain_timers_raycheck->timer.z, rain_drops_factor);

	RCache.set_Geometry(g_KD);
	RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
	RCache.set_Stencil(FALSE);
};

#include "stdafx.h"
#include "..\xrRender\xrRender_console.h"

void CRenderTarget::phase_rain_drops()
{
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static float rain_drops_factor = 0.f;
	static u32 steps_finished = 0;

	// Чтобы по команде r2_rain_drops_control off/on эффект перезапускался.
	static bool saved_rain_drops_control = false;
	bool current_rain_drops_control = !!ps_r2_pp_flags.test(R2PP_FLAG_RAIN_DROPS_CONTROL);
	if (saved_rain_drops_control != current_rain_drops_control) {
		saved_rain_drops_control = current_rain_drops_control;

		rain_drops_factor = 0.f;
		steps_finished = 0;
	}

	if (!current_rain_drops_control)
		return;

	// Функция рассчитывает интенсивность эффекта капель на худе. В шейдере нормально рассчитать слишком муторно, проще посчитать здесь и получить в шейдере через c_timers.w
	auto update_rain_drops_factor = [](bool act_on_rain)
	{
		float rain_factor = g_pGamePersistent->Environment().CurrentEnv.rain_density;
		if (!fis_zero(rain_factor))
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

			if (steps_finished < (steps_count + 1)) {
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
				}
			}
			else if (act_on_rain) { //Если актор не находится в укрытии - синхронизируем rain_drops_factor с интенсивностью дождя
				rain_drops_factor = std::max(rain_drops_factor, rain_factor);
			}

			rain_drops_factor = std::clamp(rain_drops_factor, 0.f, rain_factor); //Уравниваем, чтобы не было превышения
		}
		else {
			steps_finished = 0;
			rain_drops_factor = 0.f;
		}
	};

	static bool actor_in_hideout = true;
	static u32 last_ray_pick_time = Device.dwTimeGlobal;
	if (Device.dwTimeGlobal > (last_ray_pick_time + 1000)) { //Апдейт рейтрейса - раз в секунду. Чаще апдейтить нет смысла.
		last_ray_pick_time = Device.dwTimeGlobal;

		collide::rq_result RQ;
		actor_in_hideout = !!g_pGameLevel->ObjectSpace.RayPick(Device.vCameraPosition, Fvector().set(0, 1, 0), 50.f, collide::rqtBoth, RQ, g_pGameLevel->CurrentViewEntity());
	}

	update_rain_drops_factor(!actor_in_hideout);

	//Msg("[%s] rain_drops_factor: [%f], rain_density: [%f]", __FUNCTION__, rain_drops_factor, g_pGamePersistent->Environment().CurrentEnv.rain_density);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	u32 Offset;
	Fvector2 p0, p1;

	// common 
	RCache.set_CullMode(CULL_NONE);
	RCache.set_Stencil(FALSE);

	struct v_simple {
		Fvector4	p;
		Fvector2	uv0;
	};

	float	_w = float(Device.dwWidth);
	float	_h = float(Device.dwHeight);
	p0.set(.5f / _w, .5f / _h);
	p1.set((_w + .5f) / _w, (_h + .5f) / _h);

	// Set RT's
	u_setrt(rt_Generic_0, 0, 0, HW.pBaseZB);

	// Fill vertex buffer
	v_simple* pv = (v_simple*)RCache.Vertex.Lock(4, g_KD->vb_stride, Offset);
	pv->p.set(EPS, float(_h + EPS), EPS, 1.f); pv->uv0.set(p0.x, p1.y); pv++;
	pv->p.set(EPS, EPS, EPS, 1.f); pv->uv0.set(p0.x, p0.y); pv++;
	pv->p.set(float(_w + EPS), float(_h + EPS), EPS, 1.f); pv->uv0.set(p1.x, p1.y); pv++;
	pv->p.set(float(_w + EPS), EPS, EPS, 1.f); pv->uv0.set(p1.x, p0.y); pv++;
	RCache.Vertex.Unlock(4, g_KD->vb_stride);

	// Draw COLOR
	RCache.set_Element(s_rain_drops->E[0]);

	RCache.set_c("c_timers", 0.f, 0.f, 0.f, rain_drops_factor);

	RCache.set_Geometry(g_KD);
	RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
	RCache.set_Stencil(FALSE);
};

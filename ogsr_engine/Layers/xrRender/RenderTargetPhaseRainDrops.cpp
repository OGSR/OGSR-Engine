#include "stdafx.h"

void CRenderTarget::PhaseRainDrops()
{
    if (Device.m_SecondViewport.IsSVPFrame()) //В прицеле не рендерим
        return;

    static float rain_drops_factor = 0.f;
    static u32 steps_finished = 0;

    // Чтобы по команде r2_rain_drops_control off/on эффект перезапускался.
    static bool saved_rain_drops_control = false;
    bool current_rain_drops_control = !!ps_r2_ls_flags_ext.test(R2FLAGEXT_RAIN_DROPS_CONTROL);
    if (saved_rain_drops_control != current_rain_drops_control)
    {
        saved_rain_drops_control = current_rain_drops_control;

        rain_drops_factor = 0.f;
        steps_finished = 0;
    }

    if (!current_rain_drops_control)
        return;

    // Функция рассчитывает интенсивность эффекта капель на худе. В шейдере нормально рассчитать слишком муторно, проще посчитать здесь и получить в шейдере через c_timers.w
    auto update_rain_drops_factor = [](bool act_on_rain) {
        float rain_factor = g_pGamePersistent->pEnvironment->CurrentEnv->rain_density;
        if (!fis_zero(rain_factor))
        {
            // В данном варианте настроек - при выходе из укрытия в шторм, капли заработают на полную мощность за 20 секунд. При заходе в укрытие - эффект отключится так же через
            // 20 секунд.
            constexpr u32 change_step = 200; //Интервал в миллисекундах между ступенями изменения rain_drops_factor
            constexpr u32 steps_count = 100; //Кол-во ступеней. Чем меньше интервал - тем больше ступеней должно быть.
            constexpr float step_rain_factor_change = 1.f / float(steps_count);

            static bool saved_rain_flag = act_on_rain;
            if (saved_rain_flag != act_on_rain)
            {
                saved_rain_flag = act_on_rain;
                steps_finished = 0;
            }

            if (steps_finished < (steps_count + 1))
            { // + 1 обязательно из за неровного деления. Иначе эффект при максимальном шторме может не до конца отключаться при входе в укрытие.
                static u32 last_update = Device.dwTimeGlobal;
                if (Device.dwTimeGlobal > (last_update + change_step))
                {
                    last_update = Device.dwTimeGlobal;
                    steps_finished++;
                    if (act_on_rain)
                    { //плавное повышение интенсивности капель.
                        rain_drops_factor += step_rain_factor_change;
                    }
                    else
                    { //плавное понижение интенсивности капель.
                        rain_drops_factor -= step_rain_factor_change;
                    }
                }
            }
            else if (act_on_rain)
            { //Если актор не находится в укрытии - синхронизируем rain_drops_factor с интенсивностью дождя
                rain_drops_factor = std::max(rain_drops_factor, rain_factor);
            }

            rain_drops_factor = std::clamp(rain_drops_factor, 0.f, rain_factor); //Уравниваем, чтобы не было превышения
        }
        else
        {
            steps_finished = 0;
            rain_drops_factor = 0.f;
        }
    };

    const bool actor_in_hideout = g_pGamePersistent->IsActorInHideout();

    update_rain_drops_factor(!actor_in_hideout);

    if (fis_zero(rain_drops_factor))
        return;

        // Msg("##[%s] rain_drops_factor: [%f], rain_density: [%f]", __FUNCTION__, rain_drops_factor, g_pGamePersistent->pEnvironment->CurrentEnv->rain_density);

#if defined(USE_DX10) || defined(USE_DX11)
    ref_rt& dest_rt = RImplementation.o.dx10_msaa ? rt_Generic : rt_Color;
#else
    ref_rt& dest_rt = rt_Generic_0;
#endif
    Fvector4 params{rain_drops_factor, ps_r2_rain_drops_intensity, ps_r2_rain_drops_speed, 0.0f};
    string_unordered_map<const char*, Fvector4*> consts{{"rain_drops_params", &params}};
    RenderScreenQuad(Device.dwWidth, Device.dwHeight, dest_rt, s_rain_drops->E[0], &consts);

#if defined(USE_DX10) || defined(USE_DX11)
    HW.pContext->CopyResource(rt_Generic_0->pTexture->surface_get(), dest_rt->pTexture->surface_get());
#endif
}

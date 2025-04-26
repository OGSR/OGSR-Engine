#include "stdafx.h"

// Screen Space Sun Shafts
void CRenderTarget::PhaseSSSS(CBackend& cmd_list)
{
    // Эти рендертаргеты используются в шейдере флар, потому очищать их необходимо.
    cmd_list.ClearRT(rt_sunshafts_1->pRT, {});
    cmd_list.ClearRT(rt_SunShaftsMaskSmoothed->pRT, {});

    if (!need_to_render_sunshafts())
        return;

     // Домножаем, т.к. интенсивность плоских саншафтов примерно соответствует объемным как 1 к 0.3. А конфиги погоды рассчитаны на объемные лучи же.
    const float intensity = g_pGamePersistent->Environment().CurrentEnv->m_fSunShaftsIntensity * 3.3f;
    const u32 mode = ps_r_sunshafts_mode & SS_SS_MASK;

    if (mode == SS_SS_MANOWAR)
    {
        PIX_EVENT(phase_SS_SS_MANOWAR);

        // Mask
        RenderScreenQuad(cmd_list, Device.dwWidth, Device.dwHeight, rt_SunShaftsMask, s_ssss_mrmnwar->E[0]);

        // Smoothed mask
        RenderScreenQuad(cmd_list, Device.dwWidth, Device.dwHeight, rt_SunShaftsMaskSmoothed, s_ssss_mrmnwar->E[1]);

        // Pass 0
        RenderScreenQuad(cmd_list, Device.dwWidth, Device.dwHeight, rt_SunShaftsPass0, s_ssss_mrmnwar->E[2], [&]() {
            cmd_list.set_c("ssss_params", ps_r_prop_ss_sample_step_phase0, ps_r_prop_ss_radius, 0.0f, 0.0f);
        });

        // Pass 1
        RenderScreenQuad(cmd_list, Device.dwWidth, Device.dwHeight, rt_SunShaftsMaskSmoothed, s_ssss_mrmnwar->E[3], [&]() {
            cmd_list.set_c("ssss_params", ps_r_prop_ss_sample_step_phase1, ps_r_prop_ss_radius, 0.0f, 0.0f);
        });

        // Combine
        RenderScreenQuad(cmd_list, Device.dwWidth, Device.dwHeight, rt_Generic_combine, s_ssss_mrmnwar->E[4], [&]() {
            cmd_list.set_c("ssss_params", intensity, ps_r_prop_ss_blend, 0.0f, 0.0f);
        });

        HW.get_context(cmd_list.context_id)->CopyResource(rt_Generic_0->pSurface, rt_Generic_combine->pSurface);
    }
    else if (mode == SS_SS_OGSE)
    {
        PIX_EVENT(phase_SS_SS_OGSE);

        // ***MASK GENERATION***
        // In this pass generates geometry mask
        RenderScreenQuad(cmd_list, Device.dwWidth, Device.dwHeight, rt_sunshafts_0, s_ssss_ogse->E[0]);

        // ***FIRST PASS***
        // First blurring pass
        RenderScreenQuad(cmd_list, Device.dwWidth, Device.dwHeight, rt_sunshafts_1, s_ssss_ogse->E[1], [&]() {
            cmd_list.set_c("ssss_params", intensity, ps_r_ss_sunshafts_length, 1.0f, ps_r_ss_sunshafts_radius);
        });

        //***SECOND PASS***
        // Second blurring pass
        RenderScreenQuad(cmd_list, Device.dwWidth, Device.dwHeight, rt_sunshafts_0, s_ssss_ogse->E[2], [&]() {
            cmd_list.set_c("ssss_params", intensity, ps_r_ss_sunshafts_length, 0.7f, ps_r_ss_sunshafts_radius);
        });

        //***THIRD PASS***
        // Third blurring pass
        RenderScreenQuad(cmd_list, Device.dwWidth, Device.dwHeight, rt_sunshafts_1, s_ssss_ogse->E[3], [&]() {
            cmd_list.set_c("ssss_params", intensity, ps_r_ss_sunshafts_length, 0.3f, ps_r_ss_sunshafts_radius);
        });

        //***BLEND PASS***
        // Combining sunshafts texture and image for further processing
        RenderScreenQuad(cmd_list, Device.dwWidth, Device.dwHeight, rt_Generic_combine, s_ssss_ogse->E[4], [&]() {
            cmd_list.set_c("ssss_params", intensity, ps_r_ss_sunshafts_length, 0.0f, ps_r_ss_sunshafts_radius);
        });

        HW.get_context(cmd_list.context_id)->CopyResource(rt_Generic_0->pSurface, rt_Generic_combine->pSurface);
    }
}

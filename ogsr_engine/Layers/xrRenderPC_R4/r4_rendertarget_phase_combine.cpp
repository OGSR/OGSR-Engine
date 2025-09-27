#include "stdafx.h"
#include "../../xr_3da/igame_persistent.h"
#include "../../xr_3da/environment.h"
#include "../../xr_3da/xr_efflensflare.h"
#include "../../xr_3da/CustomHUD.h"
#include "../xrRender/dxEnvironmentRender.h"

void CRenderTarget::phase_combine(CBackend& cmd_list)
{
    ZoneScoped;

    PIX_EVENT(phase_combine);

    //*** exposure-pipeline
    {
        PIX_EVENT(exposure_pipeline);
        // if (t_LUM_src != rt_LUM_pool[0]->pTexture)
            t_LUM_src->surface_set(rt_LUM_pool[0]->pSurface);
        // if (t_LUM_dest != rt_LUM_pool[1]->pTexture)
            t_LUM_dest->surface_set(rt_LUM_pool[1]->pSurface);
    }

    u_setrt(cmd_list, rt_Generic_0, nullptr, nullptr, nullptr, rt_Base_Depth->pZRT[cmd_list.context_id]);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    {
        PIX_EVENT(CLOUDS_RENDER);

        g_pGamePersistent->Environment().RenderClouds(cmd_list);
    }

    cmd_list.set_Stencil(TRUE, D3DCMP_LESSEQUAL, 0x01, 0xff, 0x00); // stencil should be >= 1

    // Draw full-screen quad textured with our scene image
    {
        PIX_EVENT(combine_1);

        // Compute params
        CEnvDescriptorMixer& envdesc = *g_pGamePersistent->Environment().CurrentEnv;
        const float minamb = ps_pnv_mode == 1 ? 0.005f : 0.001f;
        Fvector4 ambclr = {std::max(envdesc.ambient.x * 2, minamb), std::max(envdesc.ambient.y * 2, minamb), std::max(envdesc.ambient.z * 2, minamb), 0};
        ambclr.mul(ps_r2_sun_lumscale_amb);

        Fvector4 envclr;

        if (g_pGamePersistent->Environment().USED_COP_WEATHER)
            envclr = {envdesc.hemi_color_unmodded.x * 2 + EPS, envdesc.hemi_color_unmodded.y * 2 + EPS, envdesc.hemi_color_unmodded.z * 2 + EPS, envdesc.weight};
        else
            envclr = {envdesc.sky_color.x * 2 + EPS, envdesc.sky_color.y * 2 + EPS, envdesc.sky_color.z * 2 + EPS, envdesc.weight};

        Fvector4 fogclr = {envdesc.fog_color.x, envdesc.fog_color.y, envdesc.fog_color.z, 0};
        envclr.x *= 2 * ps_r2_sun_lumscale_hemi;
        envclr.y *= 2 * ps_r2_sun_lumscale_hemi;
        envclr.z *= 2 * ps_r2_sun_lumscale_hemi;
        Fvector4 sunclr, sundir;

        // sun-params
        {
            light* fuckingsun = smart_cast<light*>(RImplementation.Lights.sun_adapted._get());
            Fvector L_dir, L_clr;
            float L_spec;
            L_clr.set(fuckingsun->color.r, fuckingsun->color.g, fuckingsun->color.b);
            L_spec = u_diffuse2s(L_clr);
            Device.mView.transform_dir(L_dir, fuckingsun->direction);
            L_dir.normalize();

            sunclr.set(L_clr.x, L_clr.y, L_clr.z, L_spec);
            sundir.set(L_dir.x, L_dir.y, L_dir.z, 0);
        }

        // Fill VB
        float scale_X = float(Device.dwWidth) / float(TEX_jitter);
        float scale_Y = float(Device.dwHeight) / float(TEX_jitter);

        u32 Offset{};

        // Fill vertex buffer
        FVF::TL* pv = (FVF::TL*)RImplementation.Vertex.Lock(4, g_combine->vb_stride, Offset);
        pv->set(-1, 1, 0, 1, 0, 0, scale_Y);
        pv++;
        pv->set(-1, -1, 0, 0, 0, 0, 0);
        pv++;
        pv->set(1, 1, 1, 1, 0, scale_X, scale_Y);
        pv++;
        pv->set(1, -1, 1, 0, 0, scale_X, 0);
        pv++;
        RImplementation.Vertex.Unlock(4, g_combine->vb_stride);

        dxEnvDescriptorMixerRender& envdescren = *smart_cast<dxEnvDescriptorMixerRender*>(&*envdesc.m_pDescriptorMixer);

        // Setup textures
        ID3DBaseTexture* e0 = envdescren.sky_r_textures_env[0].second->surface_get();
        ID3DBaseTexture* e1 = envdescren.sky_r_textures_env[1].second->surface_get();
        t_envmap_0->surface_set(e0);
        t_envmap_1->surface_set(e1);

        // Draw
        cmd_list.set_Element(s_combine->E[0]);
        cmd_list.set_Geometry(g_combine);

        cmd_list.set_c("m_inv_v", Device.mInvView);

        cmd_list.set_c("m_v2w", Device.mInvView);
        cmd_list.set_c("L_ambient", ambclr);

        cmd_list.set_c("Ldynamic_color", sunclr);
        cmd_list.set_c("Ldynamic_dir", sundir);

        cmd_list.set_c("env_color", envclr);
        cmd_list.set_c("fog_color", fogclr);

        cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    }

    // Copy rt_Generic_0 to new RT
    HW.get_context(cmd_list.context_id)->CopyResource(rt_Generic_0_temp->pSurface, rt_Generic_0->pSurface);

    // Forward rendering
    {
        PIX_EVENT(Forward_rendering);

        u_setrt(cmd_list, rt_Generic_0, rt_heat, nullptr, nullptr, rt_Base_Depth->pZRT[cmd_list.context_id]); // LDR RT

        cmd_list.set_CullMode(CULL_CCW);
        cmd_list.set_Stencil(FALSE);
        cmd_list.set_ColorWriteEnable();

        RImplementation.render_forward();
    }

    auto& dsgraph = RImplementation.get_imm_context();

    if (g_hud && g_hud->RenderActiveItemUIQuery())
        dsgraph.r_dsgraph_render_hud_ui();

    //	Igor: for volumetric lights
    //	combine light volume here
    if (m_bHasActiveVolumetric)
        phase_combine_volumetric(cmd_list);

    // Distortion filter
    if (RImplementation.o.distortion_enabled && !dsgraph.mapDistort.empty())
    {
        {
            PIX_EVENT(render_distort_objects);
            u_setrt(cmd_list, rt_Generic_1, nullptr, nullptr, nullptr, rt_Base_Depth->pZRT[cmd_list.context_id]); // Now RT is a distortion mask
            cmd_list.ClearRT(rt_Generic_1, color_rgba(127, 127, 0, 127));
            cmd_list.set_CullMode(CULL_CCW);
            cmd_list.set_Stencil(FALSE);
            cmd_list.set_ColorWriteEnable();
            dsgraph.r_dsgraph_render_distort();
        }

        {
            PIX_EVENT(combine_distort);
            RenderScreenQuad(cmd_list, Device.dwWidth, Device.dwHeight, rt_Generic_combine, s_combine->E[1]);
            HW.get_context(cmd_list.context_id)->CopyResource(rt_Generic_0->pSurface, rt_Generic_combine->pSurface);
        }
    }

    {
        // Perform blooming filter and distortion if needed
        cmd_list.set_Stencil(FALSE);
        phase_bloom(cmd_list); // HDR RT invalidated here
    }

    {
        PIX_EVENT(combine_tonemap);
        RenderScreenQuad(cmd_list, Device.dwWidth, Device.dwHeight, rt_Generic_combine, s_combine->E[2]);
        HW.get_context(cmd_list.context_id)->CopyResource(rt_Generic_0->pSurface, rt_Generic_combine->pSurface);
    }

    const bool need_heatvision = (ps_pnv_mode == 2 || ps_pnv_mode == 3);
    const bool need_3dss = !dsgraph.mapScopeHUDSorted.empty();
    bool upscaled_3dss{};

    if (need_heatvision) // должен быть до 3DSS
    {
        phase_heatvision(cmd_list);
    }

    if (ps_r_pp_aa_mode) // должно быть перед 3DSS
    {
        upscaled_3dss = (!need_heatvision && need_3dss && Phase3DSSUpscale(cmd_list));

        if (!upscaled_3dss || ps_r2_ls_flags_ext.test(R2FLAGEXT_DLSS_3DSS_USE_SECOND_PASS))
        {
            PhaseAA(cmd_list); // anti - aliasing
        }
    }

    if (!need_heatvision) // должны быть до 3DSS
    { // Screen space sunshafts
#pragma todo("Simp: Ощущение что есть микроподергивания лучей (из-за TAA jitter?)")
        PIX_EVENT(phase_ss_ss);
        PhaseSSSS(cmd_list);
    }

    if (need_3dss)
    {
        PIX_EVENT(phase_3DSSReticle);

        u_setrt(cmd_list, rt_Generic_0, nullptr, nullptr, nullptr, get_base_zb());

        cmd_list.set_CullMode(CULL_CCW);
        cmd_list.set_Stencil(FALSE);
        cmd_list.set_ColorWriteEnable();

        dsgraph.r_dsgraph_render_scope_sorted(upscaled_3dss);
    }

    // Compute blur textures
    phase_blur(cmd_list);

#pragma todo("Simp: Если отключить новый блум, то старый, который рендерится до прицела, багует, потому что рендрится очень рано. Пока запретил отключение нового блума.")
    if (ps_r2_ls_flags.test(R2FLAG_SSFX_BLOOM))
        phase_ssfx_bloom(cmd_list);

    if (!need_heatvision)
        phase_dof(cmd_list);

    if (!need_heatvision)
        phase_lut(cmd_list);

/*
    if (scope_fake_radius > 0.f)
        phase_fakescope(cmd_list); // crookr
*/

#pragma todo("Simp: эффект шлема багует в прицеле, надо бы придумать решение получше")
    extern bool Is3dssZoomed;
    if (!Is3dssZoomed &&
        ((ps_r2_ls_flags_ext.test(R2FLAGEXT_VISOR_REFL_CONTROL) && ps_r2_ls_flags_ext.test(R2FLAGEXT_VISOR_REFL)) ||
         (ps_r2_ls_flags_ext.test(R2FLAGEXT_MASK_CONTROL) && ps_r2_ls_flags_ext.test(R2FLAGEXT_MASK))))
    {
        phase_gasmask_dudv(cmd_list);
    }

    if (ps_pnv_mode == 1) // должен быть после дофа и после шлема
        phase_nightvision(cmd_list);

    // Rain droplets on screen
    if (ps_r2_ls_flags_ext.test(R2FLAGEXT_RAIN_DROPS))
        phase_rain_drops(cmd_list);

    {
        PIX_EVENT(combine_2);

        Fmatrix m_previous{}, m_current{};

        static Fmatrix m_saved_viewproj;

        // (new-camera) -> (world) -> (old_viewproj)
        m_previous.mul(m_saved_viewproj, Device.mInvView);
        m_current.set(Device.mProject);
        m_saved_viewproj.set(Device.mFullTransform);

        Fvector2 m_blur_scale{};
        if (!Is3dssZoomed)
        {
            const float scale = ps_r2_mblur / 2.f;
            m_blur_scale.set(scale, -scale).div(12.f);
        }

        RenderScreenQuad(cmd_list, Device.dwWidth, Device.dwHeight, rt_Generic_combine, s_combine->E[3], [&]() {
            cmd_list.set_c("m_current", m_current);
            cmd_list.set_c("m_previous", m_previous);
            cmd_list.set_c("m_blur", m_blur_scale.x, m_blur_scale.y, 0, 0);
        });

        HW.get_context(cmd_list.context_id)->CopyResource(rt_Generic_0->pSurface, rt_Generic_combine->pSurface);
    }

    {
        PIX_EVENT(RenderFlares);
        cmd_list.set_Stencil(FALSE);
        g_pGamePersistent->Environment().RenderFlares(cmd_list); // lens-flares
    }

    //	PP-if required
    {
        PIX_EVENT(phase_pp);
        phase_pp(cmd_list);
    }

    {
        PIX_EVENT(exposure_pipeline_clear);
        //	Re-adapt luminance
        cmd_list.set_Stencil(FALSE);

        //*** exposure-pipeline-clear
        {
            std::swap(rt_LUM_pool[0], rt_LUM_pool[1]);

            t_LUM_src->surface_set(nullptr);
            t_LUM_dest->surface_set(nullptr);
        }
    }

    phase_flares(cmd_list);

    u_setrt(cmd_list, Device.dwWidth, Device.dwHeight, get_base_rt(), nullptr, nullptr, get_base_zb()); /// TODO: check cmd_list

#ifdef DEBUG
    cmd_list.set_CullMode(CULL_CCW);
    static xr_vector<Fplane> saved_dbg_planes;
    if (bDebug)
        saved_dbg_planes = dbg_planes;
    else
        dbg_planes = saved_dbg_planes;
    if (1)
        for (u32 it = 0; it < dbg_planes.size(); it++)
        {
            Fplane& P = dbg_planes[it];
            Fvector zero;
            zero.mul(P.n, P.d);

            Fvector L_dir, L_up = P.n, L_right;
            L_dir.set(0, 0, 1);
            if (_abs(L_up.dotproduct(L_dir)) > .99f)
                L_dir.set(1, 0, 0);
            L_right.crossproduct(L_up, L_dir);
            L_right.normalize();
            L_dir.crossproduct(L_right, L_up);
            L_dir.normalize();

            Fvector p0, p1, p2, p3;
            float sz = 100.f;
            p0.mad(zero, L_right, sz).mad(L_dir, sz);
            p1.mad(zero, L_right, sz).mad(L_dir, -sz);
            p2.mad(zero, L_right, -sz).mad(L_dir, -sz);
            p3.mad(zero, L_right, -sz).mad(L_dir, +sz);
            cmd_list.dbg_DrawTRI(Fidentity, p0, p1, p2, 0xffffffff);
            cmd_list.dbg_DrawTRI(Fidentity, p2, p3, p0, 0xffffffff);
        }

    static xr_vector<dbg_line_t> saved_dbg_lines;
    if (bDebug)
        saved_dbg_lines = dbg_lines;
    else
        dbg_lines = saved_dbg_lines;
    if (1)
        for (u32 it = 0; it < dbg_lines.size(); it++)
        {
            cmd_list.dbg_DrawLINE(Fidentity, dbg_lines[it].P0, dbg_lines[it].P1, dbg_lines[it].color);
        }
#endif

#ifdef DEBUG
    dbg_spheres.clear();
    dbg_lines.clear();
    dbg_planes.clear();
#endif
}

void CRenderTarget::phase_wallmarks(CBackend& cmd_list)
{
    // Targets
    u_setrt(cmd_list, rt_Color, nullptr, nullptr, nullptr, rt_Base_Depth->pZRT[cmd_list.context_id]);
    // Stencil	- draw only where stencil >= 0x1
    cmd_list.set_Stencil(TRUE, D3DCMP_LESSEQUAL, 0x01, 0xff, 0x00);
    cmd_list.set_CullMode(CULL_CCW);
    cmd_list.set_ColorWriteEnable(D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
}

void CRenderTarget::phase_combine_volumetric(CBackend& cmd_list)
{
    PIX_EVENT(phase_combine_volumetric);

    u32 Offset = 0;

    u_setrt(cmd_list, rt_Generic_0, rt_Generic_1, nullptr, nullptr, rt_Base_Depth->pZRT[cmd_list.context_id]);

    //	Sets limits to both render targets
    cmd_list.set_ColorWriteEnable(D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
    {
        // Fill VB
        float scale_X = float(Device.dwWidth) / float(TEX_jitter);
        float scale_Y = float(Device.dwHeight) / float(TEX_jitter);

        // Fill vertex buffer
        FVF::TL* pv = (FVF::TL*)RImplementation.Vertex.Lock(4, g_combine->vb_stride, Offset);
        pv->set(-1, 1, 0, 1, 0, 0, scale_Y);
        pv++;
        pv->set(-1, -1, 0, 0, 0, 0, 0);
        pv++;
        pv->set(1, 1, 1, 1, 0, scale_X, scale_Y);
        pv++;
        pv->set(1, -1, 1, 0, 0, scale_X, 0);
        pv++;
        RImplementation.Vertex.Unlock(4, g_combine->vb_stride);

        // Draw
        cmd_list.set_Element(s_combine_volumetric->E[0]);
        // cmd_list.set_Geometry			(g_combine_VP		);
        cmd_list.set_Geometry(g_combine);
        cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    }
    cmd_list.set_ColorWriteEnable();
}

void CRenderTarget::phase_flares(CBackend& cmd_list)
{
    if (!rt_flares)
        return;

    // clear
    u_setrt(cmd_list, rt_flares, nullptr, nullptr, nullptr, rt_Base_Depth->pZRT[cmd_list.context_id]);

    // Targets
    if (dwFlareClearMark != Device.dwFrame)
    {
        // initial setup
        dwFlareClearMark = Device.dwFrame;

        cmd_list.ClearRT(rt_flares->pRT, {});            
    }
}

void CRenderTarget::render_flare(CBackend& cmd_list, light* L)
{
    if (!rt_flares)
        return;

    Fvector vLightDir{}, vecY{};
    vLightDir.sub(L->position, Device.vCameraPosition);

    // Calculate the point directly in front of us, on the far clip plane
    float fDistance = vLightDir.magnitude();

    Fmatrix matEffCamPos;
    matEffCamPos.identity();

    // Calculate our position and direction
    matEffCamPos.i.set(Device.vCameraRight);
    matEffCamPos.j.set(Device.vCameraTop);
    matEffCamPos.k.set(Device.vCameraDirection);

    Fvector vecDir = {0.0f, 0.0f, 1.0f};
    matEffCamPos.transform_dir(vecDir);
    vecDir.normalize();

    // Figure out if light is behind something else
    Fvector vecX = {1.0f, 0.0f, 0.0f};
    matEffCamPos.transform_dir(vecX);
    vecX.normalize();
    vecY.crossproduct(vecX, vecDir);
    vLightDir.normalize();

    // Figure out of light (or flare) might be visible
    float fDot = vLightDir.dotproduct(vecDir);
    if (fDot <= 0.01f)
    {
        L->fBlend = 0.f;
        return;
    }

    float cur_dist = fDistance;
    {
        L->FlareRayPick.RayPickSubmit(Device.vCameraPosition, vLightDir, cur_dist, collide::rqtBoth, g_pGameLevel->CurrentViewEntity());

        collide::rq_result RQ;
        if (L->FlareRayPick.Ready(RQ))
        {
            cur_dist = RQ.range;
        }
    }
    bool hide = (fDistance - cur_dist) > 0.2f;

    blend_lerp(L->fBlend, hide ? 0.0f : 1.0f, hide ? 8.0f : 4.0f, Device.fTimeDelta);
    clamp(L->fBlend, 0.0f, 1.0f);

    if (L->fBlend <= EPS_L)
    {
        return;
    }

    phase_flares(cmd_list);

    Fvector vecLight = L->position;

    Fvector scr_pos{};
    Device.mFullTransform.transform(scr_pos, vecLight);

    scr_pos.y *= -1.0f;

    const float sun_blend = 0.5f;
    const float sun_max = 2.5f;

    Fvector2 kXY{1.f, 1.f};
    if (_abs(scr_pos.x) > sun_blend)
        kXY.x = (sun_max - _abs(scr_pos.x)) / (sun_max - sun_blend);
    if (_abs(scr_pos.y) > sun_blend)
        kXY.y = (sun_max - _abs(scr_pos.y)) / (sun_max - sun_blend);

    float fGradientValue = 0;
    if (!((_abs(scr_pos.x) > sun_max) || (_abs(scr_pos.y) > sun_max)))
        fGradientValue = kXY.x * kXY.y * L->fBlend;

    Fvector vecSx{}, vecSy{};
    vecSx.mul(vecX, fGradientValue * (1.f + 0.02f * fDistance) * 3.f);
    vecSy.mul(vecY, fGradientValue * (1.f + 0.02f * fDistance) * 3.f);

    Fcolor color = L->color;
    color.normalize_rgb();
    color.mul_rgba(fGradientValue /* * L->fBlend*/);
    u32 c = color.get();

    u32 VS_Offset;
    FVF::LIT* pv = (FVF::LIT*)RImplementation.Vertex.Lock(4, g_flare.stride(), VS_Offset);

    pv->set(vecLight.x + vecSx.x - vecSy.x, vecLight.y + vecSx.y - vecSy.y, vecLight.z + vecSx.z - vecSy.z, c, 0, 0);
    pv++;
    pv->set(vecLight.x + vecSx.x + vecSy.x, vecLight.y + vecSx.y + vecSy.y, vecLight.z + vecSx.z + vecSy.z, c, 0, 1);
    pv++;
    pv->set(vecLight.x - vecSx.x - vecSy.x, vecLight.y - vecSx.y - vecSy.y, vecLight.z - vecSx.z - vecSy.z, c, 1, 0);
    pv++;
    pv->set(vecLight.x - vecSx.x + vecSy.x, vecLight.y - vecSx.y + vecSy.y, vecLight.z - vecSx.z + vecSy.z, c, 1, 1);
    pv++;

    RImplementation.Vertex.Unlock(4, g_flare.stride());

    cmd_list.set_xform_world(Fidentity);
    cmd_list.set_Geometry(g_flare);
    cmd_list.set_Stencil(FALSE);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Shader(s_flare);

    float intensity_invert;

    if (cur_dist > 5.0f)
        intensity_invert = std::lerp(0.0f, 1.0f, cur_dist / 100.f);
    else
        intensity_invert = 1 - std::lerp(0.0f, 1.0f, cur_dist / 5.f);


    cmd_list.set_c("flare_params", intensity_invert, L->flags.bFlare ? 1.f : 0.f, 0.f, 0.f);
    cmd_list.set_c("flare_color", L->color.r, L->color.g, L->color.b, L->color.a);


    cmd_list.Render(D3DPT_TRIANGLELIST, VS_Offset, 0, 4, 0, 2);
}
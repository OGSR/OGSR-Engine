#include "stdafx.h"

#pragma pack(push, 4)
struct v_build
{
    Fvector4 p;
    Fvector2 uv0;
    Fvector2 uv1;
    Fvector2 uv2;
    Fvector2 uv3;
};
#pragma pack(pop)

void CRenderTarget::phase_bloom(CBackend& cmd_list)
{
    PIX_EVENT(phase_bloom);
    u32 Offset;

    // Targets
    u_setrt(cmd_list, rt_Bloom_1, nullptr, nullptr, nullptr, nullptr); // No need for ZBuffer at all

    // Clear - don't clear - it's stupid here :)
    cmd_list.set_Z(FALSE);

    // Transfer into Bloom1
    {
        float _w = float(GetRenderWidth());
        float _h = float(GetRenderHeight());
        float _2w = _w / 2;
        float tw = BLOOM_size_X;
        float _2h = _h / 2;
        float th = BLOOM_size_Y;
        float _aspect_w = _2w / tw;
        float _aspect_h = _2h / th;
        Fvector2 one = {1.f / _w, 1.f / _h};
        one.x *= _aspect_w;
        one.y *= _aspect_h;
        Fvector2 half = {.5f / _w, .5f / _h};
        Fvector2 a_0 = {half.x + 0, half.y + 0};
        Fvector2 a_1 = {half.x + one.x, half.y + 0};
        Fvector2 a_2 = {half.x + 0, half.y + one.y};
        Fvector2 a_3 = {half.x + one.x, half.y + one.y};
        Fvector2 b_0 = {1 + a_0.x, 1 + a_0.y};
        Fvector2 b_1 = {1 + a_1.x, 1 + a_1.y};
        Fvector2 b_2 = {1 + a_2.x, 1 + a_2.y};
        Fvector2 b_3 = {1 + a_3.x, 1 + a_3.y};

        // Fill vertex buffer
        v_build* pv = (v_build*)RImplementation.Vertex.Lock(4, g_bloom_build->vb_stride, Offset);
        pv->p.set(EPS, float(th + EPS), EPS, 1.f);
        pv->uv0.set(a_0.x, b_0.y);
        pv->uv1.set(a_1.x, b_1.y);
        pv->uv2.set(a_2.x, b_2.y);
        pv->uv3.set(a_3.x, b_3.y);
        pv++;
        pv->p.set(EPS, EPS, EPS, 1.f);
        pv->uv0.set(a_0.x, a_0.y);
        pv->uv1.set(a_1.x, a_1.y);
        pv->uv2.set(a_2.x, a_2.y);
        pv->uv3.set(a_3.x, a_3.y);
        pv++;
        pv->p.set(float(tw + EPS), float(th + EPS), EPS, 1.f);
        pv->uv0.set(b_0.x, b_0.y);
        pv->uv1.set(b_1.x, b_1.y);
        pv->uv2.set(b_2.x, b_2.y);
        pv->uv3.set(b_3.x, b_3.y);
        pv++;
        pv->p.set(float(tw + EPS), EPS, EPS, 1.f);
        pv->uv0.set(b_0.x, a_0.y);
        pv->uv1.set(b_1.x, a_1.y);
        pv->uv2.set(b_2.x, a_2.y);
        pv->uv3.set(b_3.x, a_3.y);
        pv++;
        RImplementation.Vertex.Unlock(4, g_bloom_build->vb_stride);

        // Perform combine (all scalers must account for 4 samples + final diffuse multiply);
        float s = ps_r2_ls_bloom_threshold; // scale
        f_bloom_factor = .9f * f_bloom_factor + .1f * ps_r2_ls_bloom_speed * Device.fTimeDelta; // speed
        cmd_list.set_Element(s_bloom->E[0]);
        cmd_list.set_c("b_params", s, s, s, f_bloom_factor);
        cmd_list.set_Geometry(g_bloom_build);
        cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    }

    // Capture luminance values
    phase_luminance(cmd_list);
}

static void set_viewport_size(CBackend& cmd_list, const float w, const float h)
{
    const D3D_VIEWPORT viewport{0.f, 0.f, w, h, 0.f, 1.f};
    cmd_list.SetViewport(viewport);
}

void CRenderTarget::phase_ssfx_bloom(CBackend& cmd_list)
{
    PIX_EVENT(phase_ssfx_bloom);

    // Constants
    const float w = float(Device.dwWidth);
    const float h = float(Device.dwHeight);
    const bool mask_control = ps_r2_ls_flags_ext.test(R2FLAGEXT_MASK) && ps_r2_ls_flags_ext.test(R2FLAGEXT_MASK_CONTROL);
    const bool visor_control = ps_r2_ls_flags_ext.test(R2FLAGEXT_VISOR_REFL) && ps_r2_ls_flags_ext.test(R2FLAGEXT_VISOR_REFL_CONTROL);

    // BLOOM BUILD ////////////////////////////////////////////////////
    // Half resolution is the max size for everything
    set_viewport_size(cmd_list, w / 2.0f, h / 2.0f);
    RenderScreenTriangle(cmd_list, rt_ssfx_bloom1, s_ssfx_bloom->E[0]);

    // BLOOM LENS /////////////////////////////////////////////////////
    if (mask_control)
    {
        set_viewport_size(cmd_list, w / 4.0f, h / 4.0f);
        RenderScreenTriangle(cmd_list, rt_ssfx_bloom_tmp4, s_ssfx_bloom_lens->E[0]);

        // Lens 2 Phase blur
        ref_rt* rt_LensBlur[2] = {&rt_ssfx_bloom_tmp4_2, &rt_ssfx_bloom_lens};
        for (int lensblur = 0; lensblur < 2; lensblur++)
        {
            RenderScreenTriangle(cmd_list, *rt_LensBlur[lensblur], s_ssfx_bloom_lens->E[1 + lensblur],
                                 [&]() { cmd_list.set_c("blur_setup", w / 4, h / 4, 0, 2.0f + (3.0f * lensblur)); });
        }
    }

    // BLOOM DOWNSAMPLE ///////////////////////////////////////////////
    ref_rt* rt_Down[6] = {&rt_ssfx_bloom_tmp2, &rt_ssfx_bloom_tmp4, &rt_ssfx_bloom_tmp8, &rt_ssfx_bloom_tmp16, &rt_ssfx_bloom_tmp32, &rt_ssfx_bloom_tmp64};
    for (int downsample = 0; downsample < 6; downsample++)
    {
        int SampleScale = 1 << (downsample + 1);
        set_viewport_size(cmd_list, w / SampleScale, h / SampleScale);
        RenderScreenTriangle(cmd_list, *rt_Down[downsample], s_ssfx_bloom_downsample->E[downsample],
                             [&]() { cmd_list.set_c("blur_setup", w / SampleScale, h / SampleScale, 0, ps_ssfx_bloom_2.x); });
    }

    // BLOOM UPSAMPLE /////////////////////////////////////////////////
    ref_rt* rt_Up[5] = {&rt_ssfx_bloom_tmp32_2, &rt_ssfx_bloom_tmp16_2, &rt_ssfx_bloom_tmp8_2, &rt_ssfx_bloom_tmp4_2, &rt_ssfx_bloom_tmp2};
    for (int upsample = 0; upsample < 5; upsample++)
    {
        int SampleScale = 1 << (5 - upsample);
        set_viewport_size(cmd_list, w / SampleScale, h / SampleScale);
        RenderScreenTriangle(cmd_list, *rt_Up[upsample], s_ssfx_bloom_upsample->E[upsample],
                             [&]() { cmd_list.set_c("blur_setup", w / SampleScale, h / SampleScale, 0, ps_ssfx_bloom_2.x); });
    }

    // BLOOM COMBINE ///////////////////////////////////////////////
    RenderScreenTriangle(cmd_list, rt_ssfx_bloom1, s_ssfx_bloom->E[1], [&]() {
        cmd_list.set_c("mask_control", static_cast<float>(mask_control), static_cast<float>(visor_control), ps_r2_visor_refl_intensity, ps_r2_visor_refl_radius);
    });

    // Restore Viewport
    set_viewport_size(cmd_list, (float)Device.dwWidth, (float)Device.dwHeight);
}

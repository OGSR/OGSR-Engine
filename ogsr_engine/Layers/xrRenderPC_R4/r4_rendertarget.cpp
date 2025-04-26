#include "stdafx.h"

#include "../xrRender/resourcemanager.h"

#include "blender_light_occq.h"
#include "blender_light_mask.h"
#include "blender_light_direct.h"
#include "blender_light_point.h"
#include "blender_light_spot.h"
#include "blender_bloom_build.h"
#include "blender_luminance.h"
#include "blender_gasmask_dudv.h"
#include "blender_fakescope.h"
#include "blender_blur.h"
#include "blender_dof.h"
#include "blender_nightvision.h"
#include "blender_thermalvision.h"

#include "../xrRenderDX10/DX10 Rain/blender_rain.h"
#include "../xrRender/dxRenderDeviceRender.h"

void CRenderTarget::u_setrt(CBackend& cmd_list, const ref_rt& _1, const ref_rt& _2, const ref_rt& _3, const ref_rt& _4, ID3DDepthStencilView* zb)
{
    VERIFY(_1 || zb);
    if (_1)
    {
        dwWidth[cmd_list.context_id] = _1->dwWidth;
        dwHeight[cmd_list.context_id] = _1->dwHeight;
    }
    else
    {
        D3D_DEPTH_STENCIL_VIEW_DESC desc;
        zb->GetDesc(&desc);

        VERIFY(desc.ViewDimension == D3D_DSV_DIMENSION_TEXTURE2D);

        ID3DResource* pRes;

        zb->GetResource(&pRes);

        ID3DTexture2D* pTex = (ID3DTexture2D*)pRes;

        D3D_TEXTURE2D_DESC TexDesc;

        pTex->GetDesc(&TexDesc);

        dwWidth[cmd_list.context_id] = TexDesc.Width;
        dwHeight[cmd_list.context_id] = TexDesc.Height;
        _RELEASE(pRes);
    }

    cmd_list.set_RT(_1 ? _1->pRT : nullptr, 0);
    cmd_list.set_RT(_2 ? _2->pRT : nullptr, 1);
    cmd_list.set_RT(_3 ? _3->pRT : nullptr, 2);
    cmd_list.set_RT(_4 ? _4->pRT : nullptr, 3);

    cmd_list.set_ZB(zb);
}

void CRenderTarget::u_setrt(CBackend& cmd_list, const ref_rt& _1, const ref_rt& _2, const ref_rt& _3, ID3DDepthStencilView* zb)
{
    VERIFY(_1 || zb);
    if (_1)
    {
        dwWidth[cmd_list.context_id] = _1->dwWidth;
        dwHeight[cmd_list.context_id] = _1->dwHeight;
    }
    else
    {
        D3D_DEPTH_STENCIL_VIEW_DESC desc;
        zb->GetDesc(&desc);

        VERIFY(desc.ViewDimension == D3D_DSV_DIMENSION_TEXTURE2D);            

        ID3DResource* pRes;

        zb->GetResource(&pRes);

        ID3DTexture2D* pTex = (ID3DTexture2D*)pRes;

        D3D_TEXTURE2D_DESC TexDesc;

        pTex->GetDesc(&TexDesc);

        dwWidth[cmd_list.context_id] = TexDesc.Width;
        dwHeight[cmd_list.context_id] = TexDesc.Height;
        _RELEASE(pRes);
    }

    cmd_list.set_RT(_1 ? _1->pRT : nullptr, 0);
    cmd_list.set_RT(_2 ? _2->pRT : nullptr, 1);
    cmd_list.set_RT(_3 ? _3->pRT : nullptr, 2);

    cmd_list.set_ZB(zb);
}

void CRenderTarget::u_setrt(CBackend& cmd_list, const ref_rt& _1, const ref_rt& _2, ID3DDepthStencilView* zb)
{
    VERIFY(_1 || zb);
    if (_1)
    {
        dwWidth[cmd_list.context_id] = _1->dwWidth;
        dwHeight[cmd_list.context_id] = _1->dwHeight;
    }
    else
    {
        D3D_DEPTH_STENCIL_VIEW_DESC desc;
        zb->GetDesc(&desc);
        VERIFY(desc.ViewDimension == D3D_DSV_DIMENSION_TEXTURE2D);

        ID3DResource* pRes;

        zb->GetResource(&pRes);

        ID3DTexture2D* pTex = (ID3DTexture2D*)pRes;

        D3D_TEXTURE2D_DESC TexDesc;

        pTex->GetDesc(&TexDesc);

        dwWidth[cmd_list.context_id] = TexDesc.Width;
        dwHeight[cmd_list.context_id] = TexDesc.Height;
        _RELEASE(pRes);
    }

    cmd_list.set_RT(_1 ? _1->pRT : nullptr, 0);
    cmd_list.set_RT(_2 ? _2->pRT : nullptr, 1);

    cmd_list.set_ZB(zb);
}

void CRenderTarget::u_setrt(CBackend& cmd_list, u32 W, u32 H, ID3DRenderTargetView* _1, ID3DRenderTargetView* _2, ID3DRenderTargetView* _3, ID3DDepthStencilView* zb)
{
    dwWidth[cmd_list.context_id] = W;
    dwHeight[cmd_list.context_id] = H;

    cmd_list.set_RT(_1, 0);
    cmd_list.set_RT(_2, 1);
    cmd_list.set_RT(_3, 2);
    cmd_list.set_ZB(zb);
}

// 2D texgen (texture adjustment matrix)
void CRenderTarget::u_compute_texgen_screen(CBackend& cmd_list, Fmatrix& m_Texgen)
{
    // float	_w						= float(Device.dwWidth);
    // float	_h						= float(Device.dwHeight);
    // float	o_w						= (.5f / _w);
    // float	o_h						= (.5f / _h);
    const Fmatrix m_TexelAdjust = {0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                                   //	Removing half pixel offset
                                   // 0.5f + o_w,			0.5f + o_h,			0.0f,			1.0f
                                   0.5f, 0.5f, 0.0f, 1.0f};
    m_Texgen.mul(m_TexelAdjust, cmd_list.xforms.m_wvp);
}

// 2D texgen for jitter (texture adjustment matrix)
void CRenderTarget::u_compute_texgen_jitter(CBackend& cmd_list, Fmatrix& m_Texgen_J)
{
    // place into	0..1 space
    Fmatrix m_TexelAdjust = {0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.0f, 1.0f};
    m_Texgen_J.mul(m_TexelAdjust, cmd_list.xforms.m_wvp);

    // rescale - tile it
    const float scale_X = float(Device.dwWidth) / float(TEX_jitter);
    const float scale_Y = float(Device.dwHeight) / float(TEX_jitter);
    // float	offset			= (.5f / float(TEX_jitter));
    m_TexelAdjust.scale(scale_X, scale_Y, 1.f);
    // m_TexelAdjust.translate_over(offset,	offset,	0	);
    m_Texgen_J.mulA_44(m_TexelAdjust);
}

void generate_jitter(DWORD* dest, u32 elem_count)
{
    const int cmax = 8;
    svector<Ivector2, cmax> samples;
    while (samples.size() < elem_count * 2)
    {
        Ivector2 test;
        test.set(::Random.randI(0, 256), ::Random.randI(0, 256));
        BOOL valid = TRUE;
        for (const auto& sample : samples)
        {
            const int dist = _abs(test.x - sample.x) + _abs(test.y - sample.y);
            if (dist < 32)
            {
                valid = FALSE;
                break;
            }
        }
        if (valid)
            samples.push_back(test);
    }
    for (u32 it = 0; it < elem_count; it++, dest++)
        *dest = color_rgba(samples[2 * it].x, samples[2 * it].y, samples[2 * it + 1].y, samples[2 * it + 1].x);
}

void CRenderTarget::reset_target_dimensions()
{
    for (u32 i = 0; i < R__NUM_CONTEXTS; i++)
    {
        dwWidth[i] = Device.dwWidth;
        dwHeight[i] = Device.dwHeight;
    }
}

CRenderTarget::CRenderTarget()
{
    const auto& options = RImplementation.o;

    param_blur = 0.f;
    param_gray = 0.f;
    param_noise = 0.f;
    param_duality_h = 0.f;
    param_duality_v = 0.f;
    param_noise_fps = 25.f;
    param_noise_scale = 1.f;

    im_noise_time = 1.f / 100.0f;
    im_noise_shift_w = 0;
    im_noise_shift_h = 0;

    param_color_base = color_rgba(127, 127, 127, 0);
    param_color_gray = color_rgba(85, 85, 85, 0);
    // param_color_add		= color_rgba(0,0,0,			0);
    param_color_add.set(0.0f, 0.0f, 0.0f);

    dwAccumulatorClearMark = 0;
    dwFlareClearMark = 0;

    //	NORMAL
    {
        u32 w = Device.dwWidth, h = Device.dwHeight;

        rt_base.resize(1);
        for (u32 i = 0; i < 1; i++)
        {
            string32 temp;
            xr_sprintf(temp, "%s%u", r2_RT_base, i);
            rt_base[i].create(temp, w, h, DXGI_FORMAT_R8G8B8A8_UNORM, 1, {CRT::CreateBase});
        }
        rt_Base_Depth.create(r2_RT_base_depth, w, h, DXGI_FORMAT_R24G8_TYPELESS);

        rt_Position.create(r2_RT_P, w, h, DXGI_FORMAT_R16G16B16A16_FLOAT);

        rt_Accumulator.create(r2_RT_accum, w, h, DXGI_FORMAT_R16G16B16A16_FLOAT);

        rt_Color.create(r2_RT_albedo, w, h, DXGI_FORMAT_R8G8B8A8_UNORM);

        // generic(LDR) RTs
        rt_Generic_0.create(r2_RT_generic0, w, h, DXGI_FORMAT_R16G16B16A16_FLOAT);
        rt_Generic_0_prev.create(r2_RT_generic0_prev, w, h, DXGI_FORMAT_R16G16B16A16_FLOAT);
        rt_Generic_0_temp.create(r2_RT_generic0_temp, w, h, DXGI_FORMAT_R16G16B16A16_FLOAT);

        rt_Generic_1.create(r2_RT_generic1, w, h, DXGI_FORMAT_R8G8B8A8_UNORM);
        rt_Generic_2.create(r2_RT_generic2, w, h, DXGI_FORMAT_R16G16B16A16_FLOAT);

        // RT Blur
        rt_blur_h_2.create(r2_RT_blur_h_2, u32(w / 2), u32(h / 2), DXGI_FORMAT_R8G8B8A8_UNORM);
        rt_blur_2.create(r2_RT_blur_2, u32(w / 2), u32(h / 2), DXGI_FORMAT_R8G8B8A8_UNORM);

        rt_blur_h_4.create(r2_RT_blur_h_4, u32(w / 4), u32(h / 4), DXGI_FORMAT_R8G8B8A8_UNORM);
        rt_blur_4.create(r2_RT_blur_4, u32(w / 4), u32(h / 4), DXGI_FORMAT_R8G8B8A8_UNORM);

        rt_blur_h_8.create(r2_RT_blur_h_8, u32(w / 8), u32(h / 8), DXGI_FORMAT_R8G8B8A8_UNORM);
        rt_blur_8.create(r2_RT_blur_8, u32(w / 8), u32(h / 8), DXGI_FORMAT_R8G8B8A8_UNORM);

        rt_pp_bloom.create(r2_RT_pp_bloom, w, h, DXGI_FORMAT_R8G8B8A8_UNORM);

        rt_ssfx_bloom1.create(r2_RT_ssfx_bloom1, u32(w / 2), u32(h / 2), DXGI_FORMAT_R16G16B16A16_FLOAT); // Bloom
        rt_ssfx_bloom_emissive.create(r2_RT_ssfx_bloom_emissive, w, h, DXGI_FORMAT_R8G8B8A8_UNORM); // Emissive
        rt_ssfx_bloom_lens.create(r2_RT_ssfx_bloom_lens, u32(w / 4), u32(h / 4), DXGI_FORMAT_R8G8B8A8_UNORM); // Lens
        rt_ssfx_bloom_tmp2.create(r2_RT_ssfx_bloom_tmp2, u32(w / 2), u32(h / 2), DXGI_FORMAT_R16G16B16A16_FLOAT); // Bloom / 2
        rt_ssfx_bloom_tmp4.create(r2_RT_ssfx_bloom_tmp4, u32(w / 4), u32(h / 4), DXGI_FORMAT_R16G16B16A16_FLOAT); // Bloom / 4
        rt_ssfx_bloom_tmp8.create(r2_RT_ssfx_bloom_tmp8, u32(w / 8), u32(h / 8), DXGI_FORMAT_R16G16B16A16_FLOAT); // Bloom / 8
        rt_ssfx_bloom_tmp16.create(r2_RT_ssfx_bloom_tmp16, u32(w / 16), u32(h / 16), DXGI_FORMAT_R16G16B16A16_FLOAT); // Bloom / 16
        rt_ssfx_bloom_tmp32.create(r2_RT_ssfx_bloom_tmp32, u32(w / 32), u32(h / 32), DXGI_FORMAT_R16G16B16A16_FLOAT); // Bloom / 32
        rt_ssfx_bloom_tmp64.create(r2_RT_ssfx_bloom_tmp64, u32(w / 64), u32(h / 64), DXGI_FORMAT_R16G16B16A16_FLOAT); // Bloom / 64
        rt_ssfx_bloom_tmp32_2.create(r2_RT_ssfx_bloom_tmp32_2, u32(w / 32), u32(h / 32), DXGI_FORMAT_R16G16B16A16_FLOAT); // Bloom / 32
        rt_ssfx_bloom_tmp16_2.create(r2_RT_ssfx_bloom_tmp16_2, u32(w / 16), u32(h / 16), DXGI_FORMAT_R16G16B16A16_FLOAT); // Bloom / 16
        rt_ssfx_bloom_tmp8_2.create(r2_RT_ssfx_bloom_tmp8_2, u32(w / 8), u32(h / 8), DXGI_FORMAT_R16G16B16A16_FLOAT); // Bloom / 8
        rt_ssfx_bloom_tmp4_2.create(r2_RT_ssfx_bloom_tmp4_2, u32(w / 4), u32(h / 4), DXGI_FORMAT_R16G16B16A16_FLOAT); // Bloom / 4

        rt_dof.create(r2_RT_dof, w, h, DXGI_FORMAT_R8G8B8A8_UNORM);

        rt_second_vp.create(r2_RT_secondVP, w, h, DXGI_FORMAT_R8G8B8A8_UNORM); //--#SM+#-- +SecondVP+

        rt_fakescope.create(r2_RT_scopert, w, h, DXGI_FORMAT_R8G8B8A8_UNORM); // crookr fakescope

        rt_heat.create(r2_RT_heat, w, h, DXGI_FORMAT_R8G8B8A8_UNORM);
        rt_mask_drops_blur.create(r2_RT_mask_drops_blur, w, h, DXGI_FORMAT_R8G8B8A8_UNORM); // Create RT, full resolution

        rt_flares.create(r2_RT_flares, w, h, DXGI_FORMAT_R8G8B8A8_UNORM);

        rt_Velocity.create(r2_RT_velocity, w, h, DXGI_FORMAT_R16G16_FLOAT);
        rt_zbuffer.create(r2_RT_zbuffer, w, h, DXGI_FORMAT_R24G8_TYPELESS);
        rt_tempzb.create("$user$temp_zb", w, h, DXGI_FORMAT_R24G8_TYPELESS); // Redotix99: for 3D Shader Based Scopes
        rt_tempzb_dof.create(r2_RT_zbuffer_dof, w, h, DXGI_FORMAT_R24G8_TYPELESS);

        // Для сглаживаний и прочих постэффектов. Должен быть того же формата что и rt_Generic_0.
        Flags32 flg = {0};
        if (HW.FeatureLevel >= D3D_FEATURE_LEVEL_11_0)
            flg = {CRT::CreateUAV};

        rt_Generic_combine.create(r2_RT_generic_combine, w, h, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, flg);
    }

    reset_3dss_rendertarget(true);
    InitDLSS();
    InitFSR();

    // OCCLUSION
    CBlender_light_occq b_occq;
    s_occq.create(&b_occq, "r2\\occq");
    CBlender_blur b_blur;
    s_blur.create(&b_blur, "r2\\blur");
    CBlender_dof b_dof;
    s_dof.create(&b_dof, "r2\\dof");
    CBlender_gasmask_dudv b_gasmask_dudv;
    s_gasmask_dudv.create(&b_gasmask_dudv, "r2\\gasmask_dudv");
    CBlender_fakescope b_fakescope;
    s_fakescope.create(&b_fakescope, "r2\\fakescope"); //crookr
    CBlender_nightvision b_nightvision;
    s_nightvision.create(&b_nightvision, "r2\\nightvision");
    CBlender_heatvision b_heatvision;
    s_heatvision.create(&b_heatvision, "r2\\heatvision");
    s_flare.create("effects\\lensflare", "shaders\\lensflare");
    s_lut.create("ogsr_lut");

    s_ssfx_bloom.create("ogsr_bloom");
    s_ssfx_bloom_lens.create("ogsr_bloom_flares");
    s_ssfx_bloom_downsample.create("ogsr_bloom_downsample");
    s_ssfx_bloom_upsample.create("ogsr_bloom_upsample");

    s_taa.create("temporal_antialiasing");
    s_cas.create("contrast_adaptive_sharpening");

    // DIRECT (spot)
    const u32 size = RImplementation.o.smapsize;

    const Flags32 flags{};
    rt_smap_depth.create(r2_RT_smap_depth, size, size, DXGI_FORMAT_R24G8_TYPELESS, 1, R__NUM_CONTEXTS, flags);
    rt_smap_rain.create(r2_RT_smap_rain, options.rain_smapsize, options.rain_smapsize, DXGI_FORMAT_R24G8_TYPELESS);

    CBlender_accum_direct_mask b_accum_mask;
    s_accum_mask.create(&b_accum_mask, "r3\\accum_mask");
    CBlender_accum_direct b_accum_direct;
    s_accum_direct.create(&b_accum_direct, "r3\\accum_direct");
    s_accum_direct_volumetric.create("accum_volumetric_sun_nomsaa");

    //	RAIN
    //	TODO: DX10: Create resources only when DX10 rain is enabled.
    //	Or make DX10 rain switch dynamic?
    {
        CBlender_rain b_rain;
        s_rain.create(&b_rain, "null");
    }

    // POINT
    {
        CBlender_accum_point b_accum_point;
        s_accum_point.create(&b_accum_point, "r2\\accum_point_s");
        accum_point_geom_create();
        g_accum_point.create(D3DFVF_XYZ, g_accum_point_vb, g_accum_point_ib);
        accum_omnip_geom_create();
        g_accum_omnipart.create(D3DFVF_XYZ, g_accum_omnip_vb, g_accum_omnip_ib);
    }

    // SPOT
    {
        CBlender_accum_spot b_accum_spot;
        s_accum_spot.create(&b_accum_spot, "r2\\accum_spot_s", "lights\\lights_spot01");
        accum_spot_geom_create();
        g_accum_spot.create(D3DFVF_XYZ, g_accum_spot_vb, g_accum_spot_ib);
    }

    // VOLUME
    {
        s_accum_volume.create("accum_volumetric", "lights\\lights_spot01");
        accum_volumetric_geom_create();
        g_accum_volumetric.create(D3DFVF_XYZ, g_accum_volumetric_vb, g_accum_volumetric_ib);
    }

    // BLOOM
    {
        constexpr DXGI_FORMAT fmt = DXGI_FORMAT_R8G8B8A8_UNORM;
        constexpr u32 w = BLOOM_size_X, h = BLOOM_size_Y;
        constexpr u32 fvf_build = D3DFVF_XYZRHW | D3DFVF_TEX4 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1) | D3DFVF_TEXCOORDSIZE2(2) | D3DFVF_TEXCOORDSIZE2(3);
        constexpr u32 fvf_filter = (u32)D3DFVF_XYZRHW | D3DFVF_TEX8 | D3DFVF_TEXCOORDSIZE4(0) | D3DFVF_TEXCOORDSIZE4(1) | D3DFVF_TEXCOORDSIZE4(2) | D3DFVF_TEXCOORDSIZE4(3) |
            D3DFVF_TEXCOORDSIZE4(4) | D3DFVF_TEXCOORDSIZE4(5) | D3DFVF_TEXCOORDSIZE4(6) | D3DFVF_TEXCOORDSIZE4(7);
        rt_Bloom_1.create(r2_RT_bloom1, w, h, fmt);
        rt_Bloom_2.create(r2_RT_bloom2, w, h, fmt);
        g_bloom_build.create(fvf_build, RImplementation.Vertex.Buffer(), RImplementation.QuadIB);
        g_bloom_filter.create(fvf_filter, RImplementation.Vertex.Buffer(), RImplementation.QuadIB);
        CBlender_bloom_build b_bloom;
        s_bloom.create(&b_bloom, "r2\\bloom");
        f_bloom_factor = 0.5f;
    }

    // TONEMAP	
    {
        rt_LUM_64.create(r2_RT_luminance_t64, 64, 64, DXGI_FORMAT_R16G16B16A16_FLOAT);
        rt_LUM_8.create(r2_RT_luminance_t8, 8, 8, DXGI_FORMAT_R16G16B16A16_FLOAT);
        CBlender_luminance b_luminance;
        s_luminance.create(&b_luminance, "r2\\luminance");
        f_luminance_adapt = 0.5f;

        t_LUM_src.create(r2_RT_luminance_src);
        t_LUM_dest.create(r2_RT_luminance_cur);

        // create pool
        for (u32 it = 0; it < 2; it++)
        {
            string256 name;
            xr_sprintf(name, "%s_%d", r2_RT_luminance_pool, it);
            rt_LUM_pool[it].create(name, 1, 1, DXGI_FORMAT_R32_FLOAT);
            RCache.ClearRT(rt_LUM_pool[it]->pRT, 0x7f7f7f7f);
        }
        u_setrt(RCache, Device.dwWidth, Device.dwHeight, get_base_rt(), nullptr, nullptr, get_base_zb());
    }

    // COMBINE
    {
        constexpr D3DVERTEXELEMENT9 dwDecl[]{{0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0}, // pos+uv
                                             D3DDECL_END()};

        s_combine.create("combine");
        s_combine_volumetric.create("combine_volumetric");
        g_combine.create(FVF::F_TL, RImplementation.Vertex.Buffer(), RImplementation.QuadIB);
        g_combine_2UV.create(FVF::F_TL2uv, RImplementation.Vertex.Buffer(), RImplementation.QuadIB);
        g_combine_cuboid.create(dwDecl, RImplementation.Vertex.Buffer(), RImplementation.Index.Buffer());
        g_flare.create(FVF::F_LIT, RImplementation.Vertex.Buffer(), RImplementation.QuadIB);

        t_envmap_0.create(r2_T_envs0);
        t_envmap_1.create(r2_T_envs1);

        s_rain_drops.create("ogsr_rain_drops");
    }

    const u32 w = Device.dwWidth;
    const u32 h = Device.dwHeight;

    // SMAA RTs
    {
        rt_smaa_edgetex.create(r2_RT_smaa_edgetex, w, h, DXGI_FORMAT_R8G8_UNORM);
        rt_smaa_blendtex.create(r2_RT_smaa_blendtex, w, h, DXGI_FORMAT_R8G8B8A8_UNORM);

        s_pp_antialiasing.create("effects_pp_antialiasing");
    }

    // Mrmnwar SunShaft Screen Space
    {
        rt_SunShaftsMask.create(r2_RT_SunShaftsMask, w, h, DXGI_FORMAT_R8G8B8A8_UNORM);
        rt_SunShaftsMaskSmoothed.create(r2_RT_SunShaftsMaskSmoothed, w, h, DXGI_FORMAT_R8G8B8A8_UNORM);
        rt_SunShaftsPass0.create(r2_RT_SunShaftsPass0, w, h, DXGI_FORMAT_R8G8B8A8_UNORM);
        s_ssss_mrmnwar.create("effects\\ss_sunshafts_mrmnwar");
    }

    // RT - KD Screen space sunshafts
    {
        rt_sunshafts_0.create(r2_RT_sunshafts0, w, h, DXGI_FORMAT_R8G8B8A8_UNORM);
        rt_sunshafts_1.create(r2_RT_sunshafts1, w, h, DXGI_FORMAT_R8G8B8A8_UNORM);
        s_ssss_ogse.create("effects\\ss_sunshafts_ogse");
    }

    s_puddles.create("effects_water_puddles");

    // Build textures
    {
        // Build noise table        
        {
            constexpr u32 sampleSize = 4;
            constexpr u32 tempData[TEX_jitter_count][TEX_jitter * TEX_jitter]{};

            D3D_TEXTURE2D_DESC desc{};
            desc.Width = TEX_jitter;
            desc.Height = TEX_jitter;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Format = DXGI_FORMAT_R8G8B8A8_SNORM;
            // desc.Usage = D3D_USAGE_IMMUTABLE;
            desc.Usage = D3D_USAGE_DEFAULT;
            desc.BindFlags = D3D_BIND_SHADER_RESOURCE;
            desc.CPUAccessFlags = 0;
            desc.MiscFlags = 0;

            D3D_SUBRESOURCE_DATA subData[TEX_jitter_count];

            for (u32 it{}; it < TEX_jitter_count; it++)
            {
                subData[it].pSysMem = tempData[it];
                subData[it].SysMemPitch = desc.Width * sampleSize;
            }

            // Fill it,
            for (u32 y = 0; y < TEX_jitter; y++)
            {
                for (u32 x = 0; x < TEX_jitter; x++)
                {
                    DWORD data[TEX_jitter_count];
                    generate_jitter(data, TEX_jitter_count);
                    for (u32 it = 0; it < TEX_jitter_count; it++)
                    {
                        u32* p = (u32*)(LPBYTE(subData[it].pSysMem) + y * subData[it].SysMemPitch + x * 4);

                        *p = data[it];
                    }
                }
            }

            for (u32 it{}; it < TEX_jitter_count; it++)
            {
                string_path name;
                xr_sprintf(name, "%s%d", r2_jitter, it);
                R_CHK(HW.pDevice->CreateTexture2D(&desc, &subData[it], &t_noise_surf[it]));
                t_noise[it] = dxRenderDeviceRender::Instance().Resources->_CreateTexture(name);
                t_noise[it]->surface_set(t_noise_surf[it]);
            }          
        }
    }

    // PP
    s_postprocess.create("postprocess");
    g_postprocess.create(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX3, RImplementation.Vertex.Buffer(), RImplementation.QuadIB);

    //
    reset_target_dimensions();
}

CRenderTarget::~CRenderTarget()
{
    t_LUM_src->surface_set(nullptr);
    t_LUM_dest->surface_set(nullptr);

#ifdef DEBUG
    ID3DBaseTexture* pSurf = 0;

    pSurf = t_envmap_0->surface_get();
    if (pSurf)
        pSurf->Release();
    _SHOW_REF("t_envmap_0 - #small", pSurf);

    pSurf = t_envmap_1->surface_get();
    if (pSurf)
        pSurf->Release();
    _SHOW_REF("t_envmap_1 - #small", pSurf);
    //_SHOW_REF("t_envmap_0 - #small",t_envmap_0->pSurface);
    //_SHOW_REF("t_envmap_1 - #small",t_envmap_1->pSurface);
#endif // DEBUG

    t_envmap_0->surface_set(nullptr);
    t_envmap_1->surface_set(nullptr);
    t_envmap_0.destroy();
    t_envmap_1.destroy();

    //	TODO: DX10: Check if we need old style SMAPs
    //	_RELEASE					(rt_smap_ZB);

    // Jitter
    for (int it = 0; it < TEX_jitter_count; it++)
    {
        t_noise[it]->surface_set(nullptr);
#ifdef DEBUG
        _SHOW_REF("t_noise_surf[it]", t_noise_surf[it]);
#endif // DEBUG
        _RELEASE(t_noise_surf[it]);
    }

    //
    accum_spot_geom_destroy();
    accum_omnip_geom_destroy();
    accum_point_geom_destroy();
    accum_volumetric_geom_destroy();

    DestroyDLSS();
    DestroyFSR();
}

void CRenderTarget::reset_light_marker(CBackend& cmd_list, bool bResetStencil)
{
    dwLightMarkerID = 5;
    if (bResetStencil)
    {
        u32 Offset;
        const float _w = float(Device.dwWidth);
        const float _h = float(Device.dwHeight);
        const u32 C = color_rgba(255, 255, 255, 255);
        const float eps = 0;
        const float _dw = 0.5f;
        const float _dh = 0.5f;
        FVF::TL* pv = (FVF::TL*)RImplementation.Vertex.Lock(4, g_combine->vb_stride, Offset);
        pv->set(-_dw, _h - _dh, eps, 1.f, C, 0, 0);
        pv++;
        pv->set(-_dw, -_dh, eps, 1.f, C, 0, 0);
        pv++;
        pv->set(_w - _dw, _h - _dh, eps, 1.f, C, 0, 0);
        pv++;
        pv->set(_w - _dw, -_dh, eps, 1.f, C, 0, 0);
        pv++;
        RImplementation.Vertex.Unlock(4, g_combine->vb_stride);
        cmd_list.set_Element(s_occq->E[2]);
        cmd_list.set_Geometry(g_combine);
        cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    }
}

void CRenderTarget::increment_light_marker(CBackend& cmd_list)
{
    dwLightMarkerID += 2;

    // if (dwLightMarkerID>10)
    const u32 iMaxMarkerValue = 255;

    if (dwLightMarkerID > iMaxMarkerValue)
        reset_light_marker(cmd_list, true);
}

bool CRenderTarget::need_to_render_sunshafts()
{
	if (!ps_r_sun_shafts)
		return false;

    const CEnvDescriptor&	E = *g_pGamePersistent->Environment().CurrentEnv;
    const Fcolor sun_color= ((light*)RImplementation.Lights.sun_adapted._get())->color;
    const float fValue = E.m_fSunShaftsIntensity * u_diffuse2s(sun_color.r,sun_color.g,sun_color.b);
    if (fValue<EPS) return false;

    return true;
}
#pragma once

#include "../xrRender/ColorMapManager.h"

class light;

#define VOLUMETRIC_SLICES 100

static void dummy(){};

class CRenderTarget : public IRender_Target
{
private:
    u32 dwWidth[R__NUM_CONTEXTS];
    u32 dwHeight[R__NUM_CONTEXTS];
    u32 dwAccumulatorClearMark;
    u32 dwFlareClearMark;

public:
    enum eStencilOptimizeMode
    {
        SO_Light = 0, //	Default
        SO_Combine, //	Default
    };

    u32 dwLightMarkerID;

#ifdef DEBUG
    struct dbg_line_t
    {
        Fvector P0, P1;
        u32 color;
    };
    xr_vector<dbg_line_t> dbg_lines;
    xr_vector<Fplane> dbg_planes;
#endif

    // Base targets
    xr_vector<ref_rt> rt_base;
    ref_rt rt_Base_Depth;

    // MRT-path

    ref_rt rt_Position; // 64bit,	fat	(x,y,z,?)				(eye-space)
    ref_rt rt_Color; // 64/32bit,fat	(r,g,b,specular-gloss)	(or decompressed MET-8-8-8-8)
    ref_rt rt_Velocity; // r2_RT_velocity

    ref_rt rt_zbuffer; // r2_RT_zbuffer
    ref_rt rt_tempzb, rt_tempzb_dof;

    //
    ref_rt rt_Accumulator; // 64bit		(r,g,b,specular)
    ref_rt rt_Generic_0; // 32bit		(r,g,b,a)				// post-process, intermidiate results, etc.
    ref_rt rt_Generic_0_prev; // r2_RT_generic0_prev
    ref_rt rt_Generic_1; // 32bit		(r,g,b,a)				// post-process, intermidiate results, etc.
    ref_rt rt_Generic_combine; // r2_RT_generic_combine
    ref_rt rt_Generic_0_temp;
    ref_rt rt_Generic_combine_scope;

    //  Second viewport
    ref_rt rt_second_vp; // 32bit		(r,g,b,a) 

    ref_rt rt_dof;
    ref_rt rt_blur_2, rt_blur_h_2, rt_blur_4, rt_blur_h_4, rt_blur_8, rt_blur_h_8;
    ref_rt rt_mask_drops_blur;

    ref_rt rt_pp_bloom;

    ref_rt rt_ssfx_bloom1;
    ref_rt rt_ssfx_bloom_emissive;
    ref_rt rt_ssfx_bloom_lens;
    ref_rt rt_ssfx_bloom_tmp2;
    ref_rt rt_ssfx_bloom_tmp4;
    ref_rt rt_ssfx_bloom_tmp8;
    ref_rt rt_ssfx_bloom_tmp16;
    ref_rt rt_ssfx_bloom_tmp32;
    ref_rt rt_ssfx_bloom_tmp64;
    ref_rt rt_ssfx_bloom_tmp32_2;
    ref_rt rt_ssfx_bloom_tmp16_2;
    ref_rt rt_ssfx_bloom_tmp8_2;
    ref_rt rt_ssfx_bloom_tmp4_2;

    //	Igor: for volumetric lights
    ref_rt rt_Generic_2; // 32bit		(r,g,b,a)				// post-process, intermidiate results, etc.
    ref_rt rt_Bloom_1; // 32bit, dim/4	(r,g,b,?)
    ref_rt rt_Bloom_2; // 32bit, dim/4	(r,g,b,?)
    ref_rt rt_LUM_64; // 64bit, 64x64,	log-average in all components
    ref_rt rt_LUM_8; // 64bit, 8x8,		log-average in all components

    ref_rt rt_LUM_pool[2]; // 1xfp32,1x1, exp-result -> scaler

    ref_texture t_LUM_src; // source
    ref_texture t_LUM_dest; // destination & usage for current frame

    // env
    ref_texture t_envmap_0; // env-0
    ref_texture t_envmap_1; // env-1

    // smap
    ref_rt rt_smap_depth; // 24(32) bit,	depth
    ref_rt rt_smap_rain;

    ID3DTexture2D* t_noise_surf[TEX_jitter_count];
    ref_texture t_noise[TEX_jitter_count];

    ref_shader s_puddles;

private:
    // OCCq

    ref_shader s_occq;

    // Accum
    ref_shader s_accum_mask;
    ref_shader s_accum_direct;
    ref_shader s_accum_direct_volumetric;
    ref_shader s_accum_point;
    ref_shader s_accum_spot;
    ref_shader s_accum_volume;

    ref_shader s_flare;

    //	DX10 Rain
    ref_shader s_rain;

    ref_geom g_accum_point;
    ref_geom g_accum_spot;
    ref_geom g_accum_omnipart;
    ref_geom g_accum_volumetric;

    ref_geom g_flare;

    ID3DVertexBuffer* g_accum_point_vb;
    ID3DIndexBuffer* g_accum_point_ib;

    ID3DVertexBuffer* g_accum_omnip_vb;
    ID3DIndexBuffer* g_accum_omnip_ib;

    ID3DVertexBuffer* g_accum_spot_vb;
    ID3DIndexBuffer* g_accum_spot_ib;

    ID3DVertexBuffer* g_accum_volumetric_vb;
    ID3DIndexBuffer* g_accum_volumetric_ib;

    // Bloom
    ref_geom g_bloom_build;
    ref_geom g_bloom_filter;
    ref_shader s_bloom;
    float f_bloom_factor;

    ref_shader s_ssfx_bloom;
    ref_shader s_ssfx_bloom_lens;
    ref_shader s_ssfx_bloom_upsample;
    ref_shader s_ssfx_bloom_downsample;

    // Luminance
    ref_shader s_luminance;
    float f_luminance_adapt;

    // Combine
    ref_geom g_combine;
    ref_geom g_combine_2UV;
    ref_geom g_combine_cuboid;
    ref_shader s_combine;
    ref_shader s_combine_volumetric;

    ref_shader s_blur;
    ref_shader s_dof;
    ref_shader s_lut;
    ref_shader s_taa;
    ref_shader s_cas;

    ref_shader s_rain_drops;
    ref_shader s_gasmask_dudv;
    ref_shader s_fakescope; //crookr
    ref_shader s_nightvision;
    ref_shader s_heatvision;

    ref_rt rt_flares;
    ref_rt rt_fakescope; // crookr
    ref_rt rt_heat;

public:
    ref_shader s_postprocess;
    ref_geom g_postprocess;

private:
    float im_noise_time;
    u32 im_noise_shift_w;
    u32 im_noise_shift_h;

    float param_blur;
    float param_gray;
    float param_duality_h;
    float param_duality_v;
    float param_noise;
    float param_noise_scale;
    float param_noise_fps;
    u32 param_color_base;
    u32 param_color_gray;
    Fvector param_color_add;

    //	Color mapping
    float param_color_map_influence;
    float param_color_map_interpolate;
    ColorMapManager color_map_manager;

    //	Igor: used for volumetric lights
    bool m_bHasActiveVolumetric;

public:
    CRenderTarget();
    ~CRenderTarget();

    void reset_target_dimensions();

    void accum_point_geom_create();
    void accum_point_geom_destroy();
    void accum_omnip_geom_create();
    void accum_omnip_geom_destroy();
    void accum_spot_geom_create();
    void accum_spot_geom_destroy();
    void accum_volumetric_geom_create();
    void accum_volumetric_geom_destroy();

    void u_compute_texgen_screen(CBackend& cmd_list, Fmatrix& dest);
    void u_compute_texgen_jitter(CBackend& cmd_list, Fmatrix& dest);

    ID3DRenderTargetView* get_base_rt() const { return rt_base[0/*HW.CurrentBackBuffer*/]->pRT; }
    ID3DDepthStencilView* get_base_zb() const { return rt_Base_Depth->pZRT[CHW::IMM_CTX_ID]; }

    void u_setrt(CBackend& cmd_list, const ref_rt& _1, const ref_rt& _2, const ref_rt& _3, const ref_rt& _4, ID3DDepthStencilView* zb);
    void u_setrt(CBackend& cmd_list, const ref_rt& _1, const ref_rt& _2, const ref_rt& _3, ID3DDepthStencilView* zb);
    void u_setrt(CBackend& cmd_list, const ref_rt& _1, const ref_rt& _2, ID3DDepthStencilView* zb);
    void u_setrt(CBackend& cmd_list, u32 W, u32 H, ID3DRenderTargetView* _1, ID3DRenderTargetView* _2, ID3DRenderTargetView* _3, ID3DDepthStencilView* zb);

    void u_calc_tc_noise(CBackend& cmd_list, Fvector2& p0, Fvector2& p1);
    void u_calc_tc_duality_ss(Fvector2& r0, Fvector2& r1, Fvector2& l0, Fvector2& l1);

    bool u_need_CM() const;

    void phase_scene_prepare();
    void phase_scene_begin(CBackend& cmd_list);
    void phase_scene_end(CBackend& cmd_list);
    void phase_occq(CBackend& cmd_list);
    void phase_wallmarks(CBackend& cmd_list);
    void phase_smap_direct(CBackend& cmd_list, light* L, u32 sub_phase);
    void phase_smap_spot_clear(CBackend& cmd_list);
    void phase_smap_spot(CBackend& cmd_list, light* L) const;
    void phase_accumulator(CBackend& cmd_list);
    void phase_vol_accumulator(CBackend& cmd_list);
    void phase_blur(CBackend& cmd_list);
    void phase_dof(CBackend& cmd_list);
    void phase_lut(CBackend& cmd_list);
    void phase_gasmask_dudv(CBackend& cmd_list);
    void phase_nightvision(CBackend& cmd_list);
    void phase_heatvision(CBackend& cmd_list); //--DSR-- HeatVision

    void draw_rain(CBackend& cmd_list, light& RainSetup);

    bool need_to_render_sunshafts();

    void draw_volume(CBackend& cmd_list, light* L);
    void accum_direct_cascade(CBackend& cmd_list, u32 sub_phase, Fmatrix& xform, Fmatrix& xform_prev, float fBias);
    void accum_direct_blend(CBackend& cmd_list);
    void accum_direct_volumetric(CBackend& cmd_list, u32 sub_phase, u32 Offset, u32 i_offset, const Fmatrix& mShadow);
    void accum_point(CBackend& cmd_list, light* L);
    void accum_spot(CBackend& cmd_list, light* L);
    void accum_volumetric(CBackend& cmd_list, light* L);
    void phase_bloom(CBackend& cmd_list);
    void phase_ssfx_bloom(CBackend& cmd_list);
    void phase_luminance(CBackend& cmd_list);
    void phase_combine(CBackend& cmd_list);
    void phase_pp(CBackend& cmd_list);
    void phase_combine_volumetric(CBackend& cmd_list);

    void phase_rain_drops(CBackend& cmd_list);
    void phase_fakescope(CBackend& cmd_list); // crookr
    void phase_flares(CBackend& cmd_list);

    void render_flare(CBackend& cmd_list, light* L);

    virtual void set_blur(float f) { param_blur = f; }
    virtual void set_gray(float f) { param_gray = f; }
    virtual void set_duality_h(float f) { param_duality_h = _abs(f); }
    virtual void set_duality_v(float f) { param_duality_v = _abs(f); }
    virtual void set_noise(float f) { param_noise = f; }
    virtual void set_noise_scale(float f) { param_noise_scale = f; }
    virtual void set_noise_fps(float f) { param_noise_fps = _abs(f) + EPS_S; }
    virtual void set_color_base(u32 f) { param_color_base = f; }
    virtual void set_color_gray(u32 f) { param_color_gray = f; }
    virtual void set_color_add(const Fvector& f) { param_color_add = f; }

    virtual u32 get_width(CBackend& cmd_list) { return dwWidth[cmd_list.context_id]; }
    virtual u32 get_height(CBackend& cmd_list) { return dwHeight[cmd_list.context_id]; }

    virtual void set_cm_imfluence(float f) { param_color_map_influence = f; }
    virtual void set_cm_interpolate(float f) { param_color_map_interpolate = f; }
    virtual void set_cm_textures(const shared_str& tex0, const shared_str& tex1) { color_map_manager.SetTextures(tex0, tex1); }

    //	Need to reset stencil only when marker overflows.
    //	Don't clear when render for the first time
    void reset_light_marker(CBackend& cmd_list, bool bResetStencil = false);
    void increment_light_marker(CBackend& cmd_list);

#ifdef DEBUG
    IC void dbg_addline(Fvector& P0, Fvector& P1, u32 c)
    {
        dbg_lines.push_back(dbg_line_t());
        dbg_lines.back().P0 = P0;
        dbg_lines.back().P1 = P1;
        dbg_lines.back().color = c;
    }
    IC void dbg_addplane(Fplane& P0, u32 c) { dbg_planes.push_back(P0); }
#else
    IC void dbg_addline(Fvector& P0, Fvector& P1, u32 c) {}
    IC void dbg_addplane(Fplane& P0, u32 c) {}
#endif

private:
    void RenderScreenQuad(CBackend& cmd_list, u32 w, u32 h, const ref_rt& rt, ref_selement& sh, const std::function<void()>& lambda = dummy);

    // Anti Aliasing
    ref_shader s_pp_antialiasing;
    ref_rt rt_smaa_edgetex;
    ref_rt rt_smaa_blendtex;

    void PhaseAA(CBackend& cmd_list);
    bool Phase3DSSUpscale(CBackend& cmd_list);

    void ProcessSMAA(CBackend& cmd_list);
    void ProcessTAA(CBackend& cmd_list);

    void InitDLSS();
    bool ProcessDLSS();
    bool ProcessDLSS_3DSS(const bool need_reset);
    void DestroyDLSS();

    void InitFSR();
    bool ProcessFSR() const;
    bool ProcessFSR_3DSS(const bool need_reset);
    void DestroyFSR();

    bool reset_3dss_rendertarget(const bool need_reset = false);

    void ProcessCAS(CBackend& cmd_list, ref_selement& sh, const bool force = false);

    void PhaseSSSS(CBackend& cmd_list);

    ref_rt rt_sunshafts_0; // ss0
    ref_rt rt_sunshafts_1; // ss1
    ref_rt rt_SunShaftsMask;
    ref_rt rt_SunShaftsMaskSmoothed;
    ref_rt rt_SunShaftsPass0;
    ref_shader s_ssss_mrmnwar;
    ref_shader s_ssss_ogse;
};

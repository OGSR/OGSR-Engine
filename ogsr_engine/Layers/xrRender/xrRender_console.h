#pragma once

// Common
extern ECORE_API u32 r2_SmapSize;

extern ECORE_API u32 ps_r_pp_aa_mode;
extern ECORE_API u32 ps_r_dlss_preset, ps_r_dlss_3dss_preset, ps_r_dlss_3dss_quality;
extern float ps_r_dlss_3dss_scale_factor;

extern ECORE_API u32 ps_r_sun_shafts;
extern ECORE_API u32 ps_r_sunshafts_mode;

extern ECORE_API float ps_r_ss_sunshafts_length;
extern ECORE_API float ps_r_ss_sunshafts_radius;
extern ECORE_API float ps_r_prop_ss_radius;
extern ECORE_API float ps_r_prop_ss_blend;
extern ECORE_API float ps_r_prop_ss_sample_step_phase0;
extern ECORE_API float ps_r_prop_ss_sample_step_phase1;

extern ECORE_API Fvector3 ps_r_taa_jitter;
extern Fvector2 ps_r_taa_jitter_full;
extern ECORE_API float ps_r_cas;

extern ECORE_API u32 ps_r_ao_quality; //	=	0;

enum : u32
{
    AO_MODE_GTAO,
    AO_MODE_SSDO
};
extern ECORE_API u32 ps_r_ao_mode;

extern ECORE_API u32 ps_r_sun_quality; //	=	0;

extern ECORE_API int ps_r__LightSleepFrames;

extern ECORE_API float ps_r__WallmarkTTL;
extern ECORE_API float ps_r__WallmarkSHIFT;
extern ECORE_API float ps_r__WallmarkSHIFT_V;

extern ECORE_API float ps_r__GLOD_ssa_start;
extern ECORE_API float ps_r__GLOD_ssa_end;
extern ECORE_API float ps_r__LOD;

extern ECORE_API float ps_r__ssaDISCARD;
extern ECORE_API int ps_r__tf_Anisotropic;
extern ECORE_API float ps_r__tf_Mipbias;

extern float ps_r2_no_details_radius;
extern float ps_r2_no_rain_radius;

enum
{
    R1FLAG_DLIGHTS = 1 << 0,
};

// R2
extern ECORE_API float ps_r2_ssaLOD_A;
extern ECORE_API float ps_r2_ssaLOD_B;

// R2-specific
extern ECORE_API Flags64 ps_r2_ls_flags; // r2-only
extern ECORE_API Flags64 ps_r2_ls_flags_ext;
extern ECORE_API float ps_r2_df_parallax_h; // r2-only
extern ECORE_API float ps_r2_df_parallax_range; // r2-only
extern ECORE_API float ps_r2_gmaterial; // r2-only
extern ECORE_API float ps_r2_tonemap_middlegray; // r2-only
extern ECORE_API float ps_r2_tonemap_adaptation; // r2-only
extern ECORE_API float ps_r2_tonemap_low_lum; // r2-only
extern ECORE_API float ps_r2_tonemap_amount; // r2-only
extern ECORE_API float ps_r2_ls_bloom_kernel_scale; // r2-only	// gauss
extern ECORE_API float ps_r2_ls_bloom_kernel_g; // r2-only	// gauss
extern ECORE_API float ps_r2_ls_bloom_kernel_b; // r2-only	// bilinear
extern ECORE_API float ps_r2_ls_bloom_threshold; // r2-only
extern ECORE_API float ps_r2_ls_bloom_speed; // r2-only
extern ECORE_API float ps_r2_ls_dsm_kernel; // r2-only
extern ECORE_API float ps_r2_ls_psm_kernel; // r2-only
extern ECORE_API float ps_r2_ls_ssm_kernel; // r2-only
extern ECORE_API float ps_r2_mblur; // .5f
extern ECORE_API float ps_r2_ls_depth_scale; // 1.0f
extern ECORE_API float ps_r2_ls_depth_bias; // -0.0001f
extern ECORE_API float ps_r2_ls_squality; // 1.0f
extern ECORE_API float ps_r2_sun_near; // 10.0f
extern ECORE_API float ps_r2_sun_near_border; // 1.0f
extern ECORE_API float ps_r2_sun_tsm_bias; // 0.0001f
extern ECORE_API float ps_r2_sun_depth_far_scale; // 1.00001f
extern ECORE_API float ps_r2_sun_depth_far_bias; // -0.0001f
extern ECORE_API float ps_r2_sun_depth_near_scale; // 1.00001f
extern ECORE_API float ps_r2_sun_depth_near_bias; // -0.0001f
extern ECORE_API float ps_r2_sun_lumscale; // 0.5f
extern ECORE_API float ps_r2_sun_lumscale_hemi; // 1.0f
extern ECORE_API float ps_r2_sun_lumscale_amb; // 1.0f

extern ECORE_API float ps_r2_dhemi_sky_scale; // 1.5f
extern ECORE_API float ps_r2_dhemi_light_scale; // 1.f
extern ECORE_API float ps_r2_dhemi_light_flow; // .1f
extern ECORE_API int ps_r2_dhemi_count; // 5
extern ECORE_API float ps_r2_slight_fade; // 1.f

extern float ps_r2_gloss_factor;

extern ECORE_API float ps_r2_img_exposure; // r2-only
extern ECORE_API float ps_r2_img_gamma; // r2-only
extern ECORE_API float ps_r2_img_saturation; // r2-only

extern ECORE_API Fvector ps_r2_img_cg; // r2-only

extern ECORE_API int ps_r3_dyn_wet_surf_opt;
extern ECORE_API float ps_r3_dyn_wet_surf_near; // 10.0f
extern ECORE_API float ps_r3_dyn_wet_surf_far; // 30.0f
extern ECORE_API int ps_r3_dyn_wet_surf_sm_res; // 256
extern ECORE_API int ps_r3_dyn_wet_surf_enable_streaks;

extern ECORE_API int ps_r2_mask_numbers;

extern ECORE_API float ps_r2_rain_drops_intensity;
extern ECORE_API float ps_r2_rain_drops_speed;

extern ECORE_API float ps_r2_visor_refl_intensity;
extern ECORE_API float ps_r2_visor_refl_radius;

extern ECORE_API int ps_pnv_mode;

extern ECORE_API int scope_fake_enabled; // crookr

extern ECORE_API float scope_fake_power; // crookr
extern ECORE_API float scope_fake_radius; // crookr
extern ECORE_API float scope_fake_interp; // crookr

extern ECORE_API Fvector4 ps_scope1_params;
extern ECORE_API Fvector4 ps_scope2_params;
extern ECORE_API Fvector4 ps_scope3_params;

extern ECORE_API float ps_ssfx_wpn_dof_2;
extern Fvector4 ps_ssfx_int_grass_params_1;
extern ECORE_API Fvector4 ps_ssfx_grass_shadows;
extern Fvector3 ps_ssfx_shadow_cascades;
extern ECORE_API Fvector3 ps_ssfx_shadows;
extern ECORE_API Fvector3 ps_ssfx_volumetric;
extern Fvector4 ps_ssfx_wind_grass, ps_ssfx_wind_trees;
extern ECORE_API Fvector4 ps_ssfx_rain_1;
extern ECORE_API Fvector4 ps_ssfx_rain_2;
extern ECORE_API Fvector4 ps_ssfx_rain_3;
extern Fvector4 ps_ssfx_florafixes_1, ps_ssfx_florafixes_2;
extern int ps_ssfx_is_underground, ps_ssfx_gloss_method;

// Screen Space Shaders Stuff
extern Fvector4 ps_ssfx_blood_decals;
extern float ps_ssfx_wpn_dof_2;
extern Fvector3 ps_ssfx_shadow_bias;
extern Fvector4 ps_ssfx_lut;

extern ECORE_API int ps_ssfx_bloom_use_presets;
extern ECORE_API Fvector4 ps_ssfx_bloom_1;
extern ECORE_API Fvector4 ps_ssfx_bloom_2;

extern ECORE_API int ps_ssfx_pom_refine;
extern ECORE_API Fvector4 ps_ssfx_pom;

// textures
extern ECORE_API int psTextureLOD;

extern ECORE_API float ps_r__opt_dist;

extern ECORE_API int opt_static_geom;
extern ECORE_API int opt_shadow_geom;

extern int r_back_buffer_count;

enum : u64
{
    R2FLAG_SUN = 1ull << 0,
    // = 1ull << 1,
    // = 1ull << 2,
    R2FLAG_SUN_DETAILS = 1ull << 3,
    R2FLAG_TONEMAP = 1ull << 4,
    // = 1ull << 5,
    //R2FLAG_GI = 1ull << 6,
    R2FLAG_FASTBLOOM = 1ull << 7,
    R2FLAG_GLOBALMATERIAL = 1ull << 8,
    //R2FLAG_ZFILL = 1ull << 9,
    //R2FLAG_R1LIGHTS = 1ull << 10,
    // = 1ull << 11,
    //R3FLAG_USE_DX10_1 = 1ull << 12,
    // = 1ull << 13,
    R2FLAG_EXP_DONT_TEST_UNSHADOWED = 1ull << 14,
    R2FLAG_EXP_DONT_TEST_SHADOWED = 1ull << 15,

    R2FLAG_DBG_TAA_JITTER_ENABLE = 1 << 16,
    R2FLAG_HAT = 1 << 17,

    R2FLAG_SSFX_HEIGHT_FOG = 1ull << 19,
    R2FLAG_SSFX_SKY_DEBANDING = 1ull << 20,

    R2FLAG_VOLUMETRIC_LIGHTS = 1ull << 21,

    R2FLAG_STEEP_PARALLAX = 1ull << 22,

    // = 1ull << 23,
    R2FLAG_EXP_MT_BONES = 1ull << 24,
    R2FLAG_DETAIL_BUMP = 1ull << 25,

    R3FLAG_DYN_WET_SURF = 1ull << 26,
    R3FLAG_VOLUMETRIC_SMOKE = 1ull << 27,

    R2FLAG_SSFX_INDIRECT_LIGHT = 1ull << 28,
    R2FLAG_SSFX_BLOOM = 1ull << 29,

    R2FLAG_EXP_MT_SUN = 1ull << 30,
    R2FLAG_EXP_MT_RAIN = 1ull << 31,
    R2FLAG_EXP_MT_PARTICLES = 1ull << 32,

    R2FLAG_LIGHT_DETAILS = 1ull << 33,
    R2FLAG_LIGHT_NO_DIST_SHADOWS = 1ull << 34,

    R2FLAG_EXP_MT_LIGHTS = 1ull << 35,
};

enum : u64
{
    R2FLAGEXT_DISABLE_STATIC_NORMAL = 1ull << 0,
    R2FLAGEXT_DISABLE_STATIC_LOD = 1ull << 1,
    R2FLAGEXT_DISABLE_STATIC_TREE = 1ull << 2,
    R2FLAGEXT_DISABLE_STATIC_TREE_PROGRESSIVE = 1ull << 3,
    R2FLAGEXT_DISABLE_STATIC_PROGRESSIVE = 1ull << 4,

    R2FLAGEXT_ENABLE_TESSELLATION = 1ull << 5,
    R2FLAGEXT_WIREFRAME = 1ull << 6,

    R2FLAGEXT_HOM_DEPTH_DRAW = 1ull << 7,
    R2FLAGEXT_SUN_ZCULLING = 1ull << 8,
    R2FLAGEXT_DISABLE_DYNAMIC = 1ull << 9,
    R2FLAGEXT_DISABLE_PARTICLES = 1ull << 10,
    R2FLAGEXT_DISABLE_HOM = 1ull << 11,
    R2FLAGEXT_RAIN_DROPS = 1ull << 12,
    R2FLAGEXT_RAIN_DROPS_CONTROL = 1ull << 13,
    // = 1ull << 14,
    R2FLAGEXT_SSLR = 1ull << 15,
    R2FLAGEXT_VISOR_REFL = 1ull << 16,
    R2FLAGEXT_VISOR_REFL_CONTROL = 1ull << 17,
    R2FLAGEXT_TERRAIN_PARALLAX = 1ull << 18,
    R2FLAGEXT_MASK = 1ull << 19,
    R2FLAGEXT_MASK_CONTROL = 1ull << 20,
    R2FLAGEXT_MT_TEXLOAD = 1ull << 21,
    R2FLAGEXT_DLSS_3DSS_USE_SECOND_PASS = 1ull << 22,
    // = 1ull << 23,
    R2FLAGEXT_REFLECTIONS_ONLY_ON_TERRAIN = 1ull << 24,
    R2FLAGEXT_REFLECTIONS_ONLY_ON_PUDDLES = 1ull << 25,
    R2FLAGEXT_MOTION_BLUR = 1ull << 26,
    R2FLAGEXT_DISABLE_SECTORS = 1ull << 27,
    R2FLAGEXT_SSFX_INTER_GRASS = 1ull << 28,
    R2FLAGEXT_FONT_SHADOWS = 1ull << 29,
    // = 1ull << 30,
    R2FLAGEXT_DISABLE_LIGHT = 1ull << 31,

    R2FLAGEXT_SHADER_DBG = 1ull << 32,
    R2FLAGEXT_LENS_FLARE = 1ull << 33,
    // R2FLAGEXT_RENDER_ON_PREFETCH = 1ull << 34,
};

extern void xrRender_initconsole();

// Postprocess anti-aliasing types
enum
{
    NO_AA,
    DLSS,
    FSR2,
    TAA,
    SMAA,
};

// Sunshafts types
enum : u32
{
    SS_OFF,

    SS_SS_OGSE,
    SS_SS_MANOWAR,
    SS_SS_MASK,

    SS_VOLUMETRIC,

    SS_COMBINED_OGSE = SS_VOLUMETRIC | SS_SS_OGSE,
    SS_COMBINED_MANOWAR = SS_VOLUMETRIC | SS_SS_MANOWAR,
};

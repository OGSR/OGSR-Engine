#ifndef xrRender_consoleH
#define xrRender_consoleH
#pragma once

// Common
extern ENGINE_API	int			ps_r__Supersample;
extern ECORE_API	int			ps_r__LightSleepFrames;

extern ECORE_API	float		ps_r__Detail_l_ambient;
extern ECORE_API	float		ps_r__Detail_l_aniso;
extern ECORE_API	float		ps_r__Detail_density;

extern ECORE_API	float		ps_r__Tree_w_rot;
extern ECORE_API	float		ps_r__Tree_w_speed;
extern ECORE_API	float		ps_r__Tree_w_amp;
extern ECORE_API	float		ps_r__Tree_SBC;		// scale bias correct
extern ECORE_API	Fvector		ps_r__Tree_Wave;

extern ECORE_API	float		ps_r__WallmarkTTL		;
extern ECORE_API	float		ps_r__WallmarkSHIFT		;
extern ECORE_API	float		ps_r__WallmarkSHIFT_V	;

extern ECORE_API	float		ps_r__GLOD_ssa_start;
extern ECORE_API	float		ps_r__GLOD_ssa_end	;
extern ECORE_API	float		ps_r__LOD			;
//.extern ECORE_API	float		ps_r__LOD_Power		;
extern ECORE_API	float		ps_r__ssaDISCARD	;
extern ECORE_API	float		ps_r__ssaDONTSORT	;
extern ECORE_API	float		ps_r__ssaHZBvsTEX	;
extern ECORE_API	int			ps_r__tf_Anisotropic;

// R1
extern ECORE_API	float		ps_r1_ssaLOD_A;
extern ECORE_API	float		ps_r1_ssaLOD_B;
extern ECORE_API	float		ps_r1_tf_Mipbias;
extern ECORE_API	float		ps_r1_lmodel_lerp;
extern ECORE_API	float		ps_r1_dlights_clip;
extern ECORE_API	float		ps_r1_pps_u;
extern ECORE_API	float		ps_r1_pps_v;

// R1-specific
extern ECORE_API	int			ps_r1_GlowsPerFrame;	// r1-only
extern ECORE_API	Flags32		ps_r1_flags;			// r1-only
enum
{
	R1FLAG_DLIGHTS				= (1<<0),
};

// R2
extern ECORE_API	float		ps_r2_ssaLOD_A;
extern ECORE_API	float		ps_r2_ssaLOD_B;
extern ECORE_API	float		ps_r2_tf_Mipbias;

// R2-specific
extern ECORE_API Flags32		ps_r2_ls_flags;				// r2-only
extern ECORE_API float			ps_r2_df_parallax_h;		// r2-only
extern ECORE_API float			ps_r2_df_parallax_range;	// r2-only
extern ECORE_API float			ps_r2_gmaterial;			// r2-only
extern ECORE_API float			ps_r2_tonemap_middlegray;	// r2-only
extern ECORE_API float			ps_r2_tonemap_adaptation;	// r2-only
extern ECORE_API float			ps_r2_tonemap_low_lum;		// r2-only
extern ECORE_API float			ps_r2_tonemap_amount;		// r2-only
extern ECORE_API float			ps_r2_ls_bloom_kernel_scale;// r2-only	// gauss
extern ECORE_API float			ps_r2_ls_bloom_kernel_g;	// r2-only	// gauss
extern ECORE_API float			ps_r2_ls_bloom_kernel_b;	// r2-only	// bilinear
extern ECORE_API float			ps_r2_ls_bloom_threshold;	// r2-only
extern ECORE_API float			ps_r2_ls_bloom_speed;		// r2-only
extern ECORE_API float			ps_r2_ls_dsm_kernel;		// r2-only
extern ECORE_API float			ps_r2_ls_psm_kernel;		// r2-only
extern ECORE_API float			ps_r2_ls_ssm_kernel;		// r2-only
extern ECORE_API Fvector		ps_r2_aa_barier;			// r2-only
extern ECORE_API Fvector		ps_r2_aa_weight;			// r2-only
extern ECORE_API float			ps_r2_aa_kernel;			// r2-only
extern ECORE_API float			ps_r2_mblur;				// .5f
extern ECORE_API int			ps_r2_GI_depth;				// 1..5
extern ECORE_API int			ps_r2_GI_photons;			// 8..256
extern ECORE_API float			ps_r2_GI_clip;				// EPS
extern ECORE_API float			ps_r2_GI_refl;				// .9f
extern ECORE_API float			ps_r2_ls_depth_scale;		// 1.0f
extern ECORE_API float			ps_r2_ls_depth_bias;		// -0.0001f
extern ECORE_API float			ps_r2_ls_squality;			// 1.0f
extern ECORE_API float			ps_r2_sun_near;				// 10.0f
extern ECORE_API float			ps_r2_sun_near_border;		// 1.0f
extern ECORE_API float			ps_r2_sun_tsm_projection;	// 0.2f
extern ECORE_API float			ps_r2_sun_tsm_bias;			// 0.0001f
extern ECORE_API float			ps_r2_sun_depth_far_scale;	// 1.00001f
extern ECORE_API float			ps_r2_sun_depth_far_bias;	// -0.0001f
extern ECORE_API float			ps_r2_sun_depth_near_scale;	// 1.00001f
extern ECORE_API float			ps_r2_sun_depth_near_bias;	// -0.0001f
extern ECORE_API float			ps_r2_sun_lumscale;			// 0.5f
//extern ECORE_API float			ps_r2_sun_lumscale_hemi;	// 1.0f
extern ECORE_API float			ps_r2_sun_lumscale_amb;		// 1.0f
extern ECORE_API float			ps_r2_zfill;				// .1f

extern ECORE_API float			ps_r2_dhemi_scale;			// 1.5f
extern ECORE_API int			ps_r2_dhemi_count;			// 5
extern ECORE_API float			ps_r2_slight_fade;			// 1.f
extern ECORE_API int			ps_r2_wait_sleep;

extern void						xrRender_initconsole	();
extern BOOL						xrRender_test_hw		();
extern void						xrRender_apply_tf		();

// KD
extern Flags32		ps_r2_test_flags;
extern Flags32		ps_r2_pp_flags;
extern Flags32		ps_r2_dof_flags;
extern u32			ps_ssao_mode;
extern u32			ps_aa_mode;
extern u32			ps_sunshafts_mode;
extern u32			ps_Render_mode;

extern u32			ps_ssao_quality;
extern u32			ps_aa_quality;
extern u32			ps_sunshafts_quality;
extern u32			ps_refl_quality;
extern u32			ps_dof_quality;
extern u32			ps_soft_shadows;
extern u32			ps_steep_parallax;
extern u32			ps_r__detail_radius;
extern Fvector		ps_r2_details_opt;
extern int			rmap_size;

extern float		center_height;

extern Fvector4		ps_r2_color_grading_params;

extern Fvector		ps_r2_blur_params;
extern float		ps_r2_ndr;

extern u32			ps_light_shadow_quality;

//rain map
extern float		ps_r2_rmap_tsm_projection;
extern float		ps_r2_rmap_tsm_bias;
extern float		ps_r2_rmap_near;
extern float		ps_r2_rmap_near_border;
extern float		ps_r2_rmap_depth_far_scale;
extern float		ps_r2_rmap_depth_far_bias;
extern float		ps_r2_rmap_depth_near_scale;
extern float		ps_r2_rmap_depth_near_bias;

extern float		ps_r2_sharpening;

extern Flags32		ps_service_flags;
extern u32			ps_refl_mode;

extern float		ps_r2_ao_intensity;
extern float		ssa_discard;
enum
{
	R2FLAG_DOF_COMMON = (1 << 0),
	R2FLAG_DOF_ZOOM = (1 << 1),
	R2FLAG_DOF_RELOAD = (1 << 2),
};

enum
{
	R2FLAG_SUN = (1 << 0),
	R2FLAG_SUN_FOCUS = (1 << 1),
	R2FLAG_SUN_TSM = (1 << 2),
	R2FLAG_SUN_DETAILS = (1 << 3),
	R2FLAG_TONEMAP = (1 << 4),
	R2FLAG_AA = (1 << 5),
	R2FLAG_GI = (1 << 6),
	R2FLAG_FASTBLOOM = (1 << 7),
	R2FLAG_GLOBALMATERIAL = (1 << 8),		// not used in release build
	R2FLAG_ZFILL = (1 << 9),		// used to enabled z-prepass
	R2FLAG_R1LIGHTS = (1 << 10),
	R2FLAG_SUN_IGNORE_PORTALS = (1 << 11),		// no console command, but used in sun render

	R2FLAG_VOLLIGHT = (1 << 12),	// added by kd instead of R2FLAG_SUN_STATIC which is not used at all

	R2FLAG_EXP_SPLIT_SCENE = (1 << 13),	// no console command, but used in main render
	R2FLAG_EXP_DONT_TEST_UNSHADOWED = (1 << 14),	// no console command, but used in lights render

	R2FLAG_USE_NVDBT = (1 << 15),		// used to enable NVidia depth bound test. On modern cards there is no difference in performance.
	R2FLAG_USE_NVSTENCIL = (1 << 16),		// not used, but always set up in render flag.

	R2FLAG_EXP_MT_CALC = (1 << 17),		// no console command, but used in details and hom render

										// added by kd
										R2FLAG_SOFT_WATER = (1 << 18),
										R2FLAG_SOFT_PARTICLES = (1 << 19),
										R2FLAG_TRUE_SHADOWS = (1 << 20),
										R2FLAG_BLOODMARKS = (1 << 21),
										R2FLAG_DETAIL_BUMP_DEBUG = (1 << 22),
										R2FLAG_FOG_VOLUME = (1 << 23),
										R2FLAG_GBUFFER_OPT = (1 << 24),
										R2FLAG_LENS_FLARES = (1 << 25),
										R2FLAG_LENS_DIRT = (1 << 26),
										R2FLAG_WET_SURFACES = (1 << 27),
										R2FLAG_PUDDLES = (1 << 28),
										R2FLAG_AO = (1 << 29),
										R2FLAG_ACTOR_SHADOW = (1 << 29),
};

// enum for my test stuff :)
enum
{
	R2FLAG_RAIN_MAP = (1 << 0),
	R2FLAG_PCSS = (1 << 1),
	R2FLAG_HW_INSTANSING = (1 << 2),
	R2FLAG_REPROJECT = (1 << 3),
};

enum
{
	S_EXTENDED_GRAPH_STATS = (1 << 0),
};

enum
{
	R2PP_FLAG_AA = (1 << 0),
	R2PP_FLAG_MBLUR = (1 << 1),
	R2PP_FLAG_DOF = (1 << 2),
	R2PP_FLAG_RAIN_DROPS = (1 << 3),
	R2PP_FLAG_SUNSHAFTS = (1 << 4),
	R2PP_FLAG_SSAO = (1 << 5),
	R2PP_FLAG_SUPERSAMPLING_AA = (1 << 6),
	R2PP_FLAG_REFLECTIONS = (1 << 7),
	R2PP_FLAG_RAIN_DROPS_CONTROL = (1 << 8),
	R2PP_FLAG_LENS_DIRT_CONTROL = (1 << 9),
	R2PP_FLAG_DOF_CONTROL = (1 << 10),
	R2PP_FLAG_LIGHT_SHADOW_FADE = (1 << 11),
};

enum
{
	R2RM_NORMAL,
	R2RM_THERMAL,
};

enum
{
	R2AA_FXAA,
	R2AA_SMAA_1X,
	R2AA_SMAA_T2X,
};

enum
{
	R2SS_VOLUMETRIC,
	R2SS_SCREEN_SPACE,
};

extern bool actor_torch_enabled;
extern bool is_torch_processed;

extern Fmatrix sun_near_shadow_xform;
extern Fmatrix sun_far_shadow_xform;
extern Fmatrix sun_clouds_shadow_xform;
#endif

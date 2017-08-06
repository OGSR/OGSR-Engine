#pragma once

class light;

#define DU_SPHERE_NUMVERTEX 92
#define DU_SPHERE_NUMFACES	180
#define DU_CONE_NUMVERTEX	18
#define DU_CONE_NUMFACES	32
#define DU_VOLUMETRIC_NUMVERTEX	400
#define DU_VOLUMETRIC_NUMFACES	200

class CRenderTarget		: public IRender_Target
{
private:
	u32							dwWidth;
	u32							dwHeight;
	u32							dwAccumulatorClearMark;
public:
	u32							dwLightMarkerID;
	// 
	IBlender*					b_occq;
	IBlender*					b_accum_mask;
	IBlender*					b_accum_direct;
	IBlender*					b_accum_point;
	IBlender*					b_accum_spot;
	IBlender*					b_accum_reflected;
	IBlender*					b_bloom;
	IBlender*					b_luminance;
	IBlender*					b_combine;
#ifdef DEBUG
	struct		dbg_line_t		{
		Fvector	P0,P1;
		u32		color;
	};
	xr_vector<std::pair<Fsphere,Fcolor> >		dbg_spheres;
	xr_vector<dbg_line_t>						dbg_lines;
	xr_vector<Fplane>							dbg_planes;
#endif

	// MRT-path
	ref_rt						rt_Depth;			// Z-buffer like - initial depth
	ref_rt						rt_Position;		// 64bit,	fat	(x,y,z,?)				(eye-space)
	ref_rt						rt_Normal;			// 64bit,	fat	(x,y,z,hemi)			(eye-space)
	ref_rt						rt_Color;			// 64/32bit,fat	(r,g,b,specular-gloss)	(or decompressed MET-8-8-8-8)

	// 
	ref_rt						rt_Accumulator;		// 64bit		(r,g,b,specular)
	ref_rt						rt_Accumulator_temp;// only for HW which doesn't feature fp16 blend
	ref_rt						rt_Generic_0;		// 32bit		(r,g,b,a)				// post-process, intermidiate results, etc.
	ref_rt						rt_Generic_1;		// 32bit		(r,g,b,a)				// post-process, intermidiate results, etc.
	ref_rt						rt_Bloom_1;			// 32bit, dim/4	(r,g,b,?)
	ref_rt						rt_Bloom_2;			// 32bit, dim/4	(r,g,b,?)
	ref_rt						rt_LUM_64;			// 64bit, 64x64,	log-average in all components
	ref_rt						rt_LUM_8;			// 64bit, 8x8,		log-average in all components

	ref_rt						rt_LUM_pool	[4]	;	// 1xfp32,1x1,		exp-result -> scaler
	ref_texture					t_LUM_src		;	// source
	ref_texture					t_LUM_dest		;	// destination & usage for current frame

	// env
	ref_texture					t_envmap_0		;	// env-0
	ref_texture					t_envmap_1		;	// env-1

	// smap
	ref_rt						rt_smap_surf;	// 32bit,		color
	ref_rt						rt_smap_depth;	// 24(32) bit,	depth 
	IDirect3DSurface9*			rt_smap_ZB;		//

	// Textures
	IDirect3DVolumeTexture9*	t_material_surf;
	ref_texture					t_material;

	IDirect3DTexture9*			t_noise_surf	[TEX_jitter_count + 1];
	ref_texture					t_noise			[TEX_jitter_count + 1];
private:
	// OCCq
	ref_shader					s_occq;

	// Accum
	ref_shader					s_accum_mask	;
	ref_shader					s_accum_direct	;
	ref_shader					s_accum_point	;
	ref_shader					s_accum_spot	;
	ref_shader					s_accum_reflected;

	ref_geom					g_accum_point	;
	ref_geom					g_accum_spot	;
	ref_geom					g_accum_omnipart;

	IDirect3DVertexBuffer9*		g_accum_point_vb;
	IDirect3DIndexBuffer9*		g_accum_point_ib;

	IDirect3DVertexBuffer9*		g_accum_omnip_vb;
	IDirect3DIndexBuffer9*		g_accum_omnip_ib;

	IDirect3DVertexBuffer9*		g_accum_spot_vb	;
	IDirect3DIndexBuffer9*		g_accum_spot_ib	;

	// Bloom
	ref_geom					g_bloom_build;
	ref_geom					g_bloom_filter;
	ref_shader					s_bloom_dbg_1;
	ref_shader					s_bloom_dbg_2;
	ref_shader					s_bloom;
	float						f_bloom_factor;

	// Luminance
	ref_shader					s_luminance;
	float						f_luminance_adapt;

	// Combine
	ref_geom					g_combine;
	ref_geom					g_combine_VP;		// xy=p,zw=tc
	ref_geom					g_combine_2UV;
	ref_geom					g_aa_blur;
	ref_geom					g_aa_AA;
	ref_shader					s_combine_dbg_0;
	ref_shader					s_combine_dbg_1;
	ref_shader					s_combine_dbg_Accumulator;
	ref_shader					s_combine;
public:
	ref_shader					s_postprocess;
	ref_geom					g_postprocess;
	ref_shader					s_menu;
	ref_geom					g_menu;
private:
	float						im_noise_time;
	u32							im_noise_shift_w;
	u32							im_noise_shift_h;
	
	float						param_blur;
	float						param_gray;
	float						param_duality_h;
	float						param_duality_v;
	float						param_noise;
	float						param_noise_scale;
	float						param_noise_fps;
	u32							param_color_base;
	u32							param_color_gray;
	u32							param_color_add;

	// KD 
public:
	// blenders
	IBlender*					b_sunshafts;
	IBlender*					b_thermal_vision;
	IBlender*					b_mblur;
	IBlender*					b_dof;
	IBlender*					b_rain_drops;
	IBlender*					b_ssao;
	IBlender*					b_aa;
	IBlender*					b_rmap;
	IBlender*					b_blur;
	IBlender*					b_reflections;
	IBlender*					b_volumetric;
	// rendertargets
	ref_rt						rt_sunshafts_0;		// ss0
	ref_rt						rt_sunshafts_1;		// ss1
													//	ref_rt						rt_mblur			;
	ref_rt						rt_dof;
	//	ref_rt						rt_rain_drops		;
	ref_rt						rt_ssao;
	ref_rt						rt_rain_map_depth;	// basic rain map
	ref_rt						rt_rain_map_surf;	// rain map for not supporting blend
	ref_rt						rt_rmap;	// rain map projected
	ref_rt						rt_rmap_temp;
	ref_rt						rt_Accumulator_reflected;
	ref_rt						rt_blur;
	ref_rt						rt_blur_temp;
	ref_rt						rt_volumetric;
	//	ref_rt						rt_pre_reflections	;
	// for SMAA
	ref_rt						rt_smaa_edgetex;
	ref_rt						rt_smaa_blendtex;
	ref_rt						rt_prev_frame0;
	ref_rt						rt_flares;
	ref_rt						rt_reflections;
	ref_rt						rt_rain;
	ref_rt						rt_smap_depth_near;
	ref_rt						rt_smap_depth_far;
	/*	ref_rt						rt_prev_frame1		;
	ref_rt						rt_prev_frame2		;
	ref_rt						rt_prev_Position	;*/

	ref_texture					t_noise_hd;		// hd noise
												//	ref_texture					t_trig				;		// texture for trigonometric lookup table
												// surfaces
	IDirect3DTexture9*			t_noise_hd_surf;		// surface for hd noise
														//	IDirect3DTexture9*			t_trig_surf		;		// surface for trigonometric lookup table
														// shaders
	ref_shader					s_sunshafts;
	ref_shader					s_thermal_vision;
	ref_shader					s_mblur;
	ref_shader					s_dof;
	ref_shader					s_rain_drops;
	ref_shader					s_ssao;
	ref_shader					s_aa;
	ref_shader					s_flare;
	ref_shader					s_rmap;
	ref_shader					s_blur;
	ref_shader					s_reflections;
	ref_shader					s_water;
	//	ref_shader					s_volumetric;
	ref_shader					s_combine_volumetric;
	ref_shader					s_accum_volumetric;
	// geometry
	ref_geom					g_KD;
	ref_geom					g_Test_AA;
	ref_geom					flare_geom;
	ref_geom					water_geom;

	// buffers
	ref_geom					g_accum_volumetric;
	IDirect3DVertexBuffer9*		g_accum_volumetric_vb;
	IDirect3DIndexBuffer9*		g_accum_volumetric_ib;

	// for rain map
//	r_caster					rain_caster;
	u32							dwRainMarkerID;
	u32							dwRainTargetClearMark;
	u32							dwAccumulatorClearMarkRefl;
	u32							dwFlareClearMark;
	u32							dwVolumetricClearMark;

	u32							smap_phase;

public:
								CRenderTarget			();
								~CRenderTarget			();
	void						accum_point_geom_create	();
	void						accum_point_geom_destroy();
	void						accum_omnip_geom_create	();
	void						accum_omnip_geom_destroy();
	void						accum_spot_geom_create	();
	void						accum_spot_geom_destroy	();

	void						accum_volumetric_geom_create();
	void						accum_volumetric_geom_destroy();

	void						u_stencil_optimize		(BOOL		common_stencil=TRUE);
	void						u_compute_texgen_screen	(Fmatrix&	dest);
	void						u_compute_texgen_jitter	(Fmatrix&	dest);
	void						u_setrt					(const ref_rt& _1, const ref_rt& _2, const ref_rt& _3, IDirect3DSurface9* zb);
	void						u_setrt					(u32 W, u32 H, IDirect3DSurface9* _1, IDirect3DSurface9* _2, IDirect3DSurface9* _3, IDirect3DSurface9* zb);
	void						u_calc_tc_noise			(Fvector2& p0, Fvector2& p1);
	void						u_calc_tc_duality_ss	(Fvector2& r0, Fvector2& r1, Fvector2& l0, Fvector2& l1);
	BOOL						u_need_PP				();
	BOOL						u_DBT_enable			(float zMin, float zMax);
	void						u_DBT_disable			();

	void						phase_scene_prepare		();
	void						phase_scene_begin		();
	void						phase_scene_end			();
	void						phase_occq				();
	void						phase_wallmarks			();
	void						phase_smap_direct		(light* L,	u32 sub_phase);
	void						phase_smap_direct_tsh	(light* L,	u32 sub_phase);
	void						phase_smap_spot_clear	();
	void						phase_smap_spot			(light* L);
	void						phase_smap_spot_tsh		(light* L);
	void						phase_accumulator		();
	void						shadow_direct			(light* L, u32 dls_phase);
	
	BOOL						enable_scissor			(light* L);		// true if intersects near plane
	void						enable_dbt_bounds		(light* L);

	void						disable_aniso			();

	void						draw_volume				(light* L, bool volumetric = false);
	void						accum_direct			(u32	sub_phase);
	void						accum_direct_f			(u32	sub_phase);
	void						accum_direct_lum		();
	void						accum_direct_blend		();
	void						accum_point				(light* L);
	void						accum_spot				(light* L);
	void						accum_reflected			(light* L);
	void						phase_bloom				();
	void						phase_luminance			();
	void						phase_combine			();
	void						phase_pp				();

	void						phase_rmap_direct(u32 sub_phase);
	void						phase_rmap_direct_tsh();
	void						phase_rmap_accumulator();
	void						accum_plane_spot(light* L);
	void						accum_volumetric_spot(light* L);
	void						apply_rmap(u32 sub_phase);
	void						phase_flares			();
	void						phase_combine_volumetric();
	void						phase_aa				();
	void						phase_sunshafts			();
	void						phase_motion_blur		();
	void						phase_blur				();
	void						phase_dof				();
	void						phase_rain_drops		();
	void						phase_ssao				();
	void						phase_thermal_vision	();
	void						save_previous_frame		();
	//	void						phase_downsample		();
	void						apply_reprojection		();
	void						process_fxaa			();
	void						process_smaa			();
	void						process_taa				();
	void						phase_fog_volumes		();
	void						render_flare			(light* L, BOOL is_sun = FALSE);
	void						phase_wet_reflections	();
	//	void						phase_reflections_preprocess	();
	void						phase_puddles			();
	void						phase_rain				();
	void						phase_accumulator_reflected		();
	void						phase_accumulator_volumetric	();
	void						accum_direct_volumetric	(u32 sub_phase, Fmatrix& m_shadow);
	void						prepare_simple_quad		(ref_rt& DEST_RT, ref_selement& SHADER, u32& Offset, float downscale);
	void						prepare_simple_quad		(ref_rt& DEST_RT, ref_selement& SHADER, u32& Offset, u32 width, u32 height, float downscale);
	void						render_simple_quad		(ref_rt& DEST_RT, ref_selement& SHADER, float downscale);
	void						render_simple_quad		(ref_rt& DEST_RT, ref_selement& SHADER, u32 width, u32 height, float downscale);
	//	void						copy_shadow_map			();

	virtual void				set_blur				(float	f)		{ param_blur=f;						}
	virtual void				set_gray				(float	f)		{ param_gray=f;						}
	virtual void				set_duality_h			(float	f)		{ param_duality_h=_abs(f);			}
	virtual void				set_duality_v			(float	f)		{ param_duality_v=_abs(f);			}
	virtual void				set_noise				(float	f)		{ param_noise=f;					}
	virtual void				set_noise_scale			(float	f)		{ param_noise_scale=f;				}
	virtual void				set_noise_fps			(float	f)		{ param_noise_fps=_abs(f)+EPS_S;	}
	virtual void				set_color_base			(u32	f)		{ param_color_base=f;				}
	virtual void				set_color_gray			(u32	f)		{ param_color_gray=f;				}
	virtual void				set_color_add			(u32	f)		{ param_color_add=f;				}

	virtual u32					get_width				()				{ return dwWidth;					}
	virtual u32					get_height				()				{ return dwHeight;					}

#ifdef DEBUG
	IC void						dbg_addline				(Fvector& P0, Fvector& P1, u32 c)					{
		dbg_lines.push_back		(dbg_line_t());
		dbg_lines.back().P0		= P0;
		dbg_lines.back().P1		= P1;
		dbg_lines.back().color	= c;
	}
	IC void						dbg_addplane			(Fplane& P0,  u32 c)								{
		dbg_planes.push_back(P0);
	}
#else
	IC void						dbg_addline				(Fvector& P0, Fvector& P1, u32 c)					{}
	IC void						dbg_addplane			(Fplane& P0,  u32 c)								{}
#endif
};

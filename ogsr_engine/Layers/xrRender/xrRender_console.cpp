#include "stdafx.h"

#include "xrRender_console.h"
#include "dxRenderDeviceRender.h"
#include <..\NVIDIA_DLSS\DLSS\include\nvsdk_ngx.h>

u32 r2_SmapSize = 2048;
constexpr xr_token SmapSizeToken[] = {{"1536x1536", 1536},
                                      {"2048x2048", 2048},
                                      {"2560x2560", 2560},
                                      {"3072x3072", 3072},
                                      {"4096x4096", 4096},
                                      //{"6144x6144", 6144},
                                      //{"8192x8192", 8192},
                                      //{ "16384x16384", 16384 },
                                      {nullptr, 0}};

u32 ps_r_pp_aa_mode = DLSS;
constexpr xr_token pp_aa_mode_token[] = {
    {"st_opt_off", NO_AA},
    {"st_opt_dlss", DLSS},
    {"st_opt_fsr2", FSR2},
    {"st_opt_taa", TAA},
    {"st_opt_smaa", SMAA},

    {nullptr, 0},
};

u32 ps_r_dlss_preset = NVSDK_NGX_DLSS_Hint_Render_Preset_F;
u32 ps_r_dlss_3dss_preset = NVSDK_NGX_DLSS_Hint_Render_Preset_F;
constexpr xr_token dlss_mode_token[]{
    {"st_opt_dlss_default", NVSDK_NGX_DLSS_Hint_Render_Preset_Default}, // default behavior, may or may not change after OTA
    {"st_opt_dlss_f", NVSDK_NGX_DLSS_Hint_Render_Preset_F},
    {"st_opt_dlss_j", NVSDK_NGX_DLSS_Hint_Render_Preset_J},
    {"st_opt_dlss_k", NVSDK_NGX_DLSS_Hint_Render_Preset_K},
    {},
};

#pragma todo("Simp : что-то не вижу вообще никакой разницы от изменения этой настройки."
u32 ps_r_dlss_3dss_quality = NVSDK_NGX_PerfQuality_Value_DLAA;
constexpr xr_token dlss_quality_token[]{
    {"st_opt_dlss_max_perf", NVSDK_NGX_PerfQuality_Value_MaxPerf},
    {"st_opt_dlss_balanced", NVSDK_NGX_PerfQuality_Value_Balanced},
    {"st_opt_dlss_max_quality", NVSDK_NGX_PerfQuality_Value_MaxQuality},
    {"st_opt_dlss_ultra_performance", NVSDK_NGX_PerfQuality_Value_UltraPerformance},
    {"st_opt_dlss_ultra_quality", NVSDK_NGX_PerfQuality_Value_UltraQuality},
    {"st_opt_dlss_dlaa", NVSDK_NGX_PerfQuality_Value_DLAA},
    {},
};

float ps_r_dlss_3dss_scale_factor{2.f};

u32 ps_r_sunshafts_mode = SS_SS_MANOWAR;
constexpr xr_token sunshafts_mode_token[]{{"st_opt_off", SS_OFF},
                                          {"volumetric", SS_VOLUMETRIC},
                                          {"ss_ogse", SS_SS_OGSE},
                                          {"ss_manowar", SS_SS_MANOWAR},
                                          {"combined_ogse", SS_COMBINED_OGSE},
                                          {"combined_manowar", SS_COMBINED_MANOWAR},
                                          {}};

// Sunshafts
u32 ps_r_sun_shafts = 3;

float ps_r_ss_sunshafts_length = 0.9f; // 1.0f;
float ps_r_ss_sunshafts_radius = 2.f; // 1.0f;
float ps_r_prop_ss_radius = 1.56f;
float ps_r_prop_ss_blend = 0.25f; // 0.066f;
float ps_r_prop_ss_sample_step_phase0 = 0.09f;
float ps_r_prop_ss_sample_step_phase1 = 0.07f;

u32 ps_preset = 2;
constexpr xr_token qpreset_token[] = {{"Minimum", 0}, {"Low", 1}, {"Default", 2}, {"High", 3}, {"Extreme", 4}, {nullptr, 0}};

u32 ps_r_ao_mode = AO_MODE_GTAO;
constexpr xr_token ao_mode_token[] = {{"st_gtao", AO_MODE_GTAO}, {"st_ssdo", AO_MODE_SSDO}, {nullptr, 0}};

u32 ps_r_ao_quality = 0;
constexpr xr_token qssao_token[] = {{"st_opt_off", 0},
                                    {"st_opt_low", 1},
                                    {"st_opt_medium", 2},
                                    {"st_opt_high", 3},
                                    {nullptr, 0}};

u32 ps_r_sun_quality = 1; //	=	0;
constexpr xr_token qsun_quality_token[] = {{"st_opt_low", 0},
                                           {"st_opt_medium", 1},
                                           {"st_opt_high", 2},
                                           {"st_opt_ultra", 3},
                                           {"st_opt_extreme", 4},
                                           {nullptr, 0}};

//	“Off”
//	“DX10.0 style [Standard]”
//	“DX10.1 style [Higher quality]”

// Common
extern int psSkeletonUpdate;
extern float r__dtex_range;

Fvector3 ps_r_taa_jitter{};
Fvector2 ps_r_taa_jitter_full{};
float ps_r_cas{};

int ps_r__LightSleepFrames = 10;

float ps_r__WallmarkTTL = 50.f;
float ps_r__WallmarkSHIFT = 0.0001f;
float ps_r__WallmarkSHIFT_V = 0.0001f;

float ps_r__GLOD_ssa_start = 256.f;
float ps_r__GLOD_ssa_end = 64.f;

float ps_r__LOD = 0.75f;
float ps_r__LOD_k = 1.f;

float ps_r__ssaDISCARD = 3.5f; // RO

int ps_r__tf_Anisotropic = 16;
float ps_r__tf_Mipbias = -0.5f;

// R2
float ps_r2_ssaLOD_A = 64.f;
float ps_r2_ssaLOD_B = 48.f;

// crookr
int scope_fake_enabled = 0;

float scope_fake_power = 0.66f;
float scope_fake_radius = 0;
float scope_fake_interp = 0.15f;

Fvector4 ps_scope1_params = Fvector4().set(0.2f, 1.f, 1.f, 0.0f); // inner blur, outer blur, brightness, ___
Fvector4 ps_scope2_params = Fvector4().set(0.0066f, 0.66f, 0.25f, 0.25f); // chroma abber, fog attack(aim), fog attack(move), fog max travel
Fvector4 ps_scope3_params = Fvector4().set(1.25f, 4.f, 0.0f, 0.0f); // relative fog radius, fog sharpness, ___, ___

// R2-specific
Flags64 ps_r2_ls_flags = {
    R2FLAG_SUN |
    R2FLAG_EXP_DONT_TEST_UNSHADOWED | 
    R3FLAG_DYN_WET_SURF |
    R3FLAG_VOLUMETRIC_SMOKE |
    R2FLAG_DETAIL_BUMP | 
    R2FLAG_SSFX_HEIGHT_FOG |
    R2FLAG_SSFX_BLOOM |
    R2FLAG_STEEP_PARALLAX | 
    R2FLAG_TONEMAP | 
    R2FLAG_VOLUMETRIC_LIGHTS |
    R2FLAG_EXP_MT_RAIN |
    R2FLAG_EXP_MT_SUN |
    R2FLAG_EXP_MT_PARTICLES |
    R2FLAG_EXP_MT_LIGHTS |
    R2FLAG_EXP_MT_BONES |
    R2FLAG_LIGHT_NO_DIST_SHADOWS
};

Flags64 ps_r2_ls_flags_ext = {
    R2FLAGEXT_ENABLE_TESSELLATION | 
//    R2FLAGEXT_RAIN_DROPS | 
//    R2FLAGEXT_RAIN_DROPS_CONTROL | 
//    R2FLAGEXT_MASK | 
//    R2FLAGEXT_MASK_CONTROL | 
//    R2FLAGEXT_MT_TEXLOAD  |
    R2FLAGEXT_SSLR |
    R2FLAGEXT_SSFX_INTER_GRASS |
    R2FLAGEXT_FONT_SHADOWS
};

BOOL ps_no_scale_on_fade = 0; // Alundaio

float ps_r2_df_parallax_h = 0.02f;
float ps_r2_df_parallax_range = 75.f;
float ps_r2_tonemap_middlegray = 1.f; // r2-only
float ps_r2_tonemap_adaptation = 1.f; // r2-only
float ps_r2_tonemap_low_lum = 0.0001f; // r2-only
float ps_r2_tonemap_amount = 0.7f; // r2-only
float ps_r2_ls_bloom_kernel_g = 3.f; // r2-only
float ps_r2_ls_bloom_kernel_b = .7f; // r2-only
float ps_r2_ls_bloom_speed = 100.f; // r2-only
float ps_r2_ls_bloom_kernel_scale = .7f; // r2-only	// gauss
float ps_r2_ls_dsm_kernel = .7f; // r2-only
float ps_r2_ls_psm_kernel = .7f; // r2-only
float ps_r2_ls_ssm_kernel = .7f; // r2-only
float ps_r2_ls_bloom_threshold = .00001f; // r2-only
float ps_r2_mblur = .0f; // .5f
float ps_r2_ls_depth_scale = 1.00001f; // 1.00001f
float ps_r2_ls_depth_bias = -0.00005f; // SSS19 Edited //-0.0003f; // -0.0001f
float ps_r2_ls_squality = 1.0f; // 1.00f
float ps_r2_sun_tsm_bias = -0.01f; //
float ps_r2_sun_near = 20.f; // 12.0f

extern float OLES_SUN_LIMIT_27_01_07; //	actually sun_far

float ps_r2_sun_near_border = 0.75f; // 1.0f
float ps_r2_sun_depth_far_scale = 1.00000f; // 1.00001f
float ps_r2_sun_depth_far_bias = -0.00002f; // -0.0000f
float ps_r2_sun_depth_near_scale = 1.0000f; // 1.00001f
float ps_r2_sun_depth_near_bias = 0.00001f; // -0.00005f
float ps_r2_sun_lumscale = 1.0f; // 1.0f
float ps_r2_sun_lumscale_hemi = 1.0f; // 1.0f
float ps_r2_sun_lumscale_amb = 1.0f;
float ps_r2_gmaterial = 2.2f; //

float ps_r2_dhemi_sky_scale = 0.08f; // 1.5f
float ps_r2_dhemi_light_scale = 0.2f;
float ps_r2_dhemi_light_flow = 0.1f;

int ps_r2_dhemi_count = 5; // 5

float ps_lens_flare_sun_blend{};

float ps_r2_lt_smooth = 1.f; // 1.f
float ps_r2_slight_fade = 2.0f; // 0.5f; // 1.f

Fvector4 ps_ssfx_lut{}; // x - интенсивность, y - номер эффекта
Fvector3 ps_ssfx_shadows{
    1024.f, 1536.f,
    0.0f}; // x - Minimum shadow map resolution. When lights are away from the player the resolution of shadows drop to improve performance ( at the cost of image quality ), y -
           // Maximum shadow map resolution. When lights are closer, the resolution increases to improve the image quality of shadows ( at the cost of performance ).
Fvector3 ps_ssfx_shadow_bias{0.4f, 0.03f, 0.0f};

Fvector3 ps_ssfx_volumetric = {1.0f, 1.0f, 0.0f};

int ps_ssfx_bloom_use_presets = 0;
Fvector4 ps_ssfx_bloom_1 = {4.f, 4.f, 0.f, 0.5f}; // Threshold, Exposure, -, Sky
Fvector4 ps_ssfx_bloom_2 = {1.7f, 0.7f, 0.5f, 0.5f}; // Blur Radius, Vibrance, Lens, Dirt

int ps_ssfx_pom_refine = 0;
Fvector4 ps_ssfx_pom = {16, 12, 0.035f, 0.4f}; // Samples , Range, Height, AO

// Screen Space Shaders Stuff
Fvector4 ps_ssfx_wind_grass{9.5f, 1.4f, 1.5f, 0.4f}; // Anim Speed, Turbulence, Push, Wave
Fvector4 ps_ssfx_wind_trees{11.0f, 0.15f, 0.5f, 0.1f}; // Branches Speed, Trunk Speed, Bending, Min Wind Speed

Fvector4 ps_ssfx_florafixes_1{0.1f, 0.2f, 0.2f, 0.3f}; // Specular value when the grass is dry, Specular value when the grass is wet, Specular when trees and bushes are dry, Specular when trees and bushes are wet
Fvector4 ps_ssfx_florafixes_2{2.0f, 1.0f, 0.0f, 0.0f}; // Intensity of the flora SubSurface Scattering, How much sun color is added to the flora SubSurface Scattering (1.0 is 100% sun color)

int ps_ssfx_is_underground{};

int ps_ssfx_gloss_method{1};
Fvector3 ps_ssfx_gloss_minmax{0.6f, 0.9f, 0.0f}; // Minimum value of gloss, Maximum value of gloss, Extra gloss to the weapons HUD elements when raining
Fvector4 ps_ssfx_lightsetup_1{0.35f, 0.5f, 1.0f, 1.0f}; // intensity of specular lighting, Porcentage of the specular color. ( 0 = 0% | 1 = 100% ), Automatic adjustment of gloss based on wetness (0 or 1), Value to control the maximum value of gloss when full wetness is reached. ( 0 = 0% | 1 = 100% )
float ps_ssfx_gloss_factor{}; //Управляется из IGame_Persistent::UpdateRainGloss()

Fvector4 ps_ssfx_wetsurfaces_1_cfg{1.5f, 1.4f, 0.7f, 1.25f}; //ripples_size, ripples_speed, ripples_min_speed, ripples_intensity
Fvector4 ps_ssfx_wetsurfaces_2_cfg{1.2f, 1.5f, 0.2f, 0.7f}; // waterfall_size, waterfall_speed, waterfall_min_speed, waterfall_intensity
Fvector4 ps_ssfx_wetsurfaces_1{}, ps_ssfx_wetsurfaces_2{}; // Управляется из IGame_Persistent::UpdateRainGloss()

Fvector4 ps_ssfx_hud_drops_1_cfg{3.0f, 1.f, 1.f, 50.f}; // Quantity of drops, Refrelction intensity, Refraction intensity, Speed of the drops animation
Fvector4 ps_ssfx_hud_drops_2_cfg{50.f, 50.f, 0.75f, 2.f}; // Drops build up speed, Drying speed, Size of the drops, Raindrops gloss intensity
Fvector4 ps_ssfx_hud_drops_1{}, ps_ssfx_hud_drops_2{}; // Значениями этих векторов управляет IGame_Persistent::UpdateHudRaindrops()

Fvector4 ps_ssfx_blood_decals{0.6f, 0.6f, 0.f, 0.f};

Fvector4 ps_ssfx_rain_1{10.0f, 0.02f, 5.f, 2.f}; // Len, Width, Speed, Quality
Fvector4 ps_ssfx_rain_2{0.4f, 0.5f, 5.0f, 1.0f}; // Alpha, Brigthness, Refraction, Reflection
Fvector4 ps_ssfx_rain_3{0.95f, 0.5f, 0.0f, 0.0f}; // Alpha, Refraction ( Splashes )

Fvector3 ps_ssfx_shadow_cascades{20.f, 40.f, 160.f};
Fvector4 ps_ssfx_grass_shadows = {1.0f, 0.0f, 0.0f, 0.0f}; // X - каскады на которых будут рендериться тени (0 - на первом, 1 - на первом и втором, 2 - на всех трёх), Y - устарело и более не используется, Z - дальность на которой будут рендериться тени от источников света (НЕ СОЛНЦА)
Fvector4 ps_ssfx_grass_interactive{1.f, static_cast<float>(GRASS_SHADER_DATA_COUNT), 2000.f, 1.0f};
Fvector4 ps_ssfx_int_grass_params_1{2.0f, 1.0f, 1.0f, 25.f};
Fvector4 ps_ssfx_int_grass_params_2{1.0f, 5.0f, 1.0f, 1.0f};

BOOL ps_ssfx_terrain_grass_align{TRUE}; // Grass align
float ps_ssfx_terrain_grass_slope{1.0f}; // Grass slope limit

float ps_ssfx_wpn_dof_2 = 0.5f;

int ps_r3_dyn_wet_surf_opt = 1;
float ps_r3_dyn_wet_surf_near = 5.f; // 10.0f
float ps_r3_dyn_wet_surf_far = 270.f; // 200.0f //при 100 при резкой смене погоды видна граница намокшей земли и сухой когда вертишь камеру, но если выставить в районе 300 - намокание может вообще пропасть.

int ps_r3_dyn_wet_surf_sm_res = 1024; // 256
int ps_r3_dyn_wet_surf_enable_streaks = 0;

float ps_r2_rain_drops_intensity = 0.00003f;
float ps_r2_rain_drops_speed = 1.25f;

float ps_r2_visor_refl_intensity = 0.39f;
float ps_r2_visor_refl_radius = 0.4f;

int ps_r__detail_radius = 100;

u32 dm_size = 24;
u32 dm_cache1_line = 12; // dm_size*2/dm_cache1_count
u32 dm_cache_line = 49; // dm_size+1+dm_size
u32 dm_cache_size = 2401; // dm_cache_line*dm_cache_line
float dm_fade = 47.5; // float(2*dm_size)-.5f;
u32 dm_current_size = 24;
u32 dm_current_cache1_line = 12; // dm_current_size*2/dm_cache1_count
u32 dm_current_cache_line = 49; // dm_current_size+1+dm_current_size
u32 dm_current_cache_size = 2401; // dm_current_cache_line*dm_current_cache_line
float dm_current_fade = 47.5; // float(2*dm_current_size)-.5f;

float ps_current_detail_density = 0.6;
float ps_current_detail_scale = 1.f;
float ps_r2_no_details_radius = 0.f;
float ps_r2_no_rain_radius = 0.f;

float ps_r2_gloss_factor = 4.0f;

int ps_pnv_mode = 0;

float ps_pnv_noise = 0.15;
float ps_pnv_scanlines = 0.175;
float ps_pnv_scintillation = 0.999;
float ps_pnv_position = 100.0;
float ps_pnv_radius = 0.5;

float ps_pnv_params_1 = 0; // unused
float ps_pnv_params_2 = 1;
float ps_pnv_params_3 = 0.1f;
float ps_pnv_params_4 = 1;

float ps_pnv_params_1_2 = 2;
float ps_pnv_params_2_2 = 0;
float ps_pnv_params_3_2 = 0;
float ps_pnv_params_4_2 = 1;

// textures
int psTextureLOD = 0;

float ps_r2_img_exposure = 1.0f; // r2-only
float ps_r2_img_gamma = 1.0f; // r2-only
float ps_r2_img_saturation = 1.0f; // r2-only

Fvector ps_r2_img_cg{0.5f, 0.5f, 0.5f};

float ps_r__opt_dist = 750.f;

#include "../../xr_3da/xr_ioconsole.h"
#include "../../xr_3da/xr_ioc_cmd.h"

float ps_particle_update_coeff = 0.3f;

// Geometry optimization from Anomaly
int opt_static_geom = 0;
int opt_shadow_geom = 0;

int r_back_buffer_count{2};

extern int delay_invisible_min, delay_invisible_max;

//-----------------------------------------------------------------------
class CCC_detail_radius : public CCC_Integer
{
public:
    void apply()
    {
        dm_current_size = iFloor((float)ps_r__detail_radius / 4) * 2;
        dm_current_cache1_line = dm_current_size * 2 / 4; // assuming cache1_count = 4
        dm_current_cache_line = dm_current_size + 1 + dm_current_size;
        dm_current_cache_size = dm_current_cache_line * dm_current_cache_line;
        dm_current_fade = float(2 * dm_current_size) - .5f;
    }

    CCC_detail_radius(LPCSTR N, int* V, int _min = 0, int _max = 999) : CCC_Integer(N, V, _min, _max){};
    virtual void Execute(LPCSTR args)
    {
        CCC_Integer::Execute(args);
        apply();
    }
    virtual void Status(TStatus& S) { CCC_Integer::Status(S); }
};

class CCC_detail_reset : public CCC_Float
{
public:
    void apply()
    {
        if (RImplementation.Details)
            RImplementation.Details->need_init = true;
    }

    CCC_detail_reset(LPCSTR N, float* V, float _min = 0, float _max = 1) : CCC_Float(N, V, _min, _max){}
    virtual void Execute(LPCSTR args)
    {
        CCC_Float::Execute(args);
        apply();
    }
    virtual void Status(TStatus& S) { CCC_Float::Status(S); }
};

class CCC_tf_Aniso : public CCC_Integer
{
public:
    void apply() const
    {
        if (nullptr == HW.pDevice)
            return;
        int val = *value;
        clamp(val, 1, 16);
        SSManager.SetMaxAnisotropy(val);
    }

    CCC_tf_Aniso(LPCSTR N, int* v) : CCC_Integer(N, v, 1, 16){};

    virtual void Execute(LPCSTR args)
    {
        CCC_Integer::Execute(args);
        apply();
    }
    virtual void Status(TStatus& S)
    {
        CCC_Integer::Status(S);
        apply();
    }
};
class CCC_tf_MipBias : public CCC_Float
{
public:
    void apply()
    {
        if (nullptr == HW.pDevice)
            return;

        SSManager.SetMipLODBias(*value);
    }

    CCC_tf_MipBias(LPCSTR N, float* v) : CCC_Float(N, v, -3.f, +3.f){};
    virtual void Execute(LPCSTR args)
    {
        CCC_Float::Execute(args);
        apply();
    }
    virtual void Status(TStatus& S)
    {
        CCC_Float::Status(S);
        apply();
    }
};
class CCC_R2GM : public CCC_Float
{
public:
    CCC_R2GM(LPCSTR N, float* v) : CCC_Float(N, v, 0.f, 4.f) { *v = 0; };
    virtual void Execute(LPCSTR args)
    {
        if (0 == xr_strcmp(args, "on"))
        {
            ps_r2_ls_flags.set(R2FLAG_GLOBALMATERIAL, TRUE);
        }
        else if (0 == xr_strcmp(args, "off"))
        {
            ps_r2_ls_flags.set(R2FLAG_GLOBALMATERIAL, FALSE);
        }
        else
        {
            CCC_Float::Execute(args);
            if (ps_r2_ls_flags.test(R2FLAG_GLOBALMATERIAL))
            {
                constexpr const char* name[4] = {"oren", "blin", "phong", "metal"};
                float mid = *value;
                int m0 = iFloor(mid) % 4;
                int m1 = (m0 + 1) % 4;
                float frc = mid - float(iFloor(mid));
                Msg("* material set to [%s]-[%s], with lerp of [%f]", name[m0], name[m1], frc);
            }
        }
    }
};

class CCC_ModelPoolStat : public IConsole_Command
{
public:
    CCC_ModelPoolStat(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR args) { RImplementation.Models->dump(); }
};

class CCC_Preset : public CCC_Token
{
public:
    CCC_Preset(LPCSTR N, u32* V, const xr_token* T) : CCC_Token(N, V, T){};

    virtual void Execute(LPCSTR args)
    {
        CCC_Token::Execute(args);
        string_path _cfg;
        string_path cmd;

        switch (*value)
        {
        case 0: xr_strcpy(_cfg, "rspec_minimum.ltx"); break;
        case 1: xr_strcpy(_cfg, "rspec_low.ltx"); break;
        case 2: xr_strcpy(_cfg, "rspec_default.ltx"); break;
        case 3: xr_strcpy(_cfg, "rspec_high.ltx"); break;
        case 4: xr_strcpy(_cfg, "rspec_extreme.ltx"); break;
        }
        FS.update_path(_cfg, fsgame::game_configs, _cfg);
        strconcat(sizeof(cmd), cmd, "cfg_load", " ", _cfg);
        Console->Execute(cmd);
    }
};

class CCC_VideoMemoryStats : public IConsole_Command
{
protected:
public:
    CCC_VideoMemoryStats(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = true; };

    virtual void Execute(LPCSTR args)
    {
        Msg("memory usage  mb \t \t video    \t managed      \t system");

        const float vb_video = (float)HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_vertex][D3DPOOL_DEFAULT] / 1024 / 1024;
        const float vb_managed = (float)HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_vertex][D3DPOOL_MANAGED] / 1024 / 1024;
        const float vb_system = (float)HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_vertex][D3DPOOL_SYSTEMMEM] / 1024 / 1024;
        Msg("vertex buffer\t \t %f \t %f \t %f ", vb_video, vb_managed, vb_system);

        const float ib_video = (float)HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_index][D3DPOOL_DEFAULT] / 1024 / 1024;
        const float ib_managed = (float)HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_index][D3DPOOL_MANAGED] / 1024 / 1024;
        const float ib_system = (float)HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_index][D3DPOOL_SYSTEMMEM] / 1024 / 1024;
        Msg("index buffer\t \t %f \t %f \t %f ", ib_video, ib_managed, ib_system);

        const float rt_video = (float)HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_rtarget][D3DPOOL_DEFAULT] / 1024 / 1024;
        const float rt_managed = (float)HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_rtarget][D3DPOOL_MANAGED] / 1024 / 1024;
        const float rt_system = (float)HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_rtarget][D3DPOOL_SYSTEMMEM] / 1024 / 1024;
        Msg("rtarget\t \t %f \t %f \t %f ", rt_video, rt_managed, rt_system);

        Msg("total\t \t %f \t %f \t %f \n", vb_video + ib_video + rt_video, vb_managed + ib_managed + rt_managed, vb_system + ib_system + rt_system);

        u32 m_base = 0;
        u32 c_base = 0;
        u32 m_lmaps = 0;
        u32 c_lmaps = 0;

        dxRenderDeviceRender::Instance().ResourcesGetMemoryUsage(m_base, c_base, m_lmaps, c_lmaps);
        Msg("textures loaded size %f MB (%f bytes)", (float)(m_base + m_lmaps) / 1024 / 1024, (float)(m_base + m_lmaps));

        HW.DumpVideoMemoryUsage();
    }
};

class CCC_DumpResources : public IConsole_Command
{
public:
    CCC_DumpResources(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR args)
    {
        RImplementation.Models->dump();
        dxRenderDeviceRender::Instance().Resources->Dump(false);
    }
};

class CCC_SunshaftsIntensity : public CCC_Float
{
public:
    CCC_SunshaftsIntensity(LPCSTR N, float* V, float _min, float _max) : CCC_Float(N, V, _min, _max)
    {
        SetCanSave(FALSE);
    }
};

//	Allow real-time fog config reload
#ifdef DEBUG

#include "../xrRenderDX10/3DFluid/dx103DFluidManager.h"

class CCC_Fog_Reload : public IConsole_Command
{
public:
    CCC_Fog_Reload(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR args) { FluidManager.UpdateProfiles(); }
};
#endif //	DEBUG

class CCC_PART_Export : public IConsole_Command
{
public:
    CCC_PART_Export(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };

    virtual void Execute(LPCSTR args)
    {
        if (g_pGameLevel)
        {
            Log("Error: Unload level first!");
            return;
        }

        Msg("Exporting particles...");
        RImplementation.PSLibrary.Reload();
        RImplementation.PSLibrary.Save2(0 == xr_strcmp(args, "1"));
        Msg("Exporting particles Done!");
    }
};

class CCC_PART_Import : public IConsole_Command
{
public:
    CCC_PART_Import(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };

    virtual void Execute(LPCSTR args)
    {
        if (g_pGameLevel)
        {
            Log("Error: Unload level first!");
            return;
        }

        Msg("Importing particles...");
        RImplementation.PSLibrary.OnDestroy();
        RImplementation.PSLibrary.Load2();
        RImplementation.PSLibrary.ExportAllAsNew();
        RImplementation.PSLibrary.OnCreate();
        Msg("Importing particles Done!");
    }
};


class CCC_PART_DumpTextures : public IConsole_Command
{
public:
    CCC_PART_DumpTextures(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };

    virtual void Execute(LPCSTR args)
    {
        Msg("Dump particle effects textures...");
        RImplementation.PSLibrary.DumpTextures();
        Msg("Dump particle effects textures Done!");
    }
};

class CCC_Dbg_DumpStaticVisual : public IConsole_Command
{
public:
    CCC_Dbg_DumpStaticVisual(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };

    virtual void Execute(LPCSTR args)
    {
        if (!g_pGameLevel)
        {
            Log("Error: Load level first!");
            return;
        }

        const float dist = 10;

        const Fvector pos = Device.vCameraPosition;
        const Fvector dir = Device.vCameraDirection;

        xr_vector<std::pair<dxRender_Visual*, float>> list;

        for (u32 i = 0; i < RImplementation.getVisualCount();i++)
        {
            IRenderVisual* vis = RImplementation.getVisual(i);
            if (vis)
            {
                if (vis->getType() == MT_NORMAL)
                {
                    Fsphere& sphere = vis->getVisData().sphere;

                    Fvector C = sphere.P;
                    const float distSQ = Device.vCameraPosition.distance_to_sqr(C) + EPS;

                    if (distSQ <= dist * dist) // 10 meters
                    {
                        float dist2 = dist;
                        const Fsphere::ERP_Result result = sphere.intersect(pos, dir, dist2); // вот эта проверка часто косячит для большиъ объектов. надо как то сторону взгаляда по другому отсекать
                        if (result != Fsphere::rpNone)
                        {
                            /*if (b_nearest)
                            {
                                switch (result)
                                {
                                case Fsphere::rpOriginInside: range = dist < range ? dist : range; break;
                                case Fsphere::rpOriginOutside: range = dist; break;
                                }
                                range2 = range * range;
                            }*/

                            dxRender_Visual* dx_vis = smart_cast<dxRender_Visual*>(vis);

                            list.emplace_back(dx_vis, _sqrt(distSQ));

                            //break;
                        }
                    }
                }
            }
        }

        std::sort(list.begin(), list.end(), [](const auto& a, const auto& b) { return a.second < b.second; });

        for (const auto& pair : list)
        {
            const dxRender_Visual* dx_vis = pair.first;
            const float vis_distance = pair.second;

            Msg("! static visual dist=[%f] [%s]", vis_distance, dx_vis->getDebugInfo().c_str());
        }
    }
};

class CCC_OCC_Enable : public CCC_Bool
{
private:
    BOOL v{TRUE};

public:
    CCC_OCC_Enable(LPCSTR N) : CCC_Bool(N, &v) { };

    virtual void Execute(LPCSTR args)
    {
        CCC_Bool::Execute(args);

        RImplementation.occq_enable(v);
    }
};

void xrRender_initconsole()
{
    if (!FS.path_exist(fsgame::game_weathers))
    {
        ps_r2_ls_bloom_threshold = 1.0f;
        ps_r2_gloss_factor = 1.0f;
    }

    CMD3(CCC_Preset, "_preset", &ps_preset, qpreset_token);

    CMD4(CCC_Integer, "rs_skeleton_update", &psSkeletonUpdate, 2, 128);

#ifdef DEBUG
    CMD1(CCC_DumpResources, "dump_resources");
#endif //	 DEBUG

    //CMD4(CCC_Float, "r__dtex_range", &r__dtex_range, 5, 175);

#ifdef DEBUG
    CMD4(CCC_Integer, "r__lsleep_frames", &ps_r__LightSleepFrames, 4, 30);

    CMD4(CCC_Float, "r__wallmark_shift_pp", &ps_r__WallmarkSHIFT, 0.0f, 1.f);
    CMD4(CCC_Float, "r__wallmark_shift_v", &ps_r__WallmarkSHIFT_V, 0.0f, 1.f);
#endif // DEBUG

    CMD1(CCC_ModelPoolStat, "stat_models");

    CMD4(CCC_Float, "r__wallmark_ttl", &ps_r__WallmarkTTL, 1.0f, 10.f * 60.f);

    CMD4(CCC_Float, "r__geometry_lod", &ps_r__LOD, 0.5f, 3.f); // AVO: extended from 1.2f to 3.f
    CMD4(CCC_Float, "r__lod_k", &ps_r__LOD_k, 0.1f, 10.f);

    CMD4(CCC_detail_radius, "r__detail_radius", &ps_r__detail_radius, 70, 300);
    CMD4(CCC_detail_reset, "r__detail_density", &ps_current_detail_density, 0.2f, 0.9f);
    CMD4(CCC_detail_reset, "r__detail_scale", &ps_current_detail_scale, 0.7f, 1.5f);

    CMD4(CCC_Float, "r2_no_details_radius", &ps_r2_no_details_radius, 0.f, 5.f);
    CMD4(CCC_Float, "r2_no_rain_radius", &ps_r2_no_rain_radius, 0.f, 5.f);

    CMD2(CCC_tf_Aniso, "r__tf_aniso", &ps_r__tf_Anisotropic); //	{1..16}
    CMD2(CCC_tf_MipBias, "r__tf_mipbias", &ps_r__tf_Mipbias); // {-3 +3}

    CMD4(CCC_Float, "r2_ssa_lod_a", &ps_r2_ssaLOD_A, 16, 192);
    CMD4(CCC_Float, "r2_ssa_lod_b", &ps_r2_ssaLOD_B, 32, 128);

    CMD4(CCC_Float, "r__ssa_glod_start", &ps_r__GLOD_ssa_start, 128, 1024);
    CMD4(CCC_Float, "r__ssa_glod_end", &ps_r__GLOD_ssa_end, 16, 256);

    CMD4(CCC_Float, "r2_ssa_discard", &ps_r__ssaDISCARD, 0.5f, 10);

    CMD2(CCC_R2GM, "r2em", &ps_r2_gmaterial);

    CMD3(CCC_Mask64, "r2_tonemap", &ps_r2_ls_flags, R2FLAG_TONEMAP);
    CMD4(CCC_Float, "r2_tonemap_middlegray", &ps_r2_tonemap_middlegray, 0.0f, 2.0f);
    CMD4(CCC_Float, "r2_tonemap_adaptation", &ps_r2_tonemap_adaptation, 0.01f, 10.0f);
    CMD4(CCC_Float, "r2_tonemap_lowlum", &ps_r2_tonemap_low_lum, 0.0001f, 1.0f);
    CMD4(CCC_Float, "r2_tonemap_amount", &ps_r2_tonemap_amount, 0.0000f, 1.0f);

    CMD4(CCC_Float, "r2_ls_bloom_kernel_scale", &ps_r2_ls_bloom_kernel_scale, 0.5f, 2.f);
    CMD4(CCC_Float, "r2_ls_bloom_kernel_g", &ps_r2_ls_bloom_kernel_g, 1.f, 7.f);
    CMD4(CCC_Float, "r2_ls_bloom_kernel_b", &ps_r2_ls_bloom_kernel_b, 0.01f, 1.f);
    CMD4(CCC_Float, "r2_ls_bloom_threshold", &ps_r2_ls_bloom_threshold, 0.f, 1.f);
    CMD4(CCC_Float, "r2_ls_bloom_speed", &ps_r2_ls_bloom_speed, 0.f, 100.f);
    CMD3(CCC_Mask64, "r2_ls_bloom_fast", &ps_r2_ls_flags, R2FLAG_FASTBLOOM);

    CMD4(CCC_Float, "r2_ls_dsm_kernel", &ps_r2_ls_dsm_kernel, .1f, 3.f);
    CMD4(CCC_Float, "r2_ls_psm_kernel", &ps_r2_ls_psm_kernel, .1f, 3.f);
    CMD4(CCC_Float, "r2_ls_ssm_kernel", &ps_r2_ls_ssm_kernel, .1f, 3.f);
    CMD4(CCC_Float, "r2_ls_squality", &ps_r2_ls_squality, .5f, 3.f);

    //- Mad Max
    CMD4(CCC_Float, "r2_gloss_factor", &ps_r2_gloss_factor, .0f, 10.f);
    //- Mad Max

    // CMD3(CCC_Mask, "r_taa_jitter_enable", &ps_r2_ls_flags, R2FLAG_DBG_TAA_JITTER_ENABLE);
#pragma todo("Simp: поискать в интернетах менее лагающую реализацию, эта ужасна")
    // CMD3(CCC_Mask, "r_hat", &ps_r2_ls_flags, R2FLAG_HAT);

    CMD3(CCC_Mask64, "r2_disable_hom", &ps_r2_ls_flags_ext, R2FLAGEXT_DISABLE_HOM);
    CMD3(CCC_Mask64, "r2_disable_particles", &ps_r2_ls_flags_ext, R2FLAGEXT_DISABLE_PARTICLES);
    CMD3(CCC_Mask64, "r2_disable_dynamic", &ps_r2_ls_flags_ext, R2FLAGEXT_DISABLE_DYNAMIC);
    CMD3(CCC_Mask64, "r2_disable_light", &ps_r2_ls_flags_ext, R2FLAGEXT_DISABLE_LIGHT);
    CMD3(CCC_Mask64, "r2_disable_smapvis", &ps_r2_ls_flags_ext, R2FLAGEXT_DISABLE_SMAPVIS);
    CMD3(CCC_Mask64, "r2_disable_sectors", &ps_r2_ls_flags_ext, R2FLAGEXT_DISABLE_SECTORS);

    CMD3(CCC_Mask64, "r2_disable_static_normal", &ps_r2_ls_flags_ext, R2FLAGEXT_DISABLE_STATIC_NORMAL);
    CMD3(CCC_Mask64, "r2_disable_static_lod", &ps_r2_ls_flags_ext, R2FLAGEXT_DISABLE_STATIC_LOD);
    CMD3(CCC_Mask64, "r2_disable_static_progressive", &ps_r2_ls_flags_ext, R2FLAGEXT_DISABLE_STATIC_PROGRESSIVE);
    CMD3(CCC_Mask64, "r2_disable_static_tree", &ps_r2_ls_flags_ext, R2FLAGEXT_DISABLE_STATIC_TREE);
    CMD3(CCC_Mask64, "r2_disable_static_tree_progressive", &ps_r2_ls_flags_ext, R2FLAGEXT_DISABLE_STATIC_TREE_PROGRESSIVE);

    // CMD3(CCC_Mask64, "r2_render_on_prefetch", &ps_r2_ls_flags_ext, R2FLAGEXT_RENDER_ON_PREFETCH);

    CMD3(CCC_Mask64, "r2_rain_drops", &ps_r2_ls_flags_ext, R2FLAGEXT_RAIN_DROPS);
    CMD3(CCC_Mask64, "r2_rain_drops_control", &ps_r2_ls_flags_ext, R2FLAGEXT_RAIN_DROPS_CONTROL);
    CMD4(CCC_Float, "r2_rain_drops_intensity", &ps_r2_rain_drops_intensity, 0.f, 0.0001f);
    CMD4(CCC_Float, "r2_rain_drops_speed", &ps_r2_rain_drops_speed, 0.8f, 5.f);

    CMD3(CCC_Mask64, "r2_mask", &ps_r2_ls_flags_ext, R2FLAGEXT_MASK);
    CMD3(CCC_Mask64, "r2_mask_control", &ps_r2_ls_flags_ext, R2FLAGEXT_MASK_CONTROL);

    CMD3(CCC_Mask64, "r_sslr_enable", &ps_r2_ls_flags_ext, R2FLAGEXT_SSLR);

    #pragma todo("Simp: заменить на новый ссс-стайл параллакс с поддержкой луж")
    // CMD3(CCC_Mask, "r_terrain_parallax_enable", &ps_r2_ls_flags_ext, R2FLAGEXT_TERRAIN_PARALLAX);

    CMD3(CCC_Mask64, "r_mt_texload", &ps_r2_ls_flags_ext, R2FLAGEXT_MT_TEXLOAD);

    // Солнце на этом рендере отключать нельзя, могут появиться куча различных графических багов.
    //CMD3(CCC_Mask64, "r2_sun", &ps_r2_ls_flags, R2FLAG_SUN);
    CMD3(CCC_Mask64, "r2_sun_details", &ps_r2_ls_flags, R2FLAG_SUN_DETAILS);

    CMD3(CCC_Mask64, "r2_light_details", &ps_r2_ls_flags, R2FLAG_LIGHT_DETAILS);

    CMD3(CCC_Mask64, "r2_exp_far_no_shadows", &ps_r2_ls_flags, R2FLAG_LIGHT_NO_DIST_SHADOWS);

    CMD4(CCC_Float, "r2_sun_tsm_bias", &ps_r2_sun_tsm_bias, -0.5, +0.5);

    CMD3(CCC_Token, "r__smap_size", &r2_SmapSize, SmapSizeToken);
    CMD4(CCC_Float, "r2_sun_near", &ps_r2_sun_near, 1.f, 100.f /*50.f*/);

    CMD4(CCC_Float, "r2_sun_far", &OLES_SUN_LIMIT_27_01_07, 51.f, 180.f);

    CMD4(CCC_Float, "r2_sun_near_border", &ps_r2_sun_near_border, .5f, 3.0f);
    CMD4(CCC_Float, "r2_sun_depth_far_scale", &ps_r2_sun_depth_far_scale, 0.5, 1.5);
    CMD4(CCC_Float, "r2_sun_depth_far_bias", &ps_r2_sun_depth_far_bias, -0.5, +0.5);
    CMD4(CCC_Float, "r2_sun_depth_near_scale", &ps_r2_sun_depth_near_scale, 0.5, 1.5);
    CMD4(CCC_Float, "r2_sun_depth_near_bias", &ps_r2_sun_depth_near_bias, -0.5, +0.5);
    CMD4(CCC_Float, "r2_sun_lumscale", &ps_r2_sun_lumscale, -1.0, +3.0);
    CMD4(CCC_Float, "r2_sun_lumscale_hemi", &ps_r2_sun_lumscale_hemi, 0.0, +3.0);
    CMD4(CCC_Float, "r2_sun_lumscale_amb", &ps_r2_sun_lumscale_amb, 0.0, +3.0);

    CMD4(CCC_Float, "r2_mblur", &ps_r2_mblur, 0.0f, 1.0f);
    CMD3(CCC_Mask64, "r2_mblur_enable", &ps_r2_ls_flags_ext, R2FLAGEXT_MOTION_BLUR);

    // Shader param stuff
    constexpr Fvector4 tw2_min{-100.f, -100.f, -100.f, -100.f};
    constexpr Fvector4 tw2_max{100.f, 100.f, 100.f, 100.f};

    // CMD4(CCC_Integer, "r2_dhemi_count", &ps_r2_dhemi_count, 4, 25);
    // CMD4(CCC_Float, "r2_dhemi_sky_scale", &ps_r2_dhemi_sky_scale, 0.0f, 100.f);
    // CMD4(CCC_Float, "r2_dhemi_light_scale", &ps_r2_dhemi_light_scale, 0, 100.f);
    // CMD4(CCC_Float, "r2_dhemi_light_flow", &ps_r2_dhemi_light_flow, 0, 1.f);
    // CMD4(CCC_Float, "r2_dhemi_smooth", &ps_r2_lt_smooth, 0.f, 10.f);
    // CMD3(CCC_Mask, "rs_hom_depth_draw", &ps_r2_ls_flags_ext, R2FLAGEXT_HOM_DEPTH_DRAW);

    CMD3(CCC_Mask64, "r2_shadow_cascede_zcul", &ps_r2_ls_flags_ext, R2FLAGEXT_SUN_ZCULLING);
    //CMD3(CCC_Mask64, "r2_shadow_cascede_old", &ps_r2_ls_flags_ext, R2FLAGEXT_SUN_OLD);

    CMD4(CCC_Float, "r2_ls_depth_scale", &ps_r2_ls_depth_scale, 0.5, 1.5);
    CMD4(CCC_Float, "r2_ls_depth_bias", &ps_r2_ls_depth_bias, -0.5, +0.5);

    CMD4(CCC_Float, "r2_parallax_h", &ps_r2_df_parallax_h, .0f, .5f);
    CMD4(CCC_Float, "r2_parallax_range", &ps_r2_df_parallax_range, 5.0f, 175.0f);

    CMD4(CCC_Float, "r2_slight_fade", &ps_r2_slight_fade, .2f, 2.f);

    CMD3(CCC_Mask64, "r_mt_sun", &ps_r2_ls_flags, R2FLAG_EXP_MT_SUN);
    CMD3(CCC_Mask64, "r_mt_rain", &ps_r2_ls_flags, R2FLAG_EXP_MT_RAIN);
    CMD3(CCC_Mask64, "r_mt_particles", &ps_r2_ls_flags, R2FLAG_EXP_MT_PARTICLES);
    CMD3(CCC_Mask64, "r_mt_lights", &ps_r2_ls_flags, R2FLAG_EXP_MT_LIGHTS);
    CMD3(CCC_Mask64, "r_mt_bones", &ps_r2_ls_flags, R2FLAG_EXP_MT_BONES);

    CMD3(CCC_Mask64, "r2_volumetric_lights", &ps_r2_ls_flags, R2FLAG_VOLUMETRIC_LIGHTS);

    // Sunshafts
    CMD3(CCC_Token, "r_sunshafts_mode", &ps_r_sunshafts_mode, sunshafts_mode_token);
    CMD4(CCC_SunshaftsIntensity, "r_sunshafts_intensity", &ps_r_sunshafts_intensity, 0.0f, 5.0f); // Dbg

    CMD4(CCC_Float, "r_ss_sunshafts_length", &ps_r_ss_sunshafts_length, 0.2f, 1.5f);
    CMD4(CCC_Float, "r_ss_sunshafts_radius", &ps_r_ss_sunshafts_radius, 0.5f, 2.f);

    // CMD4(CCC_Float, "r_SunShafts_SampleStep_Phase1", &ps_r_prop_ss_sample_step_phase0, 0.01f, 0.2f);
    // CMD4(CCC_Float, "r_SunShafts_SampleStep_Phase2", &ps_r_prop_ss_sample_step_phase1, 0.01f, 0.2f);
    CMD4(CCC_Float, "r_SunShafts_Radius", &ps_r_prop_ss_radius, 0.5f, 2.0f);
    CMD4(CCC_Float, "r_SunShafts_Blend", &ps_r_prop_ss_blend, 0.01f, 1.0f);

    CMD3(CCC_Token, "r_ao_mode", &ps_r_ao_mode, ao_mode_token);
    CMD3(CCC_Token, "r2_ssao", &ps_r_ao_quality, qssao_token);

    CMD3(CCC_Mask64, "r4_enable_tessellation", &ps_r2_ls_flags_ext, R2FLAGEXT_ENABLE_TESSELLATION); // Need restart

    CMD3(CCC_Mask64, "r4_wireframe", &ps_r2_ls_flags_ext, R2FLAGEXT_WIREFRAME); // Need restart
    CMD3(CCC_Mask64, "r2_steep_parallax", &ps_r2_ls_flags, R2FLAG_STEEP_PARALLAX);
    CMD3(CCC_Mask64, "r2_detail_bump", &ps_r2_ls_flags, R2FLAG_DETAIL_BUMP);

    CMD3(CCC_Token, "r2_sun_quality", &ps_r_sun_quality, qsun_quality_token);

    CMD3(CCC_Mask64, "r2_visor_refl", &ps_r2_ls_flags_ext, R2FLAGEXT_VISOR_REFL);
    CMD3(CCC_Mask64, "r2_visor_refl_control", &ps_r2_ls_flags_ext, R2FLAGEXT_VISOR_REFL_CONTROL);
    CMD4(CCC_Float, "r2_visor_refl_intensity", &ps_r2_visor_refl_intensity, 0.f, 1.f);
    CMD4(CCC_Float, "r2_visor_refl_radius", &ps_r2_visor_refl_radius, 0.3f, 0.6f);

    CMD3(CCC_Token, "r_aa_mode", &ps_r_pp_aa_mode, pp_aa_mode_token);
    //CMD3(CCC_Token, "r_aa_dlss_preset", &ps_r_dlss_preset, dlss_mode_token);
    //CMD3(CCC_Token, "r_3dss_dlss_preset", &ps_r_dlss_3dss_preset, dlss_mode_token);
    //CMD3(CCC_Token, "r_3dss_dlss_quality", &ps_r_dlss_3dss_quality, dlss_quality_token);
    CMD4(CCC_Float, "r_3dss_scale_factor", &ps_r_dlss_3dss_scale_factor, 1.f, 2.5f);
    CMD3(CCC_Mask64, "r_3dss_use_second_pass", &ps_r2_ls_flags_ext, R2FLAGEXT_DLSS_3DSS_USE_SECOND_PASS);

    CMD4(CCC_Integer, "r__no_scale_on_fade", &ps_no_scale_on_fade, 0, 1); // Alundaio

    //	Allow real-time fog config reload
#ifdef DEBUG
    CMD1(CCC_Fog_Reload, "r3_fog_reload");
#endif //	DEBUG

    CMD3(CCC_Mask64, "r3_dynamic_wet_surfaces", &ps_r2_ls_flags, R3FLAG_DYN_WET_SURF);
    CMD4(CCC_Integer, "r3_dynamic_wet_surfaces_sm_res", &ps_r3_dyn_wet_surf_sm_res, 64, 2048);
    //CMD4(CCC_Integer, "r3_dynamic_wet_surfaces_enable_streaks", &ps_r3_dyn_wet_surf_enable_streaks, 0, 1); //Устарело

    //CMD4(CCC_Integer, "r3_dynamic_wet_surfaces_opt", &ps_r3_dyn_wet_surf_opt, 0, 1);
    CMD4(CCC_Float, "r3_dynamic_wet_surfaces_near", &ps_r3_dyn_wet_surf_near, 5, 70);
    CMD4(CCC_Float, "r3_dynamic_wet_surfaces_far", &ps_r3_dyn_wet_surf_far, 30, 279);

    CMD3(CCC_Mask64, "r3_volumetric_smoke", &ps_r2_ls_flags, R3FLAG_VOLUMETRIC_SMOKE);

    CMD1(CCC_VideoMemoryStats, "video_memory_stats");

    CMD4(CCC_Integer, "r_pnv_mode", &ps_pnv_mode, 0, 3);

    CMD4(CCC_Float, "r_pnv_noise", &ps_pnv_noise, 0.f, 1.f);
    CMD4(CCC_Float, "r_pnv_scanlines", &ps_pnv_scanlines, 0.f, 5.f);
    CMD4(CCC_Float, "r_pnv_scintillation", &ps_pnv_scintillation, 0.f, 1.f);

    CMD4(CCC_Float, "r_pnv_position", &ps_pnv_position, 0.f, 100.f);
    CMD4(CCC_Float, "r_pnv_radius", &ps_pnv_radius, 0.f, 1.f);

    // A - теперь не надо, параметры можно менять отдельно
    //CMD4(CCC_Float, "r_pnv_generation", &ps_pnv_params_1, 0.f, 100.f);
    // A - яркость lua_param_nvg_gain_current - по сути сила ПНВ
    CMD4(CCC_Float, "r_pnv_gain_current", &ps_pnv_params_2, 0.1f, 3.f);
    // A - размер виньетки
    CMD4(CCC_Float, "r_pnv_size_vignet", &ps_pnv_params_3, 0.f, 1.f);
    // A - lua_param_nvg_gain_offset
    CMD4(CCC_Float, "r_pnv_gain_offset", &ps_pnv_params_4, 0.5f, 3.f);

    // B - режим виньетки (чисто трубок) 
    CMD4(CCC_Float, "r_pnv_num_tubes", &ps_pnv_params_1_2, 1.f, 4.f); //  1, 1.1, 1.2, 2, 4
    // B - Порог для размывания источника света
    CMD4(CCC_Float, "r_pnv_washout_thresh", &ps_pnv_params_2_2, 0.1f, 0.9f);
    // B - тряска картинки
    CMD4(CCC_Float, "r_pnv_glitch", &ps_pnv_params_3_2, 0.f, 0.9f);
    // B - прозрачность краев виньетки  0, 1, 2, 3
    CMD4(CCC_Float, "r_pnv_alfa_vignete", &ps_pnv_params_4_2, 0.f, 3.f); // 0 - blur, 1 - black, 2 - image overlay

/*
    //CMD4(CCC_Integer, "r__fakescope", &scope_fake_enabled, 0, 1); // crookr for fake scope
    CMD4(CCC_Float, "fake_scope_radius", &scope_fake_radius, 0, 1); // crookr for fake scope
    CMD4(CCC_Float, "fake_scope_power", &scope_fake_power, 0, 1); // crookr for fake scope
    CMD4(CCC_Float, "fake_scope_interp", &scope_fake_interp, 0, 1); // crookr for fake scope

    constexpr Fvector4 tw_min{};
    constexpr Fvector4 tw_max{10.f, 10.f, 10.f, 10.f};

    CMD4(CCC_Vector4, "fake_scope_params_1", &ps_scope1_params, tw_min, tw_max); // crookr for fake scope
    CMD4(CCC_Vector4, "fake_scope_params_2", &ps_scope2_params, tw_min, tw_max); // crookr for fake scope
    CMD4(CCC_Vector4, "fake_scope_params_3", &ps_scope3_params, tw_min, tw_max); // crookr for fake scope
*/

    // Screen Space Shaders
    CMD4(CCC_Vector3, "ssfx_shadows", &ps_ssfx_shadows, Fvector3().set(128, 1536, 0), Fvector3().set(1536, 4096, 0));
    CMD4(CCC_Vector3, "ssfx_volumetric", &ps_ssfx_volumetric, Fvector3().set(0, 0, 0), Fvector3().set(1.0, 10.0, 1.0));

    CMD4(CCC_Vector3, "ssfx_shadow_bias", &ps_ssfx_shadow_bias, Fvector3().set(0, 0, 0), Fvector3().set(1.0, 1.0, 1.0));
    CMD4(CCC_Vector4, "ssfx_lut", &ps_ssfx_lut, Fvector4().set(0.0, 0.0, 0.0, 0.0), tw2_max);

    CMD4(CCC_Vector4, "ssfx_wind_grass", &ps_ssfx_wind_grass, (Fvector4{}), (Fvector4{20.0f, 5.0f, 5.0f, 5.0f}));
    CMD4(CCC_Vector4, "ssfx_wind_trees", &ps_ssfx_wind_trees, (Fvector4{}), (Fvector4{20.0f, 5.0f, 5.0f, 1.0f}));

    CMD4(CCC_Vector4, "ssfx_florafixes_1", &ps_ssfx_florafixes_1, (Fvector4{}), (Fvector4{1.0f, 1.0f, 1.0f, 1.0f}));
    CMD4(CCC_Vector4, "ssfx_florafixes_2", &ps_ssfx_florafixes_2, (Fvector4{}), (Fvector4{10.0f, 1.0f, 1.0f, 1.0f}));
    
    CMD4(CCC_Vector4, "ssfx_wetsurfaces_1", &ps_ssfx_wetsurfaces_1_cfg, (Fvector4{0.01f, 0.01f, 0.01f, 0.01f}), (Fvector4{2.0f, 2.0f, 2.0f, 2.0f}));
    CMD4(CCC_Vector4, "ssfx_wetsurfaces_2", &ps_ssfx_wetsurfaces_2_cfg, (Fvector4{0.01f, 0.01f, 0.01f, 0.01f}), (Fvector4{2.0f, 2.0f, 2.0f, 2.0f}));
    
    CMD4(CCC_Integer, "ssfx_is_underground", &ps_ssfx_is_underground, 0, 1);

    CMD4(CCC_Integer, "ssfx_gloss_method", &ps_ssfx_gloss_method, 0, 1);
    CMD4(CCC_Vector3, "ssfx_gloss_minmax", &ps_ssfx_gloss_minmax, (Fvector3{}), (Fvector3{1.0, 1.0, 1.0}));

    CMD4(CCC_Vector4, "ssfx_lightsetup_1", &ps_ssfx_lightsetup_1, (Fvector4{}), (Fvector4{1.0f, 1.0f, 1.0f, 1.0f}));

    CMD4(CCC_Vector4, "ssfx_hud_drops_1", &ps_ssfx_hud_drops_1_cfg, (Fvector4{}), (Fvector4{100.f, 100.f, 100.f, 100.f}));
    CMD4(CCC_Vector4, "ssfx_hud_drops_2", &ps_ssfx_hud_drops_2_cfg, (Fvector4{}), (Fvector4{100.f, 100.f, 100.f, 100.f}));

    CMD4(CCC_Vector4, "ssfx_blood_decals", &ps_ssfx_blood_decals, (Fvector4{}), (Fvector4{5.f, 5.f, 0.f, 0.f}));

    CMD4(CCC_Vector4, "ssfx_rain_1", &ps_ssfx_rain_1, (Fvector4{}), (Fvector4{10.f, 5.f, 5.f, 2.f}));
    CMD4(CCC_Vector4, "ssfx_rain_2", &ps_ssfx_rain_2, (Fvector4{}), (Fvector4{1.f, 10.f, 10.f, 10.f}));
    CMD4(CCC_Vector4, "ssfx_rain_3", &ps_ssfx_rain_3, (Fvector4{}), (Fvector4{1.f, 10.f, 10.f, 10.f}));

    CMD4(CCC_Vector4, "ssfx_grass_shadows", &ps_ssfx_grass_shadows, (Fvector4{}), (Fvector4{2.f, 0.f, 100.f, 0.f}));
    CMD4(CCC_Vector3, "ssfx_shadow_cascades", &ps_ssfx_shadow_cascades, (Fvector3{1.0f, 1.0f, 1.0f}), (Fvector3{300.f, 300.f, 300.f}));
    CMD4(CCC_Float, "ssfx_wpn_dof_2", &ps_ssfx_wpn_dof_2, 0, 1);
    CMD4(CCC_Vector4, "ssfx_grass_interactive", &ps_ssfx_grass_interactive, (Fvector4{}), (Fvector4{1.f, static_cast<float>(GRASS_SHADER_DATA_COUNT), 5000.f, 1.f}));
    CMD4(CCC_Vector4, "ssfx_int_grass_params_1", &ps_ssfx_int_grass_params_1, (Fvector4{}), (Fvector4{5.f, 5.f, 5.f, 60.f}));
    CMD4(CCC_Vector4, "ssfx_int_grass_params_2", &ps_ssfx_int_grass_params_2, (Fvector4{}), (Fvector4{5.f, 20.f, 1.f, 5.f}));

    CMD4(CCC_Integer, "ssfx_terrain_grass_align", &ps_ssfx_terrain_grass_align, FALSE, TRUE);
    CMD4(CCC_Float, "ssfx_terrain_grass_slope", &ps_ssfx_terrain_grass_slope, 0.f, 1.f);

    CMD4(CCC_Vector3, "ssfx_color_grading", &ps_r2_img_cg, (Fvector3{}), (Fvector3{1.f, 1.f, 1.f}));

    CMD3(CCC_Mask64, "ssfx_height_fog", &ps_r2_ls_flags, R2FLAG_SSFX_HEIGHT_FOG);
    CMD3(CCC_Mask64, "ssfx_sky_debanding", &ps_r2_ls_flags, R2FLAG_SSFX_SKY_DEBANDING);
    CMD3(CCC_Mask64, "ssfx_indirect_light", &ps_r2_ls_flags, R2FLAG_SSFX_INDIRECT_LIGHT);
 //   CMD3(CCC_Mask64, "ssfx_bloom", &ps_r2_ls_flags, R2FLAG_SSFX_BLOOM);

    CMD3(CCC_Mask64, "ssfx_inter_grass", &ps_r2_ls_flags_ext, R2FLAGEXT_SSFX_INTER_GRASS);
    CMD3(CCC_Mask64, "r_font_shadows", &ps_r2_ls_flags_ext, R2FLAGEXT_FONT_SHADOWS);

    CMD4(CCC_Integer, "ssfx_bloom_use_presets", &ps_ssfx_bloom_use_presets, 0, 1);
    CMD4(CCC_Vector4, "ssfx_bloom_1", &ps_ssfx_bloom_1, (Fvector4{1, 1, 0, 0}), (Fvector4{10, 100, 100, 10}));
    CMD4(CCC_Vector4, "ssfx_bloom_2", &ps_ssfx_bloom_2, (Fvector4{1, 0, 0, 0}), (Fvector4{5, 10, 10, 10}));

    CMD4(CCC_Integer, "ssfx_pom_refine", &ps_ssfx_pom_refine, 0, 1);
    CMD4(CCC_Vector4, "ssfx_pom", &ps_ssfx_pom, Fvector4().set(0, 0, 0, 0), Fvector4().set(36, 60, 1, 1));

    CMD3(CCC_Mask64, "reflections_only_on_terrain", &ps_r2_ls_flags_ext, R2FLAGEXT_REFLECTIONS_ONLY_ON_TERRAIN);
    CMD3(CCC_Mask64, "reflections_only_on_puddles", &ps_r2_ls_flags_ext, R2FLAGEXT_REFLECTIONS_ONLY_ON_PUDDLES);

    CMD4(CCC_Float, "ssfx_exposure", &ps_r2_img_exposure, 0.5f, 1.5f);
    CMD4(CCC_Float, "ssfx_gamma", &ps_r2_img_gamma, 0.5f, 1.5f);
    CMD4(CCC_Float, "ssfx_saturation", &ps_r2_img_saturation, 0.5f, 1.5f);

#pragma todo("Simp: В общем эта настройка работает, но надо убирать мипмапы у текстур ui. Да и заметного влияния на fps я не вижу.")
    //CMD4(CCC_Integer, "texture_lod", &psTextureLOD, 0, 2);

    CMD1(CCC_PART_Export, "particles_export");
    CMD1(CCC_PART_Import, "particles_import");

    CMD1(CCC_PART_DumpTextures, "particles_dump_textures");

    CMD4(CCC_Float, "particle_update_mod", &ps_particle_update_coeff, 0.04f, 10.f);

    CMD3(CCC_Mask64, "r_lens_flare", &ps_r2_ls_flags_ext, R2FLAGEXT_LENS_FLARE);

    CMD1(CCC_Dbg_DumpStaticVisual, "dbg_dump_static_at_look");

    CMD4(CCC_Float, "r__dyn_opt_dist", &ps_r__opt_dist, 100.0f, 1000.0f);

    CMD4(CCC_Float, "r_aa_cas", &ps_r_cas, 0.0f, 1.0f);

    CMD4(CCC_Integer, "exp_optimize_static_geom", &opt_static_geom, 0, 4);
    CMD4(CCC_Integer, "exp_optimize_shadow_geom", &opt_shadow_geom, 0, 1);

    CMD4(CCC_Integer, "r_back_buffer_count", &r_back_buffer_count, 2, 5);

    extern BOOL bShadersXrExport;
    CMD4(CCC_Integer, "shaders_xr_export", &bShadersXrExport, FALSE, TRUE);

    CMD1(CCC_OCC_Enable, "r_occ_enable");
    CMD4(CCC_Integer, "r_occ_delay_invisible_min", &delay_invisible_min, 1, 10);
    CMD4(CCC_Integer, "r_occ_delay_invisible_max", &delay_invisible_max, 1, 10);
}

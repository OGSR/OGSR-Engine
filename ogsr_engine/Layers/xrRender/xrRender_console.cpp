#include "stdafx.h"

#include "xrRender_console.h"
#include "dxRenderDeviceRender.h"

u32 r2_SmapSize = 2048;
constexpr xr_token SmapSizeToken[] = {{"1536x1536", 1536},
                                      {"2048x2048", 2048},
                                      {"2560x2560", 2560},
                                      {"3072x3072", 3072},
                                      {"4096x4096", 4096},
                                      {"6144x6144", 6144},
                                      {"8192x8192", 8192},
                                      //{ "16384x16384", 16384 },
                                      {nullptr, 0}};

u32 ps_r_pp_aa_mode = SMAA;
constexpr xr_token pp_aa_mode_token[] = {
    {"st_opt_off", NO_AA},
    {"st_opt_smaa", SMAA},
    {nullptr, 0},
};

u32 ps_r_sunshafts_mode = SS_SS_MANOWAR;
constexpr xr_token sunshafts_mode_token[] = {{"st_opt_off", SS_OFF}, {"volumetric", SS_VOLUMETRIC}, {"ss_ogse", SS_SS_OGSE}, {"ss_manowar", SS_SS_MANOWAR}, {nullptr, 0}};
// Sunshafts
u32 ps_r_sun_shafts = 3;
float ps_r_ss_sunshafts_length = 0.9f; // 1.0f;
float ps_r_ss_sunshafts_radius = 2.f; // 1.0f;
float ps_r_prop_ss_radius = 1.56f;
float ps_r_prop_ss_blend = 0.25f; // 0.066f;
float ps_r_prop_ss_sample_step_phase0 = 0.09f;
float ps_r_prop_ss_sample_step_phase1 = 0.07f;

float ps_r2_img_exposure = 1.0f;
float ps_r2_img_gamma = 1.0f;
float ps_r2_img_saturation = 1.0f;
Fvector ps_r2_img_cg{0.5f, 0.5f, 0.5f};

u32 ps_Preset = 2;
constexpr xr_token qpreset_token[] = {{"Minimum", 0}, {"Low", 1}, {"Default", 2}, {"High", 3}, {"Extreme", 4}, {0, 0}};

u32 ps_r_ssao = 3;
constexpr xr_token qssao_token[] = {{"st_opt_off", 0},
                                    {"st_opt_low", 1},
                                    {"st_opt_medium", 2},
                                    {"st_opt_high", 3},
                                    {0, 0}};

u32 ps_r_ao_mode = AO_MODE_GTAO;
constexpr xr_token ao_mode_token[] = {{"st_gtao", AO_MODE_GTAO}, {"st_ssdo", AO_MODE_SSDO}, {nullptr, 0}};

u32 ps_r_sun_quality = 1; //	=	0;
constexpr xr_token qsun_quality_token[] = {{"st_opt_low", 0},
                                           {"st_opt_medium", 1},
                                           {"st_opt_high", 2},
                                           {"st_opt_ultra", 3},
                                           {"st_opt_extreme", 4},

                                           {0, 0}};

u32 ps_r3_msaa = 0; //	=	0;
constexpr xr_token qmsaa_token[] = {{"st_opt_off", 0}, {"2x", 1}, {"4x", 2}, {"8x", 3}, {0, 0}};

u32 ps_r3_msaa_atest = 2;
constexpr xr_token qmsaa__atest_token[] = {{"st_opt_off", 0}, {"st_opt_atest_msaa_dx10_0", 1}, {"st_opt_atest_msaa_dx10_1", 2}, {0, 0}};

u32 ps_r3_minmax_sm = 0;
constexpr xr_token qminmax_sm_token[] = {{"off", 0}, {"on", 1}, {"auto", 2}, {"autodetect", 3}, {0, 0}};

//	“Off”
//	“DX10.0 style [Standard]”
//	“DX10.1 style [Higher quality]”

// Common
extern int psSkeletonUpdate;
extern float r__dtex_range;


int ps_r__LightSleepFrames = 10;

float ps_r__Detail_l_ambient = 0.9f;
float ps_r__Detail_l_aniso = 0.25f;
float ps_r__Detail_density = 0.3f;
float ps_r__Detail_rainbow_hemi = 0.75f;

float ps_r__Tree_w_rot = 10.0f;
float ps_r__Tree_w_speed = 1.00f;
float ps_r__Tree_w_amp = 0.005f;
Fvector ps_r__Tree_Wave = {.1f, .01f, .11f};
float ps_r__Tree_SBC = 1.5f; // scale bias correct

float ps_r__WallmarkTTL = 50.f;
float ps_r__WallmarkSHIFT = 0.0001f;
float ps_r__WallmarkSHIFT_V = 0.0001f;

float ps_r__GLOD_ssa_start = 256.f;
float ps_r__GLOD_ssa_end = 64.f;
float ps_r__LOD = 1.2f;
//. float		ps_r__LOD_Power				=  1.5f	;
float ps_r__ssaDISCARD = 3.5f; // RO
float ps_r__ssaDONTSORT = 32.f; // RO
float ps_r__ssaHZBvsTEX = 96.f; // RO

int ps_r__tf_Anisotropic = 16;
float ps_r__tf_Mipbias = -0.5f;

// R1
float ps_r1_ssaLOD_A = 64.f;
float ps_r1_ssaLOD_B = 48.f;
Flags32 ps_r1_flags = {R1FLAG_DLIGHTS}; // r1-only
float ps_r1_lmodel_lerp = 0.1f;
float ps_r1_dlights_clip = 40.f;
float ps_r1_pps_u = 0.f;
float ps_r1_pps_v = 0.f;

// R1-specific
int ps_r1_GlowsPerFrame = 16; // r1-only
float ps_r1_fog_luminance = 1.1f; // r1-only
int ps_r1_SoftwareSkinning = 0; // r1-only

// R2
float ps_r2_ssaLOD_A = 64.f;
float ps_r2_ssaLOD_B = 48.f;

// R2-specific
Flags32 ps_r2_ls_flags = {R2FLAG_SUN
                          //| R2FLAG_SUN_IGNORE_PORTALS
                          | R2FLAG_EXP_DONT_TEST_UNSHADOWED | R2FLAG_USE_NVSTENCIL | R2FLAG_EXP_SPLIT_SCENE | R2FLAG_EXP_MT_CALC | R3FLAG_DYN_WET_SURF |
                          R3FLAG_VOLUMETRIC_SMOKE
                          //| R3FLAG_MSAA
                          | R3FLAG_MSAA_OPT | R3FLAG_GBUFFER_OPT |  R2FLAG_STEEP_PARALLAX | R2FLAG_SUN_FOCUS | R2FLAG_SUN_TSM | R2FLAG_TONEMAP | R2FLAG_VOLUMETRIC_LIGHTS |
                          R2FLAG_EXP_MT_DETAILS | R2FLAG_EXP_MT_RAIN};

Flags32 ps_r2_ls_flags_ext = {R2FLAGEXT_ENABLE_TESSELLATION | R2FLAGEXT_RAIN_DROPS | R2FLAGEXT_RAIN_DROPS_CONTROL | R2FLAGEXT_SSLR | SSFX_HEIGHT_FOG | SSFX_INTER_GRASS};

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
int ps_r2_GI_depth = 1; // 1..5
int ps_r2_GI_photons = 16; // 8..64
float ps_r2_GI_clip = EPS_L; // EPS
float ps_r2_GI_refl = .9f; // .9f
float ps_r2_ls_depth_scale = 1.00001f; // 1.00001f

float ps_r2_ls_depth_bias = -0.00005f; // SSS19 Edited
float ps_r2_ls_squality = 1.0f; // Base shadow map quality. The higher the value the higher the base shadow resolution and resolution through distance.

float ps_r2_sun_tsm_projection = 0.3f; // 0.18f
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
float ps_r2_zfill = 0.25f; // .1f

float ps_r2_dhemi_sky_scale = 0.08f; // 1.5f
float ps_r2_dhemi_light_scale = 0.2f;
float ps_r2_dhemi_light_flow = 0.1f;
int ps_r2_dhemi_count = 5; // 5
int ps_r2_wait_sleep = 0;
int ps_lens_flare{};

float ps_r2_lt_smooth = 1.f; // 1.f
float ps_r2_slight_fade = 2.0f; // 1.f

// Screen Space Shaders Stuff
Fvector4 ps_ssfx_wind_grass{9.5f, 1.4f, 1.5f, 0.4f}; // Anim Speed, Turbulence, Push, Wave
Fvector4 ps_ssfx_wind_trees{11.0f, 0.15f, 0.5f, 0.1f}; // Branches Speed, Trunk Speed, Bending, Min Wind Speed
int ps_ssfx_wind_bugged_flora_enable{1}; //Включать ли ветер на багованых елках, камышах олд тч-стайл. Оставлено как костыль для OGSR GA потому что там локи уже правиться не будут, а в новых модах лучше править саму флору, и не использовать этот костыль.

Fvector4 ps_ssfx_florafixes_1{0.3f, 0.21f, 0.3f, 0.21f}; // Specular value when the grass is dry, Specular value when the grass is wet, Specular when trees and bushes are dry, Specular when trees and bushes are wet
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
Fvector4 ps_ssfx_grass_shadows = {1.0f, .25f, 20.0f, .0f};
Fvector4 ps_ssfx_grass_interactive{1.f, static_cast<float>(GRASS_SHADER_DATA_COUNT), 2000.f, 1.0f};
Fvector3 ps_ssfx_int_grass_params_1{2.0f, 1.0f, 1.0f};
Fvector4 ps_ssfx_int_grass_params_2{1.0f, 5.0f, 1.0f, 1.0f};
float ps_ssfx_wpn_dof_2 = 0.5f;

Fvector4 ps_ssfx_lut{}; //x - интенсивность, y - номер эффекта
Fvector3 ps_ssfx_shadows{256.f, 1536.f, 0.0f}; // x - Minimum shadow map resolution. When lights are away from the player the resolution of shadows drop to improve performance ( at the cost of image quality ), y - Maximum shadow map resolution. When lights are closer, the resolution increases to improve the image quality of shadows ( at the cost of performance ).
Fvector3 ps_ssfx_shadow_bias{0.4f, 0.03f, 0.0f};

Fvector3 ps_ssfx_volumetric_limits{100.f, 1.0f, 5.f}; //{1.f, 0.1f, 1.5f} //Ограничения для настроек объемного света. Настройки источников света будут ограничиваться сверху до этих значений. volumetric_distance, volumetric_intensity, volumetric_quality.
int ps_ssfx_use_new_volumetric_method{0};

//	x - min (0), y - focus (1.4), z - max (100)
Fvector3 ps_r2_dof = Fvector3().set(-1.25f, 1.4f, 600.f);
float ps_r2_dof_sky = 30; //	distance to sky
float ps_r2_dof_kernel_size = 5.0f; //	7.0f

int ps_r3_dyn_wet_surf_opt = 1;
float ps_r3_dyn_wet_surf_near = 5.f; // 10.0f
float ps_r3_dyn_wet_surf_far = 100.f; // 30.0f

int ps_r3_dyn_wet_surf_sm_res = 1024; // 256
int ps_r3_dyn_wet_surf_enable_streaks = 0;

float ps_r2_rain_drops_intensity = 0.00003f;
float ps_r2_rain_drops_speed = 1.25f;

float ps_r2_visor_refl_intensity = 0.39f;
float ps_r2_visor_refl_radius = 0.4f;

int ps_r__detail_radius = 49;
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

float ps_r2_gloss_factor = 4.0f;

// textures
int psTextureLOD = 0;

u32 psCurrentBPP = 32;

#include "../../xr_3da/xr_ioconsole.h"
#include "../../xr_3da/xr_ioc_cmd.h"

#include "../xrRenderDX10/StateManager/dx10SamplerStateCache.h"

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

class CCC_tf_Aniso : public CCC_Integer
{
public:
    void apply()
    {
        if (0 == HW.pDevice)
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
        if (0 == HW.pDevice)
            return;

        //	TODO: DX10: Implement mip bias control
        // VERIFY(!"apply not implmemented.");
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
                static LPCSTR name[4] = {"oren", "blin", "phong", "metal"};
                float mid = *value;
                int m0 = iFloor(mid) % 4;
                int m1 = (m0 + 1) % 4;
                float frc = mid - float(iFloor(mid));
                Msg("* material set to [%s]-[%s], with lerp of [%f]", name[m0], name[m1], frc);
            }
        }
    }
};
class CCC_Screenshot : public IConsole_Command
{
public:
    CCC_Screenshot(LPCSTR N) : IConsole_Command(N){};
    virtual void Execute(LPCSTR args)
    {
        string_path name;
        name[0] = 0;
        sscanf(args, "%s", name);
        LPCSTR image = xr_strlen(name) ? name : 0;
        ::Render->Screenshot(IRender_interface::SM_NORMAL, image);
    }
};

class CCC_RestoreQuadIBData : public IConsole_Command
{
public:
    CCC_RestoreQuadIBData(LPCSTR N) : IConsole_Command(N){};
    virtual void Execute(LPCSTR args) { RCache.RestoreQuadIBData(); }
};

class CCC_ModelPoolStat : public IConsole_Command
{
public:
    CCC_ModelPoolStat(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR args) { RImplementation.Models->dump(); }
};

//-----------------------------------------------------------------------
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
        FS.update_path(_cfg, "$game_config$", _cfg);
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

        float vb_video = (float)HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_vertex][D3DPOOL_DEFAULT] / 1024 / 1024;
        float vb_managed = (float)HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_vertex][D3DPOOL_MANAGED] / 1024 / 1024;
        float vb_system = (float)HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_vertex][D3DPOOL_SYSTEMMEM] / 1024 / 1024;
        Msg("vertex buffer\t \t %f \t %f \t %f ", vb_video, vb_managed, vb_system);

        float ib_video = (float)HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_index][D3DPOOL_DEFAULT] / 1024 / 1024;
        float ib_managed = (float)HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_index][D3DPOOL_MANAGED] / 1024 / 1024;
        float ib_system = (float)HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_index][D3DPOOL_SYSTEMMEM] / 1024 / 1024;
        Msg("index buffer\t \t %f \t %f \t %f ", ib_video, ib_managed, ib_system);

        float rt_video = (float)HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_rtarget][D3DPOOL_DEFAULT] / 1024 / 1024;
        float rt_managed = (float)HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_rtarget][D3DPOOL_MANAGED] / 1024 / 1024;
        float rt_system = (float)HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_rtarget][D3DPOOL_SYSTEMMEM] / 1024 / 1024;
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
    CCC_SunshaftsIntensity(LPCSTR N, float* V, float _min, float _max) : CCC_Float(N, V, _min, _max) {}
    virtual void Save(IWriter*) {}
};

//	Allow real-time fog config reload
#if (RENDER == R_R4)
#ifdef DEBUG

#include "../xrRenderDX10/3DFluid/dx103DFluidManager.h"

class CCC_Fog_Reload : public IConsole_Command
{
public:
    CCC_Fog_Reload(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR args) { FluidManager.UpdateProfiles(); }
};
#endif //	DEBUG
#endif //	(RENDER == R_R4)

class CCC_ssfx_cascades final: public CCC_Vector3
{
    void apply() { RImplementation.init_cacades(); }

public:
    CCC_ssfx_cascades(LPCSTR N, Fvector3* V, const Fvector3 _min, const Fvector3 _max) : CCC_Vector3(N, V, _min, _max) {}
    void Execute(LPCSTR args) override
    {
        CCC_Vector3::Execute(args);
        apply();
    }
    void Status(TStatus& S) override
    {
        CCC_Vector3::Status(S);
        apply();
    }
};

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


void xrRender_initconsole()
{
    if (!FS.path_exist("$game_weathers$"))
    {
        ps_r2_ls_bloom_threshold = 1.0f;
        ps_r2_gloss_factor = 1.0f;
    }

    CMD3(CCC_Preset, "_preset", &ps_Preset, qpreset_token);

    CMD4(CCC_Integer, "rs_skeleton_update", &psSkeletonUpdate, 2, 128);
#ifdef DEBUG
    CMD1(CCC_DumpResources, "dump_resources");
#endif //	 DEBUG

    CMD4(CCC_Float, "r__dtex_range", &r__dtex_range, 5, 175);

    // Common
    CMD1(CCC_Screenshot, "screenshot");

    //	Igor: just to test bug with rain/particles corruption
    CMD1(CCC_RestoreQuadIBData, "r_restore_quad_ib_data");
#ifdef DEBUG

    CMD1(CCC_BuildSSA, "build_ssa");

    CMD4(CCC_Integer, "r__lsleep_frames", &ps_r__LightSleepFrames, 4, 30);
    CMD4(CCC_Float, "r__ssa_glod_start", &ps_r__GLOD_ssa_start, 128, 512);
    CMD4(CCC_Float, "r__ssa_glod_end", &ps_r__GLOD_ssa_end, 16, 96);
    CMD4(CCC_Float, "r__wallmark_shift_pp", &ps_r__WallmarkSHIFT, 0.0f, 1.f);
    CMD4(CCC_Float, "r__wallmark_shift_v", &ps_r__WallmarkSHIFT_V, 0.0f, 1.f);
    CMD1(CCC_ModelPoolStat, "stat_models");
    CMD3(CCC_Token, "vid_bpp", &psCurrentBPP, vid_bpp_token);
#endif // DEBUG
    CMD4(CCC_Float, "r__wallmark_ttl", &ps_r__WallmarkTTL, 1.0f, 10.f * 60.f);

    CMD4(CCC_Float, "r__geometry_lod", &ps_r__LOD, 1.f, 3.f); // AVO: extended from 1.2f to 3.f
    //.	CMD4(CCC_Float,		"r__geometry_lod_pow",	&ps_r__LOD_Power,			0,		2		);

    CMD4(CCC_Float, "r__detail_density", &ps_current_detail_density, 0.06f /*0.2f*/, 1.0f);
    CMD4(CCC_Float, "r__detail_scale", &ps_current_detail_scale, 0.2f, 3.0f);

#ifdef DEBUG
    CMD4(CCC_Float, "r__detail_l_ambient", &ps_r__Detail_l_ambient, .5f, .95f);
    CMD4(CCC_Float, "r__detail_l_aniso", &ps_r__Detail_l_aniso, .1f, .5f);

    CMD4(CCC_Float, "r__d_tree_w_amp", &ps_r__Tree_w_amp, .001f, 1.f);
    CMD4(CCC_Float, "r__d_tree_w_rot", &ps_r__Tree_w_rot, .01f, 100.f);
    CMD4(CCC_Float, "r__d_tree_w_speed", &ps_r__Tree_w_speed, 1.0f, 10.f);

    tw_min.set(EPS, EPS, EPS);
    tw_max.set(2, 2, 2);
    CMD4(CCC_Vector3, "r__d_tree_wave", &ps_r__Tree_Wave, tw_min, tw_max);
#endif // DEBUG

    CMD2(CCC_tf_Aniso, "r__tf_aniso", &ps_r__tf_Anisotropic); //	{1..16}
    CMD2(CCC_tf_MipBias, "r__tf_mipbias", &ps_r__tf_Mipbias); // {-3 +3}

    CMD3(CCC_Mask, "r__actor_shadow", &ps_r2_ls_flags_ext, R2FLAGEXT_ACTOR_SHADOW);


    CMD4(CCC_Float, "r1_pps_u", &ps_r1_pps_u, -1.f, +1.f);
    CMD4(CCC_Float, "r1_pps_v", &ps_r1_pps_v, -1.f, +1.f);

    // R2
    CMD4(CCC_Float, "r2_ssa_lod_a", &ps_r2_ssaLOD_A, 16, 96);
    CMD4(CCC_Float, "r2_ssa_lod_b", &ps_r2_ssaLOD_B, 32, 64);

    // R2-specific
    CMD2(CCC_R2GM, "r2em", &ps_r2_gmaterial);
    CMD3(CCC_Mask, "r2_tonemap", &ps_r2_ls_flags, R2FLAG_TONEMAP);
    CMD4(CCC_Float, "r2_tonemap_middlegray", &ps_r2_tonemap_middlegray, 0.0f, 2.0f);
    CMD4(CCC_Float, "r2_tonemap_adaptation", &ps_r2_tonemap_adaptation, 0.01f, 10.0f);
    CMD4(CCC_Float, "r2_tonemap_lowlum", &ps_r2_tonemap_low_lum, 0.0001f, 1.0f);
    CMD4(CCC_Float, "r2_tonemap_amount", &ps_r2_tonemap_amount, 0.0000f, 1.0f);
    CMD4(CCC_Float, "r2_ls_bloom_kernel_scale", &ps_r2_ls_bloom_kernel_scale, 0.5f, 2.f);
    CMD4(CCC_Float, "r2_ls_bloom_kernel_g", &ps_r2_ls_bloom_kernel_g, 1.f, 7.f);
    CMD4(CCC_Float, "r2_ls_bloom_kernel_b", &ps_r2_ls_bloom_kernel_b, 0.01f, 1.f);
    CMD4(CCC_Float, "r2_ls_bloom_threshold", &ps_r2_ls_bloom_threshold, 0.f, 1.f);
    CMD4(CCC_Float, "r2_ls_bloom_speed", &ps_r2_ls_bloom_speed, 0.f, 100.f);
    CMD3(CCC_Mask, "r2_ls_bloom_fast", &ps_r2_ls_flags, R2FLAG_FASTBLOOM);
    CMD4(CCC_Float, "r2_ls_dsm_kernel", &ps_r2_ls_dsm_kernel, .1f, 3.f);
    CMD4(CCC_Float, "r2_ls_psm_kernel", &ps_r2_ls_psm_kernel, .1f, 3.f);
    CMD4(CCC_Float, "r2_ls_ssm_kernel", &ps_r2_ls_ssm_kernel, .1f, 3.f);
    CMD4(CCC_Float, "r2_ls_squality", &ps_r2_ls_squality, .5f, 3.f);

    CMD3(CCC_Mask, "r2_zfill", &ps_r2_ls_flags, R2FLAG_ZFILL);
    CMD4(CCC_Float, "r2_zfill_depth", &ps_r2_zfill, .001f, .5f);
    CMD3(CCC_Mask, "r2_allow_r1_lights", &ps_r2_ls_flags, R2FLAG_R1LIGHTS);

    //- Mad Max
    CMD4(CCC_Float, "r2_gloss_factor", &ps_r2_gloss_factor, .0f, 10.f);
    //- Mad Max

#ifdef DEBUG
    CMD3(CCC_Mask, "r2_use_nvdbt", &ps_r2_ls_flags, R2FLAG_USE_NVDBT);
    CMD3(CCC_Mask, "r2_mt", &ps_r2_ls_flags, R2FLAG_EXP_MT_CALC);
#endif // DEBUG

    CMD3(CCC_Mask, "r2_disable_hom", &ps_r2_ls_flags_ext, R2FLAGEXT_DISABLE_HOM);

    CMD3(CCC_Mask, "r2_rain_drops", &ps_r2_ls_flags_ext, R2FLAGEXT_RAIN_DROPS);
    CMD3(CCC_Mask, "r2_rain_drops_control", &ps_r2_ls_flags_ext, R2FLAGEXT_RAIN_DROPS_CONTROL);
    CMD4(CCC_Float, "r2_rain_drops_intensity", &ps_r2_rain_drops_intensity, 0.f, 0.0001f);
    CMD4(CCC_Float, "r2_rain_drops_speed", &ps_r2_rain_drops_speed, 0.8f, 5.f);

#if RENDER == R_R4
    CMD3(CCC_Mask, "r_sslr_enable", &ps_r2_ls_flags_ext, R2FLAGEXT_SSLR);
#endif

    CMD3(CCC_Mask, "r_terrain_parallax_enable", &ps_r2_ls_flags_ext, R2FLAGEXT_TERRAIN_PARALLAX);
    CMD3(CCC_Mask, "r_mt_texload", &ps_r2_ls_flags_ext, R2FLAGEXT_MT_TEXLOAD);

    CMD3(CCC_Mask, "r2_sun", &ps_r2_ls_flags, R2FLAG_SUN);
    CMD3(CCC_Mask, "r2_sun_details", &ps_r2_ls_flags, R2FLAG_SUN_DETAILS);
    CMD3(CCC_Mask, "r2_sun_focus", &ps_r2_ls_flags, R2FLAG_SUN_FOCUS);
    //	CMD3(CCC_Mask,		"r2_sun_static",		&ps_r2_ls_flags,			R2FLAG_SUN_STATIC);
    //	CMD3(CCC_Mask,		"r2_exp_splitscene",	&ps_r2_ls_flags,			R2FLAG_EXP_SPLIT_SCENE);
    //	CMD3(CCC_Mask,		"r2_exp_donttest_uns",	&ps_r2_ls_flags,			R2FLAG_EXP_DONT_TEST_UNSHADOWED);
    CMD3(CCC_Mask, "r2_exp_donttest_shad", &ps_r2_ls_flags, R2FLAG_EXP_DONT_TEST_SHADOWED);

    CMD3(CCC_Mask, "r2_sun_tsm", &ps_r2_ls_flags, R2FLAG_SUN_TSM);
    CMD4(CCC_Float, "r2_sun_tsm_proj", &ps_r2_sun_tsm_projection, .001f, 0.8f);
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

    CMD3(CCC_Mask, "r2_gi", &ps_r2_ls_flags, R2FLAG_GI);
    CMD4(CCC_Float, "r2_gi_clip", &ps_r2_GI_clip, EPS, 0.1f);
    CMD4(CCC_Integer, "r2_gi_depth", &ps_r2_GI_depth, 1, 5);
    CMD4(CCC_Integer, "r2_gi_photons", &ps_r2_GI_photons, 8, 256);
    CMD4(CCC_Float, "r2_gi_refl", &ps_r2_GI_refl, EPS_L, 0.99f);

    CMD4(CCC_Integer, "r2_wait_sleep", &ps_r2_wait_sleep, 0, 1);

#ifndef MASTER_GOLD
    CMD4(CCC_Integer, "r2_dhemi_count", &ps_r2_dhemi_count, 4, 25);
    CMD4(CCC_Float, "r2_dhemi_sky_scale", &ps_r2_dhemi_sky_scale, 0.0f, 100.f);
    CMD4(CCC_Float, "r2_dhemi_light_scale", &ps_r2_dhemi_light_scale, 0, 100.f);
    CMD4(CCC_Float, "r2_dhemi_light_flow", &ps_r2_dhemi_light_flow, 0, 1.f);
    CMD4(CCC_Float, "r2_dhemi_smooth", &ps_r2_lt_smooth, 0.f, 10.f);
    CMD3(CCC_Mask, "rs_hom_depth_draw", &ps_r2_ls_flags_ext, R_FLAGEXT_HOM_DEPTH_DRAW);

#endif // DEBUG

    CMD3(CCC_Mask, "r2_shadow_cascede_zcul", &ps_r2_ls_flags_ext, R2FLAGEXT_SUN_ZCULLING);
//    CMD3(CCC_Mask, "r2_shadow_cascede_old", &ps_r2_ls_flags_ext, R2FLAGEXT_SUN_OLD);

    CMD4(CCC_Float, "r2_ls_depth_scale", &ps_r2_ls_depth_scale, 0.5, 1.5);
    CMD4(CCC_Float, "r2_ls_depth_bias", &ps_r2_ls_depth_bias, -0.5, +0.5);

    CMD4(CCC_Float, "r2_parallax_h", &ps_r2_df_parallax_h, .0f, .5f);
    //	CMD4(CCC_Float,		"r2_parallax_range",	&ps_r2_df_parallax_range,	5.0f,	175.0f	);

    CMD4(CCC_Float, "r2_slight_fade", &ps_r2_slight_fade, .2f, 2.f);

    CMD3(CCC_Mask, "r_mt_details", &ps_r2_ls_flags, R2FLAG_EXP_MT_DETAILS);
    CMD3(CCC_Mask, "r_mt_sun", &ps_r2_ls_flags, R2FLAG_EXP_MT_SUN);

    CMD3(CCC_Mask, "r_mt_rain", &ps_r2_ls_flags, R2FLAG_EXP_MT_RAIN);

    CMD3(CCC_Mask, "r2_volumetric_lights", &ps_r2_ls_flags, R2FLAG_VOLUMETRIC_LIGHTS);

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
    CMD3(CCC_Token, "r2_ssao", &ps_r_ssao, qssao_token);

    CMD3(CCC_Mask, "r4_enable_tessellation", &ps_r2_ls_flags_ext, R2FLAGEXT_ENABLE_TESSELLATION); // Need restart
    CMD3(CCC_Mask, "r4_wireframe", &ps_r2_ls_flags_ext, R2FLAGEXT_WIREFRAME); // Need restart
    CMD3(CCC_Mask, "r2_steep_parallax", &ps_r2_ls_flags, R2FLAG_STEEP_PARALLAX);

    CMD3(CCC_Token, "r2_sun_quality", &ps_r_sun_quality, qsun_quality_token);

    CMD3(CCC_Mask, "r2_visor_refl", &ps_r2_ls_flags_ext, R2FLAG_VISOR_REFL);
    CMD3(CCC_Mask, "r2_visor_refl_control", &ps_r2_ls_flags_ext, R2FLAG_VISOR_REFL_CONTROL);
    CMD4(CCC_Float, "r2_visor_refl_intensity", &ps_r2_visor_refl_intensity, 0.f, 1.f);
    CMD4(CCC_Float, "r2_visor_refl_radius", &ps_r2_visor_refl_radius, 0.3f, 0.6f);

    CMD3(CCC_Token, "r_aa_mode", &ps_r_pp_aa_mode, pp_aa_mode_token);

    CMD3(CCC_Token, "r3_msaa", &ps_r3_msaa, qmsaa_token);
    // CMD3(CCC_Mask,		"r3_msaa_hybrid",				&ps_r2_ls_flags,			R3FLAG_MSAA_HYBRID);
    // CMD3(CCC_Mask,		"r3_msaa_opt",					&ps_r2_ls_flags,			R3FLAG_MSAA_OPT);
    //CMD3(CCC_Mask, "r3_gbuffer_opt", &ps_r2_ls_flags, R3FLAG_GBUFFER_OPT); //xrSimpodin: отключение оптимизации отключено
    // CMD3(CCC_Mask,		"r3_use_dx10_1",				&ps_r2_ls_flags,			(u32)R3FLAG_USE_DX10_1);
    // CMD3(CCC_Token,		"r3_msaa_alphatest",			&ps_r3_msaa_atest,			qmsaa__atest_token);
    // CMD3(CCC_Token,		"r3_minmax_sm",					&ps_r3_minmax_sm,			qminmax_sm_token);

    CMD4(CCC_detail_radius, "r__detail_radius", &ps_r__detail_radius, 49, 300);
    CMD4(CCC_Integer, "r__no_scale_on_fade", &ps_no_scale_on_fade, 0, 1); // Alundaio

    //	Allow real-time fog config reload
#if (RENDER == R_R4)
#ifdef DEBUG
    CMD1(CCC_Fog_Reload, "r3_fog_reload");
#endif //	DEBUG
#endif //	(RENDER == R_R4)

    CMD3(CCC_Mask, "r3_dynamic_wet_surfaces", &ps_r2_ls_flags, R3FLAG_DYN_WET_SURF);
    CMD4(CCC_Integer, "r3_dynamic_wet_surfaces_sm_res", &ps_r3_dyn_wet_surf_sm_res, 64, 2048);
 //   CMD4(CCC_Integer, "r3_dynamic_wet_surfaces_enable_streaks", &ps_r3_dyn_wet_surf_enable_streaks, 0, 1); //Устарело

//	CMD4(CCC_Integer, "r3_dynamic_wet_surfaces_opt", &ps_r3_dyn_wet_surf_opt, 0, 1);
	CMD4(CCC_Float, "r3_dynamic_wet_surfaces_near", &ps_r3_dyn_wet_surf_near, 5, 70);
	CMD4(CCC_Float, "r3_dynamic_wet_surfaces_far", &ps_r3_dyn_wet_surf_far, 30, 300);

    CMD3(CCC_Mask, "r3_volumetric_smoke", &ps_r2_ls_flags, R3FLAG_VOLUMETRIC_SMOKE);
    CMD1(CCC_VideoMemoryStats, "video_memory_stats");

    // Screen Space Shaders
    CMD4(CCC_Vector4, "ssfx_wind_grass", &ps_ssfx_wind_grass, (Fvector4{}), (Fvector4{20.0f, 5.0f, 5.0f, 5.0f}));
    CMD4(CCC_Vector4, "ssfx_wind_trees", &ps_ssfx_wind_trees, (Fvector4{}), (Fvector4{20.0f, 5.0f, 5.0f, 1.0f}));
    CMD4(CCC_Integer, "ssfx_wind_bugged_trees_enable", &ps_ssfx_wind_bugged_flora_enable, 0, 1);

    CMD4(CCC_Vector4, "ssfx_lut", &ps_ssfx_lut, (Fvector4{}), (Fvector4{100.f, 100.f, 100.f, 100.f}));
    CMD4(CCC_Vector3, "ssfx_shadows", &ps_ssfx_shadows, (Fvector3{128.f, 1536.f, 0.f}), (Fvector3{1536.f, 4096.f, 0.f}));
    CMD4(CCC_Vector3, "ssfx_shadow_bias", &ps_ssfx_shadow_bias, (Fvector3{}), (Fvector3{1.0f, 1.0f, 1.0f}));
    CMD4(CCC_Vector3, "ssfx_volumetric_limits", &ps_ssfx_volumetric_limits, (Fvector3{0.f, 0.f, 1.0f}), (Fvector3{100.0f, 1.0f, 5.0f}));
    CMD4(CCC_Integer, "ssfx_use_new_volumetric_method", &ps_ssfx_use_new_volumetric_method, 0, 1);
    
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

    CMD4(CCC_Vector4, "ssfx_grass_shadows", &ps_ssfx_grass_shadows, Fvector4().set(0, 0, 0, 0), Fvector4().set(3, 1, 100, 100));
    CMD4(CCC_ssfx_cascades, "ssfx_shadow_cascades", &ps_ssfx_shadow_cascades, (Fvector3{1.0f, 1.0f, 1.0f}), (Fvector3{300.f, 300.f, 300.f}));
    CMD4(CCC_Float, "ssfx_wpn_dof_2", &ps_ssfx_wpn_dof_2, 0, 1);
    CMD4(CCC_Vector4, "ssfx_grass_interactive", &ps_ssfx_grass_interactive, (Fvector4{}), (Fvector4{1.f, static_cast<float>(GRASS_SHADER_DATA_COUNT), 5000.f, 1.f}));
    CMD4(CCC_Vector3, "ssfx_int_grass_params_1", &ps_ssfx_int_grass_params_1, (Fvector3{}), (Fvector3{5.f, 5.f, 5.f}));
    CMD4(CCC_Vector4, "ssfx_int_grass_params_2", &ps_ssfx_int_grass_params_2, (Fvector4{}), (Fvector4{5.f, 20.f, 1.f, 5.f}));

    CMD3(CCC_Mask, "ssfx_height_fog", &ps_r2_ls_flags_ext, SSFX_HEIGHT_FOG);
    CMD3(CCC_Mask, "ssfx_sky_debanding", &ps_r2_ls_flags_ext, SSFX_SKY_DEBANDING);
    CMD3(CCC_Mask, "ssfx_indirect_light", &ps_r2_ls_flags_ext, SSFX_INDIRECT_LIGHT);
    CMD3(CCC_Mask, "ssfx_inter_grass", &ps_r2_ls_flags_ext, SSFX_INTER_GRASS);
    CMD4(CCC_Float, "ssfx_exposure", &ps_r2_img_exposure, 0.5f, 1.5f);
    CMD4(CCC_Float, "ssfx_gamma", &ps_r2_img_gamma, 0.5f, 1.5f);
    CMD4(CCC_Float, "ssfx_saturation", &ps_r2_img_saturation, 0.5f, 1.5f);
    CMD4(CCC_Vector3, "ssfx_color_grading", &ps_r2_img_cg, (Fvector3{}), (Fvector3{1.f, 1.f, 1.f}));
    CMD3(CCC_Mask, "reflections_only_on_terrain", &ps_r2_ls_flags_ext, REFLECTIONS_ONLY_ON_TERRAIN);
    CMD3(CCC_Mask, "reflections_only_on_puddles", &ps_r2_ls_flags_ext, REFLECTIONS_ONLY_ON_PUDDLES);

    //	CMD3(CCC_Mask,		"r2_sun_ignore_portals",		&ps_r2_ls_flags,			R2FLAG_SUN_IGNORE_PORTALS);

    CMD1(CCC_PART_Export, "particles_export");
    CMD1(CCC_PART_Import, "particles_import");

    extern BOOL bShadersXrExport;
    CMD4(CCC_Integer, "shaders_xr_export", &bShadersXrExport, FALSE, TRUE);

    extern BOOL bSenvironmentXrExport;
    CMD4(CCC_Integer, "senvironment_xr_export", &bSenvironmentXrExport, FALSE, TRUE);

    CMD4(CCC_Integer, "r_lens_flare", &ps_lens_flare, FALSE, TRUE);
}

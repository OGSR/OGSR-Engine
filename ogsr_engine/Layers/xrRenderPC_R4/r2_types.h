#pragma once

// Base targets
#define r2_RT_base "$user$base_"
#define r2_RT_base_depth "$user$base_depth"

#define r2_RT_velocity "$user$velocity" // Motion Vectors
#define r2_RT_zbuffer "$user$zbuffer"
#define r2_RT_zbuffer_dof "$user$zbuffer_dof"

// r3xx code-path (MRT)
#define r2_RT_P "$user$position" // MRT
#define r2_RT_albedo "$user$albedo" // MRT

// other
#define r2_RT_accum "$user$accum" // ---	16 bit fp or 16 bit fx

#define r2_T_envs0 "$user$env_s0" // ---
#define r2_T_envs1 "$user$env_s1" // ---

#define r2_T_sky0 "$user$sky0"
#define r2_T_sky1 "$user$sky1"

#define r2_RT_generic0 "$user$generic0"
#define r2_RT_generic0_prev "$user$generic0_previous"
#define r2_RT_generic_combine "$user$generic_combine"
#define r2_RT_generic_combine_scope "$user$generic_combine_scope"

#define r2_RT_generic1 "$user$generic1" // ---
#define r2_RT_generic2 "$user$generic2" // ---	//	Igor: for volumetric lights
#define r2_RT_generic0_temp "$user$generic_temp" // Temporal RT for water reflections

#define r2_RT_scopert "$user$scopeRT" // crookr

#define r2_RT_bloom1 "$user$bloom1" // ---
#define r2_RT_bloom2 "$user$bloom2" // ---

#define r2_RT_luminance_t64 "$user$lum_t64" // --- temp
#define r2_RT_luminance_t8 "$user$lum_t8" // --- temp

#define r2_RT_luminance_src "$user$tonemap_src" // --- prev-frame-result
#define r2_RT_luminance_cur "$user$tonemap" // --- result
#define r2_RT_luminance_pool "$user$luminance" // --- pool

//#define r2_RT_smap_surf "$user$smap_surf" // --- directional
#define r2_RT_smap_depth "$user$smap_depth" // ---directional
#define r2_RT_smap_rain "$user$smap_rain"

#define r2_jitter "$user$jitter_" // --- dither
#define r2_sunmask "sunmask"

// SMAA
#define r2_RT_smaa_edgetex "$user$smaa_edgetex"
#define r2_RT_smaa_blendtex "$user$smaa_blendtex"

#define r2_RT_blur_h_2 "$user$blur_h_2"
#define r2_RT_blur_2 "$user$blur_2"

#define r2_RT_blur_h_4 "$user$blur_h_4"
#define r2_RT_blur_4 "$user$blur_4"

#define r2_RT_blur_h_8 "$user$blur_h_8"
#define r2_RT_blur_8 "$user$blur_8"

#define r2_RT_mask_drops_blur "$user$mask_drops_blur"

#define r2_RT_pp_bloom "$user$pp_bloom"

#define r2_RT_dof "$user$dof"

#define r2_RT_heat "$user$heat"

#define r2_RT_secondVP "$user$viewport2" // Хранит картинку со второго вьюпорта

#define r2_RT_sunshafts0 "$user$sun_shafts0" // first rt
#define r2_RT_sunshafts1 "$user$sun_shafts1" // second rt

#define r2_RT_SunShaftsMask "$user$SunShaftsMask"
#define r2_RT_SunShaftsMaskSmoothed "$user$SunShaftsMaskSmoothed"
#define r2_RT_SunShaftsPass0 "$user$SunShaftsPass0"

#define r2_RT_flares "$user$flares" // KD lensflares

constexpr const char* c_sbase = "s_base";

#define r2_RT_ssfx_bloom1 "$user$ssfx_bloom1" // Bloom
#define r2_RT_ssfx_bloom_emissive "$user$ssfx_bloom_emissive" // Bloom
#define r2_RT_ssfx_bloom_lens "$user$ssfx_bloom_lens" // Bloom
#define r2_RT_ssfx_bloom_tmp2 "$user$ssfx_bloom_tmp2" // Bloom
#define r2_RT_ssfx_bloom_tmp4 "$user$ssfx_bloom_tmp4" // Bloom
#define r2_RT_ssfx_bloom_tmp8 "$user$ssfx_bloom_tmp8" // Bloom
#define r2_RT_ssfx_bloom_tmp16 "$user$ssfx_bloom_tmp16" // Bloom
#define r2_RT_ssfx_bloom_tmp32 "$user$ssfx_bloom_tmp32" // Bloom
#define r2_RT_ssfx_bloom_tmp64 "$user$ssfx_bloom_tmp64" // Bloom
#define r2_RT_ssfx_bloom_tmp32_2 "$user$ssfx_bloom_tmp32_2" // Bloom
#define r2_RT_ssfx_bloom_tmp16_2 "$user$ssfx_bloom_tmp16_2" // Bloom
#define r2_RT_ssfx_bloom_tmp8_2 "$user$ssfx_bloom_tmp8_2" // Bloom
#define r2_RT_ssfx_bloom_tmp4_2 "$user$ssfx_bloom_tmp4_2" // Bloom

#define JITTER(a) r2_jitter #a

const float SMAP_near_plane = .1f;

const u32 SMAP_adapt_min = 768; // 32	;
const u32 SMAP_adapt_optimal = 768;
const u32 SMAP_adapt_max = 1536;

const u32 TEX_jitter = 64;
const u32 TEX_jitter_count = 2; // Simp: реально в шейдерах используется только jitter0 и jitter1. Не понятно зачем вообще столько текстур шума, одной было бы достаточно.

const u32 BLOOM_size_X = 256;
const u32 BLOOM_size_Y = 256;
const u32 LUMINANCE_size = 16;

// deffer
#define SE_R2_NORMAL_HQ 0 // high quality/detail
#define SE_R2_NORMAL_LQ 1 // low quality
#define SE_R2_SHADOW 2 // shadow generation

// spot
#define SE_L_FILL 0
#define SE_L_UNSHADOWED 1
#define SE_L_NORMAL 2 // typical, scaled
#define SE_L_FULLSIZE 3 // full texture coverage
#define SE_L_TRANSLUENT 4 // with opacity/color mask

// mask
#define SE_MASK_SPOT 0
#define SE_MASK_POINT 1
#define SE_MASK_DIRECT 2

// sun
#define SE_SUN_NEAR 0
#define SE_SUN_MIDDLE 1
#define SE_SUN_FAR 2

//	For rain R3 rendering
#define SE_SUN_RAIN_SMAP 5

inline float u_diffuse2s(const float x, const float y, const float z)
{
    if (ps_ssfx_gloss_method == 0)
    {
        const float v = (x + y + z) / 3.f;
        return /*ps_r2_gloss_min +*/ ps_r2_gloss_factor * ((v < 1) ? powf(v, 2.f / 3.f) : v);
    }
    else
    {
        // Remove sun from the equation and clamp value.
        return ps_ssfx_gloss_minmax.x + clampr(ps_ssfx_gloss_minmax.y - ps_ssfx_gloss_minmax.x, 0.0f, 1.0f) * ps_ssfx_gloss_factor;
    }
}

inline float u_diffuse2s(const Fvector3& c) { return u_diffuse2s(c.x, c.y, c.z); }

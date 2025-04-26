#ifndef SLOAD_H
#define SLOAD_H

#include "common.h"

#define USE_ERROR_CORRECTION
//#define RECALCULATENORMALZ
//#define NORMALIZE_TEXTURES
static const float NORMAL_STRENGTH = 1.0;
static const float DETAIL_STRENGTH = 1.0;
static const float DETAIL_TINT = 1.0;
static const float DETAIL_GLOSS = 1.0;

uniform float4 ssfx_pom; // Samples, Range, Height, AO

//////////////////////////////////////////////////////////////////////////////////////////
// Texture samplers and blenders             //
//////////////////////////////////////////////////////////////////////////////////////////

float3 NormalBlend(float3 A, float3 B) { return normalize(float3(A.rg + B.rg, A.b * B.b)); }

float3 SampleNormal(float4 N, float4 NE)
{
    float3 Norm = unpack_normal(N.wzy);
#ifdef USE_ERROR_CORRECTION
    Norm += unpack_normal(NE.xyz);
#endif
#ifdef RECALCULATENORMALZ
    Norm.z = sqrt(1 - saturate(dot(Norm.xy, Norm.xy)));
#endif
#ifdef NORMALIZE_TEXTURES
    Norm = normalize(Norm);
#endif
    return Norm;
}

float3 NormalStrength(float3 N, float Strength)
{
    if (Strength != 1.0)
    {
        N.xy *= Strength;
        N.z = sqrt(1 - saturate(dot(N.xy, N.xy)));
        N = normalize(N);
    }
    return N;
}

float SampleGloss(float4 N) { return N.x; }

float SampleHeight(float4 NE) { return NE.w; }

float3 ApplyDetailAlbedo(float3 A1, float3 A2)
{
    // return saturate(A1 * A2 * 2);
    return saturate(A1 * exp2(DETAIL_TINT * (A2 * 2 - 1)));
}

float3 ApplyDetailNormal(float3 N1, float3 N2)
{
    N1 += float3(0, 0, 1);
    N2 *= float3(-1, -1, 1);
    return normalize(N1 * dot(N1, N2) / N1.z - N2);
}

float ApplyDetailGloss(float G1, float G2)
{
    // return saturate(G1 * G2 * 2);
    // return saturate(G1 + (DETAIL_GLOSS * (G2 * 2 - 1)));
    return saturate(G1 * exp2(DETAIL_GLOSS * (G2 * 2 - 1)));
}

float ApplyDetailHeight(float H1, float H2) { return H1 + (H2 * 2 - 1); }

//////////////////////////////////////////////////////////////////////////////////////////
// Bumped surface loader                //
//////////////////////////////////////////////////////////////////////////////////////////
struct surface_bumped
{
    float4 base;
    float3 normal;
    float gloss;
    float height;
};

float4 tbase(float2 tc) { return s_base.Sample(smp_base, tc); }

#if defined(ALLOW_STEEPPARALLAX) && defined(USE_STEEPPARALLAX)

/**
 * @ Version: SCREEN SPACE SHADERS - UPDATE 22
 * @ Description: POM Shader - Modified version from the terrain POM implementation
 * @ Author: https://www.moddb.com/members/ascii1457
 * @ Mod: https://www.moddb.com/mods/stalker-anomaly/addons/screen-space-shaders
 */
void UpdateTC(inout p_bumped I, inout float H)
{
    // Distance attenuation
    float dist_att = 1.0f - smoothstep(ssfx_pom.y * 0.8f, ssfx_pom.y, I.position.z);

    if (I.position.z >= ssfx_pom.y)
        return;

    float3 eye = normalize(mul(float3x3(I.M1.x, I.M2.x, I.M3.x, I.M1.y, I.M2.y, I.M3.y, I.M1.z, I.M2.z, I.M3.z), -I.position));

    float view_angle = abs(dot(float3(0.0, 0.0, 1.0), eye));

    // Dynamic steps
    float _step = rcp(lerp(ssfx_pom.x, 3, view_angle));
    _step *= 0.5; // We are working with 0.0 ~ 0.5

    // View direction + bias to try to minimize issues with some slopes.
    float2 viewdir = eye.xy / (abs(eye.z) + 0.41f);

    // Offset direction
    float2 tc_step = _step * viewdir * ssfx_pom.z * dist_att;

    // Init vars to store steps
    float curr_step = 0;
    float2 parallax_tc = I.tcdh;

    // Store the previous & current sample to do the [POM] calc and [Contact Refinement] if needed.
    float prev_Height = 0;
    float curr_Height = 0.5f;

    do // Step Parallax
    {
        // Save previous data
        prev_Height = curr_Height;

        // Step TexCoor
        parallax_tc -= tc_step;
        curr_step += _step;

        // Sample
        curr_Height = 0.5f - s_bumpX.SampleLevel(smp_base, parallax_tc, 0).a;

    } while (curr_Height >= curr_step);

// [ Contact Refinement ]
#ifdef SSFX_POM_REFINE

    // Step back
    parallax_tc += tc_step;
    curr_step -= _step;
    curr_Height = prev_Height; // Previous height

    // Increase precision ( 3 times seems like a good balance )
    _step /= 3;
    tc_step /= 3;

    do // Step Parallax
    {
        // Save previous data ( Used for interpolation )
        prev_Height = curr_Height;

        // Step TexCoor
        parallax_tc -= tc_step;
        curr_step += _step;

        // Sample
        curr_Height = 0.5f - s_bumpX.SampleLevel(smp_base, parallax_tc, 0).a;

    } while (curr_Height >= curr_step);

#endif

    // [ POM ] Interpolation between the previous offset and the current offset
    float currentDiff = curr_Height - curr_step;
    float ratio = currentDiff / (currentDiff - saturate(prev_Height - curr_step + _step));

    // Final TexCoor
    float2 final_tc = lerp(parallax_tc, parallax_tc + tc_step, ratio);

#if defined(USE_TDETAIL)
    I.tcdbump = final_tc * dt_params; // Apply detail_scaler
#endif

    // Apply Parallax TC
    I.tcdh = final_tc;
}

#elif defined(USE_PARALLAX) || defined(USE_STEEPPARALLAX)

void UpdateTC(inout p_bumped I, inout float H)
{
    float3 eye = mul(float3x3(I.M1.x, I.M2.x, I.M3.x, I.M1.y, I.M2.y, I.M3.y, I.M1.z, I.M2.z, I.M3.z), -I.position.xyz);

    float height = s_bumpX.Sample(smp_base, I.tcdh).w;
    H = height;
    height = height * (parallax.x) + (parallax.y);
    float2 new_tc = I.tcdh + height * normalize(eye);

    //	Output the result
    I.tcdh.xy = new_tc;

#if defined(USE_TDETAIL)
    I.tcdbump = new_tc * dt_params; // Apply detail_scaler
#endif
}

#else

void UpdateTC(inout p_bumped I, inout float H) {}

#endif

surface_bumped sload_i(p_bumped I)
{
    surface_bumped S;

    float H = 0;

    UpdateTC(I, H); // Parallax

    float4 Nu = s_bump.Sample(smp_base, I.tcdh);

#if defined(ALLOW_STEEPPARALLAX) && defined(USE_STEEPPARALLAX)
    H = s_bumpX.Sample(smp_base, I.tcdh); // Only requiered when the full parallax is used
#endif

    S.base = tbase(I.tcdh);
    S.normal = unpack_normal(Nu.wzy);
    S.gloss = Nu.x;
    S.height = H;

#if defined(ALLOW_STEEPPARALLAX) && defined(USE_STEEPPARALLAX)
    S.base.rgb *= lerp(1.0f, saturate((S.height + 0.15f) * 2.0f), ssfx_pom.w); // Apply AO
#endif

#ifdef USE_TDETAIL
#ifdef USE_TDETAIL_BUMP

    float4 NDetail = s_detailBump.Sample(smp_base, I.tcdbump);
    float4 NDetailX = s_detailBumpX.Sample(smp_base, I.tcdbump);

    float3 DetailNormal = unpack_normal(NDetail.wzy) + unpack_normal(NDetailX.xyz);
    S.normal = NormalBlend(S.normal, DetailNormal);
    S.gloss = S.gloss * NDetail.x * 2;

    float4 detail = s_detail.Sample(smp_base, I.tcdbump);
    S.base.rgb = S.base.rgb * detail.rgb * 2;

#else

    float4 detail = s_detail.Sample(smp_base, I.tcdbump);
    S.base.rgb = S.base.rgb * detail.rgb * 2;
    S.gloss = S.gloss * detail.w * 2;

#endif
#endif

    return S;
}

surface_bumped sload(p_bumped I) { return sload_i(I); }

surface_bumped sload(p_bumped I, float2 pixeloffset) { return sload_i(I); }

#ifdef USE_HAT
void calc_alpha_test(float2 tc, float alpha)
{
    // if (m_taa_jitter.z > 0.f)
    //{
    // }
    // else
    //{
    clip(alpha - def_aref);
    //}
}
#else
void calc_alpha_test(float2 tc, float alpha) { clip(alpha - def_aref); }
#endif

#endif

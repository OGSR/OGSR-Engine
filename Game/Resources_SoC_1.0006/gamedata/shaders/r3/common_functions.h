#ifndef common_functions_h_included
#define common_functions_h_included

#include "srgb.h"

float3 vibrance(float3 img, float val)
{
    float luminance = dot(float3(img.rgb), LUMINANCE_VECTOR);
    return float3(lerp(luminance, float3(img.rgb), val));
}

float2 hash22(float2 p)
{
    float3 p3 = frac(float3(p.xyx) * float3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.xx + p3.yz) * p3.zy);
}

float noise(float2 tc) { return frac(sin(dot(tc, float2(12.0, 78.0) + (timers.x))) * 43758.0) * 0.25f; }

// contrast function
float Contrast(float Input, float ContrastPower)
{
    // piecewise contrast function
    bool IsAboveHalf = Input > 0.5;
    float ToRaise = saturate(2 * (IsAboveHalf ? 1 - Input : Input));
    float Output = 0.5 * pow(ToRaise, ContrastPower);
    Output = IsAboveHalf ? 1 - Output : Output;
    return Output;
}

float3 tonemap(float3 rgb, float scale)
{
    rgb = SRGBToLinear(rgb);
    rgb = rgb * scale;
    rgb = LinearTosRGB(rgb);

    const float fWhiteIntensity = 11.2;

    return tonemap_sRGB(rgb, fWhiteIntensity);
}

// CUSTOM
float3 blend_soft(float3 a, float3 b)
{
    // return 1.0 - (1.0 - a) * (1.0 - b);

    // gamma correct and inverse tonemap to add bloom
    a = SRGBToLinear(a); // post tonemap render
    a = a / max(0.004, 1 - a); // inverse tonemap
    // a = a / max(0.001, 1-a); //inverse tonemap
    b = SRGBToLinear(b); // bloom

    // constrast reduction of ACES output
    float Contrast_Amount = 0.7;
    const float mid = 0.18;
    a = pow(a, Contrast_Amount) * mid / pow(mid, Contrast_Amount);

    ACES_LMT(b); // color grading bloom
    a += b; // bloom add

    // Boost the contrast to match ACES RRT
    float Contrast_Boost = 1.42857;
    a = pow(a, Contrast_Boost) * mid / pow(mid, Contrast_Boost);

    a = a / (1 + a); // tonemap

    a = LinearTosRGB(a);
    return a;
}

float calc_fogging(float4 w_pos) { return dot(w_pos, fog_plane); }

float2 unpack_tc_base(float2 tc, float du, float dv)
{
    return (tc.xy + float2(du, dv)) * (32.f / 32768.f); //! Increase from 32bit to 64bit floating point
}

float3 calc_sun_r1(float3 norm_w) { return L_sun_color * saturate(dot((norm_w), -L_sun_dir_w)); }

float3 calc_model_hemi_r1(float3 norm_w) { return max(0, norm_w.y) * L_hemi_color; }

float3 calc_model_lq_lighting(float3 norm_w) { return L_material.x * calc_model_hemi_r1(norm_w) + L_ambient + L_material.y * calc_sun_r1(norm_w); }

float3 unpack_normal(float3 v) { return 2 * v - 1; }
float3 unpack_bx2(float3 v) { return 2 * v - 1; }
float3 unpack_bx4(float3 v) { return 4 * v - 2; } //! reduce the amount of stretching from 4*v-2 and increase precision
float2 unpack_tc_lmap(float2 tc) { return tc * (1.f / 32768.f); } // [-1  .. +1 ]
float4 unpack_color(float4 c) { return c.bgra; }
float4 unpack_D3DCOLOR(float4 c) { return c.bgra; }
float3 unpack_D3DCOLOR(float3 c) { return c.bgr; }

float3 p_hemi(float2 tc)
{
    float4 t_lmh = s_hemi.Sample(smp_rtlinear, tc);
    return dot(t_lmh.rgb, 1.h / 3.h);
}

float get_hemi(float4 lmh) { return dot(lmh.rgb, 1.h / 3.h); }

float get_sun(float4 lmh) { return lmh.a; }

float3 v_hemi(float3 n) { return L_hemi_color * (.5f + .5f * n.y); }

float3 v_sun(float3 n) { return L_sun_color * dot(n, -L_sun_dir_w); }

float3 calc_reflection(float3 pos_w, float3 norm_w) { return reflect(normalize(pos_w - eye_position), norm_w); }

// https://knarkowicz.wordpress.com/2014/04/16/octahedron-normal-vector-encoding/
float2 OctWrap(float2 v) { return (1.0 - abs(v.yx)) * (v.xy >= 0.0 ? 1.0 : -1.0); }

float2 gbuf_pack_normal(float3 n)
{
    n /= (abs(n.x) + abs(n.y) + abs(n.z));
    n.xy = n.z >= 0.0 ? n.xy : OctWrap(n.xy);
    n.xy = n.xy * 0.5 + 0.5;
    return n.xy;
}

float3 gbuf_unpack_normal(float2 f)
{
    f = f * 2.0 - 1.0;

    float3 n = float3(f.x, f.y, 1.0 - abs(f.x) - abs(f.y));
    float t = saturate(-n.z);
    n.xy += n.xy >= 0.0 ? -t : t;
    return normalize(n);
}

#ifdef REFLECTIONS_ONLY_ON_TERRAIN

static const uint USABLE_BIT_1 = 1 << 13;
static const uint USABLE_BIT_2 = 1 << 14;
static const uint USABLE_BIT_3 = 1 << 15;
static const uint USABLE_BIT_4 = 1 << 16;
static const uint USABLE_BIT_5 = 1 << 17;
static const uint USABLE_BIT_6 = 1 << 18;
static const uint USABLE_BIT_7 = 1 << 19;
static const uint USABLE_BIT_8 = 1 << 20;
static const uint USABLE_BIT_9 = 1 << 21;
static const uint USABLE_BIT_10 = 1 << 22;
static const uint USABLE_BIT_11 = 1 << 23; // At least two of those four bit flags must be mutually exclusive (i.e. all 4 bits must not be set together)
static const uint USABLE_BIT_12 = 1 << 24; // This is because setting 0x47800000 sets all 5 FP16 exponent bits to 1 which means infinity
static const uint USABLE_BIT_13 = 1 << 25; // This will be translated to a +/-MAX_FLOAT in the FP16 render target (0xFBFF/0x7BFF), overwriting the
static const uint USABLE_BIT_14 = 1 << 26; // mantissa bits where other bit flags are stored.
static const uint MUST_BE_SET = 1 << 30; // This flag *must* be stored in the floating-point representation of the bit flag to store
static const uint USABLE_BIT_15 = 1 << 31;

float gbuf_pack_hemi_mtl(float hemi, float mtl, const bool use_reflections)
{
    uint packed_mtl = uint((mtl / 1.333333333) * 31.0);
    // Clamp hemi max value
    uint packed = (MUST_BE_SET + (uint(saturate(hemi) * 255.9) << 12) + ((packed_mtl & uint(31)) << 21));

    if ((packed & USABLE_BIT_12) == 0)
        packed |= USABLE_BIT_13;

    if (packed_mtl & uint(16))
        packed |= USABLE_BIT_14;

    if (use_reflections)
        packed |= USABLE_BIT_15;
    else
        packed &= ~USABLE_BIT_15;

    return asfloat(packed);
}

float gbuf_unpack_hemi(float mtl_hemi) { return float((asuint(mtl_hemi) >> 12) & uint(255)) * (1.0 / 254.8); }

float gbuf_unpack_mtl(float mtl_hemi)
{
    uint packed = asuint(mtl_hemi);
    uint packed_hemi = ((packed >> 21) & uint(15)) + ((packed & USABLE_BIT_14) == 0 ? 0 : 16);
    return float(packed_hemi) * (1.0 / 31.0) * 1.333333333;
}

bool gbuf_unpack_refl_flag(float mtl_hemi)
{
    uint packed = asuint(mtl_hemi);
    return (packed & USABLE_BIT_15) != 0;
}

#else

#define USABLE_BIT_1 uint(0x00002000)
#define USABLE_BIT_2 uint(0x00004000)
#define USABLE_BIT_3 uint(0x00008000)
#define USABLE_BIT_4 uint(0x00010000)
#define USABLE_BIT_5 uint(0x00020000)
#define USABLE_BIT_6 uint(0x00040000)
#define USABLE_BIT_7 uint(0x00080000)
#define USABLE_BIT_8 uint(0x00100000)
#define USABLE_BIT_9 uint(0x00200000)
#define USABLE_BIT_10 uint(0x00400000)
#define USABLE_BIT_11 uint(0x00800000) // At least two of those four bit flags must be mutually exclusive (i.e. all 4 bits must not be set together)
#define USABLE_BIT_12 uint(0x01000000) // This is because setting 0x47800000 sets all 5 FP16 exponent bits to 1 which means infinity
#define USABLE_BIT_13 uint(0x02000000) // This will be translated to a +/-MAX_FLOAT in the FP16 render target (0xFBFF/0x7BFF), overwriting the
#define USABLE_BIT_14 uint(0x04000000) // mantissa bits where other bit flags are stored.
#define USABLE_BIT_15 uint(0x80000000)
#define MUST_BE_SET uint(0x40000000) // This flag *must* be stored in the floating-point representation of the bit flag to store

float gbuf_pack_hemi_mtl(float hemi, float mtl)
{
    uint packed_mtl = uint((mtl / 1.333333333) * 31.0);
    // Clamp hemi max value
    uint packed = (MUST_BE_SET + (uint(saturate(hemi) * 255.9) << 13) + ((packed_mtl & uint(31)) << 21));

    if ((packed & USABLE_BIT_13) == 0)
        packed |= USABLE_BIT_14;

    if (packed_mtl & uint(16))
        packed |= USABLE_BIT_15;

    return asfloat(packed);
}

float gbuf_unpack_hemi(float mtl_hemi) { return float((asuint(mtl_hemi) >> 13) & uint(255)) * (1.0 / 254.8); }

float gbuf_unpack_mtl(float mtl_hemi)
{
    uint packed = asuint(mtl_hemi);
    uint packed_hemi = ((packed >> 21) & uint(15)) + ((packed & USABLE_BIT_15) == 0 ? 0 : 16);
    return float(packed_hemi) * (1.0 / 31.0) * 1.333333333;
}

#endif

#ifndef EXTEND_F_DEFFER
f_deffer pack_gbuffer(float4 norm, float4 pos, float4 col, const bool use_reflections = false)
#else
f_deffer pack_gbuffer(float4 norm, float4 pos, float4 col, uint imask, const bool use_reflections = false)
#endif
{
    f_deffer res;

    res.position = float4(gbuf_pack_normal(norm), pos.z,
                          gbuf_pack_hemi_mtl(norm.w, pos.w
#ifdef REFLECTIONS_ONLY_ON_TERRAIN
                                             ,
                                             use_reflections
#endif
                                             ));
    res.C = col;

    if (L_hotness.x > 0.0)
        res.H = float4(L_hotness.x, L_hotness.y, 0.0, 1.0);
    else
        res.H = float4(0.0, 0.0, 0.0, 0.0);

#ifdef EXTEND_F_DEFFER
    res.mask = imask;
#endif

    return res;
}

gbuffer_data gbuffer_load_data(float2 tc : TEXCOORD, float2 pos2d, int iSample)
{
    gbuffer_data gbd;

    gbd.P = float3(0, 0, 0);
    gbd.hemi = 0;
    gbd.mtl = 0;
    gbd.C = 0;
    gbd.N = float3(0, 0, 0);

    float4 P = s_position.Sample(smp_nofilter, tc);

    pos2d = pos2d - m_taa_jitter.xy * float2(0.5f, -0.5f) * pos_decompression_params2.xy;

    // 3d view space pos reconstruction math
    // center of the plane (0,0) or (0.5,0.5) at distance 1 is eyepoint(0,0,0) + lookat (assuming |lookat| ==1
    // left/right = (0,0,1) -/+ tan(fHorzFOV/2) * (1,0,0 )
    // top/bottom = (0,0,1) +/- tan(fVertFOV/2) * (0,1,0 )
    // lefttop = ( -tan(fHorzFOV/2),  tan(fVertFOV/2), 1 )
    // righttop = (  tan(fHorzFOV/2),  tan(fVertFOV/2), 1 )
    // leftbottom = ( -tan(fHorzFOV/2), -tan(fVertFOV/2), 1 )
    // rightbottom = (  tan(fHorzFOV/2), -tan(fVertFOV/2), 1 )
    gbd.P = float3(P.z * (pos2d * pos_decompression_params.zw - pos_decompression_params.xy), P.z);

    // reconstruct N
    gbd.N = gbuf_unpack_normal(P.xy);

    // reconstruct material
    gbd.mtl = gbuf_unpack_mtl(P.w);

    // reconstruct hemi
    gbd.hemi = gbuf_unpack_hemi(P.w);

#ifdef REFLECTIONS_ONLY_ON_TERRAIN
    gbd.refl_flag = gbuf_unpack_refl_flag(P.w);
#endif

    float4 C = s_diffuse.Sample(smp_nofilter, tc);

    gbd.C = C.xyz;
    gbd.gloss = C.w;

    return gbd;
}

gbuffer_data gbuffer_load_data(float2 tc : TEXCOORD, float2 pos2d) { return gbuffer_load_data(tc, pos2d, 0); }

gbuffer_data gbuffer_load_data_offset(float2 tc : TEXCOORD, float2 OffsetTC : TEXCOORD, float2 pos2d)
{
    float2 delta = ((OffsetTC - tc) * pos_decompression_params2.xy);

    return gbuffer_load_data(OffsetTC, pos2d + delta, 0);
}

gbuffer_data gbuffer_load_data_offset(float2 tc : TEXCOORD, float2 OffsetTC : TEXCOORD, float2 pos2d, uint iSample)
{
    float2 delta = ((OffsetTC - tc) * pos_decompression_params2.xy);

    return gbuffer_load_data(OffsetTC, pos2d + delta, iSample);
}

//#define SKY_WITH_DEPTH // sky renders with
// depth to avoid some problems with reflections

#define SKY_DEPTH float(10000.0)
#define SKY_EPS float(0.001)

#ifndef SKY_WITH_DEPTH
float is_sky(float depth) { return step(depth, SKY_EPS); }
float is_not_sky(float depth) { return step(SKY_EPS, depth); }
#else
float is_sky(float depth) { return step(abs(depth - SKY_DEPTH), SKY_EPS); }
float is_not_sky(float depth) { return step(SKY_EPS, abs(depth - SKY_DEPTH)); }
#endif

float3 compute_colored_ao(float ao, float3 albedo)
{ // https://www.activision.com/cdn/research/s2016_pbs_activision_occlusion.pptx
    float3 a = 2.0404 * albedo - 0.3324;
    float3 b = -4.7951 * albedo + 0.6417;
    float3 c = 2.7552 * albedo + 0.6903;

    return max(ao, ((ao * a + b) * ao + c) * ao);
}

//////////////////////////////////////////////////////////////////////////////////////////

float2 aspect_ratio_correction(float2 tc)
{
    tc.x -= 0.5f;
    tc.x *= (screen_res.x / screen_res.y);
    tc.x += 0.5f;
    return tc;
}

#endif // common_functions_h_included

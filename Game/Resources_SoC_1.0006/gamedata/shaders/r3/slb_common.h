#ifndef SLB_COMMON_H
#define SLB_COMMON_H

#include "slb_aliases.h"

#define SLB_PI 3.14159265358979323846
#define SLB_TAU 6.28318530717958647693
#define SLB_GoldenAngle 2.39996322972865332223
#define SLB_GoldenRatio 0.61803398874989484820

/// Taken from https://gist.github.com/yiwenl/3f804e80d0930e34a0b33359259b556c
float2x2 SLB_Rotate2Matrix(float a)
{
    float s = sin(a);
    float c = cos(a);
    return float2x2(float2(c, s), float2(-s, c));
}

float2 SLB_Rotate2(float2 v, float a)
{
    float2x2 m = SLB_Rotate2Matrix(a);
    return mul(v, m);
}

float SLB_ToFloat1(uint v) { return float(v) * (1.0 / float(0xffffffffu)); }

float2 SLB_ToFloat2(uint2 v) { return float2(v) * (1.0 / float(0xffffffffu)); }

float3 SLB_ToFloat3(uint3 v) { return float3(v) * (1.0 / float(0xffffffffu)); }

float4 SLB_ToFloat4(uint4 v) { return float4(v) * (1.0 / float(0xffffffffu)); }

float SLB_Min3(float3 v) { return min(v.x, min(v.y, v.z)); }

float SLB_Max3(float3 v) { return max(v.x, max(v.y, v.z)); }

/// Taken from https://web.archive.org/web/20200306054846/http://lolengine.net:80/blog/2013/07/27/rgb-to-hsv-in-glsl
float3 SLB_RGB_To_HSV(float3 c)
{
    float4 K = float4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    float4 p = c.g < c.b ? float4(c.bg, K.wz) : float4(c.gb, K.xy);
    float4 q = c.r < p.x ? float4(p.xyw, c.r) : float4(c.r, p.yzx);
    float d = q.x - min(q.w, q.y);
    const float e = 1.0e-10;
    return float3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

/// It took me long time to found this somewhere in vanilla shaders codebase;
float2 SLB_Pos2DToCoord(float2 pos2d)
{
    // return float2((pos2d.x * screen_res.z * 2.0 - 1.0), (pos2d.y * screen_res.w * 2.0 - 1.0))/2.0 * screen_res.xy;
    return (pos2d.xy * screen_res.zw - 0.5) * screen_res.xy;
}

/// Taken from https://iquilezles.org/articles/smoothsteps
float SLB_Cubic1D(float x) { return x * x * (3.0 - 2.0 * x); }

/// Taken from https://iquilezles.org/articles/smoothsteps
float SLB_Quartic1D(float x) { return x * x * (2.0 - x * x); }

/// Taken from https://iquilezles.org/articles/smoothsteps
float SLB_Quintic1D(float x) { return x * x * x * (x * (6.0 * x - 15.0) + 10.0); }

/// Taken from https://iquilezles.org/articles/smoothsteps
float SLB_CubicRational1D(float x) { return x * x * x / (3.0 * x * x - 3.0 * x + 1.0); }

/// Taken from https://iquilezles.org/articles/smoothsteps
float SLB_QuarticRational1D(float x) { return x * x * x * x / (2.0 * x * x * x * x - 4.0 * x * x * x + 6.0 * x * x - 4.0 * x + 1.0); }

/// Taken from somewhere
float SLB_Gaussian1D(float x, float sigma) { return exp(-(x * x) / (2.0 * sigma * sigma)); }

/// Taken from http://www.java-gaming.org/index.php?topic=35123.0
/// Idk how to name it
float4 SLB_Cubic_For_Bicubic(float v)
{
    float4 n = float4(1.0, 2.0, 3.0, 4.0) - v;
    float4 s = n * n * n;
    float x = s.x;
    float y = s.y - 4.0 * s.x;
    float z = s.z - 4.0 * s.y + 6.0 * s.x;
    float w = 6.0 - x - y - z;
    return float4(x, y, z, w) * (1.0 / 6.0);
}

float2 SLB_VogelDisk_Sample(const int sample_index, const int samples_count)
{
    const float r = sqrt(float(sample_index) + 0.5) / sqrt(float(samples_count));
    const float theta = float(sample_index) * SLB_GoldenAngle;

    const float sine = sin(theta), cosine = cos(theta);

    return float2(r * cosine, r * sine);
}

/// Taken from https://extremelearning.com.au/unreasonable-effectiveness-of-quasirandom-sequences
float SLB_R1(float n)
{
    const float g = 1.6180339887498948482;
    const float a1 = 1.0 - 1.0 / g;
    float _n = floor(n);
    return frac(0.5 + a1 * _n);
}

/// Taken from https://fgiesen.wordpress.com/2009/12/13/decoding-morton-codes
uint SLB_Part1By1(uint x)
{
    x &= 0x0000ffffu;
    x = (x ^ (x << 8u)) & 0x00ff00ffu;
    x = (x ^ (x << 4u)) & 0x0f0f0f0fu;
    x = (x ^ (x << 2u)) & 0x33333333u;
    x = (x ^ (x << 1u)) & 0x55555555u;
    return x;
}

/// Taken from https://fgiesen.wordpress.com/2009/12/13/decoding-morton-codes
uint SLB_EncodeMorton2(uint x, uint y) { return (SLB_Part1By1(y) << 1u) + SLB_Part1By1(x); }

/// From https://psychopath.io post
uint SLB_OwenHash4(uint n, uint seed)
{
    n ^= n * 0x3d20adeau;
    n ^= (n >> 1u) & (n << 1u) & 0x55555555u;
    n += seed;
    n *= (seed >> 16u) | 1u;
    n ^= (n >> 1u) & (n << 1u) & 0x55555555u;
    n ^= n * 0x05526c56u;
    n ^= n * 0x53a22864u;
    return n;
}

/// Inspired by https://www.shadertoy.com/view/ssBBW1 shader and https://psychopath.io posts
uint SLB_Morton_OwenHash4_Bluenoise(uint2 coord)
{
    uint m;
    m = SLB_EncodeMorton2(coord.x, coord.y);
    m = SLB_OwenHash4(reversebits(m), 0xe7843fbfu); // owen-scramble morton index
    m = SLB_OwenHash4(reversebits(m), 0x8d8fb1e0u); // map morton index to sobol sequence and owen-scramble
    return reversebits(m); // convert to float
}

float SLB_Morton_OwenHash4_Bluenoise_float(uint2 coord) { return SLB_ToFloat1(SLB_Morton_OwenHash4_Bluenoise(coord)); }

/// Taken from https://www.shadertoy.com/view/wltSDn
uint SLB_PhiNoise(uint2 uv)
{
    // flip every other tile to reduce anisotropy
    if (((uv.x ^ uv.y) & 4u) == 0u)
        uv = uv.yx;

    // constants of 2d Roberts sequence rounded to nearest primes
    const uint r0 = 3242174893u; // prime[(2^32-1) / phi_2  ]
    const uint r1 = 2447445397u; // prime[(2^32-1) / phi_2^2]

    // h = high-freq dither noise
    uint h = (uv.x * r0) + (uv.y * r1);

    // l = low-freq white noise
    uv = uv >> 2u; // 3u works equally well (I think)
    uint l = ((uv.x * r0) ^ (uv.y * r1)) * r1;

    // combine low and high
    return l + h;
}

float SLB_PhiNoise_float(uint2 uv) { return SLB_ToFloat1(SLB_PhiNoise(uv)); }

/// Taken from https://www.shadertoy.com/view/wltSDn
uint SLB_PhiNoise2(uint2 uv)
{
    uint2 uv0 = uv;
    // flip every other tile to reduce anisotropy
    if (((uv.x ^ uv.y) & 4u) == 0u)
        uv = uv.yx;
    if (((uv.x) & 4u) == 0u)
        uv.x = -uv.x; // more iso but also more low-freq content

    // constants of 2d Roberts sequence rounded to nearest primes
    const uint r0 = 3242174893u; // prime[(2^32-1) / phi_2  ]
    const uint r1 = 2447445397u; // prime[(2^32-1) / phi_2^2]

    // h = high-freq dither noise
    uint h = (uv.x * r0) + (uv.y * r1);

    uint l;
    {
        uv = uv0 >> 2u;

        if (((uv.x ^ uv.y) & 4u) == 0u)
            uv = uv.yx;
        if (((uv.x) & 4u) == 0u)
            uv.x = -uv.x;

        uint h = (uv.x * r0) + (uv.y * r1);
        h = h ^ 0xE2E17FDCu;

        l = h;

        {
            uv = uv0 >> 4u;
            if (((uv.x ^ uv.y) & 4u) == 0u)
                uv = uv.yx;
            if (((uv.x) & 4u) == 0u)
                uv.x = -uv.x;

            uint h = (uv.x * r0) + (uv.y * r1);
            h = h ^ 0x1B98264Du;

            l += h;
        }
    }

    // combine low and high
    return l + h;
}

float SLB_PhiNoise2_float(uint2 uv) { return SLB_ToFloat1(SLB_PhiNoise2(uv)); }

/// Taken from paper named "Hash Functions for GPU Rendering"
uint4 SLB_PCG4D(uint4 v)
{
    v = v * 1664525u + 1013904223u;
    v.x += v.y * v.w;
    v.y += v.z * v.x;
    v.z += v.x * v.y;
    v.w += v.y * v.z;
    v ^= v >> 16u;
    v.x += v.y * v.w;
    v.y += v.z * v.x;
    v.z += v.x * v.y;
    v.w += v.y * v.z;
    return v;
}

/// Taken from https://www.pcg-random.org
uint SLB_PCG(uint v)
{
    uint state = v * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

#endif // SLB_COMMON_H

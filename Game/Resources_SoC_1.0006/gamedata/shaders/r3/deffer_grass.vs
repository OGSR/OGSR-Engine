#include "common.h"

float4 consts; // {1/quant,1/quant,diffusescale,ambient}
float4 wave; // cx,cy,cz,tm
float4 dir2D;
float4 array[61 * 4];

v2p_bumped main(v_detail v)
{
    v2p_bumped O;
    // index
    int i = v.misc.w;
    float4 m0 = array[i + 0];
    float4 m1 = array[i + 1];
    float4 m2 = array[i + 2];
    float4 c0 = array[i + 3];

    // Transform pos to world coords
    float4 pos;
    pos.x = dot(m0, v.pos);
    pos.y = dot(m1, v.pos);
    pos.z = dot(m2, v.pos);
    pos.w = 1;

    // Wave effect
    float base = m1.w;
    float dp = calc_cyclic(dot(pos, wave));
    float H = pos.y - base; // height of vertex (scaled)
    float frac = v.misc.z * consts.x; // fractional
    float inten = H * dp;
    float2 result = calc_xz_wave(dir2D.xz * inten, frac);

    // Pos + Wave Result
    pos = float4(pos.x + result.x, pos.y, pos.z + result.y, 1);

    // FLORA FIXES & IMPROVEMENTS - SSS Update 14.6
    // https://www.moddb.com/mods/stalker-anomaly/addons/screen-space-shaders/

    // Fake Normal, Bi-Normal and Tangent
    float3 N = normalize(float3(pos.x - m0.w, pos.y - m1.w + 1.0f, pos.z - m2.w));

    float3x3 xform = mul((float3x3)m_WV, float3x3(0, 0, N.x, 0, 0, N.y, 0, 0, N.z));

    // Feed this transform to pixel shader
    O.M1 = xform[0];
    O.M2 = xform[1];
    O.M3 = xform[2];

    // Eye-space pos/normal
    float hemi = clamp(c0.w, 0.05f, 1.0f); // Some spots are bugged ( Full black ), better if we limit the value till a better solution. // Option -> v_hemi(N);
    float3 Pe = mul(m_V, pos);
    O.tcdh = float4((v.misc * consts).xyyy);
    O.hpos = mul(m_VP, pos);
    O.position = float4(Pe, hemi);

#if defined(USE_R2_STATIC_SUN) && !defined(USE_LM_HEMI)
    O.tcdh.w = hemi * c_sun.x + c_sun.y; // (,,,dir-occlusion)
#endif

    return O;
}
FXVS;

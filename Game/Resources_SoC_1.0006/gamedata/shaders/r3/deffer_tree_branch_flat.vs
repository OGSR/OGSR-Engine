#define USE_TDETAIL

#include "common.h"
#include "check_screenspace.h"

float4 benders_pos[32];
float4 benders_setup;

uniform float3x4 m_xform;
uniform float3x4 m_xform_v;
uniform float4 consts; // {1/quant,1/quant,???,???}
uniform float4 c_scale, c_bias, wind, wave;
uniform float2 c_sun; // x=*, y=+

v2p_flat main(v_tree I)
{
    I.Nh = unpack_D3DCOLOR(I.Nh);
    I.T = unpack_D3DCOLOR(I.T);
    I.B = unpack_D3DCOLOR(I.B);

    v2p_flat o;

    // Transform to world coords
    float3 pos = mul(m_xform, I.P);

    //
    float base = m_xform._24; // take base height from matrix
    float dp = calc_cyclic(wave.w + dot(pos, (float3)wave));
    float H = pos.y - base; // height of vertex (scaled, rotated, etc.)
    float frac = I.tc.z * consts.x; // fractional (or rigidity)
    float inten = H * dp; // intensity
    float2 result = calc_xz_wave(wind.xz * inten, frac);
#ifdef USE_TREEWAVE
    result = 0;
#endif
    float4 w_pos = float4(pos.x + result.x, pos.y, pos.z + result.y, 1);
    float2 tc = (I.tc * consts).xy;

    // INTERACTIVE GRASS ( Bushes ) - SSS Update 15.4
    // https://www.moddb.com/mods/stalker-anomaly/addons/screen-space-shaders/
#ifdef SSFX_INTER_GRASS
    for (int b = 0; b < benders_setup.w; b++)
    {
        // Direction, Radius & Bending Strength, Distance and Height Limit
        float3 dir = benders_pos[b + 16].xyz;
        float3 rstr = float3(benders_pos[b].w, benders_pos[b + 16].ww); // .x = Radius | .yz = Str
        bool non_dynamic = rstr.x <= 0 ? true : false;
        float dist = distance(w_pos.xz, benders_pos[b].xz);
        float height_limit = 1.0f - saturate(abs(pos.y - benders_pos[b].y) / (non_dynamic ? 2.0f : rstr.x));
        height_limit *= (1.0f - tc.y); // Bushes uses UV Coor instead of H to limit displacement

        // Adjustments ( Fix Radius or Dynamic Radius )
        rstr.x = non_dynamic ? benders_setup.x : rstr.x;
        rstr.yz *= non_dynamic ? benders_setup.yz : 1.0f;

        // Strength through distance and bending direction.
        float bend = 1.0f - saturate(dist / (rstr.x + 0.001f));
        float3 bend_dir = normalize(w_pos.xyz - benders_pos[b].xyz) * bend;
        float3 dir_limit = dir.y >= -1 ? saturate(dot(bend_dir.xyz, dir.xyz) * 5.0f) : 1.0f; // Limit if nedeed

        // Apply direction limit
        bend_dir.xz *= dir_limit.xz;

        // Apply vertex displacement
        w_pos.xz += bend_dir.xz * 2.25f * rstr.yy * height_limit; // Horizontal
        w_pos.y -= bend * 0.67f * rstr.z * height_limit * dir_limit.y; // Vertical
    }
#endif

    // Final xform(s)
    // Final xform
    float3 Pe = mul(m_V, w_pos);
    float hemi = I.Nh.w * c_scale.w + c_bias.w;
    // float 	hemi 	= I.Nh.w;
    o.hpos = mul(m_VP, w_pos);
    o.N = mul((float3x3)m_xform_v, unpack_bx2(I.Nh));
    o.tcdh = float4((I.tc * consts).xyyy);
    o.position = float4(Pe, hemi);

#if defined(USE_R2_STATIC_SUN) && !defined(USE_LM_HEMI)
    float suno = I.Nh.w * c_sun.x + c_sun.y;
    o.tcdh.w = suno; // (,,,dir-occlusion)
#endif

#ifdef USE_GRASS_WAVE
    o.tcdh.z = 1.f;
#endif

#ifdef USE_TDETAIL
    o.tcdbump = o.tcdh * dt_params; // dt tc
#endif

    return o;
}
FXVS;

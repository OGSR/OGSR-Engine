/**
 * @ Version: SCREEN SPACE SHADERS - UPDATE 19
 * @ Description: Trees - Trunk
 * @ Modified time: 2023-12-16 13:58
 * @ Author: https://www.moddb.com/members/ascii1457
 * @ Mod: https://www.moddb.com/mods/stalker-anomaly/addons/screen-space-shaders
 */

#include "common.h"
#include "check_screenspace.h"

#ifdef SSFX_WIND
#include "screenspace_wind.h"
#endif

v2p_bumped main(v_tree I)
{
    float3x4 m_xform = float3x4(I.m0, I.m1, I.m2);
    float4 consts = I.consts;

    I.Nh = unpack_D3DCOLOR(I.Nh);
    I.T = unpack_D3DCOLOR(I.T);
    I.B = unpack_D3DCOLOR(I.B);

    // Transform to world coords
    float3 pos = mul(m_xform, I.P);
    float H = pos.y - m_xform._24; // height of vertex

#if !defined(SSFX_WIND) || defined(DISABLE_WIND)
    float4 w_pos = float4(pos.xyz, 1);
    float4 w_pos_previous = w_pos;
#else
    wind_setup wset = ssfx_wind_setup();
    float2 wind_result = ssfx_wind_tree_trunk(m_xform, pos, H, wset).xy;
    float4 w_pos = float4(pos.x + wind_result.x, pos.y, pos.z + wind_result.y, 1);

    wind_setup wset_old = ssfx_wind_setup(true);
    float2 wind_result_old = ssfx_wind_tree_trunk(m_xform, pos, H, wset_old, true).xy;
    float4 w_pos_previous = float4(pos.x + wind_result_old.x, pos.y, pos.z + wind_result_old.y, 1);
#endif

    float2 tc = (I.tc * consts).xy;
    float hemi = clamp(I.Nh.w * consts.z + consts.w, 0.3f, 1.0f); // Limit hemi - SSS Update 14.5

    // Eye-space pos/normal
    v2p_bumped O;
    float3 Pe = mul(m_V, w_pos);
    O.tcdh = float4(tc.xyyy);
    O.hpos = mul(m_VP, w_pos);

    //////////
    O.hpos_old = mul(m_VP_old, w_pos_previous);
    O.hpos_curr = O.hpos;
    O.hpos.xy = get_taa_jitter(O.hpos);
    /////////////

    O.position = float4(Pe, hemi);

    // FLORA FIXES & IMPROVEMENTS - SSS Update 14
    // https://www.moddb.com/mods/stalker-anomaly/addons/screen-space-shaders/
    // Use real tree Normal, Tangent and Binormal.
    float3 N = unpack_bx4(I.Nh);
    float3 T = unpack_bx4(I.T);
    float3 B = unpack_bx4(I.B);

    float3x3 m_xform_v = mul((float3x3)m_V, (float3x3)m_xform);
    float3x3 xform = mul(m_xform_v, float3x3(T.x, B.x, N.x, T.y, B.y, N.y, T.z, B.z, N.z));

    // The pixel shader operates on the bump-map in [0..1] range
    // Remap this range in the matrix, anyway we are pixel-shader limited :)
    // ...... [ 2  0  0  0]
    // ...... [ 0  2  0  0]
    // ...... [ 0  0  2  0]
    // ...... [-1 -1 -1  1]
    // issue: strange, but it's slower :(
    // issue: interpolators? dp4? VS limited? black magic?

    // Feed this transform to pixel shader
    O.M1 = xform[0];
    O.M2 = xform[1];
    O.M3 = xform[2];

#ifdef USE_TDETAIL
    O.tcdbump = O.tcdh * dt_params; // dt tc
#endif

    return O;
}

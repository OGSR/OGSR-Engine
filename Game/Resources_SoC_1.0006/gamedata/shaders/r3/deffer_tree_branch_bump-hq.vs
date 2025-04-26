/**
 * @ Version: SCREEN SPACE SHADERS - UPDATE 19
 * @ Description: Trees - Branches/Bushes
 * @ Modified time: 2023-12-16 13:53
 * @ Author: https://www.moddb.com/members/ascii1457
 * @ Mod: https://www.moddb.com/mods/stalker-anomaly/addons/screen-space-shaders
 */

#include "common.h"
#include "check_screenspace.h"

cbuffer dynamic_inter_grass
{
    float4 benders_pos[32];
    float4 benders_pos_old[32];
    float4 benders_setup;
}

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
    float2 tc = (I.tc * consts).xy;

#ifndef SSFX_WIND
    float4 w_pos = float4(pos.xyz, 1);
    float4 w_pos_previous = w_pos;
#else
    wind_setup wset = ssfx_wind_setup();
    float3 wind_result = ssfx_wind_tree_branches(m_xform, pos, H, tc.y, wset);
    float4 w_pos = float4(pos.xyz + wind_result.xyz, 1);

    wind_setup wset_old = ssfx_wind_setup(true);
    float3 wind_result_old = ssfx_wind_tree_branches(m_xform, pos, H, tc.y, wset_old, true);
    float4 w_pos_previous = float4(pos.xyz + wind_result_old.xyz, 1);
#endif

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

    // for old frame
    for (int b = 0; b < benders_setup.w; b++)
    {
        // Direction, Radius & Bending Strength, Distance and Height Limit
        float3 dir = benders_pos_old[b + 16].xyz;
        float3 rstr = float3(benders_pos_old[b].w, benders_pos_old[b + 16].ww); // .x = Radius | .yz = Str
        bool non_dynamic = rstr.x <= 0 ? true : false;
        float dist = distance(w_pos_previous.xz, benders_pos_old[b].xz);
        float height_limit = 1.0f - saturate(abs(pos.y - benders_pos_old[b].y) / (non_dynamic ? 2.0f : rstr.x));
        height_limit *= (1.0f - tc.y); // Bushes uses UV Coor instead of H to limit displacement

        // Adjustments ( Fix Radius or Dynamic Radius )
        rstr.x = non_dynamic ? benders_setup.x : rstr.x;
        rstr.yz *= non_dynamic ? benders_setup.yz : 1.0f;

        // Strength through distance and bending direction.
        float bend = 1.0f - saturate(dist / (rstr.x + 0.001f));
        float3 bend_dir = normalize(w_pos_previous.xyz - benders_pos_old[b].xyz) * bend;
        float3 dir_limit = dir.y >= -1 ? saturate(dot(bend_dir.xyz, dir.xyz) * 5.0f) : 1.0f; // Limit if nedeed

        // Apply direction limit
        bend_dir.xz *= dir_limit.xz;

        // Apply vertex displacement
        w_pos_previous.xz += bend_dir.xz * 2.25f * rstr.yy * height_limit; // Horizontal
        w_pos_previous.y -= bend * 0.67f * rstr.z * height_limit * dir_limit.y; // Vertical
    }
#endif

    float hemi = clamp(I.Nh.w * consts.z + consts.w, 0.3f, 1.0f); // Limit hemi - SSS Update 14.5
    // float hemi = I.Nh.w;

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

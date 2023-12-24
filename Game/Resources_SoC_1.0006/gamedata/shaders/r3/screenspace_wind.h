/**
 * @ Version: SCREEN SPACE SHADERS - UPDATE 19
 * @ Description: Wind Main File
 * @ Modified time: 2023-11-23 21:02
 * @ Author: https://www.moddb.com/members/ascii1457
 * @ Mod: https://www.moddb.com/mods/stalker-anomaly/addons/screen-space-shaders
 */

// wind_direction wind_velocity E.m_fTreeAmplitudeIntensity
uniform float4 wind_params;

uniform float4 ssfx_wind; // DBG: X: Wind intensity - Y: North Dir - Z: South Dir
uniform float4 ssfx_wind_gust; // X: Intensity
uniform float4 ssfx_wind_anim;

uniform float is_bugged_flora;

Texture2D s_waves;
sampler smp_linear2;

struct wind_setup
{
    float2 direction;
    float speed;
    float sqrt_speed;

    float trees_animspeed;
    float trees_trunk_animspeed;
    float trees_bend;

    float grass_animspeed;
    float grass_turbulence;
    float grass_push;
    float grass_wave;
};

wind_setup ssfx_wind_setup()
{
    // wind : [x:time] [y:TreeAmplitudeIntensity] [z:gust] [w:ExtraPerlin]
    wind_setup wsetup;

    // Direction, speed and wind gust.
    wsetup.direction = normalize(ssfx_wind.yz);

    if (ssfx_wind.y == 0 && ssfx_wind.z == 0)
    {
        // Radians to Vector
        float r = -wind_params.x + 1.57079f;
        wsetup.direction = float2(cos(r), sin(r));
    }

    wsetup.speed = saturate(ssfx_wind.x);

    if (wsetup.speed <= 0)
        wsetup.speed = saturate(wind_params.y * 0.001);

    wsetup.sqrt_speed = saturate(sqrt(wsetup.speed * 1.66f));

    wsetup.trees_animspeed = 11.0f;
    wsetup.trees_trunk_animspeed = 0.15f;
    wsetup.trees_bend = 0.5f;

    wsetup.grass_animspeed = 10.0;
    wsetup.grass_turbulence = 1.5f;
    wsetup.grass_push = 1.5f;
    wsetup.grass_wave = 0.2f;

    return wsetup;
}

#ifdef SSFX_WIND_ISGRASS

// Flow Map - X: X-Anim | Y: Z-Anim | Z: Wind Wave | W : Detail

float3 ssfx_wind_grass(float3 pos, float H, wind_setup W)
{
    // Height Limit. ( Add stiffness to tall grass )
    float HLimit = saturate(H * H - 0.01f) * saturate(1.0f - H * 0.1f);

    // Offset animation
    float2 Offset = -ssfx_wind_anim.xy * W.grass_animspeed;

    // Sample ( The scale defines the detail of the motion )
    float3 Flow = s_waves.SampleLevel(smp_linear2, (pos.xz + Offset) * 0.018f, 0) * W.sqrt_speed;

    // Grass Motion ( -1.0 ~ 1.0 ). Turbulence.
    float2 GrassMotion = (Flow.xy * 2.0f - 1.0f) * W.sqrt_speed * W.grass_turbulence;

    // Apply wind direction and flow. Wind push.
    float2 WindDir = Flow.z * W.sqrt_speed * W.direction * W.grass_push;

    // Add everything and apply height limit
    float3 Final = float3(GrassMotion.x + WindDir.x, Flow.z * W.grass_wave, GrassMotion.y + WindDir.y) * HLimit;

    return Final;
}

#else // Non Grass

float3 ssfx_wind_tree_trunk(float3 pos, float Tree_H, wind_setup W)
{
    // Phase ( from matrix ) + Offset
    float Phase = m_xform._24 + ssfx_wind_anim.z * W.trees_trunk_animspeed;

    // Trunk wave
    float TWave = (cos(Phase) * sin(Phase * 5.0f) + 0.5f) * ssfx_wind_gust.x * W.trees_bend;

    float WSpeed = saturate(W.sqrt_speed * 1.5f);

    // Base wind speed displacement
    float Base_Bend = WSpeed * 0.007f * saturate(1.0f - Tree_H * 0.005f);

    // Intensity ( Squared height )
    Base_Bend *= Tree_H * Tree_H * TWave * WSpeed;

    // Apply direction
    float2 Final = Base_Bend.xx * W.direction;

    return float3(Final, saturate((TWave + 1.0f) * 0.5));
}

float3 ssfx_wind_tree_branches(float3 pos, float Tree_H, float tc_y, wind_setup W)
{
    // UV Offset
    float2 Offset = -ssfx_wind_anim.xy * W.trees_animspeed;

    // Sample flow map
    float3 Flow = s_waves.SampleLevel(smp_linear2, (pos.xz + Offset) * 0.02f, 0);

    // Sample 2, slower and detailed
    float3 Flow2 = s_waves.SampleLevel(smp_linear2, (pos.xz + Offset * 0.2f) * 0.1f, 0);

    // Branch motion [ -1.0 ~ 1.0 ]
    float3 branchMotion = 0;

    // Base Motion
    if (!is_bugged_flora)
        branchMotion.xyz = float3(Flow.x, Flow2.y, Flow.y) * 2.0f - 1.0f;

    // Trunk position
    float3 Trunk = ssfx_wind_tree_trunk(pos, Tree_H, W);

    // Gust from trunk data.
    branchMotion.xz *= Trunk.z * clamp(Tree_H * 0.1f, 1.0f, 2.5f);

    // Add wind direction
    if (!is_bugged_flora)
        branchMotion.xz += Flow2.z * W.direction;

    // Add wind gust
    branchMotion.y *= saturate(Tree_H * 0.1f);

    // Everything is limited by the UV and wind speed
    if (!is_bugged_flora)
        branchMotion *= (1.0f - tc_y) * W.speed;

    // Add trunk animation
    branchMotion.xz += Trunk.xy;

    return branchMotion;
}

#endif
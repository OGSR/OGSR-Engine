#include "ogse_functions.h"

Texture2D s_noise;

#define uGhostCount 7
#define uGhostSpacing 0.35
#define uGhostThreshold 0.7
#define uHaloRadius 0.3
#define uHaloThickness 0.165

float4 blendSoftLight(float4 a, float4 b)
{
    float4 c = 2 * a * b + a * a * (1 - 2 * b);
    float4 d = sqrt(a) * (2 * b - 1) + 2 * a * (1 - b);

    return (b < 0.5) ? c : d;
}

float Window_Cubic(float x, float center, float radius)
{
    x = min(abs(x - center) / radius, 1.0);
    return 1.0 - x * x * (3.0 - 2.0 * x);
}

float4 get_sun_uv()
{
    // Dist to the sun
    float sun_dist = 100 / (sqrt(1.0f - L_sun_dir_w.y * L_sun_dir_w.y));

    // Sun pos
    float3 sun_pos_world = sun_dist * L_sun_dir_w + eye_position;
    float4 sun_pos_projected = mul(m_VP, float4(sun_pos_world, 1.0f));
    float4 sun_pos_screen = proj_to_screen(sun_pos_projected) / sun_pos_projected.w;

    return sun_pos_projected.w <= 0.0f ? sun_pos_screen : float4(-1.0f, -1.0f, -1.0f, -1.0f);
}

float4 mainImageA(float2 uv)
{
    uv -= 0.5;
    uv *= float2(screen_res.x * screen_res.w, 1.0);

    float2 circle_pos = get_sun_uv().xy;
    circle_pos -= 0.5;
    circle_pos *= float2(screen_res.x * screen_res.w, 1.0);

    float circle = smoothstep(0.02, 0.0, length(uv + circle_pos));
    return float4(1.0, 0.656, 0.3, 1.0) * circle;
}

float generate_starburst(float2 uv)
{
    uv *= float2(screen_res.x / screen_res.y, 1.0).xy;
    float angle = atan(uv.y / uv.x);
    float2 sb_uv = float2(cos(angle), sin(angle)) / 3.14;
    float sb_tex = s_noise.Sample(smp_linear, sb_uv * 64.).x;
    return smoothstep(0.0, sb_tex, length(uv / 4.)) * length(uv / 2.); // soften it a little bit
}

// generate ghosts n shit
float3 generate_ghosts(float2 uv)
{
    // Draw multiple 'ghosts'
    float3 accumulated_ghosts = float3(0.0, 0.0, 0.0);
    {
        uv = 1.0 - uv;
        float2 ghostVec = (0.5 - uv) * uGhostSpacing;

        for (int i = 0; i < uGhostCount; ++i)
        {
            float2 suv = frac(uv + ghostVec * float2(i, i));
            float ghost_intensity = float(i) / float(uGhostCount);
            ghost_intensity = pow(ghost_intensity, 2.0); // so each subsequent ghost has different intensity
            float d = distance(suv, 0.5);
            float weight = 1.0 - smoothstep(0.0, 0.15, d);
            accumulated_ghosts += mainImageA(suv).xyz * weight;
        }
    }

    // Create simple halo
    float3 accumulated_halo = float3(0.0, 0.0, 0.0);
    {
        float2 haloVec = 0.5 - uv;
        haloVec.x /= screen_res.y / screen_res.x;
        haloVec = normalize(haloVec);
        haloVec.x *= screen_res.y / screen_res.x;
        float2 wuv = (uv - float2(0.5, 0.0)) / float2(screen_res.y / screen_res.x, 1.0) + float2(0.5, 0.0);
        float d = distance(wuv, 0.5);
        float haloWeight = Window_Cubic(d, uHaloRadius, uHaloThickness); // cubic window function
        haloVec *= uHaloRadius;
        accumulated_halo += mainImageA(uv + haloVec).xyz * haloWeight;

        // add starburst
        accumulated_halo *= generate_starburst(uv);
    }

    // Add all shit together
    return (accumulated_ghosts + accumulated_halo);
}

float4 mainImageB(float2 uv)
{
    float3 col = generate_ghosts(uv);
    return float4(col, 1.0);
}

float4 mainImageC(float2 uv)
{
    float ca_offset = distance(uv, 0.5) * 0.0085;
    float3 col;
    col.x = mainImageB(uv + ca_offset).x;
    col.y = mainImageB(uv).y;
    col.z = mainImageB(uv - ca_offset).z;

    return float4(col, 1.0);
}

float4 sun_shafts_intensity;

float4 generate_flare(float2 uv, float blend)
{
    float3 col = mainImageC(uv).xyz;

    col /= 1.0 + col;
    col = pow(col, float3(0.5, 0.5, 0.5));

    float4 final = float4(col * (L_sun_color * 0.2), 1.0);
    final = lerp(float4(0.f, 0.f, 0.f, 0.f), final, blend);

    return final;
}
#ifndef INFRARED
#define INFRARED

#include "common.h"
#include "night_vision.h"

#ifdef USE_MSAA
#ifndef SM_5
Texture2DMS<float4, MSAA_SAMPLES> s_heat;
#else
Texture2DMS<float4> s_heat;
#endif
#else
Texture2D s_heat;
#endif

uniform float4 heat_vision_steps;
uniform float4 heat_vision_blurring;
uniform float4 heat_fade_distance;

///////////////////////////////////////////////////////
// COLOR SETTINGS HERE
///////////////////////////////////////////////////////

//#define FADE_DISTANCE_START 13.0   // 13.0
//#define FADE_DISTANCE_END 20.0     // 20.0

#define COLOR_FAR_MIN float3(0.0, 0.03, 0.07) // 0.07
#define COLOR_FAR_MAX float3(0.0, 0.03, 0.15) // 0.15

#define COLOR_DBLUE float3(0.0, 0.10, 0.35)
#define COLOR_BLUE float3(0.0, 0.10, 0.5)
#define COLOR_LBLUE float3(0.0, 0.74, 1.0)
#define COLOR_GREEN float3(0.4, 1.0, 0.6)
#define COLOR_YELLOW float3(1.0, 0.8, 0.0)
#define COLOR_RED float3(1.0, 0.2, 0.0)

#define COLOR_BLACK float3(0.0, 0.0, 0.0)
#define COLOR_WHITE float3(1.0, 1.0, 1.0)

#define color_background_min COLOR_DBLUE
#define color_background_max COLOR_BLUE

#define color_gradient_very_low color_background_max
#define color_gradient_low COLOR_GREEN
#define color_gradient_warm COLOR_YELLOW
#define color_gradient_hot COLOR_RED

float3 normal_blur(float2 pos2d, int samples)
{
    float3 accum = (0.0, 0.0, 0.0);

    for (int i = -samples; i < samples; i++)
    {
        for (int j = -samples; j < samples; j++)
        {
            accum += gbuf_unpack_normal(s_position.Load(int3(pos2d + float2(i, j), 0), 0).xy);
        }
    }

    accum /= (samples * samples * 4);
    return accum;
}

float3 greyscale(float3 img)
{
    float Y = 0.6 * img.x + 0.5 * img.y + 0.55 * img.z;
    return float3(Y, Y, Y);
}

float3 infrared(gbuffer_data gbd, float3 original, float depth, float2 HPos, float2 Tex0)
{
    // r_pnv_mode = 0 - disable
    // r_pnv_mode = 1 - normal pnv
    // r_pnv_mode = 2 - color heatvision
    // r_pnv_mode = 3 - b/w heatvision

    float heat_mode = pnv_param_1.z - 2.f;

    float3 hotness = s_heat.Load(int3(Tex0 * screen_res.xy, 0), 0);
    float3 mixed;

    if (hotness.y > 0.0)
    {
        float3 blur_2 = s_blur_2.Sample(smp_base, Tex0).rgb;
        float luminance = dot(blur_2, float3(0.299f, 0.587f, 0.114f));

        luminance *= hotness.y;

        if (luminance >= 0.4)
        {
            mixed = color_gradient_hot;
        }
        else if (luminance >= 0.3 && luminance < 0.4)
        {
            mixed = color_gradient_warm;
        }
        else if (luminance >= 0.1 && luminance < 0.3)
        {
            mixed = color_gradient_low;
        }
        else
        {
            float projection = dot(normalize(gbd.N), float3(0.0, 0.0, -1.0));
            mixed = lerp(color_background_min, color_background_max, projection);
        }
    }
    else if (hotness.x > 0.0)
    {
        int samples = lerp(heat_vision_blurring.x, heat_vision_blurring.y, smoothstep(0.0, heat_vision_blurring.z, depth));

        mixed = normal_blur(HPos, samples);
        mixed = normalize(mixed);
        float projection = dot(mixed, float3(0.0, 0.0, -1.0));

        if (projection <= 0.f)
        {
            mixed = color_background_max;
        }
        else
        {
            float step1 = heat_vision_steps.x;
            float step2 = heat_vision_steps.y;
            float step3 = heat_vision_steps.z;

            if (projection > 0.0 && projection < step1)
            {
                mixed = color_gradient_very_low;
            }
            else if (projection >= step1 && projection < step2)
            {
                mixed = lerp(color_gradient_very_low, color_gradient_low, smoothstep(step1, step2, projection));
            }
            else if (projection >= step2 && projection < step3)
            {
                mixed = lerp(color_gradient_low, color_gradient_warm, smoothstep(step2, step3, projection));
            }
            else if (projection >= step3 && projection <= 1.0)
            {
                mixed = lerp(color_gradient_warm, color_gradient_hot, smoothstep(step3, 1.0, projection));
            }
        }

        mixed = lerp(color_background_max, mixed, hotness.x);
    }
    else // only for particle_hard.ps
        if (hotness.z > 0.0)
        {
            mixed = lerp(color_background_min, color_gradient_low, hotness.z);
        }
        else
        {
            float projection = dot(normalize(gbd.N), float3(0.0, 0.0, -1.0));

            float FADE_DISTANCE_START = heat_fade_distance.x; // shader_param_fade_distance.x; // 5.0f;   // 13.0
            float FADE_DISTANCE_END = heat_fade_distance.y; // shader_param_fade_distance.y; // 10.0f;     // 20.0

            if (depth <= 0)
            {
                depth = FADE_DISTANCE_END;
            }

            if (depth < FADE_DISTANCE_START)
            {
                mixed = lerp(color_background_min, color_background_max, projection);
            }
            else
            {
                float arg = smoothstep(FADE_DISTANCE_START, FADE_DISTANCE_END, clamp(depth, FADE_DISTANCE_START, FADE_DISTANCE_END));
                float3 min_color = lerp(color_background_min, COLOR_FAR_MIN - heat_mode * float3(0.0, 0.0, 0.03), arg);
                float3 max_color = lerp(color_background_max, COLOR_FAR_MAX - heat_mode * float3(0.0, 0.0, 0.05), arg);
                mixed = lerp(min_color, max_color, projection);
            }
        }

    float4 jitter = float4(frac(sin(dot(Tex0, float2(12.0, 78.0) + (timers.x))) * 12345.0), frac(sin(dot(Tex0, float2(12.0, 78.0) + (timers.x))) * 67890.0),
                           frac(sin(dot(Tex0, float2(12.0, 78.0) + (timers.x))) * 78372.0), frac(sin(dot(Tex0, float2(12.0, 78.0) + (timers.x))) * 37857.0));

    if (heat_mode == 1.0)
    {
        mixed += jitter.y * (0.12);
        mixed = greyscale(mixed);
    }
    else
    {
        mixed += jitter.y * (0.15);
    }

    return mixed;
}
#endif
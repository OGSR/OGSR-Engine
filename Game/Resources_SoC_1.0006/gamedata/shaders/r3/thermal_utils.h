/*
    =====================================================================
    Original Author  : vegeta1k95
    Link             : https://www.moddb.com/mods/stalker-anomaly/addons/heatvision-v02-extension-for-beefs-nvg-dx11engine-mod
    =====================================================================
*/

Texture2D s_heat;

#define FADE_DISTANCE_START 0.0 // 13.0
#define FADE_DISTANCE_END 100.0 // 20.0

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

float3 infrared(gbuffer_data gbd, float2 HPos, float2 Tex0)
{
    int BW = 1;

    float depth = gbd.P.z;
    float3 hotness = s_heat.Load(int3(Tex0 * screen_res.xy, 0), 0);
    float3 mixed;

    if (hotness.r > 0.0)
    {
        int samples = lerp(15, 4, smoothstep(0.0, 60, depth));

        mixed = normal_blur(HPos, samples);
        mixed = normalize(mixed);
        float projection = dot(mixed, float3(0.0, 0.0, -1.0));

        if (projection <= 0.f)
        {
            mixed = color_background_max;
        }
        else
        {
            mixed = lerp(color_gradient_very_low, color_gradient_hot, smoothstep(0, 1.0, projection));
        }

        mixed = lerp(color_background_max, mixed, hotness.r);
    }
    else if (hotness.g > 0.0)
    {
        float3 blur_2 = s_blur_2.Sample(smp_base, Tex0).rgb;
        float luminance = dot(blur_2, float3(0.299f, 0.587f, 0.114f));

        luminance *= hotness.g;

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
    else if (hotness.b > 0.0)
    {
        mixed = lerp(color_background_min, color_gradient_low, hotness.b);
    }
    else
    {
        float projection = dot(normalize(gbd.N), float3(0.0, 0.0, -1.0));

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
            float3 min_color = lerp(color_background_min, COLOR_FAR_MIN - float3(0.0, 0.0, 0.03), arg);
            float3 max_color = lerp(color_background_max, COLOR_FAR_MAX - float3(0.0, 0.0, 0.05), arg);
            mixed = lerp(min_color, max_color, projection);
        }
    }

    mixed = greyscale(mixed);

    return mixed;
}
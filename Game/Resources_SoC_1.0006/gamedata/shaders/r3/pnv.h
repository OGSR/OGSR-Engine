//////////////////////////////////////////////////////////////////////////////////////////
// NV Shader by LVutner (basing on yaz NV)
// Last edit: 5:12 (22.05.19)
//////////////////////////////////////////////////////////////////////////////////////////

// defines
#define NV_BRIGHTNESS 5.0 // NV_COLOR.w

// effects
#define NV_FLICKERING
#define NV_NOISE
#define NV_VIGNETTE
#define NV_SCANLINES

// effect settings
#define FLICKERING_INTENSITY 0.0015
#define FLICKERING_FREQ 60.0
#define NOISE_INTENSITY 0.15 // NV_PARAMS.x
#define SCANLINES_INTENSITY 0.175 // NV_PARAMS.y
#define VIGNETTE_RADIUS 1.0

float4 vision_effect(float2 tc0, float4 color, float4 NV_COLOR, float4 NV_PARAMS)
{
    float lum = dot(color.rgb, float3(0.3f, 0.38f, 0.22f) * NV_COLOR.w); // instead of float3 use LUMINANCE_floatTOR in stalker
    color.rgb = NV_COLOR.xyz * lum;

    // cheap noise function
    float noise = frac(sin(dot(tc0, float2(12.0, 78.0) + (timers.x * 1.17f))) * 43758.0);

//////////////////////////////////////////////////////////////////////////////////////////
// scanlines
#ifdef NV_SCANLINES
    color += NV_PARAMS.y * sin(tc0.y * screen_res.y * 2.0);
#endif
//////////////////////////////////////////////////////////////////////////////////////////
// noise
#ifdef NV_NOISE
    color += noise * NV_PARAMS.x;
#endif
//////////////////////////////////////////////////////////////////////////////////////////
// screen flickering
#ifdef NV_FLICKERING
    color += FLICKERING_INTENSITY * sin(timers.x * FLICKERING_FREQ);
#endif
//////////////////////////////////////////////////////////////////////////////////////////
// vignette
#ifdef NV_VIGNETTE
    color *= VIGNETTE_RADIUS - (distance(tc0.xy, float2(0.5f, 0.5f)));
    color *= smoothstep(0.55f, 0.4f, distance(tc0.xy, float2(0.5f, 0.5f)));
#endif

    return color;
}

float4 calc_night_vision_effect(float2 tc0, float4 color, float4 NV_COLOR, float4 NV_PARAMS, float r)
{
    if (m_hud_params.x > 0 && r > 0) //
    {
        float2 corrected_texturecoords = aspect_ratio_correction(tc0);
        float2 scope_center = float2(0.5f, 0.5f);

        float distToCenter = distance(corrected_texturecoords, scope_center);

        if (m_hud_params.x >= 1.0 && r > 0 && step(distToCenter, r) == 1)
        {
            return vision_effect(tc0, color, NV_COLOR, NV_PARAMS); // return vision_effect(tc0, color, float4(0.f,1.f,0.f, NV_COLOR.w), NV_PARAMS);
        }

        return color;
    }
    else
    {
        return vision_effect(tc0, color, NV_COLOR, NV_PARAMS);
    }
}

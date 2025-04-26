#ifndef SLB_MOTION_BLUR_H
#define SLB_MOTION_BLUR_H

#include "slb_common.h"
#include "slb_motion_blur_settings.h"

float3 SLB_Screen_GetImage(float2 tc, uint iSample) { return SLB_SAMPLER_LOAD(s_image, int3(tc * screen_res.xy - 0.25, 0)).rgb; }

float4 SLB_Screen_GetPosition(float2 tc, uint iSample)
{
#ifdef SLB_GLSL /// return dummy
    return float4(0.5, 0.5, float(abs(tc.x - 0.5) < 0.25) * 10.0 + 0.01, 0.0);
#else
#ifdef USE_MSAA
    return SLB_SAMPLER_LOAD_MSAA(s_position, int3(tc * screen_res.xy - 0.25, 0), iSample);
#else
    return SLB_SAMPLER_LOAD(s_position, int3(tc * screen_res.xy - 0.25, 0));
#endif
#endif
}

// uniform float4 shader_param_6 = float4(0.9,0,9,0);

float SLB_MBlur_SightMask(float2 tc, float3 img, float2 aspect, uint iSample)
{
    float len = saturate(SLB_MBLUR_SIGHT_MASK_SIZE - length((tc - 0.5) * aspect) * 75.);

#ifdef SLB_MBLUR_SIGHT_MASK_COLOR
#ifdef SLB_MBLUR_SIGHT_MASK_COLOR_SLOWER
    float threshold = SLB_MBLUR_SIGHT_MASK_COLOR_SLOWER_THRESHOLD;
    float mask = 0.0;

    float2 dirs[9] = {float2(0, 0),           float2(1, 0), float2(0, 1), float2(-1, 0), float2(0, -1), float2(0.7071, 0.7071), float2(0.7071, -0.7071), float2(-0.7071, -0.7071),
                      float2(-0.7071, 0.7071)};

    SLB_UNROLL(9)
    for (int i = 0; i < 9; i++)
    {
        float3 s;

        if (i == 0)
        {
            s = img;
        }
        else
        {
            float2 offset = dirs[i] * 1.5 / 1000.0 / aspect;
            s = SLB_Screen_GetImage(tc + offset, iSample);
        }

        float3 hsv = SLB_RGB_To_HSV(s);
        mask += float(hsv.y > threshold && hsv.z > threshold);
        mask += float(abs(hsv.x - 0.406) < 0.01 && hsv.y > 0.5 && hsv.z > 0.5);
        // mask += float(abs(hsv.x - 0.986) < 0.005 && hsv.y > 0.5 && hsv.z > 0.5);
    }

    return 1.0 - len * saturate(mask);
#else
    float threshold = SLB_MBLUR_SIGHT_MASK_COLOR_THRESHOLD;
    float mask = 0.0;

    mask += step(threshold, img.r - (img.g + img.b));
    mask += step(threshold, img.g - (img.r + img.b));

    return 1.0 - len * saturate(mask);
#endif
#else
    return 1.0 - len;
#endif
}

float SLB_MBlur_WeaponMask(float3 pos) { return saturate((length(pos) - SLB_MBLUR_WPN_RADIUS) / SLB_MBLUR_WPN_RADIUS_SMOOTHING); }

float3 SLB_MBlur(float2 uv, float3 pos, float3 img, float2 pixel_velocity, uint iSample)
{
    float2 aspect = screen_res.x > screen_res.y ? float2(screen_res.x / screen_res.y, 1.) : float2(1., screen_res.y / screen_res.x);

#ifdef SLB_MBLUR_REVERSED
    pixel_velocity = -pixel_velocity;
#endif

    pixel_velocity *= SLB_MBLUR_LENGTH;
    float2 pixel_velocity_clamp = abs(normalize(pixel_velocity)) * SLB_MBLUR_CLAMP / aspect;
    pixel_velocity = clamp(pixel_velocity, -pixel_velocity_clamp, +pixel_velocity_clamp);

#ifdef SLB_MBLUR_WPN
    pixel_velocity *= SLB_MBlur_WeaponMask(pos);
#endif

#ifdef SLB_MBLUR_SIGHT_MASK
    pixel_velocity *= SLB_MBlur_SightMask(uv, img, aspect, iSample);
#endif

    /// Accumulate motion blur samples
    float3 accum = img * 1e-2;
    float occ = 1e-2;

    uint noise_offset = 0;

#ifdef SLB_MBLUR_ANIMATED_DITHER
    noise_offset = SLB_PCG(asuint(timers.y));
#endif

    float noise = SLB_PhiNoise2_float(uint2(uv * screen_res.xy) + uint2(noise_offset, noise_offset));
#ifdef SLB_MBLUR_DUAL
    float dither = noise - 0.5;
#else
    float dither = noise;
#endif

    float2 p_velocity_orig = pixel_velocity;

    // gimme ze blur
    SLB_UNROLL(SLB_MBLUR_SAMPLES * 2 + 1)
#ifdef SLB_MBLUR_DUAL
    for (int i = -SLB_MBLUR_SAMPLES; i <= SLB_MBLUR_SAMPLES; i++)
    {
#else
    for (int i = 0; i <= SLB_MBLUR_SAMPLES * 2; i++)
    {
#endif

        if (abs(SLB_MBLUR_CONE_DITHER) > 0.001)
        {
            float dither_cone = SLB_R1(noise * 1024.0 + float(i)) - 0.5;
            pixel_velocity = SLB_Rotate2(p_velocity_orig, dither_cone * SLB_MBLUR_CONE_DITHER * SLB_PI);
        }

        float2 offset;

        offset = pixel_velocity * float(i);
        offset /= float(SLB_MBLUR_SAMPLES * 2 + 1);
        offset += pixel_velocity / float(SLB_MBLUR_SAMPLES * 2 + 1) * dither;

        float2 tc = uv + offset;
        float amount = float((0.0 < tc.x && tc.x < 1.0) && (0.0 < tc.y && tc.y < 1.0)); /// Fix black borders

        if (SLB_MBLUR_SAMPLES == 0)
        {
            tc = clamp(tc, 0.0, 0.999);
            amount = 1.0;
        }

        float3 simg = SLB_Screen_GetImage(tc, iSample).rgb;

        if (i != 0)
        {
#ifdef SLB_MBLUR_WPN
            float3 spos = SLB_Screen_GetPosition(tc, iSample).xyz;

            /// Fix sky ghosting caused by infinite depth value (KD)
            spos.z = is_sky(spos.z) > 0.5 ? 10000.0 : spos.z;

            amount *= SLB_MBlur_WeaponMask(spos);
#endif

#ifdef SLB_MBLUR_SIGHT_MASK
            amount *= SLB_MBlur_SightMask(tc, simg, aspect, iSample);
#endif
        }

        accum += simg * amount;

        occ += amount;
    }

    accum /= occ + 1e-6; /// fix Nans

    return accum;
}

#endif // SLB_MOTION_BLUR_H

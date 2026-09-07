#ifndef OGSR_XEGTAO_COMMON_H
#define OGSR_XEGTAO_COMMON_H

// SM5 / FXC path: FP32 arithmetic, compact R16F depth and R8 AO storage.
#define XE_GTAO_USE_HALF_FLOAT_PRECISION 0
#define VA_SATURATE saturate
#include "xegtao\XeGTAO.hlsli"

// Keep reflection flat: OGSR's constant parser does not support cbuffer structs.
cbuffer XeConstants : register(b0)
{
    float4 xe_viewport; // padded size, inverse padded size
    float4 xe_source;   // actual render size, noise frame, radius
    float4 xe_ndc;      // NDC-to-view multiply, add (including TAA jitter)
};

GTAOConstants XeConstantsForFrame()
{
    GTAOConstants c = (GTAOConstants)0;
    c.ViewportSize = int2(xe_viewport.xy);
    c.ViewportPixelSize = xe_viewport.zw;
    c.NDCToViewMul = xe_ndc.xy;
    c.NDCToViewAdd = xe_ndc.zw;
    c.NDCToViewMul_x_PixelSize = xe_ndc.xy * xe_viewport.zw;
    c.EffectRadius = xe_source.w;
    c.EffectFalloffRange = XE_GTAO_DEFAULT_FALLOFF_RANGE;
    c.RadiusMultiplier = XE_GTAO_DEFAULT_RADIUS_MULTIPLIER;
    c.FinalValuePower = XE_GTAO_DEFAULT_FINAL_VALUE_POWER;
    c.DenoiseBlurBeta = 1.2;
    c.SampleDistributionPower = XE_GTAO_DEFAULT_SAMPLE_DISTRIBUTION_POWER;
    c.ThinOccluderCompensation = XE_GTAO_DEFAULT_THIN_OCCLUDER_COMPENSATION;
    c.DepthMIPSamplingOffset = XE_GTAO_DEFAULT_DEPTH_MIP_SAMPLING_OFFSET;
    c.NoiseIndex = int(xe_source.z);
    return c;
}

float3 XeDecodeNormal(float2 packed)
{
    float2 f = packed * 2.0 - 1.0;
    float3 n = float3(f, 1.0 - abs(f.x) - abs(f.y));
    float t = saturate(-n.z);
    n.xy += float2(n.x >= 0 ? -t : t, n.y >= 0 ? -t : t);
    return normalize(n);
}
#endif

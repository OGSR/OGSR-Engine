#ifndef MBLUR_H
#define MBLUR_H

#ifndef USE_MBLUR
float3 mblur(float2 UV, float3 pos, float3 c_original) { return c_original; }
#else

#include "common.h"
#include "slb_motion_blur.h"

////////////////////////////////////////
float4x4 m_current; // Current projection matrix
float4x4 m_previous; // Previous projection matrix
float2 m_blur;
////////////////////////////////////////

float3 mblur(float2 UV, float3 pos, float3 img)
{
    // Fix sky ghosting caused by infinite depth value (KD)
    pos.z = is_sky(pos.z) > 0.5 ? 10000.0 : pos.z;

    // Sample position buffer
    float4 pos4 = float4(pos, 1.0);

    // Get current texture coordinates
    float4 p_current = mul(m_current, pos4);
    float2 current_tc = p_current.xy /= p_current.w;

    // Get previous texture coordinates
    float4 p_previous = mul(m_previous, pos4);
    float2 previous_tc = p_previous.xy / p_previous.w;

    // Get velocity (multiplied with motion blur intensity)
    float2 p_velocity = (current_tc - previous_tc) * m_blur;

    return SLB_MBlur(UV, pos, img, p_velocity, 0);
}
#endif
#endif
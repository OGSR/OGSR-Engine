#include "common.h"
#include "shared\cloudconfig.h"

struct vi
{
    float4 p : POSITION;
    float4 dir : COLOR0; // dir0,dir1(w<->z)
    float4 color : COLOR1; // rgb. intensity
};

struct vf
{
    float4 color : COLOR0; // rgb. intensity, for SM3 - tonemap-prescaled, HI-res
    float2 tc0 : TEXCOORD0;
    float2 tc1 : TEXCOORD1;
    float4 hpos : SV_Position;
};

vf main(vi v)
{
    vf o;
    o.hpos = mul(m_WVP, v.p); // xform, input in world coords
    o.hpos.xy = get_taa_jitter(o.hpos);

    // generate tcs
    float2 d0 = v.dir.xy * 2.0f - 1.0f;
    float2 d1 = v.dir.wz * 2.0f - 1.0f;
    float2 _0 = v.p.xz * CLOUD_TILE0 + d0 * timers.z * CLOUD_SPEED0;
    float2 _1 = v.p.xz * CLOUD_TILE1 + d1 * timers.z * CLOUD_SPEED1;
    o.tc0 = _0;
    o.tc1 = _1;

    o.color = v.color;
    o.color.w *= pow(v.p.y, 25.0f);

    return o;
}

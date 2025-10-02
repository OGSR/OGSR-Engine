#include "common.h"

struct vi
{
    float4 p : POSITION;
    float4 c : COLOR0;
    float3 tc0 : TEXCOORD0;
    float3 tc1 : TEXCOORD1;
};

struct v2p
{
    float4 c : COLOR0;
    float3 tc0 : TEXCOORD0;
    float3 tc1 : TEXCOORD1;
    float4 hpos_curr : HPOS_CURRENT;
    float4 hpos_old : HPOS_PREVIOUS;
    float4 hpos : SV_Position;
};

v2p main(vi v)
{
    v2p o;

    float4 tpos = float4(2000 * v.p.x, 2000 * v.p.y, 2000 * v.p.z, 2000 * v.p.w);

    o.hpos = mul(m_WVP, tpos);
    o.hpos.z = o.hpos.w;
    o.hpos_curr = o.hpos;
    o.hpos_old = mul(m_WVP_old, tpos);
    o.hpos_old.z = o.hpos_old.w;
    o.hpos.xy = get_taa_jitter(o.hpos);

    o.tc0.xyz = v.tc0; // copy tc
    o.tc1.xyz = v.tc1; // copy tc

    float3 tint = v.c.rgb * 1.7;

    o.c = float4(tint, v.c.a); // copy color, pre-scale by tonemap

    return o;
}
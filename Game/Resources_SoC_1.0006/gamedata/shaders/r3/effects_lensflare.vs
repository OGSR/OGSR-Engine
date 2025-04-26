#include "common.h"
#include "ogse_functions.h"
#include "screenspace_fog.h"

struct _in
{
    float4 P : POSITION;
    float2 tc : TEXCOORD0;
    float4 c : COLOR0;
};
struct _out
{
    float4 hpos : SV_Position;
    float2 tc0 : TEXCOORD0;
    float4 tc1 : TEXCOORD1;
    float4 tc2 : TEXCOORD2;
    float4 c0 : COLOR0;
    float fog : FOG;
};

uniform float4x4 mVPTexgen;

_out main(_in v)
{
    _out o;

    o.hpos = mul(m_WVP, v.P); // xform, input in world coords
    o.hpos.xy = get_taa_jitter(o.hpos);

    o.tc0.xy = v.tc; // copy tc
    o.tc1 = proj_to_screen(o.hpos);
    o.tc1.xy /= o.tc1.w;

    float fog = saturate(calc_fogging(v.P)); // fog, input in world coords
    o.fog = SSFX_FOGGING(1.0 - fog, v.P.y); // Add SSFX Fog
    o.c0.rgb = lerp(fog_color, v.c, o.fog * o.fog); // fog blending
    o.c0.a = o.fog; // Alpha

    // o.c0 = v.c;
    o.tc2 = mul(mVPTexgen, v.P);
    o.tc2.z = o.hpos.z;

    return o;
}

#include "common.h"

struct Out
{
    float4 hpos : SV_POSITION;
    float3 wpos : TEXCOORD0;
};

uniform float puddle_height;

static const float2 quad_geom[6] = {float2(-1.0, -1.0), float2(-1.0, 1.0), float2(1.0, -1.0), float2(-1.0, 1.0), float2(1.0, 1.0), float2(1.0, -1.0)};

Out main(uint vert_id : SV_VertexID)
{
    Out O;

    float3 v_pos;
    v_pos.xz = quad_geom[vert_id];
    v_pos.y = puddle_height;
    O.wpos = mul(m_W, float4(v_pos, 1.0));

    O.hpos = mul(m_VP, float4(O.wpos, 1.0));
    O.hpos.xy = get_taa_jitter(O.hpos);

    return O;
}

#include "common.h"

struct _in
{
    float4 P : POSITIONT; // xy=pos, zw=tc0
    float2 tcJ : TEXCOORD0; // jitter coords
};

struct v2p
{
    float2 tc0 : TEXCOORD0; // tc.xy
    float4 hpos : SV_Position;
};

// Vertex
v2p main(_in I)
{
    v2p O;
    O.hpos = float4(I.P.x, -I.P.y, 0, 1);
    O.tc0 = I.P.zw;
    return O;
}

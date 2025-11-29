#include "common.h"

struct v2p_flare
{
    float2 Tex0 : TEXCOORD0;
    float4 Tex1	: TEXCOORD1;
    float4 Color : COLOR;
    float4 HPos : SV_Position;
};

uniform float4x4 mVPTexgen;

//////////////////////////////////////////////////////////////////////////////////////////
// Vertex
v2p_flare main(v_TL I)
{
    v2p_flare O;

    O.HPos = mul(m_VP, I.P); // xform, input in world coords

    O.Tex0 = I.Tex0;
    O.Tex1 = mul(mVPTexgen, I.P);
    O.Tex1.z = O.HPos.z;

    O.Color = I.Color.bgra; //	swizzle vertex colour

    return O;
}
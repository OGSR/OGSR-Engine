#include "common.h"

struct v2p_tracer
{
    float2 Tex0 : TEXCOORD0;
    float4 tctexgen : TEXCOORD1;
    float view_z : TEXCOORD2;
    float4 Color : COLOR;
    float4 HPos : SV_Position;
};

uniform float4x4 mVPTexgen;

//////////////////////////////////////////////////////////////////////////////////////////
// Vertex
v2p_tracer main(v_TL I)
{
    v2p_tracer O;

    O.HPos = mul(m_WVP, I.P);
    O.Tex0 = I.Tex0;
    O.tctexgen = mul(mVPTexgen, I.P);
    O.view_z = mul(m_WV, I.P).z;
    O.Color = I.Color.bgra;

    return O;
}

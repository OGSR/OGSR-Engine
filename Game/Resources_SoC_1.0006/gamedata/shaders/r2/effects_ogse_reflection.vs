#include "common.h"
#include "ogse_config.h"
#include "shared\waterconfig.h"
#include "shared\watermove.h"

struct v_vert
{
	float4	P		: POSITION;        // (float,float,float,1) coords in world space
	float4	N		: NORMAL;          // (nx,ny,nz,hemi occlusion)
	float4	T		: TANGENT;
	float4	B		: BINORMAL;
	float4	color	: COLOR0;          // (r,g,b,dir-occlusion)
	float2	uv		: TEXCOORD0;       // (u0,v0)
};
struct vf
{
	float4	hpos		: POSITION;	  // position in projected space for rasterizer
	float4	pos			: TEXCOORD0;  // position world space
	float3	N			: TEXCOORD1;  // matrix for tangent2world normal transformation
	float3	v2point		: TEXCOORD2;  // eye-to-point vector in world space - for reflections
};

vf main (v_vert v)
{
	vf o;

	float4 P = v.P;

	o.v2point = P - eye_position;
	o.pos = P;

	o.N = unpack_bx2(v.N);
	o.hpos = mul(m_VP, P);

	return o;
}
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
	float2	tbase		: TEXCOORD0;  // base tc
	float4	tnorm0		: TEXCOORD1;  // tc for normal map
	float4	position	: TEXCOORD2;  // tc for normal map
	float3	M1			: TEXCOORD3;  // matrix for tangent2world normal transformation
	float3	M2			: TEXCOORD4;
	float3	M3			: TEXCOORD5;
	float3	v2point_w	: TEXCOORD6;
	float4	position_w	: TEXCOORD7;
	float4	tctexgen    : TEXCOORD8;  // position in screen space of current pixel
	float4	c0			: COLOR0;
};

uniform float4 c_depth;
vf main (v_vert v)
{
	vf o;

	float4 P = v.P;

#if defined(USE_PUDDLES) && defined(NEED_PUDDLES)
	P.xyz = mul(m_W, P);
	float speed = PUDDLES_GROW_SPEED;
	P.y += saturate(ogse_c_rain.x*ogse_c_rain.w*speed)*c_depth.x;
	P.w = 1.f;
#endif
	
#ifndef WATER_NO_MOVE
	P = watermove(P);
#endif

	float3	P_v	= mul		(m_V,  P	);
	o.position	= float4	(P_v.xyz, 	v.N.w	);
	o.position_w = float4	(P.xyz, 1.0	);
	o.v2point_w = P.xyz - eye_position.xyz;

#if defined(USE_PUDDLES) && defined(NEED_PUDDLES)
	o.tbase = v.uv;
	float3 N = v.N.xyz; 
	float3 T = v.T.xyz;
	float3 B = v.B.xyz;        
#else 
	o.tbase = unpack_tc_base(v.uv,v.T.w,v.B.w)/* + ogse_c_jitter.xy*/;                // copy tc
	float3 N = unpack_bx2(v.N.xyz);        // just scale (assume normal in the -.5f, .5f)
	float3 T = unpack_bx2(v.T.xyz);        //
	float3 B = unpack_bx2(v.B.xyz);      
#endif

#ifndef WATER_NO_MOVE	
	o.tnorm0.xy = watermove_tc(o.tbase*W_DISTORT_BASE_TILE_0, P.xz, W_DISTORT_AMP_0);
	o.tnorm0.zw = watermove_tc(o.tbase*W_DISTORT_BASE_TILE_1, P.xz, W_DISTORT_AMP_1);
#else
	o.tnorm0.xy = o.tbase;
	o.tnorm0.zw = o.tbase;
#endif
        // Calculate the 3x3 transform from tangent space to world-space
        // TangentToWorldSpace = object2world * tangent2object
        //                     = object2world * transpose(object2tangent) (since the inverse of a rotation is its transpose)
	float3x3 xform = mul ((float3x3)m_W, float3x3(
                                  T.x,B.x,N.x,
                                  T.y,B.y,N.y,
                                  T.z,B.z,N.z
                                ));

        // Feed this transform to pixel shader
	o.M1 = xform[0];
	o.M2 = xform[1];
	o.M3 = xform[2];

	o.hpos = mul(m_VP, P);
	o.c0 = float4(v.color.xyz, 1.0);
	o.tctexgen = proj_to_screen(o.hpos);

	return o;
}
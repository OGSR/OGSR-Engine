#include "common.h"

struct vv
{
	float4 P	: POSITION;
//	float4 N	: NORMAL;
	float2 tc	: TEXCOORD0;
	float4 c	: COLOR0;
};
struct vf
{
	float4 hpos	: POSITION;
	float2 tc	: TEXCOORD0;
	float4 c	: COLOR0;
//	float3 c0	: COLOR1;		// c0=all lighting
//	float  fog	: FOG;

#ifdef	USE_SOFT_PARTICLES
	float4 tctexgen	: TEXCOORD1;
#endif	//	USE_SOFT_PARTICLES
};

vf main (vv v)
{
	vf 		o;

	o.hpos 		= mul	(m_WVP, v.P);		// xform, input in world coords
	o.tc		= v.tc;// + ogse_c_jitter.xy;				// copy tc
	o.c		= v.c;				// copy color

/*	float3 	L_rgb 	= v.ñ.xyz;						// precalculated RGB lighting
	float3 	L_hemi 	= v_hemi(N)*v.Nh.w;					// hemisphere
	float3 	L_sun 	= v_sun(N)*v.color.w;					// sun
	float3 	L_final	= L_rgb*//* + L_hemi + L_sun + L_ambient*//*;

	o.c0		= L_final;
	o.fog 		= 1.0;//calc_fogging 		(v.P);			// fog, input in world coords*/

#ifdef	USE_SOFT_PARTICLES
	o.tctexgen 	= proj_to_screen(o.hpos);
	float3 Pe		= mul	(m_WV, v.P);
	o.tctexgen.z = Pe.z;
#endif	//	USE_SOFT_PARTICLES

	return o;
}

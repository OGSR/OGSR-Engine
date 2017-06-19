#include "common.h"

struct vv
{
	float4 P	: POSITION;
	float2 tc	: TEXCOORD0;
	float4 c	: COLOR0;
};
struct vf
{
	float4 hpos	: POSITION;
	float2 tc	: TEXCOORD0;
	float4 c	: COLOR0;
#ifdef	USE_SOFT_PARTICLES
	float4 tctexgen	: TEXCOORD1;
#endif	//	USE_SOFT_PARTICLES
};

vf main (vv v)
{
	vf 		o;

	o.hpos 		= mul	(m_WVP, v.P);		// xform, input in world coords
	o.tc		= v.tc;				// copy tc
	o.c		= v.c;				// copy color
#ifdef	USE_SOFT_PARTICLES
	o.tctexgen 	= proj_to_screen(o.hpos);
	float3 Pe		= mul	(m_WV, v.P);
	o.tctexgen.z = Pe.z;
#endif	//	USE_SOFT_PARTICLES

	return o;
}

#include "common.h"

struct	v_vert
{
	float4 	pos	: POSITION;	// (float,float,float,1)
	float4	color	: COLOR0;	// (r,g,b,dir-occlusion)
};
struct 	vf
{
	float4 hpos	: POSITION;
	float4 c	: COLOR0;
	float  fog	: FOG;
};

vf main (v_vert v)
{
	vf 		o;

	o.hpos 		= mul			(m_VP, v.pos);			// xform, input in world coords
	o.c 		= v.color;
	o.fog 		= calc_fogging 		(v.pos);			// fog, input in world coords

	return o;
}

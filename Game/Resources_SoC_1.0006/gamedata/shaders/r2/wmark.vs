#include "common.h"
#include "shared\wmark.h"

struct vf
{
	float4 hpos	: POSITION;
	float2 tc0	: TEXCOORD0;
};

vf main (v_static v)
{
	vf 		o;

	float3 	N 	= 	unpack_normal	(v.Nh);
	float4 	P 	= 	wmark_shift	(v.P,N);
	o.hpos 		= 	mul		(m_VP, P);			// xform, input in world coords
	o.tc0		= 	unpack_tc_base	(v.tc,v.T.w,v.B.w);		// copy tc

	return o;
}

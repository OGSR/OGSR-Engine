#include "common.h"

struct _in
{
	float4 P	: POSITION;
	float2 tc	: TEXCOORD0;
	float4 c	: COLOR0;
};
struct _out
{
	float4 hpos	: POSITION;
	float2 tc0	: TEXCOORD0;
	float4 tc1	: TEXCOORD1;
};

_out main (_in v)
{
	_out 		o;

	o.hpos 		= mul	(m_WVP, v.P);		// xform, input in world coords
	o.tc0.xy		= v.tc;				// copy tc
	o.tc1 = proj_to_screen(o.hpos);
	o.tc1.xy /= o.tc1.w;

	return o;
}

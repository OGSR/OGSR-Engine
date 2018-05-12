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
#ifdef FL_USE_DIRT
	float4 tc1	: TEXCOORD1;
#endif
	float4 c0	: COLOR0;
};

_out main (_in v)
{
	_out 		o;

	o.hpos 		= mul	(m_WVP, v.P);		// xform, input in world coords
	o.tc0		= v.tc;				// copy tc
	o.c0		= v.c;

#ifdef FL_USE_DIRT
	o.tc1 = proj_to_screen(o.hpos);
	o.tc1.xy /= o.tc1.w;
#endif

	return o;
}

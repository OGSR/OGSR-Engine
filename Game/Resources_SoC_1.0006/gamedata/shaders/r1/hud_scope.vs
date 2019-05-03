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
	float4 c0	: COLOR0;
};

_out main (_in v)
{
	_out 		o;

	o.hpos 		= mul	(m_WVP, v.P);		// xform, input in world coords
	o.tc0		= v.tc;				// copy tc
	o.c0		= v.c;

	return o;
}

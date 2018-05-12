#include "common.h"

uniform float4	vMinBounds;
uniform float4	vMaxBounds;

struct vf
{
	float4 	hpos		: POSITION;
	float3 	lightToPos	: TEXCOORD0;		// light center to plane vector
	float3 	vPos		: TEXCOORD1;		// position in camera space
	half 	fDensity	: TEXCOORD2;		// plane density alon Z axis
};

vf main (v_static v)
{
	vf 		o;
	float4	vPos;
	vPos.xyz 	= lerp( vMinBounds.xyz, vMaxBounds.xyz, v.P);	//	Position in camera space
	vPos.w 		= 1;
	o.hpos 		= mul			(m_P, vPos);		// xform, input in camera coordinates

	o.lightToPos = vPos.xyz - Ldynamic_pos.xyz;
	o.vPos = vPos;

	o.fDensity = VOL_LIGHT_MAX_INTENSITY;

	return o;
}

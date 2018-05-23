#include "common.h"

uniform float4 		consts; // {1/quant,1/quant,diffusescale,ambient}
#ifndef USE_HWI
uniform float4 		array	[240] : register(c12);
#endif

p_flat 	main (v_detail v)
{
	p_flat 		O;
#ifdef USE_HWI
/*
	float4  m0 	= v.m0;
	float4  m1 	= v.m1;
	float4  m2 	= v.m2;
	float4  c0 	= v.c0.xyyy;*/

	float4  m0 	= float4(v.m0.x,	 0, -v.m0.y, v.m1.x);
	float4  m1 	= float4(0,		v.m1.w,		  0, v.m1.y);
	float4  m2 	= float4(v.m0.y,	 0,  v.m0.x, v.m1.z);
	float4  c0 	= v.m0.zwww;
#else
	// index
	int 	i 	= v.misc.w;
	float4  arr1 	= array[i+0];
	float4  arr2 	= array[i+1];
/*	float4  m2 	= array[i+2];
	float4  c0 	= array[i+3];*/
	float4  m0 	= float4( arr1.x,	   0, -arr1.y, arr2.x);
	float4  m1 	= float4(	   0, arr2.w,		0, arr2.y);
	float4  m2 	= float4( arr1.y,	   0,  arr1.x, arr2.z);
	float4  c0 	= arr1.zwww;
#endif

	// Transform pos to world coords
	float4 	pos;
 	pos.x 		= dot	(m0, v.pos);
 	pos.y 		= dot	(m1, v.pos);
 	pos.z 		= dot	(m2, v.pos);
	pos.w 		= 1;

	// Normal in world coords
	float3 	norm;	
		norm.x 	= pos.x - m0.w	;
		norm.y 	= pos.y - m1.w	+ .75f;	// avoid zero
		norm.z	= pos.z - m2.w	;

	// Final out
	float4	Pp 	= mul		(m_WVP,	pos				);
	O.hpos 		= Pp;
	O.N 		= mul		(m_WV,  normalize(norm)	);
	float3	Pe	= mul		(m_WV,  pos				);
	O.tcdh 		= float4	((v.misc * consts).xyyy	);
	O.tcdh.z = 1.f;

	O.position	= float4	(Pe, 		c0.w		);

	return O;
}
FXVS;

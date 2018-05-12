#include "common.h"

uniform float4 		consts; // {1/quant,1/quant,diffusescale,ambient}
uniform float4 		wave; 	// cx,cy,cz,tm
uniform float4 		dir2D; 
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
	float4  m0 	= float4( arr1.x,	   0,  -arr1.y, arr2.x);
	float4  m1 	= float4(	   0, arr2.w,		 0, arr2.y);
	float4  m2 	= float4( arr1.y,	   0,   arr1.x, arr2.z);
	float4  c0 	= arr1.zwww;
#endif

	// Transform pos to world coords
	float4 	pos;
 	pos.x 		= dot	(m0, v.pos);
 	pos.y 		= dot	(m1, v.pos)+0.001;
 	pos.z 		= dot	(m2, v.pos);
	pos.w 		= 1;

	// 
	float 	base 	= m1.w;
	float 	dp	= calc_cyclic   (dot(pos,wave));
	float 	H 	= pos.y - base;			// height of vertex (scaled)
	float 	frac 	= v.misc.z*consts.x;		// fractional
	float 	inten 	= H * dp;
	float2 	result	= calc_xz_wave	(dir2D.xz*inten,frac);
	pos		= float4(pos.x+result.x, pos.y, pos.z+result.y, 1);

	// Normal in world coords
	float3 	norm;	//	= float3(0,1,0);
		norm.x 	= pos.x - m0.w	;
		norm.y 	= pos.y - m1.w	+ .75f;	// avoid zero
		norm.z	= pos.z - m2.w	;

	// Final out
	float4	Pp 	= mul		(m_WVP,	pos				);
	O.hpos 		= Pp;
	O.N 		= mul		(m_WV,  normalize(norm)	);
	float3	Pe	= mul		(m_WV,  pos				);
	O.tcdh 		= float4	((v.misc * consts).xyyy );
	O.position	= float4	(Pe, 		c0.w		);

#ifdef USE_GRASS_WAVE
	float 	_dp	= calc_cyclic   (dot(pos,wave*GRASS_WAVE_FREQ));
	O.tcdh.z	= consts.w+consts.z*max(0.f, _dp)*frac*GRASS_WAVE_POWER;
#endif

	return O;
}
FXVS;

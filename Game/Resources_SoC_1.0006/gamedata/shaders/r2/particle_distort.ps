#include "common.h"

struct 	v2p
{
 	float2 	tc0	: 	TEXCOORD0;	// base
	half4	c	:	COLOR0;		// diffuse

#ifdef	USE_SOFT_PARTICLES
//	Igor: for additional depth dest
	float4 tctexgen	: TEXCOORD1;
#endif	//	USE_SOFT_PARTICLES
    	float   fog:            FOG;		// fog
};


//////////////////////////////////////////////////////////////////////////////////////////
// Pixel
uniform sampler s_distort;
half4 	main	( v2p I )	: COLOR
{
	half4	distort	= tex2D		(s_distort,I.tc0);
	half    factor	= distort.a * dot(I.c.rgb,0.33h);
	half4	result  = half4	(distort.rgb,factor);

	result.a	*= I.fog*I.fog; // skyloader: fog fix

	return	result;
}

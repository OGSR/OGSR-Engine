#ifndef        VIGNETTE_H
#define        VIGNETTE_H

#include	"ogse_config.h"

float vignette(float2 center) 
{
	float dist = distance(center, float2(0.5,0.5));
	dist = smoothstep(1 + VIGNETTE_IN, VIGNETTE_OUT+VIGNETTE_IN, dist);
	return saturate(dist);
}
#endif
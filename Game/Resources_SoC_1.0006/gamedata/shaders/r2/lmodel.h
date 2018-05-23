#ifndef	LMODEL_H
#define LMODEL_H

#include "common.h"
#include "ogse_config.h"
//////////////////////////////////////////////////////////////////////////////////////////
// Lighting formulas			// 
float4 	plight_infinity		(float m, float3 _point, float3 normal, float3 light_direction)       				{
  float3 N		= normal;							// normal 
  float3 V 		= -normalize	(_point);					// vector2eye
  float3 L 		= -light_direction;						// vector2light
  float3 H		= normalize	(L+V);						// float-angle-vector  
  float4 light = 		tex3D(s_material,	float3( dot(L,N), dot(H,N), m ) );		// sample material
  return light;
/*  float sum_light = saturate(0.6*light.x + light.w*1.4);
  return float4(sum_light,sum_light,sum_light,sum_light);*/
}
float4 	plight_infinity2	(float m, float3 _point, float3 normal, float3 light_direction)       				{
  	float3 N		= normal;							// normal 
  	float3 V 	= -normalize		(_point);		// vector2eye
  	float3 L 	= -light_direction;					// vector2light
 	float3 H		= normalize			(L+V);			// float-angle-vector 
	float3 R     = reflect         	(-V,N);
	float 	s	= saturate(dot(L,R));
			s	= saturate(dot(H,N));
	float 	f 	= saturate(dot(-V,R));
			s  *= f;
	float4	r	= tex3D 			(s_material,	float3( dot(L,N), s, m ) );	// sample material
			r.w	= pow(saturate(s),4);
  	return	r	;
}
float4 	plight_local		(float m, float3 _point, float3 normal, float3 light_position, float light_range_rsq, out float rsqr)  {
  float3 N		= normal;							// normal 
  float3 L2P 	= _point-light_position;                         		// light2point 
  float3 V 		= -normalize	(_point);					// vector2eye
  float3 L 		= -normalize	((float3)L2P);					// vector2light
  float3 H		= normalize	(L+V);						// float-angle-vector
		rsqr	= dot		(L2P,L2P);					// distance 2 light (squared)
  float  att 	= saturate	(1 - rsqr*light_range_rsq);			// q-linear attenuate
  float4 light	= tex3D		(s_material, float3( dot(L,N), dot(H,N), m ) ); 	// sample material
  return att*light;
/*  float sum_light = saturate(0.6*light.x + light.w*1.4);
  return att*float4(sum_light,sum_light,sum_light,sum_light);*/
}
float4 	plight_local_torch		(float m, float3 _point, float3 normal, float3 light_position, float light_range_rsq, float angle_cos)  {
  float3 N		= normal;							// normal 
  float3 L2P 	= _point-light_position;                         		// light2point 
  float3 V 		= -normalize	(_point);					// vector2eye
  float3 L 		= -normalize	((float3)L2P);					// vector2light
  float3 H		= normalize	(L+V);						// float-angle-vector
  float rsqr	= dot		(L2P,L2P);					// distance 2 light (squared)
  float  att 	= saturate	(1 - rsqr*light_range_rsq)*smoothstep(angle_cos, angle_cos*1.2, abs(dot(L,float3(0,0,1))));			// q-linear attenuate
  float4 light	= tex3D		(s_material, float3( dot(L,N), dot(H,N), m ) ); 	// sample material
  return att*light;
}
float4	blendp	(float4	value, float4 	tcp)    		{
	#ifndef FP16_BLEND  
		value 	+= (float4)tex2Dproj 	(s_accumulator, tcp); 	// emulate blend
	#endif
	return 	value	;
}
float4	blend	(float4	value, float2 	tc)			{
	#ifndef FP16_BLEND  
		value 	+= (float4)tex2D 	(s_accumulator, tc); 	// emulate blend
	#endif
	return 	value	;
}

#endif

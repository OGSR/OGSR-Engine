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
  float3 H		= normalize	(L+V);						// half-angle-vector 
  return tex3D 		(s_material,	half3( dot(L,N), dot(H,N), m ) );		// sample material
}
float4 	plight_infinity2 (half m, half3 _point, half3 normal, half3 light_direction)       				{
  float3 N		= normal;							// normal 
  float3 V 		= -normalize	(_point);					// vector2eye
  float3 L 		= -light_direction;						// vector2light
  float3 H		= normalize	(L+V);						// half-angle-vector 
 // return tex3D 		(s_material,	half3( dot(L,N), dot(H,N), m ) );		// sample material
	float4 ret;
	float4 light = tex3D ( s_material, float3( dot(L,N), dot(H,N), m ) );		// sample material
	ret = light+float4(light.www*(Ldynamic_color.xyz),light.w)*Ldynamic_color.w;
	return ret;
}
float4 	plight_local		(float m, float3 _point, float3 normal, float3 light_position, float light_range_rsq, out float rsqr)  {
  float3 N		= normal;							// normal 
  float3 L2P 	= _point-light_position;                         		// light2point 
  float3 V 		= -normalize	(_point);					// vector2eye
  float3 L 		= -normalize	((float3)L2P);					// vector2light
  float3 H		= normalize	(L+V);						// float-angle-vector
		rsqr	= dot		(L2P,L2P);					// distance 2 light (squared)
  float  att 	= saturate	(1 - rsqr*light_range_rsq*ECB_LL_DIST)*ECB_LL_BRIGHTNESS;			// q-linear attenuate
  float4 light	= tex3D		(s_material, half3( dot(L,N), dot(H,N), m ) ); 	// sample material
  //new light model start;
  return float4(att*light.xxx,0)+att*float4(light.www*(Ldynamic_color.xyz*Ldynamic_color.xyz),light.w);
  //new light model end;
}
float4 	plight_local_torch		(float m, float3 _point, float3 normal, float3 light_position, float light_range_rsq, float angle_cos)  {
  float3 N		= normal;							// normal 
  float3 L2P 	= _point-light_position;                         		// light2point 
  float3 V 		= -normalize	(_point);					// vector2eye
  float3 L 		= -normalize	((float3)L2P);					// vector2light
  float3 H		= normalize	(L+V);						// float-angle-vector
  float rsqr	= dot		(L2P,L2P);					// distance 2 light (squared)
  float  att 	= saturate	(1 - rsqr*light_range_rsq*ECB_LL_DIST)*ECB_LL_BRIGHTNESS;			// q-linear attenuate
  float4 light	= tex3D		(s_material, half3( dot(L,N), dot(H,N), m ) ); 	// sample material
  //new light model start;
  return float4(att*light.xxx,0)+att*float4(light.www*(Ldynamic_color.xyz*Ldynamic_color.xyz),light.w);
  //new light model end;
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

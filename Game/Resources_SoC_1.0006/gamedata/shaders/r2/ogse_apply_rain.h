#ifndef OGSE_APPLY_RAIN_H
#define OGSE_APPLY_RAIN_H

#include "ogse_reflections.h"

uniform sampler2D s_rmap;
//uniform sampler2D s_puddles;
//uniform float4 ogse_c_rain; // x - rmap dist, y - flood level, z - wet level, w - rain intensity

float calculate_wet_level(/*float hdiffuse, float hspecular, */float2 tc, float d)
{
	// 1. calculate wet value for pixel
	float4 R        = tex2D         (s_rmap, 			tc);                // rain mask
	float time = ogse_c_rain.z;
	time *= RMAP_WETTING_SPEED*ogse_c_rain.w; //wetting speed - uncomment it when needed
	time = saturate(time);
	return saturate(R.x*time);//(lerp(saturate(R.x), 1.0, step(ogse_c_rain.x-5,d))*time);
}

float3 calculate_env_mapping(float3 P, float3 N)
{
	float3 Nw = mul(m_v2w, N);
	float3 Pw = mul(m_v2w, P);
			/*cubemap reflections*/
	float3 v2point = normalize(Pw - eye_position);
	float3 vreflect = reflect(v2point, Nw);
	
	vreflect.y = vreflect.y*2-1;
	float3	env0	= texCUBE (s_env0, vreflect).xyz;
	float3	env1	= texCUBE (s_env1, vreflect).xyz;
	float3	env	= lerp (env0,env1,L_ambient.w)*2;

	float fresnel = saturate(dot(vreflect,v2point));
	float power = pow(fresnel,9);
	float amount	= 0.15h+0.25h*power;	// 1=full env, 0=no env

	return (amount*env);
}

#endif //OGSE_APPLY_RAIN_H

/*
   // Rain effets - Specific code
   
   // Parameter to customize heightmap for rain if needed
   // because it could not match the one for bumpoffset.
   float  ScaleHeight = 1.0f;
   float  BiasHeight = 0.0f;
   Heightmap = Heightmap * ScaleHeight + BiasHeight;   

   // Get depth/accumulation water information
   float2 AccumulatedWaters;
   // Get the "size" of the accumulated water in a hole/cracks
   AccumulatedWaters.x      = min(FloodLevel.x, 1.0 - Heightmap);
   // Get the size of the accumlated water in puddle taking into account the 
   // marging (0.4 constant here)
   AccumulatedWaters.y      = saturate((FloodLevel.y - Input.Color.g) / 0.4);
   // Mix vertex color and height map accumulated water by taking the higher
   float  AccumulatedWater  = max(AccumulatedWaters.x, AccumulatedWaters .y);
   
   // Ripple part
   // 0.05 to scale world uv of this app
   float3 RippleNormal  = tex2D(RippleTexture, Input.Pos.xz * 0.05) * 2 - 1;
   RippleNormal = mul(NBTMatrix, RippleNormal); 
   // saturate(RainIntensity * 100.0) to be 1 when RainIntensity is > 0 and 0 else
   float3 WaterNormal  = lerp(float3(0, 1, 0), RippleNormal, saturate(RainIntensity * 100.0)); // World space
      
   float NewWetLevel = saturate(WetLevel + AccumulatedWater);
   
   // Water influence on material BRDF (no modification of the specular term for now)
   // Type 2 : Wet region
   DoWetProcess(BaseDiffuse, Gloss, NewWetLevel);
  
   // Apply accumulated water effect
   // When AccumulatedWater is 1.0 we are in Type 4
   // so full water properties, in between we are in Type 3
   // Water is smooth
   Gloss    = lerp(Gloss, 1.0, AccumulatedWater);
   // Water F0 specular is 0.02 (based on IOR of 1.33)
   Specular = lerp(Specular, 0.02, AccumulatedWater);
   N        = lerp(N, WaterNormal, AccumulatedWater);

   // End Rain effect specific code
   ////////////////////////
 
   // Precalc many values for lighting equation
   float3 V = normalize(Input.ViewDir);      
   float3 L = normalize(LightPosition - Input.Pos);  
   float3 H = normalize(L + V);
      
   float  dotVH = saturate(dot(V, H));
   float  dotNH = saturate(dot(N, H));
   float  dotNL = saturate(dot(N, L));
   float  dotNV = saturate(dot(N, V));
    
   float3 R = reflect(V, N);
   float3 ReflColor = SampleEnvmap(R, Gloss, Input.Pos);
   
   // Fresnel for cubemap and Fresnel for direct lighting
   float3 SpecVH = Specular + (1.0 - Specular) * pow(1.0 - dotVH, 5.0);
   // Use fresnel attenuation from Call of duty siggraph 2011 talk
   float3 SpecNV = Specular + (1.0 - Specular) * pow(1.0 - dotNV, 5.0) / (4.0 - 3.0 * Gloss);
      
   // Convert Gloss [0..1] to SpecularPower [0..2048]
   float  SpecPower = exp2(Gloss * 11);
   
   // Lighting
   float3 DiffuseLighting     = dotNL * BaseDiffuse;
   // Normalized specular lighting
   float3 SpecularLighting    = SpecVH * ((SpecPower + 2.0) / 8.0) * pow(dotNH, SpecPower);
   float3 AmbientSpecLighting = ReflColor * SpecNV;
   
   float3 FinalColor = LightIntensity * (DiffuseLighting + SpecularLighting) + AmbientSpecLighting;
   
   // Linear to gamme space
   Output.Color = float4(pow(FinalColor, 1.0 / 2.2), 1.0);*/
#ifndef SLB_ALIASES_H
#define SLB_ALIASES_H

#ifdef SLB_GLSL /// We are in glslViewer

#define SLB_SAMPLER_LOAD(sampler_name, texture_coord) texelFetch(sampler_name, texture_coord.xy, texture_coord.z)

#define SLB_BRANCH
#define SLB_UNROLL(x)
#define SLB_STATIC_CONST const

/// HLSL shit
#define frac(x) fract(x)
#define saturate(x) clamp(x, 0.0, 1.0)
#define mul(a, b) (a * b)
#define reversebits(x) bitfieldReverse(x)
#define asuint(x) floatBitsToUint(x)
#define lerp(a, b, x) mix(a, b, x)
#define ddx(x) dFdx(x)
#define ddy(x) dFdy(x)

#define float2 vec2
#define float3 vec3
#define float4 vec4

#define float2x2 mat2
#define float3x3 mat3
#define float4x4 mat4

#define int2 ivec2
#define int3 ivec3
#define int4 ivec4

#define uint2 uvec2
#define uint3 uvec3
#define uint4 uvec4

#else /// We are in stalker

#define SLB_SAMPLER_LOAD(sampler_name, texture_coord)               sampler_name.Load(texture_coord)
#define SLB_SAMPLER_LOAD_MSAA(sampler_name, texture_coord, iSample) sampler_name.Load(texture_coord, iSample)

#define SLB_BRANCH [branch]
#define SLB_UNROLL(x) [unroll(x)]
#define SLB_STATIC_CONST static const

#endif

#endif /// SLB_ALIASES_H
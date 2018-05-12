#ifndef OGSE_PCSS_H
#define OGSE_PCSS_H

#ifdef USE_TRANSPARENT_GEOM
uniform	sampler2D	s_smap_near	: register(ps,s0);	// 2D/cube shadowmap
uniform	sampler2D	s_smap_far	: register(ps,s1);	// 2D/cube shadowmap
#endif
uniform	sampler2D	s_smap	: register(ps,s0);	// 2D/cube shadowmap

uniform	sampler2D	s_depth	: register(ps,s1);	// 2D/cube shadowmap
uniform float4 pcss_params;
uniform float4x4 m_v2lv;
//--------------------------------------------------------------------------------------
// File:    PercentageCloserSoftShadows.fx
// Author:  Louis Bavoil
// Email:   sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#define MAX_LINEAR_DEPTH 1.e30f

float2 Poisson25[] = {
{-0.978698, -0.0884121},
{-0.841121, 0.521165},
{-0.71746, -0.50322},
{-0.702933, 0.903134},
{-0.663198, 0.15482},
{-0.495102, -0.232887},
{-0.364238, -0.961791},
{-0.345866, -0.564379},
{-0.325663, 0.64037},
{-0.182714, 0.321329},
{-0.142613, -0.0227363},
{-0.0564287, -0.36729},
{-0.0185858, 0.918882},
{0.0381787, -0.728996},
{0.16599, 0.093112},
{0.253639, 0.719535},
{0.369549, -0.655019},
{0.423627, 0.429975},
{0.530747, -0.364971},
{0.566027, -0.940489},
{0.639332, 0.0284127},
{0.652089, 0.669668},
{0.773797, 0.345012},
{0.968871, 0.840449},
{0.991882, -0.657338},
};

float2 Poisson32[] = {
{-0.975402, -0.0711386},
{-0.920347, -0.41142},
{-0.883908, 0.217872},
{-0.884518, 0.568041},
{-0.811945, 0.90521},
{-0.792474, -0.779962},
{-0.614856, 0.386578},
{-0.580859, -0.208777},
{-0.53795, 0.716666},
{-0.515427, 0.0899991},
{-0.454634, -0.707938},
{-0.420942, 0.991272},
{-0.261147, 0.588488},
{-0.211219, 0.114841},
{-0.146336, -0.259194},
{-0.139439, -0.888668},
{0.0116886, 0.326395},
{0.0380566, 0.625477},
{0.0625935, -0.50853},
{0.125584, 0.0469069},
{0.169469, -0.997253},
{0.320597, 0.291055},
{0.359172, -0.633717},
{0.435713, -0.250832},
{0.507797, -0.916562},
{0.545763, 0.730216},
{0.56859, 0.11655},
{0.743156, -0.505173},
{0.736442, -0.189734},
{0.843562, 0.357036},
{0.865413, 0.763726},
{0.872005, -0.927},
};

float2 Poisson64[] = {
{-0.934812, 0.366741},
{-0.918943, -0.0941496},
{-0.873226, 0.62389},
{-0.8352, 0.937803},
{-0.822138, -0.281655},
{-0.812983, 0.10416},
{-0.786126, -0.767632},
{-0.739494, -0.535813},
{-0.681692, 0.284707},
{-0.61742, -0.234535},
{-0.601184, 0.562426},
{-0.607105, 0.847591},
{-0.581835, -0.00485244},
{-0.554247, -0.771111},
{-0.483383, -0.976928},
{-0.476669, -0.395672},
{-0.439802, 0.362407},
{-0.409772, -0.175695},
{-0.367534, 0.102451},
{-0.35313, 0.58153},
{-0.341594, -0.737541},
{-0.275979, 0.981567},
{-0.230811, 0.305094},
{-0.221656, 0.751152},
{-0.214393, -0.0592364},
{-0.204932, -0.483566},
{-0.183569, -0.266274},
{-0.123936, -0.754448},
{-0.0859096, 0.118625},
{-0.0610675, 0.460555},
{-0.0234687, -0.962523},
{-0.00485244, -0.373394},
{0.0213324, 0.760247},
{0.0359813, -0.0834071},
{0.0877407, -0.730766},
{0.14597, 0.281045},
{0.18186, -0.529649},
{0.188208, -0.289529},
{0.212928, 0.063509},
{0.23661, 0.566027},
{0.266579, 0.867061},
{0.320597, -0.883358},
{0.353557, 0.322733},
{0.404157, -0.651479},
{0.410443, -0.413068},
{0.413556, 0.123325},
{0.46556, -0.176183},
{0.49266, 0.55388},
{0.506333, 0.876888},
{0.535875, -0.885556},
{0.615894, 0.0703452},
{0.637135, -0.637623},
{0.677236, -0.174291},
{0.67626, 0.7116},
{0.686331, -0.389935},
{0.691031, 0.330729},
{0.715629, 0.999939},
{0.8493, -0.0485549},
{0.863582, -0.85229},
{0.890622, 0.850581},
{0.898068, 0.633778},
{0.92053, -0.355693},
{0.933348, -0.62981},
{0.95294, 0.156896},
};

// PRESET is defined by the app when (re)loading the fx

#define PRESET 0

#if PRESET == 0

#define USE_POISSON
#define SEARCH_POISSON_COUNT 25
#define SEARCH_POISSON Poisson25
#define PCF_POISSON_COUNT 25
#define PCF_POISSON Poisson25

#elif PRESET == 1

#define USE_POISSON
#define SEARCH_POISSON_COUNT 32
#define SEARCH_POISSON Poisson32
#define PCF_POISSON_COUNT 64
#define PCF_POISSON Poisson64

#else

#define BLOCKER_SEARCH_STEP_COUNT 3
#define PCF_FILTER_STEP_COUNT 7

#endif

// Using similar triangles from the surface point to the area light
float2 SearchRegionRadiusUV(float zWorld)
{
    return pcss_params.zw * (zWorld - pcss_params.x) / zWorld;
}

// Using similar triangles between the area light, the blocking plane and the surface point
float2 PenumbraRadiusUV(float zReceiver, float zBlocker)
{
    return pcss_params.zw * (zReceiver - zBlocker) / zBlocker;
}

// Project UV size to the near plane of the light
float2 ProjectToLightUV(float2 sizeUV, float zWorld)
{
    return sizeUV * pcss_params.x / zWorld;
}

// Derivatives of light-space depth with respect to texture coordinates
float2 DepthGradient(float2 uv, float z)
{
    float2 dz_duv = 0;

    float3 duvdist_dx = ddx(float3(uv,z));
    float3 duvdist_dy = ddy(float3(uv,z));

    dz_duv.x = duvdist_dy.y * duvdist_dx.z;
    dz_duv.x -= duvdist_dx.y * duvdist_dy.z;
    
    dz_duv.y = duvdist_dx.x * duvdist_dy.z;
    dz_duv.y -= duvdist_dy.x * duvdist_dx.z;

    float det = (duvdist_dx.x * duvdist_dy.y) - (duvdist_dx.y * duvdist_dy.x);
    dz_duv /= det;

    return dz_duv;
}

float BiasedZ(float z0, float2 dz_duv, float2 offset)
{
    return z0 + dot(dz_duv, offset);
}

float ZClipToZEye(float zClip)
{
	return pcss_params.y*pcss_params.x / (pcss_params.y - zClip*(pcss_params.y-pcss_params.x));   
}

// Returns average blocker depth in the search region, as well as the number of found blockers.
// Blockers are defined as shadow-map samples between the surface point and the light.
void FindBlocker(out float avgBlockerDepth, 
                out float numBlockers,
                float2 uv,
                float z0,
                float2 dz_duv,
                float2 searchRegionRadiusUV)
{
    float blockerSum = 0;
    numBlockers = 0;

#ifdef USE_POISSON
    for ( int i = 0; i < SEARCH_POISSON_COUNT; ++i )
    {
        float2 offset = SEARCH_POISSON[i] * searchRegionRadiusUV;
        float shadowMapDepth = tex2Dlod(s_depth, float4(uv + offset, 0,0)).x;
        float z = BiasedZ(z0, dz_duv, offset);
        if ( shadowMapDepth < z )
        {
            blockerSum += shadowMapDepth;
            numBlockers++;
        }
    }
#else
    float2 stepUV = searchRegionRadiusUV / BLOCKER_SEARCH_STEP_COUNT;
    for( float x = -BLOCKER_SEARCH_STEP_COUNT; x <= BLOCKER_SEARCH_STEP_COUNT; ++x )
        for( float y = -BLOCKER_SEARCH_STEP_COUNT; y <= BLOCKER_SEARCH_STEP_COUNT; ++y )
        {
            float2 offset = float2( x, y ) * stepUV;
            float shadowMapDepth = tDepthMap.SampleLevel(PointSampler, uv + offset, 0);
            float z = BiasedZ(z0, dz_duv, offset);
            if ( shadowMapDepth < z )
            {
                blockerSum += shadowMapDepth;
                numBlockers++;
            }
        }
#endif
    avgBlockerDepth = blockerSum / numBlockers;
}

// Performs PCF filtering on the shadow map using multiple taps in the filter region.
float PCF_Filter( float2 uv, float z0, float2 dz_duv, float2 filterRadiusUV, float4 pcf_tc )
{
    float sum = 0;
    
#ifdef USE_POISSON
    for ( int i = 0; i < PCF_POISSON_COUNT; ++i )
    {
        float4 offset = float4(PCF_POISSON[i] * filterRadiusUV, 0,0);
        float z = BiasedZ(z0, dz_duv, offset);
        sum += tex2Dproj(s_smap, pcf_tc + offset).x;
    }
    return sum / PCF_POISSON_COUNT;
#else
    float2 stepUV = filterRadiusUV / PCF_FILTER_STEP_COUNT;
    for( float x = -PCF_FILTER_STEP_COUNT; x <= PCF_FILTER_STEP_COUNT; ++x )
        for( float y = -PCF_FILTER_STEP_COUNT; y <= PCF_FILTER_STEP_COUNT; ++y )
        {
            float2 offset = float2( x, y ) * stepUV;
            float z = BiasedZ(z0, dz_duv, offset);
            sum += tDepthMap.SampleCmpLevelZero(PCF_Sampler, uv + offset, z);
        }
    float numSamples = (PCF_FILTER_STEP_COUNT*2+1);
    return sum / (numSamples*numSamples);
#endif
}

float PCSS_Shadow(float2 uv, float z, float2 dz_duv, float zEye, float4 pcf_tc)
{
    // ------------------------
    // STEP 1: blocker search
    // ------------------------
    float avgBlockerDepth = 0;
    float numBlockers = 0;
    float2 searchRegionRadiusUV = SearchRegionRadiusUV(zEye);
    FindBlocker( avgBlockerDepth, numBlockers, uv, z, dz_duv, searchRegionRadiusUV );

    // Early out if no blocker found
//    if (numBlockers == 0) return 1.0;

    // ------------------------
    // STEP 2: penumbra size
    // ------------------------
    float avgBlockerDepthWorld = ZClipToZEye(avgBlockerDepth);
    float2 penumbraRadiusUV = PenumbraRadiusUV(zEye, avgBlockerDepthWorld);
    float2 filterRadiusUV = ProjectToLightUV(penumbraRadiusUV, zEye);
    
    // ------------------------
    // STEP 3: filtering
    // ------------------------
    return PCF_Filter(uv, z, dz_duv, /*filterRadiusUV*/0.2, pcf_tc);
}

float shadow_pcss ( float4 tc, float4 P )			// tc - coords in projected light space, P - camera view space position
{
    float2 uv = tc.xy / tc.w;
    float z = tc.z / tc.w;

    // Compute gradient using ddx/ddy before any branching
    float2 dz_duv = DepthGradient(uv, z);

    // Eye-space z from the light's point of view
    float zEye = P.z;
    return PCSS_Shadow(uv, z, dz_duv, zEye, tc);
}
#endif
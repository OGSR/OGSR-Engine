#ifndef OGSE_BOKEH_BLUR_H
#define OGSE_BOKEH_BLUR_H
#include "common.h"
static const float2 resolution = ogse_c_resolution.xy*0.5;
static const float2 inv_resolution = ogse_c_resolution.zw*2;
#if !defined(DDOF_QUALITY)
	#define	DDOF_SAMPLES int(1)
	#define	DDOF_RINGS int(1)
#elif DDOF_QUALITY==1
	#define	DDOF_SAMPLES int(4)
	#define	DDOF_RINGS int(3)
#elif DDOF_QUALITY==2
	#define	DDOF_SAMPLES int(4)
	#define	DDOF_RINGS int(3)
#elif DDOF_QUALITY==3
	#define	DDOF_SAMPLES int(4)
	#define	DDOF_RINGS int(7)
#elif DDOF_QUALITY==4
	#define	DDOF_SAMPLES int(4)
	#define	DDOF_RINGS int(10)
#else
	#define	DDOF_SAMPLES int(1)
	#define	DDOF_RINGS int(1)
#endif
#define INV_DDOF_RINGS (1/DDOF_RINGS)
#define EPSDEPTH float(0.001)
float penta(float2 coords) {  				//pentagonal shape
	float scale = DDOF_RINGS - 1.3f;
	float4  HS0 = float4( 1.0,         0.0,         0.0,  1.0);
	float4  HS1 = float4( 0.309016994, 0.951056516, 0.0,  1.0);
	float4  HS2 = float4(-0.809016994, 0.587785252, 0.0,  1.0);
	float4  HS3 = float4(-0.809016994,-0.587785252, 0.0,  1.0);
	float4  HS4 = float4( 0.309016994,-0.951056516, 0.0,  1.0);
	float4  HS5 = float4( 0.0        ,0.0         , 1.0,  1.0);
	
	float4  one = float4( 1.0, 1.0, 1.0, 1.0 );
	
	float4 P = float4(coords.xy, scale, scale); 
	
	float4 dist = float4(0.0, 0.0, 0.0, 0.0);
	float inorout = -4.0;
	
	dist.x = dot( P, HS0 );
	dist.y = dot( P, HS1 );
	dist.z = dot( P, HS2 );
	dist.w = dot( P, HS3 );
	
	dist = smoothstep( -DDOF_FEATHER, DDOF_FEATHER, dist );
	
	inorout += dot( dist, one );
	
	dist.x = dot( P, HS4 );
	dist.y = HS5.w - abs( P.z );
	
	dist = smoothstep( -DDOF_FEATHER, DDOF_FEATHER, dist );
	inorout += dist.x;
	
	return saturate( inorout );
}
float bdepth(float2 coords) {				//blurring depth
	float d = 0.0;
	float kernel[9];
	float2 offset[9];
	
	float2 wh = inv_resolution.xy * DDOF_KERNEL;
	
	offset[0] = float2(-wh.x,-wh.y);
	offset[1] = float2( 0.0, -wh.y);
	offset[2] = float2( wh.x,-wh.y);
	
	offset[3] = float2(-wh.x,  0.0);
	offset[4] = float2( 0.0,   0.0);
	offset[5] = float2( wh.x,  0.0);
	
	offset[6] = float2(-wh.x, wh.y);
	offset[7] = float2( 0.0,  wh.y);
	offset[8] = float2( wh.x, wh.y);
	
	kernel[0] = 1.0/16.0;   kernel[1] = 2.0/16.0;   kernel[2] = 1.0/16.0;
	kernel[3] = 2.0/16.0;   kernel[4] = 4.0/16.0;   kernel[5] = 2.0/16.0;
	kernel[6] = 1.0/16.0;   kernel[7] = 2.0/16.0;   kernel[8] = 1.0/16.0;
	
	
	for( int i=0; i<9; i++ ) {
		float tmp = get_depth(coords + offset[i]);//tex2D(s_position, coords + offset[i]).z;
		if (is_sky(tmp)) tmp = DDOF_SKY_DIST;
		d += tmp * kernel[i];
	}
	
	return d;
}
float3 color(float2 coords,float blur, float2 color_scale) {	//processing the sample
	float3 col = float3(0.0, 0.0, 0.0);
#ifdef DDOF_CHROMATIC_ABERRATION
	col.r = tex2Dlod(s_image,float4(coords.xy,0,0) + float4(0.0,1.0,0.0,0.0)*float4(color_scale.xy,0,0)).r;
	col.g = tex2Dlod(s_image,float4(coords.xy,0,0) + float4(-0.866,-0.5,0.0,0.0)*float4(color_scale.xy,0,0)).g;
	col.b = tex2Dlod(s_image,float4(coords.xy,0,0) + float4(0.866,-0.5,0.0,0.0)*float4(color_scale.xy,0,0)).b;
#else
	col = tex2Dlod(s_image,float4(coords.xy,0,0)).xyz;
#endif

	float lum = dot(col.rgb, lumcoeff);
	float thresh = max((lum-DDOF_THRESHOLD)*DDOF_GAIN, 0.0);
	return (col+lerp(float3(0.0, 0.0, 0.0),col,thresh*blur));
}
float2 rand(float2 coord)  {	 			//generating noise/pattern texture for dithering
	float2 Noise;
	Noise.x = ((frac(1.0-coord.x*(resolution.x*0.5))*0.25)+(frac(coord.y*(resolution.y*0.5))*0.75))*2.0-1.0;
	Noise.y = ((frac(1.0-coord.x*(resolution.x*0.5))*0.75)+(frac(coord.y*(resolution.y*0.5))*0.25))*2.0-1.0;
	return Noise;
}
float3 debugFocus(float3 col, float blur, float depth) {
	float edge = 0.002*depth; //distance based edge smoothing
	float m = saturate(smoothstep(0.0,edge,blur));
	float e = saturate(smoothstep(1.0-edge,1.0,blur));
	
	col = lerp(col,float3(1.0,0.5,0.0),(1.0-m)*0.6);
	col = lerp(col,float3(0.0,0.5,1.0),((1.0-e)-(1.0-m))*0.2);

	return col;
}


float3 bokeh_dof(float2 center, float d, float blur) {
	
	//scene depth calculation	
	float depth = d;	
#ifdef DDOF_DEPTHBLUR
	depth = bdepth(center);
#endif
	
	// calculation of pattern for ditering	
	float2 noise_ = rand(center)*0.0001;
	
	// getting blur x and y step factor
	float2 texel = inv_resolution.xy;
	float2 scale = (texel*DDOF_KERNEL+noise_)*blur/DDOF_QUALITY;	
	// calculation of final color
	float3 col = tex2D(s_image, center).rgb;
	float2 color_scale = texel*DDOF_FRINGE*blur;
	if (blur >= 0.05) {			//some optimization thingy
		float s = 1.0;
		int ringsamples = 0;		
		for (int i = 1; i <= DDOF_RINGS; i++) {   
			ringsamples = i * DDOF_SAMPLES;
//			float a = sin(DOUBLE_PI/ringsamples);		// let's approximate sincos by resonant filter algoritm 
//			float2 cs = float2(1.f,0.f);
			for (int j = 0; j < ringsamples; j++) {
//				float2 pwh = cs;
//				cs.x -= cs.y*a;
//				cs.y += cs.x*a;
				float2 pwh = float2(1,1);
				float stp = DOUBLE_PI*j / ringsamples;
				sincos(stp, pwh.y, pwh.x);
				pwh *= i;
				float p = 1.0;
			#ifdef DDOF_PENTAGON
				p = penta(pwh);
			#endif
				col += color(center + pwh*scale,blur, color_scale)*lerp(1.0, i*INV_DDOF_RINGS, DDOF_BIAS)*p;
				s += lerp(1.0, i*INV_DDOF_RINGS, DDOF_BIAS)*p; 
			}
		}
		col /= s; //divide by sample count
	}
#ifdef DDOF_SHOWFOCUS
	col = debugFocus(col, blur, depth);
#endif
	
//#ifdef DDOF_VIGNETTING
//	col *= vignette(center);
//#endif

	return col;
}
#endif
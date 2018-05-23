#ifndef	OGSE_RAIN_H
#define OGSE_RAIN_H

#include "ogse_config.h"
#include "common.h"

uniform	sampler	s_rmap	: register(ps,s0);	// 2D/cube rainmap

//////////////////////////////////////////////////////////////////////////////////////////
// software
//////////////////////////////////////////////////////////////////////////////////////////
float 	sample_sw	(float2 tc, float2 shift, float depth_cmp)
{
	static const float 	ts = RMAP_KERNEL / float(SMAP_size);
	tc 		+= 		shift*ts;

	float  	texsize = RMAP_size;
	float  	offset 	= 0.5f/texsize;
	float2 	Tex00 	= tc + float2(-offset, -offset);
	float2 	Tex01 	= tc + float2(-offset,  offset);
	float2 	Tex10 	= tc + float2( offset, -offset);
	float2 	Tex11 	= tc + float2( offset,  offset);
	float4 	depth 	= float4(
		depth_cmp-tex2D	(s_rmap, Tex00).x,
		depth_cmp-tex2D	(s_rmap, Tex01).x,
		depth_cmp-tex2D	(s_rmap, Tex10).x,
		depth_cmp-tex2D	(s_rmap, Tex11).x);
	float4 	compare = step	(depth,0);
	float2 	fr 		= frac	(Tex00*texsize);
	float2 	ifr 	= float2	(1,1) - fr;
	float4 	fr4 	= float4	(ifr.x*ifr.y, ifr.x*fr.y, fr.x*ifr.y,  fr.x*fr.y);
	return	dot		(compare, fr4);
}
float 	rain_sw	(float4 tc)	{ 
	float2	tc_dw	= tc.xy / tc.w;
	float4	s;
	s.x	= sample_sw	(tc_dw,float2(-1,-1),tc.z); 
	s.y	= sample_sw	(tc_dw,float2(+1,-1),tc.z); 
	s.z	= sample_sw	(tc_dw,float2(-1,+1),tc.z); 
	s.w	= sample_sw	(tc_dw,float2(+1,+1),tc.z);
	return	dot		(s, 1.h/4.h);
}

//////////////////////////////////////////////////////////////////////////////////////////
// hardware + PCF
//////////////////////////////////////////////////////////////////////////////////////////

#ifdef USE_HWSMAP_PCF

float  	sample_hw_pcf	(float4 tc,float4 shift){
	static const float 	ts = RMAP_KERNEL / float(SMAP_size);
	return tex2Dproj	(s_rmap,tc + tc.w*shift*ts).x;
}
	
float 	rain_hw	(float4 tc)		{

#ifndef SOFT_RAIN
float4 s0;
	       s0.x	= sample_hw_pcf	(tc,float4(-1,-1,0,0));
	       s0.y	= sample_hw_pcf	(tc,float4(+1,-1,0,0));
	       s0.z	= sample_hw_pcf	(tc,float4(-1,+1,0,0));
	       s0.w	= sample_hw_pcf	(tc,float4(+1,+1,0,0));

	float result	= (s0.x + s0.y + s0.z + s0.w)/4.h;
						
	return result;
#else
		/* next part is copyrighted by cjayho */
	float  s0;
	int n = SOFT_SHADOWS;

	for( int i = 1; i <= n; i++ )
	{
		s0 += sample_hw_pcf	( tc, float4( -i, -i, 0, 0 ) ); 
		s0 += sample_hw_pcf	( tc, float4( +i, -i, 0, 0 ) ); 
		s0 += sample_hw_pcf	( tc, float4( -i, +i, 0, 0 ) ); 
		s0 += sample_hw_pcf	( tc, float4( +i, +i, 0, 0 ) ); 
		s0 += sample_hw_pcf	( tc, float4( 0, +i, 0, 0 ) ); 
		s0 += sample_hw_pcf	( tc, float4( 0, -i, 0, 0 ) ); 
		s0 += sample_hw_pcf	( tc, float4( +i, 0, 0, 0 ) ); 
		s0 += sample_hw_pcf	( tc, float4( -i, 0, 0, 0 ) ); 

		// “аким вот простым извратом мы получили 
		// нормальные тени и не такой ужасающий бамп текстур
	}
	n *= 8;
	return( s0 / n );		
#endif
}

float  	sample_hw_pcf_far	(float4 tc){
	static const float 	ts = RMAP_KERNEL / float(SMAP_size);
	
	float3 s0;
	  s0.x		= sample_hw_pcf	(tc,float4(0.f,0.f,0,0)); 
  	  s0.y		= sample_hw_pcf	(tc,float4(1.f,0.f,0,0)); 
  	  s0.z		= sample_hw_pcf	(tc,float4(2.f,0.f,0,0)); 
  	
	
	float3 s1;
	  s1.x		= sample_hw_pcf	(tc,float4(0.f,1.f,0,0)); 
  	  s1.y		= sample_hw_pcf	(tc,float4(1.f,1.f,0,0)); 
  	  s1.z		= sample_hw_pcf	(tc,float4(2.f,1.f,0,0));
	

	float3 s2;
	  s2.x		= sample_hw_pcf	(tc,float4(0.f,2.f,0,0)); 
  	  s2.y		= sample_hw_pcf	(tc,float4(1.f,2.f,0,0)); 
  	  s2.z		= sample_hw_pcf	(tc,float4(2.f,2.f,0,0));
	

	float2 Weight = frac((tc.xy / tc.w) + ts);

	float3 Result = lerp(lerp(s0, s1, Weight.xxx), lerp(s0, s2, Weight.xxx),Weight.yyy);

	return (Result);

}

		 
#endif
//////////////////////////////////////////////////////////////////////////////////////////
// hardware (ATI) + DF24/Fetch4
//////////////////////////////////////////////////////////////////////////////////////////

/*
float  	sample_hw_f4	(float4 tc,float4 shift){
	static const float 	ts 	= RMAP_KERNEL / 	float(SMAP_size);
	float4	D4				= tex2Dproj	(s_rmap,tc + tc.w*shift*ts);
	float4 	dcmp			= tc.z/tc.w	;
	float4	cmp				= dcmp<D4	;
	return 	dot	(cmp,1.h/4.h);
}
*/

float  	sample_hw_f4	(float4 tc,float4 shift){
	static const float 	ts 	= 	RMAP_KERNEL / 	float(SMAP_size);
	float4 	T4				= 	tc/tc.w		;
			T4.xy			+=	shift.xy*ts	;

	float4	D4				= 	tex2D		(s_rmap, T4);
	float4	compare			= 	T4.z<D4		;

	float  	texsize 		= 	RMAP_size	;
	float2 	fr 				= 	frac		(T4.xy * texsize);
	float2 	ifr 			= 	float2		(1,1) - fr;
	float4 	fr4 			= 	float4		(ifr.x*ifr.y, ifr.x*fr.y, fr.x*ifr.y,  fr.x*fr.y);
	float4 	fr4s		 	= 	fr4.zywx	;

	return	dot	(compare, fr4s)	;
	// return 	dot	(compare, 1.h/4.h)	;
}


float 	rain_hw_f4	(float4 tc)		{
  float  s0	= sample_hw_f4	(tc,float4(-1,-1,0,0)); 
  float  s1	= sample_hw_f4	(tc,float4(+1,-1,0,0)); 
  float  s2	= sample_hw_f4	(tc,float4(-1,+1,0,0)); 
  float  s3	= sample_hw_f4	(tc,float4(+1,+1,0,0));
	return	(s0+s1+s2+s3)/4.h;
}

float  	sample_hw_f4_far	(float4 tc){
	static const float 	ts = RMAP_KERNEL / float(SMAP_size);
	
	float3 s0;
	  s0.x		= sample_hw_f4	(tc,float4(0.f,0.f,0,0)); 
  	  s0.y		= sample_hw_f4	(tc,float4(1.f,0.f,0,0)); 
  	  s0.z		= sample_hw_f4	(tc,float4(2.f,0.f,0,0)); 
  	
	
	float3 s1;
	  s1.x		= sample_hw_f4	(tc,float4(0.f,1.f,0,0)); 
  	  s1.y		= sample_hw_f4	(tc,float4(1.f,1.f,0,0)); 
  	  s1.z		= sample_hw_f4	(tc,float4(2.f,1.f,0,0));
	

	float3 s2;
	  s2.x		= sample_hw_f4	(tc,float4(0.f,2.f,0,0)); 
  	  s2.y		= sample_hw_f4	(tc,float4(1.f,2.f,0,0)); 
  	  s2.z		= sample_hw_f4	(tc,float4(2.f,2.f,0,0));
	

	float2 Weight = frac((tc.xy / tc.w) + ts);

	float Result = lerp(lerp(s0, s1, Weight.xxx), lerp(s0, s2, Weight.xxx), Weight.yyy);

	return (Result);

}


//////////////////////////////////////////////////////////////////////////////////////////
// select hardware or software rainmaps
//////////////////////////////////////////////////////////////////////////////////////////
#ifdef	USE_HWSMAP_PCF
	// D24X8+PCF
	float 	rain		(float4	tc)		{ return rain_hw	(tc);	}
#else
	#ifdef USE_FETCH4
		// DF24+Fetch4
		float 	rain 		(float4 tc)		{ return rain_hw_f4(tc);	}
	#else
		// FP32
		float 	rain		(float4 tc) 	{ return rain_sw	(tc);	}
	#endif
#endif
//////////////////////////////////////////////////////////////////////////////////////////
// testbed

uniform sampler2D	jitter0;
uniform sampler2D	jitter1;
uniform sampler2D	jitter2;
uniform sampler2D	jitter3;
uniform float4 		jitterS;
float4 	test 		(float4 tc, float2 offset)
{
	float4	tcx	= float4 (tc.xy + tc.w*offset, tc.zw);
	return 	tex2Dproj (s_rmap,tcx);
}
float 	raintest 	(float4 tc, float4 tcJ)				// jittered sampling
{
	float4	r;

	const 	float 	scale 	= (4.0f/float(RMAP_size));
	float4	J0 	= tex2Dproj	(jitter0,tcJ)*scale;
	float4	J1 	= tex2Dproj	(jitter1,tcJ)*scale;

		r.x 	= test 	(tc,J0.xy).x;
		r.y 	= test 	(tc,J0.wz).y;
		r.z		= test	(tc,J1.xy).z;
		r.w		= test	(tc,J1.wz).x;

	return	dot(r,1.h/4.h);
}
uniform float4x4	m_rain;

#endif

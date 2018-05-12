#ifndef	SHADOW_H
#define SHADOW_H

#include "common.h"
#include "ogse_config.h"
#include "ogse_pcss.h"

#define	KERNEL	.6f
//////////////////////////////////////////////////////////////////////////////////////////
// software
//////////////////////////////////////////////////////////////////////////////////////////
float 	sample_sw	(float2 tc, float2 shift, float depth_cmp)
{
	static const float 	ts = KERNEL / float(SMAP_size);
	tc 		+= 		shift*ts;
	
	float  	texsize = SMAP_size;
	float  	offset 	= 0.5f/texsize;

	float2 	Tex00 	= tc + float2(-offset, -offset);
	float2 	Tex01 	= tc + float2(-offset,  offset);
	float2 	Tex10 	= tc + float2( offset, -offset);
	float2 	Tex11 	= tc + float2( offset,  offset);
	float4 	depth 	= float4(
		depth_cmp-tex2D	(s_smap, Tex00).x,
		depth_cmp-tex2D	(s_smap, Tex01).x,
		depth_cmp-tex2D	(s_smap, Tex10).x,
		depth_cmp-tex2D	(s_smap, Tex11).x);
	float4 	compare = step	(depth,0);
	float2 	fr 		= frac	(Tex00*texsize);
	float2 	ifr 	= float2	(1,1) - fr;
	float4 	fr4 	= float4	(ifr.x*ifr.y, ifr.x*fr.y, fr.x*ifr.y,  fr.x*fr.y);
	return	dot		(compare, fr4);
}

float 	shadow_sw	(float4 tc, float mod)	{ 
	float2	tc_dw	= tc.xy / tc.w;
	float4	s;
	s.x	= sample_sw	(tc_dw,float2(-mod,-mod),tc.z); 
	s.y	= sample_sw	(tc_dw,float2(+mod,-mod),tc.z); 
	s.z	= sample_sw	(tc_dw,float2(-mod,+mod),tc.z); 
	s.w	= sample_sw	(tc_dw,float2(+mod,+mod),tc.z);
	return	dot		(s, 1.h/4.h);
}

//////////////////////////////////////////////////////////////////////////////////////////
// hardware + PCF
//////////////////////////////////////////////////////////////////////////////////////////

#ifdef USE_HWSMAP_PCF

float  	sample_hw_pcf	(float4 tc,float4 shift){
	static const float 	ts = KERNEL / float(SMAP_size);
#ifndef SUNSHAFTS_DYNAMIC
	return tex2Dproj	(s_smap,tc + tc.w*shift*ts).x;
#else	//	SUNSHAFTS_DYNAMIC
	float4 tc2 = tc / tc.w + shift * ts;
	tc2.w = 0;
	return tex2Dlod(s_smap, tc2);
#endif	//	SUNSHAFTS_DYNAMIC
}
	
float 	shadow_hw	(float4 tc, float mod)		{

#ifndef SOFT_SHADOWS
float4 s0;
	       s0.x	= sample_hw_pcf	(tc,float4(-mod,-mod,0,0));
	       s0.y	= sample_hw_pcf	(tc,float4(+mod,-mod,0,0));
	       s0.z	= sample_hw_pcf	(tc,float4(-mod,+mod,0,0));
	       s0.w	= sample_hw_pcf	(tc,float4(+mod,+mod,0,0));

	float result	= (s0.x + s0.y + s0.z + s0.w)/4.h;
						
	return result;
#else
		/* next part is copyrighted by cjayho */
	float  s0 = 0;
	int n = SOFT_SHADOWS;

	for( int i = 1; i <= n; i++ )
	{
		s0 += sample_hw_pcf	( tc, float4( -i*mod, -i*mod, 0, 0 ) ); 
		s0 += sample_hw_pcf	( tc, float4( +i*mod, -i*mod, 0, 0 ) ); 
		s0 += sample_hw_pcf	( tc, float4( -i*mod, +i*mod, 0, 0 ) ); 
		s0 += sample_hw_pcf	( tc, float4( +i*mod, +i*mod, 0, 0 ) ); 
		s0 += sample_hw_pcf	( tc, float4( 0, +i*mod, 0, 0 ) ); 
		s0 += sample_hw_pcf	( tc, float4( 0, -i*mod, 0, 0 ) ); 
		s0 += sample_hw_pcf	( tc, float4( +i*mod, 0, 0, 0 ) ); 
		s0 += sample_hw_pcf	( tc, float4( -i*mod, 0, 0, 0 ) ); 

		// “аким вот простым извратом мы получили 
		// нормальные тени и не такой ужасающий бамп текстур
	}
	n *= 8;
	return( s0 / n );		
#endif
}

float  	sample_hw_pcf_far	(float4 tc){
	static const float 	ts = KERNEL / float(SMAP_size);
	
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

	return (Result.x);

}

		 
#endif
//////////////////////////////////////////////////////////////////////////////////////////
// hardware (ATI) + DF24/Fetch4
//////////////////////////////////////////////////////////////////////////////////////////

/*
float  	sample_hw_f4	(float4 tc,float4 shift){
	static const float 	ts 	= KERNEL / 	float(SMAP_size);
	float4	D4				= tex2Dproj	(s_smap,tc + tc.w*shift*ts);
	float4 	dcmp			= tc.z/tc.w	;
	float4	cmp				= dcmp<D4	;
	return 	dot	(cmp,1.h/4.h);
}
*/

float  	sample_hw_f4	(float4 tc,float4 shift){
	static const float 	ts 	= 	KERNEL / 	float(SMAP_size);
	float4 	T4				= 	tc/tc.w		;
			T4.xy			+=	shift.xy*ts	;

	float4	D4				= 	tex2D		(s_smap, T4.xy);
	float4	compare			= 	T4.z<D4		;

	float  	texsize 		= 	SMAP_size	;
	float2 	fr 				= 	frac		(T4.xy * texsize);
	float2 	ifr 			= 	float2		(1,1) - fr;
	float4 	fr4 			= 	float4		(ifr.x*ifr.y, ifr.x*fr.y, fr.x*ifr.y,  fr.x*fr.y);
	float4 	fr4s		 	= 	fr4.zywx	;

	return	dot	(compare, fr4s)	;
	// return 	dot	(compare, 1.h/4.h)	;
}


float 	shadow_hw_f4	(float4 tc, float mod)		{
#ifndef SOFT_SHADOWS
float4 s0;
	       s0.x	= sample_hw_f4	(tc,float4(-mod,-mod,0,0));
	       s0.y	= sample_hw_f4	(tc,float4(+mod,-mod,0,0));
	       s0.z	= sample_hw_f4	(tc,float4(-mod,+mod,0,0));
	       s0.w	= sample_hw_f4	(tc,float4(+mod,+mod,0,0));

	float result	= (s0.x + s0.y + s0.z + s0.w)/4.h;
						
	return result;
#else
		/* next part is copyrighted by cjayho */
	float  s0 = 0;
	int n = SOFT_SHADOWS;

	for( int i = 1; i <= n; i++ )
	{
		s0 += sample_hw_f4	( tc, float4( -i*mod, -i*mod, 0, 0 ) ); 
		s0 += sample_hw_f4	( tc, float4( +i*mod, -i*mod, 0, 0 ) ); 
		s0 += sample_hw_f4	( tc, float4( -i*mod, +i*mod, 0, 0 ) ); 
		s0 += sample_hw_f4	( tc, float4( +i*mod, +i*mod, 0, 0 ) ); 
		s0 += sample_hw_f4	( tc, float4( 0, +i*mod, 0, 0 ) ); 
		s0 += sample_hw_f4	( tc, float4( 0, -i*mod, 0, 0 ) ); 
		s0 += sample_hw_f4	( tc, float4( +i*mod, 0, 0, 0 ) ); 
		s0 += sample_hw_f4	( tc, float4( -i*mod, 0, 0, 0 ) ); 

		// “аким вот простым извратом мы получили 
		// нормальные тени и не такой ужасающий бамп текстур
	}
	n *= 8;
	return( s0 / n );		
#endif
}

float  	sample_hw_f4_far	(float4 tc){
	static const float 	ts = KERNEL / float(SMAP_size);
	
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

	float3 Result = lerp(lerp(s0, s1, Weight.xxx), lerp(s0, s2, Weight.xxx), Weight.yyy);

	return (Result.x);

}


//////////////////////////////////////////////////////////////////////////////////////////
// select hardware or software shadowmaps
//////////////////////////////////////////////////////////////////////////////////////////
#ifndef USE_PCSS
	#ifdef	USE_HWSMAP_PCF
		// D24X8+PCF
		float 	shadow		(float4	tc, float4 P, float mod)		{ return shadow_hw	(tc, mod);	}
	#else
		#ifdef USE_FETCH4
			// DF24+Fetch4
			float 	shadow 		(float4 tc, float4 P, float mod)		{ return shadow_hw_f4(tc, mod);	}
		#else
			// FP32
			float 	shadow		(float4 tc, float4 P, float mod) 	{ return shadow_sw	(tc, mod);	}
		#endif
	#endif
#else
	float shadow	(float4 tc, float4 P, float mod)	{return shadow_pcss(tc, P);}
#endif

// sampling fo transparent geometry
#ifdef USE_TRANSPARENT_GEOM
float sample_shadow_near(float4	tc, float4 P, float mod)
{
	static const float 	ts = KERNEL / float(SMAP_size);
	float4 s0;
	s0.x = tex2Dproj	(s_smap_near,tc + tc.w*float4(-1,-1,0,0)*ts).x;
	s0.y = tex2Dproj	(s_smap_near,tc + tc.w*float4(+1,-1,0,0)*ts).x;
	s0.z = tex2Dproj	(s_smap_near,tc + tc.w*float4(-1,+1,0,0)*ts).x;
	s0.w = tex2Dproj	(s_smap_near,tc + tc.w*float4(+1,+1,0,0)*ts).x;
	float result	= (s0.x + s0.y + s0.z + s0.w)*0.25h;
						
	return result;
}

float sample_shadow_far(float4	tc, float4 P, float mod)
{
	static const float 	ts = KERNEL / float(SMAP_size);
	return tex2Dproj	(s_smap_far,tc + tc.w*ts).x;
}
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
	return 	tex2Dproj (s_smap,tcx);
}
float 	shadowtest 	(float4 tc, float4 tcJ)				// jittered sampling
{
	float4	r;

	const 	float 	scale 	= (2.7f/float(SMAP_size));
	float4	J0 	= tex2Dproj	(jitter0,tcJ)*scale;
	float4	J1 	= tex2Dproj	(jitter1,tcJ)*scale;

		r.x 	= test 	(tc,J0.xy).x;
		r.y 	= test 	(tc,J0.wz).y;
		r.z		= test	(tc,J1.xy).z;
		r.w		= test	(tc,J1.wz).x;

	return	dot(r,1.h/4.h);
}
float 	shadowtest_sun 	(float4 tc, float4 tcJ)			// jittered sampling
{
	float4	r;

	const 	float 	scale 	= (2.0f/float(SMAP_size));
	float4	J0 	= tex2D	(jitter0,tcJ.xy)*scale;
	float4	J1 	= tex2D	(jitter1,tcJ.xy)*scale;

		r.x 	= test 	(tc,J0.xy).x;
		r.y 	= test 	(tc,J0.wz).y;
		r.z		= test	(tc,J1.xy).z;
		r.w		= test	(tc,J1.wz).x;

	return	dot(r,1.h/4.h);
}
//////////////////////////////////////////////////////////////////////////////////////////
#ifdef  USE_SUNMASK	
uniform float3x4	m_sunmask	;				// ortho-projection
float 	sunmask		(float4 P)	{				// 
	float3 		tc	= mul	(m_sunmask, P);		//
	return 		tex2D 	(s_lmap,tc.xy).w;		// A8 
	
}
uniform float3x4	m_shadow_clouds_ogse	;				// ortho-projection
float 	sunmask_trans		(float4 P)	{				// 
	float3 		tc	= mul	(m_shadow_clouds_ogse, P);		//
	return 		tex2D 	(s_lmap,tc.xy).w;		// A8 
	
}
#else
float 	sunmask		(float4 P)	{ return 1.h; }	// 
float 	sunmask_trans		(float4 P)	{ return 1.h; }	// 
#endif
//////////////////////////////////////////////////////////////////////////////////////////
uniform float4x4	m_shadow;

#endif

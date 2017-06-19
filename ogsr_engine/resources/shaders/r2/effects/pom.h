/*
	Parallax Occlusion Mapping shader
	Basic idea - ATI
	Realization - GSC (Stalker Call of Pripyat)
*/

#ifndef POM_H
#define POM_H

#if !defined(ALLOW_STEEP_PARALLAX)
	#define	POM_MAX_SAMPLES int(1)
	#define	POM_MIN_SAMPLES int(1)
	#define	POM_FINAL_INTERSECTION_LOOPS int (1)
#elif ALLOW_STEEP_PARALLAX==1
	#define	POM_MAX_SAMPLES int(15)
	#define	POM_MIN_SAMPLES int(3)
	#define	POM_FINAL_INTERSECTION_LOOPS int (3)
#elif ALLOW_STEEP_PARALLAX==2
	#define	POM_MAX_SAMPLES int(30)
	#define	POM_MIN_SAMPLES int(5)
	#define	POM_FINAL_INTERSECTION_LOOPS int (5)
#elif ALLOW_STEEP_PARALLAX==3
	#define	POM_MAX_SAMPLES int(40)
	#define	POM_MIN_SAMPLES int(10)
	#define	POM_FINAL_INTERSECTION_LOOPS int (7)
#elif ALLOW_STEEP_PARALLAX==4
	#define	POM_MAX_SAMPLES int(60)
	#define	POM_MIN_SAMPLES int(20)
	#define	POM_FINAL_INTERSECTION_LOOPS int (10)
#endif

static const float fParallaxStartFade = 8.0f;
static const float fParallaxStopFade = 12.0f;
		
#if defined(ALLOW_STEEP_PARALLAX) && defined(USE_PARALLAX_OCCLUSION) && !defined(USE_PARALLAX)
void update_texcoords(inout p_bumped I)
{
	if (I.position.z < fParallaxStopFade)
	{
		float3	 eye = mul (float3x3(I.M1.x, I.M2.x, I.M3.x,
									 I.M1.y, I.M2.y, I.M3.y,
									 I.M1.z, I.M2.z, I.M3.z), -I.position.xyz);
		
		eye = normalize(eye);
		
		//	Calculate number of steps
		float nNumSteps = lerp( POM_MAX_SAMPLES, POM_MIN_SAMPLES, eye.z );

		float	fStepSize			= 1.0 / nNumSteps;
		float2	vDelta				= -eye.xy * POM_PARALLAX_OFFSET*1.2;
		float2	vTexOffsetPerStep	= fStepSize * vDelta;

		//	Prepare start data for cycle
		float2	vTexCurrentOffset	= I.tcdh;
		float	fCurrHeight			= 0.0;
		float	fCurrentBound		= 1.0;

		for( ;fCurrHeight < fCurrentBound; fCurrentBound -= fStepSize )
		{
			vTexCurrentOffset += vTexOffsetPerStep;		
			fCurrHeight = tex2Dlod( s_bumpX, float4(vTexCurrentOffset.xy,0,0) ).a; 
		}

		//	Reconstruct previouse step's data
		vTexCurrentOffset -= vTexOffsetPerStep;
		float fPrevHeight = tex2D( s_bumpX, float3(vTexCurrentOffset.xy,0) ).a;

		//	Smooth tc position between current and previouse step
		float	fDelta2 = ((fCurrentBound + fStepSize) - fPrevHeight);
		float	fDelta1 = (fCurrentBound - fCurrHeight);
		float	fParallaxAmount = (fCurrentBound * fDelta2 - (fCurrentBound + fStepSize) * fDelta1 ) / ( fDelta2 - fDelta1 );
		float	fParallaxFade 	= smoothstep(fParallaxStopFade, fParallaxStartFade, I.position.z);
		float2	vParallaxOffset = vDelta * ((1- fParallaxAmount )*fParallaxFade);
		float2	vTexCoord = I.tcdh + vParallaxOffset;
	
		//	Output the result
		I.tcdh = vTexCoord;
#if defined(USE_TDETAIL)
		I.tcdbump = vTexCoord * dt_params;
#endif
	}
}
#elif defined(USE_PARALLAX)
	void update_texcoords(inout p_bumped I)
	{
		float3	 eye = mul (float3x3(I.M1.x, I.M2.x, I.M3.x,
								 I.M1.y, I.M2.y, I.M3.y,
								 I.M1.z, I.M2.z, I.M3.z), -I.position.xyz);
		float height	= tex2D(s_bumpX, I.tcdh).w;                                //
		height = height*(parallax.x) + (parallax.y);                        //
		float2 new_tc = I.tcdh + height * normalize(eye);                //
		I.tcdh.xy = new_tc;
	}
#else
	void update_texcoords(inout p_bumped I)
	{
		;
	}
#endif
#endif
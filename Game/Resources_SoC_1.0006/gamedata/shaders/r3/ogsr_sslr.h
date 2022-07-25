////////////////////////////////////
// Screen Space Local Reflections //
// Автор: Baryshev194 ( https://github.com/Baryshev194 )
// Доработка: xrSimpodin
// Использован некоторый код из https://github.com/aosyang/FSGraphicsProject/blob/d979ac22dca53a4cf3e3e1a47bddf5dab6a6401f/DemoDeferredShading/ScreenSpaceRayTracing.hlsl
////////////////////////////////////

uniform float4 SSLR_params; // x - L
uniform float4x4 m_inv_v; 
uniform float4 screen_res;


float3 calc_envmap(float3 vreflect)
{
	const float3 vreflectabs = abs(vreflect);
	const float vreflectmax = max(vreflectabs.x, max(vreflectabs.y, vreflectabs.z));
	vreflect /= vreflectmax;
	if (vreflect.y < 0.999)
			vreflect.y= vreflect.y * 2 - 1; // fake remapping

	const float3 env0 = env_s0.SampleLevel(smp_base, vreflect.xyz, 0).xyz;
	const float3 env1 = env_s1.SampleLevel(smp_base, vreflect.xyz, 0).xyz;
	return lerp(env0, env1, L_ambient.w);
}

float3 getWpos(float2 tc0, float2 tc, float2 hpos, uint iSample)
{
	const gbuffer_data gbd = gbuffer_load_data_offset( GLDO_P(tc0, tc, hpos, iSample) );

	return mul(m_inv_v, float4(gbd.P.rgb, 1.f)).xyz;
}

void getWposWnorm(float2 tc, float2 hpos, out float3 Wpos, out float3 Wnorm, uint iSample)
{
	const gbuffer_data gbd = gbuffer_load_data( GLD_P(tc, hpos, iSample) );

	Wpos = mul(m_inv_v, float4(gbd.P.rgb, 1.f)).xyz;

	Wnorm = mul(m_inv_v, float4(gbd.N.rgb, 0.f)).xyz;
	Wnorm.y *= clamp(gbd.P.z, 1.f, 10.f);
	Wnorm = normalize(Wnorm);
}


float3 calc_sslr(float2 tc0, float4 hpos, float3 color, uint iSample)
{
	float3 pos;
	float3 norm;
	getWposWnorm(tc0, hpos, pos, norm, iSample);

	const float3 eye = normalize(pos - eye_position);

	const float3 refl = normalize(reflect(eye, norm));

	// output rDotV to the alpha channel for use in determining how much to fade the ray
	const float rDotV = saturate(dot(refl, eye));
	const float nDotV = 1.0 - saturate(dot(norm, -eye) * 2.0);

	float2 refl_tc;
	float L = SSLR_params.x;

	[unroll(6)] for (int i = 0; i < 6; i++)
	{
		float3 Npos = pos.xyz + refl * L;

		float4 Npos_proj = mul(m_VP, float4(Npos, 1.f));
		Npos_proj.xyz /= Npos_proj.w;
		float2 sample_tc;
		sample_tc.x = (Npos_proj.x + 1.f) * .5f;
		sample_tc.y = 1.f - ((Npos_proj.y + 1.f) * .5f);

		float3 Hpos = getWpos(tc0, sample_tc, hpos, iSample);
		L = length(Hpos - pos);
		refl_tc = sample_tc;
	}

	// move hit pixel from pixel position to UVs
	const bool intersection = !(refl_tc.x > 1.0f || refl_tc.x < 0.0f || refl_tc.y > 1.0f || refl_tc.y < 0.0f);

	// Same way of building color and lighting in "combine_1.ps" but we can sample a especific uv
	const gbuffer_data rgbd = gbuffer_load_data(GLD_P(refl_tc, refl_tc * screen_res.xy, iSample));

	const float4 rD = float4(rgbd.C.rgb, rgbd.gloss);

#ifndef USE_MSAA
	const float4 rL = s_accumulator.Sample( smp_nofilter, refl_tc ); // diffuse.specular
#else
#ifdef GBUFFER_OPTIMIZATION
	const float4 rL = s_accumulator.Load( int3( refl_tc, 0 ), iSample ); // diffuse.specular
#else
	const float4 rL = s_accumulator.Load( int3( refl_tc * pos_decompression_params2.xy, 0 ), iSample );
#endif
#endif

	// hemisphere
	float3 hdiffuse, hspecular;
	hmodel(hdiffuse, hspecular, rgbd.mtl, rgbd.hemi, rgbd.gloss, rgbd.P, rgbd.N);

	// Final color
	const float4 light = float4(rL.rgb + hdiffuse, rL.w);
	const float4 C = rD * light;
	const float3 spec = C.www * rL.rgb + hspecular * C.rgba;

	const float3 ssReflection = (C.rgb + spec) * rDotV;

	const float ssr_amount = rDotV * (intersection ? 1.0f : 0.0f);

	const float4 cubeReflection = float4((calc_envmap(refl)) * 0.4, 1.0f);

	return color + lerp(cubeReflection, ssReflection, ssr_amount) * nDotV;
}

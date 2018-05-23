#ifndef OGSE_REFLECTIONS_H
#define OGSE_REFLECTIONS_H
#ifndef REFLECTIONS_QUALITY
	#define	REFLECTIONS_QUALITY 1
#endif
#define EPS 0.001
//#ifdef USE_HQ_REFLECTIONS
	static const float2 resolution = ogse_c_resolution.xy;
	static const float2 inv_resolution = ogse_c_resolution.zw;
/*#else
	static const float2 resolution = ogse_c_resolution.xy*0.5;
	static const float2 inv_resolution = ogse_c_resolution.zw*2;
#endif*/

#define REFL_WATER 0
#define REFL_GROUND 1
#define REFL_BOTH 2
	
uniform samplerCUBE s_env0;
uniform samplerCUBE s_env1;
float4 get_reflection (float3 screen_pixel_pos, float3 next_screen_pixel_pos, float3 reflect)
{
	float4 final_color = {1.0,1.0,1.0,1.0};
	float2 factors = {1.f,1.f};
	
	float3 main_vec = next_screen_pixel_pos - screen_pixel_pos;
	float3 grad_vec = main_vec / (max(abs(main_vec.x), abs(main_vec.y)) * 256);
	
	// handle case when reflect vector faces the camera
	factors.x = dot(eye_direction, reflect);

	[branch]
	if (factors.x < -0.5)
		return final_color;
		
	float3 curr_pixel = screen_pixel_pos;
	curr_pixel.xy += float2(0.5,0.5)*ogse_c_resolution.zw;
	float max_it = 140;

	#if (REFLECTIONS_QUALITY == 1)
		grad_vec *= 2;
		max_it *= 0.5;
	#endif
	
	float i = 0;
	
	while (i < max_it)
	{
		curr_pixel.xyz += grad_vec.xyz;
/*		if ((curr_pixel.x > 0.99) || (curr_pixel.y > 0.99) || (curr_pixel.x < 0.01) || (curr_pixel.y < 0.01)) 
		{
			final_color.xyz = float3(1.0,0.0,0.0);
			break;
		}*/
		float depth = get_depth_fast(curr_pixel.xy);
		depth = lerp(depth, 0.f, is_sky(depth));
//		depth += 1000*step(depth, 1.0);
		float delta = step(depth, curr_pixel.z)*step(screen_pixel_pos.z, depth);
		if (delta > 0.5)
//		if ((depth < curr_pixel.z) && (screen_pixel_pos.z < depth)) 
		{
//#if (REFLECTIONS_QUALITY == 1)
//			float2 tc = floor(curr_pixel.xy*ogse_c_resolution.xy);
//			tc *= ogse_c_resolution.zw;
//			final_color.xyz = tex2Dlod(s_image, float4(tc.xy,0,0)).xyz;
			final_color.xyz = tex2Dlod(s_image, float4(curr_pixel.xy,0,0)).xyz;
			float2 temp = curr_pixel.xy;
			// make sure that there is no fade down the screen
			temp.y = lerp(temp.y, 0.5, step(0.5, temp.y));
			float screendedgefact = saturate(distance(temp , float2(0.5, 0.5)) * 2.0);
			final_color.w = pow(screendedgefact,6);// * screendedgefact;
//#endif
			break;
		}
		i += 1.0;
	}

#if (REFLECTIONS_QUALITY == 2)
	if (i >= max_it) return final_color;
	curr_pixel.xyz -= grad_vec.xyz;
	grad_vec *= 0.125;
	for (int i = 0; i < 8; ++i)
	{
		curr_pixel.xyz += grad_vec.xyz;
/*		if ((curr_pixel.x > 1.0) || (curr_pixel.y > 1.0) || (curr_pixel.x < 0.0) || (curr_pixel.y < 0.0)) 
		{
			final_color.xyz = float3(1.0,0.0,0.0);
			break;
		}*/
		float depth = get_depth_fast(curr_pixel.xy);
		depth = lerp(depth, 0.f, is_sky(depth));

//		depth += 1000*step(depth, 1.0);
		float delta = step(depth, curr_pixel.z)*step(screen_pixel_pos.z, depth);
		if (delta > 0.5)
		{
			// edge detect
			final_color.xyz = tex2Dlod(s_image, float4(curr_pixel.xy,0,0)).xyz;
			float2 temp = curr_pixel.xy;
			// make sure that there is no fade down the screen
			temp.y = lerp(temp.y, 0.5, step(0.5, temp.y));
			float screendedgefact = saturate(distance(temp , float2(0.5, 0.5)) * 2.0);
			final_color.w = pow(screendedgefact,6);// * screendedgefact;
			break;
		}
		++i;
	}
#endif
	return final_color;
}
float3 calc_envmap(float3 vreflect)
{
//	vreflect.y = vreflect.y*2-1;
	float3	env0	= texCUBElod (s_env0, float4(vreflect.xyz, 0)).xyz;
	float3	env1	= texCUBElod (s_env1, float4(vreflect.xyz, 0)).xyz;
	return lerp (env0,env1,L_ambient.w);
}
float4 calc_reflections(float4 pos, float3 vreflect)
{
	float4 refl = {1.0,1.0,1.0,1.0};
#ifdef USE_REFLECTIONS
	float3 v_pixel_pos = mul((float3x4)m_V, pos);
	float4 p_pixel_pos = mul(m_VP, pos);
	float4 s_pixel_pos = proj_to_screen(p_pixel_pos);
	s_pixel_pos.xy /= s_pixel_pos.w;
	s_pixel_pos.z = v_pixel_pos.z;
		
	float3 reflect_vec = normalize(vreflect);
	float3 W_m_point = pos.xyz + reflect_vec;

	float3 V_m_point = mul((float3x4)m_V, float4(W_m_point, 1.0));
	float4 P_m_point = mul(m_VP, float4(W_m_point, 1.0));
	float4 S_m_point = proj_to_screen(P_m_point);
	S_m_point.xy /= S_m_point.w;
	S_m_point.z = V_m_point.z;
		
	refl = get_reflection(s_pixel_pos.xyz, S_m_point.xyz, reflect_vec);
#endif
	return refl;
}
float4 calc_reflections_late_out(float4 pos, float3 vreflect, float sw)
{
	float4 refl = {1.0,1.0,1.0,1.0};

#ifdef USE_REFLECTIONS
	
	float3 v_pixel_pos = mul((float3x4)m_V, pos);
	float4 p_pixel_pos = mul(m_VP, pos);
	float4 s_pixel_pos = proj_to_screen(p_pixel_pos);
	s_pixel_pos.xy /= s_pixel_pos.w;
	s_pixel_pos.z = v_pixel_pos.z;
		
	float3 reflect_vec = normalize(vreflect);
	float3 W_m_point = pos.xyz + reflect_vec;

	float3 V_m_point = mul((float3x4)m_V, float4(W_m_point, 1.0));
	float4 P_m_point = mul(m_VP, float4(W_m_point, 1.0));
	float4 S_m_point = proj_to_screen(P_m_point);
	S_m_point.xy /= S_m_point.w;
	S_m_point.z = V_m_point.z;

	if (sw > 0.01)
		refl = get_reflection(s_pixel_pos.xyz, S_m_point.xyz, reflect_vec);
#endif
	return refl;
}
#endif
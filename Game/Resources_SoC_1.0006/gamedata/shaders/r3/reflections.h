#ifndef REFLECTIONS_H
#define REFLECTIONS_H
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //Anomaly shaders 1.5																 				 			  //
  //Credits to KD, Anonim, Crossire, daemonjax, Zhora Cementow, Meltac, X-Ray Oxygen, FozeSt, Zagolski, SonicEthers, //
 //David Hoskins, BigWIngs																							//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//*******************************************************************************************************************
#ifdef USE_OGSE_REFLECTIONS

#define REFL_RANGE 100
#define SKY_DEPTH float(10000.f)
#define SKY_EPS float(0.001)
uniform float3 eye_direction;
uniform float4 screen_res;

float4 proj_to_screen(float4 proj)
{
	float4 screen = proj;
	screen.x = (proj.x + proj.w);
	screen.y = (proj.w - proj.y);
	screen.xy *= 0.5;
	return screen;
}

float get_depth_fast(float2 tc)
{
#ifndef USE_MSAA
	return s_position.Sample( smp_nofilter, tc).z;
#else
	return s_position.Load( int3( tc * pos_decompression_params2.xy ,0),0 ).z;
#endif
}

float is_sky(float depth) { return step(abs(depth - SKY_DEPTH), SKY_EPS); }

float4 get_reflection(float3 screen_pixel_pos, float3 next_screen_pixel_pos, float3 reflect)
{
	float4 final_color = {1.0,1.0,1.0,1.0};
	float2 factors = {1.f,1.f};
	
	float3 main_vec = next_screen_pixel_pos - screen_pixel_pos;
	float3 grad_vec = main_vec / (max(abs(main_vec.x), abs(main_vec.y)) * 256);
	
	// handle case when reflect vector faces the camera
	factors.x = dot(eye_direction, reflect);

	if ((factors.x < -0.5) || (screen_pixel_pos.z > REFL_RANGE)) return final_color;
	else
	{
		float3 curr_pixel = screen_pixel_pos;
		curr_pixel.xy += float2(0.5,0.5)*screen_res.zw;
		float max_it = 64;
		float i = 0;
		bool br = false;
		
		while ((i < max_it) && (br == false))
		{
			curr_pixel.xyz += grad_vec.xyz;
			float depth = get_depth_fast(curr_pixel.xy);
			depth = lerp(depth, 0.f, is_sky(depth));
			float delta = step(depth, curr_pixel.z)*step(screen_pixel_pos.z, depth);
			if (delta > 0.5)
			{
				final_color.xyz = s_image.Sample( smp_rtlinear, curr_pixel.xy).xyz;
				float2 tmp = curr_pixel.xy;
				tmp.y = lerp(tmp.y, 0.5, step(0.5, tmp.y));
				float screendedgefact = saturate(distance(tmp , float2(0.5, 0.5)) * 2.0);
				final_color.w = pow(screendedgefact,6);
				br = true;
			}
			i += 1.0;
		}
		return lerp(final_color,float4(1.0,1.0,1.0,1.0),screen_pixel_pos.z/REFL_RANGE);
	}
}

float4 calc_reflections(float4 pos, float3 vreflect)
{
	float4 refl = {1.0,1.0,1.0,1.0};
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
	
	return refl;
}
#endif
//*******************************************************************************************************************
//*******************************************************************************************************************
TextureCube	s_env0;
TextureCube	s_env1;

float3 calc_envmap(float3 vreflect)
{
	float3 vreflectabs = abs(vreflect);
	float  vreflectmax = max(vreflectabs.x, max(vreflectabs.y, vreflectabs.z));
	vreflect /= vreflectmax;
	if (vreflect.y < 0.999)
			vreflect.y= vreflect.y * 2 - 1; // fake remapping

	float3 env0 = s_env0.SampleLevel(smp_base, vreflect.xyz, 0).xyz;
	float3 env1 = s_env1.SampleLevel(smp_base, vreflect.xyz, 0).xyz;
	return lerp(env0, env1, L_ambient.w);
}
//*******************************************************************************************************************
//*******************************************************************************************************************
float3 specular_phong(float3 pnt, float3 normal, float3 light_direction)
{
    return L_sun_color.rgb * pow( abs( dot(normalize(pnt + light_direction), normal) ), 256.0);
}
//*******************************************************************************************************************
//*******************************************************************************************************************
uniform float4 rain_params; //x = raindensity

float hash12(float2 p)
{
	float3 p3  = frac(float3(p.xyx) * .1031);
    p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.x + p3.y) * p3.z);
}

float2 hash22(float2 p)
{
	float3 p3 = frac(float3(p.xyx) * float3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yzx+19.19);
    return frac((p3.xx+p3.yz)*p3.zy);
}

float ripples(float2 tc, float size)
{
	float2 uv = tc * size;
    float2 p0 = floor(uv);

    float2 circles;
    for (int j = -2; j <= 2; ++j)
    {
        for (int i = -2; i <= 2; ++i)
        {
			float2 pi = p0 + float2(i, j);
            float2 hsh = hash22(pi);
            float2 p = pi + hash22(hsh);

            float t = frac(0.35f*timers.x + hash12(hsh));
            float2 v = p - uv;
            float d = length(v) - (float(2.f) + 1.)*t;

            float h = 1e-3;

			//1st pass
            float d1 = d - h;
            float p1 = sin(31.*d1) * smoothstep(-0.6, -0.3, d1) * smoothstep(0., -0.3, d1);
			//sec pass
            float d2 = d + h;
            float p2 = sin(31.*d2) * smoothstep(-0.6, -0.3, d2) * smoothstep(0., -0.3, d2);
            circles += 0.5 * normalize(v) * ((p2 - p1) / (2. * h) * (1. - t) * (1. - t));
        }
    }
    circles /= float((2*3+1)*(2*2+1));

    float intensity = lerp(0.025f, 0.15, smoothstep(0.1, 0.6, frac(0.05 + 0.05)*2.-1.));
    float3 n = float3(circles, sqrt(1. - dot(circles, circles)));
	
	float final = 10.*pow(clamp(dot(n, normalize(float3(1., 0.7, 0.5))), 0., 1.), 6.);
	return final;
}
//*******************************************************************************************************************

#endif
#ifndef REFLECTIONS_H
#define REFLECTIONS_H
//*******************************************************************************************************************
/*
	\\\\\\Screen Space Reflections//////

Credits goes to Xerxes1138, Danil Baryshev, and ForHaxed.

References:
https://habr.com/ru/post/244367/
https://github.com/Xerxes1138/UnitySSR/
https://www.amk-team.ru/forum/topic/14078-sslr

If you want to use this code in your project
just keep this header ;) Make modding better.

By LVutner for X-Ray Oxygen project (2020)
*/

//#define SSR_JITTER //Enables jittering

#define SSR_EDGE_ATTENUATION float(0.09) //Edge attenuation intensity
#define SSR_JITTER_INTENSITY float(0.05) //Jittering intensity

#define SSR_SAMPLES int(20) // Extreme
#define SSR_DISTANCE float(400.0)

#if defined(USE_MSAA)
TEXTURE2DMS(float4, MSAA_SAMPLES) s_last_frame;
#else
Texture2D s_last_frame;
#endif

/*Helper functions*/
float RayAttenBorder(float2 pos, float value)
{
	float borderDist = min(1.0 - max(pos.x, pos.y), min(pos.x, pos.y));
	return saturate(borderDist > value ? 1.0 : borderDist / value);
}

float4 proj_to_screen(float4 proj)
{
	float4 screen = proj;
	screen.x = (proj.x + proj.w);
	screen.y = (proj.w - proj.y);
	screen.xy *= 0.5;
	return screen;
}

half is_sky(float depth)
{
	return step(abs(depth - 10000.f), 0.001);
}

#ifdef SSR_JITTER
float3 hash(float3 a)
{
	a *= timers.x;
    a = frac(a * 0.8f);
    a += dot(a, a.yxz + 19.19);
    return frac((a.xxy + a.yxx)*a.zyx);
}
#endif

float4 compute_ssr(float3 position, float3 normal)
{
	/*Initialize step size and error*/
	float step = 1.0f/float(SSR_SAMPLES);
	int reflection = 1;

	/*Initialize reflected TC*/
	float2 refl_tc = (0,0);	

	/*Prepare ray direction and reflection vector*/
	float3 v2point = normalize(position - eye_position);
	float3 vreflect = normalize(reflect(v2point,normalize(normal)));

	/*Main loop*/
	[unroll (SSR_SAMPLES)] for(int i = 0; i < SSR_SAMPLES; i++)
	{
		/*Prepare new position*/
		float3 new_position = position + vreflect * step;

		/*Add hash to new position*/		
#ifdef SSR_JITTER
		new_position += hash(position.xyz) * SSR_JITTER_INTENSITY;
#endif

		/*Convert new position to texcoord*/
		float4 proj_position = mul(m_VP, float4(new_position, 1.f));
		float4 p2ss = proj_to_screen(proj_position);
		refl_tc.xy = p2ss.xy /= p2ss.w;

		/*Sample hit depth*/
		float hit_depth = s_position.Load(int3(refl_tc.xy * screen_res.xy,0),0).z;

		/*Intersect sky from hit depth*/
		hit_depth = lerp(hit_depth, 0.f, is_sky(hit_depth));

		/*Sample depth*/
		float depth = mul(m_V, float4(position, 1.f)).z;	

		/*Fixing incorrect refls*/
		if((depth - hit_depth) > 0.0f || (hit_depth > SSR_DISTANCE))
			reflection = 0;

		/*Depth difference*/
		step = length(hit_depth - depth);		
	}

	/*Edge attenuation*/
	float edge = RayAttenBorder(refl_tc.xy, SSR_EDGE_ATTENUATION);

	/*Sample image with reflected TC*/	
#if defined(USE_MSAA)
    float3 img = s_last_frame.Load(refl_tc.xy * screen_res.xy, 0);
#else
    float3 img = s_last_frame.Sample(smp_nofilter, refl_tc.xy);
#endif

	/*Image.rgb, Reflcontrol.a*/
	return float4(img.xyz, reflection*edge);
}
//*******************************************************************************************************************
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //Anomaly shaders 1.5																 				 			  //
  //Credits to KD, Anonim, Crossire, daemonjax, Zhora Cementow, Meltac, X-Ray Oxygen, FozeSt, Zagolski, SonicEthers, //
 //David Hoskins, BigWIngs																							//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

#define WATER_SPLASHES_MAX_RADIUS 1.5 // Maximum number of cells a ripple can cross.
//#define WATER_SPLASHES_DOUBLE_HASH	  // дополнительный шум

float hash12(float2 p)
{
	float3 p3 = frac(float3(p.xyx) * .1031);
	p3 += dot(p3, p3.yzx + 19.19);
	return frac((p3.x + p3.y) * p3.z);
}

float2 hash22(float2 p)
{
	float3 p3 = frac(float3(p.xyx) * float3(.1031, .1030, .0973));
	p3 += dot(p3, p3.yzx + 19.19);
	return frac((p3.xx + p3.yz) * p3.zy);
}

float3 calc_rain_splashes(float2 tc)
{
	float2 p0 = floor(tc * 35);

	float circles = 0;

	for (int j = -WATER_SPLASHES_MAX_RADIUS; j <= WATER_SPLASHES_MAX_RADIUS; ++j)
	{
		for (int i = -WATER_SPLASHES_MAX_RADIUS; i <= WATER_SPLASHES_MAX_RADIUS; ++i)
		{
			float2 pi = p0 + float2(i, j);
		#ifdef WATER_SPLASHES_DOUBLE_HASH
			float2 hsh = hash22(pi);
		#else
			float2 hsh = pi;
		#endif
			float2 p = pi + hash22(hsh);

			float t = frac(1.45f * timers.x + hash12(hsh));
			float2 v = p - tc * 35;

			float d = (length(v) * 2.0f) - (float(WATER_SPLASHES_MAX_RADIUS) + 1.0) * t;

			const float h = 1e-3;
			float d1 = d - h;
			float d2 = d + h;
			float p1 = sin(31. * d1) * smoothstep(-0.6, -0.3, d1) * smoothstep(0., -0.3, d1);
			float p2 = sin(31. * d2) * smoothstep(-0.6, -0.3, d2) * smoothstep(0., -0.3, d2);
			circles += 0.5 * normalize(v) * ((p2 - p1) / (2. * h) * (1. - t) * (1. - t));
		}
	}

	float c = float(WATER_SPLASHES_MAX_RADIUS * 2 + 1);
	circles /= c * c;

	return float3(circles.xx, sqrt(1.0f - dot(circles, circles)));
}

//*******************************************************************************************************************

#endif
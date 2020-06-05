#ifndef OGSE_FUNCTIONS_H
#define OGSE_FUNCTIONS_H

#define SKY_EPS   float(0.001)
#define SKY_DEPTH float(10000.f)
#define FARPLANE  float(180.0)

// Check if given depth value belongs to sky or not
float is_sky(float depth)
{
	return step(abs(depth - SKY_DEPTH), SKY_EPS);
}

float is_not_sky(float depth)
{
	return step(SKY_EPS, abs(depth - SKY_DEPTH));
}


float get_depth(float2 tc) { return tex2D(s_position, tc).z; }

float normalize_depth(float depth)
{
	return (saturate(depth/FARPLANE));
}


// new uniform variables - OGSE Team
// global constants
uniform float4 ogse_c_screen;		// x - fFOV, y - fAspect, z - Zf/(Zf-Zn), w - Zn*tan(fFov/2)

float4 proj_to_screen(float4 proj)
{
	float4 screen = proj;
	screen.x = (proj.x + proj.w);
	screen.y = (proj.w - proj.y);
	screen.xy *= 0.5;
	return screen;
}
float4 screen_to_proj(float2 screen, float z)
{
	float4 proj;
	proj.w = 1.0;
	proj.z = z;
	proj.x = screen.x*2 - proj.w;
	proj.y = -screen.y*2 + proj.w;
	return proj;
}
float is_in_range(float3 args)
{
	float mn = (args.x > args.y) ? 1: 0;
	float mx = (args.z > args.x) ? 1: 0;
	return mn*mx;
}

#endif
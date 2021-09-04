
#define VIS_GLASS_NUM 9

uniform float4 screen_res; // Screen resolution (x-Width,y-Height, zw - 1/resolution)
uniform float4 addon_VControl;

float visor_ratio(float s)
{
    float ratio = screen_res.y / screen_res.x; // 0.75 for 4:3 (normalscreen) and 0.625 for 16:10 (widescreen) resolution
    return (s - 0.5f) * ratio + 0.5f;
}

float3 visor_reflection(float3 image, float2 tc)
{
    float4 final = 0;
    float2 center = float2(0.5, 0.5);

    float x = length(float2(tc.x, visor_ratio(tc.y)) - center);

    static const float y = VIS_GLASS_NUM;
    float t = addon_VControl.y;

    if (x < t)
        return image;
    else
    {
        float p = saturate((x / t - 1.0f) * 2.0f);

        for (int i = 0; i < y; i++)
        {
            float N = 1.h - p / 0.8f + 0.15f * (i / (y - 1.0f)) * p;
            float2 m = (center - tc) * -N + center;
            final += s_image.SampleLevel(smp_rtlinear, m,0);
        }
        final /= y;
        final *= 1.h - saturate((x - t - 0.05f) * 5.2f);
	}

	image = (image + addon_VControl.x * final) / (1.f + addon_VControl.x);	
	return image;
} 

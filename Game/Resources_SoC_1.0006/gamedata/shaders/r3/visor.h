
uniform float4 addon_VControl; // x - интенсивность визора стекла, y -  насколько далеко от центра будет круг стекла

#define HELMET_VISUAL_QUALITY 3 // [1;2;3]

float3 visor_reflection(float3 color, float2 tc)
{
    static const float blackout_factor = 0.05f, rounding = 2.0f, closeness = 0.8f, elongation = 0.15f, blackout = 5.2f;

    static const uint layers_base_count = 6, layers_count = layers_base_count + (int)HELMET_VISUAL_QUALITY;
    static const float radius = addon_VControl.y, intensity = addon_VControl.x;

    float3 final = float3(0.0, 0.0, 0.0);
    float2 center = float2(0.5, 0.5);

    float ratio = screen_res.y / screen_res.x; // 0.75 for 4:3 (normalscreen) and 0.625 for 16:10 (widescreen) resolution
    float x = length(float2(tc.x, (tc.y - 0.5f) * ratio + 0.5f) - center);

    if (x < radius)
        return color;

    float p = saturate((x / radius - 1.0f) * 2.0f);

    for (int i = 0; i < layers_count; i++)
    {
        float N = 1.h - p / closeness + elongation * (i / (layers_count - 1.0f)) * p;
        float2 m = (center - tc) * -N + center;
        final += s_image.SampleLevel(smp_rtlinear, float3(m, 0), 0).rgb;
    }

    final /= layers_count;
    final *= 1.0 - saturate((x - radius - blackout_factor) * blackout);

    return (color + intensity * final) / (1.f + intensity);
}

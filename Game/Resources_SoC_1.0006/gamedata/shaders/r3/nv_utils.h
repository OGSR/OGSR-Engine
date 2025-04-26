float4x4 brightnessMatrix(float brightness) { return float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, brightness, brightness, brightness, 1); }

float4x4 contrastMatrix(float contrast)
{
    float t = (1.0 - contrast) / 2.0;

    return float4x4(contrast, 0, 0, 0, 0, contrast, 0, 0, 0, 0, contrast, 0, t, t, t, 1);
}

float4x4 saturationMatrix(float saturation)
{
    float3 luminance = float3(0.3086, 0.6094, 0.0820);

    float oneMinusSat = 1.0 - saturation;

    float3 red = float3(luminance.x * oneMinusSat, luminance.x * oneMinusSat, luminance.x * oneMinusSat);
    red += float3(saturation, 0, 0);

    float3 green = float3(luminance.y * oneMinusSat, luminance.y * oneMinusSat, luminance.y * oneMinusSat);
    green += float3(0, saturation, 0);

    float3 blue = float3(luminance.z * oneMinusSat, luminance.z * oneMinusSat, luminance.z * oneMinusSat);
    blue += float3(0, 0, saturation);

    return float4x4(red, 0, green, 0, blue, 0, 0, 0, 0, 1);
}

float3 Brightness(float3 color, float brightness, float contrast)
{
    const float saturation = 1.0;
    float4 res;

    res = mul(float4(color, 1.0), mul(mul(brightnessMatrix(brightness), contrastMatrix(contrast)), saturationMatrix(saturation)));

    return res.rgb;
}

/////////////////////////////////////////////////////////////////////////////////////

float3 RGBToHCV(in float3 RGB)
{
    float4 P = (RGB.g < RGB.b) ? float4(RGB.bg, -1.0f, 2.0f / 3.0f) : float4(RGB.gb, 0.0f, -1.0f / 3.0f);
    float4 Q1 = (RGB.r < P.x) ? float4(P.xyw, RGB.r) : float4(RGB.r, P.yzx);
    float C = Q1.x - min(Q1.w, Q1.y);
    float H = abs((Q1.w - Q1.y) / (6.0f * C + 0.000001f) + Q1.z);
    return float3(H, C, Q1.x);
}

float3 RGBToHSL(in float3 RGB)
{
    RGB.xyz = max(RGB.xyz, 0.000001f);
    float3 HCV = RGBToHCV(RGB);
    float L = HCV.z - HCV.y * 0.5f;
    float S = HCV.y / (1.0f - abs(L * 2.0f - 1.0f) + 0.000001f);
    return float3(HCV.x, S, L);
}

float curve(float x, float k)
{
    float s = sign(x - 0.5f);
    float o = (1.0f + s) / 2.0f;
    return o - 0.5f * s * pow(2.0f * (o - s * x), k);
}

float3 ProcessBW(float3 col, float r, float y, float g, float c, float b, float m)
{
    float3 hsl = RGBToHSL(col.xyz);
    float lum = 1.0f - hsl.z;

    float curve_str = 4.000000;

    float weight_r = curve(max(1.0f - abs(hsl.x * 6.0f), 0.0f), curve_str) + curve(max(1.0f - abs((hsl.x - 1.0f) * 6.0f), 0.0f), curve_str);
    float weight_y = curve(max(1.0f - abs((hsl.x - 0.166667f) * 6.0f), 0.0f), curve_str);
    float weight_g = curve(max(1.0f - abs((hsl.x - 0.333333f) * 6.0f), 0.0f), curve_str);
    float weight_c = curve(max(1.0f - abs((hsl.x - 0.5f) * 6.0f), 0.0f), curve_str);
    float weight_b = curve(max(1.0f - abs((hsl.x - 0.666667f) * 6.0f), 0.0f), curve_str);
    float weight_m = curve(max(1.0f - abs((hsl.x - 0.833333f) * 6.0f), 0.0f), curve_str);

    float sat = hsl.y * (1.0f - hsl.y) + hsl.y;
    float ret = hsl.z;
    ret += (hsl.z * (weight_r * r) * sat * lum);
    ret += (hsl.z * (weight_y * y) * sat * lum);
    ret += (hsl.z * (weight_g * g) * sat * lum);
    ret += (hsl.z * (weight_c * c) * sat * lum);
    ret += (hsl.z * (weight_b * b) * sat * lum);
    ret += (hsl.z * (weight_m * m) * sat * lum);

    return saturate(ret);
}

float3 BlackandWhite(float3 color)
{
    color.xyz = saturate(color.xyz);

    float red;
    float yellow;
    float green;
    float cyan;
    float blue;
    float magenta;

    red = -0.400000;
    yellow = 2.000001;
    green = 3.000000;
    cyan = 0.000000;
    blue = -0.600000;
    magenta = -0.200000;

    color.xyz = ProcessBW(color.xyz, red, yellow, green, cyan, blue, magenta);

    return color;
}

/////////////////////////////////////////////////////////////////////////////////////

float3 LevelsPass(float3 color)
{
    const float3 InputColor = color;
    float3 OutputColor = InputColor;

    float3 InputBlackPoint = float3(0.000000, 0.000000, 0.000000);
    float3 InputWhitePoint = float3(1.000000, 1.000000, 1.000000);
    float3 OutputBlackPoint = float3(0.000000, 0.000000, 0.000000);
    float3 OutputWhitePoint = float3(1.000000, 1.000000, 1.000000);
    float3 InputGamma = float3(3.920000, 1.000000, 7.467000);
    OutputColor = pow(abs(((InputColor)-InputBlackPoint) / (InputWhitePoint - InputBlackPoint)), InputGamma) * (OutputWhitePoint - OutputBlackPoint) + OutputBlackPoint;

    return OutputColor;
}

//////////////////////////////////////////////////////////////////////////////////////

#define permONE 1.0f / 256.0f
#define permHALF 0.5f * permONE
#define permTexSize 256

float discreteNoise(float rand) { return float(int(rand * 3.0) * 64) / 256.0; }

float4 rnm(float2 tc, float t)
{
    float grainAdjust = 1.000000;

    float noise = sin(dot(tc, float2(12.9898, 78.233))) * (43758.5453 + t);
    float noiseR = frac(noise * grainAdjust) * 2.0 - 1.0;
    float noiseG = frac(noise * 1.2154 * grainAdjust) * 2.0 - 1.0;
    float noiseB = frac(noise * 1.3453 * grainAdjust) * 2.0 - 1.0;
    float noiseA = frac(noise * 1.3647 * grainAdjust) * 2.0 - 1.0;
    return float4(noiseR, noiseG, noiseB, noiseA);
}

float4 samplerPermTex(float2 uv)
{
    float4 gen = rnm(uv, 13.14381);
    gen.x = discreteNoise(gen.x);
    gen.y = discreteNoise(gen.y);
    gen.z = discreteNoise(gen.z);
    return gen;
}

float fade(float t) { return t * t * t * (t * (t * 6.0 - 15.0) + 10.0); }

float pnoise3D(float3 p, float t, float grainSize)
{
    float3 pi = permONE * floor(p) + permHALF;
    pi.xy *= permTexSize;
    pi.xy = round((pi.xy - permHALF) / grainSize) * grainSize;
    pi.xy /= permTexSize;
    // Magic constatns
    float3 pf = float3(-0.1, 0.1, 0); // = frac( p );
    // Noise contributions from (x=0, y=0), z=0 and z=1
    float perm00 = rnm(pi.xy, t).x;
    float3 grad000 = samplerPermTex(float2(perm00, pi.z)).xyz * 4.0 - 1.0;
    float n000 = dot(grad000, pf);
    float3 grad001 = samplerPermTex(float2(perm00, pi.z + permONE)).xyz * 4.0 - 1.0;
    float n001 = dot(grad001, pf - float3(0.0, 0.0, 1.0));
    // return n001;
    //  Noise contributions from (x=0, y=1), z=0 and z=1
    float perm01 = rnm(pi.xy + float2(0.0, permONE), t).y;
    float3 grad010 = samplerPermTex(float2(perm01, pi.z)).xyz * 4.0 - 1.0;
    float n010 = dot(grad010, pf - float3(0.0, 1.0, 0.0));
    float3 grad011 = samplerPermTex(float2(perm01, pi.z + permONE)).xyz * 4.0 - 1.0;
    float n011 = dot(grad011, pf - float3(0.0, 1.0, 1.0));
    // Noise contributions from (x=1, y=0), z=0 and z=1
    float perm10 = rnm(pi.xy + float2(permONE, 0.0), t).z;
    float3 grad100 = samplerPermTex(float2(perm10, pi.z)).xyz * 4.0 - 1.0;
    float n100 = dot(grad100, pf - float3(1.0, 0.0, 0.0));
    float3 grad101 = samplerPermTex(float2(perm10, pi.z + permONE)).xyz * 4.0 - 1.0;
    float n101 = dot(grad101, pf - float3(1.0, 0.0, 1.0));
    // Noise contributions from (x=1, y=1), z=0 and z=1
    float perm11 = rnm(pi.xy + float2(permONE, permONE), t).w;
    float3 grad110 = samplerPermTex(float2(perm11, pi.z)).xyz * 4.0 - 1.0;
    float n110 = dot(grad110, pf - float3(1.0, 1.0, 0.0));
    float3 grad111 = samplerPermTex(float2(perm11, pi.z + permONE)).xyz * 4.0 - 1.0;
    float n111 = dot(grad111, pf - float3(1.0, 1.0, 1.0));
    // Blend contributions along x
    float4 n_x = lerp(float4(n000, n001, n010, n011), float4(n100, n101, n110, n111), fade(pf.x));
    // Blend contributions along y
    float2 n_xy = lerp(n_x.xy, n_x.zw, fade(pf.y));
    // Blend contributions along z
    float n_xyz = lerp(n_xy.x, n_xy.y, fade(pf.z));
    // We're done, return the final noise value
    return n_xyz;
}

float3 doGrain(float3 color, float2 texcoord, float grainSize, float grainAmount, float grainIntensity, float grainColor, float grainIntHigh, float grainIntLow, float grainDensity)
{
    float timer = timers.x % 1000.0f;
    float2 uv = texcoord.xy * float2(screen_res.x, screen_res.y);
    float3 noise = pnoise3D(float3(uv.xy, 1), timer, grainSize);
    noise.y = pnoise3D(float3(uv.xy, 2), timer, grainSize);
    noise.z = pnoise3D(float3(uv.xy, 3), timer, grainSize);

    // Old, practically does the same as grainAmount below
    // Added back on request
    noise.xyz *= grainIntensity;

    // Noise saturation
    noise.xyz = lerp(dot(noise.xyz, 1.0f), noise.xyz, grainColor);

    // Control noise density
    noise.xyz = pow(abs(noise.xyz), max(11.0f - grainDensity, 0.1f)) * sign(noise.xyz);

    // Mixing options
    float lum = dot(color.xyz, 0.333333f); // Just using average here
    noise.xyz = lerp(noise.xyz * grainIntLow, noise.xyz * grainIntHigh, fade(lum)); // Noise adjustments based on average intensity
    color.xyz = lerp(color.xyz, color.xyz + (noise.xyz), grainAmount);
    return float3(color.xyz);
}

float3 Grain1(float3 color, float2 texcoord)
{
    float grainSize = 1.0;
    float grainAmount = 0.500000;
    float grainIntensity = 0.300000;
    float grainColor = 0.000000;
    float grainIntHigh = 0.000000;
    float grainIntLow = 0.500000;
    float grainDensity = 10.000000;

    return doGrain(color, texcoord, grainSize, grainAmount, grainIntensity, grainColor, grainIntHigh, grainIntLow, grainDensity);
}

float3 Grain2(float3 color, float2 texcoord)
{
    float grainSize = 1.0;
    float grainAmount = 0.4;
    float grainIntensity = 1.000000;
    float grainColor = 0.000000;
    float grainIntHigh = 0.000000;
    float grainIntLow = 1.000000;
    float grainDensity = 0.000000;

    return doGrain(color, texcoord, grainSize, grainAmount, grainIntensity, grainColor, grainIntHigh, grainIntLow, grainDensity);
}

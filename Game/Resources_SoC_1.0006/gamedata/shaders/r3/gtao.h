// GTAO shader by Doenitz

#ifndef GTAO_H_INCLUDED
#define GTAO_H_INCLUDED

#ifndef SSAO_QUALITY

float calc_gtao(float3 cPosV, float3 normalV, float2 cTexCoord, uint iSample) { return 1.0; }

#else

#if SSAO_QUALITY == 3
#define GTAO_SAMPLE 4
#elif SSAO_QUALITY == 2
#define GTAO_SAMPLE 3
#elif SSAO_QUALITY == 1
#define GTAO_SAMPLE 2
#endif

#define GTAO_SLICES 4
#define GTAO_RADIUS 4
#define GTAO_TC_MUL 1.f

// Fast approx from... Drobot??
float fast_acos(float v)
{
    v = clamp(v, -1, 1);
    float res = -0.156583 * abs(v) + (3.14 * 0.5);
    res *= sqrt(1.0 - abs(v));
    return (v >= 0) ? res : 3.14 - res;
}

float GTAO_Offsets(float2 uv)
{
    int2 position = (int2)(uv * pos_decompression_params2.xy / GTAO_TC_MUL);
    return 0.25f * (float)((position.y - position.x) & 3);
}

float GTAO_Noise(float2 position) { return frac(52.9829189f * frac(dot(position, float2(0.06711056f, 0.00583715f)))); }

float3 gbuf_unpack_position(float z, float2 pos2d) { return float3(z * (pos2d * pos_decompression_params.zw - pos_decompression_params.xy), z); }

float3 unpack_position(float2 tc, uint iSample : SV_SAMPLEINDEX)
{
#ifndef USE_MSAA
    float depth = s_position.Sample(smp_nofilter, tc).z;
#else
    float depth = s_position.Load(int3(tc * screen_res.xy, 0), iSample).z;
#endif
    return gbuf_unpack_position((depth > 0.01f ? depth : 1000.f), tc * pos_decompression_params2.xy);
}

float3 calc_gtao(float3 cPosV, float3 normalV, float2 cTexCoord, uint iSample)
{
    float fov = atan(1.0 / m_P._m11);
    float proj_scale = float(pos_decompression_params2.y) / (tan(fov * 0.5f) * 2.f);
    float screen_radius = (GTAO_RADIUS * 0.5f * proj_scale) / cPosV.z;
    float3 viewV = -normalize(cPosV);

    float noiseOffset = GTAO_Offsets(cTexCoord);
    float noiseDirection = GTAO_Noise(cTexCoord * pos_decompression_params2.xy);

    float falloff_mul = 2.f / pow(GTAO_RADIUS, 2.f);
    float screen_res_mul = 1.f / GTAO_SAMPLE * pos_decompression_params2.zw;
    float pi_by_slices = 3.14f / GTAO_SLICES;

    float visibility = 0.f;

    for (int slice = 0; slice < GTAO_SLICES; slice++)
    {
        float phi = (slice + noiseDirection) * pi_by_slices;
        float2 omega = float2(cos(phi), sin(phi));
        float3 directionV = float3(omega.x, omega.y, 0.f);

        float3 orthoDirectionV = directionV - (dot(directionV, viewV) * viewV);
        float3 axisV = cross(directionV, viewV);
        float3 projNormalV = normalV - axisV * dot(normalV, axisV);
        float projNormalLenght = length(projNormalV);

        float sgnN = sign(dot(orthoDirectionV, projNormalV));
        float cosN = saturate(dot(projNormalV, viewV) / projNormalLenght);
        float n = sgnN * fast_acos(cosN);
        float sinN2 = 2.f * sin(n);

        for (int side = 0; side < 2; side++)
        {
            float cHorizonCos = -1.f;

            for (int sample = 0; sample < GTAO_SAMPLE; sample++)
            {
                float2 s = max(screen_radius * (float(sample) + noiseOffset), 4.f + float(sample)) * screen_res_mul; // fix for multiplying s by sample=0 from Unity code
                float2 sTexCoord = cTexCoord + (-1.f + 2.f * side) * s * float2(omega.x, -omega.y);
                float3 sPosV = unpack_position(sTexCoord, iSample);
                float3 sHorizonV = sPosV - cPosV;
                float falloff = saturate(dot(sHorizonV, sHorizonV) * falloff_mul);
                float H = dot(normalize(sHorizonV), viewV);
                cHorizonCos = (H > cHorizonCos) ? lerp(H, cHorizonCos, falloff) : cHorizonCos;
            }

            float h = n + clamp((-1.f + 2.f * side) * fast_acos(cHorizonCos) - n, -3.14f / 2.f, 3.14f / 2.f);
            visibility += projNormalLenght * (cosN + h * sinN2 - cos(2.f * h - n)) / 4.f;
        }
    }

    visibility /= GTAO_SLICES;
    return float3(visibility, cPosV.z, 0.f);
}

#endif

#endif

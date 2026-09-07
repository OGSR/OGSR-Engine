#include "ogsr_xegtao_common.h"

Texture2D<float4> xe_position : register(t0);
// Keep the template argument identical to XeGTAO_MainPass. FXC does not
// consider Texture2D<float> interchangeable with Texture2D<lpfloat>.
Texture2D<lpfloat> xe_depth : register(t1);
SamplerState xe_point : register(s0);
RWTexture2D<uint> xe_ao : register(u0);
RWTexture2D<unorm float> xe_edges : register(u1);

[numthreads(8, 8, 1)]
void main(uint2 pixel : SV_DispatchThreadID)
{
    float4 packed = xe_position.Load(int3(min(pixel, uint2(xe_source.xy) - 1), 0));
    if (packed.z <= 0.001)
    {
        xe_ao[pixel] = 170; // visibility 1 / XE_GTAO_OCCLUSION_TERM_SCALE
        xe_edges[pixel] = 0;
        return;
    }
    uint index = HilbertIndex(pixel.x % 64, pixel.y % 64) + 288 * uint(xe_source.z);
    float2 noise = frac(0.5 + index * float2(0.75487766624669276, 0.56984029099805327));
#if SSAO_QUALITY >= 3
    const float slices = 3, steps = 3;
#elif SSAO_QUALITY == 2
    const float slices = 2, steps = 2;
#else
    const float slices = 1, steps = 2;
#endif
    XeGTAO_MainPass(pixel, slices, steps, noise, XeDecodeNormal(packed.xy),
        XeConstantsForFrame(), xe_depth, xe_point, xe_ao, xe_edges);
}

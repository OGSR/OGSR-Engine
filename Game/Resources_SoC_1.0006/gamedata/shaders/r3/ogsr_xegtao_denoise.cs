#include "ogsr_xegtao_common.h"

Texture2D<uint> xe_ao : register(t0);
// Match XeGTAO_Denoise exactly; FXC distinguishes lpfloat in texture templates.
Texture2D<lpfloat> xe_edges : register(t1);
SamplerState xe_point : register(s0);
RWTexture2D<uint> xe_output : register(u0);

[numthreads(8, 8, 1)]
void main(uint2 pixel : SV_DispatchThreadID)
{
    XeGTAO_Denoise(pixel * uint2(2, 1), XeConstantsForFrame(), xe_ao, xe_edges, xe_point, xe_output, true);
}

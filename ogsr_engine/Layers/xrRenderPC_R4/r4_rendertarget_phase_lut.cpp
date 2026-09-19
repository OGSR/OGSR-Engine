#include "stdafx.h"

void CRenderTarget::phase_lut(CBackend& cmd_list)
{
    if (fis_zero(ps_ssfx_lut.x))
        return;

    PIX_EVENT(phase_LUT);

    RenderScreenTriangle(cmd_list, pp_dst(), s_lut->E[0]);
    pp_flip();
}

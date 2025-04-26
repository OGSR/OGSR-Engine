#include "stdafx.h"

void CRenderTarget::phase_lut(CBackend& cmd_list)
{
    if (fis_zero(ps_ssfx_lut.x))
        return;

    PIX_EVENT(phase_LUT);

    RenderScreenQuad(cmd_list, Device.dwWidth, Device.dwHeight, rt_Generic_combine, s_lut->E[0]);
    HW.get_context(cmd_list.context_id)->CopyResource(rt_Generic_0->pSurface, rt_Generic_combine->pSurface);
}
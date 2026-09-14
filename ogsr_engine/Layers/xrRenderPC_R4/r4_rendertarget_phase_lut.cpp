#include "stdafx.h"

void CRenderTarget::phase_lut(CBackend& cmd_list)
{
    if (fis_zero(ps_ssfx_lut.x))
        return;

    PIX_EVENT(phase_LUT);

    RenderScreenTriangle(cmd_list, rt_Generic_combine, s_lut->E[0]);
    HW.get_context(cmd_list.context_id)->CopyResource(rt_Postprocess_0->pSurface, rt_Generic_combine->pSurface);
}

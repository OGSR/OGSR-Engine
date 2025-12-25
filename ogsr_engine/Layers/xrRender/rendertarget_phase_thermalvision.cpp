#include "stdafx.h"

void CRenderTarget::phase_heatvision(CBackend& cmd_list)
{
    PIX_EVENT(phase_heatvision);

    RenderScreenQuad(cmd_list, Device.dwWidth, Device.dwHeight, rt_Generic_combine, s_heatvision->E[0]);
    HW.get_context(cmd_list.context_id)->CopyResource(rt_Generic_0->pSurface, rt_Generic_combine->pSurface);
}

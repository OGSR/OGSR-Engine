#include "stdafx.h"

void CRenderTarget::phase_nightvision(CBackend& cmd_list)
{
    PIX_EVENT(phase_nightvision);

    RenderScreenTriangle(cmd_list, rt_Generic_combine, s_nightvision->E[0]);
    HW.get_context(cmd_list.context_id)->CopyResource(rt_Postprocess_0->pSurface, rt_Generic_combine->pSurface);
}

#include "stdafx.h"

void CRenderTarget::phase_heatvision(CBackend& cmd_list)
{
    PIX_EVENT(phase_heatvision);

    RenderScreenTriangle(cmd_list, rt_Generic_scene_scratch, s_heatvision->E[0]);
    HW.get_context(cmd_list.context_id)->CopyResource(rt_Generic_0->pSurface, rt_Generic_scene_scratch->pSurface);
}

void CRenderTarget::phase_heatvision_overlay(CBackend& cmd_list)
{
    PIX_EVENT(phase_heatvision_overlay);

    RenderScreenTriangle(cmd_list, rt_Generic_combine, s_heatvision->E[1]);
    HW.get_context(cmd_list.context_id)->CopyResource(rt_Postprocess_0->pSurface, rt_Generic_combine->pSurface);
}

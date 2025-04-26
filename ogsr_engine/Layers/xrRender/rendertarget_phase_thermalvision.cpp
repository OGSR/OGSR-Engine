#include "stdafx.h"

void CRenderTarget::phase_heatvision(CBackend& cmd_list)
{
    RenderScreenQuad(cmd_list, Device.dwWidth, Device.dwHeight, rt_Generic_combine, s_heatvision->E[ps_pnv_mode == 2 || ps_pnv_mode == 3 ? 1 : 0]);
    HW.get_context(cmd_list.context_id)->CopyResource(rt_Generic_0->pSurface, rt_Generic_combine->pSurface);
};

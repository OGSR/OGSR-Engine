#include "stdafx.h"

void CRenderTarget::phase_nightvision(CBackend& cmd_list)
{
    RenderScreenQuad(cmd_list, Device.dwWidth, Device.dwHeight, rt_Generic_combine, s_nightvision->E[ps_pnv_mode == 1 ? 1 : 0]);
    HW.get_context(cmd_list.context_id)->CopyResource(rt_Generic_0->pSurface, rt_Generic_combine->pSurface);
};
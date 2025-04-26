#include "stdafx.h"

// crookr
void CRenderTarget::phase_fakescope(CBackend& cmd_list)
{
    if (Device.IsAltScopeActive())
        return;

    RenderScreenQuad(cmd_list, Device.dwWidth, Device.dwHeight, rt_Generic_combine, s_fakescope->E[0]);

    HW.get_context(cmd_list.context_id)->CopyResource(rt_Generic_0->pSurface, rt_Generic_combine->pSurface);
};

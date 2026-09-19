#include "stdafx.h"

// crookr
void CRenderTarget::phase_fakescope(CBackend& cmd_list)
{
    if (Device.IsAltScopeActive())
        return;

    RenderScreenTriangle(cmd_list, pp_dst(), s_fakescope->E[0]);
    pp_flip();
}

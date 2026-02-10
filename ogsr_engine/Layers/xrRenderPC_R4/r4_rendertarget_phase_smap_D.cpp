#include "stdafx.h"

// Only sun && rain
void CRenderTarget::phase_smap_direct(CBackend& cmd_list, light* L, u32 sub_phase)
{
    if (sub_phase == SE_SUN_RAIN_SMAP)
    {
        u_setrt(cmd_list, nullptr, nullptr, nullptr, nullptr, rt_smap_rain->pZRT[cmd_list.context_id]);
        cmd_list.ClearZB(rt_smap_rain, 1.0f);
        cmd_list.SetViewport({0, 0, rt_smap_rain->dwWidth, rt_smap_rain->dwHeight, 0.0, 1.0});
    }
    else
    {
        u_setrt(cmd_list, nullptr, nullptr, nullptr, nullptr, rt_smap_sun_cascade[sub_phase]->pZRT[cmd_list.context_id]);
        cmd_list.ClearZB(rt_smap_sun_cascade[sub_phase], 1.0f);
        cmd_list.SetViewport({0, 0, rt_smap_sun_cascade[sub_phase]->dwWidth, rt_smap_sun_cascade[sub_phase]->dwHeight, 0.0, 1.0});
    }

    // Stencil	- disable
    cmd_list.set_Stencil(FALSE);
}

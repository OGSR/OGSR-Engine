#include "stdafx.h"

void CRenderTarget::phase_accumulator(CBackend& cmd_list)
{
    // Targets
    if (dwAccumulatorClearMark == Device.dwFrame)
    {
        // normal operation - setup
        u_setrt(cmd_list, rt_Accumulator, nullptr, nullptr, nullptr, rt_Base_Depth->pZRT[cmd_list.context_id]);
    }
    else
    {
        // initial setup
        dwAccumulatorClearMark = Device.dwFrame;

        // clear
        u_setrt(cmd_list, rt_Accumulator, nullptr, nullptr, nullptr, rt_Base_Depth->pZRT[cmd_list.context_id]);

        reset_light_marker(cmd_list);

        cmd_list.ClearRT(rt_Accumulator, {}); // black

        //	render this after sun to avoid troubles with sun

        // Stencil	- draw only where stencil >= 0x1
        cmd_list.set_Stencil(TRUE, D3DCMP_LESSEQUAL, 0x01, 0xff, 0x00);
        cmd_list.set_CullMode(CULL_NONE);
        cmd_list.set_ColorWriteEnable();
    }

    //	Restore viewport after shadow map rendering
    RImplementation.rmNormal(cmd_list);
}

void CRenderTarget::phase_vol_accumulator(CBackend& cmd_list)
{
    u_setrt(cmd_list, rt_Generic_2, nullptr, nullptr, nullptr, rt_Base_Depth->pZRT[cmd_list.context_id]);

    if (!m_bHasActiveVolumetric)
    {
        m_bHasActiveVolumetric = true;
        cmd_list.ClearRT(rt_Generic_2, {}); // black
    }

    cmd_list.set_Stencil(FALSE);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_ColorWriteEnable();
}
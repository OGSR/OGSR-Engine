#include "stdafx.h"

void CRenderTarget::phase_ao(CBackend& cmd_list)
{
    PIX_EVENT(phase_ao);

    cmd_list.set_ColorWriteEnable();

    if (ps_r_ao_resolution == AO_RES_HALF)
    {
        {
            PIX_EVENT(ao_evaluate_half);
            RenderScreenTriangle(cmd_list, rt_ao_half, s_ao->E[1]);
        }
        {
            PIX_EVENT(ao_resolve);
            RenderScreenTriangle(cmd_list, rt_ao, s_ao->E[2]);
        }
    }
    else
    {
        PIX_EVENT(ao_evaluate_full);
        RenderScreenTriangle(cmd_list, rt_ao, s_ao->E[0]);
    }
}

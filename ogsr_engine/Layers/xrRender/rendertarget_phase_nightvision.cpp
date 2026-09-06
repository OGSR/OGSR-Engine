#include "stdafx.h"

void CRenderTarget::phase_nightvision(CBackend& cmd_list)
{
    PIX_EVENT(phase_nightvision);

    RenderScreenTriangle(cmd_list, pp_dst(), s_nightvision->E[0]);
    pp_flip();
}

#include "stdafx.h"

void CRenderTarget::phase_occq(CBackend& cmd_list)
{
    u_setrt(cmd_list, Device.dwWidth, Device.dwHeight, get_base_rt(), nullptr, nullptr, rt_Base_Depth->pZRT[cmd_list.context_id]);

    cmd_list.set_Shader(s_occq);
    cmd_list.set_CullMode(CULL_CCW);
    cmd_list.set_Stencil(TRUE, D3DCMP_LESSEQUAL, 0x01, 0xff, 0x00);
    cmd_list.set_ColorWriteEnable(FALSE);
}

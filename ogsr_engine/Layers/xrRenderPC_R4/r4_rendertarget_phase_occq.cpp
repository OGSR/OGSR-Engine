#include "stdafx.h"

void CRenderTarget::phase_occq(CBackend& cmd_list)
{
    // Occlusion queries only need scene depth. Binding the display-sized base
    // color target beside the render-sized depth view is invalid in D3D11.
    u_setrt(cmd_list, nullptr, nullptr, nullptr, nullptr, rt_Base_Depth->pZRT[cmd_list.context_id]);
    RImplementation.rmNormal(cmd_list);

    cmd_list.set_Shader(s_occq);
    cmd_list.set_CullMode(CULL_CCW);
    cmd_list.set_Stencil(TRUE, D3DCMP_LESSEQUAL, 0x01, 0xff, 0x00);
    cmd_list.set_ColorWriteEnable(FALSE);
    cmd_list.set_PS(nullptr);
}

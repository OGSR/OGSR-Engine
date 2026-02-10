#include "stdafx.h"

void CRenderTarget::phase_blur(CBackend& cmd_list)
{
    PIX_EVENT(PHASE_BLUR);

    float w = float(Device.dwWidth) * 0.5f;
    float h = float(Device.dwHeight) * 0.5f;
    RenderScreenQuad(cmd_list, u32(w), u32(h), rt_blur_h_2, s_blur->E[0], [&]() { cmd_list.set_c("blur_params", Fvector4{1.0, 0.0, w, h}); });
    RenderScreenQuad(cmd_list, u32(w), u32(h), rt_blur_2, s_blur->E[1], [&]() { cmd_list.set_c("blur_params", Fvector4{0.0, 1.0, w, h}); });

    w = float(Device.dwWidth) * 0.25f;
    h = float(Device.dwHeight) * 0.25f;
    RenderScreenQuad(cmd_list, u32(w), u32(h), rt_blur_h_4, s_blur->E[2], [&]() { cmd_list.set_c("blur_params", Fvector4{1.0, 0.0, w, h}); });
    RenderScreenQuad(cmd_list, u32(w), u32(h), rt_blur_4, s_blur->E[3], [&]() { cmd_list.set_c("blur_params", Fvector4{0.0, 1.0, w, h}); });

    w = float(Device.dwWidth) * 0.125f;
    h = float(Device.dwHeight) * 0.125f;
    RenderScreenQuad(cmd_list, u32(w), u32(h), rt_blur_h_8, s_blur->E[4], [&]() { cmd_list.set_c("blur_params", Fvector4{1.0, 0.0, w, h}); });
    RenderScreenQuad(cmd_list, u32(w), u32(h), rt_blur_8, s_blur->E[5], [&]() { cmd_list.set_c("blur_params", Fvector4{0.0, 1.0, w, h}); });
}

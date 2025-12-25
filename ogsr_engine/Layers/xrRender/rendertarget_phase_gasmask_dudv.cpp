#include "stdafx.h"

void CRenderTarget::phase_gasmask_dudv(CBackend& cmd_list)
{
    PIX_EVENT(phase_gasmask_dudv);

    const Fvector4 params{0.f, 0.f, static_cast<float>(ps_r2_ls_flags_ext.test(R2FLAGEXT_VISOR_REFL_CONTROL) && ps_r2_ls_flags_ext.test(R2FLAGEXT_VISOR_REFL)),
                          static_cast<float>(ps_r2_ls_flags_ext.test(R2FLAGEXT_MASK_CONTROL) && ps_r2_ls_flags_ext.test(R2FLAGEXT_MASK))};

    RenderScreenQuad(cmd_list, Device.dwWidth, Device.dwHeight, rt_Generic_combine, s_gasmask_dudv->E[0], [&]() {
        cmd_list.set_c("mask_control", params);
        cmd_list.set_c("addon_VControl", ps_r2_visor_refl_intensity, ps_r2_visor_refl_radius, 0.f, 1.f);
    });
    HW.get_context(cmd_list.context_id)->CopyResource(rt_Generic_0->pSurface, rt_Generic_combine->pSurface);

    RenderScreenQuad(cmd_list, Device.dwWidth, Device.dwHeight, rt_mask_drops_blur, s_gasmask_dudv->E[1]);

    RenderScreenQuad(cmd_list, Device.dwWidth, Device.dwHeight, rt_Generic_combine, s_gasmask_dudv->E[2]);
    HW.get_context(cmd_list.context_id)->CopyResource(rt_Generic_0->pSurface, rt_Generic_combine->pSurface);
};
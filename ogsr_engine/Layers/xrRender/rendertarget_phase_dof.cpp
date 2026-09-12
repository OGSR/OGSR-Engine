#include "stdafx.h"

void CRenderTarget::phase_dof(CBackend& cmd_list)
{
    const auto& dof_params = shader_exports.get_dof_params();
    if (fis_zero(dof_params.x) && fis_zero(dof_params.y) && fis_zero(dof_params.z) && fis_zero(dof_params.w))
        return;

    PIX_EVENT(phase_DOF);

    RenderScreenTriangle(cmd_list, rt_dof, s_dof->E[0]);
    RenderScreenTriangle(cmd_list, rt_Generic_combine, s_dof->E[1]);

	//Resolve RT
    HW.get_context(cmd_list.context_id)->CopyResource(rt_Postprocess_0->pSurface, rt_Generic_combine->pSurface);
}

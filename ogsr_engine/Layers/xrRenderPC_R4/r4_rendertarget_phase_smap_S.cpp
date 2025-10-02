#include "stdafx.h"

void CRenderTarget::phase_smap_spot_clear(CBackend& cmd_list)
{
    ZoneScoped;

    rt_smap_depth->set_slice_write(cmd_list.context_id, cmd_list.context_id);
    cmd_list.set_pass_targets(nullptr, nullptr, nullptr, nullptr, rt_smap_depth);
    cmd_list.ClearZB(rt_smap_depth, 1.0f);
}

void CRenderTarget::phase_smap_spot(CBackend& cmd_list, light* L) const
{
    // TODO: it is possible to increase lights batch size
    // by rendering into different smap array slices in parallel
    rt_smap_depth->set_slice_write(cmd_list.context_id, cmd_list.context_id);                                                             
    cmd_list.set_pass_targets(nullptr, nullptr, nullptr, nullptr, rt_smap_depth);

    const D3D_VIEWPORT viewport = {L->X.S.posX, L->X.S.posY, L->X.S.size, L->X.S.size, 0.f, 1.f};
    cmd_list.SetViewport(viewport);

    // Misc		- draw only front-faces //back-faces
    cmd_list.set_CullMode(CULL_CCW);
    cmd_list.set_Stencil(FALSE);
    // no transparency
#pragma todo("can optimize for multi-lights covering more than say 50%...")
    cmd_list.set_ColorWriteEnable(FALSE);
}

#include "stdafx.h"

// startup
void CRenderTarget::phase_scene_prepare()
{
    PIX_EVENT(phase_scene_prepare);

    // Thx to K.D.
    // We need to clean up G-buffer every frame to avoid "ghosting" on sky
    {
        const Fcolor color{}; // black

        RCache.ClearRT(rt_Position->pRT, color);
        RCache.ClearRT(rt_Color->pRT, color);
        RCache.ClearRT(rt_Accumulator->pRT, color);
        RCache.ClearRT(rt_heat->pRT, color);

        //очистка Z-буфера перенесена на этап до начала рендера неба
        //RCache.ClearZB(get_base_zb(), 1.0f, 0);
    }

    //	Igor: for volumetric lights
    m_bHasActiveVolumetric = false;
    //	Clear later if try to draw volumetric
}

// begin
void CRenderTarget::phase_scene_begin(CBackend& cmd_list)
{
    // Targets, use accumulator for temporary storage
    {
        u_setrt(cmd_list, rt_Position, rt_Color, rt_Velocity, rt_heat, rt_Base_Depth->pZRT[cmd_list.context_id]);
    }

    // Stencil - write 0x1 at pixel pos
    cmd_list.set_Stencil(TRUE, D3DCMP_ALWAYS, 0x01, 0xff, 0x7f, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);

    cmd_list.set_CullMode(CULL_CCW);
    cmd_list.set_ColorWriteEnable();
}

// end
void CRenderTarget::phase_scene_end(CBackend& cmd_list)
{
}

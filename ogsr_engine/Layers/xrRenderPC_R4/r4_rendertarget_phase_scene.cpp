#include "stdafx.h"

// startup
void CRenderTarget::phase_scene_prepare()
{
    PIX_EVENT(phase_scene_prepare);

    // Thx to K.D.
    // We need to clean up G-buffer every frame to avoid "ghosting" on sky
    {
        constexpr float ColorRGBA[]{0.0f, 0.0f, 0.0f, 0.0f};

        HW.pContext->ClearRenderTargetView(rt_Position->pRT, ColorRGBA);
        HW.pContext->ClearRenderTargetView(rt_Color->pRT, ColorRGBA);
        HW.pContext->ClearRenderTargetView(rt_Accumulator->pRT, ColorRGBA);
        HW.pContext->ClearDepthStencilView(HW.pBaseZB, D3D_CLEAR_DEPTH | D3D_CLEAR_STENCIL, 1.0f, 0);

        if (RImplementation.o.dx10_msaa)
            HW.pContext->ClearDepthStencilView(rt_MSAADepth->pZRT, D3D_CLEAR_DEPTH | D3D_CLEAR_STENCIL, 1.0f, 0);
    }

    //	Igor: for volumetric lights
    m_bHasActiveVolumetric = false;
    //	Clear later if try to draw volumetric
}

// begin
void CRenderTarget::phase_scene_begin()
{
    // Enable ANISO
    SSManager.SetMaxAnisotropy(ps_r__tf_Anisotropic);

    ID3DDepthStencilView* pZB = HW.pBaseZB;

    if (RImplementation.o.dx10_msaa)
        pZB = rt_MSAADepth->pZRT;

    // Targets, use accumulator for temporary storage
    if (!RImplementation.o.dx10_gbuffer_opt)
    {
        if (RImplementation.o.albedo_wo)
            u_setrt(rt_Position, rt_Normal, rt_Accumulator, pZB);
        else
            u_setrt(rt_Position, rt_Normal, rt_Color, pZB);
    }
    else
    {
        if (RImplementation.o.albedo_wo)
            u_setrt(rt_Position, rt_Accumulator, pZB);
        else
            u_setrt(rt_Position, rt_Color, pZB);
    }

    // Stencil - write 0x1 at pixel pos
    RCache.set_Stencil(TRUE, D3DCMP_ALWAYS, 0x01, 0xff, 0x7f, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);

    // Misc		- draw only front-faces
    //	TODO: DX10: siable two-sided stencil here
    // CHK_DX(HW.pDevice->SetRenderState	( D3DRS_TWOSIDEDSTENCILMODE,FALSE				));
    RCache.set_CullMode(CULL_CCW);
    RCache.set_ColorWriteEnable();
}

void CRenderTarget::disable_aniso()
{
    // Disable ANISO
    SSManager.SetMaxAnisotropy(1);
}

// end
void CRenderTarget::phase_scene_end()
{
    disable_aniso();

    if (!RImplementation.o.albedo_wo)
        return;

    // transfer from "rt_Accumulator" into "rt_Color"
    if (!RImplementation.o.dx10_msaa)
        u_setrt(rt_Color, 0, 0, HW.pBaseZB);
    else
        u_setrt(rt_Color, 0, 0, rt_MSAADepth->pZRT);
    RCache.set_CullMode(CULL_NONE);
    RCache.set_Stencil(TRUE, D3DCMP_LESSEQUAL, 0x01, 0xff, 0x00); // stencil should be >= 1
    if (RImplementation.o.nvstencil)
        u_stencil_optimize(CRenderTarget::SO_Combine);
    RCache.set_Stencil(TRUE, D3DCMP_LESSEQUAL, 0x01, 0xff, 0x00); // stencil should be >= 1
    RCache.set_ColorWriteEnable();

    // common calc for quad-rendering
    u32 Offset;
    u32 C = color_rgba(255, 255, 255, 255);
    float _w = float(Device.dwWidth);
    float _h = float(Device.dwHeight);
    Fvector2 p0, p1;
    p0.set(.5f / _w, .5f / _h);
    p1.set((_w + .5f) / _w, (_h + .5f) / _h);
    float d_Z = EPS_S, d_W = 1.f;

    // Fill vertex buffer
    FVF::TL* pv = (FVF::TL*)RCache.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(EPS, float(_h + EPS), d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(EPS, EPS, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(float(_w + EPS), float(_h + EPS), d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(float(_w + EPS), EPS, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RCache.Vertex.Unlock(4, g_combine->vb_stride);

    // if (stencil>=1 && aref_pass)	stencil = light_id
    RCache.set_Element(s_accum_mask->E[SE_MASK_ALBEDO]); // masker
    RCache.set_Geometry(g_combine);
    RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
}

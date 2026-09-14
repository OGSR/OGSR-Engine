#include "stdafx.h"

void CRenderTarget::RenderScreenTriangle(CBackend& cmd_list, const ref_rt& rt, ref_selement& sh, const std::function<void()>& lambda)
{
    u_setrt(cmd_list, rt->dwWidth, rt->dwHeight, rt->pRT, nullptr, nullptr, nullptr);
    cmd_list.SetViewport({0.f, 0.f, static_cast<float>(rt->dwWidth), static_cast<float>(rt->dwHeight), 0.f, 1.f});

    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    cmd_list.set_Element(sh);

    lambda();

    cmd_list.set_Geometry(TriangleGeom);
    cmd_list.Render(D3DPT_TRIANGLELIST, 0, 0, 3, 0, 1);
}

void CRenderTarget::RenderScreenQuad(CBackend& cmd_list, const u32 w, const u32 h, const ref_rt& rt, ref_selement& sh, const std::function<void()>& lambda)
{
    u32 Offset = 0;
    constexpr float d_Z = EPS_S;
    constexpr float d_W = 1.0f;
    constexpr u32 C = color_rgba(0, 0, 0, 255);

    u_setrt(cmd_list, w, h, rt->pRT, nullptr, nullptr, nullptr);
    cmd_list.SetViewport({0.f, 0.f, static_cast<float>(w), static_cast<float>(h), 0.f, 1.f});

    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Half-pixel offset (DX9 only)

    constexpr Fvector2 p0{0.0f, 0.0f}, p1{1.0f, 1.0f};

    FVF::TL* pv = (FVF::TL*)RImplementation.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, float(h), d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(float(w), float(h), d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(float(w), 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RImplementation.Vertex.Unlock(4, g_combine->vb_stride);

    cmd_list.set_Element(sh);

    lambda();

    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
}

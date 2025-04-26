#include "stdafx.h"

void CRenderTarget::phase_blur(CBackend& cmd_list)
{
    PIX_EVENT(phase_BLUR);

    // Get common data
    u32 Offset = 0;
    const float d_Z = EPS_S;
    const float d_W = 1.0f;
    const u32 C = color_rgba(0, 0, 0, 255);

    // Full resolution
    float w = float(Device.dwWidth);
    float h = float(Device.dwHeight);

    Fvector2 p0, p1;
    p0.set(0.0f, 0.0f);
    p1.set(1.0f, 1.0f);

    ///////////////////////////////////////////////////////////////////////////////////
    ////Horizontal blur
    ///////////////////////////////////////////////////////////////////////////////////
    w = float(Device.dwWidth) * 0.5f;
    h = float(Device.dwHeight) * 0.5f;

    u_setrt(cmd_list, rt_blur_h_2, nullptr, nullptr, nullptr);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
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

    // Draw COLOR
    cmd_list.set_Element(s_blur->E[0]);
    cmd_list.set_c("blur_params", 1.0, 0.0, w, h);
    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    ///////////////////////////////////////////////////////////////////////////////////
    ////Final blur
    ///////////////////////////////////////////////////////////////////////////////////
    u_setrt(cmd_list, rt_blur_2, nullptr, nullptr, nullptr);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (FVF::TL*)RImplementation.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, float(h), d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(float(w), float(h), d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(float(w), 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RImplementation.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    cmd_list.set_Element(s_blur->E[1]);
    cmd_list.set_c("blur_params", 0.0, 1.0, w, h);
    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    ///////////////////////////////////////////////////////////////////////////////////
    ////Horizontal blur / Half res
    ///////////////////////////////////////////////////////////////////////////////////
    w = float(Device.dwWidth) * 0.25f;
    h = float(Device.dwHeight) * 0.25f;

    u_setrt(cmd_list, rt_blur_h_4, nullptr, nullptr, nullptr);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (FVF::TL*)RImplementation.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, float(h), d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(float(w), float(h), d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(float(w), 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RImplementation.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    cmd_list.set_Element(s_blur->E[2]);
    cmd_list.set_c("blur_params", 1.0, 0.0, w, h);
    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    ///////////////////////////////////////////////////////////////////////////////////
    ////Final blur
    ///////////////////////////////////////////////////////////////////////////////////
    u_setrt(cmd_list, rt_blur_4, nullptr, nullptr, nullptr);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (FVF::TL*)RImplementation.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, float(h), d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(float(w), float(h), d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(float(w), 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RImplementation.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    cmd_list.set_Element(s_blur->E[3]);
    cmd_list.set_c("blur_params", 0.0, 1.0, w, h);
    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    ///////////////////////////////////////////////////////////////////////////////////
    ////Horizontal blur
    ///////////////////////////////////////////////////////////////////////////////////
    w = float(Device.dwWidth) * 0.125f;
    h = float(Device.dwHeight) * 0.125f;

    u_setrt(cmd_list, rt_blur_h_8, nullptr, nullptr, nullptr);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (FVF::TL*)RImplementation.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, float(h), d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(float(w), float(h), d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(float(w), 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RImplementation.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    cmd_list.set_Element(s_blur->E[4]);
    cmd_list.set_c("blur_params", 1.0, 0.0, w, h);
    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    ///////////////////////////////////////////////////////////////////////////////////
    ////Final blur
    ///////////////////////////////////////////////////////////////////////////////////
    u_setrt(cmd_list, rt_blur_8, nullptr, nullptr, nullptr);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_Stencil(FALSE);

    // Fill vertex buffer
    pv = (FVF::TL*)RImplementation.Vertex.Lock(4, g_combine->vb_stride, Offset);
    pv->set(0, float(h), d_Z, d_W, C, p0.x, p1.y);
    pv++;
    pv->set(0, 0, d_Z, d_W, C, p0.x, p0.y);
    pv++;
    pv->set(float(w), float(h), d_Z, d_W, C, p1.x, p1.y);
    pv++;
    pv->set(float(w), 0, d_Z, d_W, C, p1.x, p0.y);
    pv++;
    RImplementation.Vertex.Unlock(4, g_combine->vb_stride);

    // Draw COLOR
    cmd_list.set_Element(s_blur->E[5]);
    cmd_list.set_c("blur_params", 0.0, 1.0, w, h);
    cmd_list.set_Geometry(g_combine);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    ///////////////////////////////////////////////////////////////////////////////////
};
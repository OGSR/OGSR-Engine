#include "stdafx.h"

#include "r4_rendertarget.h"

void CRenderTarget::u_calc_tc_noise(CBackend& cmd_list, Fvector2& p0, Fvector2& p1)
{
    CTexture* T = cmd_list.get_ActiveTexture(2);
    VERIFY(T, "Texture #3 in noise shader should be setted up");
    const u32 tw = iCeil(float(T->get_Width()) * param_noise_scale + EPS_S);
    const u32 th = iCeil(float(T->get_Height()) * param_noise_scale + EPS_S);
    VERIFY(tw && th, "Noise scale can't be zero in any way");

    // calculate shift from FPSes
    im_noise_time -= Device.fTimeDelta;
    if (im_noise_time < 0)
    {
        im_noise_shift_w = ::Random.randI(tw ? tw : 1);
        im_noise_shift_h = ::Random.randI(th ? th : 1);
        const float fps_time = 1 / param_noise_fps;
        while (im_noise_time < 0)
            im_noise_time += fps_time;
    }

    const u32 shift_w = im_noise_shift_w;
    const u32 shift_h = im_noise_shift_h;
    const float start_u = (float(shift_w) + .5f) / (tw);
    const float start_v = (float(shift_h) + .5f) / (th);
    const u32 _w = Device.dwWidth;
    const u32 _h = Device.dwHeight;
    const u32 cnt_w = _w / tw;
    const u32 cnt_h = _h / th;
    const float end_u = start_u + float(cnt_w) + 1;
    const float end_v = start_v + float(cnt_h) + 1;

    p0.set(start_u, start_v);
    p1.set(end_u, end_v);
}

void CRenderTarget::u_calc_tc_duality_ss(Fvector2& r0, Fvector2& r1, Fvector2& l0, Fvector2& l1)
{
    // Calculate ordinaty TCs from blur and SS
    const float tw = static_cast<float>(get_width(RCache));
    const float th = static_cast<float>(get_height(RCache));
    if (th != Device.dwHeight)
        param_blur = 1.f;
    Fvector2 shift, p0, p1;
    shift.set(.5f / tw, .5f / th);
    shift.mul(param_blur);
    p0.set(.5f / tw, .5f / th).add(shift);
    p1.set((tw + .5f) / tw, (th + .5f) / th).add(shift);

    // Calculate Duality TC
    const float shift_u = param_duality_h * .5f;
    const float shift_v = param_duality_v * .5f;

    r0.set(p0.x, p0.y);
    r1.set(p1.x - shift_u, p1.y - shift_v);
    l0.set(p0.x + shift_u, p0.y + shift_v);
    l1.set(p1.x, p1.y);
}

bool CRenderTarget::u_need_CM() const { return (param_color_map_influence > 0.001f); }

struct TL_2c3uv
{
    Fvector4 p;
    u32 color0;
    u32 color1;
    Fvector2 uv[3];
    IC void set(float x, float y, u32 c0, u32 c1, float u0, float v0, float u1, float v1, float u2, float v2)
    {
        p.set(x, y, EPS_S, 1.f);
        color0 = c0;
        color1 = c1;
        uv[0].set(u0, v0);
        uv[1].set(u1, v1);
        uv[2].set(u2, v2);
    }
};

void CRenderTarget::phase_pp(CBackend& cmd_list)
{
    // combination/postprocess
    u_setrt(cmd_list, Device.dwWidth, Device.dwHeight, get_base_rt(), nullptr, nullptr, nullptr);

    //	Element 0 for normal post-process
    //	Element 4 for color map post-process
    const bool bCMap = u_need_CM();
    cmd_list.set_Element(s_postprocess->E[bCMap ? 4 : 0]);

    const int gblend = clampr(iFloor((1 - param_gray) * 255.f), 0, 255);
    const int nblend = clampr(iFloor((1 - param_noise) * 255.f), 0, 255);
    const u32 p_color = subst_alpha(param_color_base, nblend);
    const u32 p_gray = subst_alpha(param_color_gray, gblend);
    const Fvector p_brightness = param_color_add;
    // Msg				("param_gray:%f(%d),param_noise:%f(%d)",param_gray,gblend,param_noise,nblend);
    // Msg				("base: %d,%d,%d",	color_get_R(p_color),		color_get_G(p_color),		color_get_B(p_color));
    // Msg				("gray: %d,%d,%d",	color_get_R(p_gray),		color_get_G(p_gray),		color_get_B(p_gray));
    // Msg				("add:  %d,%d,%d",	color_get_R(p_brightness),	color_get_G(p_brightness),	color_get_B(p_brightness));

    // Draw full-screen quad textured with our scene image
    u32 Offset;
    const float _w = float(Device.dwWidth);
    const float _h = float(Device.dwHeight);

    Fvector2 n0, n1, r0, r1, l0, l1;
    u_calc_tc_duality_ss(r0, r1, l0, l1);
    u_calc_tc_noise(cmd_list, n0, n1);

    // Fill vertex buffer
    float du = 0, dv = 0;
    TL_2c3uv* pv = (TL_2c3uv*)RImplementation.Vertex.Lock(4, g_postprocess.stride(), Offset);
    pv->set(du + 0, dv + float(_h), p_color, p_gray, r0.x, r1.y, l0.x, l1.y, n0.x, n1.y);
    pv++;
    pv->set(du + 0, dv + 0, p_color, p_gray, r0.x, r0.y, l0.x, l0.y, n0.x, n0.y);
    pv++;
    pv->set(du + float(_w), dv + float(_h), p_color, p_gray, r1.x, r1.y, l1.x, l1.y, n1.x, n1.y);
    pv++;
    pv->set(du + float(_w), dv + 0, p_color, p_gray, r1.x, r0.y, l1.x, l0.y, n1.x, n0.y);
    pv++;
    RImplementation.Vertex.Unlock(4, g_postprocess.stride());

    // Actual rendering
    constexpr const char* s_brightness = "c_brightness";
    constexpr const char* s_colormap = "c_colormap";
    cmd_list.set_c(s_brightness, p_brightness.x, p_brightness.y, p_brightness.z, 0);
    cmd_list.set_c(s_colormap, param_color_map_influence, param_color_map_interpolate, 0, 0);
    cmd_list.set_Geometry(g_postprocess);
    cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
}
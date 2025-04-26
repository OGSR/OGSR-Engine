#include "stdafx.h"

#pragma pack(push, 4)
struct v_build
{
    Fvector4 p;
    Fvector2 uv0;
    Fvector2 uv1;
    Fvector2 uv2;
    Fvector2 uv3;
};

struct v_filter
{
    Fvector4 p;
    Fvector4 uv[8];
};
#pragma pack(pop)

struct Build_filter_kernel
{
    Fvector2 a[16]{}, b[16]{};
    consteval Build_filter_kernel(const float _src)
    {
        for (int k{}; k < 16; ++k)
        {
            const int _x = (k * 2 + 1) % 8; // 1,3,5,7
            const int _y = ((k / 4) * 2 + 1); // 1,1,1,1 ~ 3,3,3,3 ~...etc...
            a[k].set(_x, _y).div(_src);
            b[k].set(a[k]).add(1);
        }
    }
};

void CRenderTarget::phase_luminance(CBackend& cmd_list)
{
    u32 Offset = 0;
    constexpr float eps{}; //EPS_S

    // Targets
    cmd_list.set_Stencil(FALSE);
    cmd_list.set_CullMode(CULL_NONE);
    cmd_list.set_ColorWriteEnable();
    cmd_list.set_Z(FALSE);
    // CHK_DX									(HW.pDevice->SetRenderState	(D3DRS_ZENABLE,FALSE));

    // 000: Perform LUM-SAT, pass 0, 256x256 => 64x64
    u_setrt(cmd_list, rt_LUM_64, nullptr, nullptr, nullptr, nullptr);
    // RImplementation.rmNormal();
    {
        constexpr float ts{64};
        constexpr float _w{static_cast<float>(BLOOM_size_X)};
        constexpr float _h{static_cast<float>(BLOOM_size_Y)};
        constexpr Fvector2 one{2.f / _w, 2.f / _h}; // two, infact
        constexpr Fvector2 half{1.f / _w, 1.f / _h}; // one, infact
        constexpr Fvector2 a_0{half.x + 0, half.y + 0};
        constexpr Fvector2 a_1{half.x + one.x, half.y + 0};
        constexpr Fvector2 a_2{half.x + 0, half.y + one.y};
        constexpr Fvector2 a_3{half.x + one.x, half.y + one.y};
        constexpr Fvector2 b_0{1 + a_0.x, 1 + a_0.y};
        constexpr Fvector2 b_1{1 + a_1.x, 1 + a_1.y};
        constexpr Fvector2 b_2{1 + a_2.x, 1 + a_2.y};
        constexpr Fvector2 b_3{1 + a_3.x, 1 + a_3.y};

        // Fill vertex buffer
        v_build* pv = (v_build*)RImplementation.Vertex.Lock(4, g_bloom_build->vb_stride, Offset);
        pv->p.set(eps, float(ts + eps), eps, 1.f);
        pv->uv0.set(a_0.x, b_0.y);
        pv->uv1.set(a_1.x, b_1.y);
        pv->uv2.set(a_2.x, b_2.y);
        pv->uv3.set(a_3.x, b_3.y);
        pv++;
        pv->p.set(eps, eps, eps, 1.f);
        pv->uv0.set(a_0.x, a_0.y);
        pv->uv1.set(a_1.x, a_1.y);
        pv->uv2.set(a_2.x, a_2.y);
        pv->uv3.set(a_3.x, a_3.y);
        pv++;
        pv->p.set(float(ts + eps), float(ts + eps), eps, 1.f);
        pv->uv0.set(b_0.x, b_0.y);
        pv->uv1.set(b_1.x, b_1.y);
        pv->uv2.set(b_2.x, b_2.y);
        pv->uv3.set(b_3.x, b_3.y);
        pv++;
        pv->p.set(float(ts + eps), eps, eps, 1.f);
        pv->uv0.set(b_0.x, a_0.y);
        pv->uv1.set(b_1.x, a_1.y);
        pv->uv2.set(b_2.x, a_2.y);
        pv->uv3.set(b_3.x, a_3.y);
        pv++;
        RImplementation.Vertex.Unlock(4, g_bloom_build->vb_stride);
        cmd_list.set_Element(s_luminance->E[0]);
        cmd_list.set_Geometry(g_bloom_build);
        cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    }

    // 111: Perform LUM-SAT, pass 1, 64x64 => 8x8
    u_setrt(cmd_list, rt_LUM_8, nullptr, nullptr, nullptr, nullptr);
    // RImplementation.rmNormal();
    {
        // Build filter-kernel
        constexpr float _ts = 8.f;
        constexpr Build_filter_kernel filter_kernel{64.f};
        const Fvector2* a = filter_kernel.a;
        const Fvector2* b = filter_kernel.b;

        // Fill vertex buffer
        v_filter* pv = (v_filter*)RImplementation.Vertex.Lock(4, g_bloom_filter->vb_stride, Offset);
        pv->p.set(eps, float(_ts + eps), eps, 1.f);
        for (int t = 0; t < 8; t++)
            pv->uv[t].set(a[t].x, b[t].y, b[t + 8].y, a[t + 8].x); // xy/yx	- left+down
        pv++;
        pv->p.set(eps, eps, eps, 1.f);
        for (int t = 0; t < 8; t++)
            pv->uv[t].set(a[t].x, a[t].y, a[t + 8].y, a[t + 8].x); // xy/yx	- left+up
        pv++;
        pv->p.set(float(_ts + eps), float(_ts + eps), eps, 1.f);
        for (int t = 0; t < 8; t++)
            pv->uv[t].set(b[t].x, b[t].y, b[t + 8].y, b[t + 8].x); // xy/yx	- right+down
        pv++;
        pv->p.set(float(_ts + eps), eps, eps, 1.f);
        for (int t = 0; t < 8; t++)
            pv->uv[t].set(b[t].x, a[t].y, a[t + 8].y, b[t + 8].x); // xy/yx	- right+up
        pv++;
        RImplementation.Vertex.Unlock(4, g_bloom_filter->vb_stride);
        cmd_list.set_Element(s_luminance->E[1]);
        cmd_list.set_Geometry(g_bloom_filter);
        cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    }

    // 222: Perform LUM-SAT, pass 2, 8x8 => 1x1
    u_setrt(cmd_list, rt_LUM_pool[1], nullptr, nullptr, nullptr, nullptr);
    // RImplementation.rmNormal();
    {
        // Build filter-kernel
        constexpr float _ts = 1.f;
        constexpr Build_filter_kernel filter_kernel{8.f};
        const Fvector2* a = filter_kernel.a;
        const Fvector2* b = filter_kernel.b;

        // Fill vertex buffer
        v_filter* pv = (v_filter*)RImplementation.Vertex.Lock(4, g_bloom_filter->vb_stride, Offset);
        pv->p.set(eps, float(_ts + eps), eps, 1.f);
        for (int t = 0; t < 8; t++)
            pv->uv[t].set(a[t].x, b[t].y, b[t + 8].y, a[t + 8].x); // xy/yx	- left+down
        pv++;
        pv->p.set(eps, eps, eps, 1.f);
        for (int t = 0; t < 8; t++)
            pv->uv[t].set(a[t].x, a[t].y, a[t + 8].y, a[t + 8].x); // xy/yx	- left+up
        pv++;
        pv->p.set(float(_ts + eps), float(_ts + eps), eps, 1.f);
        for (int t = 0; t < 8; t++)
            pv->uv[t].set(b[t].x, b[t].y, b[t + 8].y, b[t + 8].x); // xy/yx	- right+down
        pv++;
        pv->p.set(float(_ts + eps), eps, eps, 1.f);
        for (int t = 0; t < 8; t++)
            pv->uv[t].set(b[t].x, a[t].y, a[t + 8].y, b[t + 8].x); // xy/yx	- right+up
        pv++;
        RImplementation.Vertex.Unlock(4, g_bloom_filter->vb_stride);

        f_luminance_adapt = .9f * f_luminance_adapt + .1f * Device.fTimeDelta * ps_r2_tonemap_adaptation;
        const float amount = ps_r2_ls_flags.test(R2FLAG_TONEMAP) ? ps_r2_tonemap_amount : 0;
        constexpr Fvector3 _none{1.f, 0.f, 1.f};
        const Fvector3 _full{ps_r2_tonemap_middlegray, 1.f, ps_r2_tonemap_low_lum};
        Fvector3 _result;
        _result.lerp(_none, _full, amount);

        cmd_list.set_Element(s_luminance->E[2]);
        cmd_list.set_Geometry(g_bloom_filter);
        cmd_list.set_c("MiddleGray", _result.x, _result.y, _result.z, f_luminance_adapt);
        cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    }

    // Cleanup states
    // CHK_DX		(HW.pDevice->SetRenderState(D3DRS_ZENABLE,TRUE));
    cmd_list.set_Z(TRUE);
}

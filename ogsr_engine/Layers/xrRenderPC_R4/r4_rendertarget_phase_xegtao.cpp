#include "stdafx.h"
#include "../xrRender/dxRenderDeviceRender.h"
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;
static_assert(sizeof(Fvector4) == 16, "XeGTAO constant-buffer layout must match HLSL float4");

// Kept local to AO: the general render-target abstraction has no mip UAVs.
class XeGTAOResources
{
public:
    struct Texture
    {
        ComPtr<ID3D11Texture2D> surface;
        ComPtr<ID3D11ShaderResourceView> srv;
        ComPtr<ID3D11UnorderedAccessView> uav[5];

        void create(u32 width, u32 height, DXGI_FORMAT format, u32 mips = 1)
        {
            D3D11_TEXTURE2D_DESC desc{};
            desc.Width = width;
            desc.Height = height;
            desc.MipLevels = mips;
            desc.ArraySize = 1;
            desc.Format = format;
            desc.SampleDesc.Count = 1;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
            CHK_DX(HW.pDevice->CreateTexture2D(&desc, nullptr, surface.GetAddressOf()));
            CHK_DX(HW.pDevice->CreateShaderResourceView(surface.Get(), nullptr, srv.GetAddressOf()));
            for (u32 mip = 0; mip < mips; ++mip)
            {
                D3D11_UNORDERED_ACCESS_VIEW_DESC view{};
                view.Format = format;
                view.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
                view.Texture2D.MipSlice = mip;
                CHK_DX(HW.pDevice->CreateUnorderedAccessView(surface.Get(), &view, uav[mip].GetAddressOf()));
            }
        }
    };

    u32 width{}, height{};
    Texture depth, ao, edges, filtered;
    ComPtr<ID3D11Buffer> constants;
    ComPtr<ID3D11SamplerState> pointClamp;
    ref_cs prefilter, evaluate, denoise;
    ref_texture output;
    ref_shader exportAO;

    ~XeGTAOResources()
    {
        if (output)
            output->surface_set(nullptr);
    }
};

void CRenderTarget::InitXeGTAO()
{
    R_ASSERT(!m_xegtao);
    m_xegtao = xr_new<XeGTAOResources>();
    auto& xe = *m_xegtao;
    // Five mips and complete 16x16 prefilter groups, including odd/tiny sizes.
    xe.width = (m_renderWidth + 15u) & ~15u;
    xe.height = (m_renderHeight + 15u) & ~15u;
    xe.depth.create(xe.width, xe.height, DXGI_FORMAT_R16_FLOAT, 5);
    xe.ao.create(xe.width, xe.height, DXGI_FORMAT_R8_UINT);
    xe.edges.create(xe.width, xe.height, DXGI_FORMAT_R8_UNORM);
    xe.filtered.create(xe.width, xe.height, DXGI_FORMAT_R8_UINT);

    D3D11_BUFFER_DESC cb{};
    cb.ByteWidth = 3 * sizeof(Fvector4); // ogsr_xegtao_common.h: three float4s
    cb.Usage = D3D11_USAGE_DYNAMIC;
    cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    CHK_DX(HW.pDevice->CreateBuffer(&cb, nullptr, xe.constants.GetAddressOf()));

    D3D11_SAMPLER_DESC sampler{};
    sampler.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sampler.AddressU = sampler.AddressV = sampler.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampler.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampler.MaxAnisotropy = 1;
    sampler.MaxLOD = D3D11_FLOAT32_MAX;
    CHK_DX(HW.pDevice->CreateSamplerState(&sampler, xe.pointClamp.GetAddressOf()));

    xe.prefilter = DEV->_CreateCS("ogsr_xegtao_prefilter");
    xe.evaluate = DEV->_CreateCS("ogsr_xegtao_main");
    xe.denoise = DEV->_CreateCS("ogsr_xegtao_denoise");
    xe.output.create("$user$xegtao");
    xe.output->surface_set(xe.filtered.surface.Get());
    xe.exportAO.create("ogsr_xegtao");

    Msg("* XeGTAO: full internal resolution %ux%u (padded %ux%u), quality %u; r_ao_resolution does not apply",
        m_renderWidth, m_renderHeight, xe.width, xe.height, ps_r_ao_quality);
}

void CRenderTarget::DestroyXeGTAO()
{
    xr_delete(m_xegtao);
}

void CRenderTarget::phase_xegtao(CBackend& cmd_list)
{
    PIX_EVENT_CTX(cmd_list, ao_xegtao);
    R_ASSERT(m_xegtao);
    auto& xe = *m_xegtao;
    auto* context = HW.get_context(cmd_list.context_id);

    // Match cl_pos_decompress_params and gbuffer_load_data, including jitter.
    // Scale the NDC span for padding, keeping actual pixels at their original positions.
    const float vertical = -tanf(deg2rad(Device.fFOV / 2.f));
    const float horizontal = -vertical / Device.fASPECT;
    const bool temporal = ps_r_pp_aa_mode == TAA || ps_r_pp_aa_mode == DLSS || ps_r_pp_aa_mode == FSR3;
    const Fvector4 data[3] = {
        {float(xe.width), float(xe.height), 1.f / xe.width, 1.f / xe.height},
        {float(m_renderWidth), float(m_renderHeight), float(temporal ? Device.dwFrame % 64 : 0), ps_r_xegtao_radius},
        {2.f * horizontal * xe.width / m_renderWidth, 2.f * vertical * xe.height / m_renderHeight,
            -horizontal * (1.f + ps_r_taa_jitter.x), -vertical * (1.f - ps_r_taa_jitter.y)}};
    D3D11_MAPPED_SUBRESOURCE mapped{};
    CHK_DX(context->Map(xe.constants.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped));
    CopyMemory(mapped.pData, data, sizeof(data));
    context->Unmap(xe.constants.Get(), 0);

    // Isolate direct compute bindings from the backend's graphics state cache.
    // ClearState also removes any previous SRV/RTV aliases before binding UAVs.
    context->ClearState();
    ID3D11Buffer* cb = xe.constants.Get();
    ID3D11SamplerState* sampler = xe.pointClamp.Get();
    context->CSSetConstantBuffers(0, 1, &cb);
    context->CSSetSamplers(0, 1, &sampler);
    ID3D11ShaderResourceView* noSRV[2]{};
    ID3D11UnorderedAccessView* noUAV[5]{};

    {
        PIX_EVENT_CTX(cmd_list, ao_xegtao_prefilter);
        ID3D11ShaderResourceView* input = rt_Position->pTexture->get_SRView();
        ID3D11UnorderedAccessView* output[5] = {
            xe.depth.uav[0].Get(), xe.depth.uav[1].Get(), xe.depth.uav[2].Get(), xe.depth.uav[3].Get(), xe.depth.uav[4].Get()};
        context->CSSetShader(xe.prefilter->sh, nullptr, 0);
        context->CSSetShaderResources(0, 1, &input);
        context->CSSetUnorderedAccessViews(0, 5, output, nullptr);
        context->Dispatch(xe.width / 16, xe.height / 16, 1);
        context->CSSetUnorderedAccessViews(0, 5, noUAV, nullptr);
    }
    {
        PIX_EVENT_CTX(cmd_list, ao_xegtao_evaluate);
        ID3D11ShaderResourceView* inputs[2] = {rt_Position->pTexture->get_SRView(), xe.depth.srv.Get()};
        ID3D11UnorderedAccessView* outputs[2] = {xe.ao.uav[0].Get(), xe.edges.uav[0].Get()};
        context->CSSetShader(xe.evaluate->sh, nullptr, 0);
        context->CSSetShaderResources(0, 2, inputs);
        context->CSSetUnorderedAccessViews(0, 2, outputs, nullptr);
        context->Dispatch(xe.width / 8, xe.height / 8, 1);
        context->CSSetUnorderedAccessViews(0, 2, noUAV, nullptr);
        context->CSSetShaderResources(0, 2, noSRV);
    }
    {
        PIX_EVENT_CTX(cmd_list, ao_xegtao_denoise);
        ID3D11ShaderResourceView* inputs[2] = {xe.ao.srv.Get(), xe.edges.srv.Get()};
        ID3D11UnorderedAccessView* output = xe.filtered.uav[0].Get();
        context->CSSetShader(xe.denoise->sh, nullptr, 0);
        context->CSSetShaderResources(0, 2, inputs);
        context->CSSetUnorderedAccessViews(0, 1, &output, nullptr);
        context->Dispatch(xe.width / 16, xe.height / 8, 1);
    }

    context->ClearState();
    cmd_list.Invalidate();
    cmd_list.set_ColorWriteEnable();
    {
        PIX_EVENT_CTX(cmd_list, ao_xegtao_export);
        RenderScreenTriangle(cmd_list, rt_ao, xe.exportAO->E[0]);
    }
}

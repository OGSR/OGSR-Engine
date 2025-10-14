//----------------------------------------------------------
//-- r4_imgui.cpp for imgui features and tweaks
//----------------------------------------------------------
#include "stdafx.h"
#include "..\xrRenderDX10\imgui_impl_dx11.h"

void CRender::CaptureMainRTTexture()
{
    ID3D11Resource* baseRes{};
    Target->get_base_rt()->GetResource(&baseRes);

    if (baseRes)
    {
        HW.get_context(CHW::IMM_CTX_ID)->CopyResource(Target->m_ImguiTex, baseRes);
        _RELEASE(baseRes);
    }
}

void CRender::DrawMainRTTexture(float width, float height)
{
    if (Target->m_ImguiSRV)
        ImGui::Image(Target->m_ImguiSRV, ImVec2{width, height});
}

void CRender::DrawRTTextures()
{
    if (!RTdbgBS)
    {
        D3D11_BLEND_DESC desc{};
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_DEST_COLOR;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        R_CHK(HW.pDevice->CreateBlendState(&desc, &RTdbgBS));
    }

    auto DrawTexture = [&](const ref_rt& rt) {
        auto& tex{rt->pTexture};

        ImGui::Text("RT [%s][%u x %u]", tex->cName.c_str(), rt->dwWidth, rt->dwHeight);

        auto* DL{ImGui::GetWindowDrawList()};

        DL->AddCallback(
            [](auto, const ImDrawCmd* cmd) {
                if (auto* bs = static_cast<ID3D11BlendState*>(cmd->UserCallbackData))
                {
                    constexpr FLOAT bf[4]{};
                    HW.get_context(CHW::IMM_CTX_ID)->OMSetBlendState(bs, bf, UINT(-1));
                }
            },
            RTdbgBS);

        const float cra_w{ImGui::GetContentRegionAvail().x};
        const float scl{cra_w / static_cast<float>(rt->dwWidth)};
        ImGui::Image(tex->get_SRView(), ImVec2{cra_w, static_cast<float>(rt->dwHeight) * scl});

        DL->AddCallback(
            [](auto, auto) {
                if (auto* bs = ImGui_ImplDX11_GetBlendState())
                {
                    constexpr FLOAT bf[4]{};
                    HW.get_context(CHW::IMM_CTX_ID)->OMSetBlendState(bs, bf, UINT(-1));
                }
            },
            nullptr);

        ImGui::Separator();
    };

    //-- add more RTs here if needed
    DrawTexture(Target->rt_Color);
    DrawTexture(Target->rt_Position);
    DrawTexture(Target->rt_Accumulator);
    DrawTexture(Target->rt_heat);
    DrawTexture(Target->rt_Generic_1);
    DrawTexture(Target->rt_Generic_2);
    DrawTexture(Target->rt_Bloom_1);
    DrawTexture(Target->rt_Generic_combine);
    DrawTexture(Target->rt_Generic_0);
    DrawTexture(Target->rt_Velocity);
    DrawTexture(Target->rt_Base_Depth);
}

#include "stdafx.h"
#include "dx10State.h"

//#include "dx10RSManager.h"
#include "dx10StateCache.h"

dx10State::dx10State() : m_pRasterizerState(nullptr), m_pDepthStencilState(nullptr), m_pBlendState(nullptr), m_uiStencilRef(UINT(-1)), m_uiAlphaRef(0) {}

dx10State::~dx10State()
{
    //	m_pRasterizerState is a weak link
    //	m_pDepthStencilState is a weak link
    //	m_pBlendState is a weak link
}

dx10State* dx10State::Create(SimulatorStates& state_code)
{
    dx10State* pState = xr_new<dx10State>();

    state_code.UpdateState(*pState);

    pState->m_pRasterizerState = RSManager.GetState(state_code);
    pState->m_pDepthStencilState = DSSManager.GetState(state_code);
    pState->m_pBlendState = BSManager.GetState(state_code);
    // ID3DxxDevice::CreateSamplerState

    //	Create samplers here
    {
        InitSamplers(pState->m_VSSamplers, state_code, CTexture::rstVertex);
        InitSamplers(pState->m_PSSamplers, state_code, CTexture::rstPixel);
        InitSamplers(pState->m_GSSamplers, state_code, CTexture::rstGeometry);
        InitSamplers(pState->m_HSSamplers, state_code, CTexture::rstHull);
        InitSamplers(pState->m_DSSamplers, state_code, CTexture::rstDomain);
        InitSamplers(pState->m_CSSamplers, state_code, CTexture::rstCompute);
    }

    return pState;
}

HRESULT dx10State::Apply(CBackend& cmd_list) const
{
    const auto context_id = cmd_list.context_id;

    VERIFY(m_pRasterizerState);
    cmd_list.StateManager.SetRasterizerState(m_pRasterizerState);
    VERIFY(m_pDepthStencilState);
    cmd_list.StateManager.SetDepthStencilState(m_pDepthStencilState);
    if (m_uiStencilRef != -1)
        cmd_list.StateManager.SetStencilRef(m_uiStencilRef);
    VERIFY(m_pBlendState);
    cmd_list.StateManager.SetBlendState(m_pBlendState);
    cmd_list.StateManager.SetAlphaRef(m_uiAlphaRef);

    SSManager.GSApplySamplers(context_id, m_GSSamplers);
    SSManager.VSApplySamplers(context_id, m_VSSamplers);
    SSManager.PSApplySamplers(context_id, m_PSSamplers);
    SSManager.HSApplySamplers(context_id, m_HSSamplers);
    SSManager.DSApplySamplers(context_id, m_DSSamplers);
    SSManager.CSApplySamplers(context_id, m_CSSamplers);

    return S_OK;
}

void dx10State::Release()
{
    dx10State* pState = this;
    xr_delete(pState);
}

void dx10State::InitSamplers(tSamplerHArray& SamplerArray, SimulatorStates& state_code, int iBaseSamplerIndex)
{
    D3D_SAMPLER_DESC descArray[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT];
    bool SamplerUsed[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT];

    for (int i = 0; i < D3D_COMMONSHADER_SAMPLER_SLOT_COUNT; ++i)
    {
        SamplerUsed[i] = false;
        dx10StateUtils::ResetDescription(descArray[i]);
    }

    state_code.UpdateDesc(descArray, SamplerUsed, iBaseSamplerIndex);

    int iMaxSampler = D3D_COMMONSHADER_SAMPLER_SLOT_COUNT - 1;
    for (; iMaxSampler > -1; --iMaxSampler)
    {
        if (SamplerUsed[iMaxSampler])
            break;
    }

    if (iMaxSampler > -1)
    {
        SamplerArray.reserve(iMaxSampler + 1);
        for (int i = 0; i <= iMaxSampler; ++i)
        {
            if (SamplerUsed[i])
                SamplerArray.push_back(SSManager.GetState(descArray[i]));
            else
                SamplerArray.push_back(u32(dx10SamplerStateCache::hInvalidHandle));
        }
    }
}
#include "stdafx.h"
#include "dx10SamplerStateCache.h"
#include "../dx10StateUtils.h"
#include <comdef.h> // for _com_error

using dx10StateUtils::operator==;

dx10SamplerStateCache SSManager;

dx10SamplerStateCache::dx10SamplerStateCache() : m_uiMaxAnisotropy(1), m_uiMipLODBias(0.0f)
{
    constexpr size_t iMaxRSStates = 10;

    m_StateArray.reserve(iMaxRSStates);
    m_StateArraySmap.reserve(iMaxRSStates);
}

dx10SamplerStateCache::~dx10SamplerStateCache() { ClearStateArray(); }

dx10SamplerStateCache::SHandle dx10SamplerStateCache::GetStateHandle(D3D_SAMPLER_DESC& desc)
{
    dx10StateUtils::ValidateState(desc);

    const u32 xxh = dx10StateUtils::GetHash(desc);

    // search for existing state in main table
    SHandle hResult = FindStateHandler(desc, xxh);

    if (hResult == static_cast<u32>(hInvalidHandle))
    {
        desc.MaxAnisotropy = m_uiMaxAnisotropy;
        desc.MipLODBias = m_uiMipLODBias;

        StateRecord rec;
        rec.m_crc = xxh;
        CreateState(desc, &rec.m_pState);

        hResult = m_StateArray.size();
        m_StateArray.push_back(rec);

        // create smap state
        StateDecs descSMAP{};

        rec.m_pState->GetDesc(&descSMAP);

        // SMAP states are the same as main states but with disabled anisotropy and low mip LOD bias
        descSMAP.MaxAnisotropy = ps_r__tf_Anisotropic_SMAP;
        descSMAP.MipLODBias = ps_r__tf_Mipbias_SMAP;
        dx10StateUtils::ValidateState(descSMAP);

        StateRecord rec2;
        rec2.m_crc = xxh; // not important since smap states are not searched
        CreateState(descSMAP, &rec2.m_pState);

        m_StateArraySmap.push_back(rec2);

        R_ASSERT(m_StateArray.size() == m_StateArraySmap.size());
    }

    return hResult;
}

void dx10SamplerStateCache::CreateState(const StateDecs& desc, IDeviceState** ppIState)
{
    const auto hr = HW.pDevice->CreateSamplerState(&desc, ppIState);
    if (FAILED(hr))
    {
        const auto hr2 = HW.pDevice->GetDeviceRemovedReason();
        FATAL("!!FAILED HW.pDevice->CreateSamplerState! Error: [%s] GetDeviceRemovedReason returns: [%s].", _com_error{hr}.ErrorMessage(),
              FAILED(hr2) ? _com_error{hr2}.ErrorMessage() : "No device removal error detected");
    }
}

dx10SamplerStateCache::SHandle dx10SamplerStateCache::FindStateHandler(const StateDecs& desc, const u32 StateXXH) const
{
    u32 res = 0xffffffff;
    u32 i = 0;

    for (; i < m_StateArray.size(); ++i)
    {
        if (m_StateArray[i].m_crc == StateXXH)
        {
            StateDecs descCandidate;
            m_StateArray[i].m_pState->GetDesc(&descCandidate);
            if (descCandidate == desc)
            {
                res = i;
                break;
            }
        }
    }

    return res != 0xffffffff ? i : (u32)hInvalidHandle;
}

void dx10SamplerStateCache::ClearStateArray()
{
    for (auto& i : m_StateArray)
    {
        _RELEASE(i.m_pState);
    }

    m_StateArray.clear();

    for (auto& i : m_StateArraySmap)
    {
        _RELEASE(i.m_pState);
    }

    m_StateArraySmap.clear();

    m_uiMaxAnisotropy = (1);
    m_uiMipLODBias = (0.0f);
}

void dx10SamplerStateCache::PrepareSamplerStates(const HArray& samplers, ID3DSamplerState* pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT], bool smap) const
{
    VERIFY(samplers.size() <= D3D_COMMONSHADER_SAMPLER_SLOT_COUNT);
    for (u32 i = 0; i < samplers.size(); ++i)
    {
        if (samplers[i] != static_cast<u32>(hInvalidHandle))
        {
            VERIFY(samplers[i] < m_StateArray.size());
            pSS[i] = (smap ? m_StateArraySmap : m_StateArray)[samplers[i]].m_pState;
        }
    }
}


void dx10SamplerStateCache::VSApplySamplers(const u32 context_id, const HArray& samplers, bool smap) const
{
    ID3DSamplerState* pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT] = {};
    PrepareSamplerStates(samplers, pSS, smap);
    HW.get_context(context_id)->VSSetSamplers(0, D3D_COMMONSHADER_SAMPLER_SLOT_COUNT, pSS);
}

void dx10SamplerStateCache::PSApplySamplers(const u32 context_id, const HArray& samplers, bool smap) const
{
    ID3DSamplerState* pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT] = {};
    PrepareSamplerStates(samplers, pSS, smap);
    HW.get_context(context_id)->PSSetSamplers(0, D3D_COMMONSHADER_SAMPLER_SLOT_COUNT, pSS);
}

void dx10SamplerStateCache::GSApplySamplers(const u32 context_id, const HArray& samplers, bool smap) const
{
    ID3DSamplerState* pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT] = {};
    PrepareSamplerStates(samplers, pSS, smap);
    HW.get_context(context_id)->GSSetSamplers(0, D3D_COMMONSHADER_SAMPLER_SLOT_COUNT, pSS);
}

void dx10SamplerStateCache::HSApplySamplers(const u32 context_id, const HArray& samplers, bool smap) const
{
    ID3DSamplerState* pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT] = {};
    PrepareSamplerStates(samplers, pSS, smap);
    HW.get_context(context_id)->HSSetSamplers(0, D3D_COMMONSHADER_SAMPLER_SLOT_COUNT, pSS);
}

void dx10SamplerStateCache::DSApplySamplers(const u32 context_id, const HArray& samplers, bool smap) const
{
    ID3DSamplerState* pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT] = {};
    PrepareSamplerStates(samplers, pSS, smap);
    HW.get_context(context_id)->DSSetSamplers(0, D3D_COMMONSHADER_SAMPLER_SLOT_COUNT, pSS);
}

void dx10SamplerStateCache::CSApplySamplers(const u32 context_id, const HArray& samplers, bool smap) const
{
    ID3DSamplerState* pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT] = {};
    PrepareSamplerStates(samplers, pSS, smap);
    HW.get_context(context_id)->CSSetSamplers(0, D3D_COMMONSHADER_SAMPLER_SLOT_COUNT, pSS);
}

void dx10SamplerStateCache::SetParams(u32 uiMaxAniso, float uiMipLODBias)
{
    clamp(uiMaxAniso, static_cast<u32>(1), static_cast<u32>(16));
    clamp(uiMipLODBias, -3.f, 3.f);

    if (m_uiMaxAnisotropy == uiMaxAniso && fsimilar(m_uiMipLODBias, uiMipLODBias))
        return;

    m_uiMaxAnisotropy = uiMaxAniso;
    m_uiMipLODBias = uiMipLODBias;

    // set params only for main state
    for (auto& rec : m_StateArray)
    {
        StateDecs desc{};

        rec.m_pState->GetDesc(&desc);

        //	MaxAnisitropy is reset by ValidateState if not aplicable
        //	to the filter mode used.
        //	Reason: all checks for aniso applicability are done
        //	in ValidateState.
        desc.MaxAnisotropy = m_uiMaxAnisotropy;
        desc.MipLODBias = m_uiMipLODBias;
        dx10StateUtils::ValidateState(desc);

        // This can cause fragmentation if called too often
        rec.m_pState->Release();
        CreateState(desc, &rec.m_pState);
    }
}
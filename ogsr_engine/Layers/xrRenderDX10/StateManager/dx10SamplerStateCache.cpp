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
}

dx10SamplerStateCache::~dx10SamplerStateCache() { ClearStateArray(); }

dx10SamplerStateCache::SHandle dx10SamplerStateCache::GetState(D3D_SAMPLER_DESC& desc)
{
    SHandle hResult;

    //	MaxAnisitropy is reset by ValidateState if not aplicable
    //	to the filter mode used.
    desc.MaxAnisotropy = m_uiMaxAnisotropy;
    // RZ
    desc.MipLODBias = m_uiMipLODBias;

    dx10StateUtils::ValidateState(desc);

    const u32 crc = dx10StateUtils::GetHash(desc);

    hResult = FindState(desc, crc);

    if (hResult == hInvalidHandle)
    {
        StateRecord rec;
        rec.m_crc = crc;
        CreateState(desc, &rec.m_pState);
        hResult = m_StateArray.size();
        m_StateArray.push_back(rec);
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

dx10SamplerStateCache::SHandle dx10SamplerStateCache::FindState(const StateDecs& desc, u32 StateCRC) const
{
    u32 res = 0xffffffff;
    u32 i = 0;

    for (; i < m_StateArray.size(); ++i)
    {
        if (m_StateArray[i].m_crc == StateCRC)
        {
            StateDecs descCandidate;
            m_StateArray[i].m_pState->GetDesc(&descCandidate);
            if (descCandidate == desc)
            // return i;
            //	TEST
            {
                // return i;
                res = i;
                break;
            }
            // else
            //{
            //	VERIFY(0);
            // }
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
}

void dx10SamplerStateCache::PrepareSamplerStates(const HArray& samplers, ID3DSamplerState* pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT]) const
{
    VERIFY(samplers.size() <= D3D_COMMONSHADER_SAMPLER_SLOT_COUNT);
    for (u32 i = 0; i < samplers.size(); ++i)
    {
        if (samplers[i] != hInvalidHandle)
        {
            VERIFY(samplers[i] < m_StateArray.size());
            pSS[i] = m_StateArray[samplers[i]].m_pState;
        }
    }
}


void dx10SamplerStateCache::VSApplySamplers(u32 context_id, const HArray& samplers) const
{
    ID3DSamplerState* pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT] = {};
    PrepareSamplerStates(samplers, pSS);
    HW.get_context(context_id)->VSSetSamplers(0, D3D_COMMONSHADER_SAMPLER_SLOT_COUNT, pSS);
}

void dx10SamplerStateCache::PSApplySamplers(u32 context_id, const HArray& samplers) const
{
    ID3DSamplerState* pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT] = {};
    PrepareSamplerStates(samplers, pSS);
    HW.get_context(context_id)->PSSetSamplers(0, D3D_COMMONSHADER_SAMPLER_SLOT_COUNT, pSS);
}

void dx10SamplerStateCache::GSApplySamplers(u32 context_id, const HArray& samplers) const
{
    ID3DSamplerState* pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT] = {};
    PrepareSamplerStates(samplers, pSS);
    HW.get_context(context_id)->GSSetSamplers(0, D3D_COMMONSHADER_SAMPLER_SLOT_COUNT, pSS);
}

void dx10SamplerStateCache::HSApplySamplers(u32 context_id, const HArray& samplers) const
{
    ID3DSamplerState* pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT] = {};
    PrepareSamplerStates(samplers, pSS);
    HW.get_context(context_id)->HSSetSamplers(0, D3D_COMMONSHADER_SAMPLER_SLOT_COUNT, pSS);
}

void dx10SamplerStateCache::DSApplySamplers(u32 context_id, const HArray& samplers) const
{
    ID3DSamplerState* pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT] = {};
    PrepareSamplerStates(samplers, pSS);
    HW.get_context(context_id)->DSSetSamplers(0, D3D_COMMONSHADER_SAMPLER_SLOT_COUNT, pSS);
}

void dx10SamplerStateCache::CSApplySamplers(u32 context_id, const HArray& samplers) const
{
    ID3DSamplerState* pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT] = {};
    PrepareSamplerStates(samplers, pSS);
    HW.get_context(context_id)->CSSetSamplers(0, D3D_COMMONSHADER_SAMPLER_SLOT_COUNT, pSS);
}

void dx10SamplerStateCache::SetMaxAnisotropy(u32 uiMaxAniso)
{
    clamp(uiMaxAniso, (u32)1, (u32)16);

    if (m_uiMaxAnisotropy == uiMaxAniso)
        return;

    m_uiMaxAnisotropy = uiMaxAniso;

    for (auto& rec : m_StateArray)
    {
        StateDecs desc{};

        rec.m_pState->GetDesc(&desc);

        //	MaxAnisitropy is reset by ValidateState if not aplicable
        //	to the filter mode used.
        //	Reason: all checks for aniso applicability are done
        //	in ValidateState.
        desc.MaxAnisotropy = m_uiMaxAnisotropy;
        dx10StateUtils::ValidateState(desc);

        //	This can cause fragmentation if called too often
        rec.m_pState->Release();
        CreateState(desc, &rec.m_pState);
    }
}

void dx10SamplerStateCache::SetMipLODBias(float uiMipLODBias)
{
    if (m_uiMipLODBias == uiMipLODBias)
        return;

    m_uiMipLODBias = uiMipLODBias;

    for (auto& rec : m_StateArray)
    {
        StateDecs desc{};

        rec.m_pState->GetDesc(&desc);

        desc.MipLODBias = m_uiMipLODBias;
        dx10StateUtils::ValidateState(desc);

        // This can cause fragmentation if called too often
        rec.m_pState->Release();
        CreateState(desc, &rec.m_pState);
    }
}

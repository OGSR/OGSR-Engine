#pragma once

class dx10SamplerStateCache
{
public:
    enum
    {
        hInvalidHandle = 0xFFFFFFFF
    };

    //	State handle
    typedef u32 SHandle;
    typedef xr_vector<SHandle> HArray;

public:
    dx10SamplerStateCache();
    ~dx10SamplerStateCache();

    void ClearStateArray();

    SHandle GetState(D3D_SAMPLER_DESC& desc);

    void VSApplySamplers(u32 context_id, const HArray& samplers) const;
    void PSApplySamplers(u32 context_id, const HArray& samplers) const;
    void GSApplySamplers(u32 context_id, const HArray& samplers) const;
    void HSApplySamplers(u32 context_id, const HArray& samplers) const;
    void DSApplySamplers(u32 context_id, const HArray& samplers) const;
    void CSApplySamplers(u32 context_id, const HArray& samplers) const;

    void SetMaxAnisotropy(u32 uiMaxAniso);
    void SetMipLODBias(float uiMipLODBias);

    //	Private declarations
private:
    typedef ID3DSamplerState IDeviceState;
    typedef D3D_SAMPLER_DESC StateDecs;

    struct StateRecord
    {
        u32 m_crc{};
        IDeviceState* m_pState{};
    };

private:
    void CreateState(const StateDecs& desc, IDeviceState** ppIState);
    SHandle FindState(const StateDecs& desc, u32 StateCRC) const;

    void PrepareSamplerStates(const HArray& samplers, ID3DSamplerState* pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT]) const;

    //	Private data
private:
    //	This must be cleared on device destroy
    xr_vector<StateRecord> m_StateArray;

    u32 m_uiMaxAnisotropy;
    float m_uiMipLODBias;
};

extern dx10SamplerStateCache SSManager;

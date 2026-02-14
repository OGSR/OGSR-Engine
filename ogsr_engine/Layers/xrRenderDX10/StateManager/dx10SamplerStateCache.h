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

    SHandle GetStateHandle(D3D_SAMPLER_DESC& desc);

    void VSApplySamplers(const u32 context_id, const HArray& samplers, bool smap) const;
    void PSApplySamplers(const u32 context_id, const HArray& samplers, bool smap) const;
    void GSApplySamplers(const u32 context_id, const HArray& samplers, bool smap) const;
    void HSApplySamplers(const u32 context_id, const HArray& samplers, bool smap) const;
    void DSApplySamplers(const u32 context_id, const HArray& samplers, bool smap) const;
    void CSApplySamplers(const u32 context_id, const HArray& samplers, bool smap) const;

    //void SetMaxAnisotropy(u32 uiMaxAniso);
    //void SetMipLODBias(float uiMipLODBias);

    void SetParams(u32 uiMaxAniso, float uiMipLODBias);

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

    SHandle FindStateHandler(const StateDecs& desc, u32 StateXXH) const;

    void PrepareSamplerStates(const HArray& samplers, ID3DSamplerState* pSS[D3D_COMMONSHADER_SAMPLER_SLOT_COUNT], bool smap) const;

private:

    //	This must be cleared on device destroy

    // main state 
    xr_vector<StateRecord> m_StateArray;

    // smap state
    xr_vector<StateRecord> m_StateArraySmap;

    u32 m_uiMaxAnisotropy;
    float m_uiMipLODBias;
};

extern dx10SamplerStateCache SSManager;

// HW.h: interface for the CHW class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <dxgi1_4.h>

#include "hwcaps.h"

#include "stats_manager.h"

class CHW
    : public pureAppActivate,
      public pureAppDeactivate
{
    //	Functions section
public:
    CHW();
    ~CHW();

    void CreateD3D();
    void DestroyD3D();

    void CreateDevice(HWND hw);
    void DestroyDevice();
    void ResetDevice(HWND hw);

    void DumpVideoMemoryUsage() const;

//	Variables section
public:
    static constexpr auto IMM_CTX_ID = R__NUM_PARALLEL_CONTEXTS;
    static constexpr auto INVALID_CONTEXT_ID = static_cast<u32>(-1);

    ID3D11Device* pDevice = nullptr; // render device

    IDXGIFactory1* m_pFactory = nullptr;
    IDXGIAdapter1* m_pAdapter = nullptr;
    IDXGISwapChain2* m_pSwapChain = nullptr;

    CHWCaps Caps;

    DXGI_SWAP_CHAIN_DESC m_ChainDesc;
    D3D_FEATURE_LEVEL FeatureLevel;

    stats_manager stats_manager{};

    TracyD3D11Ctx profiler_ctx{}; // TODO: this should be one per d3d11 context

    bool CheckFormatSupport(DXGI_FORMAT format, u32 feature) const;

    virtual void OnAppActivate();
    virtual void OnAppDeactivate();

    ICF ID3D11DeviceContext* get_context(u32 context_id) const
    {
        ASSERT_FMT(context_id < R__NUM_CONTEXTS, "wrong context id: [%u]", context_id);
        return d3d_contexts_pool[context_id];
    }

    bool ThisInstanceIsGlobal() const;
    void WaitOnSwapChain() const;

private:
    bool b_modern{};

    void SelectResolution(u32& dwWidth, u32& dwHeight);

    void UpdateWindowProps(HWND hw) const;

    void CreateSwapChain2(HWND hw);

    ID3D11DeviceContext* d3d_contexts_pool[R__NUM_CONTEXTS]{};

    HANDLE m_frameLatencyWaitableObject{};
};

extern ECORE_API CHW HW;

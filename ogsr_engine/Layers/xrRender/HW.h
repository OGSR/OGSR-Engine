// HW.h: interface for the CHW class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <dxgi1_4.h>

#include "hwcaps.h"

#ifndef _MAYA_EXPORT
#include "stats_manager.h"
#endif

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

    void Reset(HWND hw);

    void selectResolution(u32& dwWidth, u32& dwHeight, BOOL bWindowed);
    D3DFORMAT selectDepthStencil(D3DFORMAT);
    u32 selectPresentInterval();
    u32 selectGPU();
    u32 selectRefresh(u32 dwWidth, u32 dwHeight, D3DFORMAT fmt);

    void updateWindowProps(HWND hw);
    BOOL support(D3DFORMAT fmt, DWORD type, DWORD usage);

    void DumpVideoMemoryUsage() const;

//	Variables section
public:
    ID3D11Device* pDevice = nullptr; // render device
    ID3D11RenderTargetView* pBaseRT = nullptr; // base render target
    ID3D11DepthStencilView* pBaseZB = nullptr; // base depth-stencil buffer

    IDXGIFactory1* pFactory = nullptr;
    IDXGIAdapter1* m_pAdapter = nullptr;
    IDXGIAdapter3* m_pAdapter3{};
    ID3D11DeviceContext* pContext = nullptr;
    IDXGISwapChain* m_pSwapChain = nullptr;

    ID3DUserDefinedAnnotation* pAnnotation{};

    CHWCaps Caps;

    DXGI_SWAP_CHAIN_DESC m_ChainDesc{}; //	DevPP equivalent
    D3D_FEATURE_LEVEL FeatureLevel;

    stats_manager stats_manager{};

    bool doPresentTest{};

    void UpdateViews();

    DXGI_RATIONAL selectRefresh(u32 dwWidth, u32 dwHeight, DXGI_FORMAT fmt);

    virtual void OnAppActivate();
    virtual void OnAppDeactivate();
};

extern ECORE_API CHW HW;

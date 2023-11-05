// HW.h: interface for the CHW class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HW_H__0E25CF4A_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_HW_H__0E25CF4A_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_
#pragma once

#include "hwcaps.h"

#ifndef _MAYA_EXPORT
#include "stats_manager.h"
#endif

class CHW
#if defined(USE_DX10) || defined(USE_DX11)
    : public pureAppActivate,
      public pureAppDeactivate
#endif //	USE_DX10
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

#ifdef DEBUG
#if defined(USE_DX10) || defined(USE_DX11)
    void Validate(void){};
#else //	USE_DX10
    void Validate(void)
    {
        VERIFY(pDevice);
        VERIFY(pD3D);
    };
#endif //	USE_DX10
#else
    void Validate(void){};
#endif

//	Variables section
#if defined(USE_DX11) //	USE_DX10
public:
    ID3D11Device* pDevice = nullptr; // render device
    ID3D11RenderTargetView* pBaseRT = nullptr; // base render target
    ID3D11DepthStencilView* pBaseZB = nullptr; // base depth-stencil buffer

    IDXGIFactory1* pFactory = nullptr;
    IDXGIAdapter1* m_pAdapter = nullptr;
    ID3D11DeviceContext* pContext = nullptr;
    IDXGISwapChain* m_pSwapChain = nullptr;

    CHWCaps Caps;

    DXGI_SWAP_CHAIN_DESC m_ChainDesc; //	DevPP equivalent
    D3D_FEATURE_LEVEL FeatureLevel;
#elif defined(USE_DX10)
public:
    IDXGIFactory* pFactory = nullptr;
    IDXGIAdapter* m_pAdapter = nullptr;
    ID3D10Device1* pDevice1 = nullptr;
    ID3D10Device* pDevice = nullptr; // render device
    ID3D10Device1* pContext1 = nullptr;
    ID3D10Device* pContext = nullptr;
    IDXGISwapChain* m_pSwapChain = nullptr;
    ID3D10RenderTargetView* pBaseRT = nullptr; // base render target
    ID3D10DepthStencilView* pBaseZB = nullptr; // base depth-stencil buffer

    CHWCaps Caps;

    D3D10_DRIVER_TYPE m_DriverType; //	DevT equivalent
    DXGI_SWAP_CHAIN_DESC m_ChainDesc; //	DevPP equivalent
    D3D_FEATURE_LEVEL FeatureLevel;
#else
private:
    HINSTANCE hD3D;

public:
    IDirect3D9* pD3D; // D3D
    IDirect3DDevice9* pDevice; // render device

    IDirect3DSurface9* pBaseRT;
    IDirect3DSurface9* pBaseZB;

    CHWCaps Caps{};

    UINT DevAdapter{};
    D3DDEVTYPE DevT{};
    D3DPRESENT_PARAMETERS DevPP{};
#endif //	USE_DX10

#ifndef _MAYA_EXPORT
    stats_manager stats_manager{};
#endif
#if defined(USE_DX10) || defined(USE_DX11)
    void UpdateViews();
    DXGI_RATIONAL selectRefresh(u32 dwWidth, u32 dwHeight, DXGI_FORMAT fmt);

    virtual void OnAppActivate();
    virtual void OnAppDeactivate();
#endif //	USE_DX10
};

extern ECORE_API CHW HW;

#endif // !defined(AFX_HW_H__0E25CF4A_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)

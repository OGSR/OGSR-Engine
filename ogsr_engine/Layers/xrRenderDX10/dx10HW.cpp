// dx10HW.cpp: implementation of the DX10 specialisation of CHW.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../xrRender/HW.h"
#include "../../xr_3da/XR_IOConsole.h"
#include "../../xr_3da/xr_input.h"
#include "../../Include/xrAPI/xrAPI.h"

#include "StateManager/dx10SamplerStateCache.h"
#include "StateManager/dx10StateCache.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"

#include <dxgi1_6.h>

void fill_vid_mode_list(CHW* _hw);
void free_vid_mode_list();

CHW HW;

CHW::CHW()
{
    if (!ThisInstanceIsGlobal())
    {
        ASSERT_FMT(false, "non global CHW");
    }

    Device.seqAppActivate.Add(this);
    Device.seqAppDeactivate.Add(this);

    DEVMODE dmi{};
    EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &dmi);
    psCurrentVidMode[0] = dmi.dmPelsWidth;
    psCurrentVidMode[1] = dmi.dmPelsHeight;
}

CHW::~CHW()
{
    Device.seqAppActivate.Remove(this);
    Device.seqAppDeactivate.Remove(this);
}

#include <dxgi1_6.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void CHW::CreateD3D()
{
    // Минимально поддерживаемая версия Windows => Windows Vista SP2 или Windows 7.
    R_CHK(CreateDXGIFactory1(IID_PPV_ARGS(&m_pFactory)));

    
    UINT i = 0;
    while (m_pFactory->EnumAdapters1(i, &m_pAdapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC desc;
        m_pAdapter->GetDesc(&desc);

        Msg("* Avail GPU [vendor:%X]-[device:%X]: %S", desc.VendorId, desc.DeviceId, desc.Description);

        m_pAdapter->Release();
        m_pAdapter = nullptr;
        ++i;
    }

    // In the Windows 10 April 2018 Update, there is now a new IDXGIFactory6 interface that supports
    // a new EnumAdapterByGpuPreference method which lets you enumerate adapters by 'max performance' or 'minimum power'
    IDXGIFactory6* pFactory6 = nullptr;
    if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&pFactory6))))
    {
        pFactory6->EnumAdapterByGpuPreference(
            0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,IID_PPV_ARGS(&m_pAdapter));

        Msg(" !CHW::CreateD3D() use DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE");

        _RELEASE(pFactory6);

        b_modern = true;
    }
    else
    {
        Msg(" !CHW::CreateD3D() use EnumAdapters1(0)");

        m_pFactory->EnumAdapters1(0, &m_pAdapter);
    }

}

void CHW::DestroyD3D()
{
    _SHOW_REF("refCount:m_pAdapter", m_pAdapter);
    _RELEASE(m_pAdapter);

    _SHOW_REF("refCount:pFactory", m_pFactory);
    _RELEASE(m_pFactory);
}

void CHW::CreateSwapChain2(HWND hw)
{
    // Set up the presentation parameters
    DXGI_SWAP_CHAIN_DESC1 desc{};

    SelectResolution(desc.Width, desc.Height);

    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // DXGI_SWAP_EFFECT_FLIP_DISCARD or DXGI_SWAP_EFFECT_DISCARD
    desc.Scaling = DXGI_SCALING_NONE;

    desc.BufferCount = desc.SwapEffect == DXGI_SWAP_EFFECT_FLIP_DISCARD ? r_back_buffer_count : 1; // For DXGI_SWAP_EFFECT_FLIP_DISCARD we need at least two
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    // Multisample
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;

    // Additional setup
    desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreen_desc{};
    fullscreen_desc.Windowed = true;

    fullscreen_desc.RefreshRate.Numerator = 60;
    fullscreen_desc.RefreshRate.Denominator = 1;

    IDXGIFactory2* pFactory2{};
    m_pAdapter->GetParent(IID_PPV_ARGS(& pFactory2));

    IDXGISwapChain1* swapchain{};
    R_CHK(pFactory2->CreateSwapChainForHwnd(pDevice, hw, &desc, &fullscreen_desc, nullptr, &swapchain));

    _RELEASE(pFactory2);

    R_CHK(swapchain->GetDesc(&m_ChainDesc));
    R_CHK(swapchain->QueryInterface(IID_PPV_ARGS(&m_pSwapChain)));

    _RELEASE(swapchain);

    R_CHK(m_pSwapChain->SetMaximumFrameLatency(2));
    m_frameLatencyWaitableObject = m_pSwapChain->GetFrameLatencyWaitableObject();
}

void CHW::CreateDevice(HWND m_hWnd)
{
    CreateD3D();

    // Display the name of video board
    DXGI_ADAPTER_DESC1 Desc;
    R_CHK(m_pAdapter->GetDesc1(&Desc));

    DumpVideoMemoryUsage();

    //	Warning: Desc.Description is wide string
    Msg("* Selected GPU [vendor:%X]-[device:%X]: %S", Desc.VendorId, Desc.DeviceId, Desc.Description);

    Caps.id_vendor = Desc.VendorId;
    Caps.id_device = Desc.DeviceId;

    const bool UseDXDBG = !!strstr(Core.Params, "-dxdbg");

    UINT createDeviceFlags{};

    if (UseDXDBG)
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

    auto& pContext = d3d_contexts_pool[CHW::IMM_CTX_ID];

    constexpr D3D_FEATURE_LEVEL pFeatureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    R_CHK(D3D11CreateDevice(
        m_pAdapter
        , D3D_DRIVER_TYPE_UNKNOWN // Если мы выбираем конкретный адаптер, то мы обязаны использовать D3D_DRIVER_TYPE_UNKNOWN.
        , nullptr
        , createDeviceFlags
        , pFeatureLevels
        , std::size(pFeatureLevels)
        , D3D11_SDK_VERSION
        , &pDevice
        , &FeatureLevel
        , &pContext));

    R_ASSERT(FeatureLevel >= D3D_FEATURE_LEVEL_11_0); //На всякий случай

    for (int id = 0; id < R__NUM_PARALLEL_CONTEXTS; ++id)
    {
        const HRESULT R = pDevice->CreateDeferredContext(0, &d3d_contexts_pool[id]);
        R_ASSERT(SUCCEEDED(R));
    }

    CreateSwapChain2(m_hWnd);

    //// https://habr.com/ru/post/308980/
    //IDXGIDevice1* pDeviceDXGI = nullptr;
    //R_CHK(pDevice->QueryInterface(IID_PPV_ARGS(&pDeviceDXGI)));
    //R_CHK(pDeviceDXGI->SetMaximumFrameLatency(1));
    //_RELEASE(pDeviceDXGI);

    _SHOW_REF("* CREATE: DeviceREF:", HW.pDevice);

     // Register immediate context in profiler
    profiler_ctx = TracyD3D11Context(pDevice, pContext);

    UpdateWindowProps(m_hWnd);

    const size_t memory = Desc.DedicatedVideoMemory;
    Msg("*     Texture memory: %d M", memory / (1024 * 1024));

    fill_vid_mode_list(this);

    if (UseDXDBG)
    { // https://walbourn.github.io/direct3d-sdk-debug-layer-tricks/
        Microsoft::WRL::ComPtr<ID3D11Debug> d3dDebug;
        if (SUCCEEDED(pDevice->QueryInterface(IID_PPV_ARGS(&d3dDebug))))
        {
            Microsoft::WRL::ComPtr<ID3D11InfoQueue> d3dInfoQueue;
            if (SUCCEEDED(d3dDebug->QueryInterface(IID_PPV_ARGS(&d3dInfoQueue))))
            {
                if (IsDebuggerPresent())
                {
                    d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
                    // d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
                }

                // Add more message IDs here as needed
                D3D11_MESSAGE_ID hide[]{
                    D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
                };

                D3D11_INFO_QUEUE_FILTER filter{};
                filter.DenyList.NumIDs = std::size(hide);
                filter.DenyList.pIDList = hide;
                d3dInfoQueue->AddStorageFilterEntries(&filter);
            }
        }
    }

    ImGui_ImplDX11_Init(m_hWnd, pDevice, pContext);

    ImGui_ImplDX11_CreateDeviceObjects();
}

void CHW::DestroyDevice()
{
    ImGui_ImplDX11_Shutdown();

    //	Destroy state managers
    RSManager.ClearStateArray();
    DSSManager.ClearStateArray();
    BSManager.ClearStateArray();
    SSManager.ClearStateArray();

    _SHOW_REF("refCount:m_pSwapChain", m_pSwapChain);
    _RELEASE(m_pSwapChain);

    for (auto& id : d3d_contexts_pool)
    {
        _SHOW_REF("refCount:pContext", id);
        _RELEASE(id);
    }

    _SHOW_REF("DeviceREF:", HW.pDevice);
    _RELEASE(HW.pDevice);

    TracyD3D11Destroy(profiler_ctx);

    DestroyD3D();

    free_vid_mode_list();
}

//////////////////////////////////////////////////////////////////////
// Resetting device
//////////////////////////////////////////////////////////////////////
void CHW::ResetDevice(HWND m_hWnd)
{
    ImGui_ImplDX11_InvalidateDeviceObjects();

    DXGI_SWAP_CHAIN_DESC& cd = m_ChainDesc;

    DXGI_MODE_DESC& desc = cd.BufferDesc;

    SelectResolution(desc.Width, desc.Height);

    CHK_DX(m_pSwapChain->ResizeTarget(&desc));
    CHK_DX(m_pSwapChain->ResizeBuffers(cd.BufferCount, desc.Width, desc.Height, desc.Format
        , DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT));

    UpdateWindowProps(m_hWnd);

    Device.ShowMainWindow();

    ImGui_ImplDX11_CreateDeviceObjects();
}

bool CHW::CheckFormatSupport(const DXGI_FORMAT format, const u32 feature) const
{
    u32 supports;

    if (SUCCEEDED(pDevice->CheckFormatSupport(format, &supports)))
    {
        if (supports & feature)
            return true;
    }

    return false;
}

void CHW::SelectResolution(u32& dwWidth, u32& dwHeight)
{
    fill_vid_mode_list(this);

    {
        dwWidth = psCurrentVidMode[0];
        dwHeight = psCurrentVidMode[1];
    }
}

void CHW::OnAppActivate()
{
    //const HWND insertPos = IsDebuggerPresent() ? HWND_NOTOPMOST : HWND_TOPMOST;

    //SetWindowPos(Device.m_hWnd, insertPos, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void CHW::OnAppDeactivate()
{
    //SetWindowPos(Device.m_hWnd, HWND_NOTOPMOST , 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

bool CHW::ThisInstanceIsGlobal() const
{
    return this == &HW;
}

void CHW::WaitOnSwapChain() const { WaitForSingleObjectEx(m_frameLatencyWaitableObject, INFINITE, true); }

void CHW::DumpVideoMemoryUsage() const
{
    if (b_modern)
    {
        DXGI_ADAPTER_DESC1 Desc;
        R_CHK(m_pAdapter->GetDesc1(&Desc));

        IDXGIAdapter3* adapter3;
        DXGI_QUERY_VIDEO_MEMORY_INFO videoMemoryInfo;

        if (SUCCEEDED(m_pAdapter->QueryInterface(&adapter3)) 
            && SUCCEEDED(adapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &videoMemoryInfo)))
        {
            Msg("\n\tDedicated VRAM: %zu MB (%zu bytes)\n\tDedicated Memory: %zu MB (%zu bytes)\n\tShared Memory: %zu MB (%zu bytes)\n\tCurrentUsage: %zu MB (%zu bytes)\n\tBudget: %zu MB (%zu bytes)",
                Desc.DedicatedVideoMemory / 1024 / 1024, Desc.DedicatedVideoMemory, 
                Desc.DedicatedSystemMemory / 1024 / 1024, Desc.DedicatedSystemMemory,
                Desc.SharedSystemMemory / 1024 / 1024, Desc.SharedSystemMemory,
                videoMemoryInfo.CurrentUsage / 1024 / 1024, videoMemoryInfo.CurrentUsage,
                videoMemoryInfo.Budget/ 1024 / 1024, videoMemoryInfo.Budget
            );
        }
        else
        {
            Msg("\n\tDedicated VRAM: %zu MB (%zu bytes)\n\tDedicated Memory: %zu MB (%zu bytes)\n\tShared Memory: %zu MB (%zu bytes)",
                Desc.DedicatedVideoMemory / 1024 / 1024, Desc.DedicatedVideoMemory, 
                Desc.DedicatedSystemMemory / 1024 / 1024, Desc.DedicatedSystemMemory,
                Desc.SharedSystemMemory / 1024 / 1024, Desc.SharedSystemMemory
            );            
        }

        _RELEASE(adapter3);
    }
}

void CHW::UpdateWindowProps(HWND m_hWnd) const
{
    LONG_PTR dwWindowStyle = 0;

    // Set window properties depending on what mode were in.
    static const bool bBordersMode = !!strstr(Core.Params, "-draw_borders");
    //dwWindowStyle = WS_VISIBLE;
    if (bBordersMode)
        dwWindowStyle |= WS_BORDER | WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX;

    SetWindowLongPtr(m_hWnd, GWL_STYLE, dwWindowStyle);

    // When moving from fullscreen to windowed mode, it is important to
    // adjust the window size after recreating the device rather than
    // beforehand to ensure that you get the window size you want.  For
    // example, when switching from 640x480 fullscreen to windowed with
    // a 1000x600 window on a 1024x768 desktop, it is impossible to set
    // the window size to 1000x600 until after the display mode has
    // changed to 1024x768, because windows cannot be larger than the
    // desktop.

    RECT m_rcWindowBounds;
    int fYOffset = 0;

    auto& dxgi_mode_desc = m_ChainDesc.BufferDesc;

    static const bool bCenter = !!strstr(Core.Params, "-center_screen");

    if (bCenter)
    {
        RECT DesktopRect;

        GetClientRect(GetDesktopWindow(), &DesktopRect);

        SetRect(&m_rcWindowBounds, 
            (DesktopRect.right - dxgi_mode_desc.Width) / 2, 
            (DesktopRect.bottom - dxgi_mode_desc.Height) / 2,
            (DesktopRect.right + dxgi_mode_desc.Width) / 2, 
            (DesktopRect.bottom + dxgi_mode_desc.Height) / 2);
    }
    else
    {
        if (bBordersMode)
            fYOffset = GetSystemMetrics(SM_CYCAPTION); // size of the window title bar

        SetRect(&m_rcWindowBounds, 0, 0, dxgi_mode_desc.Width, dxgi_mode_desc.Height);
    }

    if (bBordersMode)
        AdjustWindowRect(&m_rcWindowBounds, DWORD(dwWindowStyle), FALSE);

    SetWindowPos(m_hWnd, HWND_NOTOPMOST
        , m_rcWindowBounds.left
        , m_rcWindowBounds.top + fYOffset
        , (m_rcWindowBounds.right - m_rcWindowBounds.left)
        , (m_rcWindowBounds.bottom - m_rcWindowBounds.top)
        
        , /*SWP_SHOWWINDOW | */SWP_NOCOPYBITS | SWP_DRAWFRAME);
}

struct _uniq_mode
{
    _uniq_mode(LPCSTR v) : _val(v) {}
    LPCSTR _val;
    bool operator()(LPCSTR _other) const { return !stricmp(_val, _other); }
};

void free_vid_mode_list()
{
    for (int i = 0; vid_mode_token[i].name; i++)
    {
        xr_free(vid_mode_token[i].name);
    }
    xr_free(vid_mode_token);
    vid_mode_token = nullptr;
}

void fill_vid_mode_list(CHW* _hw)
{
    if (vid_mode_token != nullptr)
        return;

    xr_vector<LPCSTR> _tmp;
    xr_vector<DXGI_MODE_DESC> modes;

    IDXGIOutput* pOutput;
    _hw->m_pAdapter->EnumOutputs(0, &pOutput);
    VERIFY(pOutput);

    UINT num = 0;
    DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
    UINT flags = 0;

    // Get the number of display modes available
    pOutput->GetDisplayModeList(format, flags, &num, nullptr);

    // Get the list of display modes
    modes.resize(num);
    pOutput->GetDisplayModeList(format, flags, &num, &modes.front());

    _RELEASE(pOutput);

    for (u32 i = 0; i < num; ++i)
    {
        const DXGI_MODE_DESC& desc = modes[i];

        string32 str;

        if (desc.Width < 800)
            continue;

        xr_sprintf(str, sizeof(str), "%dx%d", desc.Width, desc.Height);

        if (_tmp.end() != std::find_if(_tmp.begin(), _tmp.end(), _uniq_mode(str)))
            continue;

        _tmp.push_back(nullptr);
        _tmp.back() = xr_strdup(str);
    }

    u32 _cnt = _tmp.size() + 1;

    vid_mode_token = xr_alloc<xr_token>(_cnt);

    vid_mode_token[_cnt - 1].id = -1;
    vid_mode_token[_cnt - 1].name = nullptr;

#ifdef DEBUG
    Msg("Available video modes[%d]:", _tmp.size());
#endif // DEBUG
    for (u32 i = 0; i < _tmp.size(); ++i)
    {
        vid_mode_token[i].id = i;
        vid_mode_token[i].name = _tmp[i];
#ifdef DEBUG
        Msg("[%s]", _tmp[i]);
#endif // DEBUG
    }
}

void LogD3D11DebugMessages()
{
    if (!HW.pDevice || !strstr(Core.Params, "-dxdbg") || IsDebuggerPresent())
        return;

    static std::recursive_mutex logCS;
    std::scoped_lock lock(logCS); // на всякий случай

    Microsoft::WRL::ComPtr<ID3D11InfoQueue> infoQueue;
    if (SUCCEEDED(HW.pDevice->QueryInterface(IID_PPV_ARGS(&infoQueue))) && infoQueue)
    {
        const UINT64 numMessages = infoQueue->GetNumStoredMessages();
        for (UINT64 i{}; i < numMessages; ++i)
        {
            SIZE_T messageLength{};
            infoQueue->GetMessage(i, nullptr, &messageLength); // Получаем размер

            xr_vector<char> messageData(messageLength);
            auto message = reinterpret_cast<D3D11_MESSAGE*>(messageData.data());

            if (SUCCEEDED(infoQueue->GetMessage(i, message, &messageLength)))
            { // Теперь у нас есть текст сообщения
                Log("---------[" __FUNCTION__ "]------------");
                Log(std::string{message->pDescription, message->DescriptionByteLength});
                Log("----------------------------------------");
            }
            else
            {
                Msg("!![%s] Failed infoQueue->GetMessage", __FUNCTION__);
            }
        }

        infoQueue->ClearStoredMessages(); // Очистить очередь сообщений
    }
    else
    {
        Msg("!![%s] Failed infoQueue", __FUNCTION__);
    }
}

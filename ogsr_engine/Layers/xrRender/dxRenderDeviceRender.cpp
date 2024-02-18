#include "stdafx.h"
#include "dxRenderDeviceRender.h"
#include "ResourceManager.h"

#define USE_RENDERDOC

#ifdef USE_RENDERDOC
#include <../RenderDoc/renderdoc_app.h>
RENDERDOC_API_1_0_0* g_renderdoc_api{};
#endif

dxRenderDeviceRender::dxRenderDeviceRender() : Resources(0) { ; }

void dxRenderDeviceRender::Copy(IRenderDeviceRender& _in) { *this = *(dxRenderDeviceRender*)&_in; }

void dxRenderDeviceRender::setGamma(float fGamma) { m_Gamma.Gamma(fGamma); }

void dxRenderDeviceRender::setBrightness(float fGamma) { m_Gamma.Brightness(fGamma); }

void dxRenderDeviceRender::setContrast(float fGamma) { m_Gamma.Contrast(fGamma); }

void dxRenderDeviceRender::updateGamma() { m_Gamma.Update(); }

void dxRenderDeviceRender::OnDeviceDestroy(BOOL bKeepTextures)
{
    m_WireShader.destroy();
    m_SelectionShader.destroy();

    Resources->OnDeviceDestroy(bKeepTextures);
    RCache.OnDeviceDestroy();
}

void dxRenderDeviceRender::DestroyHW()
{
    xr_delete(Resources);
    HW.DestroyDevice();
}

void dxRenderDeviceRender::Reset(HWND hWnd, u32& dwWidth, u32& dwHeight, float& fWidth_2, float& fHeight_2)
{
#ifdef DEBUG
    _SHOW_REF("*ref -CRenderDevice::ResetTotal: DeviceREF:", HW.pDevice);
#endif // DEBUG

    Resources->reset_begin();
    Memory.mem_compact();
    HW.Reset(hWnd);

    dwWidth = HW.m_ChainDesc.BufferDesc.Width;
    dwHeight = HW.m_ChainDesc.BufferDesc.Height;

    fWidth_2 = float(dwWidth / 2);
    fHeight_2 = float(dwHeight / 2);
    Resources->reset_end();

#ifdef DEBUG
    _SHOW_REF("*ref +CRenderDevice::ResetTotal: DeviceREF:", HW.pDevice);
#endif // DEBUG
}

void dxRenderDeviceRender::SetupStates()
{
    HW.Caps.Update();

    //	TODO: DX10: Implement Resetting of render states into default mode
    // VERIFY(!"dxRenderDeviceRender::SetupStates not implemented.");
    SSManager.SetMaxAnisotropy(ps_r__tf_Anisotropic);
    SSManager.SetMipLODBias(ps_r__tf_Mipbias);
}

void dxRenderDeviceRender::OnDeviceCreate(LPCSTR shName)
{
    // Signal everyone - device created
    RCache.OnDeviceCreate();
    m_Gamma.Update();
    Resources->OnDeviceCreate();
    ::Render->create();
    Device.Statistic->OnDeviceCreate();

    {
        m_WireShader.create("editor\\wire");
        m_SelectionShader.create("editor\\selection");

        DUImpl.OnDeviceCreate();
    }
}

void dxRenderDeviceRender::Create(HWND hWnd, u32& dwWidth, u32& dwHeight, float& fWidth_2, float& fHeight_2)
{
#ifdef USE_RENDERDOC
    if (!g_renderdoc_api)
    {
        static HMODULE hModule = GetModuleHandle("renderdoc.dll");
        if (!hModule)
            hModule = LoadLibrary("renderdoc.dll");

        if (hModule)
        {
            const auto RENDERDOC_GetAPI = reinterpret_cast<pRENDERDOC_GetAPI>(GetProcAddress(hModule, "RENDERDOC_GetAPI"));
            const auto Result = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_0_0, reinterpret_cast<void**>(&g_renderdoc_api));

            if (Result == 1)
            {
                g_renderdoc_api->UnloadCrashHandler();

                string_path FolderName{};
                FS.update_path(FolderName, "$app_data_root$", "renderdoc_captures\\");
                VerifyPath(FolderName);
                g_renderdoc_api->SetCaptureFilePathTemplate(FolderName);
                Msg("~~[%s] RenderDoc folder: [%s]", __FUNCTION__, FolderName);

                RENDERDOC_InputButton CaptureButton[] = {eRENDERDOC_Key_Home};
                g_renderdoc_api->SetCaptureKeys(CaptureButton, std::size(CaptureButton));
                g_renderdoc_api->SetCaptureOptionU32(eRENDERDOC_Option_AllowVSync, 0);
                g_renderdoc_api->SetCaptureOptionU32(eRENDERDOC_Option_DebugOutputMute, 0);

                g_renderdoc_api->SetCaptureOptionU32(eRENDERDOC_Option_RefAllResources, 1);
                g_renderdoc_api->SetCaptureOptionU32(eRENDERDOC_Option_CaptureCallstacks, 1);
                g_renderdoc_api->SetCaptureOptionU32(eRENDERDOC_Option_VerifyBufferAccess, 1);
                g_renderdoc_api->SetCaptureOptionU32(eRENDERDOC_Option_APIValidation, 1);
                g_renderdoc_api->SetCaptureOptionU32(eRENDERDOC_Option_CaptureAllCmdLists, 1);
            }
        }
    }
#endif

    HW.CreateDevice(hWnd);

    dwWidth = HW.m_ChainDesc.BufferDesc.Width;
    dwHeight = HW.m_ChainDesc.BufferDesc.Height;

    fWidth_2 = float(dwWidth / 2);
    fHeight_2 = float(dwHeight / 2);

    Resources = xr_new<CResourceManager>();
}

void dxRenderDeviceRender::SetupGPU(BOOL bForceGPU_SW, BOOL bForceGPU_NonPure, BOOL bForceGPU_REF)
{
    HW.Caps.bForceGPU_SW = bForceGPU_SW;
    HW.Caps.bForceGPU_NonPure = bForceGPU_NonPure;
    HW.Caps.bForceGPU_REF = bForceGPU_REF;
}

void dxRenderDeviceRender::overdrawBegin()
{
    //	TODO: DX10: Implement overdrawBegin
    VERIFY(!"dxRenderDeviceRender::overdrawBegin not implemented.");
}

void dxRenderDeviceRender::overdrawEnd()
{
    //	TODO: DX10: Implement overdrawEnd
    VERIFY(!"dxRenderDeviceRender::overdrawBegin not implemented.");
}

void dxRenderDeviceRender::DeferredLoad(BOOL E) { Resources->DeferredLoad(E); }

void dxRenderDeviceRender::ResourcesDeferredUpload() { Resources->DeferredUpload(); }

void dxRenderDeviceRender::ResourcesGetMemoryUsage(u32& m_base, u32& c_base, u32& m_lmaps, u32& c_lmaps)
{
    if (Resources)
        Resources->_GetMemoryUsage(m_base, c_base, m_lmaps, c_lmaps);
}

void dxRenderDeviceRender::ResourcesDumpMemoryUsage() { Resources->_DumpMemoryUsage(); }

dxRenderDeviceRender::DeviceState dxRenderDeviceRender::GetDeviceState()
{
    if (HW.doPresentTest)
    {
        switch (HW.m_pSwapChain->Present(0, DXGI_PRESENT_TEST))
        {
        case S_OK: HW.doPresentTest = false; break;

        case DXGI_STATUS_OCCLUDED:
            // Do not render until we become visible again
            return DeviceState::dsLost;

        case DXGI_ERROR_DEVICE_RESET: return DeviceState::dsNeedReset;

        case DXGI_ERROR_DEVICE_REMOVED:
            FATAL(
                "Graphics driver was updated or GPU was physically removed from computer.\n"
                "Please, restart the game.");
            break;
        }
    }

    return DeviceState::dsOK;
}

BOOL dxRenderDeviceRender::GetForceGPU_REF() { return HW.Caps.bForceGPU_REF; }

u32 dxRenderDeviceRender::GetCacheStatPolys() { return RCache.stat.polys; }

void dxRenderDeviceRender::Begin()
{
    RCache.OnFrameBegin();
    RCache.set_CullMode(CULL_CW);
    RCache.set_CullMode(CULL_CCW);
    if (HW.Caps.SceneMode)
        overdrawBegin();
}

void dxRenderDeviceRender::Clear()
{
    HW.pContext->ClearDepthStencilView(RCache.get_ZB(), D3D_CLEAR_DEPTH | D3D_CLEAR_STENCIL, 1.0f, 0);

    if (psDeviceFlags.test(rsClearBB))
    {
        FLOAT ColorRGBA[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        HW.pContext->ClearRenderTargetView(RCache.get_RT(), ColorRGBA);
    }
}

void dxRenderDeviceRender::End()
{
    VERIFY(HW.pDevice);

    if (HW.Caps.SceneMode)
        overdrawEnd();

    RCache.OnFrameEnd();

    bool bUseVSync = psDeviceFlags.is(rsFullscreen) && psDeviceFlags.test(rsVSync); // xxx: weird tearing glitches when VSync turned on for windowed mode in DX10\11
    if (!Device.m_SecondViewport.IsSVPFrame() && !Device.m_SecondViewport.m_bCamReady)
    { //--#SM+#-- +SecondVP+ Не выводим кадр из второго вьюпорта на экран (на практике у нас экранная картинка обновляется минимум в два
      // раза реже) [don't flush image into display for SecondVP-frame]
        switch (HW.m_pSwapChain->Present(bUseVSync ? 1 : 0, 0))
        {
        case DXGI_STATUS_OCCLUDED:
        case DXGI_ERROR_DEVICE_REMOVED: HW.doPresentTest = true; break;
        }
    }
}

void dxRenderDeviceRender::ClearTarget()
{
    FLOAT ColorRGBA[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    HW.pContext->ClearRenderTargetView(RCache.get_RT(), ColorRGBA);
}

void dxRenderDeviceRender::SetCacheXform(Fmatrix& mView, Fmatrix& mProject)
{
    RCache.set_xform_view(mView);
    RCache.set_xform_project(mProject);
}

bool dxRenderDeviceRender::HWSupportsShaderYUV2RGB()
{
    u32 v_dev = CAP_VERSION(HW.Caps.raster_major, HW.Caps.raster_minor);
    u32 v_need = CAP_VERSION(2, 0);
    return (v_dev >= v_need);
}

void dxRenderDeviceRender::OnAssetsChanged()
{
    Resources->m_textures_description.UnLoad();
    Resources->m_textures_description.Load();
}

IResourceManager* dxRenderDeviceRender::GetResourceManager() const { return dynamic_cast<IResourceManager*>(Resources); }

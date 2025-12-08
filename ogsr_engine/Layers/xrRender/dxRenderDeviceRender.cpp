#include "stdafx.h"

#include "dxRenderDeviceRender.h"
#include "ResourceManager.h"

#define USE_RENDERDOC

#ifdef USE_RENDERDOC
#include <../RenderDoc/renderdoc_app.h>
RENDERDOC_API_1_0_0* g_renderdoc_api{};
#endif

dxRenderDeviceRender::dxRenderDeviceRender() : Resources(nullptr) { ; }

void dxRenderDeviceRender::Copy(IRenderDeviceRender& _in) { *this = *smart_cast<dxRenderDeviceRender*>(&_in); }

void dxRenderDeviceRender::OnDeviceCreate()
{
    // Signal everyone - device created
    RImplementation.CreateQuadIB();

    // streams
    RImplementation.Vertex.Create();
    RImplementation.Index.Create();

    RImplementation.OnDeviceCreate();

    Resources->OnDeviceCreate();
    RImplementation.create();
    Device.Statistic->OnDeviceCreate();

    {
        m_WireShader.create("editor\\wire");
        m_SelectionShader.create("editor\\selection");

        m_PortalFadeShader.create("portal");
        m_PortalFadeGeom.create(FVF::F_L, RImplementation.Vertex.Buffer(), nullptr);

        DUImpl.OnDeviceCreate();
    }
}

void dxRenderDeviceRender::OnDeviceDestroy(BOOL bKeepTextures)
{
    m_WireShader.destroy();
    m_SelectionShader.destroy();

    m_PortalFadeGeom.destroy();
    m_PortalFadeShader.destroy();

    Resources->OnDeviceDestroy(bKeepTextures);

    RImplementation.OnDeviceDestroy();

    // streams
    RImplementation.Index.Destroy();
    RImplementation.Vertex.Destroy();

    // Quad
    HW.stats_manager.decrement_stats_ib(RImplementation.QuadIB);
    _RELEASE(RImplementation.QuadIB);
}

void dxRenderDeviceRender::Destroy()
{
    xr_delete(Resources);
    HW.DestroyDevice();
}

void dxRenderDeviceRender::Reset(HWND hWnd, u32& dwWidth, u32& dwHeight, float& fWidth_2, float& fHeight_2)
{
#ifdef DEBUG
    _SHOW_REF("*ref -CRenderDevice::ResetTotal: DeviceREF:", HW.pDevice);
#endif // DEBUG

    RImplementation.HWOCC.cleanup_lost(true);

    Resources->reset_begin();
    HW.ResetDevice(hWnd);

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

void dxRenderDeviceRender::Create(HWND hWnd, u32& dwWidth, u32& dwHeight, float& fWidth_2, float& fHeight_2)
{
    if (strstr(Core.Params, "-shadersdbg"))
    {
        ps_r2_ls_flags_ext.set(R2FLAGEXT_SHADER_DBG, TRUE);
    }

#ifdef USE_RENDERDOC
    if (!g_renderdoc_api)
    {
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
                    FS.update_path(FolderName, fsgame::app_data_root, "renderdoc_captures\\");
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
    }
#endif

    HW.CreateDevice(hWnd);

    dwWidth = HW.m_ChainDesc.BufferDesc.Width;
    dwHeight = HW.m_ChainDesc.BufferDesc.Height;

    fWidth_2 = float(dwWidth / 2);
    fHeight_2 = float(dwHeight / 2);

    Resources = xr_new<CResourceManager>();
}

void dxRenderDeviceRender::DeferredLoad(BOOL E) { Resources->DeferredLoad(E); }

void dxRenderDeviceRender::ResourcesDeferredUpload() { Resources->DeferredUpload(); }

void dxRenderDeviceRender::ResourcesPrefetchCreateTexture(LPCSTR name)
{
    const auto t = Resources->_CreateTexture(name);
    ++t->ref_count; // to avoid unload
}

void dxRenderDeviceRender::ResourcesGetMemoryUsage(u32& m_base, u32& c_base, u32& m_lmaps, u32& c_lmaps)
{
    if (Resources)
        Resources->_GetMemoryUsage(m_base, c_base, m_lmaps, c_lmaps);
}

void dxRenderDeviceRender::ResourcesDumpMemoryUsage() { Resources->_DumpMemoryUsage(); }

dxRenderDeviceRender::DeviceState dxRenderDeviceRender::GetDeviceState()
{
    //const auto result = HW.m_pSwapChain->Present(0, DXGI_PRESENT_TEST);

    //switch (result)
    //{
    //// Check if the device is ready to be reset
    //case DXGI_ERROR_DEVICE_RESET: return dsNeedReset;
    //}

    return dsOK;
}

void dxRenderDeviceRender::OnAssetsChanged()
{
    Resources->m_textures_description.UnLoad();
    Resources->m_textures_description.Load();
}

IResourceManager* dxRenderDeviceRender::GetResourceManager() const { return Resources; }

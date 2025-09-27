#pragma once

#define DEV dxRenderDeviceRender::Instance().Resources

#include "../../Include/xrRender/RenderDeviceRender.h"

class CResourceManager;

class dxRenderDeviceRender : public IRenderDeviceRender
{
public:
    static dxRenderDeviceRender& Instance() { return *smart_cast<dxRenderDeviceRender*>(Device.m_pRender); }

    dxRenderDeviceRender();

    virtual void Copy(IRenderDeviceRender& _in);

    //	Destroy
    virtual void OnDeviceDestroy(BOOL bKeepTextures);
    virtual void Destroy();
    virtual void Reset(HWND hWnd, u32& dwWidth, u32& dwHeight, float& fWidth_2, float& fHeight_2);
    //	Init
    virtual void SetupStates();
    virtual void OnDeviceCreate();
    virtual void Create(HWND hWnd, u32& dwWidth, u32& dwHeight, float& fWidth_2, float& fHeight_2);

    //	Resources control
    virtual void DeferredLoad(BOOL E);
    virtual void ResourcesDeferredUpload();
    virtual void ResourcesGetMemoryUsage(u32& m_base, u32& c_base, u32& m_lmaps, u32& c_lmaps);
    virtual void ResourcesDumpMemoryUsage();
    virtual void ResourcesPrefetchCreateTexture(LPCSTR name);

    //	Device state
    virtual DeviceState GetDeviceState();

    virtual void OnAssetsChanged();

    IResourceManager* GetResourceManager() const override;

public:
    CResourceManager* Resources;

    ref_shader m_WireShader;
    ref_shader m_SelectionShader;

    ref_shader m_PortalFadeShader;
    ref_geom m_PortalFadeGeom;
};

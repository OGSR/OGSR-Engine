#pragma once

class IResourceManager;

class IRenderDeviceRender
{
public:
    enum DeviceState
    {
        dsOK = 0,
        dsLost,
        dsNeedReset
    };

public:
    virtual ~IRenderDeviceRender() { ; }
    virtual void Copy(IRenderDeviceRender& _in) = 0;

    //	Destroy
    virtual void OnDeviceDestroy(BOOL bKeepTextures) = 0;
    virtual void Destroy() = 0;
    virtual void Reset(HWND hWnd, u32& dwWidth, u32& dwHeight, float& fWidth_2, float& fHeight_2) = 0;
    //	Init
    virtual void SetupStates() = 0;
    virtual void OnDeviceCreate() = 0;
    virtual void Create(HWND hWnd, u32& dwWidth, u32& dwHeight, float& fWidth_2, float& fHeight_2) = 0;

    //	Resources control
    virtual void DeferredLoad(BOOL E) = 0;
    virtual void ResourcesDeferredUpload() = 0;
    virtual void ResourcesGetMemoryUsage(u32& m_base, u32& c_base, u32& m_lmaps, u32& c_lmaps) = 0;
    virtual void ResourcesDumpMemoryUsage() = 0;
    virtual void ResourcesPrefetchCreateTexture(LPCSTR name) = 0;

    //	Device state
    virtual DeviceState GetDeviceState() = 0;

    virtual void OnAssetsChanged() = 0;
    virtual IResourceManager* GetResourceManager() const = 0;
};

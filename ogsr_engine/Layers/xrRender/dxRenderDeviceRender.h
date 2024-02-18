#pragma once

#define DEV dxRenderDeviceRender::Instance().Resources

#include "..\..\Include\xrRender\RenderDeviceRender.h"
#include "xr_effgamma.h"

class CResourceManager;

class dxRenderDeviceRender : public IRenderDeviceRender
{
public:
    static dxRenderDeviceRender& Instance() { return *((dxRenderDeviceRender*)(&*Device.m_pRender)); }

    dxRenderDeviceRender();

    virtual void Copy(IRenderDeviceRender& _in);

    //	Gamma correction functions
    virtual void setGamma(float fGamma);
    virtual void setBrightness(float fGamma);
    virtual void setContrast(float fGamma);
    virtual void updateGamma();

    // void	gammaGenLUT(D3DGAMMARAMP &G) {m_Gamma.GenLUT(G);}

    //	Destroy
    virtual void OnDeviceDestroy(BOOL bKeepTextures);
    virtual void DestroyHW();
    virtual void Reset(HWND hWnd, u32& dwWidth, u32& dwHeight, float& fWidth_2, float& fHeight_2);
    //	Init
    virtual void SetupStates();
    virtual void OnDeviceCreate(LPCSTR shName);
    virtual void Create(HWND hWnd, u32& dwWidth, u32& dwHeight, float& fWidth_2, float& fHeight_2) override;
    virtual void SetupGPU(BOOL bForceGPU_SW, BOOL bForceGPU_NonPure, BOOL bForceGPU_REF);
    //	Overdraw
    virtual void overdrawBegin();
    virtual void overdrawEnd();

    //	Resources control
    virtual void DeferredLoad(BOOL E);
    virtual void ResourcesDeferredUpload();
    virtual void ResourcesGetMemoryUsage(u32& m_base, u32& c_base, u32& m_lmaps, u32& c_lmaps);
    virtual void ResourcesDumpMemoryUsage();

    //	HWSupport
    virtual bool HWSupportsShaderYUV2RGB();

    //	Device state
    virtual DeviceState GetDeviceState();
    virtual BOOL GetForceGPU_REF();
    virtual u32 GetCacheStatPolys();
    virtual void Begin();
    virtual void Clear();
    virtual void End();
    virtual void ClearTarget();
    virtual void SetCacheXform(Fmatrix& mView, Fmatrix& mProject);
    virtual void OnAssetsChanged();

    IResourceManager* GetResourceManager() const override;

public:
    CResourceManager* Resources;

    ref_shader m_WireShader;
    ref_shader m_SelectionShader;

private:
    CGammaControl m_Gamma;
};

#pragma once

#include "../../Include/xrRender/RenderFactory.h"

class dxRenderFactory : public IRenderFactory
{
    virtual IUISequenceVideoItem* CreateUISequenceVideoItem();
    virtual void DestroyUISequenceVideoItem(IUISequenceVideoItem* pObject);
    virtual IUIShader* CreateUIShader();
    virtual void DestroyUIShader(IUIShader* pObject);
    virtual IRenderDeviceRender* CreateRenderDeviceRender();
    virtual void DestroyRenderDeviceRender(IRenderDeviceRender* pObject);
    virtual IWallMarkArray* CreateWallMarkArray();
    virtual void DestroyWallMarkArray(IWallMarkArray* pObject);
    virtual IStatsRender* CreateStatsRender();
    virtual void DestroyStatsRender(IStatsRender* pObject);

    virtual IFlareRender* CreateFlareRender();
    virtual void DestroyFlareRender(IFlareRender* pObject);
    virtual IThunderboltRender* CreateThunderboltRender();
    virtual void DestroyThunderboltRender(IThunderboltRender* pObject);
    virtual IThunderboltDescRender* CreateThunderboltDescRender();
    virtual void DestroyThunderboltDescRender(IThunderboltDescRender* pObject);
    virtual IRainRender* CreateRainRender();
    virtual void DestroyRainRender(IRainRender* pObject);
    virtual ILensFlareRender* CreateLensFlareRender();
    virtual void DestroyLensFlareRender(ILensFlareRender* pObject);
    virtual IEnvironmentRender* CreateEnvironmentRender();
    virtual void DestroyEnvironmentRender(IEnvironmentRender* pObject);
    virtual IEnvDescriptorMixerRender* CreateEnvDescriptorMixerRender();
    virtual void DestroyEnvDescriptorMixerRender(IEnvDescriptorMixerRender* pObject);
    virtual IEnvDescriptorRender* CreateEnvDescriptorRender();
    virtual void DestroyEnvDescriptorRender(IEnvDescriptorRender* pObject);
    virtual IFontRender* CreateFontRender();
    virtual void DestroyFontRender(IFontRender* pObject);
};

extern dxRenderFactory RenderFactoryImpl;

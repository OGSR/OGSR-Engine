#pragma once

class IWallMarkArray;
class IFontRender;
class IEnvDescriptorRender;
class IEnvDescriptorMixerRender;
class IFlareRender;
class ILensFlareRender;
class IRainRender;
class IThunderboltRender;
class IEnvironmentRender;
class IStatsRender;
class IRenderDeviceRender;
class IThunderboltDescRender;
class IStatGraphRender;
class IUIShader;
class IUISequenceVideoItem;

class IRenderFactory
{
public:
    virtual IUISequenceVideoItem* CreateUISequenceVideoItem() = 0;
    virtual void DestroyUISequenceVideoItem(IUISequenceVideoItem* pObject) = 0;
    virtual IUIShader* CreateUIShader() = 0;
    virtual void DestroyUIShader(IUIShader* pObject) = 0;
    virtual IRenderDeviceRender* CreateRenderDeviceRender() = 0;
    virtual void DestroyRenderDeviceRender(IRenderDeviceRender* pObject) = 0;
    virtual IWallMarkArray* CreateWallMarkArray() = 0;
    virtual void DestroyWallMarkArray(IWallMarkArray* pObject) = 0;
    virtual IStatsRender* CreateStatsRender() = 0;
    virtual void DestroyStatsRender(IStatsRender* pObject) = 0;

    virtual IEnvironmentRender* CreateEnvironmentRender() = 0;
    virtual void DestroyEnvironmentRender(IEnvironmentRender* pObject) = 0;
    virtual IEnvDescriptorMixerRender* CreateEnvDescriptorMixerRender() = 0;
    virtual void DestroyEnvDescriptorMixerRender(IEnvDescriptorMixerRender* pObject) = 0;
    virtual IEnvDescriptorRender* CreateEnvDescriptorRender() = 0;
    virtual void DestroyEnvDescriptorRender(IEnvDescriptorRender* pObject) = 0;
    virtual IRainRender* CreateRainRender() = 0;
    virtual void DestroyRainRender(IRainRender* pObject) = 0;
    virtual ILensFlareRender* CreateLensFlareRender() = 0;
    virtual void DestroyLensFlareRender(ILensFlareRender* pObject) = 0;
    virtual IThunderboltRender* CreateThunderboltRender() = 0;
    virtual void DestroyThunderboltRender(IThunderboltRender* pObject) = 0;
    virtual IThunderboltDescRender* CreateThunderboltDescRender() = 0;
    virtual void DestroyThunderboltDescRender(IThunderboltDescRender* pObject) = 0;
    virtual IFlareRender* CreateFlareRender() = 0;
    virtual void DestroyFlareRender(IFlareRender* pObject) = 0;

    virtual IFontRender* CreateFontRender() = 0;
    virtual void DestroyFontRender(IFontRender* pObject) = 0;
};

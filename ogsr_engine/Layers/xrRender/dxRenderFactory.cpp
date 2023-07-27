#include "stdafx.h"
#include "dxRenderFactory.h"

#include "dxStatGraphRender.h"
#include "dxLensFlareRender.h"
#include "dxConsoleRender.h"
#include "dxThunderboltRender.h"
#include "dxThunderboltDescRender.h"
#include "dxRainRender.h"
#include "dxLensFlareRender.h"
#include "dxEnvironmentRender.h"

#include "dxRenderDeviceRender.h"
#include "dxObjectSpaceRender.h"

#include "dxFontRender.h"
#include "dxWallMarkArray.h"
#include "dxStatsRender.h"
#include "dxUISequenceVideoItem.h"
#include "dxUIShader.h"

dxRenderFactory RenderFactoryImpl;

IUISequenceVideoItem* dxRenderFactory::CreateUISequenceVideoItem() { return xr_new<dxUISequenceVideoItem>(); }
void dxRenderFactory::DestroyUISequenceVideoItem(IUISequenceVideoItem* pObject) { xr_delete((dxUISequenceVideoItem*&)pObject); }
IUIShader* dxRenderFactory::CreateUIShader() { return xr_new<dxUIShader>(); }
void dxRenderFactory::DestroyUIShader(IUIShader* pObject) { xr_delete((dxUIShader*&)pObject); }
IStatGraphRender* dxRenderFactory::CreateStatGraphRender() { return xr_new<dxStatGraphRender>(); }
void dxRenderFactory::DestroyStatGraphRender(IStatGraphRender* pObject) { xr_delete((dxStatGraphRender*&)pObject); }
IConsoleRender* dxRenderFactory::CreateConsoleRender() { return xr_new<dxConsoleRender>(); }
void dxRenderFactory::DestroyConsoleRender(IConsoleRender* pObject) { xr_delete((dxConsoleRender*&)pObject); }
IRenderDeviceRender* dxRenderFactory::CreateRenderDeviceRender() { return xr_new<dxRenderDeviceRender>(); }
void dxRenderFactory::DestroyRenderDeviceRender(IRenderDeviceRender* pObject) { xr_delete((dxRenderDeviceRender*&)pObject); }
#ifdef DEBUG
IObjectSpaceRender* dxRenderFactory::CreateObjectSpaceRender() { return xr_new<dxObjectSpaceRender>(); }
void dxRenderFactory::DestroyObjectSpaceRender(IObjectSpaceRender* pObject) { xr_delete((dxObjectSpaceRender*&)pObject); }
#endif // DEBUG
IWallMarkArray* dxRenderFactory::CreateWallMarkArray() { return xr_new<dxWallMarkArray>(); }
void dxRenderFactory::DestroyWallMarkArray(IWallMarkArray* pObject) { xr_delete((dxWallMarkArray*&)pObject); }
IStatsRender* dxRenderFactory::CreateStatsRender() { return xr_new<dxStatsRender>(); }
void dxRenderFactory::DestroyStatsRender(IStatsRender* pObject) { xr_delete((dxStatsRender*&)pObject); }

IThunderboltRender* dxRenderFactory::CreateThunderboltRender() { return xr_new<dxThunderboltRender>(); }
void dxRenderFactory::DestroyThunderboltRender(IThunderboltRender* pObject) { xr_delete((dxThunderboltRender*&)pObject); }
IThunderboltDescRender* dxRenderFactory::CreateThunderboltDescRender() { return xr_new<dxThunderboltDescRender>(); }
void dxRenderFactory::DestroyThunderboltDescRender(IThunderboltDescRender* pObject) { xr_delete((dxThunderboltDescRender*&)pObject); }
IRainRender* dxRenderFactory::CreateRainRender() { return xr_new<dxRainRender>(); }
void dxRenderFactory::DestroyRainRender(IRainRender* pObject) { xr_delete((dxRainRender*&)pObject); }
ILensFlareRender* dxRenderFactory::CreateLensFlareRender() { return xr_new<dxLensFlareRender>(); }
void dxRenderFactory::DestroyLensFlareRender(ILensFlareRender* pObject) { xr_delete((dxLensFlareRender*&)pObject); }
IEnvironmentRender* dxRenderFactory::CreateEnvironmentRender() { return xr_new<dxEnvironmentRender>(); }
void dxRenderFactory::DestroyEnvironmentRender(IEnvironmentRender* pObject) { xr_delete((dxEnvironmentRender*&)pObject); }
IEnvDescriptorMixerRender* dxRenderFactory::CreateEnvDescriptorMixerRender() { return xr_new<dxEnvDescriptorMixerRender>(); }
void dxRenderFactory::DestroyEnvDescriptorMixerRender(IEnvDescriptorMixerRender* pObject) { xr_delete((dxEnvDescriptorMixerRender*&)pObject); }
IEnvDescriptorRender* dxRenderFactory::CreateEnvDescriptorRender() { return xr_new<dxEnvDescriptorRender>(); }
void dxRenderFactory::DestroyEnvDescriptorRender(IEnvDescriptorRender* pObject) { xr_delete((dxEnvDescriptorRender*&)pObject); }
IFlareRender* dxRenderFactory::CreateFlareRender() { return xr_new<dxFlareRender>(); }
void dxRenderFactory::DestroyFlareRender(IFlareRender* pObject) { xr_delete((dxFlareRender*&)pObject); }
IFontRender* dxRenderFactory::CreateFontRender() { return xr_new<dxFontRender>(); }
void dxRenderFactory::DestroyFontRender(IFontRender* pObject) { xr_delete((dxFontRender*&)pObject); }

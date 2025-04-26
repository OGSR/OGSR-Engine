#include "stdafx.h"


#include "pure.h"

void rp_Frame(void* p) { static_cast<pureFrame*>(p)->OnFrame(); };
void rp_Render(void* p) { static_cast<pureRender*>(p)->OnRender(); };
void rp_AppActivate(void* p) { static_cast<pureAppActivate*>(p)->OnAppActivate(); };
void rp_AppDeactivate(void* p) { static_cast<pureAppDeactivate*>(p)->OnAppDeactivate(); };
void rp_AppStart(void* p) { static_cast<pureAppStart*>(p)->OnAppStart(); };
void rp_AppEnd(void* p) { static_cast<pureAppEnd*>(p)->OnAppEnd(); };
void rp_DeviceReset(void* p) { static_cast<pureDeviceReset*>(p)->OnDeviceReset(); };
void rp_ScreenResolutionChanged(void* p) { static_cast<pureScreenResolutionChanged*>(p)->OnScreenResolutionChanged(); };

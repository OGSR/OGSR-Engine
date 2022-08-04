#pragma once

#include "../../xrCore/xrCore.h"

class IRender_interface;
extern XRCORE_API IRender_interface* Render;

class IRenderFactory;
extern XRCORE_API IRenderFactory* RenderFactory;

class CDUInterface;
extern XRCORE_API CDUInterface* DU;

struct xr_token;
extern XRCORE_API xr_token* vid_mode_token;

class IUIRender;
extern XRCORE_API IUIRender* UIRender;

class CGameMtlLibrary;
extern XRCORE_API CGameMtlLibrary* PGMLib;

//#ifdef DEBUG
class IDebugRender;
extern XRCORE_API IDebugRender* DRender;
//#endif // DEBUG

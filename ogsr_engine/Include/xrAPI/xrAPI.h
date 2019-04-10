#pragma once

#include "../xr_3da/Engine.h"

class IRender_interface;
extern ENGINE_API IRender_interface* Render;

class IRenderFactory;
extern ENGINE_API IRenderFactory* RenderFactory;

class CDUInterface;
extern ENGINE_API CDUInterface*	DU;

struct xr_token;
extern ENGINE_API xr_token*	vid_mode_token;

class IUIRender;
extern ENGINE_API IUIRender* UIRender;


class CGameMtlLibrary;
extern ENGINE_API CGameMtlLibrary* PGMLib;

//#ifdef DEBUG
class IDebugRender;
extern ENGINE_API IDebugRender* DRender;
//#endif // DEBUG

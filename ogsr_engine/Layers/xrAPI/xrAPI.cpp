// xrAPI.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#include "../../Include/xrApi/xrAPI.h"

XRCORE_API IRender_interface* Render = nullptr;
XRCORE_API IRenderFactory* RenderFactory = nullptr;
XRCORE_API CDUInterface* DU = nullptr;
XRCORE_API xr_token* vid_mode_token = nullptr;
XRCORE_API IUIRender* UIRender = nullptr;
XRCORE_API CGameMtlLibrary* PGMLib = nullptr;
//#ifdef DEBUG
XRCORE_API IDebugRender* DRender = nullptr;
//#endif // DEBUG

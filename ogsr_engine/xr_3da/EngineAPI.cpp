// EngineAPI.cpp: implementation of the CEngineAPI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EngineAPI.h"
#include "xr_ioconsole.h"
#include "xr_ioc_cmd.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEngineAPI::CEngineAPI()
{
    hGame = nullptr;
    hRender = nullptr;
    hTuner = nullptr;
    pCreate = nullptr;
    pDestroy = nullptr;
}

CEngineAPI::~CEngineAPI()
{
}

ENGINE_API int g_current_renderer = 0;

//ENGINE_API bool is_enough_address_space_available()
//{
//    SYSTEM_INFO system_info;
//    GetSystemInfo(&system_info);
//
//    return (*(size_t*)&system_info.lpMaximumApplicationAddress) > 0x90000000ull;
//}

#ifdef XRGAME_STATIC
extern "C" {
DLL_Pure* xrFactory_Create(CLASS_ID clsid);
void xrFactory_Destroy(DLL_Pure* O);
}
#endif

void CEngineAPI::Initialize()
{
    void AttachRender();
    AttachRender();
    g_current_renderer = 4;
    Console->Execute("renderer renderer_r4");

    Device.ConnectToRender();

    pCreate = &xrFactory_Create;
    pDestroy = &xrFactory_Destroy;
    void AttachGame();
    AttachGame();
}

void CEngineAPI::Destroy()
{
    pCreate = nullptr;
    pDestroy = nullptr;
    Engine.Event.Destroy();
}
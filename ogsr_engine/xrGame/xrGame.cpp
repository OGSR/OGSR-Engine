////////////////////////////////////////////////////////////////////////////
//	Module 		: xrGame.cpp
//	Created 	: 07.01.2001
//  Modified 	: 27.05.2004
//	Author		: Aleksandr Maksimchuk and Oles' Shyshkovtsov
//	Description : Defines the entry point for the DLL application.
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "object_factory.h"
#include "ui/xrUIXmlParser.h"
#include "xr_level_controller.h"
#include "profiler.h"

extern "C" {
	DLL_API DLL_Pure*	__cdecl xrFactory_Create		(CLASS_ID clsid)
	{
		DLL_Pure			*object = object_factory().client_object(clsid);
#ifdef DEBUG
		if (!object)
			return			(0);
#endif
		object->CLS_ID		= clsid;
		return				(object);
	}

	DLL_API void		__cdecl	xrFactory_Destroy		(DLL_Pure* O)
	{
		xr_delete			(O);
	}
};

extern void CCC_RegisterCommands();
extern float g_fTimeFactor;
extern shared_str g_active_task_id;

#ifdef XRGAME_STATIC

void AttachGame()
{
	g_fTimeFactor = pSettings->r_float("alife", "time_factor");
	g_active_task_id = "";

	// register console commands
	CCC_RegisterCommands();
	// keyboard binding
	CCC_RegisterInput();
#ifdef DEBUG
	g_profiler = xr_new<CProfiler>();
#endif
}

#else

BOOL APIENTRY DllMain(HANDLE hModule, u32 ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH: {
			g_fTimeFactor = pSettings->r_float("alife", "time_factor");
			g_active_task_id = "";

			// register console commands
			CCC_RegisterCommands();
			// keyboard binding
			CCC_RegisterInput			();
#ifdef DEBUG
			g_profiler			= xr_new<CProfiler>();
#endif
			break;
		}

		case DLL_PROCESS_DETACH: {
			break;
		}
	}
    return								(TRUE);
}

#endif

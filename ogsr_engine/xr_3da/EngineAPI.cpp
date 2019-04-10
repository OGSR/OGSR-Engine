// EngineAPI.cpp: implementation of the CEngineAPI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EngineAPI.h"
#include "xrXRC.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void __cdecl dummy		(void)	{
};
CEngineAPI::CEngineAPI	()
{
	hGame			= 0;
	hRender			= 0;
	hTuner			= 0;
	pCreate			= 0;
	pDestroy		= 0;
	tune_pause		= dummy	;
	tune_resume		= dummy	;
}

CEngineAPI::~CEngineAPI()
{
}

extern u32 renderer_value; //con cmd
ENGINE_API int g_current_renderer = 0;

ENGINE_API bool is_enough_address_space_available()
{
	SYSTEM_INFO system_info;
	GetSystemInfo(&system_info);

	return (*(u32*)&system_info.lpMaximumApplicationAddress) > 0x90000000;
}


#ifdef XRGAME_STATIC
extern "C" {
	DLL_Pure* xrFactory_Create(CLASS_ID clsid);
	void xrFactory_Destroy(DLL_Pure* O);
}
#endif

void CEngineAPI::Initialize()
{
#ifdef XRRENDER_R2_STATIC
	void AttachR2();
	AttachR2();
#else
	LPCSTR r1_name = "xrRender_R1.dll";
	LPCSTR r2_name = "xrRender_R2.dll";
	LPCSTR r3_name = "xrRender_R3.dll";
	LPCSTR r4_name = "xrRender_R4.dll";

	if (psDeviceFlags.test(rsR4))
	{
		// try to initialize R4
		Log("Loading DLL:", r4_name);
		hRender = LoadLibrary(r4_name);
		if (0 == hRender)
		{
			// try to load R1
			Msg("! ...Failed - incompatible hardware/pre-Vista OS.");
			psDeviceFlags.set(rsR2, TRUE);
		}
}

	if (psDeviceFlags.test(rsR3))
	{
		// try to initialize R3
		Log("Loading DLL:", r3_name);
		hRender = LoadLibrary(r3_name);
		if (0 == hRender)
		{
			// try to load R1
			Msg("! ...Failed - incompatible hardware/pre-Vista OS.");
			psDeviceFlags.set(rsR2, TRUE);
		}
		else
			g_current_renderer = 3;
	}

#pragma todo("KRodin: временная хрень, потом сделать нормально!")

	if ( true /*psDeviceFlags.test(rsR2)*/)
	{
		// try to initialize R2
		psDeviceFlags.set(rsR4, FALSE);
		psDeviceFlags.set(rsR3, FALSE);
		Log("Loading DLL:", r2_name);
		hRender = LoadLibrary(r2_name);
		if (0 == hRender)
		{
			// try to load R1
			Msg("! ...Failed - incompatible hardware.");
		}
		else
			g_current_renderer = 2;
	}

	if (0 == hRender)
	{
		// try to load R1
		psDeviceFlags.set(rsR4, FALSE);
		psDeviceFlags.set(rsR3, FALSE);
		psDeviceFlags.set(rsR2, FALSE);
		renderer_value = 0; //con cmd

		Log("Loading DLL:", r1_name);
		hRender = LoadLibrary(r1_name);
		if (0 == hRender)	R_CHK(GetLastError());
		R_ASSERT(hRender);
		g_current_renderer = 1;
	}

#endif

	Device.ConnectToRender();

#ifdef XRGAME_STATIC
	pCreate = &xrFactory_Create;
	pDestroy = &xrFactory_Destroy;
	void AttachGame();
	AttachGame();
#else
	constexpr const char* g_name = "xrGame.dll";
	Msg("--Loading DLL: [%s]", g_name);
	hGame = LoadLibrary(g_name);
	ASSERT_FMT(hGame, "Game DLL raised exception during loading or there is no game DLL at all. Error: [%s]", Debug.error2string(GetLastError()));
	pCreate = (Factory_Create*)GetProcAddress(hGame, "xrFactory_Create");
	R_ASSERT(pCreate);
	pDestroy = (Factory_Destroy*)GetProcAddress(hGame, "xrFactory_Destroy");
	R_ASSERT(pDestroy);
#endif
}

void CEngineAPI::Destroy()
{
	if (hGame)				{ FreeLibrary(hGame);	hGame	= 0; }
	if (hRender)			{ FreeLibrary(hRender); hRender = 0; }
	pCreate					= 0;
	pDestroy				= 0;
	Engine.Event._destroy	();
	XRC.r_clear_compact		();
}

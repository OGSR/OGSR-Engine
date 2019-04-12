// EngineAPI.cpp: implementation of the CEngineAPI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EngineAPI.h"
#include "xrXRC.h"

extern xr_token* vid_quality_token;

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
	// destroy quality token here
	if (vid_quality_token)
	{
		for (int i = 0; vid_quality_token[i].name; i++)
		{
			xr_free(vid_quality_token[i].name);
		}
		xr_free(vid_quality_token);
		vid_quality_token = nullptr;
	}
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
	constexpr LPCSTR r1_name = "xrRender_R1.dll";
	constexpr LPCSTR r2_name = "xrRender_R2.dll";
	constexpr LPCSTR r3_name = "xrRender_R3.dll";
	constexpr LPCSTR r4_name = "xrRender_R4.dll";

	if (psDeviceFlags.test(rsR4))
	{
		// try to initialize R4
		Log("[" __FUNCTION__ "] Loading DLL:", r4_name);
		hRender = LoadLibrary(r4_name);
		if (!hRender)
		{
			// try to load R1
			Msg("! ...Failed - incompatible hardware/pre-Vista OS.");
			psDeviceFlags.set(rsR2, TRUE);
		}
}

	if (psDeviceFlags.test(rsR3))
	{
		// try to initialize R3
		Log("[" __FUNCTION__ "] Loading DLL:", r3_name);
		hRender = LoadLibrary(r3_name);
		if (!hRender)
		{
			// try to load R1
			Msg("! ...Failed - incompatible hardware/pre-Vista OS.");
			psDeviceFlags.set(rsR2, TRUE);
		}
		else
			g_current_renderer = 3;
	}

	if ( psDeviceFlags.test(rsR2))
	{
		// try to initialize R2
		psDeviceFlags.set(rsR4, FALSE);
		psDeviceFlags.set(rsR3, FALSE);
		Log("[" __FUNCTION__ "] Loading DLL:", r2_name);
		hRender = LoadLibrary(r2_name);
		if (!hRender)
		{
			// try to load R1
			Msg("! ...Failed - incompatible hardware.");
		}
		else
			g_current_renderer = 2;
	}

	if (!hRender)
	{
		// try to load R1
		psDeviceFlags.set(rsR4, FALSE);
		psDeviceFlags.set(rsR3, FALSE);
		psDeviceFlags.set(rsR2, FALSE);
		renderer_value = 0; //con cmd

		Log("[" __FUNCTION__ "] Loading DLL:", r1_name);
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

extern "C" {
	typedef bool __cdecl SupportsAdvancedRendering(void);
	typedef bool _declspec(dllexport) SupportsDX10Rendering();
	typedef bool _declspec(dllexport) SupportsDX11Rendering();
};

void CEngineAPI::CreateRendererList()
{
	//	TODO: ask renderers if they are supported!
	if (vid_quality_token)
		return;

	bool bSupports_r2 = false;
	bool bSupports_r2_5 = false;
	bool bSupports_r3 = false;
	bool bSupports_r4 = false;

	constexpr LPCSTR r2_name = "xrRender_R2.dll";
	constexpr LPCSTR r3_name = "xrRender_R3.dll";
	constexpr LPCSTR r4_name = "xrRender_R4.dll";

	if (strstr(Core.Params, "-perfhud_hack"))
	{
		bSupports_r2 = true;
		bSupports_r2_5 = true;
		bSupports_r3 = true;
		bSupports_r4 = true;
	}
	else
	{
		// try to initialize R2
		Log("[" __FUNCTION__ "] Loading DLL:", r2_name);
		hRender = LoadLibrary(r2_name);
		if (hRender)
		{
			bSupports_r2 = true;
			SupportsAdvancedRendering *test_rendering = (SupportsAdvancedRendering*)GetProcAddress(hRender, "SupportsAdvancedRendering");
			R_ASSERT(test_rendering);
			bSupports_r2_5 = test_rendering();
			FreeLibrary(hRender);
		}

		// try to initialize R3
		Log("[" __FUNCTION__ "] Loading DLL:", r3_name);
		//	Hide "d3d10.dll not found" message box for XP
		SetErrorMode(SEM_FAILCRITICALERRORS);
		hRender = LoadLibrary(r3_name);
		//	Restore error handling
		SetErrorMode(0);
		if (hRender)
		{
			SupportsDX10Rendering *test_dx10_rendering = (SupportsDX10Rendering*)GetProcAddress(hRender, "SupportsDX10Rendering");
			R_ASSERT(test_dx10_rendering);
			bSupports_r3 = test_dx10_rendering();
			FreeLibrary(hRender);
		}

		// try to initialize R4
		Log("[" __FUNCTION__ "] Loading DLL:", r4_name);
		//	Hide "d3d10.dll not found" message box for XP
		SetErrorMode(SEM_FAILCRITICALERRORS);
		hRender = LoadLibrary(r4_name);
		//	Restore error handling
		SetErrorMode(0);
		if (hRender)
		{
			SupportsDX11Rendering *test_dx11_rendering = (SupportsDX11Rendering*)GetProcAddress(hRender, "SupportsDX11Rendering");
			R_ASSERT(test_dx11_rendering);
			bSupports_r4 = test_dx11_rendering();
			FreeLibrary(hRender);
		}
	}

	hRender = 0;

	xr_vector<LPCSTR>			_tmp;
	u32 i = 0;
	bool bBreakLoop = false;
	for (; i < 6; ++i)
	{
		switch (i)
		{
		case 1:
			if (!bSupports_r2)
				bBreakLoop = true;
			break;
		case 3:		//"renderer_r2.5"
			if (!bSupports_r2_5)
				bBreakLoop = true;
			break;
		case 4:		//"renderer_r_dx10"
			if (!bSupports_r3)
				bBreakLoop = true;
			break;
		case 5:		//"renderer_r_dx11"
			if (!bSupports_r4)
				bBreakLoop = true;
			break;
		default:;
		}

		if (bBreakLoop) break;

		_tmp.push_back(NULL);
		LPCSTR val = NULL;
		switch (i)
		{
		case 0: val = "renderer_r1";			break;
		case 1: val = "renderer_r2a";		break;
		case 2: val = "renderer_r2";			break;
		case 3: val = "renderer_r2.5";		break;
		case 4: val = "renderer_r3";			break; //  -)
		case 5: val = "renderer_r4";			break; //  -)
		}
		if (bBreakLoop) break;
		_tmp.back() = xr_strdup(val);
	}
	u32 _cnt = _tmp.size() + 1;
	vid_quality_token = xr_alloc<xr_token>(_cnt);

	vid_quality_token[_cnt - 1].id = -1;
	vid_quality_token[_cnt - 1].name = NULL;

#ifdef DEBUG
	Msg("Available render modes[%d]:", _tmp.size());
#endif // DEBUG
	for (u32 i = 0; i < _tmp.size(); ++i)
	{
		vid_quality_token[i].id = i;
		vid_quality_token[i].name = _tmp[i];
#ifdef DEBUG
		Msg("[%s]", _tmp[i]);
#endif // DEBUG
	}
}

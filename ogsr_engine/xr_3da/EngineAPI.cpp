// EngineAPI.cpp: implementation of the CEngineAPI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EngineAPI.h"
#include "xr_ioconsole.h"
#include "xr_ioc_cmd.h"

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

#ifndef EXCLUDE_R1
extern u32 renderer_value; //con cmd
#endif

ENGINE_API int g_current_renderer = 0;

ENGINE_API bool is_enough_address_space_available()
{
	SYSTEM_INFO system_info;
	GetSystemInfo(&system_info);

	return (*(size_t*)&system_info.lpMaximumApplicationAddress) > 0x90000000ull;
}


#ifdef XRGAME_STATIC
extern "C" {
	DLL_Pure* xrFactory_Create(CLASS_ID clsid);
	void xrFactory_Destroy(DLL_Pure* O);
}
#endif

void CEngineAPI::Initialize()
{
	CCC_LoadCFG_custom pTmp("renderer ");
	pTmp.Execute(Console->ConfigFile);

#ifndef EXCLUDE_R1
	constexpr LPCSTR r1_name = "xrRender_R1.dll";
#endif
	constexpr LPCSTR r2_name = "xrRender_R2.dll";
	constexpr LPCSTR r3_name = "xrRender_R3.dll";
	constexpr LPCSTR r4_name = "xrRender_R4.dll";

	if (psDeviceFlags.test(rsR4))
	{
		// try to initialize R4
		Msg("--Loading DLL: [%s]", r4_name);
		hRender = LoadLibrary(r4_name);
		if (!hRender)
		{
			// try to load R1
			Msg("!![%s] Can't load module: [%s]! Error: %s", __FUNCTION__, r4_name, Debug.error2string(GetLastError()));
			psDeviceFlags.set(rsR2, TRUE);
		}
		else
			g_current_renderer = 4;
	}

	if (psDeviceFlags.test(rsR3))
	{
		// try to initialize R3
		Msg("--Loading DLL: [%s]", r3_name);
		hRender = LoadLibrary(r3_name);
		if (!hRender)
		{
			// try to load R1
			Msg("!![%s] Can't load module: [%s]! Error: %s", __FUNCTION__, r3_name, Debug.error2string(GetLastError()));
			psDeviceFlags.set(rsR2, TRUE);
		}
		else
			g_current_renderer = 3;
	}

#ifdef EXCLUDE_R1
	if (!hRender)
#else
	if ( psDeviceFlags.test(rsR2))
#endif
	{
		// try to initialize R2
#ifdef EXCLUDE_R1
		if (!psDeviceFlags.test(rsR2))
			Console->Execute("renderer renderer_r2");
#else
		psDeviceFlags.set(rsR4, FALSE);
		psDeviceFlags.set(rsR3, FALSE);
#endif
		Msg("--Loading DLL: [%s]", r2_name);
		hRender = LoadLibrary(r2_name);
		if (!hRender)
		{
#ifdef EXCLUDE_R1
			FATAL("!![%s] Can't load module: [%s]! Error: %s", __FUNCTION__, r2_name, Debug.error2string(GetLastError()));
#else
			// try to load R1
			Msg("!![%s] Can't load module: [%s]! Error: %s", __FUNCTION__, r2_name, Debug.error2string(GetLastError()));
#endif
		}
		else
			g_current_renderer = 2;
	}

#ifndef EXCLUDE_R1
	if (!hRender)
	{
		// try to load R1
		psDeviceFlags.set(rsR4, FALSE);
		psDeviceFlags.set(rsR3, FALSE);
		psDeviceFlags.set(rsR2, FALSE);
		renderer_value = 0; //con cmd

		Msg("--Loading DLL: [%s]", r1_name);
		hRender = LoadLibrary(r1_name);
		ASSERT_FMT(hRender, "!![%s] Can't load module: [%s]! Error: %s", __FUNCTION__, r1_name, Debug.error2string(GetLastError()));
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
	typedef bool SupportsAdvancedRendering();
	typedef bool SupportsDX10Rendering();
	typedef bool SupportsDX11Rendering();
};

class ModuleHandler {
	HMODULE Module = nullptr;

public:
	ModuleHandler(const std::string& ModuleName) {
		Module = LoadLibrary(ModuleName.c_str());
		//if (Module)
		//	Msg("--[%s] Loaded module: [%s]", __FUNCTION__, ModuleName.c_str());
	}

	~ModuleHandler() {
		if (Module) {
			FreeLibrary(Module);
			//Msg("~~[%s] Unloaded module!", __FUNCTION__);
		}
	}

	bool operator!() const { return !Module; }

	void* GetProcAddress(const char* FuncName) const {
		return ::GetProcAddress(Module, FuncName);
	}
};

void CEngineAPI::CreateRendererList()
{
	std::vector<std::string> RendererTokens;

#ifdef EXCLUDE_R1
	size_t i = 2;
#else
	size_t i = 1;
#endif
	for (; i <= 4; i++)
	{
		std::string ModuleName("xrRender_R");
		ModuleName += std::to_string(i) + ".dll";

		ModuleHandler RenderModule(ModuleName);
		if (!RenderModule) {
			Msg("!![%s] Can't load module: [%s]! Error: %s", __FUNCTION__, ModuleName.c_str(), Debug.error2string(GetLastError()));
			break;
		}

		if (i == 1) { //-V547
			RendererTokens.emplace_back("renderer_r1");
		}
		else if (i == 2) {
			RendererTokens.emplace_back("renderer_r2a");
			RendererTokens.emplace_back("renderer_r2");

			auto test_rendering = (SupportsAdvancedRendering*)RenderModule.GetProcAddress("SupportsAdvancedRendering");
			R_ASSERT(test_rendering);
			if (test_rendering())
				RendererTokens.emplace_back("renderer_r2.5");
			else {
				Msg("!![%s] test [SupportsAdvancedRendering] failed!", __FUNCTION__);
				break;
			}
		}
		else if (i == 3) {
			auto test_dx10_rendering = (SupportsDX10Rendering*)RenderModule.GetProcAddress("SupportsDX10Rendering");
			R_ASSERT(test_dx10_rendering);
			if (test_dx10_rendering())
				RendererTokens.emplace_back("renderer_r3");
			else {
				Msg("!![%s] test [SupportsDX10Rendering] failed!", __FUNCTION__);
				break;
			}
		}
		else if (i == 4) {
			auto test_dx11_rendering = (SupportsDX11Rendering*)RenderModule.GetProcAddress("SupportsDX11Rendering");
			R_ASSERT(test_dx11_rendering);
			if (test_dx11_rendering())
				RendererTokens.emplace_back("renderer_r4");
			else {
				Msg("!![%s] test [SupportsDX11Rendering] failed!", __FUNCTION__);
				break;
			}
		}
	}

	size_t cnt = RendererTokens.size() + 1;
	vid_quality_token = xr_alloc<xr_token>(cnt);

	vid_quality_token[cnt - 1].id = -1;
	vid_quality_token[cnt - 1].name = nullptr;

	Msg("--[%s] Available render modes [%u]:", __FUNCTION__, RendererTokens.size());
	for (size_t i = 0; i < RendererTokens.size(); ++i)
	{
		vid_quality_token[i].id = i;
		vid_quality_token[i].name = xr_strdup(RendererTokens[i].c_str());
		Msg("--  [%s]", RendererTokens[i].c_str());
	}
}

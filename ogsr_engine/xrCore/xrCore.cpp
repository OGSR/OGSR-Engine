// xrCore.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#pragma hdrstop

#include <mmsystem.h>
#include <objbase.h>
 
#pragma comment(lib,"winmm.lib")

XRCORE_API xrCore Core;

XRCORE_API ThreadPool* TTAPI = new ThreadPool();

//indicate that we reach WinMain, and all static variables are initialized
XRCORE_API bool gModulesLoaded = false;

static u32	init_counter	= 0;

void xrCore::_initialize	(LPCSTR _ApplicationName, LogCallback cb, BOOL init_fs, LPCSTR fs_fname)
{
	strcpy_s					(ApplicationName,_ApplicationName);
	if (0==init_counter) {
#ifdef XRCORE_STATIC	
		_clearfp();
#ifdef _M_IX86
		_controlfp(_PC_53, MCW_PC);
#endif
		_controlfp(_RC_CHOP, MCW_RC);
		_controlfp(_RC_NEAR, MCW_RC);
		_controlfp(_MCW_EM, MCW_EM);
#endif

		strcpy_s(Params, sizeof(Params), GetCommandLine());
		_strlwr_s(Params, sizeof(Params));

		if (!strstr(Params, "-editor"))
			CoInitializeEx(nullptr, COINIT_MULTITHREADED);

		if (strstr(Params, "-dbg" ) )
			ParamFlags.set(ParamFlag::dbg, TRUE);

		if (strstr(Params, "-nofpslock"))
			ParamFlags.set(ParamFlag::nofpslock, TRUE);
		if (strstr(Params, "-fpslock60"))
			ParamFlags.set(ParamFlag::fpslock60, TRUE);
		if (strstr(Params, "-fpslock120"))
			ParamFlags.set(ParamFlag::fpslock120, TRUE);
		if (strstr(Params, "-fpslock144"))
			ParamFlags.set(ParamFlag::fpslock144, TRUE);
		if (strstr(Params, "-fpslock240"))
			ParamFlags.set(ParamFlag::fpslock240, TRUE);

		string_path		fn,dr,di;

		// application path
        GetModuleFileName(GetModuleHandle(MODULE_NAME),fn,sizeof(fn));
        _splitpath		(fn,dr,di,0,0);
        strconcat		(sizeof(ApplicationPath),ApplicationPath,dr,di);

		// working path
        if( strstr(Params,"-wf") )
        {
            string_path				c_name;
            sscanf					(strstr(Core.Params,"-wf ")+4,"%[^ ] ",c_name);
            SetCurrentDirectory     (c_name);

        }
		GetCurrentDirectory(sizeof(WorkingPath),WorkingPath);

		// User/Comp Name
		DWORD	sz_user		= sizeof(UserName);
		GetUserName			(UserName,&sz_user);

		DWORD	sz_comp		= sizeof(CompName);
		GetComputerName		(CompName,&sz_comp);
		
		Memory._initialize	(strstr(Params,"-mem_debug") ? TRUE : FALSE);

		DUMP_PHASE;

		InitLog				();
		_initialize_cpu		();

//		Debug._initialize	();

		rtc_initialize		();

		xr_FS = std::make_unique<CLocatorAPI>();
		xr_EFS = std::make_unique<EFS_Utils>();
	}
	if (init_fs){
		u32 flags			= 0;
		if (0!=strstr(Params,"-build"))	 flags |= CLocatorAPI::flBuildCopy;
		if (0!=strstr(Params,"-ebuild")) flags |= CLocatorAPI::flBuildCopy|CLocatorAPI::flEBuildCopy;
#ifdef DEBUG
		if (strstr(Params,"-cache"))  flags |= CLocatorAPI::flCacheFiles;
		else flags &= ~CLocatorAPI::flCacheFiles;
#endif // DEBUG
#ifdef _EDITOR // for EDITORS - no cache
		flags 				&=~ CLocatorAPI::flCacheFiles;
#endif // _EDITOR
		flags |= CLocatorAPI::flScanAppRoot;

#ifndef	_EDITOR
	#ifndef ELocatorAPIH
		if (0!=strstr(Params,"-file_activity"))	 flags |= CLocatorAPI::flDumpFileActivity;
	#endif
#endif
		FS._initialize		(flags,0,fs_fname);

		Msg("[OGSR Engine (%s)] build date: [" __DATE__ " " __TIME__ "]", GetBuildConfiguration());
		if (strlen(APPVEYOR_BUILD_VERSION))
			Log("[AppVeyor] build version: [" APPVEYOR_BUILD_VERSION "], repo: [" APPVEYOR_REPO_NAME "]");

		EFS._initialize		();
#ifdef DEBUG
    #ifndef	_EDITOR
		Msg					("CRT heap 0x%08x",_get_heap_handle());
		Msg					("Process heap 0x%08x",GetProcessHeap());
    #endif
#endif // DEBUG
	}
	
	SetLogCB				(cb);
	init_counter++;
}

void xrCore::_destroy		()
{
	--init_counter;
	if (0==init_counter){
		FS._destroy			();
		EFS._destroy		();

		xr_FS.reset();
		xr_EFS.reset();

		Memory._destroy		();
		if (!strstr(Core.Params, "-editor"))
			CoUninitialize();
	}
}

const char* xrCore::GetEngineVersion() {
	static string256 buff;
	if (strlen(APPVEYOR_BUILD_VERSION))
		std::snprintf(buff, sizeof(buff), APPVEYOR_BUILD_VERSION " (%s) from repo: [" APPVEYOR_REPO_NAME "]", GetBuildConfiguration());
	else
		std::snprintf(buff, sizeof(buff), "1.0007 (%s) [OGSR Engine]", GetBuildConfiguration()); //KRodin: I don't know what it's better to write here...
	return buff;
}

constexpr const char* xrCore::GetBuildConfiguration() {
#ifdef _DEBUG
#	ifdef _M_X64
		return "x64_Dbg";
#	else
		return "x86_Dbg";
#	endif
#else
#	ifdef _M_X64
		return "x64";
#	else
		return "x86";
#	endif
#endif
}


#ifndef XRCORE_STATIC
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD ul_reason_for_call, LPVOID lpvReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		_clearfp();
#ifdef _M_IX86
		_controlfp( _PC_53,   MCW_PC );
#endif
		_controlfp( _RC_CHOP, MCW_RC );
		_controlfp( _RC_NEAR, MCW_RC );
		_controlfp( _MCW_EM,  MCW_EM );
		/*
			ѕо сути это не рекомендуемый Microsoft, но повсеместно используемый способ повышени€ точности
			соблюдени€ и измерени€ временных интревалов функци€ми Sleep, QueryPerformanceCounter,
			timeGetTime и GetTickCount.
			‘ункци€ действует на всю операционную систему в целом (!) и нет необходимости вызывать еЄ при
			старте нового потока. ¬ызов timeEndPeriod специалисты Microsoft считают об€зательным.
			≈сть подозрени€, что Windows сама устанавливает максимальную точность при старте таких
			приложений как, например, игры. “огда есть шанс, что вызов timeBeginPeriod здесь бессмысленен.
			Ќедостатком данного способа €вл€етс€ то, что он приводит к общему замедлению работы как
			текущего приложени€, так и всей операционной системы.
			≈щЄ можно посмотреть ссылки:
			https://msdn.microsoft.com/en-us/library/vs/alm/dd757624(v=vs.85).aspx
			https://users.livejournal.com/-winnie/151099.html
			https://github.com/tebjan/TimerTool
		*/
		timeBeginPeriod(1);
		break;
	case DLL_PROCESS_DETACH:
#ifdef USE_MEMORY_MONITOR
		memory_monitor::flush_each_time(true);
#endif // USE_MEMORY_MONITOR
		_clearfp();
		timeEndPeriod(1);
		break;
	}
    return TRUE;
}
#endif // XRCORE_STATIC

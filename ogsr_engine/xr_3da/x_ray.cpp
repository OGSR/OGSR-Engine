//-----------------------------------------------------------------------------
// File: x_ray.cpp
//
// Programmers:
//	Oles		- Oles Shishkovtsov
//	AlexMX		- Alexander Maksimchuk
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "igame_level.h"
#include "igame_persistent.h"

#include "xr_input.h"
#include "xr_ioconsole.h"
#include "x_ray.h"
#include "std_classes.h"
#include "GameFont.h"
#include "resource.h"
#include "LightAnimLibrary.h"
#include "ispatial.h"

#define CORE_FEATURE_SET( feature, section )\
  Core.Features.set( xrCore::Feature::feature, READ_IF_EXISTS( pSettings, r_bool, section, #feature, false ) )


ENGINE_API CInifile* pGameIni = nullptr;
int max_load_stage = 0;

struct _SoundProcessor	: public pureFrame
{
	virtual void OnFrame	( )
	{
		//Msg							("------------- sound: %d [%3.2f,%3.2f,%3.2f]",u32(Device.dwFrame),VPUSH(Device.vCameraPosition));
		Device.Statistic->Sound.Begin();
		::Sound->update				(Device.vCameraPosition,Device.vCameraDirection,Device.vCameraTop);
		Device.Statistic->Sound.End	();
	}
}	SoundProcessor;

// global variables
ENGINE_API	CApplication*	pApp			= NULL;
static		HWND			logoWindow		= NULL;

			void			doBenchmark		(LPCSTR name);
ENGINE_API	bool			g_bBenchmark	= false;
string512	g_sBenchmarkName;


// startup point
void InitEngine		()
{
	Engine.Initialize			( );
	Device.Initialize			( );
}

void InitSettings	()
{
	string_path					fname; 
	FS.update_path				(fname,"$game_config$","system.ltx");
	pSettings					= xr_new<CInifile>	(fname,TRUE);
	CHECK_OR_EXIT				(!pSettings->sections().empty(),make_string("Cannot find file %s.\nReinstalling application may fix this problem.",fname));

	FS.update_path				(fname,"$game_config$","game.ltx");
	pGameIni					= xr_new<CInifile>	(fname,TRUE);
	CHECK_OR_EXIT				(!pGameIni->sections().empty(),make_string("Cannot find file %s.\nReinstalling application may fix this problem.",fname));
}
void InitConsole	()
{
	Console						= xr_new<CConsole>	();
	Console->Initialize			( );

	strcpy_s						(Console->ConfigFile,"user.ltx");
	if (strstr(Core.Params,"-ltx ")) {
		string64				c_name;
		sscanf					(strstr(Core.Params,"-ltx ")+5,"%[^ ] ",c_name);
		strcpy_s					(Console->ConfigFile,c_name);
	}

	CORE_FEATURE_SET( colorize_ammo,              "dragdrop" );
	CORE_FEATURE_SET( colorize_untradable,        "dragdrop" );
	CORE_FEATURE_SET( highlight_cop,              "dragdrop" );
	CORE_FEATURE_SET( equipped_untradable,        "dragdrop" );
	CORE_FEATURE_SET( select_mode_1342,           "dragdrop" );
	CORE_FEATURE_SET( highlight_equipped,         "dragdrop" );
	CORE_FEATURE_SET( af_radiation_immunity_mod,  "features" );
	CORE_FEATURE_SET( condition_jump_weight_mod,  "features" );
	CORE_FEATURE_SET( forcibly_equivalent_slots,  "features" );
	CORE_FEATURE_SET( slots_extend_menu,          "features" );
	CORE_FEATURE_SET( dynamic_sun_movement,       "features" );
	CORE_FEATURE_SET( wpn_bobbing,                "features" );
	CORE_FEATURE_SET( show_inv_item_condition,    "features" );
	CORE_FEATURE_SET( remove_alt_keybinding,      "features" );
	CORE_FEATURE_SET( binoc_firing,               "features" );
	CORE_FEATURE_SET( no_mouse_wheel_switch_slot, "features" );
	CORE_FEATURE_SET( stop_anim_playing,          "features" );
	CORE_FEATURE_SET( corpses_collision,          "features" );
	CORE_FEATURE_SET( more_hide_weapon,           "features" );
	CORE_FEATURE_SET( keep_inprogress_tasks_only, "features" );
	CORE_FEATURE_SET( show_dialog_numbers,        "features" );
	CORE_FEATURE_SET( objects_radioactive,        "features" );
	CORE_FEATURE_SET( af_zero_condition,          "features" );
	CORE_FEATURE_SET( af_satiety,                 "features" );
	CORE_FEATURE_SET( af_psy_health,              "features" );
	CORE_FEATURE_SET( outfit_af,                  "features" );
	CORE_FEATURE_SET( gd_master_only,             "features" );
	CORE_FEATURE_SET( use_legacy_load_screens,    "features" );
	CORE_FEATURE_SET( ogse_new_slots,             "features" );
	CORE_FEATURE_SET( ogse_wpn_zoom_system,       "features" );
	CORE_FEATURE_SET( wpn_cost_include_addons,    "features" );
	CORE_FEATURE_SET( lock_reload_in_sprint,      "features" );
	CORE_FEATURE_SET( hard_ammo_reload,           "features" );
	CORE_FEATURE_SET( engine_ammo_repacker,       "features" );
	CORE_FEATURE_SET( ruck_flag_preferred,        "features" );
	CORE_FEATURE_SET( old_outfit_slot_style,      "features" );
	CORE_FEATURE_SET( npc_simplified_shooting,    "features" );
	CORE_FEATURE_SET( use_trade_deficit_factor,   "features" );
	CORE_FEATURE_SET( show_objectives_ondemand,   "features" );
	CORE_FEATURE_SET( pickup_check_overlaped,     "features" );
	CORE_FEATURE_SET( wallmarks_on_static_only,   "features" );
	CORE_FEATURE_SET( autoreload_wpn,             "features" );
}

void InitInput()
{
	bool exclusive_mode = DINPUT_ENABLE_EXCLUSIVE_MODE;
	if (strstr(Core.Params, "-switch_exclusive_dinput"))
		exclusive_mode = !exclusive_mode;

	pInput = xr_new<CInput>(exclusive_mode);
}
void destroyInput	()
{
	xr_delete					( pInput		);
}
void InitSound		()
{
	CSound_manager_interface::_create					(u64(Device.m_hWnd));
}
void destroySound	()
{
	CSound_manager_interface::_destroy				( );
}
void destroySettings()
{
	xr_delete					( pSettings		);
	xr_delete					( pGameIni		);
}
void destroyConsole	()
{
	Console->Destroy			( );
	xr_delete					(Console);
}
void destroyEngine	()
{
	Device.Destroy				( );
	Engine.Destroy				( );
}

void execUserScript				( )
{
	Console->Execute			("unbindall");

	if (FS.exist("$app_data_root$", Console->ConfigFile))
	{
		Console->ExecuteScript(Console->ConfigFile);
	}
	else
	{
		string_path default_full_name;

		FS.update_path(default_full_name, "$game_config$", "rspec_default.ltx");

		Console->ExecuteScript(default_full_name);
	}
}

void Startup					( )
{
	execUserScript	();
//.	InitInput		();
	InitSound		();

	// ...command line for auto start
	{
		LPCSTR	pStartup			= strstr				(Core.Params,"-start ");
		if (pStartup)				Console->Execute		(pStartup+1);
	}
	{
		LPCSTR	pStartup			= strstr				(Core.Params,"-load ");
		if (pStartup)				Console->Execute		(pStartup+1);
	}

	// Initialize APP
	ShowWindow( Device.m_hWnd , SW_SHOWNORMAL );
	Device.Create				( );
	LALib.OnCreate				( );
	pApp						= xr_new<CApplication>	();
	g_pGamePersistent			= (IGame_Persistent*)	NEW_INSTANCE (CLSID_GAME_PERSISTANT);
	g_SpatialSpace				= xr_new<ISpatial_DB>	();
	g_SpatialSpacePhysic		= xr_new<ISpatial_DB>	();
	
	// Destroy LOGO
	DestroyWindow				(logoWindow);
	logoWindow					= NULL;

	// Main cycle
	Memory.mem_usage();
	Device.Run					( );

	// Destroy APP
	xr_delete					( g_SpatialSpacePhysic	);
	xr_delete					( g_SpatialSpace		);
	DEL_INSTANCE				( g_pGamePersistent		);
	xr_delete					( pApp					);
	Engine.Event.Dump			( );

	// Destroying
	destroySound();
	destroyInput();

	if(!g_bBenchmark)
		destroySettings();

	LALib.OnDestroy				( );
	
	if(!g_bBenchmark)
		destroyConsole();
	else
		Console->Reset();

	destroyEngine();
}

static INT_PTR CALLBACK logDlgProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp )
{
	switch( msg ){
		case WM_INITDIALOG:
			if (auto hBMP = LoadImage(nullptr, "splash.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE))
				SendDlgItemMessage(hw, IDC_STATIC, STM_SETIMAGE, IMAGE_BITMAP, reinterpret_cast<LPARAM>(hBMP));
			break;
		case WM_DESTROY:
			break;
		case WM_CLOSE:
			DestroyWindow( hw );
			break;
		case WM_COMMAND:
			if( LOWORD(wp)==IDCANCEL )
				DestroyWindow( hw );
			break;
		default:
			return FALSE;
	}
	return TRUE;
}

static constexpr auto dwStickyKeysStructSize = sizeof(STICKYKEYS);
static constexpr auto dwFilterKeysStructSize = sizeof(FILTERKEYS);
static constexpr auto dwToggleKeysStructSize = sizeof(TOGGLEKEYS);

struct damn_keys_filter {
	BOOL bScreenSaverState;

	// Sticky & Filter & Toggle keys

	STICKYKEYS StickyKeysStruct;
	FILTERKEYS FilterKeysStruct;
	TOGGLEKEYS ToggleKeysStruct;

	DWORD dwStickyKeysFlags;
	DWORD dwFilterKeysFlags;
	DWORD dwToggleKeysFlags;

	damn_keys_filter	()
	{
		// Screen saver stuff

		bScreenSaverState = FALSE;

		// Saveing current state
		SystemParametersInfo( SPI_GETSCREENSAVEACTIVE , 0 , ( PVOID ) &bScreenSaverState , 0 );

		if ( bScreenSaverState )
			// Disable screensaver
			SystemParametersInfo( SPI_SETSCREENSAVEACTIVE , FALSE , NULL , 0 );

		dwStickyKeysFlags = 0;
		dwFilterKeysFlags = 0;
		dwToggleKeysFlags = 0;


		ZeroMemory( &StickyKeysStruct , dwStickyKeysStructSize );
		ZeroMemory( &FilterKeysStruct , dwFilterKeysStructSize );
		ZeroMemory( &ToggleKeysStruct , dwToggleKeysStructSize );

		StickyKeysStruct.cbSize = dwStickyKeysStructSize;
		FilterKeysStruct.cbSize = dwFilterKeysStructSize;
		ToggleKeysStruct.cbSize = dwToggleKeysStructSize;

		// Saving current state
		SystemParametersInfo( SPI_GETSTICKYKEYS , dwStickyKeysStructSize , ( PVOID ) &StickyKeysStruct , 0 );
		SystemParametersInfo( SPI_GETFILTERKEYS , dwFilterKeysStructSize , ( PVOID ) &FilterKeysStruct , 0 );
		SystemParametersInfo( SPI_GETTOGGLEKEYS , dwToggleKeysStructSize , ( PVOID ) &ToggleKeysStruct , 0 );

		if ( StickyKeysStruct.dwFlags & SKF_AVAILABLE ) {
			// Disable StickyKeys feature
			dwStickyKeysFlags = StickyKeysStruct.dwFlags;
			StickyKeysStruct.dwFlags = 0;
			SystemParametersInfo( SPI_SETSTICKYKEYS , dwStickyKeysStructSize , ( PVOID ) &StickyKeysStruct , 0 );
		}

		if ( FilterKeysStruct.dwFlags & FKF_AVAILABLE ) {
			// Disable FilterKeys feature
			dwFilterKeysFlags = FilterKeysStruct.dwFlags;
			FilterKeysStruct.dwFlags = 0;
			SystemParametersInfo( SPI_SETFILTERKEYS , dwFilterKeysStructSize , ( PVOID ) &FilterKeysStruct , 0 );
		}

		if ( ToggleKeysStruct.dwFlags & TKF_AVAILABLE ) {
			// Disable FilterKeys feature
			dwToggleKeysFlags = ToggleKeysStruct.dwFlags;
			ToggleKeysStruct.dwFlags = 0;
			SystemParametersInfo( SPI_SETTOGGLEKEYS , dwToggleKeysStructSize , ( PVOID ) &ToggleKeysStruct , 0 );
		}
	}

	~damn_keys_filter	()
	{
		if ( bScreenSaverState )
			// Restoring screen saver
			SystemParametersInfo( SPI_SETSCREENSAVEACTIVE , TRUE , NULL , 0 );

		if ( dwStickyKeysFlags) {
			// Restore StickyKeys feature
			StickyKeysStruct.dwFlags = dwStickyKeysFlags;
			SystemParametersInfo( SPI_SETSTICKYKEYS , dwStickyKeysStructSize , ( PVOID ) &StickyKeysStruct , 0 );
		}

		if ( dwFilterKeysFlags ) {
			// Restore FilterKeys feature
			FilterKeysStruct.dwFlags = dwFilterKeysFlags;
			SystemParametersInfo( SPI_SETFILTERKEYS , dwFilterKeysStructSize , ( PVOID ) &FilterKeysStruct , 0 );
		}

		if ( dwToggleKeysFlags ) {
			// Restore FilterKeys feature
			ToggleKeysStruct.dwFlags = dwToggleKeysFlags;
			SystemParametersInfo( SPI_SETTOGGLEKEYS , dwToggleKeysStructSize , ( PVOID ) &ToggleKeysStruct , 0 );
		}

	}
};

#include "xr_ioc_cmd.h"

int APIENTRY WinMain_impl(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* lpCmdLine, int nCmdShow )
{
	HANDLE hCheckPresenceMutex = INVALID_HANDLE_VALUE;
	if (!strstr(lpCmdLine, "-multi_instances")) { // Check for another instance
		constexpr const char* STALKER_PRESENCE_MUTEX = "STALKER-SoC";
		hCheckPresenceMutex = OpenMutex(READ_CONTROL, FALSE, STALKER_PRESENCE_MUTEX);
		if (hCheckPresenceMutex == nullptr) {
			// New mutex
			hCheckPresenceMutex = CreateMutex(nullptr, FALSE, STALKER_PRESENCE_MUTEX);
			if (hCheckPresenceMutex == nullptr)
				// Shit happens
				return 2;
		}
		else {
			// Already running
			CloseHandle(hCheckPresenceMutex);
			return 1;
		}
	}

	//SetThreadAffinityMask		(GetCurrentThread(),1);

	// Title window
	logoWindow = CreateDialog(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_STARTUP), nullptr, logDlgProc);
	HWND logoInsertPos = HWND_TOPMOST;
	if (IsDebuggerPresent())
	{
		logoInsertPos = HWND_NOTOPMOST;
	}
	SetWindowPos(logoWindow, logoInsertPos, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

	LPCSTR						fsgame_ltx_name = "-fsltx ";
	string_path					fsgame = "";
	if (strstr(lpCmdLine, fsgame_ltx_name)) {
		int						sz = xr_strlen(fsgame_ltx_name);
		sscanf					(strstr(lpCmdLine,fsgame_ltx_name)+sz,"%[^ ] ",fsgame);
	}
	
	Core._initialize			("xray",NULL, TRUE, fsgame[0] ? fsgame : NULL);
	InitSettings				();

	{
		damn_keys_filter		filter;
		(void)filter;

		FPU::m24r				();
		InitEngine				();
		InitConsole				();

		LPCSTR benchName = "-batch_benchmark ";
		if(strstr(lpCmdLine, benchName))
		{
			int sz = xr_strlen(benchName);
			string64				b_name;
			sscanf					(strstr(Core.Params,benchName)+sz,"%[^ ] ",b_name);
			doBenchmark				(b_name);
			return 0;
		}

		if(strstr(Core.Params,"-r2a"))	
			Console->Execute			("renderer renderer_r2a");
		else
		if(strstr(Core.Params,"-r2"))	
			Console->Execute			("renderer renderer_r2");
		else
		{
			CCC_LoadCFG_custom*	pTmp = xr_new<CCC_LoadCFG_custom>("renderer ");
			pTmp->Execute				(Console->ConfigFile);
			xr_delete					(pTmp);
		}

		InitInput					( );
		Engine.External.Initialize	( );
		Console->Execute			("stat_memory");
		Startup	 					( );
		Core._destroy				( );

		if (!strstr(lpCmdLine, "-multi_instances")) // Delete application presence mutex
			CloseHandle(hCheckPresenceMutex);
	}
	// here damn_keys_filter class instanse will be destroyed

	return 0;
}

int stack_overflow_exception_filter	(int exception_code)
{
   if (exception_code == EXCEPTION_STACK_OVERFLOW)
   {
       // Do not call _resetstkoflw here, because
       // at this point, the stack is not yet unwound.
       // Instead, signal that the handler (the __except block)
       // is to be executed.
       return EXCEPTION_EXECUTE_HANDLER;
   }
   else
       return EXCEPTION_CONTINUE_SEARCH;
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     char *    lpCmdLine,
                     int       nCmdShow)
{
    gModulesLoaded = true;
	__try 
	{
		Debug._initialize();

		WinMain_impl		(hInstance,hPrevInstance,lpCmdLine,nCmdShow);
	}
	__except(stack_overflow_exception_filter(GetExceptionCode()))
	{
		_resetstkoflw		();
		FATAL				("stack overflow");
	}

	ExitFromWinMain = true;

	return 0;
}

LPCSTR _GetFontTexName (LPCSTR section)
{
	static char* tex_names[]={"texture800","texture","texture1600"};
	int def_idx		= 1;//default 1024x768
	int idx			= def_idx;

	u32 h = Device.dwHeight;

	if(h<=600)		idx = 0;
	else if(h<=900)	idx = 1;
	else 			idx = 2;


	while(idx>=0){
		if( pSettings->line_exist(section,tex_names[idx]) )
			return pSettings->r_string(section,tex_names[idx]);
		--idx;
	}
	return pSettings->r_string(section,tex_names[def_idx]);
}

void _InitializeFont(CGameFont*& F, LPCSTR section, u32 flags)
{
	LPCSTR font_tex_name = _GetFontTexName(section);
	R_ASSERT(font_tex_name);

	if(!F){
		F = xr_new<CGameFont> ("font", font_tex_name, flags);
		Device.seqRender.Add( F, REG_PRIORITY_LOW-1000 );
	}else
		F->Initialize("font",font_tex_name);

	if (pSettings->line_exist(section,"size")){
		float sz = pSettings->r_float(section,"size");
		if (flags&CGameFont::fsDeviceIndependent)	F->SetHeightI(sz);
		else										F->SetHeight(sz);
	}
	if (pSettings->line_exist(section,"interval"))
		F->SetInterval(pSettings->r_fvector2(section,"interval"));

}

CApplication::CApplication()
{
	ll_dwReference	= 0;

	// events
	eQuit						= Engine.Event.Handler_Attach("KERNEL:quit",this);
	eStart						= Engine.Event.Handler_Attach("KERNEL:start",this);
	eStartLoad					= Engine.Event.Handler_Attach("KERNEL:load",this);
	eDisconnect					= Engine.Event.Handler_Attach("KERNEL:disconnect",this);

	// levels
	Level_Current				= 0;
	Level_Scan					( );

	// Font
	pFontSystem					= NULL;

	// Register us
	Device.seqFrame.Add			(this, REG_PRIORITY_HIGH+1000);
	
	Device.seqFrameMT.Add(&SoundProcessor);

	Console->Show				( );

	// App Title
	app_title[ 0 ] = '\0';
}

CApplication::~CApplication()
{
	Console->Hide				( );

	// font
	Device.seqRender.Remove		( pFontSystem		);
	xr_delete					( pFontSystem		);

	Device.seqFrameMT.Remove	(&SoundProcessor);
	Device.seqFrame.Remove		(&SoundProcessor);
	Device.seqFrame.Remove		(this);


	// events
	Engine.Event.Handler_Detach	(eDisconnect,this);
	Engine.Event.Handler_Detach	(eStartLoad,this);
	Engine.Event.Handler_Detach	(eStart,this);
	Engine.Event.Handler_Detach	(eQuit,this);
}

void CApplication::OnEvent(EVENT E, u64 P1, u64 P2)
{
	if (E==eQuit)
	{
		PostQuitMessage	(0);
		
		for (u32 i=0; i<Levels.size(); i++)
		{
			xr_free(Levels[i].folder	);
			xr_free(Levels[i].name	);
		}
	}
	else if(E==eStart) 
	{
		LPSTR		op_server		= LPSTR	(P1);
		LPSTR		op_client		= LPSTR	(P2);
		R_ASSERT	(0==g_pGameLevel);
		R_ASSERT	(0!=g_pGamePersistent);

		{		
			Console->Execute("main_menu off");
			Console->Hide();
			Device.Reset					(false);
			//-----------------------------------------------------------
			g_pGamePersistent->PreStart		(op_server);
			//-----------------------------------------------------------
			g_pGameLevel					= (IGame_Level*)NEW_INSTANCE(CLSID_GAME_LEVEL);
			pApp->LoadBegin					(); 
			g_pGamePersistent->Start		(op_server);
			g_pGameLevel->net_Start			(op_server,op_client);
			pApp->LoadEnd					(); 
		}
		xr_free							(op_server);
		xr_free							(op_client);
	} 
	else if (E==eDisconnect) 
	{
		if (g_pGameLevel) 
		{
			Console->Hide			();
			g_pGameLevel->net_Stop	();
			DEL_INSTANCE			(g_pGameLevel);
			Console->Show			();
			
			if( (FALSE == Engine.Event.Peek("KERNEL:quit")) &&(FALSE == Engine.Event.Peek("KERNEL:start")) )
			{
				Console->Execute("main_menu off");
				Console->Execute("main_menu on");
			}
		}
		R_ASSERT			(0!=g_pGamePersistent);
		g_pGamePersistent->Disconnect();
	}
}

static	CTimer	phase_timer		;
extern	ENGINE_API BOOL			g_appLoaded = FALSE;

void CApplication::LoadBegin	()
{
	ll_dwReference++;
	if (1==ll_dwReference)	{

		g_appLoaded			= FALSE;

		_InitializeFont		(pFontSystem,"ui_font_graffiti19_russian",0);

		ll_hGeom.create		(FVF::F_TL, RCache.Vertex.Buffer(), RCache.QuadIB);
		sh_progress.create	("hud\\default","ui\\ui_load");
		ll_hGeom2.create		(FVF::F_TL, RCache.Vertex.Buffer(),NULL);
		phase_timer.Start	();
		load_stage			= 0;
	}
}

void CApplication::LoadEnd		()
{
	ll_dwReference--;
	if (0==ll_dwReference)		{
		Msg						("* phase time: %d ms",phase_timer.GetElapsed_ms());
		Msg						("* phase cmem: %d K", Memory.mem_usage()/1024);
		Console->Execute		("stat_memory");
		g_appLoaded				= TRUE;
	}
}

void CApplication::destroy_loading_shaders()
{
	hLevelLogo.destroy		();
	sh_progress.destroy		();
//.	::Sound->mute			(false);
}

void CApplication::LoadDraw		()
{
	if(g_appLoaded)				return;
	Device.dwFrame				+= 1;


	if(!Device.Begin () )		return;

	load_draw_internal			();

	Device.End					();
}

void CApplication::LoadTitleInt(LPCSTR str)
{
	load_stage++;

	VERIFY						(ll_dwReference);
	VERIFY						(str && xr_strlen(str)<256);
	strcpy_s						(app_title, str);
	Msg							("* phase time: %d ms",phase_timer.GetElapsed_ms());	phase_timer.Start();
	Msg							("* phase cmem: %d K", Memory.mem_usage()/1024);
//.	Console->Execute			("stat_memory");
	Log							(app_title);
	
	if (g_pGamePersistent->GameType()==1 && !xr_strcmp(g_pGamePersistent->m_game_params.m_alife, "alife"))
		max_load_stage			= 17;
	else
		max_load_stage			= 14;

	LoadDraw					();
}

void CApplication::LoadSwitch	()
{
}

void CApplication::SetLoadLogo			(ref_shader NewLoadLogo)
{
//	hLevelLogo = NewLoadLogo;
//	R_ASSERT(0);
};

// Sequential
void CApplication::OnFrame	( )
{
	Engine.Event.OnFrame			();
	g_SpatialSpace->update			();
	g_SpatialSpacePhysic->update	();
	if (g_pGameLevel)				g_pGameLevel->SoundEvent_Dispatch	( );
}

void CApplication::Level_Append		(LPCSTR folder)
{
	string_path	N1,N2,N3,N4;
	strconcat	(sizeof(N1),N1,folder,"level");
	strconcat	(sizeof(N2),N2,folder,"level.ltx");
	strconcat	(sizeof(N3),N3,folder,"level.geom");
	strconcat	(sizeof(N4),N4,folder,"level.cform");
	if	(
		FS.exist("$game_levels$",N1)		&&
		FS.exist("$game_levels$",N2)		&&
		FS.exist("$game_levels$",N3)		&&
		FS.exist("$game_levels$",N4)	
		)
	{
		sLevelInfo			LI;
		LI.folder			= xr_strdup(folder);
		LI.name				= 0;
		Levels.push_back	(LI);
	}
}

void CApplication::Level_Scan()
{
	xr_vector<char*>*		folder			= FS.file_list_open		("$game_levels$",FS_ListFolders|FS_RootOnly);
	R_ASSERT				(folder&&folder->size());
	for (u32 i=0; i<folder->size(); i++)	Level_Append((*folder)[i]);
	FS.file_list_close		(folder);
#ifdef DEBUG
	folder									= FS.file_list_open		("$game_levels$","$debug$\\",FS_ListFolders|FS_RootOnly);
	if (folder){
		string_path	tmp_path;
		for (u32 i=0; i<folder->size(); i++)
		{
			strconcat			(sizeof(tmp_path),tmp_path,"$debug$\\",(*folder)[i]);
			Level_Append		(tmp_path);
		}

		FS.file_list_close	(folder);
	}
#endif
}

void CApplication::Level_Set(u32 L)
{
	if (L>=Levels.size())	return;
	Level_Current = L;
	FS.get_path	("$level$")->_set	(Levels[L].folder);


	string_path					temp;
	string_path					temp2;
	strconcat					(sizeof(temp),temp,"intro\\intro_",Levels[L].folder);
	temp[xr_strlen(temp)-1] = 0;
	if (FS.exist(temp2, "$game_textures$", temp, ".dds"))
		hLevelLogo.create	("font", temp);
	else
		hLevelLogo.create	("font", "intro\\intro_no_start_picture");
}

int CApplication::Level_ID(LPCSTR name)
{
	char buffer	[256];
	strconcat	(sizeof(buffer),buffer,name,"\\");
	for (u32 I=0; I<Levels.size(); I++)
	{
		if (0==stricmp(buffer,Levels[I].folder))	return int(I);
	}
	return -1;
}


void doBenchmark(LPCSTR name)
{
	g_bBenchmark = true;
	string_path in_file;
	FS.update_path(in_file,"$app_data_root$", name);
	CInifile ini(in_file);
	int test_count = ini.line_count("benchmark");
	LPCSTR test_name,t;
	shared_str test_command;
	for(int i=0;i<test_count;++i){
		ini.r_line			( "benchmark", i, &test_name, &t);
		strcpy_s				(g_sBenchmarkName, test_name);
		
		test_command		= ini.r_string_wb("benchmark",test_name);
		strcpy_s			(Core.Params,*test_command);
		_strlwr_s				(Core.Params);
		
		InitInput					();
		if(i){
			ZeroMemory(&HW,sizeof(CHW));
			InitEngine();
		}


		Engine.External.Initialize	( );

		strcpy_s						(Console->ConfigFile,"user.ltx");
		if (strstr(Core.Params,"-ltx ")) {
			string64				c_name;
			sscanf					(strstr(Core.Params,"-ltx ")+5,"%[^ ] ",c_name);
			strcpy_s				(Console->ConfigFile,c_name);
		}

		Startup	 				();
	}
}

u32 calc_progress_color(u32, u32, int, int);

void CApplication::load_draw_internal()
{
	if (!sh_progress) {
		CHK_DX(HW.pDevice->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1, 0));
		return;
		}

	// Draw logo
	u32	Offset;
	u32	C = 0xffffffff;
	u32	_w = Device.dwWidth;
	u32	_h = Device.dwHeight;
	FVF::TL* pv = NULL;

	//progress
	float bw = 1024.0f;
	float bh = 768.0f;
	Fvector2					k; k.set(float(_w) / bw, float(_h) / bh);

	RCache.set_Shader(sh_progress);
	CTexture*	T = RCache.get_ActiveTexture(0);
	Fvector2					tsz;
	tsz.set((float)T->get_Width(), (float)T->get_Height());
	Frect						back_text_coords;
	Frect						back_coords;
	Fvector2					back_size;

	//progress background
	static float offs = -0.5f;

	if (Core.Features.test(xrCore::Feature::use_legacy_load_screens) || !hLevelLogo)
	{
		back_size.set(1024, 768);
		back_text_coords.lt.set(0, 0); back_text_coords.rb.add(back_text_coords.lt, back_size);
		back_coords.lt.set(offs, offs); back_coords.rb.add(back_coords.lt, back_size);

		back_coords.lt.mul(k); back_coords.rb.mul(k);

		back_text_coords.lt.x /= tsz.x; back_text_coords.lt.y /= tsz.y; back_text_coords.rb.x /= tsz.x; back_text_coords.rb.y /= tsz.y;
		pv = (FVF::TL*) RCache.Vertex.Lock(4, ll_hGeom.stride(), Offset);
		pv->set(back_coords.lt.x, back_coords.rb.y, C, back_text_coords.lt.x, back_text_coords.rb.y);	pv++;
		pv->set(back_coords.lt.x, back_coords.lt.y, C, back_text_coords.lt.x, back_text_coords.lt.y);	pv++;
		pv->set(back_coords.rb.x, back_coords.rb.y, C, back_text_coords.rb.x, back_text_coords.rb.y);	pv++;
		pv->set(back_coords.rb.x, back_coords.lt.y, C, back_text_coords.rb.x, back_text_coords.lt.y);	pv++;
		RCache.Vertex.Unlock(4, ll_hGeom.stride());

		RCache.set_Geometry(ll_hGeom);
		RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);

		if (Core.Features.test(xrCore::Feature::use_legacy_load_screens))
		{
			//progress bar
			back_size.set(268, 37);
			back_text_coords.lt.set(0, 768); back_text_coords.rb.add(back_text_coords.lt, back_size);
			back_coords.lt.set(379, 726); back_coords.rb.add(back_coords.lt, back_size);

			back_coords.lt.mul(k); back_coords.rb.mul(k);

			back_text_coords.lt.x /= tsz.x; back_text_coords.lt.y /= tsz.y; back_text_coords.rb.x /= tsz.x; back_text_coords.rb.y /= tsz.y;

			u32 v_cnt = 40;
			pv = (FVF::TL*)RCache.Vertex.Lock(2 * (v_cnt + 1), ll_hGeom2.stride(), Offset);
			float pos_delta = back_coords.width() / v_cnt;
			float tc_delta = back_text_coords.width() / v_cnt;
			u32 clr = C;

			for (u32 idx = 0; idx < v_cnt + 1; ++idx) {
				clr = calc_progress_color(idx, v_cnt, load_stage, max_load_stage);
				pv->set(back_coords.lt.x + pos_delta * idx + offs, back_coords.rb.y + offs, 0 + EPS_S, 1, clr, back_text_coords.lt.x + tc_delta * idx, back_text_coords.rb.y);	pv++;
				pv->set(back_coords.lt.x + pos_delta * idx + offs, back_coords.lt.y + offs, 0 + EPS_S, 1, clr, back_text_coords.lt.x + tc_delta * idx, back_text_coords.lt.y);	pv++;
			}
			RCache.Vertex.Unlock(2 * (v_cnt + 1), ll_hGeom2.stride());

			RCache.set_Geometry(ll_hGeom2);
			RCache.Render(D3DPT_TRIANGLESTRIP, Offset, 2 * v_cnt);
		}
	}

	//draw level-specific screenshot
	if (hLevelLogo)
	{
		float bx, by;
		if (Core.Features.test(xrCore::Feature::use_legacy_load_screens)) {
			bw = 512.0f;
			bh = 256.0f;

			bx = 257.0f;
			by = 369.0f;
		}
		else {
			bx = 0.0f;
			by = 0.0f;
		}

		Frect r;
		r.lt.set(bx, by);
		r.lt.x += offs;
		r.lt.y += offs;
		r.rb.add(r.lt, Fvector2().set(bw, bh));
		r.lt.mul(k);
		r.rb.mul(k);
		pv = (FVF::TL*) RCache.Vertex.Lock(4, ll_hGeom.stride(), Offset);
		pv->set(r.lt.x, r.rb.y, C, 0, 1);	pv++;
		pv->set(r.lt.x, r.lt.y, C, 0, 0);	pv++;
		pv->set(r.rb.x, r.rb.y, C, 1, 1);	pv++;
		pv->set(r.rb.x, r.lt.y, C, 1, 0);	pv++;
		RCache.Vertex.Unlock(4, ll_hGeom.stride());

		RCache.set_Shader(hLevelLogo);
		RCache.set_Geometry(ll_hGeom);
		RCache.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
	}

	// Draw title
	VERIFY(pFontSystem);
	pFontSystem->Clear();
	pFontSystem->SetColor(color_rgba(157, 140, 120, 255));
	pFontSystem->SetAligment(CGameFont::alCenter);
	pFontSystem->OutI(0.f, 0.815f, app_title);
	pFontSystem->OnRender();
}

u32 calc_progress_color(u32 idx, u32 total, int stage, int max_stage)
{
	if(idx>(total/2)) 
		idx	= total-idx;


	float kk			= (float(stage+1)/float(max_stage))*(total/2.0f);
	float f				= 1/(exp((float(idx)-kk)*0.5f)+1.0f);

	return color_argb_f		(f,1.0f,1.0f,1.0f);
}

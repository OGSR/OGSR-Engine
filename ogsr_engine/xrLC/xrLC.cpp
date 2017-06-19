// xrLC.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "math.h"
#include "build.h"

//#pragma comment(linker,"/STACK:0x800000,0x400000")
//#pragma comment(linker,"/HEAP:0x70000000,0x10000000")

#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"IMAGEHLP.LIB")
#pragma comment(lib,"winmm.LIB")
#pragma comment(lib,"xrCDB.lib")
#pragma comment(lib,"FreeImage.lib")
#pragma comment(lib,"xrCore.lib")

#define PROTECTED_BUILD

#ifdef PROTECTED_BUILD
#	define TRIVIAL_ENCRYPTOR_ENCODER
#	define TRIVIAL_ENCRYPTOR_DECODER
#	include "../xr_3da/trivial_encryptor.h"
#	undef TRIVIAL_ENCRYPTOR_ENCODER
#	undef TRIVIAL_ENCRYPTOR_DECODER
#endif // PROTECTED_BUILD

CBuild*	pBuild		= NULL;

extern void logThread(void *dummy);
extern volatile BOOL bClose;

static const char* h_str = 
	"The following keys are supported / required:\n"
	"-? or -h		== this help\n"
	"-o				== modify build options\n"
	"-nosun			== disable sun-lighting\n"
	"-noise			== disable converting to MU\n"
	"-norgb			== disable common lightmap calculating\n"
	"-nolmaps		== disable lightmaps calculating\n"
	"-skipinvalid	== skip crash if invalid faces exists\n"
	"-lmap_quality	== lightmap quality\n"
	"-f<NAME>		== compile level in GameData\\Levels\\<NAME>\\\n"
	"\n"
	"NOTE: The last key is required for any functionality\n";

void Help()
{
	MessageBox(0,h_str,"Command line options",MB_OK|MB_ICONINFORMATION);
}

// computing build id
XRCORE_API	LPCSTR	build_date;
XRCORE_API	u32		build_id;
static LPSTR month_id[12] = {
	"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"
};

static int days_in_month[12] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static int start_day	= 31;	// 31
static int start_month	= 1;	// January
static int start_year	= 1999;	// 1999

void compute_build_id	()
{
	build_date			= __DATE__;

	int					days;
	int					months = 0;
	int					years;
	string16			month;
	string256			buffer;
	strcpy_s				(buffer,__DATE__);
	sscanf				(buffer,"%s %d %d",month,&days,&years);

	for (int i=0; i<12; i++) {
		if (_stricmp(month_id[i],month))
			continue;

		months			= i;
		break;
	}

	build_id			= (years - start_year)*365 + days - start_day;

	for (int i=0; i<months; ++i)
		build_id		+= days_in_month[i];

	for (int i=0; i<start_month-1; ++i)
		build_id		-= days_in_month[i];
}

void get_console_param(const char *cmd, const char *param_name, const char *expr, float* param)
{
	if (strstr(cmd, param_name)) {
		int						sz = xr_strlen(param_name);
		sscanf					(strstr(cmd,param_name)+sz,expr,param);
	}
}
void get_console_float(const char *cmd, const char *param_name, float* param)
{
	get_console_param(cmd, param_name, "%f", param);
};

typedef int __cdecl xrOptions(b_params* params, u32 version, bool bRunBuild);

void Startup(LPSTR     lpCmdLine)
{
	char cmd[512],name[256];
	BOOL bModifyOptions		= FALSE;

	strcpy(cmd,lpCmdLine);
	strlwr(cmd);
	if (strstr(cmd,"-?") || strstr(cmd,"-h"))			{ Help(); return; }
	if (strstr(cmd,"-f")==0)							{ Help(); return; }
	if (strstr(cmd,"-o"))								bModifyOptions	= TRUE;
	if (strstr(cmd,"-gi"))								b_radiosity		= TRUE;
	if (strstr(cmd,"-noise"))							b_noise			= TRUE;
	if (strstr(cmd,"-nosun"))							b_nosun			= TRUE;

// KD: new options
	if (strstr(cmd,"-norgb"))							b_norgb			= TRUE;
	if (strstr(cmd,"-nolmaps"))							b_nolmaps		= TRUE;
	if (strstr(cmd,"-skipinvalid"))						b_skipinvalid	= TRUE;
	get_console_float(lpCmdLine, "-lmap_quality ", &f_lmap_quality);
	
	// Give a LOG-thread a chance to startup
	//_set_sbh_threshold(1920);
	InitCommonControls		();
	thread_spawn			(logThread, "log-update",	1024*1024,0);
	Sleep					(150);
	
	// Faster FPU 
	SetPriorityClass		(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);

	/*
	u32	dwMin			= 1800*(1024*1024);
	u32	dwMax			= 1900*(1024*1024);
	if (0==SetProcessWorkingSetSize(GetCurrentProcess(),dwMin,dwMax))
	{
		clMsg("*** Failed to expand working set");
	};
	*/
	
	// Load project
	name[0]=0;				sscanf(strstr(cmd,"-f")+2,"%s",name);
	string_path				prjName;
	FS.update_path			(prjName,"$game_levels$",strconcat(sizeof(prjName),prjName,name,"\\build.prj"));
	string256				phaseName;
	Phase					(strconcat(sizeof(phaseName),phaseName,"Reading project [",name,"]..."));

	string256 inf;
	extern  HWND logWindow;
	IReader*	F			= FS.r_open(prjName);
	if (NULL==F){
		sprintf				(inf,"Build failed!\nCan't find level: '%s'",name);
		clMsg				(inf);
		MessageBox			(logWindow,inf,"Error!",MB_OK|MB_ICONERROR);
		return;
	}

	// Version
	u32 version;
	F->r_chunk			(EB_Version,&version);
	clMsg				("version: %d",version);
	R_ASSERT(XRCL_CURRENT_VERSION==version);

	// Header
	b_params				Params;
	F->r_chunk			(EB_Parameters,&Params);

	//KD start
	Params.m_lm_pixels_per_meter	= f_lmap_quality;
	//KD end

	// Show options if needed
	if (bModifyOptions)		
	{
		Phase		("Project options...");
		HMODULE		L = LoadLibrary		("xrLC_Options.dll");
		void*		P = GetProcAddress	(L,"_frmScenePropertiesRun");
		R_ASSERT	(P);
		xrOptions*	O = (xrOptions*)P;
		int			R = O(&Params,version,false);
		FreeLibrary	(L);
		if (R==2)	{
			ExitProcess(0);
		}
	}
	
	// Conversion
	Phase					("Converting data structures...");
	pBuild					= xr_new<CBuild>();
	pBuild->Load			(Params,*F);
	FS.r_close				(F);
	
	// Call for builder
	string_path				lfn;
	CTimer	dwStartupTime;	dwStartupTime.Start();
	FS.update_path			(lfn,_game_levels_,name);
	pBuild->Run				(lfn);
	xr_delete				(pBuild);

	// Show statistic
	extern	std::string make_time(u32 sec);
	u32	dwEndTime			= dwStartupTime.GetElapsed_ms();
	sprintf					(inf,"Time elapsed: %s",make_time(dwEndTime/1000).c_str());
	clMsg					("Build succesful!\n%s",inf);
	MessageBox				(logWindow,inf,"Congratulation!",MB_OK|MB_ICONINFORMATION);

	// Close log
	bClose					= TRUE;
	Sleep					(500);
}

typedef void DUMMY_STUFF (const void*,const u32&,void*);
XRCORE_API DUMMY_STUFF	*g_temporary_stuff;
XRCORE_API DUMMY_STUFF	*g_dummy_stuff;

int APIENTRY WinMain(HINSTANCE hInst,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	// KD: let's init debug to enable exception handling
	Debug._initialize	(false);

	// KD: custom log name
	char app_name[10];
#ifdef _WIN64
	strcpy(app_name, "xrLC_x64");
#else
	strcpy(app_name, "xrLC");
#endif
	// KD: let it be build number like in game
	compute_build_id	();
	
	g_temporary_stuff	= &trivial_encryptor::decode;
	g_dummy_stuff		= &trivial_encryptor::encode;

	Core._initialize	(app_name);
	Startup				(lpCmdLine);
	Core._destroy		();
	
	return 0;
}

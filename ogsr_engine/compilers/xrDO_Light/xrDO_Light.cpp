// xrAI.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "process.h"
#include "global_options.h"

//#pragma comment(linker,"/STACK:0x800000,0x400000")

#pragma comment(lib,"comctl32.lib")
//#pragma comment(lib,"d3dx9.lib")
//#pragma comment(lib,"IMAGEHLP.LIB")
#pragma comment(lib,"winmm.LIB")
#pragma comment(lib,"xrCDB.lib")
#pragma comment(lib,"xrCore.lib")
#pragma comment(lib,"FreeImage.lib")

extern void	xrCompiler			(LPCSTR name);
extern void logThread			(void *dummy);
extern volatile BOOL bClose;

static const char* h_str = 
	"The following keys are supported / required:\n"
	"-? or -h	== this help\n"
	"-f<NAME>	== compile level in gamedata\\levels\\<NAME>\\\n"
	"-norgb			== disable common lightmap calculating\n"
	"-nosun			== disable sun-lighting\n"
	"-o			== modify build options\n"
	"\n"
	"NOTE: The last key is required for any functionality\n";

void Help()
{	MessageBox(0,h_str,"Command line options",MB_OK|MB_ICONINFORMATION); }

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

void Startup(LPSTR     lpCmdLine)
{
	char cmd[512],name[256];
	BOOL bModifyOptions		= FALSE;

	strcpy(cmd,lpCmdLine);
	strlwr(cmd);
	if (strstr(cmd,"-?") || strstr(cmd,"-h"))			{ Help(); return; }
	if (strstr(cmd,"-f")==0)							{ Help(); return; }
	if (strstr(cmd,"-o"))								bModifyOptions = TRUE;
// KD: new options
	if (strstr(cmd,"-norgb"))							b_norgb			= TRUE;
	if (strstr(cmd,"-nosun"))							b_nosun			= TRUE;

	// Give a LOG-thread a chance to startup
	InitCommonControls	();
	thread_spawn		(logThread,	"log-update", 1024*1024,0);
	Sleep				(150);
	
	// Load project
	name[0]=0; sscanf	(strstr(cmd,"-f")+2,"%s",name);
	//FS.update_path	(name,"$game_levels$",name);
	FS.get_path			("$level$")->_set	(name);

	CTimer				dwStartupTime; dwStartupTime.Start();
	xrCompiler			(0);

	// Show statistic
	char	stats[256];
	extern	std::string make_time(u32 sec);
	extern  HWND logWindow;
	sprintf				(stats,"Time elapsed: %s",make_time((dwStartupTime.GetElapsed_ms())/1000).c_str());
	MessageBox			(logWindow,stats,"Congratulation!",MB_OK|MB_ICONINFORMATION);

	bClose				= TRUE;
	Sleep				(500);
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	// KD: let's init debug to enable exception handling
	Debug._initialize	(false);

	// KD: custom log name
	char app_name[10];
#ifdef _WIN64
	strcpy(app_name, "xrDO_x64");
#else
	strcpy(app_name, "xrDO");
#endif
	// KD: let it be build number like in game
	compute_build_id	();
	
	Core._initialize	(app_name);
	Startup				(lpCmdLine);
	Core._destroy		();
	
	return 0;
}

#pragma once

//KRodin: это инклудить только здесь и нигде больше!
#if __has_include("..\build_config_overrides\build_config_defines.h")
#	include "..\build_config_overrides\build_config_defines.h"
#else
#	include "..\build_config_defines.h"
#endif

#pragma warning(disable:4595)
#pragma warning(disable:4996)
#pragma warning(disable:4530)

#ifndef _MT // multithreading disabled
	#error Please enable multi-threaded library...
#endif

#if defined(_DEBUG) && !defined(DEBUG) // Visual Studio defines _DEBUG when you specify the /MTd or /MDd option
#	define DEBUG
#endif

#if defined( _DEBUG ) && defined( NDEBUG )
#error Something strange...
#endif

#if defined(DEBUG) && defined(NDEBUG)
#error Something strange...
#endif

#if defined( _DEBUG ) && defined( DISABLE_DBG_ASSERTIONS )
#define NDEBUG
#undef DEBUG
#endif

#include "xrCore_platform.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#define IC inline
#define ICF __forceinline // !!! this should be used only in critical places found by PROFILER
#define ICN __declspec(noinline)

#include <time.h>
#define ALIGN(a) alignas(a)
#include <sys\utime.h>

// Warnings
#pragma warning (disable : 4251 )		// object needs DLL interface
#pragma warning (disable : 4201 )		// nonstandard extension used : nameless struct/union
#pragma warning (disable : 4100 )		// unreferenced formal parameter //TODO: Надо б убрать игнор и всё поправить.
#pragma warning (disable : 4127 )		// conditional expression is constant
#pragma warning (disable : 4714 )		// __forceinline not inlined
#ifdef _M_X64
#pragma warning (disable : 4512 )
#endif

// stl
#pragma warning (push)
#pragma warning (disable:4702)
#include <algorithm>
#include <limits>
#include <vector>
#include <stack>
#include <list>
#include <set>
#include <map>
#include <string>
#include <functional>
#include <mutex>
#include <typeinfo>
#pragma warning (pop)

// Our headers
#ifdef XRCORE_STATIC
#	define XRCORE_API
#else
#	ifdef XRCORE_EXPORTS
#		define XRCORE_API __declspec(dllexport)
#	else
#		define XRCORE_API __declspec(dllimport)
#	endif
#endif

#include "xrDebug.h"
#include "vector.h"
                        
#include "clsid.h"
#include "xrSyncronize.h"
#include "xrMemory.h"
                        
#include "_stl_extensions.h"
#include "xrsharedmem.h"
#include "xrstring.h"
#include "xr_resource.h"
#include "rt_compressor.h"
#include "xr_shared.h"

// stl ext
struct XRCORE_API xr_rtoken{
    shared_str	name;
    int	   	id;
           	xr_rtoken	(LPCSTR _nm, int _id){name=_nm;id=_id;}
public:
    void	rename		(LPCSTR _nm)		{name=_nm;}
    bool	equal		(LPCSTR _nm)		{return (0==xr_strcmp(*name,_nm));}
};

#pragma pack (push,1)
struct XRCORE_API xr_shortcut{
    enum{
        flShift	= 0x20,
        flCtrl	= 0x40,
        flAlt	= 0x80,
    };
    union{
    	struct{
            u8	 	key;
            Flags8	ext;
        };
        u16		hotkey;
    };
                xr_shortcut		(u8 k, BOOL a, BOOL c, BOOL s):key(k){ext.assign(u8((a?flAlt:0)|(c?flCtrl:0)|(s?flShift:0)));}
                xr_shortcut		(){ext.zero();key=0;}
    bool		similar			(const xr_shortcut& v)const{return ext.equal(v.ext)&&(key==v.key);}
};
#pragma pack (pop)

DEFINE_VECTOR	(shared_str,RStringVec,RStringVecIt);
DEFINE_SET		(shared_str,RStringSet,RStringSetIt);
DEFINE_VECTOR	(xr_rtoken,RTokenVec,RTokenVecIt);
                        
#include "FS.h"
#include "log.h"
#include "xr_trims.h"
#include "xr_ini.h"


#ifdef OGSR_TOTAL_DBG
#	define LogDbg Log
#	define MsgDbg Msg
#	define FuncDbg(...) __VA_ARGS__
#	define ASSERT_FMT_DBG ASSERT_FMT
#else
#	define LogDbg __noop
#	define MsgDbg __noop
#	define FuncDbg __noop
#	define ASSERT_FMT_DBG(cond, ...) do { if (!(cond)) Msg(__VA_ARGS__); } while(0) //Вылета не будет, просто в лог напишем
#endif


#include "LocatorAPI.h"

#include "FileSystem.h"
#include "FTimer.h"
#include "fastdelegate.h"
#include "intrusive_ptr.h"

// destructor
template <class T>
class destructor
{
	T* ptr;
public:
	destructor(T* p)	{ ptr=p;			}
	~destructor()		{ xr_delete(ptr);	}
	IC T& operator() ()
	{	return *ptr; }
};

// ********************************************** The Core definition
class XRCORE_API xrCore
{
public:
	string64	ApplicationName;
	string_path	ApplicationPath;
	string_path	WorkingPath;
	string64	UserName;
	string64	CompName;
	string512	Params;

	Flags16		ParamFlags;
	enum		ParamFlag {
	  dbg = ( 1 << 0 ),
	};

	Flags64 Features;
	struct  Feature {
	  static constexpr u64
	  equipped_untradable        = 1ull << 0,
	  highlight_equipped         = 1ull << 1,
	  af_radiation_immunity_mod  = 1ull << 2,
	  condition_jump_weight_mod  = 1ull << 3,
	  forcibly_equivalent_slots  = 1ull << 4,
	  slots_extend_menu          = 1ull << 5,
	  dynamic_sun_movement       = 1ull << 6,
	  wpn_bobbing                = 1ull << 7,
	  show_inv_item_condition    = 1ull << 8,
	  remove_alt_keybinding      = 1ull << 9,
	  binoc_firing               = 1ull << 10,
	  no_mouse_wheel_switch_slot = 1ull << 11,
	  stop_anim_playing          = 1ull << 12,
	  corpses_collision          = 1ull << 13,
	  more_hide_weapon           = 1ull << 14,
	  keep_inprogress_tasks_only = 1ull << 15,
	  show_dialog_numbers        = 1ull << 16,
	  objects_radioactive        = 1ull << 17,
	  af_zero_condition          = 1ull << 18,
	  af_satiety                 = 1ull << 19,
	  af_psy_health              = 1ull << 20,
	  outfit_af                  = 1ull << 21,
	  gd_master_only             = 1ull << 22,
	  use_legacy_load_screens    = 1ull << 23,
	  ogse_new_slots             = 1ull << 24,
	  ogse_wpn_zoom_system       = 1ull << 25,
	  wpn_cost_include_addons    = 1ull << 26,
	  lock_reload_in_sprint      = 1ull << 27,
	  hard_ammo_reload           = 1ull << 28,
	  engine_ammo_repacker       = 1ull << 29,
	  ruck_flag_preferred        = 1ull << 30,
	  colorize_ammo              = 1ull << 31,
	  highlight_cop              = 1ull << 32,
	  colorize_untradable        = 1ull << 33,
	  select_mode_1342           = 1ull << 34,
	  old_outfit_slot_style      = 1ull << 35,
	  npc_simplified_shooting    = 1ull << 36,
	  autoreload_wpn             = 1ull << 37,
	  use_trade_deficit_factor   = 1ull << 38,
	  show_objectives_ondemand   = 1ull << 39,
	  pickup_check_overlaped     = 1ull << 40,
	  wallmarks_on_static_only   = 1ull << 41;
	};

	void		_initialize	(LPCSTR ApplicationName, LogCallback cb=0, BOOL init_fs=TRUE, LPCSTR fs_fname=0);
	void		_destroy	();

	constexpr const char* GetBuildConfiguration();
	const char* GetEngineVersion();
};

//Borland class dll interface
#define	_BCL

//Borland global function dll interface
#define	_BGCL

extern XRCORE_API xrCore Core;

#include "Utils/thread_pool.hpp"
extern XRCORE_API ThreadPool* TTAPI;

extern XRCORE_API bool gModulesLoaded;

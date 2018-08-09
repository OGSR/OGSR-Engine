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
#define MODULE_NAME "xrCore.dll"

// Warnings
#pragma warning (disable : 4251 )		// object needs DLL interface
#pragma warning (disable : 4201 )		// nonstandard extension used : nameless struct/union
#pragma warning (disable : 4100 )		// unreferenced formal parameter //TODO: Ќадо б убрать игнор и всЄ поправить.
#pragma warning (disable : 4127 )		// conditional expression is constant
#pragma warning (disable : 4714 )		// __forceinline not inlined
#ifdef _M_AMD64
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
		dbg	= ( 1 << 0 ),
		fpslock60 = (1 << 1),
		fpslock120 = (1 << 2),
		fpslock144 = (1 << 3),
		fpslock240 = (1 << 4),
		nofpslock = (1 << 5),
	};

	void		_initialize	(LPCSTR ApplicationName, LogCallback cb=0, BOOL init_fs=TRUE, LPCSTR fs_fname=0);
	void		_destroy	();

	static constexpr const char* GetBuildConfiguration();
	const char* GetEngineVersion();
};
extern XRCORE_API xrCore Core;

#include "Utils/thread_pool.hpp"
extern XRCORE_API ThreadPool* TTAPI;

extern XRCORE_API bool gModulesLoaded;

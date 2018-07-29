#pragma once

#include "../xrCore/xrCore.h"

#ifdef DEBUG
	#define D3D_DEBUG_INFO
#endif

#pragma warning(disable:4995)
#include <d3d9.h>
//#include <dplay8.h>
#pragma warning(default:4995)

// you must define ENGINE_BUILD then building the engine itself
// and not define it if you are about to build DLL
#ifdef ENGINE_BUILD
#	define DLL_API			__declspec(dllimport)
#	define ENGINE_API		__declspec(dllexport)
#else
#	define DLL_API			__declspec(dllexport)
#	define ENGINE_API		__declspec(dllimport)
#endif

#define ECORE_API

// Our headers
#include "engine.h"
#include "defines.h"
#include "../xrCore/log.h"
#include "device.h"
#include "fs.h"

#include "xrXRC.h"

#include "../xrSound/sound.h"

extern ENGINE_API CInifile *pGameIni;

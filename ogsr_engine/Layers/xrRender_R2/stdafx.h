// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

#include <build_config_defines.h>

#pragma warning(disable:4995)
#include "..\..\xr_3da\stdafx.h"
#pragma warning(disable:4995)
#include <d3dx9.h>
#pragma warning(default:4995)
#pragma warning(disable:4714)
#pragma warning( 4 : 4018 )
#pragma warning( 4 : 4244 )
#pragma warning(disable:4237)

#define		R_R1	1
#define		R_R2	2
#define		RENDER	R_R2

// you must define ENGINE_BUILD then building the engine itself
// and not define it if you are about to build DLL
#ifndef NO_ENGINE_API
#ifdef	ENGINE_BUILD
#define DLL_API			__declspec(dllimport)
#define ENGINE_API		__declspec(dllexport)
#else
#define DLL_API			__declspec(dllexport)
#define ENGINE_API		__declspec(dllimport)
#endif
#else
#define ENGINE_API
#define DLL_API
#endif // NO_ENGINE_API

#include "..\..\xr_3da\resourcemanager.h"
#include "..\..\xr_3da\vis_common.h"
#include "..\..\xr_3da\render.h"
#include "_d3d_extensions.h"
#include "..\..\xr_3da\igame_level.h"
#include "..\..\xr_3da\blenders\blender.h"
#include "..\..\xr_3da\blenders\blender_clsid.h"
#include "..\..\xrParticles\psystem.h"
#include "..\xrRender\xrRender_console.h"
#include "r2.h"

#ifdef _EDITOR
#	include "igame_persistent.h"
#	include "environment.h"
#else
#	include "..\..\xr_3da\igame_persistent.h"
#	include "..\..\xr_3da\environment.h"
#endif

IC	void	jitter(CBlender_Compile& C)
{
	C.r_Sampler	("jitter0",	JITTER(0), true, D3DTADDRESS_WRAP, D3DTEXF_POINT, D3DTEXF_NONE, D3DTEXF_POINT);
	C.r_Sampler	("jitter1",	JITTER(1), true, D3DTADDRESS_WRAP, D3DTEXF_POINT, D3DTEXF_NONE, D3DTEXF_POINT);
	C.r_Sampler	("jitter2",	JITTER(2), true, D3DTADDRESS_WRAP, D3DTEXF_POINT, D3DTEXF_NONE, D3DTEXF_POINT);
	C.r_Sampler	("jitter3",	JITTER(3), true, D3DTADDRESS_WRAP, D3DTEXF_POINT, D3DTEXF_NONE, D3DTEXF_POINT);
}

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

#pragma warning(disable:4995)
#include "..\stdafx.h"
#pragma warning(disable:4995)
#include <d3dx9.h>
#pragma warning(default:4995)
#pragma warning(disable:4714)
#pragma warning( 4 : 4018 )
#pragma warning( 4 : 4244 )

#define		R_R1	1
#define		R_R2	2
#define		RENDER	R_R1

#include "..\resourcemanager.h"
#include "..\vis_common.h"
#include "..\render.h"
#include "..\igame_level.h"
#include "..\blenders\blender.h"
#include "..\blenders\blender_clsid.h"
#include "..\psystem.h"
#include "..\xrRender\xrRender_console.h"
#include "FStaticRender.h"

#define		TEX_POINT_ATT	"internal\\internal_light_attpoint"
#define		TEX_SPOT_ATT	"internal\\internal_light_attclip"

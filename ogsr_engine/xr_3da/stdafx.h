#pragma once

#include "../xrCore/xrCore.h"

#ifdef DEBUG
	#define D3D_DEBUG_INFO
#endif

#pragma warning(disable:4995)
#include <d3d9.h>
#pragma warning(default:4995)

// Our headers
#include "engine.h"
#include "defines.h"
#include "device.h"
#include "fs.h"

#include "xrXRC.h"

#include "../xrSound/sound.h"

extern ENGINE_API CInifile *pGameIni;

#define READ_IF_EXISTS(ltx,method,section,name,default_value)\
	((ltx->line_exist(section,name)) ? (ltx->method(section,name)) : (default_value))

#pragma once

#pragma warning(disable : 4995)
#pragma warning(disable : 4577)
#include "../xr_3da/stdafx.h"
#pragma warning(default : 4995)
#pragma warning(4 : 4018)
#pragma warning(4 : 4244)
#pragma warning(disable : 4505)

#include "..\xr_3da\ai_script_space.h" //KRodin: пусть луа для рендера и скриптов инитится в одном месте.

// this include MUST be here, since smart_cast is used >1800 times in the project
#include <smart_cast.h>

#define THROW VERIFY
#define THROW2 VERIFY2
#define THROW3 VERIFY3

#include "../xr_3da/gamefont.h"
#include "../xr_3da/xr_object.h"
#include "../xr_3da/igame_level.h"

#include "../xrCore/_vector3d_ext.h"

#define D3DCOLOR_RGBA(r, g, b, a) (color_argb(a, r, g, b))
#define D3DCOLOR_XRGB(r, g, b) (color_argb(0xff, r, g, b))

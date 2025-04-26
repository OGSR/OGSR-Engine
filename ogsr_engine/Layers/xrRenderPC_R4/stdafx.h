// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

#pragma warning(disable : 4995)
#include "../../xr_3da/stdafx.h"
#pragma warning(disable : 4995)
#include <d3dx/d3dx9.h>
#pragma warning(default : 4995)
#pragma warning(disable : 4714)
#pragma warning(4 : 4018)
#pragma warning(4 : 4244)
#pragma warning(disable : 4237)

#include <DirectXMath.h>
#include <d3d11_1.h>
#include <D3DCompiler.h>

#include <tracy/TracyD3D11.hpp>

#include "../xrRender/xrD3DDefs.h"

#define R_R1 1
#define R_R2 2
#define R_R3 3
#define R_R4 4
#define RENDER R_R4
#define USE_DX11 // оставлено на случай появления кода откуда-то в котором будет нужен этот макрос
#define DX10_FLUID_ENABLE

#include "../../xrParticles/psystem.h"

#include "../xrRender/HW.h"
#include "../xrRender/Shader.h"
#include "../xrRender/R_Backend.h"
#include "../xrRender/R_Backend_Runtime.h"

#include "../xrRender/resourcemanager.h"

#include "../../xr_3da/vis_common.h"
#include "../../xr_3da/render.h"
#include "../../xr_3da/_d3d_extensions.h"
#include "../../xr_3da/igame_level.h"

#include "../xrRender/blenders/blender.h"
#include "../xrRender/blenders/blender_clsid.h"
#include "../xrRender/xrRender_console.h"

#include "../xrRender/Debug/dxPixEventWrapper.h"

#include "r4.h"

IC void jitter(CBlender_Compile& C)
{
    C.r_dx10Texture("jitter0", JITTER(0));
    C.r_dx10Texture("jitter1", JITTER(1));
    C.r_dx10Sampler("smp_jitter");
}

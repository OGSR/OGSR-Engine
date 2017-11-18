#pragma once

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif // _WIN32_WINNT

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>
#include <stdio.h>

#pragma warning(disable:4995)
#include <xmmintrin.h>
#pragma warning(default:4995)

#include "..\xr_3da\stdafx.h"

#include "xrCPU_Pipe.h"
#include "ttapi.h"
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef VC_EXTRALEAN
#	define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif // VC_EXTRALEAN

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#	define WINVER 0x0601		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif // WINVER

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#	define _WIN32_WINNT 0x0601		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif // _WIN32_WINNT

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#	define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif // _WIN32_WINDOWS

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components

#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes

#ifndef _AFX_NO_AFXCMN_SUPPORT
#	include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>		// MFC socket extensions

#define NO_XRNEW
#undef	min
#undef	max
#include "xrcore.h"
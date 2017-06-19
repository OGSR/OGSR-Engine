// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__81632403_DFD8_4A42_A4D3_0AFDD8EA0D25__INCLUDED_)
#define AFX_STDAFX_H__81632403_DFD8_4A42_A4D3_0AFDD8EA0D25__INCLUDED_

#pragma once

#include "../xrCore/xrCore.h"

#pragma warning(disable:4995)
#include <commctrl.h>
#include <d3dx9.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#pragma warning(default:4995)

#ifndef	NDEBUG
#define X_TRY 
#define X_CATCH if (0)
#else
#define X_TRY try
#define X_CATCH catch(...)
#endif

#define ENGINE_API				// fake, to enable sharing with engine
#define ECORE_API				// fake, to enable sharing with editors
#define XR_EPROPS_API
#include "clsid.h"
#include "defines.h"
#include "cl_log.h"

#include "_d3d_extensions.h"
#include "communicate.h"
extern b_params	g_params;

#include "build.h"

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__81632403_DFD8_4A42_A4D3_0AFDD8EA0D25__INCLUDED_)

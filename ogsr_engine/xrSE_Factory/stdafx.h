////////////////////////////////////////////////////////////////////////////
//	Module 		: stdafx.h
//	Created 	: 18.06.2004
//  Modified 	: 18.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Precompiled header creator
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../xrCore/xrCore.h"

#define WIN32_LEAN_AND_MEAN

#define ENGINE_API
#define ECORE_API
#define DLL_API					__declspec(dllexport)
#define TIXML_USE_STL

#include "clsid_game.h"

#define BOOST_THROW_EXCEPTION_HPP_INCLUDED

namespace boost {void throw_exception(const std::exception &A);};

#include "smart_cast.h"

#define READ_IF_EXISTS(ltx,method,section,name,default_value)\
	(ltx->line_exist(section,name)) ? ltx->method(section,name) : default_value

#if XRAY_EXCEPTIONS
IC	xr_string string2xr_string(LPCSTR s) {return s ? s : "";}
#	define	THROW(xpr)				if (!(xpr)) {throw *shared_str(__FILE__LINE__"\""#xpr"\"");}
#	define	THROW2(xpr,msg0)		if (!(xpr)) {throw *shared_str(xr_string(__FILE__LINE__).append(" \"").append(#xpr).append(string2xr_string(msg0)).c_str());}
#	define	THROW3(xpr,msg0,msg1)	if (!(xpr)) {throw *shared_str(xr_string(__FILE__LINE__).append(" \"").append(#xpr).append(string2xr_string(msg0)).append(", ").append(string2xr_string(msg1)).c_str());}
#else
#	define	THROW					VERIFY
#	define	THROW2					VERIFY2
#	define	THROW3					VERIFY3
#endif

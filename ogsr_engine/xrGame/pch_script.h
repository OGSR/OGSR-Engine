////////////////////////////////////////////////////////////////////////////
//	Module 		: pch_script.h
//	Created 	: 23.05.2007
//  Modified 	: 23.05.2007
//	Author		: Dmitriy Iassenev
//	Description : precompiled header for lua and luabind users
////////////////////////////////////////////////////////////////////////////

#ifndef PCH_SCRIPT_H
#define PCH_SCRIPT_H

#include "stdafx.h"

//#pragma warning(disable:4244)
//#pragma warning(disable:4995)
//#pragma warning(disable:4530)
//#pragma warning(disable:4267)

extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
	#include <luajit.h>
};

#pragma warning(push)
#pragma warning(disable:4995)
#include <luabind/luabind.hpp>
#pragma warning(pop)

#include <luabind/object.hpp>
#include <luabind/functor.hpp>
#include <luabind/operator.hpp>
#include <luabind/adopt_policy.hpp>
#include <luabind/return_reference_to_policy.hpp>
#include <luabind/out_value_policy.hpp>
#include <luabind/iterator_policy.hpp>

#endif // PCH_SCRIPT_H
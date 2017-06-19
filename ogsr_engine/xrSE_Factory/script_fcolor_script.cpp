////////////////////////////////////////////////////////////////////////////
//	Module 		: script_Fcolor_script.cpp
//	Created 	: 28.06.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script float vector script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_fcolor.h"
#include "script_space.h"
#include <luabind/return_reference_to_policy.hpp>
#include <luabind/out_value_policy.hpp>

using namespace luabind;

#pragma optimize("s",on)
void CScriptFcolor::script_register(lua_State *L)
{
	module(L)
	[
		class_<Fcolor>("fcolor")
			.def_readwrite("r",					&Fcolor::r)
			.def_readwrite("g",					&Fcolor::g)
			.def_readwrite("b",					&Fcolor::b)
			.def_readwrite("a",					&Fcolor::a)
			.def(								constructor<>())
			.def("set",							(Fcolor & (Fcolor::*)(float,float,float,float))(&Fcolor::set),														return_reference_to(_1))
			.def("set",							(Fcolor & (Fcolor::*)(const Fcolor &))(&Fcolor::set),																return_reference_to(_1))
			.def("set",							(Fcolor & (Fcolor::*)(u32))(&Fcolor::set),																			return_reference_to(_1))
	];
}

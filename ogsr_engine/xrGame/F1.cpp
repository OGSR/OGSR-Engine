#include "pch_script.h"
#include "f1.h"

CF1::CF1(void) {
}

CF1::~CF1(void) {
}

using namespace luabind;

#pragma optimize("s",on)
void CF1::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CF1,CGameObject>("CF1")
			.def(constructor<>())
	];
}

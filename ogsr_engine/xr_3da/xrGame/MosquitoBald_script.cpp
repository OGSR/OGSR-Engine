#include "pch_script.h"
#include "MosquitoBald.h"

using namespace luabind;

#pragma optimize("s",on)
void CMosquitoBald::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CMosquitoBald,CGameObject>("CMosquitoBald")
			.def(constructor<>())
	];
}

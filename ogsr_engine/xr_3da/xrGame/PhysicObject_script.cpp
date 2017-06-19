#include "pch_script.h"
#include "PhysicObject.h"

using namespace luabind;

#pragma optimize("s",on)
void CPhysicObject::script_register(lua_State *L)
{
	module(L)
	[
		class_<CPhysicObject,CGameObject>("CPhysicObject")
			.def(constructor<>())
	];
}

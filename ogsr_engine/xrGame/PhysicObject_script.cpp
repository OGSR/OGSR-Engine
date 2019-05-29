#include "stdafx.h"
#include "PhysicObject.h"
#include "PHCollisionDamageReceiver.h"
#include "PHDestroyable.h"
#include "hit_immunity.h"
#include "damage_manager.h"
#include "DestroyablePhysicsObject.h"

using namespace luabind;

#pragma optimize("s",on)
void CPhysicObject::script_register(lua_State *L)
{
	module(L)
	[
		class_<CPhysicObject,CGameObject>("CPhysicObject")
			.def(constructor<>())
			.def("set_door_ignore_dynamics", &CPhysicObject::set_door_ignore_dynamics)
			.def("unset_door_ignore_dynamics", &CPhysicObject::unset_door_ignore_dynamics),
		class_<CDestroyablePhysicsObject, CPhysicObject>("CDestroyablePhysicsObject")
			.def(constructor<>())
	];
}

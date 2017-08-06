#include "pch_script.h"
#include "weaponusp45.h"

CWeaponUSP45::CWeaponUSP45(void) : CWeaponPistol("USP")
{
	m_weight = .5f;
	m_slot = 1;
}

CWeaponUSP45::~CWeaponUSP45(void)
{
}

using namespace luabind;

#pragma optimize("s",on)
void CWeaponUSP45::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CWeaponUSP45,CGameObject>("CWeaponUSP45")
			.def(constructor<>())
	];
}

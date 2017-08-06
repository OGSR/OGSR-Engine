#include "pch_script.h"
#include "weaponwalther.h"

CWeaponWalther::CWeaponWalther(void) : CWeaponPistol("WALTHER")
{
	m_weight = .5f;
	m_slot = 1;
}

CWeaponWalther::~CWeaponWalther(void)
{
}

using namespace luabind;

#pragma optimize("s",on)
void CWeaponWalther::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CWeaponWalther,CGameObject>("CWeaponWalther")
			.def(constructor<>())
	];
}

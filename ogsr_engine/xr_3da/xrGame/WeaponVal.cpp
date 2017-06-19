#include "pch_script.h"
#include "weaponval.h"

CWeaponVal::CWeaponVal(void) : CWeaponMagazined("VAL",SOUND_TYPE_WEAPON_SUBMACHINEGUN)
{
	m_weight = 1.5f;
	m_slot = 2;
}

CWeaponVal::~CWeaponVal(void)
{
}

using namespace luabind;

#pragma optimize("s",on)
void CWeaponVal::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CWeaponVal,CGameObject>("CWeaponVal")
			.def(constructor<>())
	];
}

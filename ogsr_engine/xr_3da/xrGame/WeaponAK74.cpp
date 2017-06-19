#include "pch_script.h"
#include "WeaponAK74.h"
#include "WeaponHUD.h"

CWeaponAK74::CWeaponAK74(LPCSTR name, ESoundTypes eSoundType) : CWeaponMagazinedWGrenade(name, eSoundType)
{
}

CWeaponAK74::~CWeaponAK74()
{
}

using namespace luabind;

#pragma optimize("s",on)
void CWeaponAK74::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CWeaponAK74,CGameObject>("CWeaponAK74")
			.def(constructor<>())
	];
}

#include "pch_script.h"
#include "WeaponFN2000.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponFN2000::CWeaponFN2000() : CWeaponMagazined("FN2000",SOUND_TYPE_WEAPON_SNIPERRIFLE)
{
	m_weight = 1.5f;
	m_slot = 2;
}

CWeaponFN2000::~CWeaponFN2000()
{
}

using namespace luabind;

#pragma optimize("s",on)
void CWeaponFN2000::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CWeaponFN2000,CGameObject>("CWeaponFN2000")
			.def(constructor<>())
	];
}

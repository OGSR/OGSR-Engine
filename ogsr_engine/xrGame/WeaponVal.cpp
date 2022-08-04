#include "stdafx.h"
#include "weaponval.h"

CWeaponVal::CWeaponVal(void) : CWeaponMagazined("VAL", SOUND_TYPE_WEAPON_SUBMACHINEGUN)
{
    m_weight = 1.5f;
    SetSlot(SECOND_WEAPON_SLOT);
}

CWeaponVal::~CWeaponVal(void) {}

using namespace luabind;

#pragma optimize("s", on)
void CWeaponVal::script_register(lua_State* L) { module(L)[class_<CWeaponVal, CGameObject>("CWeaponVal").def(constructor<>())]; }

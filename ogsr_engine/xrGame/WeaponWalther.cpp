#include "stdafx.h"
#include "weaponwalther.h"

CWeaponWalther::CWeaponWalther(void) : CWeaponPistol("WALTHER")
{
    m_weight = .5f;
    SetSlot(FIRST_WEAPON_SLOT);
}

CWeaponWalther::~CWeaponWalther(void) {}

using namespace luabind;

#pragma optimize("s", on)
void CWeaponWalther::script_register(lua_State* L) { module(L)[class_<CWeaponWalther, CGameObject>("CWeaponWalther").def(constructor<>())]; }

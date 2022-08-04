#include "stdafx.h"
#include "weapongroza.h"

CWeaponGroza::CWeaponGroza(void) : CWeaponMagazinedWGrenade("GROZA", SOUND_TYPE_WEAPON_SUBMACHINEGUN)
{
    m_weight = 1.5f;
    SetSlot(SECOND_WEAPON_SLOT);
}

CWeaponGroza::~CWeaponGroza(void) {}

using namespace luabind;

#pragma optimize("s", on)
void CWeaponGroza::script_register(lua_State* L) { module(L)[class_<CWeaponGroza, CGameObject>("CWeaponGroza").def(constructor<>())]; }

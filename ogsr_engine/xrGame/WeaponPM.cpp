#include "stdafx.h"
#include "WeaponPM.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponPM::CWeaponPM() : CWeaponPistol("PM")
{
    m_weight = .5f;
    SetSlot(FIRST_WEAPON_SLOT);
}

CWeaponPM::~CWeaponPM() {}

using namespace luabind;

#pragma optimize("s", on)
void CWeaponPM::script_register(lua_State* L) { module(L)[class_<CWeaponPM, CGameObject>("CWeaponPM").def(constructor<>())]; }

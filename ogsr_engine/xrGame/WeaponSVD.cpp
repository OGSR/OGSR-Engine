#include "stdafx.h"
#include "weaponsvd.h"
#include "game_object_space.h"

CWeaponSVD::CWeaponSVD(void) : CWeaponCustomPistol("SVD") {}

CWeaponSVD::~CWeaponSVD(void) {}

void CWeaponSVD::switch2_Fire()
{
    SetPending(TRUE);
    inherited::switch2_Fire();
}

void CWeaponSVD::OnAnimationEnd(u32 state) { inherited::OnAnimationEnd(state); }

using namespace luabind;

#pragma optimize("s", on)
void CWeaponSVD::script_register(lua_State* L) { module(L)[class_<CWeaponSVD, CGameObject>("CWeaponSVD").def(constructor<>())]; }

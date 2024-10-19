#include "stdafx.h"
#include "WeaponKnife.h"

using namespace luabind;


void CWeaponKnife::script_register(lua_State* L) { module(L)[class_<CWeaponKnife, CGameObject>("CWeaponKnife").def(constructor<>())]; }

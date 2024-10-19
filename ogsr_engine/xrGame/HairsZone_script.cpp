#include "stdafx.h"
#include "HairsZone.h"

using namespace luabind;


void CHairsZone::script_register(lua_State* L) { module(L)[class_<CHairsZone, CGameObject>("CHairsZone").def(constructor<>())]; }

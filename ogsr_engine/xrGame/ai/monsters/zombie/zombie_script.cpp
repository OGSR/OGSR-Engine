#include "stdafx.h"
#include "zombie.h"

using namespace luabind;


void CZombie::script_register(lua_State* L) { module(L)[class_<CZombie, CGameObject>("CZombie").def(constructor<>())]; }

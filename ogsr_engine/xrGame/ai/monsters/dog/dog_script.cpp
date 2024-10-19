#include "stdafx.h"
#include "dog.h"

using namespace luabind;


void CAI_Dog::script_register(lua_State* L) { module(L)[class_<CAI_Dog, CGameObject>("CAI_Dog").def(constructor<>())]; }

#include "stdafx.h"
#include "poltergeist.h"

using namespace luabind;


void CPoltergeist::script_register(lua_State* L) { module(L)[class_<CPoltergeist, CGameObject>("CPoltergeist").def(constructor<>())]; }

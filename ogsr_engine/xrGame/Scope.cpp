#include "stdafx.h"
#include "scope.h"

CScope::CScope() {}

CScope::~CScope() {}

using namespace luabind;

#pragma optimize("s", on)
void CScope::script_register(lua_State* L) { module(L)[class_<CScope, CGameObject>("CScope").def(constructor<>())]; }

#include "stdafx.h"
#include "mincer.h"

using namespace luabind;


void CMincer::script_register(lua_State* L) { module(L)[class_<CMincer, CGameObject>("CMincer").def(constructor<>())]; }

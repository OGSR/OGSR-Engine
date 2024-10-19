#include "stdafx.h"
#include "Explosive.h"

using namespace luabind;


void CExplosive::script_register(lua_State* L) { module(L)[class_<CExplosive>("explosive").def("explode", (&CExplosive::Explode))]; }

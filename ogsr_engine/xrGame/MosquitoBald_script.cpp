#include "stdafx.h"
#include "MosquitoBald.h"

using namespace luabind;


void CMosquitoBald::script_register(lua_State* L) { module(L)[class_<CMosquitoBald, CGameObject>("CMosquitoBald").def(constructor<>())]; }

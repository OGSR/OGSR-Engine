#include "stdafx.h"
#include "ai_trader.h"

using namespace luabind;


void CAI_Trader::script_register(lua_State* L) { module(L)[class_<CAI_Trader, CGameObject>("CAI_Trader").def(constructor<>())]; }

////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_human_brain_script.cpp
//	Created 	: 02.11.2005
//  Modified 	: 02.11.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife human brain class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_human_brain.h"

using namespace luabind;


void CALifeHumanBrain::script_register(lua_State* L) { module(L)[class_<CALifeHumanBrain, CALifeMonsterBrain>("CALifeHumanBrain")]; }

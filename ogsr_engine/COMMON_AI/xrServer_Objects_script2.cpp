#include "stdafx.h"
#include "xrServer_Objects.h"
#include "xrServer_script_macroses.h"

using namespace luabind;


void CSE_PHSkeleton::script_register(lua_State* L) { module(L)[class_<CSE_PHSkeleton>("cse_ph_skeleton")]; }

void CSE_AbstractVisual::script_register(lua_State* L)
{
    module(L)[luabind_class_abstract2(CSE_AbstractVisual, "CSE_AbstractVisual", CSE_Visual, CSE_Abstract).def("getStartupAnimation", &CSE_AbstractVisual::getStartupAnimation)];
}

/**
void CSE_SpawnGroup::script_register(lua_State *L)
{
    module(L)[
        luabind_class_abstract1(
            CSE_SpawnGroup,
            "cse_event",
            CSE_Abstract
        )
    ];
}
/**/
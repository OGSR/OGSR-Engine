////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_script3.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Server objects for ALife simulator, script export, the third part
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "xrServer_script_macroses.h"

#ifdef XRGAME_EXPORTS
#include "alife_smart_terrain_task.h"
#endif

using namespace luabind;


void CSE_ALifeObjectHangingLamp::script_register(lua_State* L)
{
    module(L)[luabind_class_dynamic_alife2(CSE_ALifeObjectHangingLamp, "cse_alife_object_hanging_lamp", CSE_ALifeDynamicObjectVisual, CSE_PHSkeleton)];
}
void CSE_ALifeObjectPhysic::script_register(lua_State* L)
{
    module(L)[
        luabind_class_dynamic_alife2(CSE_ALifeObjectPhysic, "cse_alife_object_physic", CSE_ALifeDynamicObjectVisual, CSE_PHSkeleton)
        .def_readwrite("mass", &CSE_ALifeObjectPhysic::mass)
        .property( "fixed_bones", [](CSE_ALifeObjectPhysic* self) { return self->fixed_bones.c_str(); }, [](CSE_ALifeObjectPhysic* self, const char* name) { self->fixed_bones = name; })
    ];
}

void CSE_ALifeSmartZone::script_register(lua_State* L)
{
    module(L)[luabind_class_zone2(CSE_ALifeSmartZone, "cse_alife_smart_zone", CSE_ALifeSpaceRestrictor, CSE_ALifeSchedulable)];
}

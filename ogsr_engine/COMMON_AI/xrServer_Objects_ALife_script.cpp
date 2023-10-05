////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife_script.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Server objects for ALife simulator, script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects_ALife.h"
#include "xrServer_script_macroses.h"
#include "xrServer_Objects_ALife_Monsters.h"

using namespace luabind;

extern u32 get_level_id(u32 gvid);
extern LPCSTR get_level_name_by_id(u32 level_id);

u32 se_obj_level_id(CSE_ALifeObject* O) { return get_level_id(O->m_tGraphID); }
LPCSTR se_obj_level_name(CSE_ALifeObject* O) { return get_level_name_by_id(se_obj_level_id(O)); }

bool se_obj_is_alive(CSE_ALifeObject* O)
{
    CSE_ALifeCreatureAbstract* cr = smart_cast<CSE_ALifeCreatureAbstract*>(O);
    if (cr)
        return cr->g_Alive();
    else
        return false;
}

#pragma optimize("s", on)
void CSE_ALifeSchedulable::script_register(lua_State* L)
{
    module(L)[class_<IPureSchedulableObject>("ipure_schedulable_object"),
              //			.def(		constructor<>()),

              class_<CSE_ALifeSchedulable, IPureSchedulableObject>("cse_alife_schedulable")
              //			.def(		constructor<LPCSTR>())
    ];
}

void CSE_ALifeGraphPoint::script_register(lua_State* L) { module(L)[luabind_class_abstract1(CSE_ALifeGraphPoint, "cse_alife_graph_point", CSE_Abstract)]; }

Flags32& get_flags_ref(CSE_ALifeObject* sobj) { return sobj->m_flags; }

void cse_obj_set_position(CSE_ALifeObject* o, const Fvector& pos) { o->position().set(pos); }

template <typename T>
T* cse_object_cast(CSE_ALifeDynamicObject* se_obj)
{
    return smart_cast<T*>(se_obj);
}

void CSE_ALifeObject::script_register(lua_State* L)
{
    module(L)[luabind_class_alife1(CSE_ALifeObject, "cse_alife_object", CSE_Abstract)
                  .def_readonly("online", &CSE_ALifeObject::m_bOnline)
                  .def("move_offline", (bool(CSE_ALifeObject::*)() const)(&CSE_ALifeObject::move_offline))
                  .def("move_offline", (void(CSE_ALifeObject::*)(bool))(&CSE_ALifeObject::move_offline))
                  .def("visible_for_map", (bool(CSE_ALifeObject::*)() const)(&CSE_ALifeObject::visible_for_map))
                  .def("visible_for_map", (void(CSE_ALifeObject::*)(bool))(&CSE_ALifeObject::visible_for_map))
                  .def("can_switch_online", (void(CSE_ALifeObject::*)(bool))(&CSE_ALifeObject::can_switch_online))
                  .def("can_switch_offline", (void(CSE_ALifeObject::*)(bool))(&CSE_ALifeObject::can_switch_offline))
                  .def("used_ai_locations", (void(CSE_ALifeObject::*)(bool))(&CSE_ALifeObject::used_ai_locations))
                  .def("set_position", &cse_obj_set_position) // alpet: для коррекции позиции в оффлайне
                  .def_readwrite("m_level_vertex_id", &CSE_ALifeObject::m_tNodeID)
                  .def_readwrite("m_game_vertex_id", &CSE_ALifeObject::m_tGraphID)
                  .def_readonly("m_story_id", &CSE_ALifeObject::m_story_id)
                  .property("m_flags", &get_flags_ref)
                  .property("level_id", &se_obj_level_id)
                  .property("level_name", &se_obj_level_name)
                  .property("is_alive", &se_obj_is_alive)
                  .def("get_inventory_item", &cse_object_cast<CSE_ALifeInventoryItem>)
                  .def("get_level_changer", &cse_object_cast<CSE_ALifeLevelChanger>)
                  .def("get_space_restrictor", &cse_object_cast<CSE_ALifeSpaceRestrictor>)
                  .def("get_weapon", &cse_object_cast<CSE_ALifeItemWeapon>)
                  .def("get_weapon_m", &cse_object_cast<CSE_ALifeItemWeaponMagazined>)
                  .def("get_weapon_gl", &cse_object_cast<CSE_ALifeItemWeaponMagazinedWGL>)
                  .def("get_trader", &cse_object_cast<CSE_ALifeTraderAbstract>)
                  .def("get_visual", &cse_object_cast<CSE_Visual>)

                  .def("get_object_physic", &cse_object_cast<CSE_ALifeObjectPhysic>)
                  .def("get_start_zone", &cse_object_cast<CSE_ALifeSmartZone>)
                  .def("get_anomalous_zone", &cse_object_cast<CSE_ALifeAnomalousZone>)
                  .def("get_creature", &cse_object_cast<CSE_ALifeCreatureAbstract>)
                  .def("get_human", &cse_object_cast<CSE_ALifeHumanAbstract>)
                  .def("get_monster", &cse_object_cast<CSE_ALifeMonsterAbstract>)
    ];
}

void CSE_ALifeGroupAbstract::script_register(lua_State* L)
{
    module(L)[class_<CSE_ALifeGroupAbstract>("cse_alife_group_abstract")
              //			.def(		constructor<LPCSTR>())
    ];
}

void CSE_ALifeDynamicObject::script_register(lua_State* L) { module(L)[luabind_class_dynamic_alife1(CSE_ALifeDynamicObject, "cse_alife_dynamic_object", CSE_ALifeObject)]; }

void CSE_ALifeDynamicObjectVisual::script_register(lua_State* L)
{
    module(L)[luabind_class_dynamic_alife2(CSE_ALifeDynamicObjectVisual, "cse_alife_dynamic_object_visual", CSE_ALifeDynamicObject, CSE_Visual)];
}

void CSE_ALifePHSkeletonObject::script_register(lua_State* L)
{
    module(L)[luabind_class_dynamic_alife2(CSE_ALifePHSkeletonObject, "cse_alife_ph_skeleton_object", CSE_ALifeDynamicObjectVisual, CSE_PHSkeleton)];
}

u8 cse_get_restrictor_type(CSE_ALifeDynamicObject* se_obj)
{
    CSE_ALifeSpaceRestrictor* SR = smart_cast<CSE_ALifeSpaceRestrictor*>(se_obj);
    if (SR)
        return SR->m_space_restrictor_type;
    return 0;
}

void CSE_ALifeSpaceRestrictor::script_register(lua_State* L)
{
    module(L)[luabind_class_dynamic_alife2(CSE_ALifeSpaceRestrictor, "cse_alife_space_restrictor", CSE_ALifeDynamicObject, CSE_Shape)
                  .def_readwrite("restrictor_type", &CSE_ALifeSpaceRestrictor::m_space_restrictor_type),
              def("cse_get_restrictor_type", &cse_get_restrictor_type)];
}

void CSE_ALifeLevelChanger::script_register(lua_State* L)
{
    module(L)[luabind_class_dynamic_alife1(CSE_ALifeLevelChanger, "cse_alife_level_changer", CSE_ALifeSpaceRestrictor)
                  .def_readwrite("dest_game_vertex_id", &CSE_ALifeLevelChanger::m_tNextGraphID)
                  .def_readwrite("dest_level_vertex_id", &CSE_ALifeLevelChanger::m_dwNextNodeID)
                  .def_readwrite("dest_position", &CSE_ALifeLevelChanger::m_tNextPosition)
                  .def_readwrite("dest_direction", &CSE_ALifeLevelChanger::m_tAngles)
                  .def_readwrite("silent_mode", &CSE_ALifeLevelChanger::m_SilentMode)];
}

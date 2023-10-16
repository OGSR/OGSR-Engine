////////////////////////////////////////////////////////////////////////////
//	Module 		: script_game_object_script4.cpp
//	Created 	: 14.08.2014
//  Modified 	: 15.08.2014
//	Author		: Alexander Petrov
//	Description : Script Actor (params)
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "pda_space.h"
#include "memory_space.h"
#include "movement_manager_space.h"

#include "cover_point.h"

#include "script_binder_object.h"
#include "script_entity_action.h"
#include "script_game_object.h"
#include "script_hit.h"
#include "script_ini_file.h"
#include "script_monster_hit_info.h"
#include "script_sound_info.h"

#include "action_planner.h"
#include "PhysicsShell.h"

#include "script_zone.h"
#include "relation_registry.h"
#include "danger_object.h"

#include "alife_space.h"

#include "hit_immunity.h"
#include "ActorCondition.h"
#include "EntityCondition.h"
#include "holder_custom.h"

#include "ai_space_inline.h"

#include "exported_classes_def.h"
#include "script_actor.h"

#include "script_engine.h"

#include "xrServer_Objects_ALife.h"
#include "ai_object_location.h"
#include "clsid_game.h"

template <typename T>
T* script_game_object_cast(CScriptGameObject* script_obj)
{
    CGameObject* obj = &script_obj->object();
    return smart_cast<T*>(obj);
}

CEntityCondition* get_obj_conditions(CScriptGameObject* script_obj)
{
    CGameObject* obj = &script_obj->object();
    CActor* pA = smart_cast<CActor*>(obj);
    if (pA)
        return &pA->conditions();

    CEntity* pE = smart_cast<CEntity*>(obj);
    if (pE)
        return pE->conditions();

    return NULL;
}

CHitImmunity* get_obj_immunities(CScriptGameObject* script_obj)
{
    CEntityCondition* cond = get_obj_conditions(script_obj);
    if (cond)
        return smart_cast<CHitImmunity*>(cond);

    CGameObject* obj = &script_obj->object();
    CArtefact* pArt = smart_cast<CArtefact*>(obj);
    if (pArt)
        return &pArt->m_ArtefactHitImmunities;
    return NULL;
}

CInventory* get_obj_inventory(CScriptGameObject* script_obj)
{
    CInventoryOwner* owner = smart_cast<CInventoryOwner*>(&script_obj->object());
    if (owner)
        return owner->m_inventory;
    CHolderCustom* holder = script_obj->get_current_holder();
    if (holder)
        return holder->GetInventory();
    return NULL;
}

CSE_ALifeDynamicObject* CScriptGameObject::alife_object() const { return object().alife_object(); }

u32 get_level_id(u32 gvid)
{
    CGameGraph& gg = ai().game_graph();
    if (gg.valid_vertex_id(gvid))
        return gg.vertex(gvid)->level_id();
    return (u32)-1; // ERROR signal
}

LPCSTR get_level_name_by_id(u32 level_id)
{
    if (level_id < 0xff)
        return ai().game_graph().header().level((GameGraph::_LEVEL_ID)level_id).name().c_str();
    else
        return "l255_invalid_level";
}

bool get_obj_alive(CScriptGameObject* O)
{
    CGameObject* obj = &O->object();
    CEntityAlive* ent = smart_cast<CEntityAlive*>(obj);
    if (ent)
        return ent->g_Alive();
    else
        return false;
}

bool actor_can_take(CScriptGameObject* O)
{
    CGameObject* obj = &O->object();

    if (obj->getDestroy())
        return false;

    CInventoryItem* pIItem = smart_cast<CInventoryItem*>(obj);
    if (0 == pIItem)
        return false;

    if (pIItem->object().H_Parent() != NULL)
        return false;

    if (!pIItem->CanTake())
        return false;

    if (pIItem->object().CLS_ID == CLSID_OBJECT_G_RPG7 || pIItem->object().CLS_ID == CLSID_OBJECT_G_FAKE)
        return false;

    CGrenade* pGrenade = smart_cast<CGrenade*>(obj);
    if (pGrenade && !pGrenade->Useful())
        return false;

    CMissile* pMissile = smart_cast<CMissile*>(obj);
    if (pMissile && !pMissile->Useful())
        return false;

    return true;
}

u32 obj_level_id(CScriptGameObject* O) { return get_level_id(O->object().ai_location().game_vertex_id()); }

LPCSTR obj_level_name(CScriptGameObject* O) { return get_level_name_by_id(obj_level_id(O)); }

#pragma optimize("s", on)
using namespace luabind;

class_<CScriptGameObject> script_register_game_object3(class_<CScriptGameObject>&& instance)
{
    return std::move(instance)
        //#pragma message("+ game_object.extensions export begin")
        // alpet: export object cast
        .def("get_game_object", &CScriptGameObject::object)
        .def("get_alife_object", &CScriptGameObject::alife_object)
        .def("get_actor", &script_game_object_cast<CActorObject>)
        .def("get_anomaly", &script_game_object_cast<CCustomZone>)
        .def("get_artefact", &script_game_object_cast<CArtefact>)
        .def("get_base_monster", &script_game_object_cast<CBaseMonster>)
        .def("get_container", &script_game_object_cast<CInventoryContainer>)
        .def("get_custom_monster", &script_game_object_cast<CCustomMonster>)
        .def("get_eatable_item", &script_game_object_cast<CEatableItem>)
        .def("get_grenade", &script_game_object_cast<CGrenade>)
        .def("get_inventory_box", &script_game_object_cast<IInventoryBox>)
        .def("get_inventory_item", &script_game_object_cast<CInventoryItem>)
        .def("get_inventory_owner", &script_game_object_cast<CInventoryOwner>)
        .def("get_missile", &script_game_object_cast<CMissile>)
        .def("get_outfit", &script_game_object_cast<CCustomOutfit>)
        .def("get_space_restrictor", &script_game_object_cast<CSpaceRestrictor>)
        .def("get_torch", &script_game_object_cast<CTorch>)
        .def("get_weapon", &script_game_object_cast<CWeapon>)
        .def("get_weapon_m", &script_game_object_cast<CWeaponMagazined>)
        .def("get_weapon_mwg", &script_game_object_cast<CWeaponMagazinedWGrenade>)
        .def("get_weapon_sg", &script_game_object_cast<CWeaponShotgun>)

        .def("ph_capture_object", (void(CScriptGameObject::*)(CScriptGameObject*))(&CScriptGameObject::PHCaptureObject))
        .def("ph_capture_object", (void(CScriptGameObject::*)(CScriptGameObject*, LPCSTR))(&CScriptGameObject::PHCaptureObject))
        .def("ph_capture_object", (void(CScriptGameObject::*)(CScriptGameObject*, u16))(&CScriptGameObject::PHCaptureObject))
        .def("ph_capture_object", (void(CScriptGameObject::*)(CScriptGameObject*, u16, LPCSTR))(&CScriptGameObject::PHCaptureObject))
        .def("ph_release_object", &CScriptGameObject::PHReleaseObject)
        .def("ph_capture", &CScriptGameObject::PHCapture)

        .def("throw_target", (bool(CScriptGameObject::*)(const Fvector&, CScriptGameObject*))(&CScriptGameObject::throw_target))
        .def("throw_target", (bool(CScriptGameObject::*)(const Fvector&, u32 const, CScriptGameObject*))(&CScriptGameObject::throw_target))

        .def("g_fireParams", &CScriptGameObject::g_fireParams)
        .def("can_kill_enemy", &CScriptGameObject::can_kill_enemy)
        .def("can_fire_to_enemy", &CScriptGameObject::can_fire_to_enemy)

        .def("register_in_combat", &CScriptGameObject::register_in_combat)
        .def("unregister_in_combat", &CScriptGameObject::unregister_in_combat)

        .def("stalker_disp_base", (float(CScriptGameObject::*)())(&CScriptGameObject::stalker_disp_base))
        .def("stalker_disp_base", (void(CScriptGameObject::*)(float))(&CScriptGameObject::stalker_disp_base))
        .def("stalker_disp_base", (void(CScriptGameObject::*)(float, float))(&CScriptGameObject::stalker_disp_base))

        .def("drop_item_and_throw", &CScriptGameObject::DropItemAndThrow)
        .def("controller_psy_hit_active", &CScriptGameObject::controller_psy_hit_active)

        .def("setEnabled", &CScriptGameObject::setEnabled)
        .def("setVisible", &CScriptGameObject::setVisible)

        .def("actor_can_take", &actor_can_take)

        .property("inventory", &get_obj_inventory)
        .property("immunities", &get_obj_immunities)
        .property("is_alive", &get_obj_alive)
        .property("conditions", &get_obj_conditions)
        .property("level_id", &obj_level_id)
        .property("level_name", &obj_level_name)

        //#pragma message("+ game_object.extensions export end")
        ;
}

void script_register_game_object4(lua_State* L) { module(L)[def("get_actor_obj", &Actor), def("get_level_id", &get_level_id)]; }

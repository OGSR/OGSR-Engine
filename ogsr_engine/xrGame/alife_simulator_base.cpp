////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_simulator_base.cpp
//	Created 	: 25.12.2002
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator base class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_simulator_base.h"
#include "alife_simulator_header.h"
#include "alife_time_manager.h"
#include "alife_spawn_registry.h"
#include "alife_object_registry.h"
#include "alife_graph_registry.h"
#include "alife_schedule_registry.h"
#include "alife_story_registry.h"
#include "alife_smart_terrain_registry.h"
#include "alife_group_registry.h"
#include "alife_registry_container.h"
#include "xrserver.h"
#include "level_graph.h"

using namespace ALife;

CALifeSimulatorBase::CALifeSimulatorBase(xrServer* server, LPCSTR section)
{
    m_server = server;
    m_initialized = false;
    m_header = nullptr;
    m_time_manager = nullptr;
    m_spawns = nullptr;
    m_objects = nullptr;
    m_graph_objects = nullptr;
    m_scheduled = nullptr;
    m_story_objects = nullptr;
    m_smart_terrains = nullptr;
    m_groups = nullptr;
    m_registry_container = nullptr;
    m_can_register_objects = true;
    m_unloading = false;
}

CALifeSimulatorBase::~CALifeSimulatorBase() { VERIFY(!m_initialized); }

void CALifeSimulatorBase::destroy() { unload(); }

void CALifeSimulatorBase::unload()
{
    m_unloading = true;
    xr_delete(m_objects);
    xr_delete(m_header);
    xr_delete(m_time_manager);
    xr_delete(m_spawns);
    xr_delete(m_graph_objects);
    xr_delete(m_scheduled);
    xr_delete(m_story_objects);
    xr_delete(m_smart_terrains);
    xr_delete(m_groups);
    xr_delete(m_registry_container);
    m_initialized = false;
    m_unloading = false;
}

void CALifeSimulatorBase::reload(LPCSTR section)
{
    m_header = xr_new<CALifeSimulatorHeader>(section);
    m_time_manager = xr_new<CALifeTimeManager>(section);
    m_spawns = xr_new<CALifeSpawnRegistry>(section);
    m_objects = xr_new<CALifeObjectRegistry>(section);
    m_graph_objects = xr_new<CALifeGraphRegistry>();
    m_scheduled = xr_new<CALifeScheduleRegistry>();
    m_story_objects = xr_new<CALifeStoryRegistry>();
    m_smart_terrains = xr_new<CALifeSmartTerrainRegistry>();
    m_groups = xr_new<CALifeGroupRegistry>();
    m_registry_container = xr_new<CALifeRegistryContainer>();
    m_initialized = true;
}

CSE_Abstract* CALifeSimulatorBase::spawn_item(LPCSTR section, const Fvector& position, u32 level_vertex_id, GameGraph::_GRAPH_ID game_vertex_id, u16 parent_id, bool registration)
{
    CSE_Abstract* abstract = F_entity_Create(section);
    R_ASSERT3(abstract, "Cannot find item with section", section);

    abstract->s_name = section;
    abstract->s_gameid = u8(GAME_SINGLE); // GameID()
    abstract->s_RP = 0xff;
    abstract->ID = server().PerformIDgen(0xffff);
    abstract->ID_Parent = parent_id;
    abstract->ID_Phantom = 0xffff;
    abstract->o_Position = position;
    abstract->m_wVersion = SPAWN_VERSION;

    string256 s_name_replace;
    strcpy_s(s_name_replace, *abstract->s_name);
    if (abstract->ID < 1000)
        strcat_s(s_name_replace, "0");
    if (abstract->ID < 100)
        strcat_s(s_name_replace, "0");
    if (abstract->ID < 10)
        strcat_s(s_name_replace, "0");
    string16 S1;
    strcat_s(s_name_replace, itoa(abstract->ID, S1, 10));
    abstract->set_name_replace(s_name_replace);

    CSE_ALifeDynamicObject* dynamic_object = smart_cast<CSE_ALifeDynamicObject*>(abstract);
    VERIFY(dynamic_object);

    //оружие спавним с полным магазинои
    CSE_ALifeItemWeapon* weapon = smart_cast<CSE_ALifeItemWeapon*>(dynamic_object);
    if (weapon)
        weapon->a_elapsed = weapon->get_ammo_magsize();

    dynamic_object->m_tNodeID = level_vertex_id;
    dynamic_object->m_tGraphID = game_vertex_id;
    dynamic_object->m_tSpawnID = u16(-1);

    if (registration)
        register_object(dynamic_object, true);

    dynamic_object->spawn_supplies();
    dynamic_object->on_spawn();

    //	Msg							("LSS : SPAWN : [%s],[%s], level
    //%s",*dynamic_object->s_name,dynamic_object->name_replace(),*ai().game_graph().header().level(ai().game_graph().vertex(dynamic_object->m_tGraphID)->level_id()).name());
    return (dynamic_object);
}

CSE_Abstract* CALifeSimulatorBase::create(CSE_ALifeGroupAbstract* tpALifeGroupAbstract, CSE_ALifeDynamicObject* j)
{
    NET_Packet tNetPacket;
    LPCSTR S = pSettings->r_string(tpALifeGroupAbstract->base()->s_name, "monster_section");
    CSE_Abstract* l_tpAbstract = F_entity_Create(S);
    R_ASSERT2(l_tpAbstract, "Can't create entity.");
    CSE_ALifeDynamicObject* k = smart_cast<CSE_ALifeDynamicObject*>(l_tpAbstract);
    R_ASSERT2(k, "Non-ALife object in the 'game.spawn'");

    j->Spawn_Write(tNetPacket, TRUE);
    k->Spawn_Read(tNetPacket);
    tNetPacket.w_begin(M_UPDATE);
    j->UPDATE_Write(tNetPacket);
    u16 id;
    tNetPacket.r_begin(id);
    k->UPDATE_Read(tNetPacket);
    k->s_name = S;
    k->m_tSpawnID = j->m_tSpawnID;
    k->ID = server().PerformIDgen(0xffff);
    k->m_bDirectControl = false;
    k->m_bALifeControl = true;

    string256 s_name_replace;
    strcpy_s(s_name_replace, *k->s_name);
    if (k->ID < 1000)
        strcat_s(s_name_replace, "0");
    if (k->ID < 100)
        strcat_s(s_name_replace, "0");
    if (k->ID < 10)
        strcat_s(s_name_replace, "0");
    string16 S1;
    strcat_s(s_name_replace, itoa(k->ID, S1, 10));
    k->set_name_replace(s_name_replace);

    register_object(k, true);
    k->spawn_supplies();
    k->on_spawn();
    return (k);
}

void CALifeSimulatorBase::create(CSE_ALifeDynamicObject*& i, CSE_ALifeDynamicObject* j, const _SPAWN_ID& tSpawnID)
{
    CSE_Abstract* tpSE_Abstract = F_entity_Create(*j->s_name);
    R_ASSERT3(tpSE_Abstract, "Cannot find item with section", *j->s_name);
    i = smart_cast<CSE_ALifeDynamicObject*>(tpSE_Abstract);
    R_ASSERT2(i, "Non-ALife object in the 'game.spawn'");

    NET_Packet tNetPacket;
    j->Spawn_Write(tNetPacket, TRUE);
    i->Spawn_Read(tNetPacket);
    tNetPacket.w_begin(M_UPDATE);
    j->UPDATE_Write(tNetPacket);
    u16 id;
    tNetPacket.r_begin(id);
    i->UPDATE_Read(tNetPacket);

    R_ASSERT3(!(i->used_ai_locations()) || (i->m_tNodeID != u32(-1)), "Invalid vertex for object ", i->name_replace());

    i->m_tSpawnID = tSpawnID;
    if (!graph().actor() && smart_cast<CSE_ALifeCreatureActor*>(i))
        i->ID = 0;
    else
        i->ID = server().PerformIDgen(0xffff);

    register_object(i, true);
    i->m_bALifeControl = true;

    CSE_ALifeMonsterAbstract* monster = smart_cast<CSE_ALifeMonsterAbstract*>(i);
    if (monster)
        graph().assign(monster);

    CSE_ALifeGroupAbstract* group = smart_cast<CSE_ALifeGroupAbstract*>(i);
    if (group)
    {
        group->m_tpMembers.resize(group->m_wCount);
        OBJECT_IT I = group->m_tpMembers.begin();
        OBJECT_IT E = group->m_tpMembers.end();
        for (; I != E; ++I)
        {
            CSE_Abstract* object = create(group, j);
            *I = object->ID;
        }
    }
    else
        i->spawn_supplies();

    i->on_spawn();
}

void CALifeSimulatorBase::create(CSE_ALifeObject* object)
{
    CSE_ALifeDynamicObject* dynamic_object = smart_cast<CSE_ALifeDynamicObject*>(object);
    if (!dynamic_object)
        return;

    if (!dynamic_object->can_save())
    {
        dynamic_object->m_bALifeControl = false;
        return;
    }
    VERIFY(dynamic_object->m_bOnline);

#ifdef DEBUG
//	Msg							("Creating object from client spawn
//[%d][%d][%s][%s]",dynamic_object->ID,dynamic_object->ID_Parent,dynamic_object->name(),dynamic_object->name_replace());
#endif

    if (0xffff != dynamic_object->ID_Parent)
    {
        u16 id = dynamic_object->ID_Parent;
        CSE_ALifeDynamicObject* parent = objects().object(id);
        VERIFY(parent);
        dynamic_object->m_tGraphID = parent->m_tGraphID;
        dynamic_object->o_Position = parent->o_Position;
        dynamic_object->m_tNodeID = parent->m_tNodeID;
        dynamic_object->ID_Parent = 0xffff;
        register_object(dynamic_object, true);
        dynamic_object->ID_Parent = id;
    }
    else
        register_object(dynamic_object, true);
}

void CALifeSimulatorBase::release(CSE_Abstract* abstract, bool alife_query)
{
#ifdef DEBUG
    if (psAI_Flags.test(aiALife))
    {
        Msg("[LSS] Releasing object [%s][%s][%d][%x]", abstract->name_replace(), *abstract->s_name, abstract->ID, smart_cast<void*>(abstract));
    }
#endif
    CSE_ALifeDynamicObject* object = objects().object(abstract->ID);
    VERIFY(object);

    if (!object->children.empty())
    {
        u32 children_count = object->children.size();
        u32 bytes = children_count * sizeof(ALife::_OBJECT_ID);
        ALife::_OBJECT_ID* children = (ALife::_OBJECT_ID*)_alloca(bytes);
        CopyMemory(children, &*object->children.begin(), bytes);

        ALife::_OBJECT_ID* I = children;
        ALife::_OBJECT_ID* E = children + children_count;
        for (; I != E; ++I)
        {
            CSE_ALifeDynamicObject* child = objects().object(*I, true);
            if (!child)
                continue;

            release(child, alife_query);
        }
    }

    unregister_object(object, alife_query);

    object->m_bALifeControl = false;

    if (alife_query)
        server().entity_Destroy(abstract);
}

void CALifeSimulatorBase::append_item_vector(OBJECT_VECTOR& tObjectVector, ITEM_P_VECTOR& tItemList)
{
    OBJECT_IT I = tObjectVector.begin();
    OBJECT_IT E = tObjectVector.end();
    for (; I != E; ++I)
    {
        CSE_ALifeInventoryItem* l_tpALifeInventoryItem = smart_cast<CSE_ALifeInventoryItem*>(objects().object(*I));
        if (l_tpALifeInventoryItem)
            tItemList.push_back(l_tpALifeInventoryItem);
    }
}

shared_str CALifeSimulatorBase::level_name() const { return (ai().game_graph().header().level(ai().level_graph().level_id()).name()); }

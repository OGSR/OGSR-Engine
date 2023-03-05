////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_dynamic_object.cpp
//	Created 	: 27.10.2005
//  Modified 	: 27.10.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife dynamic object class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects_ALife.h"
#include "alife_simulator.h"
#include "alife_schedule_registry.h"
#include "alife_graph_registry.h"
#include "alife_object_registry.h"
#include "level_graph.h"
#include "game_level_cross_table.h"
#include "game_graph.h"
#include "xrServer.h"

void CSE_ALifeDynamicObject::on_spawn()
{
#ifdef DEBUG
//	Msg			("[LSS] spawning object [%d][%d][%s][%s]",ID,ID_Parent,name(),name_replace());
#endif
}

void CSE_ALifeDynamicObject::on_register()
{
    CSE_ALifeObject* object = this;
    while (object->ID_Parent != ALife::_OBJECT_ID(-1))
    {
        object = ai().alife().objects().object(object->ID_Parent);
        VERIFY(object);
    }

    if (!alife().graph().level().object(object->ID, true) && !keep_saved_data_anyway())
        client_data.clear();
}

void CSE_ALifeDynamicObject::on_before_register() {}

#include "level.h"
#include "map_manager.h"

void CSE_ALifeDynamicObject::on_unregister() { Level().MapManager().RemoveMapLocationByObjectID(ID); }

void CSE_ALifeDynamicObject::switch_online()
{
    R_ASSERT(!m_bOnline);
    m_bOnline = true;
    alife().add_online(this);
}

void CSE_ALifeDynamicObject::switch_offline()
{
    R_ASSERT(m_bOnline);
    m_bOnline = false;
    alife().remove_online(this);
#ifdef DEBUG
    if (!client_data.empty())
        Msg("CSE_ALifeDynamicObject::switch_offline: client_data is cleared for [%d][%s]", ID, name_replace());
#endif // DEBUG
    if (!keep_saved_data_anyway())
        client_data.clear();
}

void CSE_ALifeDynamicObject::add_online(const bool& update_registries)
{
    if (!update_registries)
        return;

    alife().scheduled().remove(this);
    alife().graph().remove(this, m_tGraphID, false);
}

void CSE_ALifeDynamicObject::add_offline(const xr_vector<ALife::_OBJECT_ID>& saved_children, const bool& update_registries)
{
    if (!update_registries)
        return;

    alife().scheduled().add(this);
    alife().graph().add(this, m_tGraphID, false);
}

bool CSE_ALifeDynamicObject::synchronize_location()
{
    if (!ai().level_graph().valid_vertex_position(o_Position) || ai().level_graph().inside(ai().level_graph().vertex(m_tNodeID), o_Position))
        return (true);

    m_tNodeID = ai().level_graph().vertex(m_tNodeID, o_Position);

    GameGraph::_GRAPH_ID tGraphID = ai().cross_table().vertex(m_tNodeID).game_vertex_id();
    if (tGraphID != m_tGraphID)
    {
        if (!m_bOnline)
        {
            Fvector position = o_Position;
            u32 level_vertex_id = m_tNodeID;
            alife().graph().change(this, m_tGraphID, tGraphID);
            if (ai().level_graph().inside(ai().level_graph().vertex(level_vertex_id), position))
            {
                level_vertex_id = m_tNodeID;
                o_Position = position;
            }
        }
        else
        {
            VERIFY(ai().game_graph().vertex(tGraphID)->level_id() == alife().graph().level().level_id());
            m_tGraphID = tGraphID;
        }
    }

    m_fDistance = ai().cross_table().vertex(m_tNodeID).distance();

    return (true);
}

void CSE_ALifeDynamicObject::try_switch_online()
{
    CSE_ALifeSchedulable* schedulable = smart_cast<CSE_ALifeSchedulable*>(this);
    // checking if the abstract monster has just died
    if (schedulable)
    {
        if (!schedulable->need_update(this))
        {
            if (alife().scheduled().object(ID, true))
                alife().scheduled().remove(this);
        }
        else if (!alife().scheduled().object(ID, true))
            alife().scheduled().add(this);
    }

    if (!can_switch_online())
    {
#ifdef DEBUG
        if (!client_data.empty())
            Msg("CSE_ALifeDynamicObject::try_switch_online: client_data is cleared for [%d][%s]", ID, name_replace());
#endif // DEBUG
        if (!keep_saved_data_anyway())
            client_data.clear();
        return;
    }

    if (!can_switch_offline())
    {
        alife().switch_online(this);
        return;
    }

    if (alife().graph().actor()->o_Position.distance_to(o_Position) > alife().online_distance())
    {
#ifdef DEBUG
        if (!client_data.empty())
            Msg("CSE_ALifeDynamicObject::try_switch_online2: client_data is cleared for [%d][%s]", ID, name_replace());
#endif // DEBUG
        if (!keep_saved_data_anyway())
            client_data.clear();
        return;
    }

    alife().switch_online(this);
}

void CSE_ALifeDynamicObject::try_switch_offline()
{
    if (!can_switch_offline())
        return;

    if (!can_switch_online())
    {
        alife().switch_offline(this);
        return;
    }

    if (alife().graph().actor()->o_Position.distance_to(o_Position) <= alife().offline_distance())
        return;

    alife().switch_offline(this);
}

bool CSE_ALifeDynamicObject::redundant() const { return (false); }

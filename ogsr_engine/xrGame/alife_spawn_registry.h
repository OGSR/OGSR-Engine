////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_spawn_registry.h
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife spawn registry
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_spawn_registry_header.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "game_graph.h"
#include "graph_abstract.h"
#include "server_entity_wrapper.h"
#include "ai_debug.h"

class CServerEntityWrapper;
class CGameGraph;

class CALifeSpawnRegistry
{
public:
    typedef CGameGraph::LEVEL_POINT_VECTOR ARTEFACT_SPAWNS;
    typedef CGraphAbstractSerialize<CServerEntityWrapper*, float, ALife::_SPAWN_ID> SPAWN_GRAPH;

public:
    typedef xr_vector<ALife::_SPAWN_ID> SPAWN_IDS;

private:
    CALifeSpawnHeader m_header{};
    SPAWN_GRAPH m_spawns;
    //ARTEFACT_SPAWNS m_artefact_spawn_positions;
    shared_str m_spawn_name;
    SPAWN_IDS m_spawn_roots;
    SPAWN_IDS m_temp0;
    SPAWN_IDS m_temp1;

    xr_unordered_map<ALife::_SPAWN_STORY_ID, ALife::_SPAWN_ID> m_spawn_story_ids;
    string_unordered_map<shared_str, ALife::_SPAWN_ID> m_spawn_ids_by_name;

private:
    IReader* m_file;
    IReader* m_spawn_chunk;
    CGameGraph* m_game_graph;
    bool m_separated_graphs;

protected:
    // void							save_updates				(IWriter &stream);
    // void							load_updates				(IReader &stream);
    void build_story_spawns();
    void build_root_spawns();
    void fill_new_spawns_single(SPAWN_GRAPH::CVertex* vertex, xr_vector<ALife::_SPAWN_ID>& spawns, ALife::_TIME_ID game_time);
    void fill_new_spawns(SPAWN_GRAPH::CVertex* vertex, xr_vector<ALife::_SPAWN_ID>& spawns, ALife::_TIME_ID game_time);
    IC void process_spawns(xr_vector<ALife::_SPAWN_ID>& spawns);
    IC bool enabled_spawn(CSE_Abstract& abstract) const;
    IC bool count_limit(CSE_Abstract& abstract) const;
    IC bool time_limit(CSE_Abstract& abstract, ALife::_TIME_ID game_time) const;
    IC bool spawned_item(SPAWN_GRAPH::CVertex* vertex);
    IC bool object_existance_limit(CSE_Abstract& abstract) const;
    IC bool can_spawn(CSE_Abstract& abstract, ALife::_TIME_ID game_time) const;

public:
    CALifeSpawnRegistry(LPCSTR section);
    virtual ~CALifeSpawnRegistry();
    virtual void load(IReader& file_stream, xrGUID* save_guid = 0);
    virtual void save(IWriter& memory_stream);
    void load(IReader& file_stream, LPCSTR game_name);
    void load(LPCSTR spawn_name);
    void fill_new_spawns(xr_vector<ALife::_SPAWN_ID>& spawns, ALife::_TIME_ID game_time);
    IC const CALifeSpawnHeader& header() const;
    IC const SPAWN_GRAPH& spawns() const;
    shared_str const& get_spawn_name() const { return m_spawn_name; }
    IReader* get_spawn_file() const { return m_file; }
    //IC void assign_artefact_position(CSE_ALifeAnomalousZone* anomaly, CSE_ALifeDynamicObject* object) const;

    //Используется только в луа
    ALife::_SPAWN_ID spawn_id(const ALife::_SPAWN_STORY_ID& spawn_story_id) const;
    //Используется только в луа
    ALife::_SPAWN_ID spawn_id(const char* obj_name) const;
};

#include "alife_spawn_registry_inline.h"
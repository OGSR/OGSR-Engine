////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path_storage.cpp
//	Created 	: 15.06.2004
//  Modified 	: 15.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Patrol path storage
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "patrol_path_storage.h"
#include "patrol_path.h"
#include "patrol_point.h"
#include "levelgamedef.h"
#include "ai_space.h"
#include "level_graph.h"
#include "game_graph.h"

CPatrolPathStorage::~CPatrolPathStorage() { delete_data(m_registry); }

void CPatrolPathStorage::load_raw(const CLevelGraph* level_graph, const CGameLevelCrossTable* cross, const CGameGraph* game_graph, IReader& stream)
{
    IReader* chunk = stream.open_chunk(WAY_PATROLPATH_CHUNK);

    if (!chunk)
        return;

    u32 chunk_iterator;
    for (IReader* sub_chunk = chunk->open_chunk_iterator(chunk_iterator); sub_chunk; sub_chunk = chunk->open_chunk_iterator(chunk_iterator, sub_chunk))
    {
        R_ASSERT(sub_chunk->find_chunk(WAYOBJECT_CHUNK_VERSION));
        R_ASSERT(sub_chunk->r_u16() == WAYOBJECT_VERSION);
        R_ASSERT(sub_chunk->find_chunk(WAYOBJECT_CHUNK_NAME));

        shared_str patrol_name;
        sub_chunk->r_stringZ(patrol_name);
        VERIFY3(m_registry.find(patrol_name) == m_registry.end(), "Duplicated patrol path found", *patrol_name);
        m_registry.insert(std::make_pair(patrol_name, &xr_new<CPatrolPath>(patrol_name)->load_raw(level_graph, cross, game_graph, *sub_chunk)));
    }

    chunk->close();
}

void CPatrolPathStorage::append_from_ini(CInifile& way_inifile)
{
    PATROL_REGISTRY::value_type pair;

    int i = 0;
    int r = 0;
    for (const auto& it : way_inifile.sections())
    {
        const shared_str patrol_name = it.first;

        if (m_registry.erase(patrol_name))
        {
            r++;
        }
        if (it.second->line_count() == 0)
            continue;

        m_registry.insert(std::make_pair(patrol_name, &xr_new<CPatrolPath>(patrol_name)->load_ini(*it.second)));

        i++;
    }

    Msg("Loaded %d items from custom_waypoints, %d from all.spawn was replaced!", i, r);
}

void CPatrolPathStorage::load(IReader& stream)
{
    IReader* chunk;

    chunk = stream.open_chunk(0);
    u32 size = chunk->r_u32();
    chunk->close();

    m_registry.clear();

    PATROL_REGISTRY::value_type pair;

    chunk = stream.open_chunk(1);
    for (u32 i = 0; i < size; ++i)
    {
        IReader* chunk1;
        chunk1 = chunk->open_chunk(i);

        IReader* chunk2;
        chunk2 = chunk1->open_chunk(0);
        load_data(pair.first, *chunk2);
        chunk2->close();

        chunk2 = chunk1->open_chunk(1);
        load_data(pair.second, *chunk2);
        chunk2->close();

        chunk1->close();

        VERIFY3(m_registry.find(pair.first) == m_registry.end(), "Duplicated patrol path found ", *pair.first);

#ifdef DEBUG
        pair.second->name(pair.first);
#endif

        m_registry.insert(pair);
    }

    chunk->close();
}

void CPatrolPathStorage::save(IWriter& stream)
{
    stream.open_chunk(0);
    stream.w_u32(m_registry.size());
    stream.close_chunk();

    stream.open_chunk(1);

    PATROL_REGISTRY::iterator I = m_registry.begin();
    PATROL_REGISTRY::iterator E = m_registry.end();
    for (int i = 0; I != E; ++I, ++i)
    {
        stream.open_chunk(i);

        stream.open_chunk(0);
        save_data((*I).first, stream);
        stream.close_chunk();

        stream.open_chunk(1);
        save_data((*I).second, stream);
        stream.close_chunk();

        stream.close_chunk();
    }

    stream.close_chunk();
}

void CPatrolPathStorage::remove_path(shared_str patrol_name) { m_registry.erase(patrol_name); }

void CPatrolPathStorage::add_path(shared_str patrol_name, CPatrolPath* path)
{
    remove_path(patrol_name);
    m_registry.insert(std::make_pair(patrol_name, path));
}

const CPatrolPath* CPatrolPathStorage::safe_path(shared_str patrol_name, bool no_assert, bool on_level) const
{
    auto it = m_registry.find(patrol_name);
    if (it == m_registry.end())
        return path(patrol_name, no_assert);

    for (auto& it2 : (*it).second->vertices())
    {
        auto& pp = it2.second->data();
        if (on_level || (ai().game_graph().valid_vertex_id(pp.m_game_vertex_id) && ai().game_graph().vertex(pp.m_game_vertex_id)->level_id() == ai().level_graph().level_id()))
        {
            if (!ai().level_graph().valid_vertex_id(pp.m_level_vertex_id))
            {
                u32 prev_vertex_id = pp.m_level_vertex_id;
                pp.m_level_vertex_id = ai().level_graph().vertex(pp.m_position);
                Msg("* [%s]: path[%s] pp[%s] level_vertex_id[%u] -> %u", __FUNCTION__, patrol_name.c_str(), pp.m_name.c_str(), prev_vertex_id, pp.m_level_vertex_id);
            }
        }
        else
            return path(patrol_name, no_assert);
    }

    return path(patrol_name, no_assert);
}

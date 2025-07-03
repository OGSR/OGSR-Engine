////////////////////////////////////////////////////////////////////////////
//	Module 		: saved_game_wrapper.cpp
//	Created 	: 21.02.2006
//  Modified 	: 21.02.2006
//	Author		: Dmitriy Iassenev
//	Description : saved game wrapper class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <zstd.h>

#include "saved_game_wrapper.h"
#include "alife_time_manager.h"
#include "alife_object_registry.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "ai_space.h"
#include "game_graph.h"
#include "alife_simulator_header.h"
#include "alife_simulator.h"
#include "alife_spawn_registry.h"
#include "string_table.h"

extern LPCSTR alife_section;

LPCSTR CSavedGameWrapper::saved_game_full_name(LPCSTR saved_game_name, string_path& result)
{
    string_path temp;
    strconcat(sizeof(temp), temp, saved_game_name, SAVE_EXTENSION);
    FS.update_path(result, "$game_saves$", temp);
    return (result);
}

bool CSavedGameWrapper::saved_game_exist(LPCSTR saved_game_name)
{
    string_path file_name;
    return (!!FS.exist(saved_game_full_name(saved_game_name, file_name)));
}

bool CSavedGameWrapper::valid_saved_game(IReader& stream, u32& magic)
{
    if (stream.length() < 8)
        return (false);

    magic = stream.r_u32();
    if (magic != u32(-1) && magic != u32(0))
        return (false);

    if (stream.r_u32() < 0x0002)
        return (false);

    return (true);
}

bool CSavedGameWrapper::valid_saved_game(LPCSTR saved_game_name)
{
    if (!saved_game_name || strlen(saved_game_name) == 0)
        return false;

    string_path file_name;
    if (!FS.exist(saved_game_full_name(saved_game_name, file_name)))
        return (false);

    u32 magic = 0;

    IReader* stream = FS.r_open(file_name);
    const bool result = valid_saved_game(*stream, magic);
    FS.r_close(stream);
    return (result);
}

CSavedGameWrapper::CSavedGameWrapper(LPCSTR saved_game_name)
{
    string_path file_name;
    saved_game_full_name(saved_game_name, file_name);
    R_ASSERT3(FS.exist(file_name), "There is no saved game ", file_name);

    u32 magic = 0;

    IReader* stream = FS.r_open(file_name);
    if (!valid_saved_game(*stream, magic))
    {
        FS.r_close(stream);
        CALifeTimeManager time_manager(alife_section);
        m_game_time = time_manager.game_time();
        m_actor_health = 1.f;
        m_level_id = _LEVEL_ID(-1);
        m_level_name = "";
        return;
    }

    u32 source_count = stream->r_u32();
    void* source_data = xr_malloc(source_count);
    if (magic == 0)
        R_ASSERT(ZSTD_decompress(source_data, source_count, stream->pointer(), stream->length() - 3 * sizeof(u32)) == source_count);
    else
        rtc_decompress(source_data, source_count, stream->pointer(), stream->length() - 3 * sizeof(u32));
    FS.r_close(stream);

    IReader reader(source_data, source_count);

    {
        CALifeTimeManager time_manager(alife_section);
        time_manager.load(reader);
        m_game_time = time_manager.game_time();
    }

    {
        R_ASSERT2(reader.find_chunk(OBJECT_CHUNK_DATA), "Can't find chunk OBJECT_CHUNK_DATA!");
#ifdef DEBUG
        u32 count =
#endif
            reader.r_u32();
        VERIFY(count > 0);
        CSE_ALifeDynamicObject* object = CALifeObjectRegistry::get_object(reader);
        VERIFY(object->ID == 0);
        CSE_ALifeCreatureActor* actor = smart_cast<CSE_ALifeCreatureActor*>(object);
        VERIFY(actor);

        m_actor_health = actor->g_Health();

        IReader* R = reader.open_chunk(SPAWN_CHUNK_DATA);
        R_ASSERT(R, "Spawn version mismatch - REBUILD SPAWN!");

        string_path spawn_file_name;
        {
            IReader* sub_chunk = R->open_chunk(0);
            if (!sub_chunk)
            {
                R->close();
                F_entity_Destroy(object);
                m_level_id = _LEVEL_ID(-1);
                m_level_name = "";
                return;
            }
            sub_chunk->r_stringZ(spawn_file_name, sizeof(spawn_file_name));
            sub_chunk->close();
        }
        R->close();

        if (!FS.exist(file_name, "$game_spawn$", spawn_file_name, ".spawn"))
        {
            F_entity_Destroy(object);
            m_level_id = _LEVEL_ID(-1);
            m_level_name = "";
            return;
        }

        IReader* spawn = nullptr;
        bool b_destroy_spawn = true;
        if (ai().get_alife() && ai().alife().spawns().get_spawn_name() == spawn_file_name)
        {
            spawn = ai().alife().spawns().get_spawn_file();
            b_destroy_spawn = false;
        }
        else
            spawn = FS.r_open(file_name);

        if (!spawn)
        {
            F_entity_Destroy(object);
            m_level_id = _LEVEL_ID(-1);
            m_level_name = "";
            return;
        }

        bool separated_graphs = false;
        R = spawn->open_chunk(4);
        if (!R)
        {
            string_path graph_path;
            FS.update_path(graph_path, fsgame::game_data, GRAPH_NAME);
            R = FS.r_open(graph_path);
            separated_graphs = true;
        }

        if (!R)
        {
            F_entity_Destroy(object);
            if (b_destroy_spawn)
                FS.r_close(spawn);
            m_level_id = _LEVEL_ID(-1);
            m_level_name = "";
            return;
        }

        {
            CGameGraph graph(R, separated_graphs);
            m_level_id = graph.vertex(object->m_tGraphID)->level_id();
            if (graph.header().level_exist(m_level_id))
                m_level_name = graph.header().level(m_level_id).name();
            else
                m_level_name = CStringTable().translate("ui_st_error");
        }

        if (!separated_graphs)
            R->close();
        if (b_destroy_spawn)
            FS.r_close(spawn);
        F_entity_Destroy(object);
    }

    xr_free(source_data);
}

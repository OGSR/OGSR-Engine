////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_space.h
//	Created 	: 12.11.2003
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : AI space class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "game_graph.h"
#include "game_level_cross_table.h"
#include "level_graph.h"
#include "graph_engine.h"
#include "ef_storage.h"
#include "ai_space.h"
#include "cover_manager.h"
#include "cover_point.h"
#include "script_engine.h"
#include "patrol_path_storage.h"
#include "alife_simulator.h"

CAI_Space* g_ai_space = 0;

CAI_Space::CAI_Space()
{
    m_ef_storage = 0;
    m_game_graph = 0;
    m_graph_engine = 0;
    m_cover_manager = 0;
    m_level_graph = 0;
    m_alife_simulator = 0;
    m_patrol_path_storage = 0;
    m_script_engine = 0;
}

void CAI_Space::init()
{
    VERIFY(!m_ef_storage);
    m_ef_storage = xr_new<CEF_Storage>();

    VERIFY(!m_graph_engine);
    m_graph_engine = xr_new<CGraphEngine>(1024); // game_graph().header().vertex_count() ???

    VERIFY(!m_cover_manager);
    m_cover_manager = xr_new<CCoverManager>();

    VERIFY(!m_patrol_path_storage);
    m_patrol_path_storage = xr_new<CPatrolPathStorage>();

    VERIFY(!m_script_engine);
    m_script_engine = xr_new<CScriptEngine>();
    script_engine().init();
}

CAI_Space::~CAI_Space()
{
    unload();

    xr_delete(m_patrol_path_storage);
    xr_delete(m_ef_storage);
    xr_delete(m_graph_engine);

    VERIFY(!m_game_graph);

    xr_delete(m_script_engine);

    xr_delete(m_cover_manager);
}

void CAI_Space::load(LPCSTR level_name)
{
    VERIFY(m_game_graph);

    unload(true);

#ifdef DEBUG
    Memory.mem_compact();
    u32 mem_usage = Memory.mem_usage();
    CTimer timer;
    timer.Start();
#endif

    const CGameGraph::SLevel& current_level = game_graph().header().level(level_name);

    m_level_graph = xr_new<CLevelGraph>();
    game_graph().set_current_level(current_level.id());
    auto& crossHeader = cross_table().header();
    auto& levelHeader = level_graph().header();
    auto& gameHeader = game_graph().header();
    R_ASSERT(crossHeader.level_guid() == levelHeader.guid(), "cross_table doesn't correspond to the AI-map");
    R_ASSERT(crossHeader.game_guid() == gameHeader.guid(), "graph doesn't correspond to the cross table");

    u32 vertexCount = _max(gameHeader.vertex_count(), levelHeader.vertex_count());
    m_graph_engine = xr_new<CGraphEngine>(vertexCount);
    R_ASSERT(current_level.guid() == levelHeader.guid(), "graph doesn't correspond to the AI-map");

#ifdef DEBUG
    if (!xr_strcmp(current_level.name(), level_name))
        validate(current_level.id());
#endif

    level_graph().level_id(current_level.id());

    m_cover_manager->compute_static_cover();

#ifdef DEBUG
    Msg("* Loading ai space is successfully completed (%.3fs, %7.3f Mb)", timer.GetElapsed_sec(), float(Memory.mem_usage() - mem_usage) / 1048576.0);
#endif
}

void CAI_Space::unload(bool reload)
{
    script_engine().unload();
    xr_delete(m_graph_engine);
    xr_delete(m_level_graph);
    
    if (!reload && m_game_graph)
        m_graph_engine = xr_new<CGraphEngine>(game_graph().header().vertex_count());
}

#ifdef DEBUG
void CAI_Space::validate(const u32 level_id) const
{
    VERIFY(level_graph().header().vertex_count() == cross_table().header().level_vertex_count());
    for (GameGraph::_GRAPH_ID i = 0, n = game_graph().header().vertex_count(); i < n; ++i)
        if ((level_id == game_graph().vertex(i)->level_id()) &&
            (!level_graph().valid_vertex_id(game_graph().vertex(i)->level_vertex_id()) || (cross_table().vertex(game_graph().vertex(i)->level_vertex_id()).game_vertex_id() != i) ||
             !level_graph().inside(game_graph().vertex(i)->level_vertex_id(), game_graph().vertex(i)->level_point())))
        {
            Msg("! Graph doesn't correspond to the cross table");
            R_ASSERT2(false, "Graph doesn't correspond to the cross table");
        }

    //	Msg						("death graph point id : %d",cross_table().vertex(455236).game_vertex_id());

    for (u32 i = 0, n = game_graph().header().vertex_count(); i < n; ++i)
    {
        if (level_id != game_graph().vertex(i)->level_id())
            continue;

        CGameGraph::const_spawn_iterator I, E;
        game_graph().begin_spawn(i, I, E);
        //		Msg									("vertex [%d] has %d death points",i,game_graph().vertex(i)->death_point_count());
        for (; I != E; ++I)
        {
            VERIFY(cross_table().vertex((*I).level_vertex_id()).game_vertex_id() == i);
        }
    }

    //	Msg						("* Graph corresponds to the cross table");
}
#endif

void CAI_Space::patrol_path_storage(IReader& stream)
{
    xr_delete(m_patrol_path_storage);
    m_patrol_path_storage = xr_new<CPatrolPathStorage>();
    m_patrol_path_storage->load(stream);
}

void CAI_Space::patrol_path_storage_ini(CInifile& way_inifile) { m_patrol_path_storage->append_from_ini(way_inifile); }

void CAI_Space::set_alife(CALifeSimulator* alife_simulator)
{
    VERIFY((!m_alife_simulator && alife_simulator) || (m_alife_simulator && !alife_simulator));
    m_alife_simulator = alife_simulator;

    VERIFY(!alife_simulator || !m_game_graph);
    if (alife_simulator)
        return;
    set_game_graph(nullptr);
}
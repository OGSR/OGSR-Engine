////////////////////////////////////////////////////////////////////////////
//	Module 		: game_graph_inline.h
//	Created 	: 18.02.2003
//  Modified 	: 13.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Game graph inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC CGameGraph::CGameGraph(IReader* stream, bool separatedGraphs)
{
    m_reader = stream;
    m_separated_graphs = separatedGraphs;
    VERIFY(m_reader);
    m_header.load(m_reader);
    R_ASSERT2(header().version() == XRAI_CURRENT_VERSION, "Graph version mismatch!");
    m_nodes = (CVertex*)m_reader->pointer();
    m_current_level_some_vertex_id = _GRAPH_ID(-1);
    m_enabled.assign(header().vertex_count(), true);

	if (m_separated_graphs)
    {
        m_cross_tables = nullptr;
        m_current_level_cross_table = nullptr;
    }
    else
    {
        u8* temp = (u8*)(m_nodes + header().vertex_count());
        temp += header().edge_count() * sizeof(CGameGraph::CEdge);
        m_cross_tables = (u32*)(((CLevelPoint*)temp) + header().death_point_count());
        m_current_level_cross_table = nullptr;
    }
}

IC CGameGraph::~CGameGraph()
{
    xr_delete(m_current_level_cross_table);
    if (m_separated_graphs)
        FS.r_close(m_reader);
}

IC const CGameGraph::CHeader& CGameGraph::header() const { return (m_header); }

IC bool CGameGraph::mask(const svector<_LOCATION_ID, GameGraph::LOCATION_TYPE_COUNT>& M, const _LOCATION_ID E[GameGraph::LOCATION_TYPE_COUNT]) const
{
    for (int i = 0; i < GameGraph::LOCATION_TYPE_COUNT; ++i)
        if ((M[i] != E[i]) && (255 != M[i]))
            return (false);
    return (true);
}

IC bool CGameGraph::mask(const _LOCATION_ID M[GameGraph::LOCATION_TYPE_COUNT], const _LOCATION_ID E[GameGraph::LOCATION_TYPE_COUNT]) const
{
    for (int i = 0; i < GameGraph::LOCATION_TYPE_COUNT; ++i)
        if ((M[i] != E[i]) && (255 != M[i]))
            return (false);
    return (true);
}

IC float CGameGraph::distance(const _GRAPH_ID tGraphID0, const _GRAPH_ID tGraphID1) const
{
    const_iterator i, e;
    begin(tGraphID0, i, e);
    for (; i != e; ++i)
        if (value(tGraphID0, i) == tGraphID1)
            return (edge_weight(i));
    //#pragma todo("KD: не всегда построенный путь для монстра валидный. Пока вывожу в лог и сбрасываю путь в вызвавшей функции.")
    //	R_ASSERT2					(false,"There is no proper graph point neighbour!");
    Msg("!![%s] There is no way to get distance from vertex [%u] to vertex [%u]!!!", __FUNCTION__, tGraphID0, tGraphID1);
    return (_GRAPH_ID(-1));
}

IC bool CGameGraph::accessible(const u32& vertex_id) const
{
    VERIFY(valid_vertex_id(vertex_id));
    return valid_vertex_id(vertex_id) && m_enabled[vertex_id];
}

IC void CGameGraph::accessible(const u32& vertex_id, bool value) const
{
#ifdef CRASH_ON_INVALID_VERTEX_ID
    ASSERT_FMT(valid_vertex_id(vertex_id), "invalid vertex_id %u", vertex_id);
#else
    VERIFY(valid_vertex_id(vertex_id));
#endif
    m_enabled[vertex_id] = value;
}

IC bool CGameGraph::valid_vertex_id(const u32& vertex_id) const { return (vertex_id < header().vertex_count()); }

IC void CGameGraph::begin(const u32& vertex_id, const_iterator& start, const_iterator& end) const
{
    end = (start = (const CEdge*)((BYTE*)m_nodes + vertex(_GRAPH_ID(vertex_id))->edge_offset())) + vertex(_GRAPH_ID(vertex_id))->edge_count();
}

IC const CGameGraph::_GRAPH_ID& CGameGraph::value(const u32& vertex_id, const_iterator& i) const { return (i->vertex_id()); }

IC const float& CGameGraph::edge_weight(const_iterator i) const { return (i->distance()); }

IC const CGameGraph::CVertex* CGameGraph::vertex(const u32& vertex_id) const
{
#ifdef CRASH_ON_INVALID_VERTEX_ID
    ASSERT_FMT(valid_vertex_id(vertex_id), "invalid vertex_id %u", vertex_id);
    return (m_nodes + vertex_id);
#else
#if _M_X64
    return (valid_vertex_id(vertex_id)) ? (m_nodes + vertex_id) : (m_nodes + header().vertex_count() - 1);
#else
    return (m_nodes + vertex_id);
#endif
#endif
}

IC const u8& CGameGraph::CHeader::version() const { return (m_version); }

IC GameGraph::_LEVEL_ID GameGraph::CHeader::level_count() const
{
    VERIFY(m_levels.size() < (u32(1) << (8 * sizeof(GameGraph::_LEVEL_ID))));
    return ((GameGraph::_LEVEL_ID)m_levels.size());
}

IC const GameGraph::_GRAPH_ID& GameGraph::CHeader::vertex_count() const { return (m_vertex_count); }

IC const u32& GameGraph::CHeader::edge_count() const { return (m_edge_count); }

IC const u32& GameGraph::CHeader::death_point_count() const { return (m_death_point_count); }

IC const GameGraph::LEVEL_MAP& GameGraph::CHeader::levels() const { return (m_levels); }

IC bool GameGraph::CHeader::level_exist(const _LEVEL_ID& id) const { return levels().find(id) != levels().end(); }

IC bool GameGraph::CHeader::level_exist(pcstr level_name) const
{
    for (const auto& levelPair : levels())
        if (xr_strcmp(levelPair.second.name(), level_name) == 0)
            return true;
    return false;
}

IC const GameGraph::SLevel& GameGraph::CHeader::level(const _LEVEL_ID& id) const
{
    LEVEL_MAP::const_iterator I = levels().find(id);
    R_ASSERT2(I != levels().end(), make_string("there is no specified level in the game graph : %d", id));
    return ((*I).second);
}

IC const GameGraph::SLevel& GameGraph::CHeader::level(LPCSTR level_name) const
{
    LEVEL_MAP::const_iterator I = levels().begin();
    LEVEL_MAP::const_iterator E = levels().end();
    for (; I != E; ++I)
        if (!xr_strcmp((*I).second.name(), level_name))
            return ((*I).second);

#ifdef DEBUG
    Msg("! There is no specified level %s in the game graph!", level_name);
    return (levels().begin()->second);
#else
    R_ASSERT3(false, "There is no specified level in the game graph!", level_name);
    NODEFAULT;
#endif
}

IC const GameGraph::SLevel* GameGraph::CHeader::level(LPCSTR level_name, bool) const
{
    LEVEL_MAP::const_iterator I = levels().begin();
    LEVEL_MAP::const_iterator E = levels().end();
    for (; I != E; ++I)
        if (!xr_strcmp((*I).second.name(), level_name))
            return (&(*I).second);

    return (0);
}

IC const xrGUID& CGameGraph::CHeader::guid() const { return (m_guid); }

IC const Fvector& GameGraph::CVertex::level_point() const { return (tLocalPoint); }

IC const Fvector& GameGraph::CVertex::game_point() const { return (tGlobalPoint); }

IC GameGraph::_LEVEL_ID GameGraph::CVertex::level_id() const { return (tLevelID); }

IC u32 GameGraph::CVertex::level_vertex_id() const { return (tNodeID); }

IC const u8* GameGraph::CVertex::vertex_type() const { return (tVertexTypes); }

IC const u8& GameGraph::CVertex::edge_count() const { return (tNeighbourCount); }

IC const u8& GameGraph::CVertex::death_point_count() const { return (tDeathPointCount); }

IC const u32& GameGraph::CVertex::edge_offset() const { return (dwEdgeOffset); }

IC const u32& GameGraph::CVertex::death_point_offset() const { return (dwPointOffset); }

IC const GameGraph::_GRAPH_ID& GameGraph::CEdge::vertex_id() const { return (m_vertex_id); }

IC const float& GameGraph::CEdge::distance() const { return (m_path_distance); }

IC void CGameGraph::begin_spawn(const u32& vertex_id, const_spawn_iterator& start, const_spawn_iterator& end) const
{
    const CVertex* object = vertex(vertex_id);
    start = (const_spawn_iterator)((u8*)m_nodes + object->death_point_offset());
    end = start + object->death_point_count();
}

IC void CGameGraph::set_invalid_vertex(_GRAPH_ID& vertex_id) const
{
    vertex_id = _GRAPH_ID(-1);
    VERIFY(!valid_vertex_id(vertex_id));
}

IC GameGraph::_GRAPH_ID CGameGraph::vertex_id(const CGameGraph::CVertex* vertex) const
{
    VERIFY(valid_vertex_id(_GRAPH_ID(vertex - m_nodes)));
    return (_GRAPH_ID(vertex - m_nodes));
}

IC const GameGraph::_GRAPH_ID& CGameGraph::current_level_vertex() const
{
    VERIFY(valid_vertex_id(m_current_level_some_vertex_id));
    return (m_current_level_some_vertex_id);
}

IC void GameGraph::SLevel::load(IReader* reader)
{
    reader->r_stringZ(m_name);
    reader->r_fvector3(m_offset);
    reader->r(&m_id, sizeof(m_id));
    reader->r_stringZ(m_section);
    reader->r(&m_guid, sizeof(m_guid));
}

IC void GameGraph::SLevel::save(IWriter* writer)
{
    writer->w_stringZ(m_name);
    writer->w_fvector3(m_offset);
    writer->w(&m_id, sizeof(m_id));
    writer->w_stringZ(m_section);
    writer->w(&m_guid, sizeof(m_guid));
}

IC void GameGraph::CHeader::load(IReader* reader)
{
    reader->r(&m_version, sizeof(m_version));
    reader->r(&m_vertex_count, sizeof(m_vertex_count));
    reader->r(&m_edge_count, sizeof(m_edge_count));
    reader->r(&m_death_point_count, sizeof(m_death_point_count));
    reader->r(&m_guid, sizeof(m_guid));

    u32 level_count = reader->r_u8();

    m_levels.clear();
    for (u32 i = 0; i < level_count; ++i)
    {
        SLevel l_tLevel;
        l_tLevel.load(reader);
        m_levels.insert(mk_pair(l_tLevel.id(), l_tLevel));
    }
}

IC void GameGraph::CHeader::save(IWriter* writer)
{
    writer->w(&m_version, sizeof(m_version));
    writer->w(&m_vertex_count, sizeof(m_vertex_count));
    writer->w(&m_edge_count, sizeof(m_edge_count));
    writer->w(&m_death_point_count, sizeof(m_death_point_count));
    writer->w(&m_guid, sizeof(m_guid));

    VERIFY(m_levels.size() < u32((1) << (8 * sizeof(u8))));
    writer->w_u8((u8)m_levels.size());

    LEVEL_MAP::iterator I = m_levels.begin();
    LEVEL_MAP::iterator E = m_levels.end();
    for (; I != E; ++I)
        (*I).second.save(writer);
}

IC void CGameGraph::set_current_level(const u32& level_id)
{
    xr_delete(m_current_level_cross_table);
    if (m_cross_tables)
    {
        u32* current_cross_table = m_cross_tables;
        for (const auto& levelPair : header().levels())
        {
            if (level_id != levelPair.first)
            {
                current_cross_table = (u32*)((u8*)current_cross_table + *current_cross_table);
                continue;
            }

            m_current_level_cross_table = xr_new<CGameLevelCrossTable>(current_cross_table + 1, *current_cross_table);
            break;
        }
    }
    else
    {
        string_path fName;
        FS.update_path(fName, "$level$", CROSS_TABLE_NAME);
        m_current_level_cross_table = xr_new<CGameLevelCrossTable>(fName);
    }
    VERIFY(m_current_level_cross_table);
    m_current_level_some_vertex_id = _GRAPH_ID(-1);
    for (_GRAPH_ID i = 0, n = header().vertex_count(); i < n; ++i)
    {
        if (level_id != vertex(i)->level_id())
            continue;

        m_current_level_some_vertex_id = i;
        break;
    }

    VERIFY(valid_vertex_id(m_current_level_some_vertex_id));
}

IC const CGameLevelCrossTable& CGameGraph::cross_table() const
{
    VERIFY(m_current_level_cross_table);
    return (*m_current_level_cross_table);
}
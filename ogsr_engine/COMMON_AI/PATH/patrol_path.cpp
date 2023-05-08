////////////////////////////////////////////////////////////////////////////
//	Module 		: patrol_path.cpp
//	Created 	: 15.06.2004
//  Modified 	: 15.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Patrol path
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "patrol_path.h"
#include "levelgamedef.h"

CPatrolPath::CPatrolPath(shared_str name)
{
#ifdef DEBUG
    m_name = name;
#endif
}

CPatrolPath::~CPatrolPath() {}

CPatrolPath& CPatrolPath::load_raw(const CLevelGraph* level_graph, const CGameLevelCrossTable* cross, const CGameGraph* game_graph, IReader& stream)
{
    R_ASSERT(stream.find_chunk(WAYOBJECT_CHUNK_POINTS));
    u32 vertex_count = stream.r_u16();
    for (u32 i = 0; i < vertex_count; ++i)
        add_vertex(CPatrolPoint(this).load_raw(level_graph, cross, game_graph, stream), i);

    R_ASSERT(stream.find_chunk(WAYOBJECT_CHUNK_LINKS));
    u32 edge_count = stream.r_u16();
    for (u32 i = 0; i < edge_count; ++i)
    {
        u16 vertex0 = stream.r_u16();
        u16 vertex1 = stream.r_u16();
        float probability = stream.r_float();
        add_edge(vertex0, vertex1, probability);
    }

    return (*this);
}

CPatrolPath& CPatrolPath::load_ini(CInifile::Sect& section)
{
    const char* points = section.r_string("points");
    const int vertex_count = _GetItemCount(points);

    string16 prefix;
    for (int i = 0; i < vertex_count; ++i)
    {
        _GetItem(points, i, prefix);

        add_vertex(CPatrolPoint(this).load_ini(section, prefix), i);
    }

    for (int i = 0; i < vertex_count; ++i)
    {
        _GetItem(points, i, prefix);

        string256 full_name;
        strconcat(sizeof(full_name), full_name, prefix, ":", "links");

        if (section.line_exist(full_name))
        {
            const char* links = section.r_string(full_name);
            const int links_count = _GetItemCount(links);

            string32 link;
            for (int k = 0; k < links_count; ++k)
            {
                _GetItem(links, k, link);

                u32 vertex_idx;
                float probability;

                sscanf(link, "p%d(%f)", &vertex_idx, &probability);

                add_edge(i, vertex_idx, probability);
            }
        }
    }

    return (*this);
}

#ifdef DEBUG
void CPatrolPath::load(IReader& stream)
{
    inherited::load(stream);
    vertex_iterator I = vertices().begin();
    vertex_iterator E = vertices().end();
    for (; I != E; ++I)
        (*I).second->data().path(this);
}
#endif

CPatrolPoint CPatrolPath::add_point(Fvector pos)
{
    auto pp = CPatrolPoint(this).position(pos);
    u32 index = vertices().size();
    add_vertex(pp, index);
    return vertex(index)->data();
}

CPatrolPoint CPatrolPath::point(u32 index) { return vertex(index)->data(); }

CPatrolPoint* CPatrolPath::point_raw(u32 index)
{
    return &vertex(index)->data();
}

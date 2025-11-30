#include "stdafx.h"

#include "xrCDB.h"

namespace CDB
{
u32 Collector::VPack(const Fvector& V, float eps)
{
    xr_vector<Fvector>::iterator I, E;
    I = verts.begin();
    E = verts.end();
    for (; I != E; I++)
        if (I->similar(V, eps))
            return u32(I - verts.begin());
    verts.push_back(V);
    return verts.size() - 1;
}

void Collector::add_face_packed_D(const Fvector& v0, const Fvector& v1, const Fvector& v2, // vertices
                                  u32 dummy, float eps)
{
    TRI T;
    T.verts[0] = VPack(v0, eps);
    T.verts[1] = VPack(v1, eps);
    T.verts[2] = VPack(v2, eps);
    T.dummy = dummy;
    faces.push_back(T);
}

#pragma pack(push, 1)
struct edge
{
    u32 face_id;
    u32 edge_id;
    u32 vertex_id0;
    u32 vertex_id1;

    static_assert(std::is_same_v<decltype(vertex_id0), std::remove_all_extents_t<decltype(TRI::verts)>>);
    static_assert(std::is_same_v<decltype(vertex_id1), std::remove_all_extents_t<decltype(TRI::verts)>>);
};
#pragma pack(pop)

void Collector::calc_adjacency(xr_vector<u32>& dest) const
{
    ZoneScoped;

    const size_t edge_count = faces.size() * 3;

    xr_vector<edge> _edges(edge_count);
    edge* edges = _edges.data();

    edge* i = edges;

    const auto B = faces.cbegin(), E = faces.cend();
    auto I = B;
    for (; I != E; ++I)
    {
        const u32 face_id = u32(I - B);

        i->face_id = face_id;
        i->edge_id = 0;
        i->vertex_id0 = I->verts[0];
        i->vertex_id1 = I->verts[1];
        if (i->vertex_id0 > i->vertex_id1)
            std::swap(i->vertex_id0, i->vertex_id1);
        ++i;

        i->face_id = face_id;
        i->edge_id = 1;
        i->vertex_id0 = I->verts[1];
        i->vertex_id1 = I->verts[2];
        if (i->vertex_id0 > i->vertex_id1)
            std::swap(i->vertex_id0, i->vertex_id1);
        ++i;

        i->face_id = face_id;
        i->edge_id = 2;
        i->vertex_id0 = I->verts[2];
        i->vertex_id1 = I->verts[0];
        if (i->vertex_id0 > i->vertex_id1)
            std::swap(i->vertex_id0, i->vertex_id1);
        ++i;
    }

    std::sort(_edges.begin(), _edges.end(), [](const edge& edge0, const edge& edge1) {
        if (edge0.vertex_id0 < edge1.vertex_id0)
            return true;

        if (edge1.vertex_id0 < edge0.vertex_id0)
            return false;

        if (edge0.vertex_id1 < edge1.vertex_id1)
            return true;

        if (edge1.vertex_id1 < edge0.vertex_id1)
            return false;

        return edge0.face_id < edge1.face_id;
    });

    dest.assign(edge_count, u32(-1));

    {
        edge *I = edges, *J;
        edge* E = edges + edge_count;
        for (; I != E; ++I)
        {
            if (I + 1 == E)
                continue;

            J = I + 1;

            if (I->vertex_id0 != J->vertex_id0)
                continue;

            if (I->vertex_id1 != J->vertex_id1)
                continue;

            dest[I->face_id * 3 + I->edge_id] = J->face_id;
            dest[J->face_id * 3 + J->edge_id] = I->face_id;
        }
    }
}

}; // namespace CDB

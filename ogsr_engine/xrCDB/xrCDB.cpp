#include "stdafx.h"

#include "xrCDB.h"

using namespace CDB;
using namespace Opcode;

MODEL::~MODEL()
{
    syncronize(); // maybe model still in building
    status = S_INIT;
    xr_delete(tree);
    xr_free(tris);
    tris_count = 0;
    xr_free(verts);
    verts_count = 0;
}

void MODEL::build(const Fvector* V, const size_t Vcnt, const TRI* T, const size_t Tcnt, build_callback* bc, void* bcp)
{
    R_ASSERT(S_INIT == status);
    R_ASSERT((Vcnt >= 4) && (Tcnt >= 2));

    build_internal(V, Vcnt, T, Tcnt, bc, bcp);
    status = S_READY;
}

void MODEL::build_internal(const Fvector* V, const size_t Vcnt, const TRI* T, const size_t Tcnt, build_callback* bc, void* bcp)
{
    // verts
    verts_count = Vcnt;
    verts = xr_alloc<Fvector>(verts_count);
    std::memcpy(verts, V, verts_count * sizeof(Fvector));

    // tris
    tris_count = Tcnt;
    tris = xr_alloc<TRI>(tris_count);
    std::memcpy(tris, T, tris_count * sizeof(TRI));

    // callback
    if (bc)
        bc(verts, Vcnt, tris, Tcnt, bcp);

    // Release data pointers
    status = S_BUILD;
    MeshInterface* mif = xr_new<MeshInterface>();
    mif->SetNbTriangles(tris_count);
    mif->SetNbVertices(verts_count);
    mif->SetPointers(reinterpret_cast<const IceMaths::IndexedTriangle*>(tris), reinterpret_cast<const IceMaths::Point*>(verts));
    mif->SetStrides(sizeof(TRI));

    // Build a non quantized no-leaf tree
    OPCODECREATE OPCC = OPCODECREATE();
    OPCC.mIMesh = mif;
    OPCC.mQuantized = false;

    tree = xr_new<Model>();

    if (!tree->Build(OPCC))
    {
        xr_free(verts);
        xr_free(tris);
        xr_delete(mif);
        return;
    }

    xr_delete(mif);
}

u32 MODEL::memory()
{
    if (S_BUILD == status)
    {
        Msg("! xrCDB: model still isn't ready");
        return 0;
    }
    const u32 V = verts_count * sizeof(Fvector);
    const u32 T = tris_count * sizeof(TRI);
    return tree->GetUsedBytes() + V + T + sizeof(*this) + sizeof(*tree);
}

RESULT& COLLIDER::r_add() { return rd.emplace_back(); }

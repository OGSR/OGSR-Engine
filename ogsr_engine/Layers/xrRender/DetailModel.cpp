#include "stdafx.h"
#include "detailmodel.h"

constexpr D3DVERTEXELEMENT9 dwDecl[]{
    {0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
    {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},

    {1, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
    {1, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 1},
    {1, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 2},
    {1, 48, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 3},
    {1, 64, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 4},
    D3DDECL_END()
};

CDetail::~CDetail() {}

void CDetail::Unload()
{
    if (vertices)
    {
        xr_free(vertices);
        vertices = nullptr;
    }
    if (indices)
    {
        xr_free(indices);
        indices = nullptr;
    }
    shader.destroy();
}

void CDetail::transfer(Fmatrix& mXform, fvfVertexOut* vDest, u32 C, u16* iDest, u32 iOffset)
{
    // Transfer vertices
    {
        CDetail::fvfVertexIn *srcIt = vertices, *srcEnd = vertices + number_vertices;
        CDetail::fvfVertexOut* dstIt = vDest;
        for (; srcIt != srcEnd; srcIt++, dstIt++)
        {
            mXform.transform_tiny(dstIt->P, srcIt->P);
            dstIt->C = C;
            dstIt->u = srcIt->u;
            dstIt->v = srcIt->v;
        }
    }

    // Transfer indices (in 32bit lines)
    VERIFY(iOffset < 65535);
    {
        const u32 item = (iOffset << 16) | iOffset;
        const u32 count = number_indices / 2;
        LPDWORD sit = LPDWORD(indices);
        const LPDWORD send = sit + count;
        LPDWORD dit = LPDWORD(iDest);
        for (; sit != send; dit++, sit++)
            *dit = *sit + item;
        if (number_indices & 1)
            iDest[number_indices - 1] = u16(indices[number_indices - 1] + u16(iOffset));
    }
}

void CDetail::transfer(Fmatrix& mXform, fvfVertexOut* vDest, u32 C, u16* iDest, u32 iOffset, float du, float dv)
{
    // Transfer vertices
    {
        CDetail::fvfVertexIn *srcIt = vertices, *srcEnd = vertices + number_vertices;
        CDetail::fvfVertexOut* dstIt = vDest;
        for (; srcIt != srcEnd; srcIt++, dstIt++)
        {
            mXform.transform_tiny(dstIt->P, srcIt->P);
            dstIt->C = C;
            dstIt->u = srcIt->u + du;
            dstIt->v = srcIt->v + dv;
        }
    }

    // Transfer indices (in 32bit lines)
    VERIFY(iOffset < 65535);
    {
        const u32 item = (iOffset << 16) | iOffset;
        const u32 count = number_indices / 2;
        LPDWORD sit = LPDWORD(indices);
        const LPDWORD send = sit + count;
        LPDWORD dit = LPDWORD(iDest);
        for (; sit != send; dit++, sit++)
            *dit = *sit + item;
        if (number_indices & 1)
            iDest[number_indices - 1] = u16(indices[number_indices - 1] + u16(iOffset));
    }
}

void CDetail::LoadGeom()
{
    xr_vector<DetailVertData> detail_verts;
    detail_verts.reserve(number_vertices);
    for (u32 v{}; v < number_vertices; ++v)
    {
        const auto& vert = vertices[v];
        detail_verts.emplace_back(DetailVertData{vert.P.x, vert.P.y, vert.P.z, vert.u, vert.v, vert.P.y / (bv_bb.max.y - bv_bb.min.y)});
    }

    D3D11_BUFFER_DESC vbuff_desc{};
    vbuff_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbuff_desc.Usage = D3D11_USAGE_IMMUTABLE;
    vbuff_desc.StructureByteStride = sizeof(DetailVertData);
    vbuff_desc.ByteWidth = number_vertices * sizeof(DetailVertData);
    D3D11_SUBRESOURCE_DATA vdata{};
    vdata.pSysMem = detail_verts.data();
    HW.pDevice->CreateBuffer(&vbuff_desc, &vdata, &DetailVb);

    D3D11_BUFFER_DESC ibuff_desc{};
    ibuff_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibuff_desc.Usage = D3D11_USAGE_IMMUTABLE;
    ibuff_desc.StructureByteStride = sizeof(u16);
    ibuff_desc.ByteWidth = number_indices * sizeof(u16);
    D3D11_SUBRESOURCE_DATA idata{};
    idata.pSysMem = indices;
    HW.pDevice->CreateBuffer(&ibuff_desc, &idata, &DetailIb);

    DetailGeom.create(dwDecl, DetailVb, DetailIb);

    for (size_t it{2}; it <= 6; ++it)
    {
        auto& dcl = DetailGeom->dcl->dx10_dcl_code.at(it);
        dcl.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
        dcl.InstanceDataStepRate = 1;
    }
}

void CDetail::Load(IReader* S)
{
    // Shader
    string256 fnT, fnS;
    S->r_stringZ(fnS, sizeof(fnS));
    S->r_stringZ(fnT, sizeof(fnT));
    shader.create(fnS, fnT);

    // Params
    m_Flags.assign(S->r_u32());
    m_fMinScale = S->r_float();
    m_fMaxScale = S->r_float();

    number_vertices = S->r_u32();
    number_indices = S->r_u32();

    R_ASSERT(0 == (number_indices % 3));

    // Vertices
    const u32 size_vertices = number_vertices * sizeof(fvfVertexIn);
    vertices = xr_alloc<CDetail::fvfVertexIn>(number_vertices);

    S->r(vertices, size_vertices);

    // Indices
    const u32 size_indices = number_indices * sizeof(u16);
    indices = xr_alloc<u16>(number_indices);

    S->r(indices, size_indices);

    // Validate indices
#ifdef DEBUG
    for (u32 idx = 0; idx < number_indices; idx++)
        R_ASSERT(indices[idx] < (u16)number_vertices);
#endif

    // Calc BB & SphereRadius
    bv_bb.invalidate();
    for (u32 i = 0; i < number_vertices; i++)
        bv_bb.modify(vertices[i].P);

    bv_bb.getsphere(bv_sphere.P, bv_sphere.R);

    Optimize();

    _RELEASE(DetailVb);
    _RELEASE(DetailIb);
    DetailGeom.destroy();

    LoadGeom();
}

#include "xrstripify.h"

void CDetail::Optimize() const
{
    xr_vector<u16> vec_indices, vec_permute;
    const int cache = HW.Caps.geometry.dwVertexCache;

    // Stripify
    vec_indices.assign(indices, indices + number_indices);
    vec_permute.resize(number_vertices);
    const int vt_old = xrSimulate(vec_indices, cache);
    xrStripify(vec_indices, vec_permute, cache, 0);
    const int vt_new = xrSimulate(vec_indices, cache);
    if (vt_new < vt_old)
    {
        //Msg("* DM: %d verts, %d indices, VT: %d/%d",number_vertices,number_indices,vt_old,vt_new);

        // Copy faces
        CopyMemory(indices, vec_indices.data(), vec_indices.size() * sizeof(u16));

        // Permute vertices
        xr_vector<fvfVertexIn> verts;
        verts.assign(vertices, vertices + number_vertices);
        for (u32 i = 0; i < verts.size(); i++)
            vertices[i] = verts[vec_permute[i]];
    }
}

#include "stdafx.h"
#include "ftreevisual.h"

constexpr D3DVERTEXELEMENT9 FTreeVisualDecl[]{{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
                                              {0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
                                              {0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0},
                                              {0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},
                                              {0, 24, D3DDECLTYPE_SHORT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},

                                              {1, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
                                              {1, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 1},
                                              {1, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 2},
                                              {1, 48, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 3},
                                              D3DDECL_END()};
constexpr u32 FTreeVisualVertBufferSize{32};

static inline void q_hash(u32& seed, const u32 v)
{
    seed ^= v + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

FTreeVisual::FTreeVisual(void) {}
FTreeVisual::~FTreeVisual(void) {}

void FTreeVisual::Release() { dxRender_Visual::Release(); }

void FTreeVisual::Load(const char* N, IReader* data, u32 dwFlags)
{
    dxRender_Visual::Load(N, data, dwFlags);

    // read vertices
    R_ASSERT(data->find_chunk(OGF_GCONTAINER));
    {
        // verts
        u32 ID = data->r_u32();
        vBase = data->r_u32();
        vCount = data->r_u32();

        q_hash(base_crc, ID);
        q_hash(base_crc, vBase);
        q_hash(base_crc, vCount);

        p_rm_Vertices = RImplementation.getVB(ID);
        p_rm_Vertices->AddRef();

        // indices
        ID = data->r_u32();
        iBase = data->r_u32();
        iCount = data->r_u32();

        q_hash(base_crc, ID);
        q_hash(base_crc, iBase);
        q_hash(base_crc, iCount);

        for (u32 id = 0; id < R__NUM_CONTEXTS; ++id)
            crc[id] = base_crc;

        dwPrimitives = iCount / 3;

        p_rm_Indices = RImplementation.getIB(ID);
        p_rm_Indices->AddRef();
    }

    // load tree-def
    R_ASSERT(data->find_chunk(OGF_TREEDEF2));

    rm_geom.create(FTreeVisualDecl, p_rm_Vertices, p_rm_Indices);

    for(size_t it{5}; it <= 8; ++it)
    {
        auto& dcl = rm_geom->dcl->dx10_dcl_code.at(it);
        dcl.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
        dcl.InstanceDataStepRate = 1;
    }

    {
        constexpr float ps_r__Tree_SBC = 1.5f; // scale bias correct
        constexpr float s = ps_r__Tree_SBC * 1.3333f;

        struct _5color // Simp: не менять размер структуры! она должна читаться чанками по 20 байт
        {
            Fvector rgb; // - unused
            float hemi; // - hemisphere
            float sun; // - unused
        } c_scale{}, c_bias{};

        Fmatrix xform{};

        data->r(&xform, sizeof(xform));
        data->r(&c_scale, sizeof(c_scale));
        c_scale.hemi *= .5f;
        c_scale.hemi *= s;

        data->r(&c_bias, sizeof(c_bias));
        c_bias.hemi *= .5f;
        c_bias.hemi *= s;

        constexpr float FTreeVisual_tile = 16.f;
        constexpr float FTreeVisual_quant = 32768.f / FTreeVisual_tile;
        constexpr float FTreeVisual_scale = 1.f / FTreeVisual_quant;

        tree_data = {
            xform._11, xform._21, xform._31, xform._41,
            xform._12, xform._22, xform._32, xform._42,
            xform._13, xform._23, xform._33, xform._43,
            FTreeVisual_scale, FTreeVisual_scale, c_scale.hemi, c_bias.hemi,
        };
    }
}

void FTreeVisual::Render(CBackend& cmd_list, float lod, bool use_fast_geo) { R_ASSERT(0); }

void FTreeVisual::DoRenderInstanced(CBackend& cmd_list, const xr_vector<FloraVertData*>& data, const u32 countV, const u32 startI, const u32 PC)
{
    ZoneScoped;

    u32 sizeof_vbuffer = static_cast<u32>(data.size());
    ID3DVertexBuffer* current_vbuffer = cmd_list.GetFloraVbuff(sizeof_vbuffer);

    constexpr u32 vb_stride[]{FTreeVisualVertBufferSize, sizeof(FloraVertData)};
    constexpr u32 i_offset[]{0, 0};
    ID3DVertexBuffer* final_vbuffer[]{rm_geom->vb, current_vbuffer};

    cmd_list.set_Format(&*rm_geom->dcl);
    cmd_list.set_Vertices_Forced(std::size(final_vbuffer), final_vbuffer, vb_stride, i_offset);
    cmd_list.set_Indices(rm_geom->ib);

    D3D11_MAPPED_SUBRESOURCE pSubRes{};
    R_CHK(HW.get_context(cmd_list.context_id)->Map(current_vbuffer, 0, D3D_MAP_WRITE_DISCARD, 0, &pSubRes));

    u32 flora_count{};
    auto c_storage = reinterpret_cast<FloraVertData*>(pSubRes.pData);

    for (const auto* draw_data : data)
    {
        std::memcpy(&c_storage[flora_count++], draw_data, sizeof(*draw_data));
    }

    HW.get_context(cmd_list.context_id)->Unmap(current_vbuffer, 0);

    cmd_list.Render(D3DPT_TRIANGLELIST, vBase, 0, countV, startI, PC, flora_count);
    cmd_list.stat.r.s_flora.add(flora_count);
    //Msg("--[%s] rendered [%u] trees", __FUNCTION__, flora_count);
}

#define PCOPY(a) a = pFrom->a
void FTreeVisual::Copy(dxRender_Visual* pSrc)
{
    dxRender_Visual::Copy(pSrc);

    FTreeVisual* pFrom = dynamic_cast<FTreeVisual*>(pSrc);

    PCOPY(rm_geom);

    PCOPY(p_rm_Vertices);
    if (p_rm_Vertices)
        p_rm_Vertices->AddRef();

    PCOPY(vBase);
    PCOPY(vCount);

    PCOPY(p_rm_Indices);
    if (p_rm_Indices)
        p_rm_Indices->AddRef();

    PCOPY(iBase);
    PCOPY(iCount);

    PCOPY(dwPrimitives);

    PCOPY(tree_data);
    for (u32 id = 0; id < R__NUM_CONTEXTS; ++id)
        PCOPY(crc[id]);
    PCOPY(base_crc);
}

//-----------------------------------------------------------------------------------
// Stripified Tree
//-----------------------------------------------------------------------------------
FTreeVisual_ST::FTreeVisual_ST(void) {}
FTreeVisual_ST::~FTreeVisual_ST(void) {}

void FTreeVisual_ST::Release() { inherited::Release(); }
void FTreeVisual_ST::Load(const char* N, IReader* data, u32 dwFlags) { inherited::Load(N, data, dwFlags); }
void FTreeVisual_ST::Render(CBackend& cmd_list, float lod, bool use_fast_geo) { R_ASSERT(0); }

void FTreeVisual_ST::RenderInstanced(CBackend& cmd_list, const xr_vector<FloraVertData*>& data)
{
    inherited::DoRenderInstanced(cmd_list, data, vCount, iBase, dwPrimitives);
}

void FTreeVisual_ST::Copy(dxRender_Visual* pSrc) { inherited::Copy(pSrc); }

//-----------------------------------------------------------------------------------
// Progressive Tree
//-----------------------------------------------------------------------------------
FTreeVisual_PM::FTreeVisual_PM(void) {}
FTreeVisual_PM::~FTreeVisual_PM(void) {}

void FTreeVisual_PM::select_lod_id(float lod, u32 context_id)
{
    // inherited::select_lod_id(lod);

    const int lod_id = iFloor((1.f - clampr(lod, 0.f, 1.f)) * static_cast<float>(pSWI->count - 1)+ 0.5f);
    selected_lod_id[context_id] = lod_id;

    crc[context_id] = base_crc; // restore initial crc, update hash if selected lod_id > 0

    if (selected_lod_id[context_id] > 0)
    {
        q_hash(crc[context_id], selected_lod_id[context_id]);
    }
}

void FTreeVisual_PM::Release() { inherited::Release(); }
void FTreeVisual_PM::Load(const char* N, IReader* data, u32 dwFlags)
{
    inherited::Load(N, data, dwFlags);
    R_ASSERT(data->find_chunk(OGF_SWICONTAINER));
    {
        u32 ID = data->r_u32();

        q_hash(base_crc, ID);

        for (u32 id = 0; id < R__NUM_CONTEXTS; ++id)
            crc[id] = base_crc;

        pSWI = RImplementation.getSWI(ID);
    }
}

void FTreeVisual_PM::Render(CBackend& cmd_list, float lod, bool use_fast_geo) { R_ASSERT(0); }

void FTreeVisual_PM::RenderInstanced(CBackend& cmd_list, const xr_vector<FloraVertData*>& data)
{
    const FSlideWindow& SW = pSWI->sw[selected_lod_id[cmd_list.context_id]];
    //R_ASSERT(selected_lod_id[cmd_list.context_id] >= 0 && selected_lod_id[cmd_list.context_id] < int(pSWI->count));

    //Msg("--[%s] selected_lod_id [%u]", __FUNCTION__, selected_lod_id[cmd_list.context_id]);
    inherited::DoRenderInstanced(cmd_list, data, SW.num_verts, iBase + SW.offset, SW.num_tris);
}

void FTreeVisual_PM::Copy(dxRender_Visual* pSrc)
{
    inherited::Copy(pSrc);
    FTreeVisual_PM* pFrom = dynamic_cast<FTreeVisual_PM*>(pSrc);
    PCOPY(pSWI);
}

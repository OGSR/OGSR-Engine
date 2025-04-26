#include "stdafx.h"

#include "../xrRenderDX10/dx10BufferUtils.h"

CBackend::CBackend()
    : xforms(*this)
    , hemi(*this)
    , lod(*this)
    , constants(*this)
    , StateManager(*this)
{
    Invalidate();
}

constexpr class IndexBufferGen
{
    static constexpr size_t dwTriCount{4 * 1024};
    static constexpr size_t dwIdxCount{dwTriCount * 2 * 3};

    size_t ICnt{};
    u16 Cnt{};

public:
    u16 Indices[dwIdxCount]{};

    consteval IndexBufferGen()
    {
        for (size_t i{}; i < dwTriCount; i++)
        {
            Indices[ICnt++] = Cnt;
            Indices[ICnt++] = Cnt + 1;
            Indices[ICnt++] = Cnt + 2;

            Indices[ICnt++] = Cnt + 3;
            Indices[ICnt++] = Cnt + 2;
            Indices[ICnt++] = Cnt + 1;

            Cnt += 4;
        }
    }
} IndexBuffer{};

// Create Quad-IB
void CRender::CreateQuadIB()
{
    D3D_BUFFER_DESC desc{};
    desc.ByteWidth = sizeof IndexBuffer.Indices;
    // desc.Usage = D3D_USAGE_IMMUTABLE;
    desc.Usage = D3D_USAGE_DEFAULT;
    desc.BindFlags = D3D_BIND_INDEX_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D_SUBRESOURCE_DATA subData{};
    subData.pSysMem = IndexBuffer.Indices;

    R_CHK(HW.pDevice->CreateBuffer(&desc, &subData, &QuadIB));
    HW.stats_manager.increment_stats_ib(QuadIB);
}

// Device dependance
void CBackend::OnDeviceCreate()
{
#ifdef TRACY_ENABLE
    HW.get_context(context_id)->QueryInterface(IID_PPV_ARGS(&pAnnotation));
#endif
    // Debug Draw
    InitializeDebugDraw();

    // invalidate caching
    Invalidate();

    for (const auto& size : FloraVbufSizes)
    {
        D3D11_BUFFER_DESC buff_desc{};
        buff_desc.ByteWidth = size * sizeof(FloraVertData);
        buff_desc.Usage = D3D_USAGE_DYNAMIC;
        buff_desc.BindFlags = D3D_BIND_VERTEX_BUFFER;
        buff_desc.CPUAccessFlags = D3D_CPU_ACCESS_WRITE;
        buff_desc.MiscFlags = 0;
        buff_desc.StructureByteStride = sizeof(FloraVertData);

        ID3DVertexBuffer* buff{};
        R_CHK(HW.pDevice->CreateBuffer(&buff_desc, nullptr, &buff));
        FloraVbuffers.emplace(size, buff);
    }
}

void CBackend::OnDeviceDestroy()
{
    // Debug Draw
    DestroyDebugDraw();

    StateManager.Reset();

    _RELEASE(pAnnotation);

    for (auto& it : FloraVbuffers)
        _RELEASE(it.second);

    FloraVbuffers.clear();
}

ID3DVertexBuffer* CBackend::GetFloraVbuff(u32& size)
{
    auto it = FloraVbuffers.lower_bound(size);
    if (it == FloraVbuffers.end())
    {
        constexpr u32 max_buf = FloraVbufSizes[std::size(FloraVbufSizes) - 1];
        it = FloraVbuffers.find(max_buf);
    }

    size = it->first;
    return it->second;
}
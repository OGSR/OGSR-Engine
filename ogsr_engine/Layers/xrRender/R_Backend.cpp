#include "stdafx.h"


#include "../xrRenderDX10/dx10BufferUtils.h"

CBackend RCache;

// Create Quad-IB

// Igor: is used to test bug with rain, particles corruption
void CBackend::RestoreQuadIBData() {}

void CBackend::CreateQuadIB()
{
    static const u32 dwTriCount = 4 * 1024;
    static const u32 dwIdxCount = dwTriCount * 2 * 3;
    u16 IndexBuffer[dwIdxCount];
    u16* Indices = IndexBuffer;
    // u32		dwUsage			= D3DUSAGE_WRITEONLY;
    // if (HW.Caps.geometry.bSoftware)	dwUsage|=D3DUSAGE_SOFTWAREPROCESSING;
    // R_CHK(HW.pDevice->CreateIndexBuffer(dwIdxCount*2,dwUsage,D3DFMT_INDEX16,D3DPOOL_MANAGED,&QuadIB,NULL));

    D3D_BUFFER_DESC desc{};
    desc.ByteWidth = dwIdxCount * 2;
    // desc.Usage = D3D_USAGE_IMMUTABLE;
    desc.Usage = D3D_USAGE_DEFAULT;
    desc.BindFlags = D3D_BIND_INDEX_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D_SUBRESOURCE_DATA subData{};
    subData.pSysMem = IndexBuffer;

    // R_CHK(QuadIB->Lock(0,0,(void**)&Indices,0));
    {
        int Cnt = 0;
        int ICnt = 0;
        for (int i = 0; i < dwTriCount; i++)
        {
            Indices[ICnt++] = u16(Cnt + 0);
            Indices[ICnt++] = u16(Cnt + 1);
            Indices[ICnt++] = u16(Cnt + 2);

            Indices[ICnt++] = u16(Cnt + 3);
            Indices[ICnt++] = u16(Cnt + 2);
            Indices[ICnt++] = u16(Cnt + 1);

            Cnt += 4;
        }
    }
    // R_CHK(QuadIB->Unlock());

    // R_CHK(HW.pDevice->CreateIndexBuffer(dwIdxCount*2,dwUsage,D3DFMT_INDEX16,D3DPOOL_MANAGED,&QuadIB,NULL));
    R_CHK(HW.pDevice->CreateBuffer(&desc, &subData, &QuadIB));
    HW.stats_manager.increment_stats_ib(QuadIB);
}

// Device dependance
void CBackend::OnDeviceCreate()
{
    CreateQuadIB();

    // streams
    Vertex.Create();
    Index.Create();

    // Debug Draw
    InitializeDebugDraw();

    // invalidate caching
    Invalidate();
}

void CBackend::OnDeviceDestroy()
{
    // streams
    Index.Destroy();
    Vertex.Destroy();

    // Quad
    HW.stats_manager.decrement_stats_ib(QuadIB);
    _RELEASE(QuadIB);

    // Debug Draw
    DestroyDebugDraw();
}

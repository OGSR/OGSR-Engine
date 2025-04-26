#include "stdafx.h"


#include "ResourceManager.h"
#include "R_DStreams.h"

#include "../xrRender/dxRenderDeviceRender.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int rsDVB_Size = 4096; // Fixed: (bytes_need<=mSize) && vl_Count
int rsDIB_Size = 512;

void _VertexStream::Create()
{
    mSize = rsDVB_Size * 1024;

    D3D_BUFFER_DESC bufferDesc;
    bufferDesc.ByteWidth = mSize;
    bufferDesc.Usage = D3D_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0;

    R_CHK(HW.pDevice->CreateBuffer(&bufferDesc, 0, &pVB));
    HW.stats_manager.increment_stats_vb(pVB);

    R_ASSERT(pVB);

    mPosition = 0;
    mDiscardID = 0;

    Msg("* DVB created: %dK", mSize / 1024);
}

void _VertexStream::Destroy()
{
    HW.stats_manager.decrement_stats_vb(pVB);
    _RELEASE(pVB);
    _clear();
}

void* _VertexStream::Lock(u32 vl_Count, u32 Stride, u32& vOffset)
{
    D3D11_MAPPED_SUBRESOURCE MappedSubRes;

#ifdef DEBUG
    PGO(Msg("PGO:VB_LOCK:%d", vl_Count));
    VERIFY(0 == dbg_lock);
    dbg_lock++;
#endif

    R_ASSERT(vl_Count, "Missing or invalid texture! vl_Count=0.");

    // Ensure there is enough space in the VB for this data
    const u32 bytes_need = vl_Count * Stride;
    ASSERT_FMT(bytes_need <= mSize, "bytes_need = [%u], mSize = [%u]", bytes_need, mSize);

    // Vertex-local info
    const u32 vl_mSize = mSize / Stride;
    const u32 vl_mPosition = mPosition / Stride + 1;

    // ASSERT_FMT(Device.OnMainThread(), "!![%s] NOT ON MAIN THREAD! THREAD ID: [%u]", __FUNCTION__, _Thrd_id());

    // Check if there is need to flush and perform lock
    BYTE* pData = nullptr;
    if ((vl_Count + vl_mPosition) >= vl_mSize)
    {
        // FLUSH-LOCK
        mPosition = 0;
        vOffset = 0;
        mDiscardID++;

        HW.get_context(CHW::IMM_CTX_ID)->Map(pVB, 0, D3D_MAP_WRITE_DISCARD, 0, &MappedSubRes);
        pData = (BYTE*)MappedSubRes.pData;
        pData += vOffset;

    }
    else
    {
        // APPEND-LOCK
        mPosition = vl_mPosition * Stride;
        vOffset = vl_mPosition;

        HW.get_context(CHW::IMM_CTX_ID)->Map(pVB, 0, D3D_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubRes);
        pData = (BYTE*)MappedSubRes.pData;
        pData += vOffset * Stride;

    }
    VERIFY(pData);

    return LPVOID(pData);
}

void _VertexStream::Unlock(u32 Count, u32 Stride)
{
#ifdef DEBUG
    PGO(Msg("PGO:VB_UNLOCK:%d", Count));
    VERIFY(1 == dbg_lock);
    dbg_lock--;
#endif
    mPosition += Count * Stride;

    VERIFY(pVB);

    // ASSERT_FMT(Device.OnMainThread(), "!![%s] NOT ON MAIN THREAD! THREAD ID: [%u]", __FUNCTION__, _Thrd_id());

    HW.get_context(CHW::IMM_CTX_ID)->Unmap(pVB, 0);
}

void _VertexStream::reset_begin()
{
    old_pVB = pVB;
    Destroy();
}
void _VertexStream::reset_end()
{
    Create();
    // old_pVB				= NULL;
}

_VertexStream::_VertexStream() { _clear(); };

void _VertexStream::_clear()
{
    pVB = nullptr;
    mSize = 0;
    mPosition = 0;
    mDiscardID = 0;
#ifdef DEBUG
    dbg_lock = 0;
#endif
}

//////////////////////////////////////////////////////////////////////////
void _IndexStream::Create()
{
    mSize = rsDIB_Size * 1024;

    D3D_BUFFER_DESC bufferDesc;
    bufferDesc.ByteWidth = mSize;
    bufferDesc.Usage = D3D_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D_BIND_INDEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0;

    R_CHK(HW.pDevice->CreateBuffer(&bufferDesc, 0, &pIB));
    HW.stats_manager.increment_stats_ib(pIB);

    R_ASSERT(pIB);

    mPosition = 0;
    mDiscardID = 0;

    Msg("* DIB created: %dK", mSize / 1024);
}

void _IndexStream::Destroy()
{
    HW.stats_manager.decrement_stats_ib(pIB);
    _RELEASE(pIB);
    _clear();
}

u16* _IndexStream::Lock(u32 Count, u32& vOffset)
{
    D3D11_MAPPED_SUBRESOURCE MappedSubRes;
    PGO(Msg("PGO:IB_LOCK:%d", Count));
    vOffset = 0;
    BYTE* pLockedData = nullptr;

    // Ensure there is enough space in the VB for this data
    R_ASSERT((2 * Count <= mSize) && Count);

    // If either user forced us to flush,
    // or there is not enough space for the index data,
    // then flush the buffer contents
    u32 dwFlags = LOCKFLAGS_APPEND;
    if (2 * (Count + mPosition) >= mSize)
    {
        mPosition = 0; // clear position
        dwFlags = LOCKFLAGS_FLUSH; // discard it's contens
        mDiscardID++;
    }

    // ASSERT_FMT(Device.OnMainThread(), "!![%s] NOT ON MAIN THREAD! THREAD ID: [%u]", __FUNCTION__, _Thrd_id());

    const D3D_MAP MapMode = (dwFlags == LOCKFLAGS_APPEND) ? D3D_MAP_WRITE_NO_OVERWRITE : D3D_MAP_WRITE_DISCARD;
    HW.get_context(CHW::IMM_CTX_ID)->Map(pIB, 0, MapMode, 0, &MappedSubRes);
    pLockedData = (BYTE*)MappedSubRes.pData;
    pLockedData += mPosition * 2;

    VERIFY(pLockedData);

    vOffset = mPosition;

    return LPWORD(pLockedData);
}

void _IndexStream::Unlock(u32 RealCount)
{
    PGO(Msg("PGO:IB_UNLOCK:%d", RealCount));
    mPosition += RealCount;
    VERIFY(pIB);

    // ASSERT_FMT(Device.OnMainThread(), "!![%s] NOT ON MAIN THREAD! THREAD ID: [%u]", __FUNCTION__, _Thrd_id());

    HW.get_context(CHW::IMM_CTX_ID)->Unmap(pIB, 0);
}

void _IndexStream::reset_begin()
{
    old_pIB = pIB;
    Destroy();
}
void _IndexStream::reset_end()
{
    Create();
    // old_pIB				= NULL;
}

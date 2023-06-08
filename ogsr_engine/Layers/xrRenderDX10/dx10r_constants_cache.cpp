#include "stdafx.h"
#pragma hdrstop

#include "../xrRender/r_constants_cache.h"

dx10ConstantBuffer& R_constants::GetCBuffer(R_constant* C, BufferType BType)
{
    if (BType == BT_PixelBuffer)
    {
        //	Decode index
        int iBufferIndex = (C->destination & RC_dest_pixel_cb_index_mask) >> RC_dest_pixel_cb_index_shift;

        VERIFY(iBufferIndex < CBackend::MaxCBuffers);
        VERIFY(RCache.m_aPixelConstants[iBufferIndex]);
        return *RCache.m_aPixelConstants[iBufferIndex];
    }
    else if (BType == BT_VertexBuffer)
    {
        //	Decode index
        int iBufferIndex = (C->destination & RC_dest_vertex_cb_index_mask) >> RC_dest_vertex_cb_index_shift;

        VERIFY(iBufferIndex < CBackend::MaxCBuffers);
        VERIFY(RCache.m_aVertexConstants[iBufferIndex]);
        return *RCache.m_aVertexConstants[iBufferIndex];
    }
    else if (BType == BT_GeometryBuffer)
    {
        //	Decode index
        int iBufferIndex = (C->destination & RC_dest_geometry_cb_index_mask) >> RC_dest_geometry_cb_index_shift;

        VERIFY(iBufferIndex < CBackend::MaxCBuffers);
        VERIFY(RCache.m_aGeometryConstants[iBufferIndex]);
        return *RCache.m_aGeometryConstants[iBufferIndex];
    }
#ifdef USE_DX11
    else if (BType == BT_HullBuffer)
    {
        //	Decode index
        int iBufferIndex = (C->destination & RC_dest_hull_cb_index_mask) >> RC_dest_hull_cb_index_shift;

        VERIFY(iBufferIndex < CBackend::MaxCBuffers);
        VERIFY(RCache.m_aHullConstants[iBufferIndex]);
        return *RCache.m_aHullConstants[iBufferIndex];
    }
    else if (BType == BT_DomainBuffer)
    {
        //	Decode index
        int iBufferIndex = (C->destination & RC_dest_domain_cb_index_mask) >> RC_dest_domain_cb_index_shift;

        VERIFY(iBufferIndex < CBackend::MaxCBuffers);
        VERIFY(RCache.m_aDomainConstants[iBufferIndex]);
        return *RCache.m_aDomainConstants[iBufferIndex];
    }
    else if (BType == BT_Compute)
    {
        //	Decode index
        int iBufferIndex = (C->destination & RC_dest_compute_cb_index_mask) >> RC_dest_compute_cb_index_shift;

        VERIFY(iBufferIndex < CBackend::MaxCBuffers);
        VERIFY(RCache.m_aComputeConstants[iBufferIndex]);
        return *RCache.m_aComputeConstants[iBufferIndex];
    }
#endif

    FATAL("Unreachable code");
    // Just hack to avoid warning;
    dx10ConstantBuffer* ptr = 0;
    return *ptr; //-V522
}

void R_constants::flush_cache()
{
    for (int i = 0; i < CBackend::MaxCBuffers; ++i)
    {
        if (RCache.m_aVertexConstants[i])
            RCache.m_aVertexConstants[i]->Flush();

        if (RCache.m_aPixelConstants[i])
            RCache.m_aPixelConstants[i]->Flush();

        if (RCache.m_aGeometryConstants[i])
            RCache.m_aGeometryConstants[i]->Flush();

#ifdef USE_DX11
        if (RCache.m_aHullConstants[i])
            RCache.m_aHullConstants[i]->Flush();

        if (RCache.m_aDomainConstants[i])
            RCache.m_aDomainConstants[i]->Flush();

        if (RCache.m_aComputeConstants[i])
            RCache.m_aComputeConstants[i]->Flush();
#endif
    }
}

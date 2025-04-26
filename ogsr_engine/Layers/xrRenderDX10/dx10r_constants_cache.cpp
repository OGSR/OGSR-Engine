#include "stdafx.h"


#include "../xrRender/r_constants_cache.h"


template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_PixelBuffer>(R_constant* C) const
{
    //	Decode index
    const int iBufferIndex = (C->destination & RC_dest_pixel_cb_index_mask) >> RC_dest_pixel_cb_index_shift;

    R_ASSERT(iBufferIndex < CBackend::MaxCBuffers);
    return *cmd_list.m_aPixelConstants[iBufferIndex];
}

template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_VertexBuffer>(R_constant* C) const
{
    //	Decode index
    const int iBufferIndex = (C->destination & RC_dest_vertex_cb_index_mask) >> RC_dest_vertex_cb_index_shift;

    R_ASSERT(iBufferIndex < CBackend::MaxCBuffers);
    return *cmd_list.m_aVertexConstants[iBufferIndex];
}

template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_GeometryBuffer>(R_constant* C) const
{
    //	Decode index
    const int iBufferIndex = (C->destination & RC_dest_geometry_cb_index_mask) >> RC_dest_geometry_cb_index_shift;

    R_ASSERT(iBufferIndex < CBackend::MaxCBuffers);
    return *cmd_list.m_aGeometryConstants[iBufferIndex];
}

template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_HullBuffer>(R_constant* C) const
{
    //	Decode index
    const int iBufferIndex = (C->destination & RC_dest_hull_cb_index_mask) >> RC_dest_hull_cb_index_shift;

    R_ASSERT(iBufferIndex < CBackend::MaxCBuffers);
    return *cmd_list.m_aHullConstants[iBufferIndex];
}

template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_DomainBuffer>(R_constant* C) const
{
    //	Decode index
    const int iBufferIndex = (C->destination & RC_dest_domain_cb_index_mask) >> RC_dest_domain_cb_index_shift;

    R_ASSERT(iBufferIndex < CBackend::MaxCBuffers);
    return *cmd_list.m_aDomainConstants[iBufferIndex];
}

void R_constants::flush_cache() const
{
    const auto context_id = cmd_list.context_id; // TODO: constant buffer should be encapsulated, so no ctx ID needed

    for (int i = 0; i < CBackend::MaxCBuffers; ++i)
    {
        if (cmd_list.m_aVertexConstants[i])
            cmd_list.m_aVertexConstants[i]->Flush(context_id);

        if (cmd_list.m_aPixelConstants[i])
            cmd_list.m_aPixelConstants[i]->Flush(context_id);

        if (cmd_list.m_aGeometryConstants[i])
            cmd_list.m_aGeometryConstants[i]->Flush(context_id);

        if (cmd_list.m_aHullConstants[i])
            cmd_list.m_aHullConstants[i]->Flush(context_id);

        if (cmd_list.m_aDomainConstants[i])
            cmd_list.m_aDomainConstants[i]->Flush(context_id);

        if (cmd_list.m_aComputeConstants[i])
            cmd_list.m_aComputeConstants[i]->Flush(context_id);
    }
}
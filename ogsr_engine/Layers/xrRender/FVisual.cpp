// FVisual.cpp: implementation of the FVisual class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../../xr_3da/fmesh.h"
#include "fvisual.h"

#include "../xrRenderDX10/dx10BufferUtils.h"

#include <Utilities\FlexibleVertexFormat.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Fvisual::Fvisual() : dxRender_Visual() { m_fast = nullptr; }

Fvisual::~Fvisual()
{
    HW.stats_manager.decrement_stats_vb(p_rm_Vertices);
    HW.stats_manager.decrement_stats_ib(p_rm_Indices);

    xr_delete(m_fast);
}

void Fvisual::Release() { dxRender_Visual::Release(); }

void Fvisual::Load(const char* N, IReader* data, u32 dwFlags)
{
    dxRender_Visual::Load(N, data, dwFlags);

    auto dcl = xr_vector<D3DVERTEXELEMENT9>(MAXD3DDECLLENGTH + 1);
    D3DVERTEXELEMENT9* vFormat = nullptr;
    dwPrimitives = 0;
    BOOL loaded_v = false;

    if (data->find_chunk(OGF_GCONTAINER))
    {
        // verts
        u32 ID = data->r_u32();
        vBase = data->r_u32();
        vCount = data->r_u32();

        vFormat = RImplementation.getVB_Format(ID);

        VERIFY(NULL == p_rm_Vertices);

        p_rm_Vertices = RImplementation.getVB(ID);
        p_rm_Vertices->AddRef();

        // indices
        ID = data->r_u32();
        iBase = data->r_u32();
        iCount = data->r_u32();

        dwPrimitives = iCount / 3;

        VERIFY(NULL == p_rm_Indices);
        p_rm_Indices = RImplementation.getIB(ID);
        p_rm_Indices->AddRef();

        loaded_v = true;

        // check for fast-vertices
        if (data->find_chunk(OGF_FASTPATH))
        {
            destructor<IReader> geomdef(data->open_chunk(OGF_FASTPATH));
            destructor<IReader> def(geomdef().open_chunk(OGF_GCONTAINER));

            // we have fast-mesh
            m_fast = xr_new<IRender_Mesh>();

            // verts
            ID = def().r_u32();
            m_fast->vBase = def().r_u32();
            m_fast->vCount = def().r_u32();

            VERIFY(NULL == m_fast->p_rm_Vertices);
            m_fast->p_rm_Vertices = RImplementation.getVB(ID, true);
            m_fast->p_rm_Vertices->AddRef();
            D3DVERTEXELEMENT9* fmt = RImplementation.getVB_Format(ID, true);

            // indices
            ID = def().r_u32();
            m_fast->iBase = def().r_u32();
            m_fast->iCount = def().r_u32();
            m_fast->dwPrimitives = iCount / 3;

            VERIFY(NULL == m_fast->p_rm_Indices);
            m_fast->p_rm_Indices = RImplementation.getIB(ID, true);
            m_fast->p_rm_Indices->AddRef();

            // geom
            m_fast->rm_geom.create(fmt, m_fast->p_rm_Vertices, m_fast->p_rm_Indices);
        }
    }

    if (!loaded_v)
    {
        // read vertices
        if ((dwFlags & VLOAD_NOVERTICES) == 0)
        {
            if (data->find_chunk(OGF_VCONTAINER))
            {
                R_ASSERT(0, "pls notify andy about this.");
            }
            else
            {
                R_ASSERT(data->find_chunk(OGF_VERTICES));
                vBase = 0;
                const u32 fvf = data->r_u32();
                CHK_DX(FVF::CreateDeclFromFVF(fvf, dcl));
                vFormat = dcl.data();
                vCount = data->r_u32();
                const u32 vStride = FVF::ComputeVertexSize(fvf);

                VERIFY(NULL == p_rm_Vertices);
                R_CHK(dx10BufferUtils::CreateVertexBuffer(&p_rm_Vertices, data->pointer(), vCount * vStride));
                HW.stats_manager.increment_stats_vb(p_rm_Vertices);
            }
        }

        // read indices
        if (data->find_chunk(OGF_ICONTAINER))
        {
            R_ASSERT(0, "pls notify andy about this.");
        }
        else
        {
            R_ASSERT(data->find_chunk(OGF_INDICES));
            iBase = 0;
            iCount = data->r_u32();
            dwPrimitives = iCount / 3;

            VERIFY(NULL == p_rm_Indices);
            R_CHK(dx10BufferUtils::CreateIndexBuffer(&p_rm_Indices, data->pointer(), iCount * 2));
            HW.stats_manager.increment_stats_ib(p_rm_Indices);
        }
    }

    if (dwFlags & VLOAD_NOVERTICES)
        return;

    rm_geom.create(vFormat, p_rm_Vertices, p_rm_Indices);
}


void Fvisual::Render(CBackend& cmd_list, float, bool use_fast_geo)
{
    ZoneScoped;

    if (m_fast && use_fast_geo && !cmd_list.is_TessEnabled())
    {
        cmd_list.set_Geometry(m_fast->rm_geom);
        cmd_list.Render(D3DPT_TRIANGLELIST, m_fast->vBase, 0, m_fast->vCount, m_fast->iBase, m_fast->dwPrimitives);
        cmd_list.stat.r.s_static.add(m_fast->vCount);
    }
    else
    {
        cmd_list.set_Geometry(rm_geom);
        cmd_list.Render(D3DPT_TRIANGLELIST, vBase, 0, vCount, iBase, dwPrimitives);
        cmd_list.stat.r.s_static.add(vCount);
    }
}

#define PCOPY(a) a = pFrom->a

void Fvisual::Copy(dxRender_Visual* pSrc)
{
    dxRender_Visual::Copy(pSrc);

    const Fvisual* pFrom = dynamic_cast<Fvisual*>(pSrc);

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

    PCOPY(m_fast);
}

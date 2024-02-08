#include "stdafx.h"
#include "dx103DFluidVolume.h"
#include "dx103DFluidManager.h"

dx103DFluidVolume::dx103DFluidVolume() {}

dx103DFluidVolume::~dx103DFluidVolume() {}

void dx103DFluidVolume::Load(LPCSTR N, IReader* data, u32 dwFlags)
{
    //	Uncomment this if choose to read from OGF
    //	dxRender_Visual::Load		(N,data,dwFlags);

    //	Create shader for correct sort while rendering
    //	shader name can't start from a digit
    shader.create("fluid3d_stub", "water\\water_ryaska1");

    //	Create debug geom
    m_Geom.create(FVF::F_LIT, RCache.Vertex.Buffer(), RCache.QuadIB);

    Type = MT_3DFLUIDVOLUME;

    //	Version 3>
    m_FluidData.Load(data);

    //	Prepare transform
    const Fmatrix& Transform = m_FluidData.GetTransform();

    //	Update visibility data
    vis.box.min.set(-0.5f, -0.5f, -0.5f);
    vis.box.max.set(0.5f, 0.5f, 0.5f);

    vis.box.xform(Transform);

    vis.box.getcenter(vis.sphere.P);
    vis.sphere.R = vis.box.getradius();
}

void dx103DFluidVolume::Render(float LOD) // LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored ?
{
    if (!ps_r2_ls_flags.test(R3FLAG_VOLUMETRIC_SMOKE))
        return;

    u32 dwOffset, dwCount;

    FVF::LIT* pv_start = (FVF::LIT*)RCache.Vertex.Lock(6 * 3 * 2, m_Geom->vb_stride, dwOffset);
    FVF::LIT* pv = pv_start;

    constexpr u32 clr = 0xFFFFFFFF;

    Fbox box;
    box.min.set(-0.5f, -0.5f, -0.5f);
    box.max.set(0.5f, 0.5f, 0.5f);

    //	Prepare box here
    {
        //	Face 0
        pv->set(box.x1, box.y1, box.z1, clr, 0, 0);
        pv++;
        pv->set(box.x1, box.y1, box.z2, clr, 0, 1);
        pv++;
        pv->set(box.x1, box.y2, box.z1, clr, 1, 0);
        pv++;
        pv->set(box.x1, box.y2, box.z2, clr, 1, 1);
        pv++;

        //	Face 1
        pv->set(box.x2, box.y1, box.z2, clr, 0, 1);
        pv++;
        pv->set(box.x2, box.y1, box.z1, clr, 0, 0);
        pv++;
        pv->set(box.x2, box.y2, box.z2, clr, 1, 1);
        pv++;
        pv->set(box.x2, box.y2, box.z1, clr, 1, 0);
        pv++;

        //	Face 2
        pv->set(box.x1, box.y1, box.z2, clr, 0, 1);
        pv++;
        pv->set(box.x1, box.y1, box.z1, clr, 0, 0);
        pv++;
        pv->set(box.x2, box.y1, box.z2, clr, 1, 1);
        pv++;
        pv->set(box.x2, box.y1, box.z1, clr, 1, 0);
        pv++;

        //	Face 3
        pv->set(box.x1, box.y2, box.z1, clr, 0, 0);
        pv++;
        pv->set(box.x1, box.y2, box.z2, clr, 0, 1);
        pv++;
        pv->set(box.x2, box.y2, box.z1, clr, 1, 0);
        pv++;
        pv->set(box.x2, box.y2, box.z2, clr, 1, 1);
        pv++;

        //	Face 4
        pv->set(box.x1, box.y1, box.z1, clr, 0, 1);
        pv++;
        pv->set(box.x1, box.y2, box.z1, clr, 0, 0);
        pv++;
        pv->set(box.x2, box.y1, box.z1, clr, 1, 1);
        pv++;
        pv->set(box.x2, box.y2, box.z1, clr, 1, 0);
        pv++;

        //	Face 5
        pv->set(box.x1, box.y2, box.z2, clr, 0, 0);
        pv++;
        pv->set(box.x1, box.y1, box.z2, clr, 0, 1);
        pv++;
        pv->set(box.x2, box.y2, box.z2, clr, 1, 0);
        pv++;
        pv->set(box.x2, box.y1, box.z2, clr, 1, 1);
        pv++;
    }

    RCache.set_xform_world(m_FluidData.GetTransform());

    dwCount = u32(pv - pv_start);
    RCache.Vertex.Unlock(dwCount, m_Geom->vb_stride);
    RCache.set_Geometry(m_Geom);

    //	Render obstacles
    const xr_vector<Fmatrix>& Obstacles = m_FluidData.GetObstaclesList();
    int iObstNum = Obstacles.size();
    for (int i = 0; i < iObstNum; ++i)
    {
        RCache.set_xform_world(Obstacles[i]);
    }

    constexpr float fTimeStep = 2.0f;

    FluidManager.Update(m_FluidData, fTimeStep);
    FluidManager.RenderFluid(m_FluidData);
}

void dx103DFluidVolume::Copy(dxRender_Visual* pFrom) { dxRender_Visual::Copy(pFrom); }

void dx103DFluidVolume::Release() { dxRender_Visual::Release(); }

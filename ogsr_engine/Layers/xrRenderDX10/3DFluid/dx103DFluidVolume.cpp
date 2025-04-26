#include "stdafx.h"

#ifdef DX10_FLUID_ENABLE

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
    m_Geom.create(FVF::F_LIT, RImplementation.Vertex.Buffer(), RImplementation.QuadIB);

    Type = MT_3DFLUIDVOLUME;

    //	Version 3>
    m_FluidData.Load(data);

    //	Prepare transform
    const Fmatrix& Transform = m_FluidData.GetTransform();

    //	Update visibility data
    vis.box.min = Fvector3().set(-0.5f, -0.5f, -0.5f);
    vis.box.max = Fvector3().set(0.5f, 0.5f, 0.5f);

    vis.box.xform(Transform);

    vis.box.getcenter(vis.sphere.P);
    vis.sphere.R = vis.box.getradius();
}

void dx103DFluidVolume::Render(CBackend& cmd_list, float, bool) // LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored ?
{
    if (!ps_r2_ls_flags.test(R3FLAG_VOLUMETRIC_SMOKE))
        return;

    //	Render debug box
    //	Do it BEFORE update since update resets shaders and other pipeline settings

    //	FluidManager.RenderFluid( m_FluidData );

    u32 dwOffset, dwCount;

    FVF::LIT* pv_start = (FVF::LIT*)RImplementation.Vertex.Lock(6 * 3 * 2, m_Geom->vb_stride, dwOffset);
    FVF::LIT* pv = pv_start;

    const u32 clr = 0xFFFFFFFF;

    Fbox box;
    box.min = Fvector3().set(-0.5f, -0.5f, -0.5f);
    box.max = Fvector3().set(0.5f, 0.5f, 0.5f);

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

    cmd_list.set_xform_world(m_FluidData.GetTransform());

    dwCount = u32(pv - pv_start);
    RImplementation.Vertex.Unlock(dwCount, m_Geom->vb_stride);
    cmd_list.set_Geometry(m_Geom);

    //	Render obstacles
    const xr_vector<Fmatrix>& Obstacles = m_FluidData.GetObstaclesList();
    const int iObstNum = Obstacles.size();
    for (int i = 0; i < iObstNum; ++i)
    {
        cmd_list.set_xform_world(Obstacles[i]);
    }

    // float fTimeStep = Device.fTimeDelta*30*2.0f;
    const float fTimeStep = 2.0f;

    // FluidManager.Update( m_FluidData, 2.0f);
    FluidManager.Update(cmd_list, m_FluidData, fTimeStep);
    FluidManager.RenderFluid(cmd_list, m_FluidData);
}

void dx103DFluidVolume::Copy(dxRender_Visual* pFrom) { dxRender_Visual::Copy(pFrom); }

void dx103DFluidVolume::Release() { dxRender_Visual::Release(); }

#endif

#include "stdafx.h"
#include "dxThunderboltRender.h"

#include "../../xr_3da/thunderbolt.h"
#include "dxThunderboltDescRender.h"
#include "dxLensFlareRender.h"

dxThunderboltRender::dxThunderboltRender()
{
    // geom
    hGeom_model.create(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, RImplementation.Vertex.Buffer(), RImplementation.Index.Buffer());
    hGeom_gradient.create(FVF::F_LIT, RImplementation.Vertex.Buffer(), RImplementation.QuadIB);
}

dxThunderboltRender::~dxThunderboltRender()
{
    hGeom_model.destroy();
    hGeom_gradient.destroy();
}

void dxThunderboltRender::Copy(IThunderboltRender& _in) { *this = *smart_cast<dxThunderboltRender*>(&_in); }

void dxThunderboltRender::Render(CBackend& cmd_list, CEffect_Thunderbolt& owner)
{
    VERIFY(owner.current);

    // lightning model
    float dv = owner.lightning_phase * 0.5f;
    dv = (owner.lightning_phase > 0.5f) ? Random.randI(2) * 0.5f : dv;

    cmd_list.set_CullMode(CULL_NONE);
    u32 v_offset, i_offset;

    const dxThunderboltDescRender* pThRen = smart_cast<dxThunderboltDescRender*>(&*owner.current->m_pRender);

    const u32 vCount_Lock = pThRen->l_model->number_vertices;
    const u32 iCount_Lock = pThRen->l_model->number_indices;
    IRender_DetailModel::fvfVertexOut* v_ptr = (IRender_DetailModel::fvfVertexOut*)RImplementation.Vertex.Lock(vCount_Lock, hGeom_model->vb_stride, v_offset);
    u16* i_ptr = RImplementation.Index.Lock(iCount_Lock, i_offset);
    // XForm verts
    pThRen->l_model->transfer(owner.current_xform, v_ptr, 0xffffffff, i_ptr, 0, 0.f, dv);
    // Flush if needed
    RImplementation.Vertex.Unlock(vCount_Lock, hGeom_model->vb_stride);
    RImplementation.Index.Unlock(iCount_Lock);
    cmd_list.set_xform_world(Fidentity);
    cmd_list.set_Shader(pThRen->l_model->shader);
    cmd_list.set_Geometry(hGeom_model);
    cmd_list.Render(D3DPT_TRIANGLELIST, v_offset, 0, vCount_Lock, i_offset, iCount_Lock / 3);
    cmd_list.set_CullMode(CULL_CCW);

    // gradient
    Fvector vecSx, vecSy;
    u32 VS_Offset;
    FVF::LIT* pv = (FVF::LIT*)RImplementation.Vertex.Lock(8, hGeom_gradient.stride(), VS_Offset);
    // top
    {
        const u32 c_val = iFloor(owner.current->m_GradientTop->fOpacity * owner.lightning_phase * 255.f);
        const u32 c = color_rgba(c_val, c_val, c_val, c_val);
        vecSx.mul(Device.vCameraRight, owner.current->m_GradientTop->fRadius.x * owner.lightning_size);
        vecSy.mul(Device.vCameraTop, -owner.current->m_GradientTop->fRadius.y * owner.lightning_size);
        pv->set(owner.current_xform.c.x + vecSx.x - vecSy.x, owner.current_xform.c.y + vecSx.y - vecSy.y, owner.current_xform.c.z + vecSx.z - vecSy.z, c, 0, 0);
        pv++;
        pv->set(owner.current_xform.c.x + vecSx.x + vecSy.x, owner.current_xform.c.y + vecSx.y + vecSy.y, owner.current_xform.c.z + vecSx.z + vecSy.z, c, 0, 1);
        pv++;
        pv->set(owner.current_xform.c.x - vecSx.x - vecSy.x, owner.current_xform.c.y - vecSx.y - vecSy.y, owner.current_xform.c.z - vecSx.z - vecSy.z, c, 1, 0);
        pv++;
        pv->set(owner.current_xform.c.x - vecSx.x + vecSy.x, owner.current_xform.c.y - vecSx.y + vecSy.y, owner.current_xform.c.z - vecSx.z + vecSy.z, c, 1, 1);
        pv++;
    }
    // center
    {
        const u32 c_val = iFloor(owner.current->m_GradientTop->fOpacity * owner.lightning_phase * 255.f);
        const u32 c = color_rgba(c_val, c_val, c_val, c_val);
        vecSx.mul(Device.vCameraRight, owner.current->m_GradientCenter->fRadius.x * owner.lightning_size);
        vecSy.mul(Device.vCameraTop, -owner.current->m_GradientCenter->fRadius.y * owner.lightning_size);
        pv->set(owner.lightning_center.x + vecSx.x - vecSy.x, owner.lightning_center.y + vecSx.y - vecSy.y, owner.lightning_center.z + vecSx.z - vecSy.z, c, 0, 0);
        pv++;
        pv->set(owner.lightning_center.x + vecSx.x + vecSy.x, owner.lightning_center.y + vecSx.y + vecSy.y, owner.lightning_center.z + vecSx.z + vecSy.z, c, 0, 1);
        pv++;
        pv->set(owner.lightning_center.x - vecSx.x - vecSy.x, owner.lightning_center.y - vecSx.y - vecSy.y, owner.lightning_center.z - vecSx.z - vecSy.z, c, 1, 0);
        pv++;
        pv->set(owner.lightning_center.x - vecSx.x + vecSy.x, owner.lightning_center.y - vecSx.y + vecSy.y, owner.lightning_center.z - vecSx.z + vecSy.z, c, 1, 1);
        pv++;
    }
    RImplementation.Vertex.Unlock(8, hGeom_gradient.stride());
    cmd_list.set_xform_world(Fidentity);
    cmd_list.set_Geometry(hGeom_gradient);

    cmd_list.set_Shader(smart_cast<dxFlareRender*>(&*owner.current->m_GradientTop->m_pFlare)->hShader);
    //	Hack. Since lightning gradient uses sun shader override z write settings manually
    cmd_list.set_Z(TRUE);
    cmd_list.set_ZFunc(D3DCMP_LESSEQUAL);
    cmd_list.Render(D3DPT_TRIANGLELIST, VS_Offset, 0, 4, 0, 2);

    cmd_list.set_Shader(smart_cast<dxFlareRender*>(&*owner.current->m_GradientCenter->m_pFlare)->hShader);
    //	Hack. Since lightning gradient uses sun shader override z write settings manually
    cmd_list.set_Z(TRUE);
    cmd_list.set_ZFunc(D3DCMP_LESSEQUAL);
    cmd_list.Render(D3DPT_TRIANGLELIST, VS_Offset + 4, 0, 4, 0, 2);
}
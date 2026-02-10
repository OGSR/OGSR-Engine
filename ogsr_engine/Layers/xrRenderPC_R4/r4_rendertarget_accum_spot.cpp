#include "stdafx.h"
#include "../xrRender/du_cone.h"

void CRenderTarget::accum_spot(CBackend& cmd_list, light* L)
{
    phase_accumulator(cmd_list);
    RImplementation.stats.l_visible++;

    // *** assume accumulator already setup ***
    // *****************************	Mask by stencil		*************************************
    const bool omni = IRender_Light::OMNIPART == L->flags.type;
    ref_shader shader = omni ? L->s_point : L->s_spot;

    if (!shader)
        shader = omni ? s_accum_point : s_accum_spot;

    {
        // setup xform
        L->xform_calc();
        cmd_list.set_xform_world(L->m_xform);
        cmd_list.set_xform_view(Device.mView);
        cmd_list.set_xform_project(Device.mProject);

        cmd_list.set_Element(s_accum_mask->E[SE_MASK_SPOT]);
        geom_volume(cmd_list, L);

        // backfaces: if (stencil>=1 && zfail)			stencil = light_id
        cmd_list.set_CullMode(CULL_CW);
        cmd_list.set_Stencil(TRUE, D3DCMP_LESSEQUAL, dwLightMarkerID, 0x01, 0xff, D3DSTENCILOP_KEEP, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE);
        render_volume(cmd_list, L);

        // frontfaces: if (stencil>=light_id && zfail)	stencil = 0x1
        cmd_list.set_CullMode(CULL_CCW);
        cmd_list.set_Stencil(TRUE, D3DCMP_LESSEQUAL, 0x01, 0xff, 0xff, D3DSTENCILOP_KEEP, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE);
        render_volume(cmd_list, L);
    }

    // *****************************	Minimize overdraw	*************************************
    // Select shader (front or back-faces), *** back, if intersect near plane
    cmd_list.set_ColorWriteEnable();
    cmd_list.set_CullMode(CULL_CW); // back

    // Shadow xform (+texture adjustment matrix)
    Fmatrix m_Shadow, m_Lmap;
    {
        const float _smapsize = static_cast<float>(RImplementation.o.lights_smapsize);
        float fTexelOffs = (.5f / _smapsize);
        float view_dim = float(L->X.S.size - 2) / _smapsize;
        float view_sx = float(L->X.S.posX + 1) / _smapsize;
        float view_sy = float(L->X.S.posY + 1) / _smapsize;
        float fRange = float(1.f) * ps_r2_ls_depth_scale;
        float fBias = ps_r2_ls_depth_bias;
        Fmatrix m_TexelAdjust = {view_dim / 2.f,
                                 0.0f,
                                 0.0f,
                                 0.0f,
                                 0.0f,
                                 -view_dim / 2.f,
                                 0.0f,
                                 0.0f,
                                 0.0f,
                                 0.0f,
                                 fRange,
                                 0.0f,
                                 view_dim / 2.f + view_sx + fTexelOffs,
                                 view_dim / 2.f + view_sy + fTexelOffs,
                                 fBias,
                                 1.0f};

        // compute xforms
        Fmatrix xf_view = L->X.S.view;
        Fmatrix xf_project;
        xf_project.mul(m_TexelAdjust, L->X.S.project);
        m_Shadow.mul(xf_view, Device.mInvView);
        m_Shadow.mulA_44(xf_project);

        // lmap
        view_dim = 1.f;
        view_sx = 0.f;
        view_sy = 0.f;
        Fmatrix m_TexelAdjust2 = {view_dim / 2.f,
                                  0.0f,
                                  0.0f,
                                  0.0f,
                                  0.0f,
                                  -view_dim / 2.f,
                                  0.0f,
                                  0.0f,
                                  0.0f,
                                  0.0f,
                                  fRange,
                                  0.0f,
                                  view_dim / 2.f + view_sx + fTexelOffs,
                                  view_dim / 2.f + view_sy + fTexelOffs,
                                  fBias,
                                  1.0f};

        // compute xforms
        xf_project.mul(m_TexelAdjust2, L->X.S.project);
        m_Lmap.mul(xf_view, Device.mInvView);
        m_Lmap.mulA_44(xf_project);
    }

    // Common constants
    Fvector L_clr, L_pos;
    float L_spec;
    L_clr.set(L->color.r, L->color.g, L->color.b);
    L_clr.mul(L->get_LOD());
    L_spec = u_diffuse2s(L_clr);
    Device.mView.transform_tiny(L_pos, L->position);

    // Draw volume with projective texgen
    {
        // Select shader
        u32 _id = 0;
        if (L->flags.bShadow)
        {
            bool bFullSize = L->X.S.size == RImplementation.o.lights_smapsize;
            if (bFullSize)
                _id = SE_L_FULLSIZE;
            else
                _id = SE_L_NORMAL;
        }
        else
        {
            _id = SE_L_UNSHADOWED;
            m_Shadow = m_Lmap;
        }
        cmd_list.set_Element(shader->E[_id]);

        cmd_list.set_CullMode(CULL_CW); // back

        // Constants
        float att_R = L->range * .95f;
        float att_factor = 1.f / (att_R * att_R);
        cmd_list.set_c("Ldynamic_pos", L_pos.x, L_pos.y, L_pos.z, att_factor);
        cmd_list.set_c("Ldynamic_color", L_clr.x, L_clr.y, L_clr.z, L_spec);
        cmd_list.set_c("m_shadow", m_Shadow);
        cmd_list.set_ca("m_lmap", 0, m_Lmap._11, m_Lmap._21, m_Lmap._31, m_Lmap._41);
        cmd_list.set_ca("m_lmap", 1, m_Lmap._12, m_Lmap._22, m_Lmap._32, m_Lmap._42);

        cmd_list.set_Stencil(TRUE, D3DCMP_LESSEQUAL, dwLightMarkerID, 0xff, 0x00);
        render_volume(cmd_list, L);
    }

    increment_light_marker(cmd_list);

    if (ps_r2_ls_flags_ext.test(R2FLAGEXT_LENS_FLARE) && L->flags.bFlare)
        render_flare(cmd_list, L);
}

void CRenderTarget::accum_volumetric(CBackend& cmd_list, light* L)
{
    if (!L->flags.bVolumetric)
        return;

    //float dist_normalized = std::max(L->vis.distance - 20.0f, 0.0f);
    //float falloff = 1.f - std::min(dist_normalized * dist_normalized * 0.0001f, 1.0f);

    //if (falloff <= 0.f)
    //    return;

    phase_vol_accumulator(cmd_list, false);

    // setup xform
    L->xform_calc();
    cmd_list.set_xform_world(L->m_xform);
    cmd_list.set_xform_view(Device.mView);
    cmd_list.set_xform_project(Device.mProject);

    // Shadow xform (+texture adjustment matrix)
    Fmatrix m_Shadow;
    {
        const float _smapsize = static_cast<float>(RImplementation.o.lights_smapsize);
        float fTexelOffs = (.5f / _smapsize);
        float view_dim = float(L->X.S.size - 2) / _smapsize;
        float view_sx = float(L->X.S.posX + 1) / _smapsize;
        float view_sy = float(L->X.S.posY + 1) / _smapsize;
        float fRange = float(1.f) * ps_r2_ls_depth_scale;
        float fBias = ps_r2_ls_depth_bias;
        Fmatrix m_TexelAdjust = {
            view_dim / 2.f, 0.0f, 0.0f, 0.0f,
            0.0f, -view_dim / 2.f, 0.0f, 0.0f,
            0.0f, 0.0f, fRange, 0.0f,
            view_dim / 2.f + view_sx + fTexelOffs, view_dim / 2.f + view_sy + fTexelOffs, fBias, 1.0f
        };

        // compute xforms
        Fmatrix xf_view = L->X.S.view;
        Fmatrix xf_project;
        xf_project.mul(m_TexelAdjust, L->X.S.project);

        m_Shadow.set(xf_view);
        m_Shadow.mulA_44(xf_project);
    }

    Fvector L_dir, L_clr, L_pos;
    L_clr.set(L->color.r, L->color.g, L->color.b);
    L_clr.mul(L->m_volumetric_intensity);
    // L_clr.mul(falloff);
    // L_clr.mul(L->m_volumetric_distance); // SIMP: ????
    // L_clr.mul(L->get_LOD()); // SIMP: ????

    L_pos.set(L->position);
    L_dir.set(L->direction);
    L_dir.normalize();

    // Attenuation
    float att_R = L->m_volumetric_distance * L->range /** .95f*/;
    float att_factor = 1.f / (att_R * att_R);

    // Set the shader
    cmd_list.set_Element(s_accum_volume->E[0]);
    cmd_list.set_CullMode(CULL_CW);
    cmd_list.set_Stencil(FALSE);

    cmd_list.set_ColorWriteEnable();

    // Set constants
    cmd_list.set_c("Ldynamic_pos", L_pos.x, L_pos.y, L_pos.z, att_factor);
    cmd_list.set_c("Ldynamic_dir", L_dir.x, L_dir.y, L_dir.z, -cosf(L->cone * 0.5f));
    cmd_list.set_c("Ldynamic_color", L_clr.x, L_clr.y, L_clr.z, 1.f /*L->get_LOD()*/);

    cmd_list.set_c("m_shadow", m_Shadow);

    geom_volume(cmd_list, L);
    render_volume(cmd_list, L);
}
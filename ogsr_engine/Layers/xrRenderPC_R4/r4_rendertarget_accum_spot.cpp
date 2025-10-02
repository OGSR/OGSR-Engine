#include "stdafx.h"
#include "../xrRender/du_cone.h"

void CRenderTarget::accum_spot(CBackend& cmd_list, light* L)
{
    phase_accumulator(cmd_list);
    RImplementation.stats.l_visible++;

    // *** assume accumulator already setup ***
    // *****************************	Mask by stencil		*************************************
    ref_shader shader;
    if (IRender_Light::OMNIPART == L->flags.type)
    {
        shader = L->s_point;
        if (!shader)
        {
            shader = s_accum_point;
        }
    }
    else
    {
        shader = L->s_spot;
        if (!shader)
        {
            shader = s_accum_spot;
        }
    }

    {
        // setup xform
        L->xform_calc();
        cmd_list.set_xform_world(L->m_xform);
        cmd_list.set_xform_view(Device.mView);
        cmd_list.set_xform_project(Device.mProject);
        enable_scissor(L);

        // *** similar to "Carmack's reverse", but assumes convex, non intersecting objects,
        // *** thus can cope without stencil clear with 127 lights
        // *** in practice, 'cause we "clear" it back to 0x1 it usually allows us to > 200 lights :)
        //	Done in blender!
        // cmd_list.set_ColorWriteEnable		(FALSE);
        cmd_list.set_Element(s_accum_mask->E[SE_MASK_SPOT]); // masker

        // backfaces: if (stencil>=1 && zfail)			stencil = light_id
        cmd_list.set_CullMode(CULL_CW);
        cmd_list.set_Stencil(TRUE, D3DCMP_LESSEQUAL, dwLightMarkerID, 0x01, 0xff, D3DSTENCILOP_KEEP, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE);
        draw_volume(cmd_list, L);

        // frontfaces: if (stencil>=light_id && zfail)	stencil = 0x1
        cmd_list.set_CullMode(CULL_CCW);
        cmd_list.set_Stencil(TRUE, D3DCMP_LESSEQUAL, 0x01, 0xff, 0xff, D3DSTENCILOP_KEEP, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE);
        draw_volume(cmd_list, L);
    }

    // *****************************	Minimize overdraw	*************************************
    // Select shader (front or back-faces), *** back, if intersect near plane
    cmd_list.set_ColorWriteEnable();
    cmd_list.set_CullMode(CULL_CW); // back

    // 2D texgens
    Fmatrix m_Texgen;
    u_compute_texgen_screen(cmd_list, m_Texgen);
    Fmatrix m_Texgen_J;
    u_compute_texgen_jitter(cmd_list, m_Texgen_J);

    // Shadow xform (+texture adjustment matrix)
    Fmatrix m_Shadow, m_Lmap;
    {
        float smapsize = float(RImplementation.o.smapsize);
        float fTexelOffs = (.5f / smapsize);
        float view_dim = float(L->X.S.size - 2) / smapsize;
        float view_sx = float(L->X.S.posX + 1) / smapsize;
        float view_sy = float(L->X.S.posY + 1) / smapsize;
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
    Fvector L_dir, L_clr, L_pos;
    float L_spec;
    L_clr.set(L->color.r, L->color.g, L->color.b);
    L_clr.mul(L->get_LOD());
    L_spec = u_diffuse2s(L_clr);
    Device.mView.transform_tiny(L_pos, L->position);
    Device.mView.transform_dir(L_dir, L->direction);
    L_dir.normalize();

    // Draw volume with projective texgen
    {
        // Select shader
        u32 _id = 0;
        if (L->flags.bShadow)
        {
            bool bFullSize = (L->X.S.size == static_cast<u32>(RImplementation.o.smapsize));
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
        cmd_list.set_c("m_texgen", m_Texgen);
        cmd_list.set_c("m_texgen_J", m_Texgen_J);
        cmd_list.set_c("m_shadow", m_Shadow);
        cmd_list.set_ca("m_lmap", 0, m_Lmap._11, m_Lmap._21, m_Lmap._31, m_Lmap._41);
        cmd_list.set_ca("m_lmap", 1, m_Lmap._12, m_Lmap._22, m_Lmap._32, m_Lmap._42);

        cmd_list.set_Stencil(TRUE, D3DCMP_LESSEQUAL, dwLightMarkerID, 0xff, 0x00);
        draw_volume(cmd_list, L);
    }

    // dwLightMarkerID					+=	2;	// keep lowest bit always setted up
    increment_light_marker(cmd_list);

    if (ps_r2_ls_flags_ext.test(R2FLAGEXT_LENS_FLARE) && L->flags.bFlare)
        render_flare(cmd_list, L);
}

void CRenderTarget::accum_volumetric(CBackend& cmd_list, light* L)
{
    // if (L->flags.type != IRender_Light::SPOT) return;
    if (!L->flags.bVolumetric || !(ps_ssfx_volumetric.x > 0))
        return;

    phase_vol_accumulator(cmd_list);

    ref_shader shader;

    shader = L->s_volumetric;
    if (!shader)
    {
        shader = s_accum_volume;
    }

    // *** assume accumulator setted up ***
    // *****************************	Mask by stencil		*************************************

    {
        // setup xform
        L->xform_calc();
        cmd_list.set_xform_world(L->m_xform);
        cmd_list.set_xform_view(Device.mView);
        cmd_list.set_xform_project(Device.mProject);
        enable_scissor(L);
    }

    cmd_list.set_ColorWriteEnable();
    cmd_list.set_CullMode(CULL_NONE); // back

    // 2D texgens
    /*Fmatrix m_Texgen;
    u_compute_texgen_screen(cmd_list, m_Texgen);
    Fmatrix m_Texgen_J;
    u_compute_texgen_jitter(cmd_list, m_Texgen_J);*/

    // Shadow xform (+texture adjustment matrix)
    Fmatrix m_Shadow, m_Lmap;
    Fmatrix mFrustumSrc;
    CFrustum ClipFrustum;
    {
        float smapsize = float(RImplementation.o.smapsize);
        float fTexelOffs = (.5f / smapsize);
        float view_dim = float(L->X.S.size - 2) / smapsize;
        float view_sx = float(L->X.S.posX + 1) / smapsize;
        float view_sy = float(L->X.S.posY + 1) / smapsize;
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

        // Compute light frustum in world space
        mFrustumSrc.mul(L->X.S.project, xf_view);
        ClipFrustum.CreateFromMatrix(mFrustumSrc, FRUSTUM_P_ALL);
        //	Adjust frustum far plane
        //	4 - far, 5 - near
        ClipFrustum.planes[4].d -= (ClipFrustum.planes[4].d + ClipFrustum.planes[5].d) * (1 - L->m_volumetric_distance);
    }

    //	Calculate camera space AABB
    //	Adjust AABB according to the adjusted distance for the light volume
    Fbox aabb;

    // float	scaledRadius = L->spatial.sphere.R * (1+L->m_volumetric_distance)*0.5f;
    float scaledRadius = L->spatial.sphere.R * L->m_volumetric_distance;
    Fvector rr = Fvector().set(scaledRadius, scaledRadius, scaledRadius);
    Fvector pt = L->spatial.sphere.P;
    pt.sub(L->position);
    pt.mul(L->m_volumetric_distance);
    pt.add(L->position);
    //	Don't adjust AABB
    // float	scaledRadius = L->spatial.sphere.R;
    // Fvector	rr = Fvector().set(scaledRadius,scaledRadius,scaledRadius);
    // Fvector pt = L->spatial.sphere.P;
    Device.mView.transform(pt);
    aabb.setb(pt, rr);

    // Common constants
    float fQuality = L->m_volumetric_quality;
    int iNumSlises = 24; // (int)(VOLUMETRIC_SLICES* fQuality);
    //			min 10 surfaces
    if (L->flags.type == IRender_Light::OMNIPART)
        iNumSlises = 8;
    // iNumSlises = _max(1, iNumSlises);
    //	Adjust slice intensity
    fQuality = ((float)iNumSlises) / 24;
    Fvector L_clr, L_pos;
    float L_spec;
    L_clr.set(L->color.r, L->color.g, L->color.b);
    L_clr.mul(L->m_volumetric_intensity * ps_ssfx_volumetric.y);
    L_clr.mul(L->m_volumetric_distance);
    L_clr.mul(1 / fQuality);
    L_clr.mul(L->get_LOD());
    L_spec = u_diffuse2s(L_clr);
    Device.mView.transform_tiny(L_pos, L->position);

    // Draw volume with projective texgen
    {
        cmd_list.set_Element(shader->E[0]);

        // Constants
        float att_R = L->m_volumetric_distance * L->range * .95f;
        float att_factor = 1.f / (att_R * att_R);
        cmd_list.set_c("Ldynamic_pos", L_pos.x, L_pos.y, L_pos.z, att_factor);
        cmd_list.set_c("Ldynamic_color", L_clr.x, L_clr.y, L_clr.z, L_spec);
        //cmd_list.set_c("m_texgen", m_Texgen);
        //cmd_list.set_c("m_texgen_J", m_Texgen_J);
        cmd_list.set_c("m_shadow", m_Shadow);
        cmd_list.set_ca("m_lmap", 0, m_Lmap._11, m_Lmap._21, m_Lmap._31, m_Lmap._41);
        cmd_list.set_ca("m_lmap", 1, m_Lmap._12, m_Lmap._22, m_Lmap._32, m_Lmap._42);
        cmd_list.set_c("vMinBounds", aabb.x1, aabb.y1, aabb.z1, 0);
        //	Increase camera-space aabb z size to compensate decrease of slices number
        cmd_list.set_c("vMaxBounds", aabb.x2, aabb.y2, aabb.z1 + (aabb.z2 - aabb.z1) / fQuality, 0);

        //	Set up user clip planes
        {
            constexpr const char* strFrustumClipPlane = "FrustumClipPlane";
            //	TODO: DX10: Check if it's equivalent to the previouse code.
            // cmd_list.set_ClipPlanes (TRUE,ClipFrustum.planes,ClipFrustum.p_count);

            //	Transform frustum to clip space
            Fmatrix PlaneTransform;
            PlaneTransform.transpose(Device.mInvFullTransform);
            // HW.pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, 0x3F);

            for (int i = 0; i < 6; ++i)
            {
                Fvector4& ClipPlane = *(Fvector4*)&ClipFrustum.planes[i].n.x;
                Fvector4 TransformedPlane;
                PlaneTransform.transform(TransformedPlane, ClipPlane);
                TransformedPlane.mul(-1.0f);
                cmd_list.set_ca(strFrustumClipPlane, i, TransformedPlane);
                // HW.pDevice->SetClipPlane( i, &TransformedPlane.x);
            }
        }

        cmd_list.set_ColorWriteEnable(D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);

        cmd_list.set_Geometry(g_accum_volumetric);
        //	Igor: no need to do it per sub-sample. Plain AA will go just fine.
        cmd_list.Render(D3DPT_TRIANGLELIST, 0, 0, VOLUMETRIC_SLICES * 4, 0, VOLUMETRIC_SLICES * 2);

        cmd_list.set_ColorWriteEnable();
    }
}
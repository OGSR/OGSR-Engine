#include "stdafx.h"
#include "../../xr_3da/igame_persistent.h"
#include "../../xr_3da/environment.h"

//////////////////////////////////////////////////////////////////////////
// tables to calculate view-frustum bounds in world space
// note: D3D uses [0..1] range for Z
namespace accum_direct
{
constexpr Fvector3 corners[8]{
    { -1, -1, 0.7f }, { -1, -1, +1   },
    { -1, +1, +1   }, { -1, +1, 0.7f },
    { +1, +1, +1   }, { +1, +1, 0.7f },
    { +1, -1, +1   }, { +1, -1, 0.7f }
};

constexpr u16 facetable[16][3]{
    { 3, 2, 1 },
    { 3, 1, 0 },
    { 7, 6, 5 },
    { 5, 6, 4 },
    { 3, 5, 2 },
    { 4, 2, 5 },
    { 1, 6, 7 },
    { 7, 0, 1 },

    { 5, 3, 0 },
    { 7, 5, 0 },

    { 1, 4, 6 },
    { 2, 4, 1 },
};
} // namespace accum_direct
void CRenderTarget::accum_direct_cascade(CBackend& cmd_list, u32 sub_phase, Fmatrix& xform, Fmatrix& xform_prev, float fBias)
{
    // Choose normal code-path or filtered
    phase_accumulator(cmd_list);

    //	choose correct element for the sun shader
    u32 uiElementIndex = sub_phase;

    //	TODO: DX10: Remove half pixe offset
    // *** assume accumulator setted up ***
    light* fuckingsun = smart_cast<light*>(RImplementation.Lights.sun_adapted._get());

    // Common calc for quad-rendering
    u32 Offset;
    constexpr u32 C = color_rgba(255, 255, 255, 255);
    float _w = float(Device.dwWidth);
    float _h = float(Device.dwHeight);
    Fvector2 p0, p1;
    p0.set(.5f / _w, .5f / _h);
    p1.set((_w + .5f) / _w, (_h + .5f) / _h);
    float d_Z = EPS_S, d_W = 1.f;

    // Common constants (light-related)
    Fvector L_dir, L_clr;
    float L_spec;
    L_clr.set(fuckingsun->color.r, fuckingsun->color.g, fuckingsun->color.b);
    L_spec = u_diffuse2s(L_clr);
    Device.mView.transform_dir(L_dir, fuckingsun->direction);
    L_dir.normalize();

    // Perform masking (only once - on the first/near phase)
    cmd_list.set_CullMode(CULL_NONE);
    PIX_EVENT_CTX(cmd_list, SE_SUN_NEAR_sub_phase);
    if (SE_SUN_NEAR == sub_phase)
    {
        // Fill vertex buffer
        FVF::TL* pv = (FVF::TL*)RImplementation.Vertex.Lock(4, g_combine->vb_stride, Offset);
        pv->set(EPS, float(_h + EPS), d_Z, d_W, C, p0.x, p1.y);
        pv++;
        pv->set(EPS, EPS, d_Z, d_W, C, p0.x, p0.y);
        pv++;
        pv->set(float(_w + EPS), float(_h + EPS), d_Z, d_W, C, p1.x, p1.y);
        pv++;
        pv->set(float(_w + EPS), EPS, d_Z, d_W, C, p1.x, p0.y);
        pv++;
        RImplementation.Vertex.Unlock(4, g_combine->vb_stride);
        cmd_list.set_Geometry(g_combine);

        // setup
        float intensity = 0.3f * fuckingsun->color.r + 0.48f * fuckingsun->color.g + 0.22f * fuckingsun->color.b;
        Fvector dir = L_dir;
        dir.normalize().mul(-_sqrt(intensity + EPS));
        cmd_list.set_Element(s_accum_mask->E[SE_MASK_DIRECT]); // masker
        cmd_list.set_c("Ldynamic_dir", dir.x, dir.y, dir.z, 0);

        // if (stencil>=1 && aref_pass)	stencil = light_id
        //	Done in blender!
        // cmd_list.set_ColorWriteEnable	(FALSE		);
        cmd_list.set_Stencil(TRUE, D3DCMP_LESSEQUAL, dwLightMarkerID, 0x01, 0xff, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
        cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
    }

    PIX_EVENT_CTX(cmd_list, Perform_lighting);

    // Perform lighting
    {
        phase_accumulator(cmd_list);
        cmd_list.set_CullMode(CULL_CCW);
        cmd_list.set_ColorWriteEnable();

        const float fRange = (SE_SUN_NEAR == sub_phase) ? ps_r2_sun_depth_near_scale : ps_r2_sun_depth_far_scale;
        // float			fBias				= (SE_SUN_NEAR==sub_phase)?ps_r2_sun_depth_near_bias:ps_r2_sun_depth_far_bias;
        //	TODO: DX10: Remove this when fix inverse culling for far region
        //		float			fBias				= (SE_SUN_NEAR==sub_phase)?(-ps_r2_sun_depth_near_bias):ps_r2_sun_depth_far_bias;
        const Fmatrix m_TexelAdjust = {0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, fRange, 0.0f, 0.5f, 0.5f, fBias, 1.0f};

        // shadow xform
        Fmatrix m_shadow;
        {
            Fmatrix xf_project;
            xf_project.mul(m_TexelAdjust, fuckingsun->X.D[sub_phase].combine);
            m_shadow.mul(xf_project, Device.mInvView);

            // tsm-bias
            if ((SE_SUN_FAR == sub_phase))
            {
                Fvector bias;
                bias.mul(L_dir, ps_r2_sun_tsm_bias);
                Fmatrix bias_t;
                bias_t.translate(bias);
                m_shadow.mulB_44(bias_t);
            }
        }

        // clouds xform
        Fmatrix m_clouds_shadow;
        {
            static float w_shift = 0;
            Fmatrix m_xform;
            Fvector direction = fuckingsun->direction;
            float w_dir = g_pGamePersistent->Environment().CurrentEnv->wind_direction;
            float w_speed = g_pGamePersistent->Environment().CurrentEnv->wind_velocity * 0.001f;
            clamp(w_speed, 0.1f, 1.0f);
            Fvector normal;
            normal.setHP(-w_dir, 0);
            w_shift -= 0.005f * w_speed * Device.fTimeDelta;
            Fvector position;
            position.set(0, 0, 0);
            m_xform.build_camera_dir(position, direction, normal);
            Fvector localnormal;
            m_xform.transform_dir(localnormal, normal);
            localnormal.normalize();
            m_clouds_shadow.mul(m_xform, Device.mInvView);
            m_xform.scale(0.002f, 0.002f, 1.f);
            m_clouds_shadow.mulA_44(m_xform);
            m_xform.translate(localnormal.mul(w_shift));
            m_clouds_shadow.mulA_44(m_xform);
        }

        // Compute textgen texture for pixel shader, for possitions texture.
        Fmatrix m_Texgen;
        m_Texgen.identity();
        cmd_list.xforms.set_W(m_Texgen);
        cmd_list.xforms.set_V(Device.mView);
        cmd_list.xforms.set_P(Device.mProject);
        u_compute_texgen_screen(cmd_list, m_Texgen);

        // Fill vertex buffer
        u32 i_offset;
        {
            u16* pib = RImplementation.Index.Lock(sizeof(accum_direct::facetable) / sizeof(u16), i_offset);
            CopyMemory(pib, &accum_direct::facetable, sizeof(accum_direct::facetable));
            RImplementation.Index.Unlock(sizeof(accum_direct::facetable) / sizeof(u16));

            // corners

            constexpr u32 ver_count = sizeof(accum_direct::corners) / sizeof(Fvector3);
            Fvector4* pv = (Fvector4*)RImplementation.Vertex.Lock(ver_count, g_combine_cuboid.stride(), Offset);

            Fmatrix inv_XDcombine;
            if (/*ps_r2_ls_flags_ext.is(R2FLAGEXT_SUN_ZCULLING) &&*/ sub_phase == SE_SUN_FAR)
                inv_XDcombine.invert(xform_prev);
            else
                inv_XDcombine.invert(xform);

            for (u32 i = 0; i < ver_count; ++i)
            {
                Fvector3 tmp_vec;
                inv_XDcombine.transform(tmp_vec, accum_direct::corners[i]);
                pv->set(tmp_vec.x, tmp_vec.y, tmp_vec.z, 1);
                pv++;
            }
            RImplementation.Vertex.Unlock(ver_count, g_combine_cuboid.stride());
        }

        cmd_list.set_Geometry(g_combine_cuboid);

        // setup
        cmd_list.set_Element(s_accum_direct->E[uiElementIndex]);
        cmd_list.set_c("m_texgen", m_Texgen);
        cmd_list.set_c("Ldynamic_dir", L_dir.x, L_dir.y, L_dir.z, 0);
        cmd_list.set_c("Ldynamic_color", L_clr.x, L_clr.y, L_clr.z, L_spec);
        cmd_list.set_c("m_shadow", m_shadow);
        cmd_list.set_c("m_sunmask", m_clouds_shadow);

        if (sub_phase == SE_SUN_FAR)
        {
            Fvector3 view_viewspace;
            view_viewspace.set(0, 0, 1);

            m_shadow.transform_dir(view_viewspace);
            Fvector4 view_projlightspace;
            view_projlightspace.set(view_viewspace.x, view_viewspace.y, 0, 0);
            view_projlightspace.normalize();

            cmd_list.set_c("view_shadow_proj", view_projlightspace);
        }

        // Enable Z function only for near and middle cascades, the far one is restricted by only stencil.
        if (SE_SUN_NEAR == sub_phase || SE_SUN_MIDDLE == sub_phase)
            cmd_list.set_ZFunc(D3DCMP_GREATEREQUAL);
        else if (!ps_r2_ls_flags_ext.is(R2FLAGEXT_SUN_ZCULLING))
            cmd_list.set_ZFunc(D3DCMP_ALWAYS);
        else
            cmd_list.set_ZFunc(D3DCMP_LESS);

        u32 st_mask = 0xFE;
        _D3DSTENCILOP st_pass = D3DSTENCILOP_ZERO;

        if (sub_phase == SE_SUN_FAR)
        {
            st_mask = 0x00;
            st_pass = D3DSTENCILOP_KEEP;
        }

        // cmd_list.set_Stencil	(TRUE,D3DCMP_LESSEQUAL,dwLightMarkerID,0xff,0x00);
        cmd_list.set_Stencil(TRUE, D3DCMP_LESSEQUAL, dwLightMarkerID, 0xff, st_mask, D3DSTENCILOP_KEEP, st_pass, D3DSTENCILOP_KEEP);
        cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 8, i_offset, 16);

        // Igor: draw volumetric here
        if ((ps_r_sunshafts_mode & SS_VOLUMETRIC) && sub_phase == SE_SUN_FAR)
            accum_direct_volumetric(cmd_list, sub_phase, Offset, i_offset, m_shadow);
    }
}

void CRenderTarget::accum_direct_blend(CBackend& cmd_list)
{
    PIX_EVENT_CTX(cmd_list, accum_direct_blend);

    increment_light_marker(cmd_list);
}

void CRenderTarget::accum_direct_volumetric(CBackend& cmd_list, u32 sub_phase, const u32 Offset, const u32 i_offset, const Fmatrix& mShadow)
{
    PIX_EVENT_CTX(cmd_list, accum_direct_volumetric);

    if (!ps_r_sun_shafts)
        return;

    if (!need_to_render_sunshafts())
        return;

    if ((sub_phase != SE_SUN_NEAR) && (sub_phase != SE_SUN_MIDDLE) && (sub_phase != SE_SUN_FAR))
        return;

    phase_vol_accumulator(cmd_list);

    cmd_list.set_ColorWriteEnable();

    ref_selement& Element = s_accum_direct_volumetric->E[0];

    // Perform lighting
    {
        // *** assume accumulator setted up ***
        const light* fuckingsun = smart_cast<light*>(RImplementation.Lights.sun_adapted._get());

        // setup
        cmd_list.set_Element(Element);
        cmd_list.set_CullMode(CULL_CCW);

        //
        Fvector L_dir;
        Device.mView.transform_dir(L_dir, fuckingsun->direction);
        L_dir.normalize();
        // Msg("~~[%s] Ldynamic_dir is [%f, %f, %f]", __FUNCTION__, L_dir.x, L_dir.y, L_dir.z);
        cmd_list.set_c("Ldynamic_dir", L_dir.x, L_dir.y, L_dir.z, 0);

        /* Ещё вариант
        float intensity = 0.3f*fuckingsun->color.r + 0.48f*fuckingsun->color.g + 0.22f*fuckingsun->color.b;
        Fvector dir = L_dir;
        dir.normalize().mul(-_sqrt(intensity + EPS));
        //Msg("~~[%s] Ldynamic_dir is [%f, %f, %f]", __FUNCTION__, dir.x, dir.y, dir.z);
        cmd_list.set_c("Ldynamic_dir", dir.x, dir.y, dir.z, 0);
        */

        const Fvector L_clr = {fuckingsun->color.r, fuckingsun->color.g, fuckingsun->color.b};
        // Msg("~~[%s] Ldynamic_color (r,g,b) is [%f, %f, %f]", __FUNCTION__, L_clr.x, L_clr.y, L_clr.z);
        cmd_list.set_c("Ldynamic_color", L_clr.x, L_clr.y, L_clr.z, 0);

        cmd_list.set_c("m_shadow", mShadow);

        Fmatrix m_Texgen;
        m_Texgen.identity();
        cmd_list.xforms.set_W(m_Texgen);
        cmd_list.xforms.set_V(Device.mView);
        cmd_list.xforms.set_P(Device.mProject);
        u_compute_texgen_screen(cmd_list, m_Texgen);
        cmd_list.set_c("m_texgen", m_Texgen);

        if (SE_SUN_NEAR == sub_phase)
            cmd_list.set_ZFunc(D3DCMP_GREATER);
        else
            cmd_list.set_ZFunc(D3DCMP_ALWAYS);

        cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 8, i_offset, 16);
    }
}

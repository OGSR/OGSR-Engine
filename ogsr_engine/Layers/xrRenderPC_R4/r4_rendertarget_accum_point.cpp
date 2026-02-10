#include "stdafx.h"

#include "HUDInitializer.h"

void CRenderTarget::accum_point(CBackend& cmd_list, light* L)
{
    phase_accumulator(cmd_list);
    RImplementation.stats.l_visible++;

    ref_shader shader = L->s_point;
    if (!shader)
    {
        shader = s_accum_point;
    }

    CHUDTransformHelper initializer(cmd_list, false);

    if (L->flags.bHudMode)
    {
        initializer.SetHUDMode();
        RImplementation.rmNear(cmd_list);
    } 

    // Common
    Fvector L_pos;
    float L_spec;
    // float		L_R					= L->range;
    float L_R = L->range * .95f;
    Fvector L_clr;
    L_clr.set(L->color.r, L->color.g, L->color.b);
    L_spec = u_diffuse2s(L_clr);
    Device.mView.transform_tiny(L_pos, L->position);

    // Xforms
    L->xform_calc();
    cmd_list.set_xform_world(L->m_xform);
    cmd_list.set_xform_view(Device.mView);
    cmd_list.set_xform_project(Device.mProject);

    cmd_list.set_Element(s_accum_mask->E[SE_MASK_POINT]);
    geom_volume(cmd_list, L);

    cmd_list.set_CullMode(CULL_CW);
    cmd_list.set_Stencil(TRUE, D3DCMP_LESSEQUAL, dwLightMarkerID, 0x01, 0xff, D3DSTENCILOP_KEEP, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE);
    render_volume(cmd_list, L);

    // frontfaces: if (1<=stencil && zfail)	stencil = 0x1
    cmd_list.set_CullMode(CULL_CCW);
    cmd_list.set_Stencil(TRUE, D3DCMP_LESSEQUAL, 0x01, 0xff, 0xff, D3DSTENCILOP_KEEP, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE);
    render_volume(cmd_list, L);

    // *****************************	Minimize overdraw	*************************************
    // Select shader (front or back-faces), *** back, if intersect near plane
    cmd_list.set_ColorWriteEnable();
    cmd_list.set_CullMode(CULL_CW); // back
    /*
    if (bIntersect)	cmd_list.set_CullMode		(CULL_CW);		// back
    else			cmd_list.set_CullMode		(CULL_CCW);		// front
    */

    // 2D texgens
    Fmatrix m_Texgen;
    u_compute_texgen_screen(cmd_list, m_Texgen);

    // Draw volume with projective texgen
    {
        // Select shader
        u32 _id = 0;
        if (L->flags.bShadow)
        {
            bool bFullSize = (L->X.S.size == static_cast<u32>(RImplementation.o.lights_smapsize));
            if (bFullSize)
                _id = SE_L_FULLSIZE;
            else
                _id = SE_L_NORMAL;
        }
        else
        {
            _id = SE_L_UNSHADOWED;
        }
        cmd_list.set_Element(shader->E[_id]);

        // Constants
        cmd_list.set_c("Ldynamic_pos", L_pos.x, L_pos.y, L_pos.z, 1 / (L_R * L_R));
        cmd_list.set_c("Ldynamic_color", L_clr.x, L_clr.y, L_clr.z, L_spec);
        cmd_list.set_c("m_texgen", m_Texgen);

        cmd_list.set_CullMode(CULL_CW); // back
        // Render if (light_id <= stencil && z-pass)
        cmd_list.set_Stencil(TRUE, D3DCMP_EQUAL, dwLightMarkerID, 0xff, 0x00);
        render_volume(cmd_list, L);
    }

    // dwLightMarkerID					+=	2;	// keep lowest bit always setted up
    increment_light_marker(cmd_list);

    if (L->flags.bHudMode)
    {
        RImplementation.rmNormal(cmd_list);
        initializer.SetDefaultMode();
    }
}

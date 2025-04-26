#include "stdafx.h"

void CRenderTarget::draw_rain(CBackend& cmd_list, light& RainSetup)
{
    float fRainFactor = g_pGamePersistent->Environment().CurrentEnv->rain_density;

    // Common calc for quad-rendering
    u32 Offset;
    u32 C = color_rgba(255, 255, 255, 255);
    float _w = float(Device.dwWidth);
    float _h = float(Device.dwHeight);
    Fvector2 p0, p1;
    p0.set(.5f / _w, .5f / _h);
    p1.set((_w + .5f) / _w, (_h + .5f) / _h);
    float d_Z = EPS_S, d_W = 1.f;

    // Common constants (light-related)
    Fvector L_dir;
    Device.mView.transform_dir(L_dir, RainSetup.direction);
    L_dir.normalize();

    Fvector W_dirX;
    Device.mView.transform_dir(W_dirX, Fvector().set(1.0f, 0.0f, 0.0f));
    W_dirX.normalize();

    Fvector W_dirZ;
    Device.mView.transform_dir(W_dirZ, Fvector().set(0.0f, 0.0f, 1.0f));
    W_dirZ.normalize();

    // Perform masking (only once - on the first/near phase)
    // cmd_list.set_CullMode			(CULL_NONE	);
    // if (SE_SUN_NEAR==sub_phase)	//.
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
    }

    // recalculate d_Z, to perform depth-clipping
    const float fRainFar = ps_r3_dyn_wet_surf_far;

    Fvector center_pt;
    center_pt.mad(Device.vCameraPosition, Device.vCameraDirection, fRainFar);
    Device.mFullTransform.transform(center_pt);
    d_Z = center_pt.z;

    // Perform lighting
    {
        float fRange = 1;
        float fBias = -0.0001;
        float smapsize = float(RImplementation.o.rain_smapsize);
        float fTexelOffs = (.5f / smapsize);

        float view_dimX = float(RainSetup.X.D[0].maxX - RainSetup.X.D[0].minX) / smapsize;
        float view_dimY = float(RainSetup.X.D[0].maxX - RainSetup.X.D[0].minX) / smapsize;

        float view_sx = float(RainSetup.X.D[0].minX) / smapsize;
        float view_sy = float(RainSetup.X.D[0].minY) / smapsize;

        Fmatrix m_TexelAdjust = {view_dimX / 2.f,
                                 0.0f,
                                 0.0f,
                                 0.0f,
                                 0.0f,
                                 -view_dimY / 2.f,
                                 0.0f,
                                 0.0f,
                                 0.0f,
                                 0.0f,
                                 fRange,
                                 0.0f,
                                 view_dimX / 2.f + view_sx + fTexelOffs,
                                 view_dimY / 2.f + view_sy + fTexelOffs,
                                 fBias,
                                 1.0f};

        // compute xforms

        // shadow xform
        Fmatrix m_shadow;
        {
            Fmatrix xf_project;
            xf_project.mul(m_TexelAdjust, RainSetup.X.D[0].combine);
            m_shadow.mul(xf_project, Device.mInvView);
        }

        // clouds xform
        Fmatrix m_clouds_shadow;
        {
            constexpr float w_shift = 0;
            Fmatrix m_xform;
            Fvector normal;
            normal.setHP(1, 0);
            m_xform.identity();
            Fvector localnormal;
            m_xform.transform_dir(localnormal, normal);
            localnormal.normalize();
            m_clouds_shadow.mul(m_xform, Device.mInvView);
            m_xform.scale(1.f, 1.f, 1.f);
            m_clouds_shadow.mulA_44(m_xform);
            m_xform.translate(localnormal.mul(w_shift));
            m_clouds_shadow.mulA_44(m_xform);
        }

        // Make jitter texture
        Fvector2 j0, j1;
        float scale_X = float(Device.dwWidth) / float(TEX_jitter);
        float offset = (.5f / float(TEX_jitter));
        j0.set(offset, offset);
        j1.set(scale_X, scale_X).add(offset);

        // Fill vertex buffer
        FVF::TL2uv* pv = (FVF::TL2uv*)RImplementation.Vertex.Lock(3, g_combine_2UV->vb_stride, Offset);
        pv->set(-1, -1, d_Z, d_W, C, 0, 1, 0, scale_X);
        pv++;
        pv->set(-1, 3, d_Z, d_W, C, 0, -1, 0, -scale_X);
        pv++;
        pv->set(3, -1, d_Z, d_W, C, 2, 1, 2 * scale_X, scale_X);
        pv++;
        RImplementation.Vertex.Unlock(3, g_combine_2UV->vb_stride);
        cmd_list.set_Geometry(g_combine_2UV);

        //	Use for intermediate results
        //	Patch normal
        u_setrt(cmd_list, rt_Accumulator, nullptr, nullptr, rt_Base_Depth->pZRT[cmd_list.context_id]);

        cmd_list.set_Element(s_rain->E[1]);
        cmd_list.set_c("Ldynamic_dir", L_dir.x, L_dir.y, L_dir.z, 0);
        cmd_list.set_c("WorldX", W_dirX.x, W_dirX.y, W_dirX.z, 0);
        cmd_list.set_c("WorldZ", W_dirZ.x, W_dirZ.y, W_dirZ.z, 0);
        cmd_list.set_c("m_shadow", m_shadow);
        cmd_list.set_c("m_sunmask", m_clouds_shadow);
        cmd_list.set_c("RainDensity", fRainFactor, float(ps_r3_dyn_wet_surf_enable_streaks), 0, 0);
        cmd_list.set_c("RainFallof", ps_r3_dyn_wet_surf_near, ps_r3_dyn_wet_surf_far, 0, 0);

        cmd_list.set_Stencil(TRUE, D3DCMP_EQUAL, 0x01, 0x01, 0);
        cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 3, 0, 1);

        //	Apply normal
        cmd_list.set_Element(s_rain->E[2]);
        cmd_list.set_c("Ldynamic_dir", L_dir.x, L_dir.y, L_dir.z, 0);
        cmd_list.set_c("m_shadow", m_shadow);
        cmd_list.set_c("m_sunmask", m_clouds_shadow);

        u_setrt(cmd_list, rt_Position, nullptr, nullptr, rt_Base_Depth->pZRT[cmd_list.context_id]);

        cmd_list.set_Stencil(TRUE, D3DCMP_EQUAL, 0x01, 0x01, 0);
        cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 3, 0, 1);

        //	Apply gloss
        cmd_list.set_Element(s_rain->E[3]);
        cmd_list.set_c("Ldynamic_dir", L_dir.x, L_dir.y, L_dir.z, 0);
        cmd_list.set_c("m_shadow", m_shadow);
        cmd_list.set_c("m_sunmask", m_clouds_shadow);

        //	It is restored automatically by a set_Element call
        // StateManager.SetColorWriteEnable( D3Dxx_COLOR_WRITE_ENABLE_ALL );
        u_setrt(cmd_list, rt_Color, nullptr, nullptr, rt_Base_Depth->pZRT[cmd_list.context_id]);

        cmd_list.set_Stencil(TRUE, D3DCMP_EQUAL, 0x01, 0x01, 0);
        cmd_list.Render(D3DPT_TRIANGLELIST, Offset, 0, 3, 0, 1);
    }
}

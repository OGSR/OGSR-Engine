#include "stdafx.h"
#include "../../xr_3da/igame_persistent.h"

#include "r4_R_sun_support.h"

using namespace DirectX;

constexpr float tweak_rain_COP_initial_offs = 1200.f;
constexpr float tweak_rain_ortho_xform_initial_offs = 1000.f; //. ?

//////////////////////////////////////////////////////////////////////////
// tables to calculate view-frustum bounds in world space
// note: D3D uses [0..1] range for Z
namespace rain
{
constexpr Fvector3 corners[8]{
    {-1, -1, 0}, {-1, -1, +1},
    {-1, +1, +1}, {-1, +1, 0},
    {+1, +1, +1}, {+1, +1, 0},
    {+1, -1, +1}, {+1, -1, 0}
};
constexpr int facetable[6][4]{
    {0, 3, 5, 7}, {1, 2, 3, 0},
    {6, 7, 5, 4}, {4, 2, 1, 6},
    {3, 2, 4, 5}, {1, 0, 7, 6},
};
}

void render_rain::init()
{
    if (ps_ssfx_gloss_method == 0)
        rain_factor = g_pGamePersistent->Environment().CurrentEnv->rain_density;
    else
        rain_factor = g_pGamePersistent->Environment().wetness_factor;

    o.active = ps_r2_ls_flags.test(R3FLAG_DYN_WET_SURF);
    o.active &= rain_factor >= EPS_L;
    //o.active &= !Device.vCameraPositionSaved.similar(Device.vCameraPosition, EPS_L) || !Device.vCameraDirectionSaved.similar(Device.vCameraDirection, EPS_L);

    if (!o.active)
        return;

    o.mt_calc_enabled = ps_r2_ls_flags.test(R2FLAG_EXP_MT_RAIN); // RImplementation.o.mt_calculate;
    o.mt_draw_enabled = ps_r2_ls_flags.test(R2FLAG_EXP_MT_RAIN); // RImplementation.o.mt_render;

    // pre-allocate context
    context_id = RImplementation.alloc_context();
}

//////////////////////////////////////////////////////////////////////////
void render_rain::calculate()
{
    ZoneScoped;

    constexpr float source_offset = 10000.f;
    RainLight.direction.set(0.0f, -1.0f, 0.0f);
    RainLight.position.set(Device.vCameraPosition.x, Device.vCameraPosition.y + source_offset, Device.vCameraPosition.z);

    float fBoundingSphereRadius = 0;

    // calculate view-frustum bounds in world space
    Fmatrix ex_project{}, ex_full{}, ex_full_inverse{};
    {
        float fRainFar = 250.f;
        if (ps_ssfx_gloss_method == 0)
            fRainFar = ps_r3_dyn_wet_surf_far;

        ex_project.build_projection(deg2rad(Device.fFOV /* * Device.fASPECT*/), Device.fASPECT, VIEWPORT_NEAR, fRainFar);
        ex_full.mul(ex_project, Device.mView);

        XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&ex_full_inverse), XMMatrixInverse(nullptr, XMLoadFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&ex_full))));

        //	Calculate view frustum were we can see dynamic rain radius
        {
            //	b^2 = 2RH, B - side enge of the pyramid, h = height
            //	R = b^2/(2*H)
            const float H = fRainFar;
            const float a = tanf(deg2rad(Device.fFOV) / 2);
            const float c = tanf(deg2rad(Device.fFOV * Device.fASPECT) / 2);
            const float b_2 = H * H * (1.0f + a * a + c * c);
            fBoundingSphereRadius = b_2 / (2.0f * H);
        }
    }

    // Compute volume(s) - something like a frustum for infinite directional light
    // Also compute virtual light position and sector it is inside
    CFrustum cull_frustum;
    xr_vector<Fplane> cull_planes;
    Fvector3 cull_COP;
    Fmatrix cull_xform;
    {
        // Lets begin from base frustum
        Fmatrix fullxform_inv = ex_full_inverse;
#ifdef DEBUG
        typedef DumbConvexVolume<true> t_volume;
#else
        typedef DumbConvexVolume<false> t_volume;
#endif
        t_volume hull;
        {
            hull.points.reserve(9);
            for (const auto& corner : rain::corners)
            {
                hull.points.emplace_back(wform(fullxform_inv, corner));
            }
            for (auto& plane : rain::facetable)
            {
                hull.polys.emplace_back();
                for (int pt = 0; pt < 4; pt++)
                    hull.polys.back().points.push_back(plane[pt]);
            }
        }
        // hull.compute_caster_model	(cull_planes,fuckingsun->direction);
        hull.compute_caster_model(cull_planes, RainLight.direction);
#ifdef DEBUG
        for (u32 it = 0; it < cull_planes.size(); it++)
            RImplementation.Target->dbg_addplane(cull_planes[it], 0xffffffff);
#endif

        // COP - 100 km away
        cull_COP.mad(Device.vCameraPosition, RainLight.direction, -tweak_rain_COP_initial_offs);
        cull_COP.x += fBoundingSphereRadius * Device.vCameraDirection.x;
        cull_COP.z += fBoundingSphereRadius * Device.vCameraDirection.z;

        // Create frustum for query
        cull_frustum._clear();
        for (auto& cull_plane : cull_planes)
            cull_frustum._add(cull_plane);

        // Create approximate ortho-xform
        // view: auto find 'up' and 'right' vectors
        Fmatrix mdir_View, mdir_Project;
        Fvector L_dir, L_up, L_right, L_pos;
        L_pos.set(RainLight.position);
        L_dir.set(RainLight.direction).normalize();
        L_right.set(1, 0, 0);
        if (_abs(L_right.dotproduct(L_dir)) > .99f)
            L_right.set(0, 0, 1);
        L_up.crossproduct(L_dir, L_right).normalize();
        L_right.crossproduct(L_up, L_dir).normalize();
        mdir_View.build_camera_dir(L_pos, L_dir, L_up);

        // projection: box
        //	Simple
        Fbox frustum_bb;
        frustum_bb.invalidate();
        for (int it = 0; it < 8; it++)
        {
            // for (int it=0; it<9; it++)	{
            Fvector xf = wform(mdir_View, hull.points[it]);
            frustum_bb.modify(xf);
        }
        Fbox& bb = frustum_bb;
        bb.grow(EPS);

        //	HACK
        //	TODO: DX11: Calculate bounding sphere for view frustum
        //	TODO: DX11: Reduce resolution.
        // bb.vMin.x = -50;
        // bb.vMax.x = 50;
        // bb.vMin.y = -50;
        // bb.vMax.y = 50;

        //	Offset RainLight position to center rain shadowmap
        Fvector3 vRectOffset = {fBoundingSphereRadius * Device.vCameraDirection.x, 0, fBoundingSphereRadius * Device.vCameraDirection.z};
        bb.min.x = -fBoundingSphereRadius + vRectOffset.x;
        bb.max.x = fBoundingSphereRadius + vRectOffset.x;
        bb.min.y = -fBoundingSphereRadius + vRectOffset.z;
        bb.max.y = fBoundingSphereRadius + vRectOffset.z;

        XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&mdir_Project),
                        XMMatrixOrthographicOffCenterLH(bb.min.x, bb.max.x, bb.min.y, bb.max.y, bb.min.z - tweak_rain_ortho_xform_initial_offs, bb.min.z + 2 * tweak_rain_ortho_xform_initial_offs));

        cull_xform.mul(mdir_Project, mdir_View);

        s32 const limit = RImplementation.o.rain_smapsize;

        // build viewport xform
        float view_dim = float(limit);
        float fTexelOffs = .5f / RImplementation.o.rain_smapsize;

        Fmatrix m_viewport = {view_dim / 2.f, 0.0f, 0.0f, 0.0f, 0.0f, -view_dim / 2.f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, view_dim / 2.f + fTexelOffs, view_dim / 2.f + fTexelOffs, 0.0f, 1.0f};
        Fmatrix m_viewport_inv;
        XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&m_viewport_inv), XMMatrixInverse(nullptr, XMLoadFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&m_viewport))));

        // snap view-position to pixel
        //	snap zero point to pixel
        Fvector cam_proj = wform(cull_xform, Fvector().set(0, 0, 0));
        Fvector cam_pixel = wform(m_viewport, cam_proj);
        cam_pixel.x = floorf(cam_pixel.x);
        cam_pixel.y = floorf(cam_pixel.y);
        Fvector cam_snapped = wform(m_viewport_inv, cam_pixel);
        Fvector diff;
        diff.sub(cam_snapped, cam_proj);
        Fmatrix adjust;
        adjust.translate(diff);
        cull_xform.mulA_44(adjust);

        RainLight.X.D[0].minX = 0;
        RainLight.X.D[0].maxX = limit;
        RainLight.X.D[0].minY = 0;
        RainLight.X.D[0].maxY = limit;
    }

    // Begin SMAP-render
    auto& dsgraph = RImplementation.get_context(context_id);
    {
        dsgraph.phase = CRender::PHASE_SMAP;
        dsgraph.r_pmask(true, false);

        //dsgraph.o.sector_id = RImplementation.get_largest_sector();
        //dsgraph.o.xform = cull_xform;
        //dsgraph.o.view_frustum = cull_frustum;
        //dsgraph.o.view_pos = cull_COP;

        IRender_Sector::sector_id_t id = RImplementation.get_largest_sector();

        // Fill the database
        dsgraph.build_subspace(id, cull_frustum, cull_xform, cull_COP, TRUE);
    }

    // Finalize & Cleanup
    RainLight.X.D[0].combine = cull_xform;
}

void render_rain::render()
{
    if (o.active)
    {
        auto& dsgraph = RImplementation.get_context(context_id);

        // Render shadow-map
        {
            const bool bNormal = dsgraph.mapNormalCount > 0 || dsgraph.mapMatrixCount > 0;
            if (bNormal)
            {
                PIX_EVENT_CTX(dsgraph.cmd_list, RAIN);

                RImplementation.Target->phase_smap_direct(dsgraph.cmd_list, &RainLight, SE_SUN_RAIN_SMAP);
                dsgraph.cmd_list.set_xform_world(Fidentity);
                dsgraph.cmd_list.set_xform_view(Fidentity);
                dsgraph.cmd_list.set_xform_project(RainLight.X.D[0].combine);
                dsgraph.r_dsgraph_render_graph(0);
            }
        }
    }
}

void render_rain::flush()
{
    if (o.active)
    {
        auto& dsgraph = RImplementation.get_context(context_id);

        dsgraph.cmd_list.submit();
        RImplementation.release_context(context_id);
    }

    auto& cmd_list_imm = RImplementation.get_imm_context().cmd_list;

    cmd_list_imm.Invalidate();

    // Restore XForms
    cmd_list_imm.set_xform_world(Fidentity);
    cmd_list_imm.set_xform_view(Device.mView);
    cmd_list_imm.set_xform_project(Device.mProject);

    // Accumulate
    if (rain_factor >= EPS_L)
    {
        PIX_EVENT_CTX(cmd_list_imm, RainApply);

        cmd_list_imm.set_pass_targets(
            RImplementation.Target->rt_Color, /*rt_Normal*/
            nullptr,
            nullptr, 
            nullptr, 
            RImplementation.Target->rt_Base_Depth);

        RImplementation.Target->draw_rain(cmd_list_imm, RainLight);
    }
}

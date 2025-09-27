#include "stdafx.h"
#include "dxRainRender.h"

#include "../../xr_3da/Rain.h"


dxRainRender::dxRainRender()
{
    IReader* F = FS.r_open(fsgame::game_meshes, "dm\\rain.dm");
    ASSERT_FMT(F, "Can't open file [dm\\rain.dm]!");

    DM_Drop = ::RImplementation.model_CreateDM(F);

    SH_Rain.create("effects\\rain", "fx\\fx_rain");
    hGeom_Rain.create(FVF::F_LIT, RImplementation.Vertex.Buffer(), RImplementation.QuadIB);
    hGeom_Drops.create(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, RImplementation.Vertex.Buffer(), RImplementation.Index.Buffer());

    SH_Splash.create("effects\\rain_splash", "fx\\fx_rain");

    FS.r_close(F);
}

dxRainRender::~dxRainRender() { ::RImplementation.model_Delete(DM_Drop); }

void dxRainRender::Copy(IRainRender& _in) { *this = *smart_cast<dxRainRender*>(&_in); }

#include "../../xr_3da/iGame_persistent.h"

void dxRainRender::Render(CBackend& cmd_list, CEffect_Rain & owner)
{
    float factor = g_pGamePersistent->Environment().CurrentEnv->rain_density;
    if (factor < EPS_L)
        return;

    if (owner.items.empty())
        return;

    float _drop_len = ps_ssfx_rain_1.x;
    float _drop_width = ps_ssfx_rain_1.y;

    ref_shader& _splash_SH = SH_Splash;

    // visual
    float factor_visual = factor / 2.f + .5f;
    Fvector3 f_rain_color = g_pGamePersistent->Environment().CurrentEnv->rain_color;
    u32 u_rain_color = color_rgba_f(f_rain_color.x, f_rain_color.y, f_rain_color.z, factor_visual);

    // build source plane
    Fplane src_plane;
    constexpr Fvector norm{0.f, -1.f, 0.f};
    Fvector upper;
    upper.set(Device.vCameraPosition.x, Device.vCameraPosition.y + source_offset, Device.vCameraPosition.z);
    src_plane.build(upper, norm);

    // perform update
    u32 vOffset;
    FVF::LIT* verts = (FVF::LIT*)RImplementation.Vertex.Lock(owner.items.size() * 4, hGeom_Rain->vb_stride, vOffset);
    FVF::LIT* start = verts;
    const Fvector& vEye = Device.vCameraPosition;

    for (auto& one : owner.items)
    {
        // physics and time control
        // Build line
        Fvector& pos_head = one.P;
        Fvector pos_trail;
        pos_trail.mad(pos_head, one.D, -_drop_len * factor_visual);

        // Culling
        Fvector sC, lineD;
        float sR;
        sC.sub(pos_head, pos_trail);
        lineD.normalize(sC);
        sC.mul(.5f);
        sR = sC.magnitude();
        sC.add(pos_trail);

        if (!RImplementation.ViewBase.testSphere_dirty(sC, sR))
            continue;

        constexpr Fvector2 UV[2][4]{{{0, 1}, {0, 0}, {1, 1}, {1, 0}}, {{1, 0}, {1, 1}, {0, 0}, {0, 1}}};

        // Everything OK - build vertices
        Fvector P, lineTop, camDir;
        camDir.sub(sC, vEye);
        camDir.normalize();
        lineTop.crossproduct(camDir, lineD);
        float w = _drop_width;
        u32 s = one.uv_set;
        P.mad(pos_trail, lineTop, -w);
        verts->set(P, u_rain_color, UV[s][0].x, UV[s][0].y);
        verts++;
        P.mad(pos_trail, lineTop, w);
        verts->set(P, u_rain_color, UV[s][1].x, UV[s][1].y);
        verts++;
        P.mad(pos_head, lineTop, -w);
        verts->set(P, u_rain_color, UV[s][2].x, UV[s][2].y);
        verts++;
        P.mad(pos_head, lineTop, w);
        verts->set(P, u_rain_color, UV[s][3].x, UV[s][3].y);
        verts++;
    }
    u32 vCount = (u32)(verts - start);
    RImplementation.Vertex.Unlock(vCount, hGeom_Rain->vb_stride);

    // Render if needed
    if (vCount)
    {
        // HW.pDevice->SetRenderState	(D3DRS_CULLMODE,D3DCULL_NONE);
        cmd_list.set_CullMode(CULL_NONE);
        cmd_list.set_xform_world(Fidentity);
        cmd_list.set_Shader(SH_Rain);
        cmd_list.set_Geometry(hGeom_Rain);
        cmd_list.Render(D3DPT_TRIANGLELIST, vOffset, 0, vCount, 0, vCount / 2);
        // HW.pDevice->SetRenderState	(D3DRS_CULLMODE,D3DCULL_CCW);
        cmd_list.set_CullMode(CULL_CCW);
        cmd_list.set_c("ssfx_rain_setup", ps_ssfx_rain_2); // Alpha, Brigthness, Refraction, Reflection
    }

    // Particles
    CEffect_Rain::Particle* P = owner.particle_active;
    if (nullptr == P)
        return;

    {
        float dt = Device.fTimeDelta;
        _IndexStream& _IS = RImplementation.Index;
        cmd_list.set_Shader(_splash_SH);
        cmd_list.set_c("ssfx_rain_setup", ps_ssfx_rain_3); // Alpha, Refraction

        Fmatrix mXform, mScale;
        int pcount = 0;
        u32 v_offset, i_offset;
        u32 vCount_Lock = particles_cache * DM_Drop->number_vertices;
        u32 iCount_Lock = particles_cache * DM_Drop->number_indices;
        IRender_DetailModel::fvfVertexOut* v_ptr = (IRender_DetailModel::fvfVertexOut*)RImplementation.Vertex.Lock(vCount_Lock, hGeom_Drops->vb_stride, v_offset);
        u16* i_ptr = _IS.Lock(iCount_Lock, i_offset);
        while (P)
        {
            CEffect_Rain::Particle* next = P->next;

            // Update
            // P can be zero sometimes and it crashes
            P->time -= dt;
            if (P->time < 0)
            {
                owner.p_free(P);
                P = next;
                continue;
            }

            // Render
            if (RImplementation.ViewBase.testSphere_dirty(P->bounds.P, P->bounds.R) 
                && (fis_zero(ps_r2_no_details_radius) || Device.vCameraPosition.distance_to(P->bounds.P) > ps_r2_no_rain_radius))
            {
                // Build matrix
                float scale = P->time / particles_time;
                mScale.scale(scale, scale, scale);
                mXform.mul_43(P->mXForm, mScale);

                // XForm verts
                DM_Drop->transfer(mXform, v_ptr, u_rain_color, i_ptr, pcount * DM_Drop->number_vertices);
                v_ptr += DM_Drop->number_vertices;
                i_ptr += DM_Drop->number_indices;
                pcount++;

                if (pcount >= particles_cache)
                {
                    // flush
                    u32 dwNumPrimitives = iCount_Lock / 3;
                    RImplementation.Vertex.Unlock(vCount_Lock, hGeom_Drops->vb_stride);
                    _IS.Unlock(iCount_Lock);
                    cmd_list.set_Geometry(hGeom_Drops);
                    cmd_list.Render(D3DPT_TRIANGLELIST, v_offset, 0, vCount_Lock, i_offset, dwNumPrimitives);

                    v_ptr = (IRender_DetailModel::fvfVertexOut*)RImplementation.Vertex.Lock(vCount_Lock, hGeom_Drops->vb_stride, v_offset);
                    i_ptr = _IS.Lock(iCount_Lock, i_offset);

                    pcount = 0;
                }
            }

            P = next;
        }

        // Flush if needed
        vCount_Lock = pcount * DM_Drop->number_vertices;
        iCount_Lock = pcount * DM_Drop->number_indices;
        u32 dwNumPrimitives = iCount_Lock / 3;
        RImplementation.Vertex.Unlock(vCount_Lock, hGeom_Drops->vb_stride);
        _IS.Unlock(iCount_Lock);
        if (pcount)
        {
            cmd_list.set_Geometry(hGeom_Drops);
            cmd_list.Render(D3DPT_TRIANGLELIST, v_offset, 0, vCount_Lock, i_offset, dwNumPrimitives);
        }
    }
}

void dxRainRender::Calculate(CEffect_Rain& owner)
{
    const float factor = g_pGamePersistent->Environment().CurrentEnv->rain_density;
    if (factor < EPS_L)
        return;

    const float _drop_speed = ps_ssfx_rain_1.z;
    const size_t desired_items = iFloor(min_desired_items + (factor * (max_desired_items - min_desired_items)));

    // born _new_ if needed
    constexpr float b_radius_wrap_sqr = _sqr(source_radius * 1.5f);

    owner.items.reserve(desired_items);
    while (owner.items.size() < desired_items)
    {
        auto& one = owner.items.emplace_back();
        owner.Born(one, source_radius, _drop_speed);
    }

    // build source plane
    Fplane src_plane;
    constexpr Fvector norm{0.f, -1.f, 0.f};
    Fvector upper;
    upper.set(Device.vCameraPosition.x, Device.vCameraPosition.y + source_offset, Device.vCameraPosition.z);
    src_plane.build(upper, norm);

    const Fvector& vEye = Device.vCameraPosition;

    u32 total_cnt = owner.items.size();

    for (auto& one : owner.items)
    {
        // physics and time control
        if (one.dwTime_Hit < Device.dwTimeGlobal)
        {
            owner.Hit(one.Phit);
        }

        if (one.dwTime_Life < Device.dwTimeGlobal)
        {
            if (total_cnt <= desired_items)
                owner.Born(one, source_radius, _drop_speed);
            else
                total_cnt--;
        }

        const float dt = Device.fTimeDelta;
        one.P.mad(one.D, one.fSpeed * dt);

        Device.Statistic->dxRainRender.Begin();
        Fvector wdir;
        wdir.set(one.P.x - vEye.x, 0, one.P.z - vEye.z);
        float wlen = wdir.square_magnitude();
        if (wlen > b_radius_wrap_sqr)
        {
            wlen = _sqrt(wlen);

            if ((one.P.y - vEye.y) < sink_offset)
            {
                // need born
                one.invalidate();
            }
            else
            {
                Fvector inv_dir;
                inv_dir.invert(one.D);
                wdir.div(wlen);
                one.P.mad(one.P, wdir, -(wlen + source_radius));

                if (Fvector src_p; src_plane.intersectRayPoint(one.P, inv_dir, src_p))
                {
                    const float dist_sqr = one.P.distance_to_sqr(src_p);
                    float height = max_distance;

                    if (owner.RayPick(src_p, one.D, height, collide::rqtBoth))
                    {
                        if (_sqr(height) <= dist_sqr)
                        {
                            one.invalidate(); // need born
                        }
                        else
                        {
                            owner.RenewItem(one, height - _sqrt(dist_sqr), TRUE); // fly to point
                        }
                    }
                    else
                    {
                        owner.RenewItem(one, max_distance - _sqrt(dist_sqr), FALSE); // fly ...
                    }
                }
                else
                {
                    // need born
                    one.invalidate();
                }
            }
        }
        Device.Statistic->dxRainRender.End();
    }
}

const Fsphere& dxRainRender::GetDropBounds() const { return DM_Drop->bv_sphere; }

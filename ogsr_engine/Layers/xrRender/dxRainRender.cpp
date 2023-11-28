#include "stdafx.h"
#include "dxRainRender.h"
#include "../../xr_3da/perlin.h"

#include "../../xr_3da/Rain.h"

dxRainRender::dxRainRender()
{
    RainPerlin = std::make_unique<CPerlinNoise1D>(Random.randI(0, 0xFFFF));
    RainPerlin->SetOctaves(2);
    RainPerlin->SetAmplitude(0.66666f);
    IReader* F = FS.r_open("$game_meshes$", "dm\\rain.dm");
    VERIFY3(F, "Can't open file.", "dm\\rain.dm");

    DM_Drop = ::RImplementation.model_CreateDM(F);

    SH_Rain.create("effects\\rain", "fx\\fx_rain");
    hGeom_Rain.create(FVF::F_LIT, RCache.Vertex.Buffer(), RCache.QuadIB);
    hGeom_Drops.create(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, RCache.Vertex.Buffer(), RCache.Index.Buffer());

    SH_Splash.create("effects\\rain_splash", "fx\\fx_rain");

    FS.r_close(F);
}

dxRainRender::~dxRainRender() { ::RImplementation.model_Delete(DM_Drop); }

#include "../../xr_3da/iGame_persistent.h"

void dxRainRender::Render(CEffect_Rain& owner)
{
    const float factor = g_pGamePersistent->Environment().CurrentEnv->rain_density;
    if (factor < EPS_L)
        return;

    ref_shader& _splash_SH = SH_Splash;
    constexpr const char* s_shader_setup = "ssfx_rain_setup";

    // visual
    const float factor_visual = factor / 2.f + .5f;
    const Fvector3& f_rain_color = g_pGamePersistent->Environment().CurrentEnv->rain_color;
    const u32 u_rain_color = color_rgba_f(f_rain_color.x, f_rain_color.y, f_rain_color.z, factor_visual);

    if (!owner.items.empty())
    {

    const float _drop_len = ps_ssfx_rain_1.x;
    const float _drop_width = ps_ssfx_rain_1.y;

    // perform update
    u32 vOffset;
    FVF::LIT* verts = (FVF::LIT*)RCache.Vertex.Lock(owner.items.size() * 4, hGeom_Rain->vb_stride, vOffset);
    FVF::LIT* start = verts;
    const Fvector& vEye = Device.vCameraPosition;

    for (const auto& one : owner.items)
    {
        // Build line
        const Fvector& pos_head = one.P;
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
        if (!::Render->ViewBase.testSphere_dirty(sC, sR))
            continue;

        constexpr Fvector2 UV[2][4] = {{{0, 1}, {0, 0}, {1, 1}, {1, 0}}, {{1, 0}, {1, 1}, {0, 0}, {0, 1}}};

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
    RCache.Vertex.Unlock(vCount, hGeom_Rain->vb_stride);

    // Render if needed
    if (vCount)
    {
        // HW.pDevice->SetRenderState	(D3DRS_CULLMODE,D3DCULL_NONE);
        RCache.set_CullMode(CULL_NONE);
        RCache.set_xform_world(Fidentity);
        RCache.set_Shader(SH_Rain);
        RCache.set_Geometry(hGeom_Rain);
        RCache.Render(D3DPT_TRIANGLELIST, vOffset, 0, vCount, 0, vCount / 2);
        // HW.pDevice->SetRenderState	(D3DRS_CULLMODE,D3DCULL_CCW);
        RCache.set_CullMode(CULL_CCW);
        RCache.set_c(s_shader_setup, ps_ssfx_rain_2); // Alpha, Brigthness, Refraction, Reflection
    }

    }

    // Particles
    CEffect_Rain::Particle* P = owner.particle_active;
    if (0 == P)
        return;

    {
        float dt = Device.fTimeDelta;
        _IndexStream& _IS = RCache.Index;
        RCache.set_Shader(_splash_SH);
        RCache.set_c(s_shader_setup, ps_ssfx_rain_3); // Alpha, Refraction

        Fmatrix mXform, mScale;
        u32 pcount = 0;
        u32 v_offset, i_offset;
        u32 vCount_Lock = particles_cache * DM_Drop->number_vertices;
        u32 iCount_Lock = particles_cache * DM_Drop->number_indices;
        IRender_DetailModel::fvfVertexOut* v_ptr = (IRender_DetailModel::fvfVertexOut*)RCache.Vertex.Lock(vCount_Lock, hGeom_Drops->vb_stride, v_offset);
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
            if (::Render->ViewBase.testSphere_dirty(P->bounds.P, P->bounds.R))
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
                    RCache.Vertex.Unlock(vCount_Lock, hGeom_Drops->vb_stride);
                    _IS.Unlock(iCount_Lock);
                    RCache.set_Geometry(hGeom_Drops);
                    RCache.Render(D3DPT_TRIANGLELIST, v_offset, 0, vCount_Lock, i_offset, dwNumPrimitives);

                    v_ptr = (IRender_DetailModel::fvfVertexOut*)RCache.Vertex.Lock(vCount_Lock, hGeom_Drops->vb_stride, v_offset);
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
        RCache.Vertex.Unlock(vCount_Lock, hGeom_Drops->vb_stride);
        _IS.Unlock(iCount_Lock);
        if (pcount)
        {
            RCache.set_Geometry(hGeom_Drops);
            RCache.Render(D3DPT_TRIANGLELIST, v_offset, 0, vCount_Lock, i_offset, dwNumPrimitives);
        }
    }
}

void dxRainRender::Calculate(CEffect_Rain& owner)
{
    float factor = g_pGamePersistent->Environment().CurrentEnv->rain_density;
    if (factor < EPS_L)
        return;

    float _drop_speed = ps_ssfx_rain_1.z;

    // Prepare correct angle and distance to hit the player
    Fvector Rain_Axis{0.f, -1.f, 0.f};
    Fvector2 Rain_Offset;

    const float Wind_Direction = -g_pGamePersistent->Environment().CurrentEnv->wind_direction;

    // Wind Velocity [ From 0 ~ 1000 to 0 ~ 1 ]
    float Wind_Velocity = g_pGamePersistent->Environment().CurrentEnv->wind_velocity * 0.001f;
    clamp(Wind_Velocity, 0.0f, 1.0f);

    auto Prepare = [&] {
        // Wind direction

        // Wind gust, to add variation.
        const float Wind_Gust = RainPerlin->GetContinious(Device.fTimeGlobal * 0.3f) * 2.0f;

        // Wind velocity [ 0 ~ 1 ]
        float _Wind_Velocity = Wind_Velocity + Wind_Gust;

        if (ps_ssfx_wind.x > 0) // Debug
            _Wind_Velocity = ps_ssfx_wind.x;

        clamp(_Wind_Velocity, 0.0f, 1.0f);

        // Wind velocity controles the angle
        const float pitch = drop_max_angle * _Wind_Velocity;
        Rain_Axis.setHP(Wind_Direction, pitch - PI_DIV_2);

        // Get distance
        float dist = _sin(pitch) * source_offset;
        const float C = PI_DIV_2 - pitch;
        dist /= _sin(C);

        // 0 is North
        const float fixNorth = Wind_Direction - PI_DIV_2;

        // Set offset
        Rain_Offset.set(dist * _cos(fixNorth), dist * _sin(fixNorth));
    };

    Prepare();

    const size_t desired_items = iFloor(min_desired_items + (factor * (max_desired_items - min_desired_items)));

    if (size_t sz = owner.items.size(); sz < desired_items)
    {
        // born _new_ if needed
        for (; sz < desired_items; sz++)
        {
            CEffect_Rain::Item one;
            owner.Born(one, source_radius, _drop_speed, Wind_Velocity, Rain_Offset, Rain_Axis);
            owner.items.emplace_back(std::move(one));
        }
    }
    else
    {
        owner.items.resize(desired_items);
    }

    // build source plane
    float b_radius_wrap_sqr = _sqr((source_radius * 1.5f));
    Fplane src_plane;
    Fvector norm = {0.f, -1.f, 0.f};
    Fvector upper;
    upper.set(Device.vCameraPosition.x, Device.vCameraPosition.y + source_offset, Device.vCameraPosition.z);
    src_plane.build(upper, norm);

    const Fvector& vEye = Device.vCameraPosition;

    for (auto& one : owner.items)
    {
        if (one.dwTime_Hit < Device.dwTimeGlobal)
        {
            owner.Hit(one.Phit);
        }

        if (one.dwTime_Life < Device.dwTimeGlobal)
        {
            owner.Born(one, source_radius, _drop_speed, Wind_Velocity, Rain_Offset, Rain_Axis);
        }

        float dt = Device.fTimeDelta;
        one.P.mad(one.D, one.fSpeed * dt);

        Device.Statistic->TEST1.Begin();
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
                Fvector inv_dir, src_p;
                inv_dir.invert(one.D);
                wdir.div(wlen);
                one.P.mad(one.P, wdir, -(wlen + source_radius));
                if (src_plane.intersectRayPoint(one.P, inv_dir, src_p))
                {
                    float dist_sqr = one.P.distance_to_sqr(src_p);
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
        Device.Statistic->TEST1.End();
    }
}

const Fsphere& dxRainRender::GetDropBounds() const { return DM_Drop->bv_sphere; }

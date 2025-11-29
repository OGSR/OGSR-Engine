#include "stdafx.h"
#include "../../xr_3da/ps_instance.h"
#include "../../xr_3da/customhud.h"
#include "../xrRender/FBasicVisual.h"
#include "../../xr_3da/XR_IOConsole.h"
#include "../../xr_3da/xr_ioc_cmd.h"

float g_fSCREEN;

extern float r_dtex_paralax_range;
extern float r_ssaDISCARD;
extern float r_ssaLOD_A;
extern float r_ssaLOD_B;
extern float r_ssaGLOD_start, r_ssaGLOD_end;

void render_main::init()
{
}

void render_main::calculate_static()
{
    static_calc_frame = Device.dwFrame;

    static_waiter = TTAPI->submit([] {
        ZoneScoped;

        RImplementation.HOM.DispatchRender();

        auto& dsgraph_main = RImplementation.get_imm_context();

        Device.Statistic->RenderCALC.Begin();
        dsgraph_main.r_pmask(true, true, true); // enable priority "0,1",+ capture wmarks

        dsgraph_main.main_pass = true;
        dsgraph_main.phase = CRender::PHASE_NORMAL;
        dsgraph_main.marker++;

        RImplementation.main_pass_static(dsgraph_main);
    });
}

void render_main::ensure_calculate_static()
{
    if (static_calc_frame != Device.dwFrame)
    {
        calculate_static();
    }
}

void render_main::calculate_dynamic()
{
    dynamic_waiter = TTAPI->submit([] {
        ZoneScoped;

        // ждем партиклы тут
        RImplementation.calculate_particles_wait();

        // ждем кости тут же
        RImplementation.calculate_bones_wait();

        auto& dsgraph_main = RImplementation.get_imm_context();

        RImplementation.main_pass_dynamic(dsgraph_main, true);

        Device.Statistic->RenderCALC.End();
    });
}

void render_main::wait_static() const
{
    if (static_waiter.valid())
        static_waiter.wait();
}

void render_main::wait_dynamic() const
{
    if (dynamic_waiter.valid())
        dynamic_waiter.wait();
}

void render_main::sync() const
{
    // just to be safe

    wait_static();

    wait_dynamic();
}


// IC bool pred_sp_sort(ISpatial* _1, ISpatial* _2)
//{
//     float d1 = _1->spatial.sphere.P.distance_to_sqr(Device.vCameraPosition);
//     float d2 = _2->spatial.sphere.P.distance_to_sqr(Device.vCameraPosition);
//     return d1 < d2;
// }

constexpr u32 batch_size = 5;

void CRender::calculate_particles_async()
{
    if (!ps_r2_ls_flags_ext.test(R2FLAGEXT_DISABLE_PARTICLES))
    {
        CFrustum v{};
        v.CreateFromMatrix(Device.mFullTransform, FRUSTUM_P_LRTB + FRUSTUM_P_FAR);

        g_SpatialSpace->q_frustum(lstParticlesCalculation, 0, STYPE_RENDERABLE | STYPE_PARTICLE, v);

        //Msg("Particles calculation: %d objects", lstParticlesCalculation.size());

        auto calculate_particles = [this] {
            ZoneScoped;

            CTimer t_total;

            t_total.Start();

            // (almost) Exact sorting order (front-to-back)
            // std::sort(lstParticlesCalculation.begin(), lstParticlesCalculation.end(), pred_sp_sort);

            xr_vector<CPS_Instance*> batch;

            // Traverse
            for (const auto& spatial : lstParticlesCalculation)
            {
                if (const auto ps = smart_cast<CPS_Instance*>(spatial))
                {
                    // is it correct ???

                    // vis_data& v_orig = ps->renderable.visual->getVisData();

                    // Fvector pos;
                    // ps->renderable.xform.transform_tiny(pos, v_orig.sphere.P);

                    // if (!ps->renderable.visual->ignore_optimization && !InFieldOfViewR(pos, ps_r__opt_dist, false))
                    //    continue;

                    batch.emplace_back(ps);

                    if (batch.size() > batch_size)
                    {
                        const xr_vector<CPS_Instance*> t = batch;

                        batch.clear();

                        particles_pool.submit_detach([](const xr_vector<CPS_Instance*>& l) {
                            for (CPS_Instance* instance : l)
                            {
                                instance->PerformFrame();   
                            }
                        }, t);
                    }
                }
            }

            if (!batch.empty())
            {
                const xr_vector<CPS_Instance*> t = batch;

                batch.clear();

                particles_pool.submit_detach(
                    [](const xr_vector<CPS_Instance*>& l) {
                        for (CPS_Instance* instance : l)
                        {
                            instance->PerformFrame();
                        }
                    },
                    t);
            }

            particles_pool.wait_for_tasks();

            if (t_total.GetElapsed_ms() > 5)
            {
                MsgDbg("Long PerformAllTheWork !!! duration [%d]ms. updated: %d objects!", t_total.GetElapsed_ms(), lstParticlesCalculation.size());
            }
        };

        if (ps_r2_ls_flags.test(R2FLAG_EXP_MT_PARTICLES))
        {
            particles_async_awaiter = TTAPI->submit(calculate_particles);
        }
        else
        {
            calculate_particles();
        }
    }
}

void CRender::calculate_particles_wait()
{
    if (particles_async_awaiter.valid())
    {
        particles_async_awaiter.wait();
    }
}

#include "../../Include/xrRender/Kinematics.h"

void CRender::calculate_bones_async()
{
    if (ps_r2_ls_flags.test(R2FLAG_EXP_MT_BONES))
    {
        CFrustum v{};
        v.CreateFromMatrix(Device.mFullTransform, FRUSTUM_P_LRTB + FRUSTUM_P_FAR);

        g_SpatialSpace->q_frustum(lstBonesCalculation, 0, STYPE_RENDERABLE, v);

        auto calculate_bones = [this] {
            ZoneScoped;

            // (almost) Exact sorting order (front-to-back)
            // std::sort(lstBonesCalculation.begin(), lstBonesCalculation.end(), pred_sp_sort);
#pragma todo("Simp: Раскомментировать когда будет новый TTP")
            /*if (lstBonesCalculation.size() > 12)
            {
                 poolstl::for_each_chunk(poolstl::par.on(*TTAPI)
                    , lstBonesCalculation.begin()
                    , lstBonesCalculation.end(), [](auto spatial) 
                    {
                        if (IRenderable* renderable = spatial->dcast_Renderable())
                        {
                            if (renderable->renderable.visual && renderable->renderable.visual->dcast_PKinematics())
                                if (IKinematics* pKin = renderable->renderable.visual->dcast_PKinematics())
                                {
                                    pKin->CalculateBones(TRUE);
                                }
                        }
                    });
            }
            else*/
            {
                // Traverse frustums
                for (auto spatial : lstBonesCalculation)
                {
                    if (IRenderable* renderable = spatial->dcast_Renderable())
                    {
                        if (!renderable->renderable.visual)
                            continue;
                        if (!renderable->renderable.visual->dcast_PKinematics())
                            continue;

                        if (IKinematics* pKin = renderable->renderable.visual->dcast_PKinematics())
                        {
                            pKin->CalculateBones(TRUE);
                        }
                    }
                }
            }
        };

        bones_async_awaiter = TTAPI->submit(calculate_bones);
    }
}

void CRender::calculate_bones_wait()
{
    if (bones_async_awaiter.valid())
    {
        bones_async_awaiter.wait();
    }
}

bool CRender::ShouldSkipRender()
{
    IMainMenu* pMainMenu = g_pGamePersistent ? g_pGamePersistent->m_pMainMenu : nullptr;
    const bool bMenu = pMainMenu ? pMainMenu->CanSkipSceneRendering() : false;

    if (!(g_pGameLevel && g_hud) || bMenu /*|| (!ps_r2_ls_flags_ext.test(R2FLAGEXT_RENDER_ON_PREFETCH) && Device.dwPrecacheFrame > 0)*/)
    {
        return true;
    }
    return false;
}

void CRender::ExportLights()
{
    ZoneScoped;

    g_SpatialSpace->q_sphere(lstLights, 0, STYPE_LIGHTSOURCE, Device.vCameraPosition, EPS_L);

    for (const auto& spatial : lstLights)
    {
        update_sector(light_sectors_xrc, spatial);

        const auto& sector_id = spatial->spatial.sector_id;
        if (sector_id == IRender_Sector::INVALID_SECTOR_ID)
        {
            continue; // disassociated from S/P structure
        }

        VERIFY(spatial->spatial.type & STYPE_LIGHTSOURCE);

        // lightsource
        light* L = smart_cast<light*>(spatial->dcast_Light());
        R_ASSERT(L);

        //const float lod = L->get_LOD();
        //if (lod > EPS_L)
        {
            Lights.add_light(L);
        }
    }

    g_SpatialSpace->q_frustum(lstLights, 0, STYPE_LIGHTSOURCE, ViewBase);

    // Traverse frustums
    for (auto spatial : lstLights)
    {
        // lightsource
        light* L = smart_cast<light*>(spatial->dcast_Light());
        R_ASSERT(L);

        if (Device.dwFrame == L->frame_render)
            continue;

        update_sector(light_sectors_xrc, spatial);

        const auto& sector_id = spatial->spatial.sector_id;
        if (sector_id == IRender_Sector::INVALID_SECTOR_ID)
        {
            continue; // disassociated from S/P structure
        }

        const float lod = L->get_LOD();
        if (lod > EPS_L)
        {
            bool can_add = true;

            if (HOM.Allowed())
            {
                vis_data& vis = L->get_homdata();
                can_add = HOM.visible(vis);
            }

            if (can_add)
                Lights.add_light(L);
        }
    }
}

void CRender::UpdateSectors()
{
    ZoneScoped;

    static CFrustum v{};
    v.CreateFromMatrix(Device.mFullTransform, FRUSTUM_P_LRTB + FRUSTUM_P_FAR);

    g_SpatialSpace->q_frustum(lstUpdateSector, 0, STYPE_RENDERABLE, v);

    // (almost) Exact sorting order (front-to-back)
    // std::sort(lstParticlesCalculation.begin(), lstParticlesCalculation.end(), pred_sp_sort);

#pragma todo("Simp: Раскомментировать когда будет новый TTP")
    /*if (lstUpdateSector.size() > 16)
    {
        poolstl::for_each_chunk(poolstl::par.on(*TTAPI)
                    , lstUpdateSector.begin()
                    , lstUpdateSector.end(), [this](auto spatial) 
                    {
                        update_sector(renderable_sectors_xrc, spatial);
                    });
    }
    else*/
    {
        // Traverse frustums
        for (auto spatial : lstUpdateSector)
        {
            update_sector(renderable_sectors_xrc, spatial);
        }
    }
}

void CRender::Calculate()
{
    if (!g_pGameLevel)
        return;

    ZoneScopedN("r2_calculate");

    //IMainMenu* pMainMenu = g_pGamePersistent ? g_pGamePersistent->m_pMainMenu : 0;
    //bool bMenu = pMainMenu ? pMainMenu->CanSkipSceneRendering() : false;

    //if (!(g_pGameLevel && g_hud) || bMenu)
    //{
    //    return;
    //}

    // Transfer to global space to avoid deep pointer access
    IRender_Target* T = getTarget();
    const float fov_factor = _sqr(90.f / Device.fFOV);
    g_fSCREEN = float(T->get_width(RCache) * T->get_height(RCache)) * fov_factor * (EPS_S + ps_r__LOD);

    r_ssaDISCARD = _sqr(ps_r__ssaDISCARD) / g_fSCREEN;

    r_ssaLOD_A = _sqr(ps_r2_ssaLOD_A / 3) / g_fSCREEN;
    r_ssaLOD_B = _sqr(ps_r2_ssaLOD_B / 3) / g_fSCREEN;

    r_ssaGLOD_start = _sqr(ps_r__GLOD_ssa_start / 3) / g_fSCREEN;
    r_ssaGLOD_end = _sqr(ps_r__GLOD_ssa_end / 3) / g_fSCREEN;

    r_dtex_paralax_range = ps_r2_df_parallax_range * g_fSCREEN / (1024.f * 768.f);

    o.distortion = o.distortion_enabled;

    if (m_bFirstFrameAfterReset)
        return;

    std::copy(std::begin(grass_shader_data.pos), std::end(grass_shader_data.pos), std::begin(grass_shader_data_old.pos));
    std::copy(std::begin(grass_shader_data.dir), std::end(grass_shader_data.dir), std::begin(grass_shader_data_old.dir));

    //
    Lights.UpdateSun();

    // Clear selection
    Lights.package.clear();

    if (ShouldSkipRender())
    {
        return;
    }

    calculate_particles_async();
    calculate_bones_async();

    update_sectors_awaiter = TTAPI->submit([this] { UpdateSectors(); });
    light_waiter = TTAPI->submit([this] { ExportLights(); });

    if (ps_r2_ls_flags.test(R2FLAG_EXP_MT_RAIN))
    {
        g_pGamePersistent->Environment().StartCalculateAsync();
    }

    r_sun.init();
    r_rain.init();

    r_rain.run();
    r_sun.run();
}


IRender_Sector::sector_id_t CRender::detect_sector(xrXRC& Sectors_xrc, const Fvector& P) const
{
    Fvector dir{0, -1, 0};
    auto sector = detect_sector(Sectors_xrc, P, dir);
    if (sector == IRender_Sector::INVALID_SECTOR_ID)
    {
        dir = {.x = 0, .y = 1, .z = 0};
        sector = detect_sector(Sectors_xrc, P, dir);
    }
    return sector;
}

IRender_Sector::sector_id_t CRender::detect_sector(xrXRC& Sectors_xrc, const Fvector& P, const Fvector& dir) const
{
    // Portals model
    int id1 = -1;
    float range1 = 500.f;
    if (RImplementation.rmPortals)
    {
        Sectors_xrc.ray_query(CDB::OPT_ONLYNEAREST, RImplementation.rmPortals, P, dir, range1);
        if (Sectors_xrc.r_count())
        {
            const CDB::RESULT* RP1 = Sectors_xrc.r_begin();
            id1 = RP1->id;
            range1 = RP1->range;
        }
    }

    // Geometry model
    int id2 = -1;
    float range2 = range1;
    Sectors_xrc.ray_query(CDB::OPT_ONLYNEAREST, g_pGameLevel->ObjectSpace.GetStaticModel(), P, dir, range2);
    if (Sectors_xrc.r_count())
    {
        const CDB::RESULT* RP2 = Sectors_xrc.r_begin();
        id2 = RP2->id;
        range2 = RP2->range;
    }

    // Select ID
    int ID;
    if (id1 >= 0)
    {
        if (id2 >= 0)
            ID = (range1 <= range2 + EPS_L) ? id1 : id2; // both was found
        else
            ID = id1; // only id1 found
    }
    else if (id2 >= 0)
        ID = id2; // only id2 found
    else
        return IRender_Sector::INVALID_SECTOR_ID;

    if (ID == id1)
    {
        __try
        {
            // Take sector, facing to our point from portal
            const CDB::TRI* pTri = RImplementation.rmPortals->get_tris() + ID;
            if (!pTri)
            {
                Msg("!![%s] nullptr pTri detected! tris ID: [%d]", __FUNCTION__, ID);
                return IRender_Sector::INVALID_SECTOR_ID;
            }

            const auto& Portals = sector_portals_structure.Portals;
            if (pTri->dummy >= Portals.size())
            {
                Msg("!![%s] out of range detected! tris ID: [%d], pTri->dummy: [%u], Portals.size(): [%u]", __FUNCTION__, ID, pTri->dummy, Portals.size());
                return IRender_Sector::INVALID_SECTOR_ID;
            }

            const CPortal* pPortal = Portals.at(pTri->dummy);
            if (!pPortal)
            {
                Msg("!![%s] nullptr pPortal detected! tris ID: [%d], pTri->dummy: [%u]", __FUNCTION__, ID, pTri->dummy);
                return IRender_Sector::INVALID_SECTOR_ID;
            }

            return pPortal->getSectorFacing(P)->unique_id;
        }
        __except (ExceptStackTrace("Exception catched in " __FUNCTION__))
        {
            Msg("!![%s] possible bad tris ID: [%d]", __FUNCTION__, ID);

            { // Отключаем сектора до перезапуска двига без сохранения этого отключения в юзере
                auto it = Console->Commands.find("r2_disable_sectors");
                if (it != Console->Commands.end())
                {
                    auto* cmd = it->second;
                    cmd->SetCanSave(false);
                    cmd->Execute("on");
                }
            }

            return IRender_Sector::INVALID_SECTOR_ID;
        }
    }

    // Take triangle at ID and use it's Sector
    const CDB::TRI* pTri = g_pGameLevel->ObjectSpace.GetStaticTris() + ID;
    return pTri->sector;
}

void CRender::update_sector(xrXRC& Sectors_xrc, ISpatial* S) const
{
    if (S->spatial.type & STYPEFLAG_INVALIDSECTOR)
    {
        const auto& entity_pos = S->spatial_sector_point();
        const auto sector_id = detect_sector(Sectors_xrc, entity_pos);
        S->spatial_updatesector(sector_id);
    }
}
#include "stdafx.h"

bool check_grass_shadow(light* L, CFrustum VB)
{
    // Inside the range?
    if (L->vis.distance > ps_ssfx_grass_shadows.z)
        return false;

    // Is in view? L->vis.visible?
    u32 mask = 0xff;
    if (!VB.testSphere(L->position, L->range * 0.6f, mask))
        return false;

    return true;
}

struct light_ctx
{
    u32 context_id{CHW::INVALID_CONTEXT_ID};
    u32 curr{};
    xr_vector<light*> lights;
};

void CRender::render_lights_shadowed_one(light_ctx& task)
{
    auto build = [this, &task] {
        auto& dsgraph = get_context(task.context_id);
        light* L = task.lights[task.curr];

        // use smapvis only for static light sources
        if (!ps_r2_ls_flags_ext.test(R2FLAGEXT_DISABLE_SMAPVIS) && !L->get_moveable())
        {
            L->svis[dsgraph.context_id].begin();
        }

        dsgraph.phase = PHASE_SMAP;
        dsgraph.r_pmask(true, false);

        CFrustum view_frustum;
        view_frustum.CreateFromMatrix(L->X.S.combine, FRUSTUM_P_ALL & (~FRUSTUM_P_NEAR));

        dsgraph.max_render_distance = std::max(30.f, L->get_range() + 20.f);

        dsgraph.build_subspace(L->spatial.sector_id, view_frustum, L->X.S.combine, L->position, TRUE);

        const bool bNormal = dsgraph.mapNormalCount > 0 || dsgraph.mapMatrixCount > 0;
        if (bNormal)
        {
            auto render = [this, &task] {
                auto& dsgraph = get_context(task.context_id);
                light* L = task.lights[task.curr];

                stats.s_merged++;

                Target->phase_smap_spot(dsgraph.cmd_list, L);
                dsgraph.cmd_list.set_xform_world(Fidentity);
                dsgraph.cmd_list.set_xform_view(L->X.S.view);
                dsgraph.cmd_list.set_xform_project(L->X.S.project);
                dsgraph.r_dsgraph_render_graph(0);

                if (ps_r2_ls_flags.test(R2FLAG_LIGHT_DETAILS))
                {
                    if (check_grass_shadow(L, ViewBase))
                    {
                        // Use light position to calc "fade"
                        Details->Render(dsgraph.cmd_list, true, L);
                    }
                }

                if (!ps_r2_ls_flags_ext.test(R2FLAGEXT_DISABLE_SMAPVIS) && !L->get_moveable())
                {
                    L->svis[dsgraph.context_id].end();
                }

                if (++task.curr < task.lights.size())
                    render_lights_shadowed_one(task);
            };

            if (ps_r2_ls_flags.test(R2FLAG_EXP_MT_LIGHTS))
            {
                light_pool.submit_detach(render);
            }
            else
            {
                render();
            }
        }
        else
        {
            stats.s_finalclip++;

            if (!ps_r2_ls_flags_ext.test(R2FLAGEXT_DISABLE_SMAPVIS) && !L->get_moveable())
            {
                L->svis[dsgraph.context_id].end();
            }

            task.lights.erase(task.lights.begin() + task.curr);

            if (task.curr < task.lights.size())
                render_lights_shadowed_one(task);
        }
    };

    if (ps_r2_ls_flags.test(R2FLAG_EXP_MT_LIGHTS))
    {
        light_pool.submit_detach(build);
    }
    else
    {
        build();
    }
}

void CRender::render_lights_shadowed(light_Package& LP)
{
    auto& source = LP.v_shadowed;

    // Refactor order based on ability to pack shadow-maps

    // 1. calculate area + sort in descending order
    {
        for (u32 it = 0; it < source.size(); it++)
        {
            light* L = source[it];
            // L->vis_update();
            if (!L->vis.visible)
            {
                source.erase(source.begin() + it);
                it--;
            }
            else
            {
                L->optimize_smap_size();

                if (!ps_r2_ls_flags_ext.test(R2FLAGEXT_DISABLE_SMAPVIS) && !L->get_moveable())
                {
                    Lights_LastFrame.push_back(L);
                }
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // sort lights by importance???
    // while (has_any_lights_that_cast_shadows) {
    //		if (has_point_shadowed)		->	generate point shadowmap
    //		if (has_spot_shadowed)		->	generate spot shadowmap
    //		switch-to-accumulator
    //		if (has_point_unshadowed)	-> 	accum point unshadowed
    //		if (has_spot_unshadowed)	-> 	accum spot unshadowed
    //		if (was_point_shadowed)		->	accum point shadowed
    //		if (was_spot_shadowed)		->	accum spot shadowed
    //	}
    //	if (left_some_lights_that_doesn't cast shadows)
    //		accumulate them

    // if (has_spot_shadowed)
    while (!source.empty())
    {
        stats.s_used++;

        static xr_vector<light_ctx> light_tasks;

        if (ps_r2_ls_flags.test(R2FLAG_EXP_MT_LIGHTS))
        {
            light_tasks.reserve(R__NUM_PARALLEL_CONTEXTS);

            for (size_t i{}; i < std::min<size_t>(R__NUM_PARALLEL_CONTEXTS, source.size()); i++)
            {
                light_tasks.emplace_back().context_id = alloc_context();
            }
        }
        else
        {
            light_tasks.emplace_back().context_id = get_imm_context().cmd_list.context_id;
        }

        const size_t num_tasks = light_tasks.size();

        // 2. refactor - infact we could go from the backside and sort in ascending order
        const auto light_cmp = [](const light* l1, const light* l2) {
            const u32 a0 = l1->X.S.size;
            const u32 a1 = l2->X.S.size;
            return a0 > a1; // reverse -> descending
        };

        std::ranges::sort(source, light_cmp);

        size_t curr_task = 0;
        bool init = true;

        for (size_t test = 0; test < source.size(); test++)
        {
            light* L = source[test];
            size_t fails = 0;

            while (true)
            {
                light_ctx& task = light_tasks[curr_task];

                auto& cmd_list = get_context(task.context_id).cmd_list;
                if (init)
                    cmd_list.LP_smap_pool.initialize(o.smapsize);

                if (++curr_task == num_tasks)
                {
                    curr_task = 0;
                    init = false;
                }

                SMAP_Rect R{};
                if (cmd_list.LP_smap_pool.push(R, L->X.S.size))
                {
                    // OK
                    L->X.S.posX = R.min.x;
                    L->X.S.posY = R.min.y;
                    L->vis.smap_ID = static_cast<u16>(cmd_list.context_id); // ???
                    task.lights.push_back(L);

                    source.erase(source.begin() + test);
                    test--;
                    break;
                }

                if (++fails == num_tasks)
                {
                    goto calc;
                }
            }
        }

    calc:
        for (auto& task : light_tasks)
        {
            Target->phase_smap_spot_clear(get_context(task.context_id).cmd_list);
            render_lights_shadowed_one(task);
        }

        light_pool.wait_for_tasks();

        static xr_vector<light*> L_spot_s;

        auto& cmd_list = get_imm_context().cmd_list;

        {
            PIX_EVENT_CTX(cmd_list, SHADOWED_LIGHTS);

            for (auto& task : light_tasks)
            {
                for (auto* light : task.lights)
                    L_spot_s.push_back(light);

                if (ps_r2_ls_flags.test(R2FLAG_EXP_MT_LIGHTS))
                {
                    get_context(task.context_id).cmd_list.submit();
                    release_context(task.context_id);
                }
            }
        }

        cmd_list.Invalidate();

        light_tasks.clear();
        std::ranges::sort(L_spot_s, light_cmp);

        // Spot lighting (shadowed)
        if (!L_spot_s.empty())
        {
            PIX_EVENT_CTX(cmd_list, SPOT_LIGHTS_ACCUM_VOLUMETRIC);

            ZoneScopedN("render_lights spot shadowed");

            const bool needVolumetric = ps_r2_ls_flags.is(R2FLAG_VOLUMETRIC_LIGHTS);

            for (light* p_light : L_spot_s)
            {
                Target->rt_smap_depth->set_slice_read(p_light->vis.smap_ID);
                Target->accum_spot(cmd_list, p_light);
                if (needVolumetric)
                {
                    Target->accum_volumetric(cmd_list, p_light);
                }
            }

            L_spot_s.clear();
        }
    }
}

void CRender::render_lights(light_Package& LP)
{
    ZoneScoped;

    if (!LP.v_shadowed.empty())
        render_lights_shadowed(LP);

    {
        auto& cmd_list = get_imm_context().cmd_list;

        // Point lighting (unshadowed)
        if (!LP.v_point.empty())
        {
            ZoneScopedN("render_lights point unshadowed");

            PIX_EVENT_CTX(cmd_list, POINT_LIGHTS_ACCUM);

            for (auto* p_light : LP.v_point)
            {
                if (p_light->vis.visible)
                {
                    Target->accum_point(cmd_list, p_light);
                }
            }
            LP.v_point.clear();
        }

        // Spot lighting (unshadowed)
        if (!LP.v_spot.empty())
        {
            ZoneScopedN("render_lights spot unshadowed");

            PIX_EVENT_CTX(cmd_list, SPOT_LIGHTS_ACCUM);

            for (auto* p_light : LP.v_spot)
            {
                //p_light->vis_update();
                if (p_light->vis.visible)
                {
                    p_light->optimize_smap_size();
                    Target->accum_spot(cmd_list, p_light);
                }
            }
            LP.v_spot.clear();
        }
    }
}
#include "stdafx.h"

#include "../../xr_3da/render.h"
#include "../../xr_3da/irenderable.h"
#include "../../xr_3da/igame_persistent.h"
#include "../../xr_3da/environment.h"
#include "../../xr_3da/CustomHUD.h"

#include "FBasicVisual.h"
#include "HUDInitializer.h"

extern ENGINE_API float psHUD_FOV;

using namespace R_dsgraph;

extern float ps_r__LOD_k;

template <class T>
bool cmp_ssa(const T& lhs, const T& rhs)
{
    return lhs.ssa > rhs.ssa;
}

// Sorting by SSA and changes minimizations
template <typename T>
bool cmp_pass(const T& left, const T& right)
{
    if (left->first->equal(*right->first))
        return false;
    return left->second.ssa >= right->second.ssa;
}

void R_dsgraph_structure::r_dsgraph_render_graph_static(u32 _priority)
{
    Device.Statistic->RenderDUMP.Begin();

    cmd_list.set_xform_world(Fidentity);

    // **************************************************** NORMAL
    // Perform sorting based on ScreenSpaceArea
    // Sorting by SSA and changes minimizations
    {
        ZoneScopedN("dsgraph_render_static");

        PIX_EVENT_CTX(cmd_list, dsgraph_render_static);

        // Render several passes
        for (u32 iPass = 0; iPass < SHADER_PASSES_MAX; ++iPass)
        {
            auto& map = mapNormalPasses[_priority][iPass];

            {
                ZoneScopedN("dsgraph_render_static get_and_sort");
                map.get_any_p(nrmPasses);
                std::sort(nrmPasses.begin(), nrmPasses.end(), cmp_pass<mapNormal_T::value_type*>);
            }

            for (const auto& it : nrmPasses)
            {
                cmd_list.set_Pass(it->first);
                cmd_list.apply_lmaterial();

                mapNormalItems& items = it->second;
                items.ssa = 0;

                {
                    ZoneScopedN("dsgraph_render_static second_sort");
                    std::sort(items.items->begin(), items.items->end(), cmp_ssa<_NormalItem>);
                }

                for (const auto& item : *items.items)
                {
                    const float lod = calcLOD(item.ssa, item.pVisual->getVisData().sphere.R);
                    cmd_list.lod.set_lod(lod);

                    // --#SM+#-- Обновляем шейдерные данные модели [update shader values for this model]
                    // RCache.hemi.c_update(item.pVisual);

                    {
                        ZoneScopedN("render_static_visual");

                        item.pVisual->Render(cmd_list,  clampr(1.f - (1.f - lod) * ps_r__LOD_k, 0.01f, 1.f), phase == CRender::PHASE_SMAP);
                    }
                }

                if (!items.trees->empty())
                {
                    cmd_list.set_c("benders_setup",
                                   Fvector4{ps_ssfx_int_grass_params_1.x, ps_ssfx_int_grass_params_1.y, ps_ssfx_int_grass_params_1.z,
                                            ps_r2_ls_flags_ext.test(R2FLAGEXT_SSFX_INTER_GRASS) ? ps_ssfx_grass_interactive.y : 0.f});

                    if (ps_r2_ls_flags_ext.test(R2FLAGEXT_SSFX_INTER_GRASS))
                    {
                        static const shared_str benders_pos{"benders_pos"}, benders_pos_old{"benders_pos_old"};

                        Fvector4* c_grass{};
                        cmd_list.get_ConstantDirect(benders_pos, sizeof grass_shader_data.pos + sizeof grass_shader_data.dir, reinterpret_cast<void**>(&c_grass), nullptr, nullptr);
                        if (c_grass)
                        {
                            std::memcpy(c_grass, &grass_shader_data.pos, sizeof grass_shader_data.pos);
                            std::memcpy(c_grass + std::size(grass_shader_data.pos), &grass_shader_data.dir, sizeof grass_shader_data.dir);
                        }

                        Fvector4* b_pos_old{};
                        cmd_list.get_ConstantDirect(benders_pos_old, sizeof grass_shader_data_old.pos + sizeof grass_shader_data_old.dir, reinterpret_cast<void**>(&b_pos_old), nullptr, nullptr);
                        if (b_pos_old)
                        {
                            std::memcpy(b_pos_old, &grass_shader_data_old.pos, sizeof grass_shader_data_old.pos);
                            std::memcpy(b_pos_old + std::size(grass_shader_data_old.pos), &grass_shader_data_old.dir, sizeof grass_shader_data_old.dir);
                        }
                    }

                    const float lod = 1.f; // fixed lod for shaders
                    cmd_list.lod.set_lod(lod);

                    for (const auto& tree : *items.trees)
                    {
                        {
                            ZoneScopedN("render_static_visual_trees");

                            tree.second.pVisual->RenderInstanced(cmd_list, tree.second.data);
                        }
                    }
                }

                items.items->clear();
                items.trees->clear();
            }
            nrmPasses.clear();
            map.clear();
        }
    }
}

void R_dsgraph_structure::r_dsgraph_render_graph_dynamic(u32 _priority)
{
    // **************************************************** MATRIX
    // Perform sorting based on ScreenSpaceArea
    // Sorting by SSA and changes minimizations
    {
        ZoneScopedN("dsgraph_render_dynamic");

        PIX_EVENT_CTX(cmd_list, dsgraph_render_dynamic);

        // Render several passes
        for (u32 iPass = 0; iPass < SHADER_PASSES_MAX; ++iPass)
        {
            auto& map = mapMatrixPasses[_priority][iPass];

            map.get_any_p(matPasses);
            std::sort(matPasses.begin(), matPasses.end(), cmp_pass<mapMatrix_T::value_type*>);
            for (const auto& it : matPasses)
            {
                cmd_list.set_Pass(it->first);

                mapMatrixItems& items = it->second;
                items.ssa = 0;

                std::sort(items.items->begin(), items.items->end(), cmp_ssa<_MatrixItem>);
                for (auto& item : *items.items)
                {
                    cmd_list.set_xform_world(item.Matrix);
                    RImplementation.apply_object(cmd_list, item.pObject);
                    cmd_list.apply_lmaterial();

                    const float lod = calcLOD(item.ssa, item.pVisual->getVisData().sphere.R);
                    cmd_list.lod.set_lod(lod);

                    // --#SM+#-- Обновляем шейдерные данные модели [update shader values for this model]
                    // RCache.hemi.c_update(item.pVisual);

                    item.pVisual->Render(cmd_list, lod, phase == CRender::PHASE_SMAP);
                }
                items.items->clear();
            }
            matPasses.clear();
            map.clear();
        }
    }

    Device.Statistic->RenderDUMP.End();
}

void R_dsgraph_structure::r_dsgraph_render_graph(u32 _priority)
{    
    PIX_EVENT_CTX(cmd_list, r_dsgraph_render_graph);

    r_dsgraph_render_graph_static(_priority);
    r_dsgraph_render_graph_dynamic(_priority);
}

template <class T>
void render_item(u32 context_id, const T& item)
{
    auto& dsgraph = RImplementation.get_context(context_id);

    dxRender_Visual* V = item.second.pVisual;
    VERIFY(V && V->shader._get());
    dsgraph.cmd_list.set_Element(item.second.se);
    dsgraph.cmd_list.set_xform_world(item.second.Matrix);
    RImplementation.apply_object(dsgraph.cmd_list, item.second.pObject);
    dsgraph.cmd_list.apply_lmaterial();

    // Change culling mode if HUD meshes were flipped
    // if (cullMode != CULL_NONE)
    //{
    //     cmd_list.set_CullMode(cullMode == CULL_CW ? CULL_CCW : CULL_CW);
    // }

    //--#SM+#-- Обновляем шейдерные данные модели [update shader values for this model]
    // RCache.hemi.c_update(V);

    const float lod = calcLOD(item.first, V->getVisData().sphere.R);
    dsgraph.cmd_list.lod.set_lod(lod); // !!!
    V->Render(dsgraph.cmd_list, lod, dsgraph.phase == CRender::PHASE_SMAP);
}

template <class T>
ICF void sort_front_to_back_render_and_clean(u32 context_id, T& vec)
{
    vec.traverse_left_right(context_id, render_item);
    vec.clear();
}

template <class T>
ICF void sort_back_to_front_render_and_clean(u32 context_id, T& vec)
{
    vec.traverse_right_left(context_id, render_item);
    vec.clear();
}

//////////////////////////////////////////////////////////////////////////
// HUD render
void R_dsgraph_structure::r_dsgraph_render_hud()
{
    ZoneScoped;

    PIX_EVENT_CTX(cmd_list, dsgraph_render_hud);

    // Rendering
    if (!mapHUD.empty())
    {
        CHUDTransformHelper initializer(cmd_list,true, true);
        
        RImplementation.rmNear(cmd_list);

        sort_front_to_back_render_and_clean(context_id, mapHUD);
        
        RImplementation.rmNormal(cmd_list);
    }
}

void R_dsgraph_structure::r_dsgraph_render_hud_scope_depth()
{
    ZoneScoped;

    PIX_EVENT_CTX(cmd_list, r_dsgraph_render_hud_scope_depth);

    if (!mapScopeHUD.empty())
    {
        CHUDTransformHelper initializer(cmd_list, true, true);

        RImplementation.rmNormal(cmd_list);

        sort_front_to_back_render_and_clean(context_id, mapScopeHUD);
    }
}

void R_dsgraph_structure::r_dsgraph_render_hud_ui()
{
    ZoneScoped;

    PIX_EVENT_CTX(cmd_list, dsgraph_render_hud_ui);

    CHUDTransformHelper initializer(cmd_list,true, true);

    RImplementation.rmNear(cmd_list);
    g_hud->RenderActiveItemUI();
    RImplementation.rmNormal(cmd_list);
}

//////////////////////////////////////////////////////////////////////////
// strict-sorted render
void R_dsgraph_structure::r_dsgraph_render_sorted()
{
    ZoneScoped;

    PIX_EVENT_CTX(cmd_list, dsgraph_render_sorted);

    sort_back_to_front_render_and_clean(context_id, mapSorted);

    if (!mapHUDSorted.empty())
    {
        CHUDTransformHelper initializer(cmd_list,true, true);

        RImplementation.rmNear(cmd_list);

        sort_back_to_front_render_and_clean(context_id, mapHUDSorted);

        RImplementation.rmNormal(cmd_list);
    }
}

//////////////////////////////////////////////////////////////////////////
// strict-sorted render
void R_dsgraph_structure::r_dsgraph_render_scope_sorted(const bool upscaled)
{
    PIX_EVENT_CTX(cmd_list, dsgraph_render_scope_sorted);

    CHUDTransformHelper initializer(cmd_list, true, true);

    RImplementation.rmNear(cmd_list);

    sort_back_to_front_render_and_clean(context_id, upscaled ? mapScopeHUDSorted2 : mapScopeHUDSorted);

    RImplementation.rmNormal(cmd_list);
}

//////////////////////////////////////////////////////////////////////////
// strict-sorted render
void R_dsgraph_structure::r_dsgraph_render_emissive(bool clear)
{
    ZoneScoped;

    PIX_EVENT_CTX(cmd_list, dsgraph_render_emissive);

    //sort_front_to_back_render_and_clean(context_id, mapEmissive);
    mapEmissive.traverse_left_right(context_id, render_item);
    if (clear)
        mapEmissive.clear();

    if (!mapHUDEmissive.empty())
    {
        CHUDTransformHelper initializer(cmd_list, true, true);

        RImplementation.rmNear(cmd_list);

        //sort_front_to_back_render_and_clean(context_id, mapHUDEmissive);
        mapHUDEmissive.traverse_left_right(context_id, render_item);
        if (clear)
            mapHUDEmissive.clear();

        RImplementation.rmNormal(cmd_list);
    }
}

//////////////////////////////////////////////////////////////////////////
// strict-sorted render
void R_dsgraph_structure::r_dsgraph_render_wmarks()
{
    ZoneScoped;

    PIX_EVENT_CTX(cmd_list, dsgraph_render_wmarks);

    // Sorted (back to front)
    sort_front_to_back_render_and_clean(context_id, mapWmark);
}

//////////////////////////////////////////////////////////////////////////
// strict-sorted render
void R_dsgraph_structure::r_dsgraph_render_distort()
{
    ZoneScoped;

    PIX_EVENT_CTX(cmd_list, dsgraph_render_distort);

    // Sorted (back to front)
    sort_back_to_front_render_and_clean(context_id, mapDistort);
}

// sub-space rendering - main procedure
void R_dsgraph_structure::build_subspace(IRender_Sector::sector_id_t& sector_id, CFrustum* frustum, Fmatrix& xform, Fvector& camera_position, BOOL add_dynamic)
{
    ZoneScoped;

    IRender_Sector* _sector = Sectors[sector_id];

    render_position = camera_position;

    VERIFY(_sector);
    marker++; // !!! critical here

    if (false) /*RImplementation.SectorsLoadDisabled*/ // check disabled for subspace (sun, light, rain)
    {
        //add_static(Sectors[0]->root(), *frustum, frustum->getMask());

        for (const auto& sector : Sectors)
        {
            dxRender_Visual* root = sector->root();

            // for (u32 v_it = 0; v_it < sector->r_frustums.size(); v_it++)
            {
                add_static(root, *frustum, frustum->getMask());
            }
        }
    }
    else
    {
        // Traverse sector/portal structure
        PortalTraverser.traverse(_sector, *frustum, camera_position, xform, 0);

        // Determine visibility for static geometry hierrarhy
        for (const auto& r_sector : PortalTraverser.r_sectors)
        {
            dxRender_Visual* root = r_sector->root();
            for (const auto& view : r_sector->r_frustums)
            {
                add_static(root, view, view.getMask());
            }
        }
    }

    if (add_dynamic)
    {
        // Traverse object database
        if (max_render_distance > 0)
        {
            g_SpatialSpace->q_sphere(lstRenderables, 0, STYPE_RENDERABLE, render_position, max_render_distance);   
        }
        else
        {
            g_SpatialSpace->q_frustum(lstRenderables, 0, STYPE_RENDERABLE, *frustum);
        }

        // Determine visibility for dynamic part of scene
        for (const auto spatial : lstRenderables)
        {
            /*if (o.is_main_pass)
            {
                const auto& entity_pos = spatial->spatial_sector_point();
                const auto sector_id = detect_sector(entity_pos);
                spatial->spatial_updatesector(sector_id);
            }*/

            const auto& sector_id = spatial->spatial.sector_id;
            if (sector_id == IRender_Sector::INVALID_SECTOR_ID)
            {
                continue; // disassociated from S/P structure
            }

            const auto* sector = Sectors[sector_id];

            if (PortalTraverser.frame() == Device.dwFrame)
            {
                if (PortalTraverser.marker() != sector->r_marker)
                    continue; // inactive (untouched) sector
            }

            if ((spatial->spatial.type & STYPE_RENDERABLE) && !ps_r2_ls_flags_ext.test(R2FLAGEXT_DISABLE_DYNAMIC))
            {
                // renderable
                IRenderable* renderable = spatial->dcast_Renderable();
                R_ASSERT(renderable);

                // casting is faster then using getVis method
                vis_data& v_orig = renderable->renderable.visual->getVisData();

                if (max_render_distance > 0.f)
                {
                    Fvector pos;
                    renderable->renderable.xform.transform_tiny(pos, v_orig.sphere.P);

                    if (!renderable->renderable.visual->ignore_optimization && render_position.distance_to(pos) - v_orig.sphere.R / 2 > max_render_distance)
                        continue;
                }

                for (auto& view : sector->r_frustums)
                {
                    if (!view.testSphere_dirty(spatial->spatial.sphere.P, spatial->spatial.sphere.R))
                         continue;

                    renderable->renderable_Render(context_id, renderable);

                    break; // exit loop on frustums
                }
            }
        }

        if (g_pGameLevel && g_hud)
        {
            if (phase == CRender::PHASE_SMAP)
            {
                g_hud->Render_SMAP(context_id);
            }
        }
    }
}

#include "stdafx.h"

#include "../../xr_3da/render.h"
#include "../../xr_3da/CustomHUD.h"

#include "FBasicVisual.h"
#include "HUDInitializer.h"

extern float psHUD_FOV;
extern float ps_r__LOD_k;

using namespace R_dsgraph;

void R_dsgraph_structure::r_dsgraph_render_graph_static(const u32 _priority)
{
    Device.Statistic->RenderDUMP.Begin();

    cmd_list.set_xform_world(Fidentity);

    // **************************************************** NORMAL
    {
        ZoneScopedN("dsgraph_render_static");

        PIX_EVENT_CTX(cmd_list, dsgraph_render_static);

        // Render several passes
        for (u32 iPass = 0; iPass < SHADER_PASSES_MAX; ++iPass)
        {
            auto& map = mapNormalPasses[_priority][iPass];

            ID3DState* state{};
            STextureList* textures{};

            for (const auto& it : map)
            {
                if (it.second.items->empty() && it.second.trees->empty())
                    continue;

                // cmd_list.set_Pass(it->first);

                cmd_list.set_VS(it.first->vs);
                cmd_list.set_GS(it.first->gs);
                cmd_list.set_PS(it.first->ps);
                cmd_list.set_HS(it.first->hs);
                cmd_list.set_DS(it.first->ds);

                cmd_list.set_Constants(it.first->constants);
                if (it.first->state._get()->state != state)
                {
                    cmd_list.set_States(state = it.first->state._get()->state);
                }
                if (it.first->T._get() != textures)
                {
                    cmd_list.set_Textures(textures = it.first->T._get());
                    cmd_list.apply_lmaterial();
                }

                const mapNormalItems& items = it.second;
                for (const auto& item : *items.items)
                {
                    const float lod = calcLOD(item.ssa, item.pVisual->getVisData().sphere.R);
                    cmd_list.lod.set_lod(lod);

                    {
                        ZoneScopedN("render_static_visual");

                        item.pVisual->Render(cmd_list, clampr(1.f - (1.f - lod) * ps_r__LOD_k, 0.01f, 1.f), phase == CRender::PHASE_SMAP);
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
                        cmd_list.get_ConstantDirect(benders_pos_old, sizeof grass_shader_data_old.pos + sizeof grass_shader_data_old.dir, reinterpret_cast<void**>(&b_pos_old),
                                                    nullptr, nullptr);
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
            map.clear(); // that should be removed, but having strange issues with trees map
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

            ID3DState* state{};
            STextureList* textures{};

            for (const auto& it : map)
            {
                if (it.second.items->empty())
                    continue;

                // cmd_list.set_Pass(it->first);

                cmd_list.set_VS(it.first->vs);
                cmd_list.set_GS(it.first->gs);
                cmd_list.set_PS(it.first->ps);
                cmd_list.set_HS(it.first->hs);
                cmd_list.set_DS(it.first->ds);

                cmd_list.set_Constants(it.first->constants);
                if (it.first->state._get()->state != state)
                {
                    cmd_list.set_States(state = it.first->state._get()->state);
                }
                if (it.first->T._get() != textures)
                {
                    cmd_list.set_Textures(textures = it.first->T._get());
                    cmd_list.apply_lmaterial();
                }

                const mapMatrixItems& items = it.second;
                for (auto& item : *items.items)
                {
                    cmd_list.set_xform_world(item.Matrix);
                    RImplementation.apply_object(cmd_list, item.pObject);
                    cmd_list.apply_lmaterial();

                    const float lod = calcLOD(item.ssa, item.pVisual->getVisData().sphere.R);
                    cmd_list.lod.set_lod(lod);
                    item.pVisual->Render(cmd_list, lod, phase == CRender::PHASE_SMAP);
                }
                items.items->clear();
            }
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

namespace
{
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

void render_large_map(const u32 context_id, mapSortedLarge_T& map)
{
    auto& dsgraph = RImplementation.get_context(context_id);

    ID3DState* state{};
    STextureList* textures{};

    for (const auto& it : map)
    {
        if (it.second.items->empty())
            continue;

        // cmd_list.set_Pass(it->first);

        dsgraph.cmd_list.set_VS(it.first->vs);
        dsgraph.cmd_list.set_GS(it.first->gs);
        dsgraph.cmd_list.set_PS(it.first->ps);
        dsgraph.cmd_list.set_HS(it.first->hs);
        dsgraph.cmd_list.set_DS(it.first->ds);

        dsgraph.cmd_list.set_Constants(it.first->constants);
        if (it.first->state._get()->state != state)
        {
            dsgraph.cmd_list.set_States(state = it.first->state._get()->state);
        }
        if (it.first->T._get() != textures)
        {
            dsgraph.cmd_list.set_Textures(textures = it.first->T._get());
            dsgraph.cmd_list.apply_lmaterial();
        }

        const mapMatrixItems& items = it.second;
        for (auto& item : *items.items)
        {
            dsgraph.cmd_list.set_xform_world(item.Matrix);
            RImplementation.apply_object(dsgraph.cmd_list, item.pObject);
            dsgraph.cmd_list.apply_lmaterial();

            const float lod = calcLOD(item.ssa, item.pVisual->getVisData().sphere.R);
            dsgraph.cmd_list.lod.set_lod(lod);
            item.pVisual->Render(dsgraph.cmd_list, lod, dsgraph.phase == CRender::PHASE_SMAP);
        }
        items.items->clear();
    }
    map.clear();
}
} // namespace

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

    render_large_map(context_id, mapSorted);

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
    render_large_map(context_id, mapDistort);
}

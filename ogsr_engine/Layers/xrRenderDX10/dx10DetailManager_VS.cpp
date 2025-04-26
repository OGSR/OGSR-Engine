#include "stdafx.h"
#include "../xrRender/DetailManager.h"

void CDetailManager::SetupCBuffer(CBackend& cmd_list, const ref_pass& pass, bool shadows)
{
    static const shared_str benders_pos{"benders_pos"}, benders_pos_old{"benders_pos_old"};

    // Setup matrices + colors (and flush it as necessary)
    cmd_list.set_Pass(pass);
    cmd_list.apply_lmaterial();

    cmd_list.set_c("m_taa_jitter_disable", shadows ? 1.f : 0.f);
    cmd_list.set_c("grass_align", ps_ssfx_terrain_grass_align);

    cmd_list.set_c("benders_setup",
                   Fvector4{ps_ssfx_int_grass_params_1.x, ps_ssfx_int_grass_params_1.y, ps_ssfx_int_grass_params_1.z,
                            ps_r2_ls_flags_ext.test(R2FLAGEXT_SSFX_INTER_GRASS) ? ps_ssfx_grass_interactive.y : 0.f});

    if (ps_r2_ls_flags_ext.test(R2FLAGEXT_SSFX_INTER_GRASS))
    {
        Fvector4* b_pos{};
        cmd_list.get_ConstantDirect(benders_pos, sizeof grass_shader_data.pos + sizeof grass_shader_data.dir, reinterpret_cast<void**>(&b_pos), nullptr, nullptr, true);
        R_ASSERT(b_pos, "Something strange in grass shader!");

        std::memcpy(b_pos, &grass_shader_data.pos, sizeof grass_shader_data.pos);
        std::memcpy(b_pos + std::size(grass_shader_data.pos), &grass_shader_data.dir, sizeof grass_shader_data.dir);

        Fvector4* b_pos_old{};
        cmd_list.get_ConstantDirect(benders_pos_old, sizeof grass_shader_data_old.pos + sizeof grass_shader_data_old.dir, reinterpret_cast<void**>(&b_pos_old), nullptr, nullptr, true);
        R_ASSERT(b_pos_old, "Something strange in grass shader!");

        std::memcpy(b_pos_old, &grass_shader_data_old.pos, sizeof grass_shader_data_old.pos);
        std::memcpy(b_pos_old + std::size(grass_shader_data_old.pos), &grass_shader_data_old.dir, sizeof grass_shader_data_old.dir);
    }
}

void CDetailManager::hw_Render(CBackend& cmd_list, const bool shadows, light* L)
{
    ZoneScoped;

    if (!shadows)
        Device.Statistic->RenderDUMP_DT_Count = 0;

    // тут обычно не больше 16 или 32 визуалов. можно сгруппировать по шейдеру их...
    // так же как правил везде один pass

    const SPass* last_pass{};

    u32 dwBatchTotal{};

    // Iterate
    for (u32 O{}; const auto& object : objects)
    {
        auto& vis = m_visibles[O++]; // by object index

        if (vis.empty())
            continue;

        if (ref_pass& pass = object.shader->E[0]->passes[0]; !last_pass || !pass->equal(*last_pass))
        {
            last_pass = pass._get();

            SetupCBuffer(cmd_list, pass, shadows);
        }

        dwBatchTotal += render_items(cmd_list, object, vis, shadows, L);
    }

    ZoneValue(dwBatchTotal);

    if (!shadows)
        Device.Statistic->RenderDUMP_DT_Count += dwBatchTotal;
}

u32 CDetailManager::render_items(CBackend& cmd_list, const CDetail& object, const vis_list_inner& vis_list, const bool shadows, light* L)
{
    u32 dwBatchTotal{}, sizeof_vbuffer{}, all_items_size{vis_list.instance_count}, details_count{};
    ID3DVertexBuffer* current_vbuffer{};
    D3D11_MAPPED_SUBRESOURCE pSubRes{};
    FloraVertData* c_storage{};

    auto update_vertex_buffer = [&] {
        sizeof_vbuffer = all_items_size;
        current_vbuffer = cmd_list.GetFloraVbuff(sizeof_vbuffer);

        constexpr u32 vb_stride[]{sizeof(DetailVertData), sizeof(FloraVertData)};
        constexpr u32 i_offset[]{0, 0};
        ID3DVertexBuffer* final_vbuffer[]{object.DetailVb, current_vbuffer};

        cmd_list.set_Format(&*object.DetailGeom->dcl);
        cmd_list.set_Vertices_Forced(std::size(final_vbuffer), final_vbuffer, vb_stride, i_offset);
        cmd_list.set_Indices(object.DetailIb);
    };

    auto render_call = [&] {
        ZoneScopedN("render_call");

        if (c_storage)
        {
            HW.get_context(cmd_list.context_id)->Unmap(current_vbuffer, 0);
            c_storage = nullptr;
        }
        if (details_count > 0)
        {
            dwBatchTotal += details_count;

            //Msg("--Called rendering [%u] grass instances", details_count);
            cmd_list.Render(D3DPT_TRIANGLELIST, 0, 0, object.number_vertices, 0, object.number_indices / 3, details_count);
            cmd_list.stat.r.s_details.add(details_count);

            // restart
            details_count = 0;
        }
    };

    {
        ZoneScopedN("render");

        update_vertex_buffer();

        const float L_range_sqr = L ? _sqr(L->range) * 1.05f : 0.f;

        for (const auto* items : vis_list)
        {
            for (const auto* instance : *items)
            {
                if (instance->alpha <= 0.f || instance->scale_calculated <= 0.f)
                    break;

                if (shadows) // on SMAP phase only
                {
                    if (L && L->position.distance_to_sqr(instance->xform.c) > L_range_sqr)
                        continue;
                }

                if (!c_storage)
                {
                    //Msg("~~Called mapping buffer with size [%u]", sizeof_vbuffer);
                    R_CHK(HW.get_context(cmd_list.context_id)->Map(current_vbuffer, 0, D3D_MAP_WRITE_DISCARD, 0, &pSubRes));
                    c_storage = reinterpret_cast<FloraVertData*>(pSubRes.pData);
                }

                std::memcpy(&c_storage[details_count++], &instance->data, sizeof(instance->data));

                if (details_count == sizeof_vbuffer)
                {
                    all_items_size -= details_count;

                    render_call();

                    if (all_items_size > 0)
                    {
                        u32 temp_buff_size = all_items_size;
                        cmd_list.GetFloraVbuff(temp_buff_size);
                        if (temp_buff_size != sizeof_vbuffer)
                        {
                            //Msg("--Updated buff from [%u] to [%u]", sizeof_vbuffer, temp_buff_size);
                            update_vertex_buffer();
                        }
                    }
                }
            }
        }

        render_call();
    }

    return dwBatchTotal;
}
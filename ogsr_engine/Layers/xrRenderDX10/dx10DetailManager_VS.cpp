#include "stdafx.h"
#include "../xrRender/DetailManager.h"

#include "../../xr_3da/igame_persistent.h"
#include "../../xr_3da/environment.h"

#include "../xrRenderDX10/dx10BufferUtils.h"

constexpr int quant = 16384;

void CDetailManager::hw_Render()
{
    // Render-prepare
    //	Update timer
    //	Can't use Device.fTimeDelta since it is smoothed! Don't know why, but smoothed value looks more choppy!
    float fDelta = Device.fTimeGlobal - m_global_time_old;
    if ((fDelta < 0) || (fDelta > 1))
        fDelta = 0.03;
    m_global_time_old = Device.fTimeGlobal;

    m_time_rot_1 += (PI_MUL_2 * fDelta / swing_current.rot1);
    m_time_rot_2 += (PI_MUL_2 * fDelta / swing_current.rot2);
    m_time_pos += fDelta * swing_current.speed;

    float tm_rot1 = m_time_rot_1;
    float tm_rot2 = m_time_rot_2;

    Fvector4 dir1, dir2;
    dir1.set(_sin(tm_rot1), 0, _cos(tm_rot1), 0).normalize().mul(swing_current.amp1);
    dir2.set(_sin(tm_rot2), 0, _cos(tm_rot2), 0).normalize().mul(swing_current.amp2);

    // Setup geometry and DMA
    RCache.set_Geometry(hw_Geom);

    // Wave0
    constexpr float scale = 1.f / float(quant);
    Fvector4 wave;
    Fvector4 consts;
    consts.set(scale, scale, ps_r__Detail_l_aniso, ps_r__Detail_l_ambient);

    wave.set(1.f / 5.f, 1.f / 7.f, 1.f / 3.f, m_time_pos);

    hw_Render_dump(consts, wave.div(PI_MUL_2), dir1, 1, 0);

    // Wave1
    wave.set(1.f / 3.f, 1.f / 7.f, 1.f / 5.f, m_time_pos);

    hw_Render_dump(consts, wave.div(PI_MUL_2), dir2, 2, 0);

    // Still
    consts.set(scale, scale, scale, 1.f);

    hw_Render_dump(consts, wave.div(PI_MUL_2), dir2, 0, 1);
}

void CDetailManager::hw_Render_dump(const Fvector4& consts, const Fvector4& wave, const Fvector4& wind, u32 var_id, u32 lod_id)
{
    constexpr const char* strConsts = "consts";
    constexpr const char* strWave = "wave";
    constexpr const char* strDir2D = "dir2D";
    constexpr const char* strArray = "array";
    constexpr const char* strXForm = "xform";
    constexpr const char* strBendersSetup = "benders_setup";
    constexpr const char* strBendersPos = "benders_pos";

    Device.Statistic->RenderDUMP_DT_Count = 0;

    // Matrices and offsets
    u32 vOffset = 0;
    u32 iOffset = 0;

    vis_list& list = m_visibles[var_id];

    CEnvDescriptor& desc = *g_pGamePersistent->Environment().CurrentEnv;
    Fvector c_sun, c_ambient, c_hemi;
    c_sun.set(desc.sun_color.x, desc.sun_color.y, desc.sun_color.z);
    c_sun.mul(.5f);
    c_ambient.set(desc.ambient.x, desc.ambient.y, desc.ambient.z);
    c_hemi.set(desc.hemi_color.x, desc.hemi_color.y, desc.hemi_color.z);

    // Iterate
    for (u32 O = 0; O < objects.size(); O++)
    {
        CDetail& Object = *objects[O];
        auto& vis = list[O];
        if (!vis.empty())
        {
            for (u32 iPass = 0; iPass < Object.shader->E[lod_id]->passes.size(); ++iPass)
            {
                // Setup matrices + colors (and flush it as necessary)
                RCache.set_Element(Object.shader->E[lod_id], iPass);
                RImplementation.apply_lmaterial();

                //	This could be cached in the corresponding consatant buffer
                //	as it is done for DX9
                RCache.set_c(strConsts, consts);
                RCache.set_c(strWave, wave);
                RCache.set_c(strDir2D, wind);
                RCache.set_c(strXForm, Device.mFullTransform);

                RCache.set_c(strBendersSetup,
                             Fvector4{ps_ssfx_int_grass_params_1.x, ps_ssfx_int_grass_params_1.y, ps_ssfx_int_grass_params_1.z,
                                      ps_r2_ls_flags_ext.test(SSFX_INTER_GRASS) ? ps_ssfx_grass_interactive.y : 0.f});

                if (ps_r2_ls_flags_ext.test(SSFX_INTER_GRASS))
                {
                    Fvector4* c_grass{};
                    RCache.get_ConstantDirect(strBendersPos, sizeof grass_shader_data.pos + sizeof grass_shader_data.dir, reinterpret_cast<void**>(&c_grass), nullptr, nullptr);
                    if (c_grass)
                    {
                        std::memcpy(c_grass, &grass_shader_data.pos, sizeof grass_shader_data.pos);
                        std::memcpy(c_grass + std::size(grass_shader_data.pos), &grass_shader_data.dir, sizeof grass_shader_data.dir);
                    }
                }

                Fvector4* c_storage{};
                RCache.get_ConstantDirect(strArray, hw_BatchSize * sizeof(Fvector4) * 4, reinterpret_cast<void**>(&c_storage), nullptr, nullptr);
                R_ASSERT(c_storage);

                u32 dwBatch = 0;

                auto _vI = vis.begin();
                auto _vE = vis.end();
                for (; _vI != _vE; _vI++)
                {
                    SlotItemVec* items = *_vI;
                    SlotItemVecIt _iI = items->begin();
                    SlotItemVecIt _iE = items->end();
                    for (; _iI != _iE; _iI++)
                    {
                        if (*_iI == nullptr)
                            continue;

                        SlotItem& Instance = **_iI;
                        u32 base = dwBatch * 4;

                        // Build matrix ( 3x4 matrix, last row - color )
                        float scale = Instance.scale_calculated;

                        // Sort of fade using the scale
                        // fade_distance == -1 use light_position to define "fade", anything else uses fade_distance
                        if (fade_distance <= -1)
                            scale *= 1.0f - Instance.position.distance_to_xz_sqr(light_position) * 0.005f;
                        else if (Instance.distance > fade_distance)
                            scale *= 1.0f - abs(Instance.distance - fade_distance) * 0.005f;
                        if (scale <= 0)
                            break;
                        // Build matrix ( 3x4 matrix, last row - color )
                        // float scale = Instance.scale_calculated;

                        Fmatrix& M = Instance.mRotY;
                        c_storage[base + 0].set(M._11 * scale, M._21 * scale, M._31 * scale, M._41);
                        c_storage[base + 1].set(M._12 * scale, M._22 * scale, M._32 * scale, M._42);
                        c_storage[base + 2].set(M._13 * scale, M._23 * scale, M._33 * scale, M._43);

                        // Build color
                        // R2 only needs hemisphere
                        float h = Instance.c_hemi;
                        float s = Instance.c_sun;
                        c_storage[base + 3].set(s, s, s, h);
                        dwBatch++;
                        if (dwBatch == hw_BatchSize)
                        {
                            // flush
                            Device.Statistic->RenderDUMP_DT_Count += dwBatch;
                            u32 dwCNT_verts = dwBatch * Object.number_vertices;
                            u32 dwCNT_prims = (dwBatch * Object.number_indices) / 3;
                            RCache.Render(D3DPT_TRIANGLELIST, vOffset, 0, dwCNT_verts, iOffset, dwCNT_prims);
                            RCache.stat.r.s_details.add(dwCNT_verts);

                            // restart
                            dwBatch = 0;

                            //	Remap constants to memory directly (just in case anything goes wrong)
                            RCache.get_ConstantDirect(strArray, hw_BatchSize * sizeof(Fvector4) * 4, reinterpret_cast<void**>(&c_storage), nullptr, nullptr);
                            R_ASSERT(c_storage);
                        }
                    }
                }

                // flush if nessecary
                if (dwBatch)
                {
                    Device.Statistic->RenderDUMP_DT_Count += dwBatch;
                    u32 dwCNT_verts = dwBatch * Object.number_vertices;
                    u32 dwCNT_prims = (dwBatch * Object.number_indices) / 3;
                    RCache.Render(D3DPT_TRIANGLELIST, vOffset, 0, dwCNT_verts, iOffset, dwCNT_prims);
                    RCache.stat.r.s_details.add(dwCNT_verts);
                }
            }
        }
        vOffset += hw_BatchSize * Object.number_vertices;
        iOffset += hw_BatchSize * Object.number_indices;
    }
}

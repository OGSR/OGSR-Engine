// DetailManager.cpp: implementation of the CDetailManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DetailManager.h"

//--------------------------------------------------- Decompression
constexpr int magic4x4[4][4] = {{0, 14, 3, 13}, {11, 5, 8, 6}, {12, 2, 15, 1}, {7, 9, 4, 10}};

void bwdithermap(int levels, int magic[16][16])
{
    /* Get size of each step */
    const float N = 255.0f / (levels - 1);

    /*
     * Expand 4x4 dither pattern to 16x16.  4x4 leaves obvious patterning,
     * and doesn't give us full intensity range (only 17 sublevels).
     *
     * magicfact is (N - 1)/16 so that we get numbers in the matrix from 0 to
     * N - 1: mod N gives numbers in 0 to N - 1, don't ever want all
     * pixels incremented to the next level (this is reserved for the
     * pixel value with mod N == 0 at the next level).
     */

    const float magicfact = (N - 1) / 16;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 4; k++)
                for (int l = 0; l < 4; l++)
                    magic[4 * k + i][4 * l + j] = (int)(0.5 + magic4x4[i][j] * magicfact + (magic4x4[k][l] / 16.) * magicfact);
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDetailManager::CDetailManager()
{
    dtFS = nullptr;
    dtSlots = nullptr;

    // KD: variable detail radius
    dm_size = dm_current_size;
    dm_cache_line = dm_current_cache_line;
    dm_cache1_line = dm_current_cache1_line;
    dm_cache_size = dm_current_cache_size;
    dm_fade = dm_current_fade;
    
    cache_level1 = (CacheSlot1**)xr_malloc(dm_cache1_line * sizeof(CacheSlot1*));
    for (u32 i = 0; i < dm_cache1_line; ++i)
    {
        cache_level1[i] = (CacheSlot1*)xr_malloc(dm_cache1_line * sizeof(CacheSlot1));
        for (u32 j = 0; j < dm_cache1_line; ++j)
            new (&(cache_level1[i][j])) CacheSlot1();
    }

    cache = (Slot***)xr_malloc(dm_cache_line * sizeof(Slot**));
    for (u32 i = 0; i < dm_cache_line; ++i)
        cache[i] = (Slot**)xr_malloc(dm_cache_line * sizeof(Slot*));

    cache_pool = (Slot*)xr_malloc(dm_cache_size * sizeof(Slot));
    for (u32 i = 0; i < dm_cache_size; ++i)
        new (&(cache_pool[i])) Slot();
}

CDetailManager::~CDetailManager()
{
    if (dtFS)
    {
        FS.r_close(dtFS);
        dtFS = nullptr;
    }

    for (u32 i = 0; i < dm_cache_size; ++i)
        cache_pool[i].~Slot();
    xr_mfree(cache_pool);

    for (u32 i = 0; i < dm_cache_line; ++i)
        xr_mfree(cache[i]);
    xr_mfree(cache);

    for (u32 i = 0; i < dm_cache1_line; ++i)
    {
        for (u32 j = 0; j < dm_cache1_line; ++j)
            cache_level1[i][j].~CacheSlot1();
        xr_mfree(cache_level1[i]);
    }
    xr_mfree(cache_level1);
}

void CDetailManager::Load()
{
    // Open file stream
    if (!FS.exist(fsgame::level, fsgame::level_files::level_details))
    {
        dtFS = nullptr;
        return;
    }

    string_path fn;
    FS.update_path(fn, fsgame::level, fsgame::level_files::level_details);
    dtFS = FS.r_open(fn);

    // Header
    dtFS->r_chunk_safe(0, &dtH, sizeof(dtH));
    R_ASSERT(dtH.version == DETAIL_VERSION);
    const u32 m_count = dtH.object_count;

    // Models
    IReader* m_fs = dtFS->open_chunk(1);
    objects.reserve(m_count);
    for (u32 m_id = 0; m_id < m_count; m_id++)
    {
        CDetail& dt = objects.emplace_back();
        IReader* S = m_fs->open_chunk(m_id);
        dt.Load(S);
        S->close();
    }
    m_fs->close();

    // Get pointer to database (slots)
    IReader* m_slots = dtFS->open_chunk(2);
    dtSlots = (DetailSlot*)m_slots->pointer();
    m_slots->close();

    // Initialize 'vis' and 'cache'
    m_visibles.resize(objects.size());

    cache_Initialize();

    // Make dither matrix
    bwdithermap(2, dither);
}

void CDetailManager::Unload()
{
    {
        check_lock.lock();

        if (awaiter.valid())
            awaiter.wait();

        async_started = false;

        check_lock.unlock();
    }

    for (auto& object : objects)
        object.Unload();
    objects.clear();

    for (auto& vec : m_visibles)
        vec.clear();

    FS.r_close(dtFS);
    dtFS = nullptr;
}

extern ECORE_API float r_ssaDISCARD;
extern BOOL ps_no_scale_on_fade;

static float go_to_target(float& current, const float target)
{
    const float diff = abs(current - target);
    const float r_value = Device.fTimeDelta;
    if (diff - r_value <= 0.f)
    {
        current = target;
        return 0.f;
    }
    return current < target ? r_value : -r_value;
}

void CDetailManager::UpdateVisibleM()
{
    // сюда можно всунуть многопоток но смысла мало - оно всеравно успевает отработать раньше чем начало рендера

    ZoneScoped;

    // Clean up
    for (auto& vec : m_visibles)
        vec.clear();

    const Fvector EYE = Device.vCameraPositionSaved;

    CFrustum View;
    View.CreateFromMatrix(Device.mFullTransformSaved, FRUSTUM_P_LRTB + FRUSTUM_P_FAR);

    const float fade_limit_sqr = dm_fade * dm_fade;
    const float fade_start_sqr = 1.f * 1.f;

    const float fade_range = fade_limit_sqr - fade_start_sqr;

    // Initialize 'vis' and 'cache'
    // Collect objects for rendering
    Device.Statistic->RenderDUMP_DT_VIS.Begin();
    for (u32 _mz = 0; _mz < dm_cache1_line; _mz++)
    {
        for (u32 _mx = 0; _mx < dm_cache1_line; _mx++)
        {
            CacheSlot1& MS = cache_level1[_mz][_mx];
            if (MS.empty)
            {
                continue;
            }

            u32 mask = 0xff;
            const u32 res = View.testSphere(MS.vis.sphere.P, MS.vis.sphere.R, mask);
            if (fcvNone == res)
            {
                continue; // invisible-view frustum
            }

            const u32 dwCC = dm_cache1_count * dm_cache1_count;

            for (u32 _i = 0; _i < dwCC; _i++)
            {
                Slot* PS = *MS.slots[_i];
                Slot& S = *PS;

                // if slot empty - continue
                if (S.empty)
                {
                    continue;
                }

                // if upper test = fcvPartial - test inner slots
                if (fcvPartial == res)
                {
                    u32 _mask = mask;
                    const u32 _res = View.testSphere(S.vis.sphere.P, S.vis.sphere.R, _mask);
                    if (fcvNone == _res)
                    {
                        continue; // invisible-view frustum
                    }
                }

                if (!RImplementation.HOM.visible(S.vis))
                {
                    continue; // invisible-occlusion
                }

                // Add to visibility structures
                if (Device.dwFrame > S.frame)
                {
                    // Calc fade factor	(per slot)
                    const float dist_sqr = EYE.distance_to_sqr(S.vis.sphere.P);
                    if (dist_sqr > fade_limit_sqr)
                    {
                        S.hidden = true;
                        continue;
                    }

                    S.frame = Device.dwFrame + Random.randI(15, 30);

                    const float alpha = (dist_sqr < fade_start_sqr) ? 0.f : (dist_sqr - fade_start_sqr) / fade_range;
                    const float alpha_i = 1.f - alpha;
                    const float dist_sq_rcp = 1.f / dist_sqr;

                    for (auto& sp : S.G)
                    {
                        if (sp.id == DetailSlot::ID_Empty)
                            continue;

                        sp.r_items.clear();

                        float R = objects.at(sp.id).bv_sphere.R;
                        float Rq_drcp = R * R * dist_sq_rcp; // reordered expression for 'ssa' calc

                        for (auto& Item : sp.items)
                        {
                            const float scale = ps_no_scale_on_fade ? (Item.scale_calculated = Item.scale) : (Item.scale_calculated = Item.scale * alpha_i);
                            const float ssa = ps_no_scale_on_fade ? scale : scale * scale * Rq_drcp;

                            const Fmatrix& M = Item.xform;
                            Item.data.data[0] = M._11 * Item.scale_calculated;
                            Item.data.data[1] = M._21 * Item.scale_calculated;
                            Item.data.data[2] = M._31 * Item.scale_calculated;

                            Item.data.data[4] = M._12 * Item.scale_calculated;
                            Item.data.data[5] = M._22 * Item.scale_calculated;
                            Item.data.data[6] = M._32 * Item.scale_calculated;

                            Item.data.data[8] = M._13 * Item.scale_calculated;
                            Item.data.data[9] = M._23 * Item.scale_calculated;
                            Item.data.data[10] = M._33 * Item.scale_calculated;

                            if (ssa < r_ssaDISCARD)
                            {
                                Item.alpha_target = 0;
                            }
                            else
                            {

                                bool should_add = true;
                                if (!fis_zero(ps_r2_no_details_radius))
                                {
                                    if ((dist_sqr < ps_r2_no_details_radius * ps_r2_no_details_radius))
                                        should_add = false;
                                }

                                if (should_add)
                                {
                                    if (S.hidden)
                                    {
                                        Item.alpha = 0;
                                        S.hidden = false;
                                    }
                                    Item.alpha_target = 1;

                                    sp.r_items.push_back(&Item);
                                }
                            }

                            if (Device.dwPrecacheFrame)
                            {
                                Item.alpha = Item.alpha_target;
                            }
                        }
                    }
                }

                for (auto& sp : S.G)
                {
                    if (sp.id == DetailSlot::ID_Empty)
                        continue;

                    if (!sp.r_items.empty())
                    {
                        m_visibles[sp.id].push_back(&sp.r_items);
                    }
                }
            }
        }
    }
    Device.Statistic->RenderDUMP_DT_VIS.End();

    // update alpha. it takes time but still much faster than update during render
    for (auto& vec : m_visibles)
    {
        u32 cnt = 0;

        for (const auto& slot_items : vec)
        {
            for (const auto& instance : *slot_items)
            {
                if (!fsimilar(instance->alpha, instance->alpha_target))
                {
                    instance->alpha += go_to_target(instance->alpha, instance->alpha_target);
                    instance->alpha = std::clamp(instance->alpha, 0.0f, 1.0f);
                }

                instance->data.data[13] = instance->alpha;

                if (instance->alpha > 0.f && instance->scale_calculated > 0.f)
                    cnt++;
            }
        }

        vec.instance_count = cnt;
    }
}

void CDetailManager::Render(CBackend& cmd_list, const bool shadows, light* L)
{
    if (!RImplementation.Details)
        return; // possibly deleted
    if (!dtFS)
        return;
    if (!psDeviceFlags.is(rsDetails))
        return;
    if (CRender::ShouldSkipRender())
        return;

    ZoneScoped;

    {
        check_lock.lock();

        if (awaiter.valid())
            awaiter.wait();

        async_started = false;

        check_lock.unlock();
    }

    Device.Statistic->RenderDUMP_DT_Render.Begin();

    cmd_list.set_CullMode(CULL_NONE);

    cmd_list.set_xform_world(Fidentity);
    cmd_list.set_xform_world_old(Fidentity);

    hw_Render(cmd_list, shadows, L);

    cmd_list.set_CullMode(CULL_CCW);

    Device.Statistic->RenderDUMP_DT_Render.End();
}

u32 reset_frame = 0;

void CDetailManager::StartCalculationAsync()
{
    check_lock.lock();

    if (!async_started)
    {
        awaiter = TTAPI->submit([this]() { MT_CALC(); });
        async_started = true;
    }

    check_lock.unlock();
}

void CDetailManager::MT_CALC()
{
    if (reset_frame == Device.dwFrame)
        return;
    if (!RImplementation.Details)
        return; // possibly deleted
    if (!dtFS)
        return;
    if (!psDeviceFlags.is(rsDetails))
        return;
    if (RImplementation.ShouldSkipRender())
        return;

    ZoneScoped;

    if (need_init)
    {
        need_init = false;
        cache_Initialize();
    }

    Fvector EYE = Device.vCameraPositionSaved;

    const int s_x = iFloor(EYE.x / dm_slot_size + .5f);
    const int s_z = iFloor(EYE.z / dm_slot_size + .5f);

    Device.Statistic->RenderDUMP_DT_Cache.Begin();
    cache_Update(s_x, s_z, EYE);
    Device.Statistic->RenderDUMP_DT_Cache.End();

    UpdateVisibleM();
}

void CDetailManager::Clear()
{
    //if (ps_ssfx_grass_shadows.x <= 0)
    //    return;

    //for (auto& list : m_visibles)
    //    for (auto& vis : list)
    //        vis.clear();
}
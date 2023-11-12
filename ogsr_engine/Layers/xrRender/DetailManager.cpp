// DetailManager.cpp: implementation of the CDetailManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "DetailManager.h"

#include "cl_intersect.h"

#include "../../xr_3da/igame_persistent.h"
#include "../../xr_3da/environment.h"
#include <xmmintrin.h>

const float dbgOffset = 0.f;
const int dbgItems = 128;

//--------------------------------------------------- Decompression
static int magic4x4[4][4] = {{0, 14, 3, 13}, {11, 5, 8, 6}, {12, 2, 15, 1}, {7, 9, 4, 10}};

void bwdithermap(int levels, int magic[16][16])
{
    /* Get size of each step */
    float N = 255.0f / (levels - 1);

    /*
     * Expand 4x4 dither pattern to 16x16.  4x4 leaves obvious patterning,
     * and doesn't give us full intensity range (only 17 sublevels).
     *
     * magicfact is (N - 1)/16 so that we get numbers in the matrix from 0 to
     * N - 1: mod N gives numbers in 0 to N - 1, don't ever want all
     * pixels incremented to the next level (this is reserved for the
     * pixel value with mod N == 0 at the next level).
     */

    float magicfact = (N - 1) / 16;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 4; k++)
                for (int l = 0; l < 4; l++)
                    magic[4 * k + i][4 * l + j] = (int)(0.5 + magic4x4[i][j] * magicfact + (magic4x4[k][l] / 16.) * magicfact);
}
//--------------------------------------------------- Decompression

void CDetailManager::SSwingValue::lerp(const SSwingValue& A, const SSwingValue& B, float f)
{
    float fi = 1.f - f;
    amp1 = fi * A.amp1 + f * B.amp1;
    amp2 = fi * A.amp2 + f * B.amp2;
    rot1 = fi * A.rot1 + f * B.rot1;
    rot2 = fi * A.rot2 + f * B.rot2;
    speed = fi * A.speed + f * B.speed;
}
//---------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDetailManager::CDetailManager()
{
    dtFS = 0;
    dtSlots = 0;
    soft_Geom = 0;
    hw_Geom = 0;
    hw_BatchSize = 0;
    hw_VB = 0;
    hw_IB = 0;
    m_time_rot_1 = 0;
    m_time_rot_2 = 0;
    m_time_pos = 0;
    m_global_time_old = 0;

    // KD: variable detail radius
    dm_size = dm_current_size;
    dm_cache_line = dm_current_cache_line;
    dm_cache1_line = dm_current_cache1_line;
    dm_cache_size = dm_current_cache_size;
    dm_fade = dm_current_fade;
    ps_r__Detail_density = ps_current_detail_density;
    cache_level1 = (CacheSlot1**)Memory.mem_alloc(dm_cache1_line * sizeof(CacheSlot1*));
    for (u32 i = 0; i < dm_cache1_line; ++i)
    {
        cache_level1[i] = (CacheSlot1*)Memory.mem_alloc(dm_cache1_line * sizeof(CacheSlot1));
        for (u32 j = 0; j < dm_cache1_line; ++j)
            new (&(cache_level1[i][j])) CacheSlot1();
    }

    cache = (Slot***)Memory.mem_alloc(dm_cache_line * sizeof(Slot**));
    for (u32 i = 0; i < dm_cache_line; ++i)
        cache[i] = (Slot**)Memory.mem_alloc(dm_cache_line * sizeof(Slot*));

    cache_pool = (Slot*)Memory.mem_alloc(dm_cache_size * sizeof(Slot));
    for (u32 i = 0; i < dm_cache_size; ++i)
        new (&(cache_pool[i])) Slot();
}

CDetailManager::~CDetailManager()
{
    if (dtFS)
    {
        FS.r_close(dtFS);
        dtFS = NULL;
    }

    for (u32 i = 0; i < dm_cache_size; ++i)
        cache_pool[i].~Slot();
    Memory.mem_free(cache_pool);

    for (u32 i = 0; i < dm_cache_line; ++i)
        Memory.mem_free(cache[i]);
    Memory.mem_free(cache);

    for (u32 i = 0; i < dm_cache1_line; ++i)
    {
        for (u32 j = 0; j < dm_cache1_line; ++j)
            cache_level1[i][j].~CacheSlot1();
        Memory.mem_free(cache_level1[i]);
    }
    Memory.mem_free(cache_level1);
}
/*
 */

/*
void dump	(CDetailManager::vis_list& lst)
{
    for (int i=0; i<lst.size(); i++)
    {
        Msg("%8x / %8x / %8x",	lst[i]._M_start, lst[i]._M_finish, lst[i]._M_end_of_storage._M_data);
    }
}
*/
void CDetailManager::Load()
{
    // Open file stream
    if (!FS.exist("$level$", "level.details"))
    {
        dtFS = NULL;
        return;
    }

    string_path fn;
    FS.update_path(fn, "$level$", "level.details");
    dtFS = FS.r_open(fn);

    // Header
    dtFS->r_chunk_safe(0, &dtH, sizeof(dtH));
    R_ASSERT(dtH.version == DETAIL_VERSION);
    u32 m_count = dtH.object_count;

    // Models
    IReader* m_fs = dtFS->open_chunk(1);
    for (u32 m_id = 0; m_id < m_count; m_id++)
    {
        CDetail* dt = xr_new<CDetail>();
        IReader* S = m_fs->open_chunk(m_id);
        dt->Load(S);
        objects.push_back(dt);
        S->close();
    }
    m_fs->close();

    // Get pointer to database (slots)
    IReader* m_slots = dtFS->open_chunk(2);
    dtSlots = (DetailSlot*)m_slots->pointer();
    m_slots->close();

    // Initialize 'vis' and 'cache'
    for (u32 i = 0; i < 3; ++i)
        m_visibles[i].resize(objects.size());
    cache_Initialize();

    // Make dither matrix
    bwdithermap(2, dither);

    // Hardware specific optimizations
    if (UseVS())
        hw_Load();
    else
        soft_Load();

    // swing desc
    // normal
    swing_desc[0].amp1 = pSettings->r_float("details", "swing_normal_amp1");
    swing_desc[0].amp2 = pSettings->r_float("details", "swing_normal_amp2");
    swing_desc[0].rot1 = pSettings->r_float("details", "swing_normal_rot1");
    swing_desc[0].rot2 = pSettings->r_float("details", "swing_normal_rot2");
    swing_desc[0].speed = pSettings->r_float("details", "swing_normal_speed");
    // fast
    swing_desc[1].amp1 = pSettings->r_float("details", "swing_fast_amp1");
    swing_desc[1].amp2 = pSettings->r_float("details", "swing_fast_amp2");
    swing_desc[1].rot1 = pSettings->r_float("details", "swing_fast_rot1");
    swing_desc[1].rot2 = pSettings->r_float("details", "swing_fast_rot2");
    swing_desc[1].speed = pSettings->r_float("details", "swing_fast_speed");
}

void CDetailManager::Unload()
{
    if (UseVS())
        hw_Unload();
    else
        soft_Unload();

    for (DetailIt it = objects.begin(); it != objects.end(); it++)
    {
        (*it)->Unload();
        xr_delete(*it);
    }
    objects.clear();
    m_visibles[0].clear();
    m_visibles[1].clear();
    m_visibles[2].clear();
    FS.r_close(dtFS);
    dtFS = NULL;
}

extern ECORE_API float r_ssaDISCARD;
extern BOOL ps_no_scale_on_fade;

void CDetailManager::UpdateVisibleM()
{
    // Фикс мерцания и прочих глюков травы при активном двойном рендеринге. Для теней от травы SSS16 фикс не нужен, наоборот вредит.
   // if (Device.m_SecondViewport.IsSVPFrame())
   //     return;

    // Clean up
    for (auto& vec : m_visibles)
        for (auto& vis : vec)
            vis.clear();

    Fvector EYE = RDEVICE.vCameraPosition_saved;

    CFrustum View;
    View.CreateFromMatrix(RDEVICE.mFullTransform_saved, FRUSTUM_P_LRTB + FRUSTUM_P_FAR);

    float fade_limit = dm_fade;
    fade_limit = fade_limit * fade_limit;
    float fade_start = 1.f;
    fade_start = fade_start * fade_start;
    float fade_range = fade_limit - fade_start;
    float r_ssaCHEAP = 16 * r_ssaDISCARD;

    // Initialize 'vis' and 'cache'
    // Collect objects for rendering
    RDEVICE.Statistic->RenderDUMP_DT_VIS.Begin();
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
            u32 res = View.testSphere(MS.vis.sphere.P, MS.vis.sphere.R, mask);
            if (fcvNone == res)
            {
                continue; // invisible-view frustum
            }
            // test slots

            u32 dwCC = dm_cache1_count * dm_cache1_count;

            for (u32 _i = 0; _i < dwCC; _i++)
            {
                Slot* PS = *MS.slots[_i];
                Slot& S = *PS;

                //				if ( ( _i + 1 ) < dwCC );
                //					_mm_prefetch( (char *) *MS.slots[ _i + 1 ]  , _MM_HINT_T1 );

                // if slot empty - continue
                if (S.empty)
                {
                    continue;
                }

                // if upper test = fcvPartial - test inner slots
                if (fcvPartial == res)
                {
                    u32 _mask = mask;
                    u32 _res = View.testSphere(S.vis.sphere.P, S.vis.sphere.R, _mask);
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
                if (RDEVICE.dwFrame > S.frame)
                {
                    // Calc fade factor	(per slot)
                    float dist_sq = EYE.distance_to_sqr(S.vis.sphere.P);
                    if (dist_sq > fade_limit)
                        continue;
                    float alpha = (dist_sq < fade_start) ? 0.f : (dist_sq - fade_start) / fade_range;
                    float alpha_i = 1.f - alpha;
                    float dist_sq_rcp = 1.f / dist_sq;

                    S.frame = RDEVICE.dwFrame + Random.randI(15, 30);
                    for (int sp_id = 0; sp_id < dm_obj_in_slot; sp_id++)
                    {
                        SlotPart& sp = S.G[sp_id];
                        if (sp.id == DetailSlot::ID_Empty)
                            continue;

                        sp.r_items[0].clear();
                        sp.r_items[1].clear();
                        sp.r_items[2].clear();

                        float R = objects[sp.id]->bv_sphere.R;
                        float Rq_drcp = R * R * dist_sq_rcp; // reordered expression for 'ssa' calc

                        for (auto& el : sp.items)
                        {
                            if (el == nullptr)
                                continue;

                            SlotItem& Item = *el;
                            float scale = ps_no_scale_on_fade ? (Item.scale_calculated = Item.scale) : (Item.scale_calculated = Item.scale * alpha_i);
                            float ssa = ps_no_scale_on_fade ? scale : scale * scale * Rq_drcp;
                            if (ssa < r_ssaDISCARD)
                            {
                                continue;
                            }
                            u32 vis_id = 0;
                            if (ssa > r_ssaCHEAP)
                                vis_id = Item.vis_ID;

                            sp.r_items[vis_id].push_back(el);
                            Item.distance = dist_sq;
                            Item.position = S.vis.sphere.P;

                            // 2							visible[vis_id][sp.id].push_back(&Item);
                        }
                    }
                }
                for (int sp_id = 0; sp_id < dm_obj_in_slot; sp_id++)
                {
                    SlotPart& sp = S.G[sp_id];
                    if (sp.id == DetailSlot::ID_Empty)
                        continue;
                    if (!sp.r_items[0].empty())
                    {
                        m_visibles[0][sp.id].push_back(&sp.r_items[0]);
                    }
                    if (!sp.r_items[1].empty())
                    {
                        m_visibles[1][sp.id].push_back(&sp.r_items[1]);
                    }
                    if (!sp.r_items[2].empty())
                    {
                        m_visibles[2][sp.id].push_back(&sp.r_items[2]);
                    }
                }
            }
        }
    }
    RDEVICE.Statistic->RenderDUMP_DT_VIS.End();
}

void CDetailManager::Render()
{
    if (!RImplementation.Details)
        return; // possibly deleted
    if (!dtFS)
        return;
    if (!psDeviceFlags.is(rsDetails))
        return;
    if (g_pGamePersistent && g_pGamePersistent->m_pMainMenu && g_pGamePersistent->m_pMainMenu->IsActive())
        return;

    // MT wait
    if (ps_r2_ls_flags.test((u32)R2FLAG_EXP_MT_DETAILS) && async_started)
    {
        //Msg("--[%s] async! frame №[%u] svpactive:[%d], svpframe:[%d]", __FUNCTION__, Device.dwFrame, Device.m_SecondViewport.IsSVPActive(), Device.m_SecondViewport.IsSVPFrame());
        WaitAsync();
    }
    else
    {
        //Msg("~~[%s] NO async! frame №[%u] svpactive:[%d], svpframe:[%d]", __FUNCTION__, Device.dwFrame, Device.m_SecondViewport.IsSVPActive(), Device.m_SecondViewport.IsSVPFrame());
        MT_CALC();
    }

    RDEVICE.Statistic->RenderDUMP_DT_Render.Begin();

    const float factor = g_pGamePersistent->Environment().wind_strength_factor;

    swing_current.lerp(swing_desc[0], swing_desc[1], factor);

    RCache.set_CullMode(CULL_NONE);
    RCache.set_xform_world(Fidentity);

    if (UseVS())
        hw_Render();
    else
        soft_Render();

    RCache.set_CullMode(CULL_CCW);
    RDEVICE.Statistic->RenderDUMP_DT_Render.End();
}

u32 reset_frame = 0;

void CDetailManager::StartAsync()
{
    if (!(ps_r2_ls_flags.test((u32)R2FLAG_EXP_MT_DETAILS) && //костыли чтоб в 3д прицелах не мерцала трава. Фикс не совсем идеальный, иногда всё равно проскакивает, но не критично.
          (Device.m_SecondViewport.IsSVPFrame() || !Device.m_SecondViewport.IsSVPActive() || (((Device.dwFrame - 1) % g_3dscopes_fps_factor) != 0))))
    {
        async_started = false;
        return;
    }

    if (reset_frame == Device.dwFrame)
        return;

    if (!RImplementation.Details)
        return; // possibly deleted
    if (!dtFS)
        return;
    if (!psDeviceFlags.is(rsDetails))
        return;
    if (g_pGamePersistent && g_pGamePersistent->m_pMainMenu && g_pGamePersistent->m_pMainMenu->IsActive())
        return;

    //Заметка: сначала рендерится фрейм для 3д прицела, а уже следующий фрейм для всего мира вне прицела
    //Msg("##[%s] frame №[%u] svpactive:[%d], svpframe:[%d]", __FUNCTION__, Device.dwFrame, Device.m_SecondViewport.IsSVPActive(), Device.m_SecondViewport.IsSVPFrame());

    awaiter = TTAPI->submit([this]() { MT_CALC(); });
    async_started = true;
}

void CDetailManager::WaitAsync()
{
    if (awaiter.valid())
        awaiter.get();
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
    if (g_pGamePersistent && g_pGamePersistent->m_pMainMenu && g_pGamePersistent->m_pMainMenu->IsActive())
        return;

    MT.Enter();

    Fvector EYE = RDEVICE.vCameraPosition_saved;

    int s_x = iFloor(EYE.x / dm_slot_size + .5f);
    int s_z = iFloor(EYE.z / dm_slot_size + .5f);

    RDEVICE.Statistic->RenderDUMP_DT_Cache.Begin();
    cache_Update(s_x, s_z, EYE, dm_max_decompress);
    RDEVICE.Statistic->RenderDUMP_DT_Cache.End();

    UpdateVisibleM();

    MT.Leave();
}

void CDetailManager::details_clear()
{
    // Disable fade, next render will be scene
    fade_distance = 99999;
    if (ps_ssfx_grass_shadows.x <= 0)
        return;

    for (auto& list : m_visibles)
        for (auto& vis : list)
            vis.clear();
}
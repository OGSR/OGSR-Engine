// DetailManager.h: interface for the CDetailManager class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "detailformat.h"
#include "detailmodel.h"

const int dm_max_decompress = 7;
const int dm_cache1_count = 4; //
const int dm_max_objects = 64;
const int dm_obj_in_slot = 4;
const float dm_slot_size = DETAIL_SLOT_SIZE;

const u32 dm_max_cache_size = 62001 * 2; // assuming max dm_size = 124

extern u32 dm_size;
extern u32 dm_cache1_line;
extern u32 dm_cache_line;
extern u32 dm_cache_size;
extern float dm_fade;
extern u32 dm_current_size; //				= iFloor((float)ps_r__detail_radius/4)*2;				//!
extern u32 dm_current_cache1_line; //		= dm_current_size*2/dm_cache1_count;		//! dm_current_size*2 must be div dm_cache1_count
extern u32 dm_current_cache_line; //		= dm_current_size+1+dm_current_size;
extern u32 dm_current_cache_size; //		= dm_current_cache_line*dm_current_cache_line;
extern float dm_current_fade; //				= float(2*dm_current_size)-.5f;
extern float ps_current_detail_density;

struct DetailVertData
{
    float data[6];
};

class ECORE_API CDetailManager
{
public:
    static constexpr float max_fade_distance = 99999.f;

private:
    struct SlotItem
    {
        // один кустик
        float scale{};
        float scale_calculated{};

        Fmatrix xform{};

        float alpha{};
        float alpha_target{};

        FloraVertData data{};
    };
    using SlotItemVec = xr_vector<SlotItem*>;
    struct SlotPart
    { //
        u32 id{}; // ID модельки
        xr_vector<SlotItem> items; // список кустиков
        SlotItemVec r_items; // список указателей на кустики for render
    };
    enum SlotType
    {
        stReady = 0, // Ready to use
        stPending, // Pending for decompression

        stFORCEDWORD = 0xffffffff
    };
    struct Slot
    { // распакованый слот размером DETAIL_SLOT_SIZE
        struct
        {
            u32 empty : 1;
            u32 type : 1;
            u32 frame : 30;
        };
        int sx, sz; // координаты слота X x Y
        vis_data vis; //
        SlotPart G[dm_obj_in_slot]; //
        bool hidden;

        Slot()
            : G{}
        {
            frame = 0;
            empty = 1;
            type = stReady;
            sx = sz = 0;
            vis.clear();
        }
    };
    struct CacheSlot1
    {
        u32 empty;
        vis_data vis;
        Slot** slots[dm_cache1_count * dm_cache1_count];
        CacheSlot1()
            : slots{}
        {
            empty = 1;
            vis.clear();
        }
    };
    class vis_list_inner : public xr_vector<SlotItemVec*>
    {
    public:
        u32 instance_count{};
    };

    int dither[16][16];

    IReader* dtFS;
    DetailHeader dtH;
    DetailSlot* dtSlots; // note: pointer into VFS
    DetailSlot DS_empty;

    xr_vector<CDetail> objects;
    xr_vector<vis_list_inner> m_visibles; // per object store vector of pointer to draw items

    xrXRC xrc;
    CacheSlot1** cache_level1;
    Slot*** cache; // grid-cache itself
    svector<Slot*, dm_max_cache_size> cache_task; // non-unpacked slots
    Slot* cache_pool; // just memory for slots
    int cache_cx;
    int cache_cz;

    void UpdateVisibleM();

    static void SetupCBuffer(CBackend& cmd_list, const ref_pass& pass, bool shadows);

    void hw_Render(CBackend& cmd_list, bool shadows, light* L);

    static u32 render_items(CBackend& cmd_list, const CDetail& object, const vis_list_inner& vis_list, bool shadows, light* L);

    std::future<void> awaiter;
    bool async_started{};
    SpinLock check_lock;

    void MT_CALC();

    // get unpacked slot
    DetailSlot& QueryDB(int sx, int sz);

    void cache_Initialize();
    void cache_Update(int sx, int sz, Fvector& view);
    void cache_Task(int gx, int gz, Slot* D);
    void cache_Decompress(Slot* D);

    // cache grid to world
    int cg2w_X(int x) const { return cache_cx - dm_size + x; }
    int cg2w_Z(int z) const { return cache_cz - dm_size + (dm_cache_line - 1 - z); }
    // world to cache grid
    int w2cg_X(int x) const { return x - cache_cx + dm_size; }
    int w2cg_Z(int z) const { return cache_cz - dm_size + (dm_cache_line - 1 - z); }

public:
    bool need_init{};

public:
    CDetailManager();
    virtual ~CDetailManager();

    void StartCalculationAsync();

    void Load();
    void Unload();
    void Render(CBackend& cmd_list, bool shadows = false, light* L = nullptr);
    void Clear();
};

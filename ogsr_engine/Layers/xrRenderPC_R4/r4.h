#pragma once

#include "../xrRender/r__dsgraph_structure.h"
#include "../xrRender/r__occlusion.h"

#include "../xrRender/PSLibrary.h"

#include "r2_types.h"
#include "r4_rendertarget.h"
#include "render_phase_sun.h"

#include "../xrRender/hom.h"
#include "../xrRender/detailmanager.h"
#include "../xrRender/modelpool.h"
#include "../xrRender/wallmarksengine.h"

#include "../xrRender/light_db.h"
#include "../xrRender/LightTrack.h"
#include "../xrRender/r_sun_cascades.h"

#include "../../xr_3da/irenderable.h"
#include "../../xr_3da/fmesh.h"

struct render_main /*: public i_render_phase*/
{
    render_main() /*: i_render_phase("main_render")*/ {}

    void init();

    void calculate_static();
    void calculate_dynamic();

    void ensure_calculate_static();

    void wait_static() const;
    void wait_dynamic() const;

    void sync() const;

private:
    std::future<void> static_waiter;
    std::future<void> dynamic_waiter;

    u32 static_calc_frame{};
};

class dxRender_Visual;
struct light_ctx;

// definition
class CRender : public IRender_interface, public pureFrame
{
public:
    enum
    {
        PHASE_NORMAL = 0, // E[0]
        PHASE_SMAP = 1, // E[1]
    };

public:
    struct _options
    {
        u32 rain_smapsize : 16;
        u32 smapsize : 16;

        u32 distortion : 1;
        u32 distortion_enabled : 1;

        u32 noshadows : 1;
        u32 disasm : 1;

        u32 dx11_enable_tessellation : 1;
    } o;
    struct _stats
    {
        u32 l_total, l_visible;
        u32 l_shadowed, l_unshadowed;
        s32 s_used, s_merged, s_finalclip;
        u32 o_queries, o_culled;
        u32 ic_total, ic_culled;
    } stats;

public:
    // Sector detection and visibility
    IRender_Sector::sector_id_t last_sector_id{IRender_Sector::INVALID_SECTOR_ID};
    u32 uLastLTRACK;

    xrXRC main_xrc;
    xrXRC light_sectors_xrc;
    xrXRC renderable_sectors_xrc;

    CDB::MODEL* rmPortals{};
    CHOM HOM;
    R_occlusion HWOCC;

    // Global vertex-buffer container
    xr_vector<FSlideWindowItem> SWIs;
    xr_vector<ref_shader> Shaders;
    typedef svector<D3DVERTEXELEMENT9, MAXD3DDECLLENGTH + 1> VertexDeclarator;
    xr_vector<VertexDeclarator> nDC, xDC;
    xr_vector<ID3DVertexBuffer*> nVB, xVB;
    xr_vector<ID3DIndexBuffer*> nIB, xIB;

    xr_vector<dxRender_Visual*> Visuals;

    // Dynamic geometry streams
    _VertexStream Vertex;
    _IndexStream Index;

    ID3DIndexBuffer* QuadIB;
    ID3DIndexBuffer* old_QuadIB;

    CPSLibrary PSLibrary;

    CDetailManager* Details;
    CModelPool* Models;
    CWallmarksEngine* Wallmarks;

    CRenderTarget* Target; // Render-target

    CLight_DB Lights;

    xr_vector<light*> Lights_LastFrame;
    //SMAP_Allocator LP_smap_pool;
    //light_Package LP_normal;

    xr_vector<Fbox3> main_coarse_structure;

    bool b_loaded{};
    bool m_bFirstFrameAfterReset; // Determines weather the frame is the first after resetting device.

    xr_vector<sun::cascade> m_sun_cascades;

    bool need_to_render_sunshafts{false};
    bool last_cascade_chain_mode{false};

private:
    struct Puddle
    {
        Fmatrix xform{Fidentity};
        float height{}, radius{};
    };
    xr_vector<Puddle> current_level_puddles;

    // Loading / Unloading
    void LoadBuffers(CStreamReader* fs, BOOL _alternative);
    void LoadVisuals(IReader* fs);
    void LoadLights(IReader* fs);
    void LoadSectors(IReader* fs);
    void LoadSWIs(CStreamReader* fs);
    void Load3DFluid();

public:
    static bool InFieldOfViewR(Fvector pos, float max_dist, bool check_direction);

public:
    void main_pass_static(R_dsgraph_structure& dsgraph);
    void main_pass_dynamic(R_dsgraph_structure& dsgraph, bool fill_lights);

    void render_forward();

private:
    void render_lights_shadowed_one(light_ctx& task);
    void render_lights_shadowed(light_Package& LP);
    void render_lights(light_Package& LP);

    render_main r_main;
    render_sun r_sun;
    render_rain r_rain;

private:
    std::future<void> particles_async_awaiter;
    std::future<void> bones_async_awaiter;
    std::future<void> update_sectors_awaiter;

    xr_vector<ISpatial*> lstParticlesCalculation;
    xr_vector<ISpatial*> lstBonesCalculation; 
    xr_vector<ISpatial*> lstUpdateSector; 

    // constexpr unsigned number_of_threads = 8;

    task_thread_pool::task_thread_pool particles_pool{"MT_PARTICLES",16};
    task_thread_pool::task_thread_pool light_pool{"MT_LIGHT", R__NUM_PARALLEL_CONTEXTS};

    std::future<void> light_waiter;

public:
    ShaderElement* rimp_select_sh_static(dxRender_Visual* pVisual, float cdist_sq, u32 phase);
    ShaderElement* rimp_select_sh_dynamic(dxRender_Visual* pVisual, float cdist_sq, bool hud, u32 phase);

    D3DVERTEXELEMENT9* getVB_Format(int id, BOOL _alt = FALSE);
    ID3DVertexBuffer* getVB(int id, BOOL _alt = FALSE) const;
    ID3DIndexBuffer* getIB(int id, BOOL _alt = FALSE);
    FSlideWindowItem* getSWI(int id);
    
    // HW-occlusion culling
    IC u32 occq_begin(u32& ID, u32 context_id) { return HWOCC.occq_begin(ID, context_id); }
    IC void occq_end(const u32& ID, u32 context_id) { HWOCC.occq_end(ID, context_id); }
    IC R_occlusion::occq_result occq_get(u32& ID, const float max_wait_occ) { return HWOCC.occq_get(ID, max_wait_occ); }
    IC void occq_free(const u32 ID) { HWOCC.occq_free(ID); }
    IC void occq_enable(const bool v) { HWOCC.set_enabled(v); }

    ICF void apply_object(CBackend& cmd_list, IRenderable* O)
    {
        if (nullptr == O)
            return;
        if (nullptr == O->renderable_ROS())
            return;
        CROS_impl& LT = *((CROS_impl*)O->renderable_ROS());
        LT.update_smooth(O);
        cmd_list.o_hemi = 0.75f * LT.get_hemi();
        cmd_list.o_sun = 0.75f * LT.get_sun();
        cmd_list.hemi.set_hotness(O->GetHotness(), O->GetTransparency(), 0.f, 0.f);
        CopyMemory(cmd_list.o_hemi_cube, LT.get_hemi_cube(), sizeof cmd_list.o_hemi_cube);
    }

public:
    // Constructor/destructor/loader
    CRender();
    virtual ~CRender();

    auto get_largest_sector() const { return largest_sector_id; }

    // Loading / Unloading
    virtual void create();
    virtual void destroy();
    virtual void reset_begin();
    virtual void reset_end();

    virtual void level_Load(IReader*);
    virtual void level_Unload();

    static ID3DBaseTexture* texture_load(LPCSTR fRName, u32& ret_msize);

    virtual HRESULT shader_compile(LPCSTR name, DWORD const* pSrcData, UINT SrcDataLen, LPCSTR pFunctionName, LPCSTR pTarget, DWORD Flags, void*& result);

    void CreateQuadIB();

    // Information
    virtual void Statistics(CGameFont* F);

    virtual LPCSTR getShaderPath() { return "r3\\"; }

    virtual ref_shader getShader(int id);
    
    virtual IRenderVisual* getVisual(int id);
    virtual u32 getVisualCount() { return Visuals.size(); }
    virtual IRender_Target* getTarget();

    // Main
    virtual void add_Visual(u32 context_id, IRenderable* root, IRenderVisual* V, Fmatrix& m); // add visual leaf	(no culling performed at all)

    // wallmarks
    virtual void add_StaticWallmark(IWallMarkArray* pArray, const Fvector& P, float s, CDB::TRI* T, Fvector* V);
    virtual void add_StaticWallmark(const wm_shader& S, const Fvector& P, float s, CDB::TRI* T, Fvector* V);

    virtual void append_SkeletonWallmark(intrusive_ptr<CSkeletonWallmark> wm);
    virtual void add_SkeletonWallmark(Fmatrix* xf, IKinematics* obj, IWallMarkArray* pArray, Fvector& start, Fvector& dir, float size);

    virtual void clear_static_wallmarks();

    //
    virtual IBlenderXr* blender_create(CLASS_ID cls);
    virtual void blender_destroy(IBlenderXr*&);

    //
    virtual IRender_ObjectSpecific* ros_create(IRenderable* parent);
    virtual void ros_destroy(IRender_ObjectSpecific*&);

    // Lighting
    virtual IRender_Light* light_create();

    virtual float GetParticlesTimeLimit(LPCSTR name);

    // Models
    virtual IRenderVisual* model_CreateParticles(LPCSTR name, BOOL bNoPool = FALSE);
    virtual IRenderVisual* model_Create(LPCSTR name, IReader* data = nullptr);
    virtual IRenderVisual* model_CreateChild(LPCSTR name, IReader* data);
    virtual IRenderVisual* model_Duplicate(IRenderVisual* V);

    virtual IRender_DetailModel* model_CreateDM(IReader* F);

    virtual void model_Delete(IRenderVisual*& V, BOOL bDiscard = FALSE);
    virtual void model_Delete(IRender_DetailModel*& F);
    virtual void model_Logging(BOOL bEnable) { Models->Logging(bEnable); }
    virtual void models_Prefetch();
    virtual void models_Clear(BOOL b_complete);
    virtual void models_savePrefetch();
    virtual void models_begin_prefetch1(bool val);

    // Occlusion culling
    //virtual BOOL occ_visible(vis_data& V);
    //virtual BOOL occ_visible(Fbox& B);
    //virtual BOOL occ_visible(sPoly& P);

    void calculate_particles_async();
    void calculate_particles_wait();

    void calculate_bones_async();
    void calculate_bones_wait();

    // Main
    virtual void Calculate();
    virtual void Render();

    virtual void Screenshot(ScreenshotMode mode = SM_NORMAL, LPCSTR name = nullptr);

    virtual void OnFrame();

    void AfterWorldRender() override; //Вызывается после рендера мира и перед UI ПДА
    void AfterUIRender() override; //После рендеринга UI. Вызывать только если нам нужно отрендерить кадр для пда.

    // Render mode
    virtual void rmNear(CBackend& cmd_list);
    virtual void rmFar(CBackend& cmd_list);
    virtual void rmNormal(CBackend& cmd_list);

    void addShaderOption(const char* name, const char* value);
    void clearAllShaderOptions() { m_ShaderOptions.clear(); }

    virtual void ParticleEffectFillName(xr_vector<shared_str>& s);
    virtual void ParticleGroupFillName(xr_vector<shared_str>& s);

    u32 memory_usage() override
    {
        return 0;
    }

    virtual u32 GetCacheStatPolys();

    static bool ShouldSkipRender();

    ICF IRender_Sector* get_sector(const size_t id) const
    {
        return sector_portals_structure.Sectors.at(id);
    }

private:
    xr_vector<D3D_SHADER_MACRO> m_ShaderOptions;

    xr_vector<ISpatial*> lstRenderables;
    xr_vector<ISpatial*> lstLights;

    IRender_Sector::sector_id_t largest_sector_id{IRender_Sector::INVALID_SECTOR_ID};

    SectorPortalStructure sector_portals_structure;

protected:
    virtual void ScreenshotImpl(ScreenshotMode mode, LPCSTR name);

    virtual void add_StaticWallmark(ref_shader& S, const Fvector& P, float s, CDB::TRI* T, Fvector* V);
    virtual void add_SkeletonWallmark(Fmatrix* xf, CKinematics* obj, ref_shader& sh, Fvector& start, Fvector& dir, float size);

    void ExportLights();
    void UpdateSectors();

    IRender_Sector::sector_id_t detect_sector(xrXRC& Sectors_xrc, const Fvector& P) const;
    IRender_Sector::sector_id_t detect_sector(xrXRC& Sectors_xrc, const Fvector& P, const Fvector& D) const;

    void update_sector(xrXRC& Sectors_xrc, ISpatial* S) const;

private:
    R_dsgraph_structure contexts_pool[R__NUM_CONTEXTS];
    bool contexts_used[R__NUM_PARALLEL_CONTEXTS]{};

public:
    virtual CBackend& get_imm_command_list()
    {
        return get_imm_context().cmd_list;
    }

    ICF u32 alloc_context()
    {
        for (size_t id{}; id < std::size(contexts_used); ++id)
        {
            if (!contexts_used[id])
            {
                contexts_used[id] = true;
                auto& ctx = contexts_pool[id];
                ctx.reset();
                ctx.context_id = id;
                ctx.cmd_list.context_id = id;
                return id;
            }
        }

        FATAL("Can't find free context! All contexts used!");
        return CHW::INVALID_CONTEXT_ID;
    }

    ICF R_dsgraph_structure& get_context(u32 id)
    {
        if (id == CHW::IMM_CTX_ID)
            return get_imm_context();

        R_ASSERT(id < std::size(contexts_pool));
        R_ASSERT(contexts_used[id]);
        auto& ctx = contexts_pool[id];
        R_ASSERT(ctx.context_id == id);

        return ctx;
    }

    ICF void release_context(u32 id)
    {
        R_ASSERT(id != CHW::IMM_CTX_ID); // never release immediate context
        R_ASSERT(id < R__NUM_PARALLEL_CONTEXTS);
        R_ASSERT(contexts_used[id]);
        R_ASSERT(contexts_pool[id].context_id != CHW::INVALID_CONTEXT_ID);
        contexts_used[id] = false;
    }

    ICF R_dsgraph_structure& get_imm_context()
    {
        auto& ctx = contexts_pool[CHW::IMM_CTX_ID];
        ctx.context_id = CHW::IMM_CTX_ID;
        return ctx;
    }

    ICF void cleanup_contexts()
    {
        for (auto& id : contexts_pool)
        {
            id.reset();
        }
        ZeroMemory(&contexts_used, sizeof contexts_used);

        //if (R_dsgraph::mapNormalItems::instance_cnt)
        //    Msg("mapNormalItems::instance_cnt={%d}", R_dsgraph::mapNormalItems::instance_cnt);
        //if (R_dsgraph::mapMatrixItems::instance_cnt)
        //    Msg("mapMatrixItems::instance_cnt={%d}", R_dsgraph::mapMatrixItems::instance_cnt);
    }

    virtual void Begin();
    virtual void Clear();
    virtual void End();
    virtual void ClearTarget();

    virtual void SetCacheXform(Fmatrix& mView, Fmatrix& mProject);
    virtual void SetCacheXformOld(Fmatrix& mView, Fmatrix& mProject);

    void OnDeviceCreate();
    void OnDeviceDestroy();

    virtual void OnCameraUpdated(bool from_actor);
};

void fix_texture_name(const char* fn);

extern CRender RImplementation;

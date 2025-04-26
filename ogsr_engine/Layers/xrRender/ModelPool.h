#pragma once

// refs
class dxRender_Visual;
namespace PS
{
struct SEmitter;
};

// defs
class ECORE_API CModelPool
{
private:
    friend class CRender;

    struct ModelDef
    {
        shared_str name;
        dxRender_Visual* model;
        u32 refs;
        ModelDef()
        {
            refs = 0;
            model = nullptr;
        }
    };

    typedef xr_multimap<shared_str, dxRender_Visual*, pred_shared_str> POOL;
    typedef POOL::iterator POOL_IT;

    typedef xr_map<dxRender_Visual*, shared_str> REGISTRY;
    typedef REGISTRY::iterator REGISTRY_IT;

private:
    xr_vector<ModelDef> Models; // Reference / Base
    xr_vector<dxRender_Visual*> ModelsToDelete; //

    SpinLock ModelsToDelete_lock;
    SpinLock ModelsPool_lock;
    SpinLock Registry_lock;

    REGISTRY Registry; // Just pairing of pointer / Name
    POOL Pool; // Unused / Inactive

    BOOL bLogging;
    BOOL bForceDiscard;
    BOOL bAllowChildrenDuplicate;

    string_unordered_map<std::string, bool> m_prefetched;

    void Destroy();
    void process_vis_prefetch() const;
    void refresh_prefetch(const char* low_name, const bool is_hud_visual);

    CInifile* vis_prefetch_ini = nullptr;

    bool now_prefetch1 = false;
    bool now_prefetch2 = false;

    dxRender_Visual* CreatePE(PS::CPEDef* source);
    dxRender_Visual* CreatePG(PS::CPGDef* source);


public:
    CModelPool();
    virtual ~CModelPool();
    dxRender_Visual* Instance_Create(u32 Type);
    dxRender_Visual* Instance_Duplicate(dxRender_Visual* V);
    dxRender_Visual* Instance_Load(LPCSTR N, BOOL allow_register);
    dxRender_Visual* Instance_Load(LPCSTR N, IReader* data, BOOL allow_register);
    void Instance_Register(LPCSTR N, dxRender_Visual* V);
    dxRender_Visual* Instance_Find(LPCSTR N);

    dxRender_Visual* CreateParticles(LPCSTR name, BOOL bNoPool);
    dxRender_Visual* CreateParticleEffect(LPCSTR name);

    dxRender_Visual* Create(LPCSTR name, IReader* data = nullptr);
    dxRender_Visual* CreateChild(LPCSTR name, IReader* data);

    void Delete(dxRender_Visual*& V, BOOL bDiscard = FALSE);
    void Discard(dxRender_Visual*& V, BOOL b_complete);
    void DeleteInternal(dxRender_Visual*& V, BOOL bDiscard = FALSE);
    void DeleteQueue();

    void Logging(BOOL bEnable) { bLogging = bEnable; }

    void Prefetch();
    void ClearPool(BOOL b_complete);

    void dump();

    void memory_stats(u32& vb_mem_video, u32& vb_mem_system, u32& ib_mem_video, u32& ib_mem_system);

	void save_vis_prefetch() const;
	void begin_prefetch1( bool val );

    CInifile* bone_override_ini = nullptr;
    CInifile* omf_override_ini = nullptr;
    CInifile* userdata_override_ini = nullptr;
};

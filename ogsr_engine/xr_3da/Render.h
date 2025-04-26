#pragma once

#include "../xrCDB/frustum.h"

#include "vis_common.h"

#include "../Include/xrAPI/xrAPI.h"
#include "../Include/xrRender/FactoryPtr.h"

class IUIShader;
typedef FactoryPtr<IUIShader> wm_shader;

// refs
class ENGINE_API IRenderable;
struct ENGINE_API FSlideWindowItem;

//	Igor
class IRenderVisual;
class IKinematics;
class CGameFont;

extern const float fLightSmoothFactor;

//////////////////////////////////////////////////////////////////////////
// definition (Dynamic Light)
class ENGINE_API IRender_Light : public xr_resource
{
public:
    enum LT
    {
        DIRECT = 0, // unused ???
        POINT = 1, // OMNI
        SPOT = 2,
        OMNIPART = 3, // SECTION OF OMNI
    };

public:
    virtual void set_type(LT type) = 0;

    virtual void set_active(bool) = 0;
    virtual bool get_active() = 0;

    virtual void set_shadow(bool) = 0;
    virtual bool get_shadow() = 0;

    virtual void set_volumetric(bool) = 0;
    virtual bool get_volumetric() = 0;

    virtual void set_volumetric_quality(float) = 0;
    virtual void set_volumetric_intensity(float) = 0;
    virtual void set_volumetric_distance(float) = 0;

    virtual void set_flare(bool b) = 0;
    virtual bool get_flare() = 0;

    virtual void set_position(const Fvector& P) = 0;
    virtual void set_rotation(const Fvector& D, const Fvector& R) = 0;

    virtual void set_cone(float angle) = 0;

    virtual void set_range(float R) = 0;
    virtual float get_range() const = 0;

    virtual void set_virtual_size(float R) = 0;
    virtual void set_texture(LPCSTR name) = 0;

    virtual void set_color(const Fcolor& C) = 0;
    virtual void set_color(float r, float g, float b) = 0;
    virtual Fcolor get_color() const = 0;

    virtual void set_hud_mode(bool b) = 0;
    virtual bool get_hud_mode() = 0;

    virtual void set_moveable(bool) = 0;
    virtual bool get_moveable() = 0;

    virtual ~IRender_Light();
};
struct ENGINE_API resptrcode_light : public resptr_base<IRender_Light>
{
    void destroy() { _set(nullptr); }
};
typedef resptr_core<IRender_Light, resptrcode_light> ref_light;

//////////////////////////////////////////////////////////////////////////
// definition (Dynamic Glow)
class ENGINE_API IRender_Glow : public xr_resource
{
public:
    virtual void set_active(bool) = 0;
    virtual bool get_active() = 0;
    virtual void set_position(const Fvector& P) = 0;
    virtual void set_direction(const Fvector& P) = 0;
    virtual void set_radius(float R) = 0;
    virtual void set_texture(LPCSTR name) = 0;
    virtual void set_color(const Fcolor& C) = 0;
    virtual void set_color(float r, float g, float b) = 0;

    virtual ~IRender_Glow();
};
struct ENGINE_API resptrcode_glow : public resptr_base<IRender_Glow>
{
    void destroy() { _set(nullptr); }
};
typedef resptr_core<IRender_Glow, resptrcode_glow> ref_glow;

//////////////////////////////////////////////////////////////////////////
// definition (Per-object render-specific data)
class ENGINE_API IRender_ObjectSpecific
{
public:
    enum mode
    {
        TRACE_LIGHTS = (1 << 0),
        TRACE_SUN = (1 << 1),
        TRACE_HEMI = (1 << 2),
        TRACE_ALL = (TRACE_LIGHTS | TRACE_SUN | TRACE_HEMI),
    };

public:
    virtual float get_luminocity() = 0;
    virtual float get_luminocity_hemi() = 0;
    virtual float* get_luminocity_hemi_cube() = 0;

    virtual ~IRender_ObjectSpecific(){};
};

//////////////////////////////////////////////////////////////////////////
// definition (Backend)
class CBackend; // TODO: the real command list interface should be defined here

//////////////////////////////////////////////////////////////////////////
// definition (Target)
class ENGINE_API IRender_Target
{
public:
    virtual void set_blur(float f) = 0;
    virtual void set_gray(float f) = 0;
    virtual void set_duality_h(float f) = 0;
    virtual void set_duality_v(float f) = 0;
    virtual void set_noise(float f) = 0;
    virtual void set_noise_scale(float f) = 0;
    virtual void set_noise_fps(float f) = 0;
    virtual void set_color_base(u32 f) = 0;
    virtual void set_color_gray(u32 f) = 0;
    virtual void set_color_add(const Fvector& f) = 0;
    virtual u32 get_width(CBackend& cmd_list) = 0;
    virtual u32 get_height(CBackend& cmd_list) = 0;
    virtual void set_cm_imfluence(float f) = 0;
    virtual void set_cm_interpolate(float f) = 0;
    virtual void set_cm_textures(const shared_str& tex0, const shared_str& tex1) = 0;
    virtual ~IRender_Target(){};
};

//////////////////////////////////////////////////////////////////////////
// definition (Renderer)
class ENGINE_API IRender_interface
{
public:
    enum ScreenshotMode
    {
        SM_NORMAL = 0, // jpeg,	name ignored
        SM_FOR_CUBEMAP = 1, // tga,		name used as postfix
        SM_FOR_GAMESAVE = 2, // dds/dxt1,name used as full-path
        SM_FOR_LEVELMAP = 3, // tga,		name used as postfix (level_name)
        SM_forcedword = u32(-1)
    };

public:
    // options
    bool hud_loading{};
    //bool HAT{};
    s32 m_skinning;
    u32 m_SMAPSize;

    // data
    CFrustum ViewBase;

public:
  
    // Loading / Unloading
    virtual void create() = 0;
    virtual void destroy() = 0;
    virtual void reset_begin() = 0;
    virtual void reset_end() = 0;

    virtual void level_Load(IReader*) = 0;
    virtual void level_Unload() = 0;

    void shader_option_skinning(s32 mode) { m_skinning = mode; }
    virtual HRESULT shader_compile(LPCSTR name, DWORD const* pSrcData, UINT SrcDataLen, LPCSTR pFunctionName, LPCSTR pTarget, DWORD Flags, void*& result) = 0;

    // Information
    virtual void Statistics(CGameFont* F){};

    virtual LPCSTR getShaderPath() = 0;

    virtual IRenderVisual* getVisual(int id) = 0;
    virtual u32 getVisualCount() = 0;

    virtual IRender_Target* getTarget() = 0;

    // Main
    virtual void add_Visual(u32 context_id, IRenderable* root, IRenderVisual* V, Fmatrix& m) = 0; // add visual leaf	(no culling performed at all)
    virtual void add_StaticWallmark(const wm_shader& S, const Fvector& P, float s, CDB::TRI* T, Fvector* V) = 0;
    //	Prefer this function when possible
    virtual void add_StaticWallmark(IWallMarkArray* pArray, const Fvector& P, float s, CDB::TRI* T, Fvector* V) = 0;
    //	Prefer this function when possible
    virtual void add_SkeletonWallmark(Fmatrix* xf, IKinematics* obj, IWallMarkArray* pArray, Fvector& start, Fvector& dir, float size) = 0;

    virtual void clear_static_wallmarks() = 0;

    virtual IRender_ObjectSpecific* ros_create(IRenderable* parent) = 0;
    virtual void ros_destroy(IRender_ObjectSpecific*&) = 0;

    // Lighting/glowing
    virtual IRender_Light* light_create() = 0;
    virtual void light_destroy(IRender_Light* p_){};
    virtual IRender_Glow* glow_create() = 0;
    virtual void glow_destroy(IRender_Glow* p_){};

    virtual void ParticleEffectFillName(xr_vector<shared_str>& s) = 0;
    virtual void ParticleGroupFillName(xr_vector<shared_str>& s) = 0;

    virtual float GetParticlesTimeLimit(LPCSTR name) = 0;

    // Models
    virtual IRenderVisual* model_CreateParticles(LPCSTR name, BOOL bNoPool = FALSE) = 0;

    virtual IRenderVisual* model_Create(LPCSTR name, IReader* data = nullptr) = 0;
    virtual IRenderVisual* model_CreateChild(LPCSTR name, IReader* data) = 0;
    virtual IRenderVisual* model_Duplicate(IRenderVisual* V) = 0;

    virtual void model_Delete(IRenderVisual*& V, BOOL bDiscard = FALSE) = 0;
    virtual void model_Logging(BOOL bEnable) = 0;

    virtual void models_Prefetch() = 0;
    virtual void models_Clear(BOOL b_complete) = 0;
    virtual void models_savePrefetch() = 0;
    virtual void models_begin_prefetch1(bool val) = 0;

    // Occlusion culling
    //virtual BOOL occ_visible(vis_data& V) = 0;
    //virtual BOOL occ_visible(Fbox& B) = 0;
    //virtual BOOL occ_visible(sPoly& P) = 0;

    // Main
    virtual void Calculate() = 0;
    virtual void Render() = 0;
    virtual void AfterWorldRender() = 0; //После рендеринга мира (перед UI ПДА)
    virtual void AfterUIRender() = 0; //После рендеринга UI. Вызывать только если нам нужно отрендерить кадр для пда.

    virtual void Screenshot(ScreenshotMode mode = SM_NORMAL, LPCSTR name = nullptr) = 0;

    // Render mode
    virtual void rmNear(CBackend& cmd_list) = 0;
    virtual void rmFar(CBackend& cmd_list) = 0;
    virtual void rmNormal(CBackend& cmd_list) = 0;

    virtual u32 memory_usage() = 0;

    virtual u32 GetCacheStatPolys() = 0;

    virtual void Begin() = 0;
    virtual void Clear() = 0;
    virtual void End() = 0;
    virtual void ClearTarget() = 0;

    virtual void SetCacheXform(Fmatrix& mView, Fmatrix& mProject) = 0;
    virtual void SetCacheXformOld(Fmatrix& mView, Fmatrix& mProject) = 0;

    virtual CBackend& get_imm_command_list() = 0;

    virtual void OnCameraUpdated(bool from_actor) = 0;

    // Constructor/destructor
    virtual ~IRender_interface();

protected:
    virtual void ScreenshotImpl(ScreenshotMode mode, LPCSTR name) = 0;
};

class ITexture
{
public:
    virtual ~ITexture() = default;

    virtual const char* GetName() const = 0;
    virtual const char* GetLoadedName() const = 0;

    virtual void Load(const char* Name) = 0;
    virtual void Unload() = 0;

    virtual u32 get_Width() = 0;
    virtual u32 get_Height() = 0;
};

class IResourceManager
{
public:
    virtual ~IResourceManager() = default;

    virtual xr_vector<ITexture*> FindTexture(const char* Name) const = 0;
};

class ShExports final
{
    template <typename T, u32 Size>
    struct PositionsStorage
    {
        T Positions[Size]{};
        T& operator[](const u32& key)
        {
            ASSERT_FMT(key < std::size(Positions), "Out of range! key: [%u], size: [%u]", key, std::size(Positions));
            return Positions[key];
        }
    };

    PositionsStorage<Fvector2, 24> artefacts_position{};
    PositionsStorage<Fvector2, 24> anomalys_position{};

    Ivector2 detector_params{};
    Fvector pda_params{}, actor_params{}, actor_params2{}, laser_params{};
    Fvector4 dof_params{};

    // [zoom_rotate_factor, NULL, NULL, NULL] - Параметры худа оружия
    Fvector4 hud_params{};

    // [fFPCamYawMagnitudeSmooth, fFPCamPitchMagnitudeSmooth, fFPCamRollMagnitudeSmooth, NULL]
    Fvector4 cam_inertia_smooth{};

public:
    xr_map<shared_str, Fvector4> customExports{};

public:
    void set_artefact_position(const u32& _i, const Fvector2& _pos) { artefacts_position[_i] = _pos; };
    void set_anomaly_position(const u32& _i, const Fvector2& _pos) { anomalys_position[_i] = _pos; };

    const Fvector2& get_artefact_position(const u32& _i) { return artefacts_position[_i]; }
    const Fvector2& get_anomaly_position(const u32& _i) { return anomalys_position[_i]; }

    void set_custom_params(const char* key, const Fvector4& v) { customExports[key] = v; };
    const Fvector4& get_custom_params(const shared_str& key) { return customExports[key]; };

    void set_detector_params(const Ivector2& v) { detector_params = v; };
    void set_pda_params(const Fvector& v) { pda_params = v; };
    void set_actor_params(const Fvector& v) { actor_params = v; };
    void set_actor_params2(const Fvector& v) { actor_params2 = v; };
    void set_laser_params(const Fvector& v) { laser_params = v; };
    void set_dof_params(const float a, const float b, const float c, const float d) { dof_params = {a, b, c, d}; };

    const Ivector2& get_detector_params() const { return detector_params; }
    const Fvector& get_pda_params() const { return pda_params; }
    const Fvector& get_actor_params() const { return actor_params; }
    const Fvector& get_actor_params2() const { return actor_params2; }
    const Fvector& get_laser_params() const { return laser_params; }
    const Fvector4& get_dof_params() const { return dof_params; }

    const Fvector4& get_hud_params() const { return hud_params; }
    const Fvector4& get_cam_inertia_smooth() const { return cam_inertia_smooth; }

    void set_hud_params(const Fvector4& v) { hud_params = v; };
    void set_cam_inertia_smooth(const Fvector4& v) { cam_inertia_smooth = v; };
};

ENGINE_API extern ShExports shader_exports;

// Увеличивая или уменьшая максимальное кол-во здесь, обязательно нужно сделать тоже самое в вершинном шейдере в объявлении benders_pos. Там должно быть это значение умноженное на два.
constexpr size_t GRASS_SHADER_DATA_COUNT = 16;

struct GRASS_SHADER_DATA
{
    size_t index{};
    s8 anim[GRASS_SHADER_DATA_COUNT]{};
    u16 id[GRASS_SHADER_DATA_COUNT]{};

    Fvector4 pos[GRASS_SHADER_DATA_COUNT]{}; //x,y,z - pos, w - radius_curr
    Fvector4 dir[GRASS_SHADER_DATA_COUNT]{}; // x,y,z - dir, w - str

    float radius[GRASS_SHADER_DATA_COUNT]{};
    float str_target[GRASS_SHADER_DATA_COUNT]{};
    float time[GRASS_SHADER_DATA_COUNT]{};
    float fade[GRASS_SHADER_DATA_COUNT]{};
    float speed[GRASS_SHADER_DATA_COUNT]{};
};


struct GRASS_SHADER_DATA_OLD
{
    Fvector4 pos[GRASS_SHADER_DATA_COUNT]{}; // x,y,z - pos, w - radius_curr
    Fvector4 dir[GRASS_SHADER_DATA_COUNT]{}; // x,y,z - dir, w - str
};

ENGINE_API extern GRASS_SHADER_DATA grass_shader_data;
ENGINE_API extern GRASS_SHADER_DATA_OLD grass_shader_data_old;

extern Fvector4 ps_ssfx_grass_interactive;
extern Fvector4 ps_ssfx_int_grass_params_2;
extern Fvector4 ps_ssfx_hud_drops_1, ps_ssfx_hud_drops_2, ps_ssfx_hud_drops_1_cfg, ps_ssfx_hud_drops_2_cfg;
extern Fvector4 ps_ssfx_wetsurfaces_1, ps_ssfx_wetsurfaces_2, ps_ssfx_wetsurfaces_1_cfg, ps_ssfx_wetsurfaces_2_cfg;
extern Fvector4 ps_ssfx_lightsetup_1;
extern float ps_ssfx_gloss_factor;
extern Fvector3 ps_ssfx_gloss_minmax;
extern BOOL ps_ssfx_terrain_grass_align;
extern float ps_ssfx_terrain_grass_slope;

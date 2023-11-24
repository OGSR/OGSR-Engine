#include "stdafx.h"

#include "ResourceManager.h"
#include "blenders\Blender_Recorder.h"
#include "blenders\Blender.h"

#include "../../xr_3da/igame_persistent.h"
#include "../../xr_3da/environment.h"

#include "dxRenderDeviceRender.h"

// matrices
#define BIND_DECLARE(xf) \
    static class cl_xform_##xf final : public R_constant_setup \
    { \
        void setup(R_constant* C) override { RCache.xforms.set_c_##xf(C); } \
    } binder_##xf
BIND_DECLARE(w);
BIND_DECLARE(invw);
BIND_DECLARE(v);
BIND_DECLARE(p);
BIND_DECLARE(wv);
BIND_DECLARE(vp);
BIND_DECLARE(wvp);

#define DECLARE_TREE_BIND(c) \
    static class cl_tree_##c final : public R_constant_setup \
    { \
        void setup(R_constant* C) override { RCache.tree.set_c_##c(C); } \
    } tree_binder_##c
DECLARE_TREE_BIND(m_xform_v);
DECLARE_TREE_BIND(m_xform);
DECLARE_TREE_BIND(consts);
DECLARE_TREE_BIND(wave);
DECLARE_TREE_BIND(wind);
DECLARE_TREE_BIND(c_scale);
DECLARE_TREE_BIND(c_bias);
DECLARE_TREE_BIND(c_sun);

static class cl_hemi_cube_pos_faces final : public R_constant_setup
{
    void setup(R_constant* C) override { RCache.hemi.set_c_pos_faces(C); }
} binder_hemi_cube_pos_faces;

static class cl_hemi_cube_neg_faces final : public R_constant_setup
{
    void setup(R_constant* C) override { RCache.hemi.set_c_neg_faces(C); }
} binder_hemi_cube_neg_faces;

static class cl_material final : public R_constant_setup
{
    void setup(R_constant* C) override { RCache.hemi.set_c_material(C); }
} binder_material;

static class cl_texgen final : public R_constant_setup
{
    void setup(R_constant* C) override
    {
        Fmatrix mTexgen;

#if defined(USE_DX10) || defined(USE_DX11)
        Fmatrix mTexelAdjust = {0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.0f, 1.0f};
#else //	USE_DX10
        float _w = float(RDEVICE.dwWidth);
        float _h = float(RDEVICE.dwHeight);
        float o_w = (.5f / _w);
        float o_h = (.5f / _h);
        Fmatrix mTexelAdjust = {0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f + o_w, 0.5f + o_h, 0.0f, 1.0f};
#endif //	USE_DX10

        mTexgen.mul(mTexelAdjust, RCache.xforms.m_wvp);

        RCache.set_c(C, mTexgen);
    }
} binder_texgen;

static class cl_VPtexgen final : public R_constant_setup
{
    void setup(R_constant* C) override
    {
        Fmatrix mTexgen;

#if defined(USE_DX10) || defined(USE_DX11)
        Fmatrix mTexelAdjust = {0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.0f, 1.0f};
#else //	USE_DX10
        float _w = float(RDEVICE.dwWidth);
        float _h = float(RDEVICE.dwHeight);
        float o_w = (.5f / _w);
        float o_h = (.5f / _h);
        Fmatrix mTexelAdjust = {0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f + o_w, 0.5f + o_h, 0.0f, 1.0f};
#endif //	USE_DX10

        mTexgen.mul(mTexelAdjust, RCache.xforms.m_vp);

        RCache.set_c(C, mTexgen);
    }
} binder_VPtexgen;

// fog
static class cl_fog_plane final : public R_constant_setup
{
    Fvector4 result;
    void setup(R_constant* C) override
    {
        // Plane
        Fvector4 plane;
        Fmatrix& M = Device.mFullTransform;
        plane.x = -(M._14 + M._13);
        plane.y = -(M._24 + M._23);
        plane.z = -(M._34 + M._33);
        plane.w = -(M._44 + M._43);
        float denom = -1.0f / _sqrt(_sqr(plane.x) + _sqr(plane.y) + _sqr(plane.z));
        plane.mul(denom);

        // Near/Far
        float A = g_pGamePersistent->Environment().CurrentEnv->fog_near;
        float B = 1 / (g_pGamePersistent->Environment().CurrentEnv->fog_far - A);
        result.set(-plane.x * B, -plane.y * B, -plane.z * B, 1 - (plane.w - A) * B); // view-plane

        RCache.set_c(C, result);
    }
} binder_fog_plane;

// fog-params
static class cl_fog_params final : public R_constant_setup
{
    Fvector4 result;
    void setup(R_constant* C) override
    {
        // Near/Far
        float n = g_pGamePersistent->Environment().CurrentEnv->fog_near;
        float f = g_pGamePersistent->Environment().CurrentEnv->fog_far;
        float r = 1 / (f - n);
        result.set(-n * r, n, f, r);

        RCache.set_c(C, result);
    }
} binder_fog_params;

// fog-color
static class cl_fog_color final : public R_constant_setup
{
    Fvector4 result;
    void setup(R_constant* C) override
    {
        CEnvDescriptor& desc = *g_pGamePersistent->Environment().CurrentEnv;
        result.set(desc.fog_color.x, desc.fog_color.y, desc.fog_color.z, 0);

        RCache.set_c(C, result);
    }
} binder_fog_color;

// times
static class cl_times final : public R_constant_setup
{
    void setup(R_constant* C) override
    {
        float t = RDEVICE.fTimeGlobal;
        RCache.set_c(C, t, t * 10, t / 10, _sin(t));
    }
} binder_times;

// eye-params
static class cl_eye_P final : public R_constant_setup
{
    void setup(R_constant* C) override
    {
        Fvector& V = RDEVICE.vCameraPosition;
        RCache.set_c(C, V.x, V.y, V.z, 1);
    }
} binder_eye_P;

// eye-params
static class cl_eye_D final : public R_constant_setup
{
    void setup(R_constant* C) override
    {
        Fvector& V = RDEVICE.vCameraDirection;
        RCache.set_c(C, V.x, V.y, V.z, 0);
    }
} binder_eye_D;

// eye-params
static class cl_eye_N final : public R_constant_setup
{
    void setup(R_constant* C) override
    {
        Fvector& V = RDEVICE.vCameraTop;
        RCache.set_c(C, V.x, V.y, V.z, 0);
    }
} binder_eye_N;

// D-Light0
static class cl_sun0_color final : public R_constant_setup
{
    Fvector4 result;
    void setup(R_constant* C) override
    {
        CEnvDescriptor& desc = *g_pGamePersistent->Environment().CurrentEnv;
        result.set(desc.sun_color.x, desc.sun_color.y, desc.sun_color.z, 0);

        RCache.set_c(C, result);
    }
} binder_sun0_color;

static class cl_sun0_dir_w final : public R_constant_setup
{
    Fvector4 result;
    void setup(R_constant* C) override
    {
        CEnvDescriptor& desc = *g_pGamePersistent->Environment().CurrentEnv;
        result.set(desc.sun_dir.x, desc.sun_dir.y, desc.sun_dir.z, 0);

        RCache.set_c(C, result);
    }
} binder_sun0_dir_w;

static class cl_sun0_dir_e final : public R_constant_setup
{
    Fvector4 result;
    void setup(R_constant* C) override
    {
        Fvector D;
        CEnvDescriptor& desc = *g_pGamePersistent->Environment().CurrentEnv;
        Device.mView.transform_dir(D, desc.sun_dir);
        D.normalize();
        result.set(D.x, D.y, D.z, 0);

        RCache.set_c(C, result);
    }
} binder_sun0_dir_e;

static class cl_amb_color final : public R_constant_setup
{
    Fvector4 result;
    void setup(R_constant* C) override
    {
        CEnvDescriptorMixer& desc = *g_pGamePersistent->Environment().CurrentEnv;
        result.set(desc.ambient.x, desc.ambient.y, desc.ambient.z, desc.weight);

        RCache.set_c(C, result);
    }
} binder_amb_color;

static class cl_hemi_color final : public R_constant_setup
{
    Fvector4 result;
    void setup(R_constant* C) override
    {
        CEnvDescriptor& desc = *g_pGamePersistent->Environment().CurrentEnv;
        result.set(desc.hemi_color.x, desc.hemi_color.y, desc.hemi_color.z, desc.hemi_color.w);

        RCache.set_c(C, result);
    }
} binder_hemi_color;

static class cl_screen_res final : public R_constant_setup
{
    void setup(R_constant* C) override { RCache.set_c(C, (float)RDEVICE.dwWidth, (float)RDEVICE.dwHeight, 1.0f / (float)RDEVICE.dwWidth, 1.0f / (float)RDEVICE.dwHeight); }
} binder_screen_res;

static class cl_screen_params final : public R_constant_setup
{
    Fvector4 result;
    void setup(R_constant* C) override
    {
        result.set(Device.fFOV, Device.fASPECT, tan(deg2rad(Device.fFOV) / 2), g_pGamePersistent->Environment().CurrentEnv->far_plane * 0.75f);

        RCache.set_c(C, result);
    }
} binder_screen_params;

static class cl_rain_params final : public R_constant_setup
{
    void setup(R_constant* C) override { RCache.set_c(C, g_pGamePersistent->Environment().CurrentEnv->rain_density, g_pGamePersistent->Environment().wetness_factor, 0.0f, 0.0f); }
} binder_rain_params;

static class cl_artifacts final : public R_constant_setup
{
    Fmatrix result{};

    void setup(R_constant* C) override
    {
        result._11 = shader_exports.get_artefact_position(start_val).x;
        result._12 = shader_exports.get_artefact_position(start_val).y;
        result._13 = shader_exports.get_artefact_position(start_val + 1).x;
        result._14 = shader_exports.get_artefact_position(start_val + 1).y;
        result._21 = shader_exports.get_artefact_position(start_val + 2).x;
        result._22 = shader_exports.get_artefact_position(start_val + 2).y;
        result._23 = shader_exports.get_artefact_position(start_val + 3).x;
        result._24 = shader_exports.get_artefact_position(start_val + 3).y;
        result._31 = shader_exports.get_artefact_position(start_val + 4).x;
        result._32 = shader_exports.get_artefact_position(start_val + 4).y;
        result._33 = shader_exports.get_artefact_position(start_val + 5).x;
        result._34 = shader_exports.get_artefact_position(start_val + 5).y;
        result._41 = shader_exports.get_artefact_position(start_val + 6).x;
        result._42 = shader_exports.get_artefact_position(start_val + 6).y;
        result._43 = shader_exports.get_artefact_position(start_val + 7).x;
        result._44 = shader_exports.get_artefact_position(start_val + 7).y;

        RCache.set_c(C, result);
    }

    u32 start_val;

public:
    cl_artifacts() = delete;
    cl_artifacts(u32 v) : start_val(v) {}
} binder_artifacts{0}, binder_artifacts2{8}, binder_artifacts3{16};

static class cl_anomalys final : public R_constant_setup
{
    Fmatrix result{};

    void setup(R_constant* C) override
    {
        result._11 = shader_exports.get_anomaly_position(start_val).x;
        result._12 = shader_exports.get_anomaly_position(start_val).y;
        result._13 = shader_exports.get_anomaly_position(start_val + 1).x;
        result._14 = shader_exports.get_anomaly_position(start_val + 1).y;
        result._21 = shader_exports.get_anomaly_position(start_val + 2).x;
        result._22 = shader_exports.get_anomaly_position(start_val + 2).y;
        result._23 = shader_exports.get_anomaly_position(start_val + 3).x;
        result._24 = shader_exports.get_anomaly_position(start_val + 3).y;
        result._31 = shader_exports.get_anomaly_position(start_val + 4).x;
        result._32 = shader_exports.get_anomaly_position(start_val + 4).y;
        result._33 = shader_exports.get_anomaly_position(start_val + 5).x;
        result._34 = shader_exports.get_anomaly_position(start_val + 5).y;
        result._41 = shader_exports.get_anomaly_position(start_val + 6).x;
        result._42 = shader_exports.get_anomaly_position(start_val + 6).y;
        result._43 = shader_exports.get_anomaly_position(start_val + 7).x;
        result._44 = shader_exports.get_anomaly_position(start_val + 7).y;

        RCache.set_c(C, result);
    }

    u32 start_val;

public:
    cl_anomalys() = delete;
    cl_anomalys(u32 v) : start_val(v) {}
} binder_anomalys{0}, binder_anomalys2{8}, binder_anomalys3{16};

static class cl_detector final : public R_constant_setup
{
    Fvector4 result;
    void setup(R_constant* C) override
    {
        result.set((float)(shader_exports.get_detector_params().x), (float)(shader_exports.get_detector_params().y), 0.f, 0.f);

        RCache.set_c(C, result);
    }
} binder_detector;

static class cl_hud_params final : public R_constant_setup //--#SM+#--
{
    void setup(R_constant* C) override { RCache.set_c(C, g_pGamePersistent->m_pGShaderConstants.hud_params); }
} binder_hud_params;

static class cl_script_params final : public R_constant_setup //--#SM+#--
{
    void setup(R_constant* C) override { RCache.set_c(C, g_pGamePersistent->m_pGShaderConstants.m_script_params); }
} binder_script_params;

static class cl_blend_mode final : public R_constant_setup //--#SM+#--
{
    void setup(R_constant* C) override { RCache.set_c(C, g_pGamePersistent->m_pGShaderConstants.m_blender_mode); }
} binder_blend_mode;

static class cl_ogsr_game_time final : public R_constant_setup
{
    void setup(R_constant* C) override
    {
        u32 hours{0}, mins{0}, secs{0}, milisecs{0};
        if (g_pGameLevel)
            g_pGameLevel->GetGameTimeForShaders(hours, mins, secs, milisecs);
        RCache.set_c(C, float(hours), float(mins), float(secs), float(milisecs));
    }
} binder_ogsr_game_time;

static class cl_addon_VControl final : public R_constant_setup
{
    void setup(R_constant* C) override
    {
        if (ps_r2_ls_flags_ext.test(R2FLAG_VISOR_REFL) && ps_r2_ls_flags_ext.test(R2FLAG_VISOR_REFL_CONTROL))
            RCache.set_c(C, ps_r2_visor_refl_intensity, ps_r2_visor_refl_radius, 0.f, 1.f);
        else
            RCache.set_c(C, 0.f, 0.f, 0.f, 0.f);
    }
} binder_addon_VControl;

static class cl_pda_params final : public R_constant_setup
{
    void setup(R_constant* C) override
    {
        const auto& P = shader_exports.get_pda_params();
        RCache.set_c(C, P.x, P.y, 0.f, P.z);
    }
} binder_pda_params;

static class cl_actor_params final : public R_constant_setup
{
    void setup(R_constant* C) override
    {
        const auto& P = shader_exports.get_actor_params();
        RCache.set_c(C, P.x, P.y, P.z, g_pGamePersistent->Environment().USED_COP_WEATHER ? 1.0f : 0.0f);
    }
} binder_actor_params;

static class cl_sky_color final : public R_constant_setup
{
    Fvector4 result{};
    void setup(R_constant* C) override
    {
        auto* desc = g_pGamePersistent->Environment().CurrentEnv;
        result.set(desc->sky_color.x, desc->sky_color.y, desc->sky_color.z, desc->sky_rotation);
        RCache.set_c(C, result);
    }
} binder_sky_color;

static class cl_inv_v final : public R_constant_setup
{
    Fmatrix result;
    void setup(R_constant* C) override
    {
        result.invert(Device.mView);
        RCache.set_c(C, result);
    }
} binder_inv_v;

// Screen Space Shaders Stuff

static class ssfx_wpn_dof_1 final : public R_constant_setup
{
    void setup(R_constant* C) override
    {
        const auto& P = shader_exports.get_dof_params();
        RCache.set_c(C, P.x, P.y, P.z, P.w);
    }
} ssfx_wpn_dof_1;

static class ssfx_wpn_dof_2 final : public R_constant_setup
{
    void setup(R_constant* C) override
    {
        RCache.set_c(C, ps_ssfx_wpn_dof_2, 0, 0, 0);    
    }
} ssfx_wpn_dof_2;

static class ssfx_blood_decals final : public R_constant_setup
{
    void setup(R_constant* C) override { RCache.set_c(C, ps_ssfx_blood_decals); }
} ssfx_blood_decals;

static class ssfx_hud_drops_1 final : public R_constant_setup
{
    void setup(R_constant* C) override { RCache.set_c(C, ps_ssfx_hud_drops_1); }
} ssfx_hud_drops_1;

static class ssfx_hud_drops_2 final : public R_constant_setup
{
    void setup(R_constant* C) override { RCache.set_c(C, ps_ssfx_hud_drops_2); }
} ssfx_hud_drops_2;

static class ssfx_lightsetup_1 final : public R_constant_setup
{
    void setup(R_constant* C) override { RCache.set_c(C, ps_ssfx_lightsetup_1); }
} ssfx_lightsetup_1;

static class ssfx_is_underground final : public R_constant_setup
{
    void setup(R_constant* C) override { RCache.set_c(C, ps_ssfx_is_underground ? 1.f : 0.f, 0.f, 0.f, 0.f); }
} ssfx_is_underground;

static class ssfx_wetsurfaces_1 final : public R_constant_setup
{
    void setup(R_constant* C) override { RCache.set_c(C, ps_ssfx_wetsurfaces_1); }
} ssfx_wetsurfaces_1;

static class ssfx_wetsurfaces_2 final : public R_constant_setup
{
    void setup(R_constant* C) override { RCache.set_c(C, ps_ssfx_wetsurfaces_2); }
} ssfx_wetsurfaces_2;

static class ssfx_gloss final : public R_constant_setup
{
    void setup(R_constant* C) override { RCache.set_c(C, ps_ssfx_gloss_minmax.x, ps_ssfx_gloss_minmax.y, ps_ssfx_gloss_factor, 0.f); }
} ssfx_gloss;

static class ssfx_florafixes_1 final : public R_constant_setup
{
    void setup(R_constant* C) override { RCache.set_c(C, ps_ssfx_florafixes_1); }
} ssfx_florafixes_1;

static class ssfx_florafixes_2 final : public R_constant_setup
{
    void setup(R_constant* C) override { RCache.set_c(C, ps_ssfx_florafixes_2); }
} ssfx_florafixes_2;

static class pp_image_corrections final : public R_constant_setup
{
    void setup(R_constant* C) override { RCache.set_c(C, ps_r2_img_exposure, ps_r2_img_gamma, ps_r2_img_saturation, 1); }
} pp_image_corrections;

static class pp_color_grading final : public R_constant_setup
{
    void setup(R_constant* C) override { RCache.set_c(C, ps_r2_img_cg.x, ps_r2_img_cg.y, ps_r2_img_cg.z, 1); }
} pp_color_grading;

static class cl_wind_params final : public R_constant_setup
{
    void setup(R_constant* C) override
    {
        CEnvDescriptor& E = *g_pGamePersistent->Environment().CurrentEnv;
        RCache.set_c(C, E.wind_direction, E.wind_velocity, E.m_fTreeAmplitudeIntensity, 0.0f);
    }
} binder_wind_params;

static class ssfx_wind final : public R_constant_setup
{
    void setup(R_constant* C) override { RCache.set_c(C, ps_ssfx_wind); }
} ssfx_wind;

static class ssfx_wind_gust final : public R_constant_setup
{
    void setup(R_constant* C) override { RCache.set_c(C, ps_ssfx_wind_gust); }
} ssfx_wind_gust;

static class ssfx_wind_anim final : public R_constant_setup
{
    void setup(R_constant* C) override
    {
        const Fvector3& WindAni = g_pGamePersistent->Environment().wind_anim;
        RCache.set_c(C, WindAni.x, WindAni.y, WindAni.z, 0.f);
    }
} ssfx_wind_anim;


// Standart constant-binding
void CBlender_Compile::SetMapping()
{
    // matrices
    r_Constant("m_W", &binder_w);
    r_Constant("m_invW", &binder_invw);
    r_Constant("m_V", &binder_v);
    r_Constant("m_inv_V", &binder_inv_v);
    r_Constant("m_P", &binder_p);
    r_Constant("m_WV", &binder_wv);
    r_Constant("m_VP", &binder_vp);
    r_Constant("m_WVP", &binder_wvp);

    r_Constant("m_xform_v", &tree_binder_m_xform_v);
    r_Constant("m_xform", &tree_binder_m_xform);
    r_Constant("consts", &tree_binder_consts);
    r_Constant("wave", &tree_binder_wave);
    r_Constant("wind", &tree_binder_wind);
    r_Constant("c_scale", &tree_binder_c_scale);
    r_Constant("c_bias", &tree_binder_c_bias);
    r_Constant("c_sun", &tree_binder_c_sun);

    // hemi cube
    r_Constant("L_material", &binder_material);
    r_Constant("hemi_cube_pos_faces", &binder_hemi_cube_pos_faces);
    r_Constant("hemi_cube_neg_faces", &binder_hemi_cube_neg_faces);

    //	Igor	temp solution for the texgen functionality in the shader
    r_Constant("m_texgen", &binder_texgen);
    r_Constant("mVPTexgen", &binder_VPtexgen);

    // fog-params
    r_Constant("fog_plane", &binder_fog_plane);
    r_Constant("fog_params", &binder_fog_params);
    r_Constant("fog_color", &binder_fog_color);

    // Rain
    r_Constant("rain_params", &binder_rain_params);

    // time
    r_Constant("timers", &binder_times);

    // eye-params
    r_Constant("eye_position", &binder_eye_P);
    r_Constant("eye_direction", &binder_eye_D);
    r_Constant("eye_normal", &binder_eye_N);

    // global-lighting (env params)
    r_Constant("L_sun_color", &binder_sun0_color);
    r_Constant("L_sun_dir_w", &binder_sun0_dir_w);
    r_Constant("L_sun_dir_e", &binder_sun0_dir_e);
    //	r_Constant				("L_lmap_color",	&binder_lm_color);
    r_Constant("L_hemi_color", &binder_hemi_color);
    r_Constant("L_ambient", &binder_amb_color);

    r_Constant("screen_res", &binder_screen_res);
    r_Constant("ogse_c_screen", &binder_screen_params);

    r_Constant("ogse_c_artefacts", &binder_artifacts);
    r_Constant("ogse_c_artefacts2", &binder_artifacts2);
    r_Constant("ogse_c_artefacts3", &binder_artifacts3);
    r_Constant("ogse_c_anomalys", &binder_anomalys);
    r_Constant("ogse_c_anomalys2", &binder_anomalys2);
    r_Constant("ogse_c_anomalys3", &binder_anomalys3);
    r_Constant("ogse_c_detector", &binder_detector);

    // detail
    // if (bDetail	&& detail_scaler)
    //	Igor: bDetail can be overridden by no_detail_texture option.
    //	But shader can be deatiled implicitly, so try to set this parameter
    //	anyway.
    if (detail_scaler)
        r_Constant("dt_params", detail_scaler);

    // misc
    r_Constant("m_hud_params", &binder_hud_params); //--#SM+#--
    r_Constant("m_script_params", &binder_script_params); //--#SM+#--
    r_Constant("m_blender_mode", &binder_blend_mode); //--#SM+#--

    r_Constant("ogsr_game_time", &binder_ogsr_game_time);

    r_Constant("addon_VControl", &binder_addon_VControl);

    r_Constant("m_affects", &binder_pda_params);

    r_Constant("m_actor_params", &binder_actor_params);

    r_Constant("sky_color", &binder_sky_color);

    r_Constant("ssfx_wpn_dof_1", &ssfx_wpn_dof_1);
    r_Constant("ssfx_wpn_dof_2", &ssfx_wpn_dof_2);
    r_Constant("ssfx_blood_decals", &ssfx_blood_decals);
    r_Constant("ssfx_hud_drops_1", &ssfx_hud_drops_1);
    r_Constant("ssfx_hud_drops_2", &ssfx_hud_drops_2);
    r_Constant("ssfx_lightsetup_1", &ssfx_lightsetup_1);
    r_Constant("ssfx_is_underground", &ssfx_is_underground);
    r_Constant("ssfx_wetsurfaces_1", &ssfx_wetsurfaces_1);
    r_Constant("ssfx_wetsurfaces_2", &ssfx_wetsurfaces_2);
    r_Constant("ssfx_gloss", &ssfx_gloss);
    r_Constant("ssfx_florafixes_1", &ssfx_florafixes_1);
    r_Constant("ssfx_florafixes_2", &ssfx_florafixes_2);
    r_Constant("wind_params", &binder_wind_params);
    r_Constant("ssfx_wind_anim", &ssfx_wind_anim);
    r_Constant("ssfx_wind", &ssfx_wind);
    r_Constant("ssfx_wind_gust", &ssfx_wind_gust);

    r_Constant("pp_img_corrections", &pp_image_corrections);
    r_Constant("pp_img_cg", &pp_color_grading);

    // other common
    for (const auto& [name, s] : DEV->v_constant_setup)
        r_Constant(name.c_str(), s);
}

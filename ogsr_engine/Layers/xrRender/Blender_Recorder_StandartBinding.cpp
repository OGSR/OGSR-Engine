#include "stdafx.h"

#include "ResourceManager.h"
#include "blenders/Blender_Recorder.h"
#include "blenders/Blender.h"

#include "../../xr_3da/igame_persistent.h"
#include "../../xr_3da/environment.h"

#include "dxRenderDeviceRender.h"

// -- capture R_constant !!!
#define BIND_DECLARE(xf) \
    static class cl_xform_##xf final : public R_constant_setup_cap \
    { \
        void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.xforms.set_c_##xf(C); } \
    } binder_##xf
BIND_DECLARE(w);
BIND_DECLARE(invw);
BIND_DECLARE(v);
BIND_DECLARE(p);
BIND_DECLARE(wv);
BIND_DECLARE(vp);
BIND_DECLARE(wvp);

BIND_DECLARE(w_old);
BIND_DECLARE(v_old);
BIND_DECLARE(p_old);
BIND_DECLARE(wv_old);
BIND_DECLARE(vp_old);
BIND_DECLARE(wvp_old);

static class cl_hemi_cube_pos_faces final : public R_constant_setup_cap
{
    void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.hemi.set_c_pos_faces(C); }
} binder_hemi_cube_pos_faces;

static class cl_hemi_cube_neg_faces final : public R_constant_setup_cap
{
    void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.hemi.set_c_neg_faces(C); }
} binder_hemi_cube_neg_faces;

static class cl_material final : public R_constant_setup_cap
{
    void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.hemi.set_c_material(C); }
} binder_material;

static class cl_heatvision_hotness final : public R_constant_setup_cap
{
    virtual void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.hemi.set_c_hotness(C); }
} binder_heatvision_hotness;

static class cl_dt_scaler final : public R_constant_setup_cap
{
    virtual void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.hemi.set_c_scale(C); }
} binder_scale;

static class cl_lod : public R_constant_setup_cap
{
    virtual void setup(CBackend& cmd_list, R_constant* C) { cmd_list.lod.set_lod(C); }
} binder_lod;

static class cl_alpha_ref : public R_constant_setup_cap
{
    virtual void setup(CBackend& cmd_list, R_constant* C) { cmd_list.StateManager.BindAlphaRef(C); }
} binder_alpha_ref;

// -- set R_constant !!!

static class cl_texgen final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        Fmatrix mTexgen;
        const Fmatrix mTexelAdjust = {0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.0f, 1.0f};

        mTexgen.mul(mTexelAdjust, cmd_list.xforms.m_wvp);

        cmd_list.set_c(C, mTexgen);
    }
} binder_texgen;

static class cl_VPtexgen final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        Fmatrix mTexgen;
        const Fmatrix mTexelAdjust = {0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.0f, 1.0f};

        mTexgen.mul(mTexelAdjust, cmd_list.xforms.m_vp);

        cmd_list.set_c(C, mTexgen);
    }
} binder_VPtexgen;

// fog
static class cl_fog_plane final : public R_constant_setup
{
    Fvector4 result;

    void setup(CBackend& cmd_list, R_constant* C) override
    {
        // Plane
        Fvector4 plane;
        const Fmatrix& M = Device.mFullTransform;
        plane.x = -(M._14 + M._13);
        plane.y = -(M._24 + M._23);
        plane.z = -(M._34 + M._33);
        plane.w = -(M._44 + M._43);
        const float denom = -1.0f / _sqrt(_sqr(plane.x) + _sqr(plane.y) + _sqr(plane.z));
        plane.mul(denom);

        // Near/Far
        const float A = g_pGamePersistent->Environment().CurrentEnv->fog_near;
        const float B = 1 / (g_pGamePersistent->Environment().CurrentEnv->fog_far - A);
        result.set(-plane.x * B, -plane.y * B, -plane.z * B, 1 - (plane.w - A) * B); // view-plane

        cmd_list.set_c(C, result);
    }
} binder_fog_plane;

// fog-params
static class cl_fog_params final : public R_constant_setup
{
    Fvector4 result;

    void setup(CBackend& cmd_list, R_constant* C) override
    {
        // Near/Far
        const float n = g_pGamePersistent->Environment().CurrentEnv->fog_near;
        const float f = g_pGamePersistent->Environment().CurrentEnv->fog_far;
        const float r = 1 / (f - n);
        result.set(-n * r, n, f, r);

        cmd_list.set_c(C, result);
    }
} binder_fog_params;

// fog-color
static class cl_fog_color final : public R_constant_setup
{
    Fvector4 result{};

    void setup(CBackend& cmd_list, R_constant* C) override
    {
        const CEnvDescriptor& desc = *g_pGamePersistent->Environment().CurrentEnv;
        result.set(desc.fog_color.x, desc.fog_color.y, desc.fog_color.z, 0);

        cmd_list.set_c(C, result);
    }
} binder_fog_color;

static class cl_wind_params : public R_constant_setup
{
    u32 marker{};
    Fvector4 result{};

    virtual void setup(CBackend& cmd_list, R_constant* C)
    {
        if (marker != Device.dwFrame)
        {
            const CEnvDescriptor& E = *g_pGamePersistent->Environment().CurrentEnv;
            result.set(E.wind_direction, E.wind_velocity, E.m_fTreeAmplitudeIntensity, 0.0f);

            marker = Device.dwFrame;
        }
        cmd_list.set_c(C, result);
    }
} binder_wind_params;

static class cl_wind_params_old : public R_constant_setup
{
    u32 marker{};
    Fvector4 result{};

    virtual void setup(CBackend& cmd_list, R_constant* C)
    {
        if (marker != Device.dwFrame)
        {
            const CEnvDescriptor& E = *g_pGamePersistent->Environment().CurrentEnv;
            result.set(E.wind_direction_old, E.wind_velocity_old, E.m_fTreeAmplitudeIntensity_old, 0.0f);

            marker = Device.dwFrame;
        }
        cmd_list.set_c(C, result);
    }
} binder_wind_params_old;

// times
static class cl_times final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        const float t = Device.fTimeGlobal;
        cmd_list.set_c(C, t, t * 10, t / 10, _sin(t));
    }
} binder_times;

// eye-params
static class cl_eye_P final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        const Fvector& V = Device.vCameraPosition;
        cmd_list.set_c(C, V.x, V.y, V.z, 1);
    }
} binder_eye_P;

// interpolated eye position (crookr scope parallax)
static class cl_eye_PL final : public R_constant_setup
{
    Fvector tV{};
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        const Fvector& V = Device.vCameraPosition;
        if (!tV.similar(V, 1.5f))
            tV = V;
        tV = tV.lerp(tV, V, scope_fake_interp);
        cmd_list.set_c(C, tV.x, tV.y, tV.z, 1);
    }
} binder_eye_PL;

// eye-params
static class cl_eye_D final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        const Fvector& V = Device.vCameraDirection;
        cmd_list.set_c(C, V.x, V.y, V.z, 0);
    }
} binder_eye_D;

// interpolated eye direction (crookr scope parallax)
static class cl_eye_DL final : public R_constant_setup
{
    Fvector tV{};

    void setup(CBackend& cmd_list, R_constant* C) override
    {
        const Fvector& V = Device.vCameraDirection;
        if (!tV.similar(V, 0.5f))
            tV = V;
        tV = tV.lerp(tV, V, scope_fake_interp);
        cmd_list.set_c(C, tV.x, tV.y, tV.z, 0);
    }
} binder_eye_DL;

// eye-params
static class cl_eye_N final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        const Fvector& V = Device.vCameraTop;
        cmd_list.set_c(C, V.x, V.y, V.z, 0);
    }
} binder_eye_N;

// fake scope params (crookr)
static class cl_fakescope final : public R_constant_setup
{
    virtual void setup(CBackend& cmd_list, R_constant* C) { cmd_list.set_c(C, scope_fake_power, scope_fake_radius, 0.f, 0.f); }
} binder_fakescope;
static class cl_fakescope_params1 final : public R_constant_setup
{
    virtual void setup(CBackend& cmd_list, R_constant* C) { cmd_list.set_c(C, ps_scope1_params.x, ps_scope1_params.y, ps_scope1_params.z, ps_scope1_params.w); }
} binder_fakescope_params1;
static class cl_fakescope_params2 final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.set_c(C, ps_scope2_params.x, ps_scope2_params.y, ps_scope2_params.z, ps_scope2_params.w); }
} binder_fakescope_params2;
static class cl_fakescope_params3 final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.set_c(C, ps_scope3_params.x, ps_scope3_params.y, ps_scope3_params.z, ps_scope3_params.w); }
} binder_fakescope_params3;

// D-Light0
static class cl_sun0_color final : public R_constant_setup
{
    Fvector4 result;
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        const CEnvDescriptor& desc = *g_pGamePersistent->Environment().CurrentEnv;
        result.set(desc.sun_color.x, desc.sun_color.y, desc.sun_color.z, 0);

        cmd_list.set_c(C, result);
    }
} binder_sun0_color;

static class cl_sun0_dir_w final : public R_constant_setup
{
    Fvector4 result;
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        const CEnvDescriptor& desc = *g_pGamePersistent->Environment().CurrentEnv;
        result.set(desc.sun_dir.x, desc.sun_dir.y, desc.sun_dir.z, 0);

        cmd_list.set_c(C, result);
    }
} binder_sun0_dir_w;

static class cl_sun0_dir_e final : public R_constant_setup
{
    Fvector4 result;
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        Fvector D;
        const CEnvDescriptor& desc = *g_pGamePersistent->Environment().CurrentEnv;
        Device.mView.transform_dir(D, desc.sun_dir);
        D.normalize();
        result.set(D.x, D.y, D.z, 0);

        cmd_list.set_c(C, result);
    }
} binder_sun0_dir_e;

static class cl_amb_color final : public R_constant_setup
{
    Fvector4 result;
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        const CEnvDescriptorMixer& desc = *g_pGamePersistent->Environment().CurrentEnv;
        result.set(desc.ambient.x, desc.ambient.y, desc.ambient.z, desc.weight);

        cmd_list.set_c(C, result);
    }
} binder_amb_color;

static class cl_hemi_color final : public R_constant_setup
{
    Fvector4 result;
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        const CEnvDescriptor& desc = *g_pGamePersistent->Environment().CurrentEnv;
        result.set(desc.hemi_color.x, desc.hemi_color.y, desc.hemi_color.z, desc.hemi_color.w);

        cmd_list.set_c(C, result);
    }
} binder_hemi_color;

static class cl_screen_res final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        cmd_list.set_c(C, (float)Device.dwWidth, (float)Device.dwHeight, 1.0f / (float)Device.dwWidth, 1.0f / (float)Device.dwHeight);
    }
} binder_screen_res;

static class cl_screen_params final : public R_constant_setup
{
    Fvector4 result;
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        result.set(Device.fFOV, Device.fASPECT, tan(deg2rad(Device.fFOV) / 2), g_pGamePersistent->Environment().CurrentEnv->far_plane * 0.75f);

        cmd_list.set_c(C, result);
    }
} binder_screen_params;

static class cl_rain_params final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        cmd_list.set_c(C, g_pGamePersistent->Environment().CurrentEnv->rain_density, g_pGamePersistent->Environment().wetness_factor, 0.0f, 0.0f);
    }
} binder_rain_params;

static class cl_cam_inertia_smooth : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.set_c(C, shader_exports.get_cam_inertia_smooth()); }
} binder_cam_inertia_smooth;

static class cl_hud_params final : public R_constant_setup //--#SM+#--
{
    void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.set_c(C, shader_exports.get_hud_params()); }
} binder_hud_params;

static class cl_ogsr_game_time final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        u32 hours{0}, mins{0}, secs{0}, milisecs{0};
        if (g_pGameLevel)
            g_pGameLevel->GetGameTimeForShaders(hours, mins, secs, milisecs);
        cmd_list.set_c(C, float(hours), float(mins), float(secs), float(milisecs));
    }
} binder_ogsr_game_time;

static class cl_pda_params final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        const auto& P = shader_exports.get_pda_params();
        cmd_list.set_c(C, P.x, P.y, 0.f, P.z);
    }
} binder_pda_params;

static class cl_actor_params final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        const auto& P = shader_exports.get_actor_params();
        cmd_list.set_c(C, P.x, P.y, P.z, g_pGamePersistent->Environment().USED_COP_WEATHER ? 1.0f : 0.0f);
    }
} binder_actor_params;

static class cl_actor_params2 final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        const auto& P = shader_exports.get_actor_params2();
        cmd_list.set_c(C, P.x, P.y, P.z, static_cast<float>(ps_r2_ls_flags_ext.test(R2FLAGEXT_LENS_FLARE)));
    }
} binder_actor_params2;

static class cl_flare_params final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        extern float ps_lens_flare_sun_blend;
        cmd_list.set_c(C, 1.f, ps_lens_flare_sun_blend, 0.f, 0.f);
    }
} binder_flare_params;

static class cl_laser_params final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        const auto& P = shader_exports.get_laser_params();
        cmd_list.set_c(C, P.x, P.y, 0.f, P.z);
    }
} binder_laser_params;

static class cl_sky_color final : public R_constant_setup
{
    Fvector4 result{};
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        const auto* desc = g_pGamePersistent->Environment().CurrentEnv;
        result.set(desc->sky_color.x, desc->sky_color.y, desc->sky_color.z, desc->sky_rotation);
        cmd_list.set_c(C, result);
    }
} binder_sky_color;

static class cl_inv_v final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        cmd_list.set_c(C, Device.mInvView);
    }
} binder_inv_v;

static class ssfx_wpn_dof_1 final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        const auto& P = shader_exports.get_dof_params();
        cmd_list.set_c(C, P.x, P.y, P.z, P.w);
    }
} ssfx_wpn_dof_1;

static class ssfx_wpn_dof_2 final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.set_c(C, ps_ssfx_wpn_dof_2, 0, 0, 0); }
} ssfx_wpn_dof_2;

extern float ps_pnv_noise;
extern float ps_pnv_scanlines;
extern float ps_pnv_scintillation;
extern float ps_pnv_position;
extern float ps_pnv_radius;

extern float ps_pnv_params_1;
extern float ps_pnv_params_2;
extern float ps_pnv_params_3;
extern float ps_pnv_params_4;

extern float ps_pnv_params_1_2;
extern float ps_pnv_params_2_2;
extern float ps_pnv_params_3_2;
extern float ps_pnv_params_4_2;

static class pnv_param_1 final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.set_c(C, ps_pnv_position, ps_pnv_radius, static_cast<float>(ps_pnv_mode), 0.f); }
} pnv_param_1;

static class pnv_param_2 final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.set_c(C, ps_pnv_params_1, ps_pnv_params_2, ps_pnv_params_3, ps_pnv_params_4); }
} pnv_param_2;

static class pnv_param_3 final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.set_c(C, ps_pnv_noise, ps_pnv_scanlines, ps_pnv_scintillation, 0.f); }
} pnv_param_3;

static class pnv_param_4 final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.set_c(C, ps_pnv_params_1_2, ps_pnv_params_2_2, ps_pnv_params_3_2, ps_pnv_params_4_2); }
} pnv_param_4;

static class ssfx_blood_decals final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.set_c(C, ps_ssfx_blood_decals); }
} ssfx_blood_decals;

static class ssfx_hud_drops_1 final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.set_c(C, ps_ssfx_hud_drops_1); }
} ssfx_hud_drops_1;

static class ssfx_hud_drops_2 final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.set_c(C, ps_ssfx_hud_drops_2); }
} ssfx_hud_drops_2;

static class ssfx_lightsetup_1 final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.set_c(C, ps_ssfx_lightsetup_1); }
} ssfx_lightsetup_1;

static class ssfx_is_underground final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.set_c(C, ps_ssfx_is_underground ? 1.f : 0.f, 0.f, 0.f, 0.f); }
} ssfx_is_underground;

static class ssfx_wetsurfaces_1 final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.set_c(C, ps_ssfx_wetsurfaces_1); }
} ssfx_wetsurfaces_1;

static class ssfx_wetsurfaces_2 final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.set_c(C, ps_ssfx_wetsurfaces_2); }
} ssfx_wetsurfaces_2;

static class pp_image_corrections : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) { cmd_list.set_c(C, ps_r2_img_exposure, ps_r2_img_gamma, ps_r2_img_saturation, 1); }
} pp_image_corrections;

static class pp_color_grading final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.set_c(C, ps_r2_img_cg.x, ps_r2_img_cg.y, ps_r2_img_cg.z, 1); }
} pp_color_grading;

static class ssfx_gloss final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.set_c(C, ps_ssfx_gloss_minmax.x, ps_ssfx_gloss_minmax.y, ps_ssfx_gloss_factor, 0.f); }
} ssfx_gloss;

static class ssfx_florafixes_1 final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.set_c(C, ps_ssfx_florafixes_1); }
} ssfx_florafixes_1;

static class ssfx_florafixes_2 final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.set_c(C, ps_ssfx_florafixes_2); }
} ssfx_florafixes_2;

static class ssfx_wind_grass final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.set_c(C, ps_ssfx_wind_grass); }
} ssfx_wind_grass;

static class ssfx_wind_trees final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.set_c(C, ps_ssfx_wind_trees); }
} ssfx_wind_trees;

static class ssfx_wind_anim : public R_constant_setup
{
    virtual void setup(CBackend& cmd_list, R_constant* C)
    {
        Fvector3 WindAni = g_pGamePersistent->Environment().wind_anim;
        cmd_list.set_c(C, WindAni.x, WindAni.y, WindAni.z, 0);
    }
} ssfx_wind_anim;

static class ssfx_wind_anim_old : public R_constant_setup
{
    virtual void setup(CBackend& cmd_list, R_constant* C)
    {
        Fvector3 WindAni = g_pGamePersistent->Environment().wind_anim_old;
        cmd_list.set_c(C, WindAni.x, WindAni.y, WindAni.z, 0);
    }
} ssfx_wind_anim_old;

static class ssfx_lut : public R_constant_setup
{
    virtual void setup(CBackend& cmd_list, R_constant* C) { cmd_list.set_c(C, ps_ssfx_lut); }
} ssfx_lut;

static class ssfx_shadow_bias : public R_constant_setup
{
    virtual void setup(CBackend& cmd_list, R_constant* C) { cmd_list.set_c(C, ps_ssfx_shadow_bias.x, ps_ssfx_shadow_bias.y, 0, 0); }
} ssfx_shadow_bias;

static class ssfx_bloom_1 : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        Fvector4 BloomSetup{};
        if (ps_ssfx_bloom_use_presets)
        {
            BloomSetup.x = g_pGamePersistent->Environment().CurrentEnv->bloom_threshold;
            BloomSetup.y = g_pGamePersistent->Environment().CurrentEnv->bloom_exposure;
            BloomSetup.w = g_pGamePersistent->Environment().CurrentEnv->bloom_sky_intensity;
        }
        else
        {
            BloomSetup.x = ps_ssfx_bloom_1.x;
            BloomSetup.y = ps_ssfx_bloom_1.y;
            BloomSetup.w = ps_ssfx_bloom_1.w;
        }
        cmd_list.set_c(C, BloomSetup);
    }
} ssfx_bloom_1;

static class ssfx_bloom_2 : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.set_c(C, ps_ssfx_bloom_2); }
} ssfx_bloom_2;

static class ssfx_pom: public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override { cmd_list.set_c(C, ps_ssfx_pom); }
} ssfx_pom;

static class cl_taa_jitter final : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        cmd_list.set_c(C, 
            ps_r_taa_jitter.x, 
            ps_r_taa_jitter.y,
            ps_r2_ls_flags.test(R2FLAG_HAT) ? 1.f : 0.f, 
            0.f);
    }
} binder_taa_jitter;

extern float r_dtex_paralax_range;
static class cl_parallax : public R_constant_setup
{
    virtual void setup(CBackend& cmd_list, R_constant* C)
    {
        const float h = ps_r2_df_parallax_h;
        cmd_list.set_c(C, h, -h / 2.f, 1.f / r_dtex_paralax_range, 1.f / r_dtex_paralax_range);
    }
} binder_parallax;


static class cl_pos_decompress_params : public R_constant_setup
{
    virtual void setup(CBackend& cmd_list, R_constant* C)
    {
        const float VertTan = -1.0f * tanf(deg2rad(Device.fFOV / 2.0f));
        const float HorzTan = -VertTan / Device.fASPECT;

        cmd_list.set_c(C, HorzTan, VertTan, (2.0f * HorzTan) / (float)Device.dwWidth, (2.0f * VertTan) / (float)Device.dwHeight);
    }
} binder_pos_decompress_params;

static class cl_pos_decompress_params2 : public R_constant_setup
{
    virtual void setup(CBackend& cmd_list, R_constant* C)
    {
        cmd_list.set_c(C, (float)Device.dwWidth, (float)Device.dwHeight, 1.0f / (float)Device.dwWidth, 1.0f / (float)Device.dwHeight);
    }
} binder_pos_decompress_params2;

static class cl_mProject : public R_constant_setup
{
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        cmd_list.set_c(C, Device.mProjectSaved._43, Device.mProjectSaved._33, Device.mProject_hud._43, Device.mProject_hud._33);
    }
} binder_mProject;

static class cl_water_intensity : public R_constant_setup
{
    virtual void setup(CBackend& cmd_list, R_constant* C)
    {
        const CEnvDescriptor& E = *g_pGamePersistent->Environment().CurrentEnv;
        const float fValue = E.m_fWaterIntensity;
        cmd_list.set_c(C, fValue, fValue, fValue, 0);
    }
} binder_water_intensity;

static class cl_sun_shafts_intensity : public R_constant_setup
{
    virtual void setup(CBackend& cmd_list, R_constant* C)
    {
        const CEnvDescriptor& E = *g_pGamePersistent->Environment().CurrentEnv;
        const float fValue = E.m_fSunShaftsIntensity;
        cmd_list.set_c(C, fValue, fValue, fValue, 0);
    }
} binder_sun_shafts_intensity;

static class cl_artifacts final : public R_constant_setup
{
    u32 marker{};
    Fmatrix result{};

    void setup(CBackend& cmd_list, R_constant* C) override
    {
        if (marker != Device.dwFrame)
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

            marker = Device.dwFrame;
        }
        cmd_list.set_c(C, result);
    }

    u32 start_val;

public:
    cl_artifacts() = delete;
    cl_artifacts(u32 v) : start_val(v) {}
} binder_artifacts{0}, binder_artifacts2{8}, binder_artifacts3{16};

static class cl_anomalys final : public R_constant_setup
{
    u32 marker{};
    Fmatrix result{};

    void setup(CBackend& cmd_list, R_constant* C) override
    {
        if (marker != Device.dwFrame)
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

            marker = Device.dwFrame;
        }
        cmd_list.set_c(C, result);
    }

    u32 start_val;

public:
    cl_anomalys() = delete;
    cl_anomalys(u32 v) : start_val(v) {}
} binder_anomalys{0}, binder_anomalys2{8}, binder_anomalys3{16};

static class cl_detector final : public R_constant_setup
{
    Fvector4 result;
    void setup(CBackend& cmd_list, R_constant* C) override
    {
        result.set((float)(shader_exports.get_detector_params().x), (float)(shader_exports.get_detector_params().y), 0.f, 0.f);

        cmd_list.set_c(C, result);
    }
} binder_detector;


// Standart constant-binding
void CBlender_Compile::SetMapping() const
{
    // Igor temp solution for the texgen functionality in the shader
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
    r_Constant("eye_position_lerp", &binder_eye_PL); // crookr
    r_Constant("eye_direction", &binder_eye_D);
    r_Constant("eye_direction_lerp", &binder_eye_DL); // crookr

    r_Constant("eye_normal", &binder_eye_N);

    // global-lighting (env params)
    r_Constant("L_ambient", &binder_amb_color);
    r_Constant("L_sun_color", &binder_sun0_color);
    r_Constant("L_sun_dir_w", &binder_sun0_dir_w);
    r_Constant("L_sun_dir_e", &binder_sun0_dir_e);
    r_Constant("L_hemi_color", &binder_hemi_color);

    r_Constant("screen_res", &binder_screen_res);
    r_Constant("ogse_c_screen", &binder_screen_params);

    // misc
    r_Constant("m_hud_params", &binder_hud_params); //--#SM+#--

    r_Constant("ogsr_game_time", &binder_ogsr_game_time);

    r_Constant("m_affects", &binder_pda_params);

    r_Constant("m_actor_params", &binder_actor_params);
    r_Constant("m_actor_position", &binder_actor_params2);

    r_Constant("m_flare_params", &binder_flare_params);

    r_Constant("laser_params", &binder_laser_params);

    r_Constant("fakescope", &binder_fakescope);
    r_Constant("fakescope_params1", &binder_fakescope_params1);
    r_Constant("fakescope_params2", &binder_fakescope_params2);
    r_Constant("fakescope_params3", &binder_fakescope_params3);

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
    r_Constant("wind_params_old", &binder_wind_params_old);

    r_Constant("ssfx_wind_anim", &ssfx_wind_anim);
    r_Constant("ssfx_wind_anim_old", &ssfx_wind_anim_old);

    r_Constant("ssfx_wsetup_grass", &ssfx_wind_grass);
    r_Constant("ssfx_wsetup_trees", &ssfx_wind_trees);
    r_Constant("ssfx_lut", &ssfx_lut);
    r_Constant("ssfx_shadow_bias", &ssfx_shadow_bias);

    r_Constant("ssfx_bloom_1", &ssfx_bloom_1);
    r_Constant("ssfx_bloom_2", &ssfx_bloom_2);
    r_Constant("ssfx_pom", &ssfx_pom);

    r_Constant("pnv_param_1", &pnv_param_1);
    r_Constant("pnv_param_2", &pnv_param_2);
    r_Constant("pnv_param_3", &pnv_param_3);
    r_Constant("pnv_param_4", &pnv_param_4);

    // Image corrections
    r_Constant("pp_img_corrections", &pp_image_corrections);
    r_Constant("pp_img_cg", &pp_color_grading);

    r_Constant("m_cam_inertia_smooth", &binder_cam_inertia_smooth); // yohji - smooth camera inertia
    r_Constant("m_taa_jitter", &binder_taa_jitter);

    r_Constant("parallax", &binder_parallax);
    r_Constant("water_intensity", &binder_water_intensity);
    r_Constant("sun_shafts_intensity", &binder_sun_shafts_intensity);

    r_Constant("pos_decompression_params", &binder_pos_decompress_params);
    r_Constant("pos_decompression_params2", &binder_pos_decompress_params2);

    r_Constant("dev_mProject", &binder_mProject);

    // system.ltx common
    for (const auto& [name, s] : DEV->v_constant_setup)
        r_Constant(name.c_str(), s);

    // capture constants !!!

    r_Constant("L_hotness", &binder_heatvision_hotness);
    r_Constant("triLOD", &binder_lod);
    r_Constant("m_AlphaRef", &binder_alpha_ref);

    // hemi cube
    r_Constant("hemi_cube_pos_faces", &binder_hemi_cube_pos_faces);
    r_Constant("hemi_cube_neg_faces", &binder_hemi_cube_neg_faces);
    r_Constant("L_material", &binder_material);
    r_Constant("dt_params", &binder_scale);

    // matrices
    r_Constant("m_W", &binder_w);
    r_Constant("m_invW", &binder_invw);
    r_Constant("m_V", &binder_v);
    r_Constant("m_inv_V", &binder_inv_v);
    r_Constant("m_P", &binder_p);
    r_Constant("m_WV", &binder_wv);
    r_Constant("m_VP", &binder_vp);
    r_Constant("m_WVP", &binder_wvp);

    r_Constant("m_W_old", &binder_w_old);
    r_Constant("m_V_old", &binder_v_old);
    r_Constant("m_P_old", &binder_p_old);
    r_Constant("m_WV_old", &binder_wv_old);
    r_Constant("m_VP_old", &binder_vp_old);
    r_Constant("m_WVP_old", &binder_wvp_old);

    r_Constant("ogse_c_artefacts", &binder_artifacts);
    r_Constant("ogse_c_artefacts2", &binder_artifacts2);
    r_Constant("ogse_c_artefacts3", &binder_artifacts3);
    r_Constant("ogse_c_anomalys", &binder_anomalys);
    r_Constant("ogse_c_anomalys2", &binder_anomalys2);
    r_Constant("ogse_c_anomalys3", &binder_anomalys3);
    r_Constant("ogse_c_detector", &binder_detector);
}

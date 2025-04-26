#include "stdafx.h"

#include "render.h"
#include "Thunderbolt.h"
#include "igame_persistent.h"
#include "LightAnimLibrary.h"
#include "igame_level.h"
#include "xr_object.h"

SThunderboltDesc::SThunderboltDesc() : m_GradientTop(nullptr), m_GradientCenter(nullptr) {}

SThunderboltDesc::~SThunderboltDesc()
{
    m_pRender->DestroyModel();
    m_GradientTop->m_pFlare->DestroyShader();
    m_GradientCenter->m_pFlare->DestroyShader();
    snd.destroy();

    xr_delete(m_GradientTop);
    xr_delete(m_GradientCenter);
}

void SThunderboltDesc::create_top_gradient(CInifile& pIni, shared_str const& sect)
{
    m_GradientTop = xr_new<SFlare>();
    m_GradientTop->shader = pIni.r_string(sect, "gradient_top_shader");
    m_GradientTop->texture = pIni.r_string(sect, "gradient_top_texture");
    m_GradientTop->fRadius = pIni.r_fvector2(sect, "gradient_top_radius");
    m_GradientTop->fOpacity = pIni.r_float(sect, "gradient_top_opacity");
    m_GradientTop->m_pFlare->CreateShader(*m_GradientTop->shader, *m_GradientTop->texture);
}

void SThunderboltDesc::create_center_gradient(CInifile& pIni, shared_str const& sect)
{
    m_GradientCenter = xr_new<SFlare>();
    m_GradientCenter->shader = pIni.r_string(sect, "gradient_center_shader");
    m_GradientCenter->texture = pIni.r_string(sect, "gradient_center_texture");
    m_GradientCenter->fRadius = pIni.r_fvector2(sect, "gradient_center_radius");
    m_GradientCenter->fOpacity = pIni.r_float(sect, "gradient_center_opacity");
    m_GradientCenter->m_pFlare->CreateShader(*m_GradientCenter->shader, *m_GradientCenter->texture);
}

void SThunderboltDesc::load(CInifile& pIni, shared_str const& sect)
{
    create_top_gradient(pIni, sect);
    create_center_gradient(pIni, sect);

    name = sect;
    color_anim = LALib.FindItem(pIni.r_string(sect, "color_anim"));
    VERIFY(color_anim);
    color_anim->fFPS = (float)color_anim->iFrameCount;

    // models
    LPCSTR m_name;
    m_name = pIni.r_string(sect, "lightning_model");
    m_pRender->CreateModel(m_name);

    /*
    IReader* F			= 0;
    F					= FS.r_open("$game_meshes$",m_name); R_ASSERT2(F,"Empty 'lightning_model'.");
    l_model				= ::Render->model_CreateDM(F);
    FS.r_close			(F);
    */

    // sound
    m_name = pIni.r_string(sect, "sound");
    if (m_name && m_name[0])
        snd.create(m_name, st_Effect, sg_Undefined);
}

void SThunderboltDesc::create_top_gradient_shoc(CInifile* pIni, shared_str const& sect)
{
    m_GradientTop = xr_new<SFlare>();
    m_GradientTop->shader = pIni->r_string(sect, "gradient_top_shader");
    m_GradientTop->texture = pIni->r_string(sect, "gradient_top_texture");
    m_GradientTop->fRadius = pIni->r_fvector2(sect, "gradient_top_radius");
    m_GradientTop->fOpacity = pIni->r_float(sect, "gradient_top_opacity");
    m_GradientTop->m_pFlare->CreateShader(*m_GradientTop->shader, *m_GradientTop->texture);
}

void SThunderboltDesc::create_center_gradient_shoc(CInifile* pIni, shared_str const& sect)
{
    m_GradientCenter = xr_new<SFlare>();
    m_GradientCenter->shader = pIni->r_string(sect, "gradient_center_shader");
    m_GradientCenter->texture = pIni->r_string(sect, "gradient_center_texture");
    m_GradientCenter->fRadius = pIni->r_fvector2(sect, "gradient_center_radius");
    m_GradientCenter->fOpacity = pIni->r_float(sect, "gradient_center_opacity");
    m_GradientCenter->m_pFlare->CreateShader(*m_GradientCenter->shader, *m_GradientCenter->texture);
}

void SThunderboltDesc::load_shoc(CInifile* pIni, shared_str const& sect)
{
    create_top_gradient_shoc(pIni, sect);
    create_center_gradient_shoc(pIni, sect);

    name = sect;
    color_anim = LALib.FindItem(pIni->r_string(sect, "color_anim"));
    VERIFY(color_anim);
    color_anim->fFPS = (float)color_anim->iFrameCount;

    // models
    LPCSTR m_name;
    m_name = pIni->r_string(sect, "lightning_model");
    m_pRender->CreateModel(m_name);

    /*
    IReader* F			= 0;
    F					= FS.r_open("$game_meshes$",m_name); R_ASSERT2(F,"Empty 'lightning_model'.");
    l_model				= ::Render->model_CreateDM(F);
    FS.r_close			(F);
    */

    // sound
    m_name = pIni->r_string(sect, "sound");
    if (m_name && m_name[0])
        snd.create(m_name, st_Effect, sg_Undefined);
}

//----------------------------------------------------------------------------------------------
// collection
//----------------------------------------------------------------------------------------------
SThunderboltCollection::SThunderboltCollection() {}

void SThunderboltCollection::load(CInifile* pIni, CInifile* thunderbolts, LPCSTR sect)
{
    section = sect;
    int tb_count = pIni->line_count(sect);
    for (int tb_idx = 0; tb_idx < tb_count; tb_idx++)
    {
        LPCSTR N, V;
        if (pIni->r_line(sect, tb_idx, &N, &V))
            palette.push_back(g_pGamePersistent->Environment().thunderbolt_description(*thunderbolts, N));
    }
}

void SThunderboltCollection::load_shoc(CInifile* pIni, LPCSTR sect)
{
    section = sect;
    int tb_count = pIni->line_count(sect);
    for (int tb_idx = 0; tb_idx < tb_count; tb_idx++)
    {
        LPCSTR N, V;
        if (pIni->r_line(sect, tb_idx, &N, &V))
            palette.push_back(g_pGamePersistent->Environment().thunderbolt_description_shoc(pIni, N));
    }
}

SThunderboltCollection::~SThunderboltCollection()
{
    for (auto& d_it : palette)
        xr_delete(d_it);

    palette.clear();
}

//----------------------------------------------------------------------------------------------
// thunderbolt effect
//----------------------------------------------------------------------------------------------
CEffect_Thunderbolt::CEffect_Thunderbolt()
{
    current = nullptr;
    life_time = 0.f;
    state = stIdle;
    next_lightning_time = 0.f;
    bEnabled = FALSE;
}

CEffect_Thunderbolt::~CEffect_Thunderbolt()
{
    for (auto& d_it : collection)
        xr_delete(d_it);
    collection.clear();
    // hGeom_model.destroy			();
    // hGeom_gradient.destroy		();
}

shared_str CEffect_Thunderbolt::AppendDef(CEnvironment& environment, CInifile* pIni, CInifile* thunderbolts, LPCSTR sect)
{
    if (!sect || (0 == sect[0]))
        return "";

    for (const auto* it : collection)
        if (it->section == sect)
            return it->section;

    return collection.emplace_back(environment.thunderbolt_collection(pIni, thunderbolts, sect))->section;
}

shared_str CEffect_Thunderbolt::AppendDef_shoc(CEnvironment& environment, CInifile* pIni, LPCSTR sect)
{
    if (!sect || (0 == sect[0]))
        return "";

    for (const auto* it : collection)
        if (it->section == sect)
            return it->section;

    return collection.emplace_back(environment.thunderbolt_collection_shoc(pIni, sect))->section;
}

BOOL CEffect_Thunderbolt::RayPick(const Fvector& s, const Fvector& d, float& dist)
{
    ZoneScoped;

    collide::rq_result RQ;
    CObject* E = g_pGameLevel->CurrentViewEntity();
    BOOL bRes = g_pGameLevel->ObjectSpace.RayPick(s, d, dist, collide::rqtBoth, RQ, E);
    if (bRes)
        dist = RQ.range;
    else
    {
        Fvector N = {0.f, -1.f, 0.f};
        Fvector P = {0.f, 0.f, 0.f};
        Fplane PL;
        PL.build(P, N);
        float dst = dist;
        if (PL.intersectRayDist(s, d, dst) && (dst <= dist))
        {
            dist = dst;
            return true;
        }
        else
            return false;
    }

    return bRes;
}

void CEffect_Thunderbolt::Bolt(shared_str id, float period, float lt)
{
    ZoneScoped;

    VERIFY(id.size());
    state = stWorking;
    life_time = lt + Random.randF(-lt * 0.5f, lt * 0.5f);
    current_time = 0.f;

    current = g_pGamePersistent->Environment().thunderbolt_collection(collection, id)->GetRandomDesc();
    VERIFY(current);

    float far_plane = g_pGamePersistent->Environment().CurrentEnv->far_plane;

    Fmatrix XF, S;
    Fvector pos, dev;
    float sun_h, sun_p;
    CEnvironment& environment = g_pGamePersistent->Environment();
    environment.CurrentEnv->sun_dir.getHP(sun_h, sun_p);
    float alt = environment.p_var_alt; // Random.randF(environment.p_var_alt.x,environment.p_var_alt.y);
    float lng = Random.randF(sun_h - environment.p_var_long + PI, sun_h + environment.p_var_long + PI);
    float dist = Random.randF(far_plane * environment.p_min_dist, far_plane * .95f);
    current_direction.setHP(lng, alt);
    pos.mad(Device.vCameraPosition, current_direction, dist);
    dev.x = Random.randF(-environment.p_tilt, environment.p_tilt);
    dev.y = Random.randF(0, PI_MUL_2);
    dev.z = Random.randF(-environment.p_tilt, environment.p_tilt);
    XF.setXYZi(dev);

    Fvector light_dir = {0.f, -1.f, 0.f};
    XF.transform_dir(light_dir);
    lightning_size = far_plane * 2.f;
    RayPick(pos, light_dir, lightning_size);

    lightning_center.mad(pos, light_dir, lightning_size * 0.5f);

    S.scale(lightning_size, lightning_size, lightning_size);
    XF.translate_over(pos);
    current_xform.mul_43(XF, S);

    float next_v = Random.randF();

    if (next_v < environment.p_second_prop)
    {
        next_lightning_time = Device.fTimeGlobal + lt + EPS_L;
    }
    else
    {
        next_lightning_time = Device.fTimeGlobal + period + Random.randF(-period * 0.3f, period * 0.3f);
        current->snd.play_no_feedback(nullptr, 0, dist / 300.f, &pos, nullptr, nullptr, &Fvector2().set(dist / 2, dist * 2.f));
    }

    current_direction.invert(); // for env-sun
}

void CEffect_Thunderbolt::OnFrame(shared_str id, float period, float duration)
{
    ZoneScoped;

    BOOL enabled = !!(id.size());
    if (bEnabled != enabled)
    {
        bEnabled = enabled;
        next_lightning_time = Device.fTimeGlobal + period + Random.randF(-period * 0.5f, period * 0.5f);
    }
    else if (bEnabled && (Device.fTimeGlobal > next_lightning_time))
    {
        if (state == stIdle && !!(id.size()))
            Bolt(id, period, duration);
    }
    if (state == stWorking)
    {
        if (current_time > life_time)
            state = stIdle;
        current_time += Device.fTimeDelta;
        Fvector fClr;
        int frame;
        u32 uClr = current->color_anim->CalculateRGB(current_time / life_time, frame);
        fClr.set(clampr(float(color_get_R(uClr) / 255.f), 0.f, 1.f), clampr(float(color_get_G(uClr) / 255.f), 0.f, 1.f), clampr(float(color_get_B(uClr) / 255.f), 0.f, 1.f));

        lightning_phase = 1.5f * (current_time / life_time);
        clamp(lightning_phase, 0.f, 1.f);

        CEnvironment& environment = g_pGamePersistent->Environment();

        Fvector& sky_color = environment.CurrentEnv->sky_color;
        sky_color.mad(fClr, environment.p_sky_color);
        clamp(sky_color.x, 0.f, 1.f);
        clamp(sky_color.y, 0.f, 1.f);
        clamp(sky_color.z, 0.f, 1.f);

        environment.CurrentEnv->sun_color.mad(fClr, environment.p_sun_color);
        environment.CurrentEnv->fog_color.mad(fClr, environment.p_fog_color);

        R_ASSERT(_valid(current_direction));
        g_pGamePersistent->Environment().CurrentEnv->sun_dir = current_direction;
        VERIFY(g_pGamePersistent->Environment().CurrentEnv->sun_dir.y < 0, "Invalid sun direction settings while CEffect_Thunderbolt");
    }
}

void CEffect_Thunderbolt::Render(CBackend& cmd_list)
{
    if (state == stWorking)
    {
        m_pRender->Render(cmd_list, *this);
    }
}

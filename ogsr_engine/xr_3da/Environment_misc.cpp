#include "stdafx.h"

#include "Environment.h"
#include "xr_efflensflare.h"
#include "thunderbolt.h"
#include "rain.h"

#include "IGame_Level.h"
#include "igame_persistent.h"

#include "../COMMON_AI/object_broker.h"
#include "../COMMON_AI/LevelGameDef.h"
#include "../xrGame/ShapeData.h"

ENGINE_API float ps_r_sunshafts_intensity = 0.0f;
ENGINE_API float puddles_drying = 2.f;
ENGINE_API float puddles_wetting = 4.f;
ENGINE_API BOOL bLevelEnvModExport{};

bool CEnvModifier::load(IReader* fs, u32 version)
{
    // Проверка на корректность файла. Real Wolf.
    if (fs->tell() + (version < 0x0016 ? 76 : (version < 0x0018 ? 78 : 103)) > fs->length())
    {
        R_ASSERT(version < 0x0018, "!!Invalid file level.env_mod!");
        return false;
    }

    fs->r_fvector3(position);
    radius = fs->r_float();
    power = fs->r_float();
    far_plane = fs->r_float();
    fs->r_fvector3(fog_color);
    fog_density = fs->r_float();
    fs->r_fvector3(ambient);
    fs->r_fvector3(sky_color);
    fs->r_fvector3(hemi_color);

    use_flags.one();
    if (version >= 0x0016)
    {
        use_flags.assign(fs->r_u16());
    }

    if (version >= 0x0018)
    {
        shape_type = fs->r_u8();
        fs->r_fvector3(shape_rotate);
        fs->r_fvector3(shape_halfsize);

        obb.m_translate.set(position);
        obb.m_rotate.set(Fmatrix().setXYZ(shape_rotate));
        obb.m_halfsize.set(shape_halfsize);
    }

    return true;
}

bool CEnvModifier::loadIni(CInifile& ini, LPCSTR section)
{
    position = ini.r_fvector3(section, "position");
    radius = ini.r_float(section, "radius");
    power = ini.r_float(section, "power");
    far_plane = ini.r_float(section, "far_plane");
    fog_color = ini.r_fvector3(section, "fog_color");
    fog_density = ini.r_float(section, "fog_density");
    ambient = ini.r_fvector3(section, "ambient");
    sky_color = ini.r_fvector3(section, "sky_color");
    hemi_color = ini.r_fvector3(section, "hemi_color");
    use_flags.one();
    if (ini.line_exist(section, "use_flags"))
    {
        use_flags.assign(ini.r_u16(section, "use_flags"));
    }
    if (ini.line_exist(section, "shape_type"))
    {
        shape_type = ini.r_u8(section, "shape_type");
        shape_rotate = ini.r_fvector3(section, "shape_rotate");
        shape_halfsize = ini.r_fvector3(section, "shape_halfsize");

        obb.m_translate.set(position);
        obb.m_rotate.set(Fmatrix().setXYZ(shape_rotate));
        obb.m_halfsize.set(shape_halfsize);
    }

    return true;
}

bool CEnvModifier::saveIni(CInifile& ini, LPCSTR section)
{
    ini.w_fvector3(section, "position", position);
    ini.w_float(section, "radius", radius);
    ini.w_float(section, "power", power);
    ini.w_float(section, "far_plane", far_plane);
    ini.w_fvector3(section, "fog_color", fog_color);
    ini.w_float(section, "fog_density", fog_density);
    ini.w_fvector3(section, "ambient", ambient);
    ini.w_fvector3(section, "sky_color", sky_color);
    ini.w_fvector3(section, "hemi_color", hemi_color);

    ini.w_u16(section, "use_flags", use_flags.get());

    ini.w_u8(section, "shape_type", shape_type);
    ini.w_fvector3(section, "shape_rotate", shape_rotate);
    ini.w_fvector3(section, "shape_halfsize", shape_halfsize);

    return true;
}

float CEnvModifier::sum(CEnvModifier& M, Fvector3& view)
{
    float _dist_sq{};
    switch (M.shape_type)
    {
    case CShapeData::cfSphere: _dist_sq = view.distance_to_sqr(M.position); break;
    case CShapeData::cfBox:
        Fmatrix xform;
        M.obb.xform_get(xform);
        Fvector n, p;
        // make 6 planes from OBB
        Fplane pl[6];
        pl[0].build_unit_normal(p.set(M.obb.m_halfsize.x, 0, 0), n.set(1, 0, 0)).transform(xform); // +X
        pl[1].build_unit_normal(p.set(-M.obb.m_halfsize.x, 0, 0), n.set(-1, 0, 0)).transform(xform); // -X
        pl[2].build_unit_normal(p.set(0, M.obb.m_halfsize.y, 0), n.set(0, 1, 0)).transform(xform); // +Y
        pl[3].build_unit_normal(p.set(0, -M.obb.m_halfsize.y, 0), n.set(0, -1, 0)).transform(xform); // -Y
        pl[4].build_unit_normal(p.set(0, 0, M.obb.m_halfsize.z), n.set(0, 0, 1)).transform(xform); // +Z
        pl[5].build_unit_normal(p.set(0, 0, -M.obb.m_halfsize.z), n.set(0, 0, -1)).transform(xform); // -Z
        p.set(view);
        for (int i = 0; i < 6; i++)
            if (pl[i].classify(p) > 0.0f)
                pl[i].project(p, p);
        _dist_sq = view.distance_to_sqr(p);
        break;
    default: FATAL("Invalid shape_type in level.env_mod!");
    }

    if (_dist_sq >= (M.radius * M.radius))
        return 0;

    float _att = 1 - _sqrt(_dist_sq) / M.radius; //[0..1];
    float _power = M.power * _att;

    if (M.use_flags.test(eViewDist))
    {
        far_plane += M.far_plane * _power;
        use_flags.set(eViewDist, TRUE);
    }
    if (M.use_flags.test(eFogColor))
    {
        fog_color.mad(M.fog_color, _power);
        use_flags.set(eFogColor, TRUE);
    }
    if (M.use_flags.test(eFogDensity))
    {
        fog_density += M.fog_density * _power;
        use_flags.set(eFogDensity, TRUE);
    }
    if (M.use_flags.test(eAmbientColor))
    {
        ambient.mad(M.ambient, _power);
        use_flags.set(eAmbientColor, TRUE);
    }
    if (M.use_flags.test(eSkyColor))
    {
        sky_color.mad(M.sky_color, _power);
        use_flags.set(eSkyColor, TRUE);
    }
    if (M.use_flags.test(eHemiColor))
    {
        hemi_color.mad(M.hemi_color, _power);
        use_flags.set(eHemiColor, TRUE);
    }

    return _power;
}

//-----------------------------------------------------------------------------
// Environment ambient
//-----------------------------------------------------------------------------
void CEnvAmbient::SSndChannel::load(CInifile& config, LPCSTR sect)
{
    m_load_section = sect;

    m_sound_dist.x = config.r_float(sect, "min_distance");
    m_sound_dist.y = config.r_float(sect, "max_distance");
    m_sound_period.x = config.r_s32(sect, "period0");
    m_sound_period.y = config.r_s32(sect, "period1");
    m_sound_period.z = config.r_s32(sect, "period2");
    m_sound_period.w = config.r_s32(sect, "period3");

    //	m_sound_period			= config.r_ivector4(sect,"sound_period");
    R_ASSERT(m_sound_period.x <= m_sound_period.y && m_sound_period.z <= m_sound_period.w);
    //	m_sound_period.mul		(1000);// now in ms
    //	m_sound_dist			= config.r_fvector2(sect,"sound_dist");
    R_ASSERT(m_sound_dist.y > m_sound_dist.x, sect);

    LPCSTR snds = config.r_string(sect, "sounds");
    u32 cnt = _GetItemCount(snds);
    string_path tmp;
    R_ASSERT(cnt, "sounds empty", sect);

    m_sounds.resize(cnt);

    for (u32 k = 0; k < cnt; ++k)
    {
        _GetItem(snds, k, tmp);
        m_sounds[k].create(tmp, st_Effect, sg_SourceType);
    }
}

CEnvAmbient::SEffect* CEnvAmbient::create_effect(CInifile& config, LPCSTR sect)
{
    SEffect* result = xr_new<SEffect>();
    result->life_time = iFloor(config.r_float(sect, "life_time") * 1000.f);
    result->m_load_section = sect;

    result->particles = config.r_string(sect, "particles");
    VERIFY(result->particles.size());
#pragma todo("SIMP: Пересмотреть это! Пока убрал метод CreateAmbientParticle, в xrGame по этой части правки не вносились.") //
    //g_pGamePersistent->CreateAmbientParticle(result);

    result->offset = config.r_fvector3(sect, "offset");
    result->wind_gust_factor = config.r_float(sect, "wind_gust_factor");

    if (config.line_exist(sect, "sound"))
        result->sound.create(config.r_string(sect, "sound"), st_Effect, sg_SourceType);

    if (config.line_exist(sect, "wind_blast_strength"))
    {
        result->wind_blast_strength = config.r_float(sect, "wind_blast_strength");
        result->wind_blast_direction.setHP(deg2rad(config.r_float(sect, "wind_blast_longitude")), 0.f);
        result->wind_blast_in_time = config.r_float(sect, "wind_blast_in_time");
        result->wind_blast_out_time = config.r_float(sect, "wind_blast_out_time");

        return (result);
    }

    result->wind_blast_strength = 0.f;
    result->wind_blast_direction.set(0.f, 0.f, 1.f);
    result->wind_blast_in_time = 0.f;
    result->wind_blast_out_time = 0.f;

    return (result);
}

CEnvAmbient::SSndChannel* CEnvAmbient::create_sound_channel(CInifile& config, LPCSTR id)
{
    SSndChannel* result = xr_new<SSndChannel>();
    result->load(config, id);
    return (result);
}

CEnvAmbient::~CEnvAmbient() { destroy(); }

void CEnvAmbient::destroy()
{
    delete_data(m_effects);
    delete_data(m_sound_channels);
}

void CEnvAmbient::load(CInifile& ambients_config, CInifile& sound_channels_config, CInifile& effects_config, const shared_str& sect)
{
    m_ambients_config_filename = ambients_config.fname();
    m_load_section = sect;
    string_path tmp;

    // sounds
    LPCSTR channels = ambients_config.r_string(sect, "sound_channels");
    u32 cnt = _GetItemCount(channels);
    //	R_ASSERT				(cnt,"sound_channels empty", sect.c_str());
    m_sound_channels.resize(cnt);

    for (u32 i = 0; i < cnt; ++i)
        m_sound_channels[i] = create_sound_channel(sound_channels_config, _GetItem(channels, i, tmp));

    // effects
    m_effect_period.set(iFloor(ambients_config.r_float(sect, "min_effect_period") * 1000.f), iFloor(ambients_config.r_float(sect, "max_effect_period") * 1000.f));
    LPCSTR effs = ambients_config.r_string(sect, "effects");
    cnt = _GetItemCount(effs);
    //	R_ASSERT				(cnt,"effects empty", sect.c_str());

    m_effects.resize(cnt);
    for (u32 k = 0; k < cnt; ++k)
        m_effects[k] = create_effect(effects_config, _GetItem(effs, k, tmp));

    R_ASSERT(!m_sound_channels.empty() || !m_effects.empty());
}

void CEnvAmbient::load_shoc(const shared_str& sect)
{
    section = sect;
    string_path tmp;
    // sounds
    ASSERT_FMT_DBG(pSettings->line_exist(sect, "sounds"), "CEnvAmbient::load: section '%s' not found", section.c_str());
    if (pSettings->line_exist(sect, "sounds"))
    {
        Fvector2 t = pSettings->r_fvector2(sect, "sound_period");
        sound_period.set(iFloor(t.x * 1000.f), iFloor(t.y * 1000.f));
        sound_dist = pSettings->r_fvector2(sect, "sound_dist");
        if (sound_dist[0] > sound_dist[1])
            std::swap(sound_dist[0], sound_dist[1]);
        LPCSTR snds = pSettings->r_string(sect, "sounds");
        u32 cnt = _GetItemCount(snds);
        if (cnt)
        {
            sounds.resize(cnt);
            for (u32 k = 0; k < cnt; ++k)
                sounds[k].create(_GetItem(snds, k, tmp), st_Effect, sg_SourceType);
        }
    }
    // effects
    if (pSettings->line_exist(sect, "effects"))
    {
        Fvector2 t = pSettings->r_fvector2(sect, "effect_period");
        effect_period.set(iFloor(t.x * 1000.f), iFloor(t.y * 1000.f));
        LPCSTR effs = pSettings->r_string(sect, "effects");
        u32 cnt = _GetItemCount(effs);
        if (cnt)
        {
            m_effects.resize(cnt);
            for (u32 k = 0; k < cnt; ++k)
            {
                SEffect* result = xr_new<SEffect>();
                _GetItem(effs, k, tmp);
                result->life_time = iFloor(pSettings->r_float(tmp, "life_time") * 1000.f);
                result->particles = pSettings->r_string(tmp, "particles");
                VERIFY(effects[k].particles.size());
                result->offset = pSettings->r_fvector3(tmp, "offset");
                result->wind_gust_factor = pSettings->r_float(tmp, "wind_gust_factor");
                if (pSettings->line_exist(tmp, "sound"))
                    result->sound.create(pSettings->r_string(tmp, "sound"), st_Effect, sg_SourceType);
                m_effects[k] = result;
            }
        }
    }
    VERIFY(!sounds.empty() || !m_effects.empty());
}

//-----------------------------------------------------------------------------
// Environment descriptor
//-----------------------------------------------------------------------------
CEnvDescriptor::CEnvDescriptor(shared_str const& identifier) : m_identifier(identifier)
{
    exec_time = 0.0f;
    exec_time_loaded = 0.0f;

    clouds_color.set(1, 1, 1, 1);
    sky_color.set(1, 1, 1);
    sky_rotation = 0.0f;

    far_plane = 400.0f;
    

    fog_color.set(1, 1, 1);
    fog_density = 0.0f;
    fog_distance = 400.0f;

    rain_density = 0.0f;
    rain_color.set(0, 0, 0);

    bolt_period = 0.0f;
    bolt_duration = 0.0f;

    wind_velocity = 0.0f;
    wind_direction = 0.0f;

    ambient.set(0, 0, 0);
    hemi_color.set(1, 1, 1, 1);
    sun_color.set(1, 1, 1);
    sun_dir.set(0, -1, 0);

    m_fSunShaftsIntensity = 0.f;
    m_fWaterIntensity = 1;
    m_fTreeAmplitudeIntensity = 0.01f;

    bloom_threshold = 3.5f;
    bloom_exposure = 3.f;
    bloom_sky_intensity = 0.6f;

    lens_flare_id = "";
    tb_id = "";

    env_ambient = nullptr;
}

#define C_CHECK(C) \
    if (C.x < 0 || C.x > 2 || C.y < 0 || C.y > 2 || C.z < 0 || C.z > 2) \
    { \
        Msg("! Invalid '%s' in env-section '%s'", #C, m_identifier.c_str()); \
    }

void CEnvDescriptor::load(CEnvironment& environment, CInifile& config)
{
    Ivector3 tm = {0, 0, 0};
    sscanf(m_identifier.c_str(), "%d:%d:%d", &tm.x, &tm.y, &tm.z);
    R_ASSERT((tm.x >= 0) && (tm.x < 24) && (tm.y >= 0) && (tm.y < 60) && (tm.z >= 0) && (tm.z < 60), "Incorrect weather time", m_identifier.c_str());
    exec_time = tm.x * 3600.f + tm.y * 60.f + tm.z;
    exec_time_loaded = exec_time;
    string_path st, st_env;
    xr_strcpy(st, config.r_string(m_identifier.c_str(), "sky_texture"));
    strconcat(sizeof(st_env), st_env, st, "#small");
    sky_texture_name = st;
    sky_texture_env_name = st_env;
    clouds_texture_name = config.r_string(m_identifier.c_str(), "clouds_texture");
    LPCSTR cldclr = config.r_string(m_identifier.c_str(), "clouds_color");
    float multiplier = 0;
    sscanf(cldclr, "%f,%f,%f,%f,%f", &clouds_color.x, &clouds_color.y, &clouds_color.z, &clouds_color.w, &multiplier);
    if (!fis_zero(multiplier))
    {
        float save = clouds_color.w;
        clouds_color.mul(.5f * multiplier);
        clouds_color.w = save;
    }

    sky_color = config.r_fvector3(m_identifier.c_str(), "sky_color");

    if (config.line_exist(m_identifier.c_str(), "sky_rotation"))
        sky_rotation = deg2rad(config.r_float(m_identifier.c_str(), "sky_rotation"));
    else
        sky_rotation = 0;
    far_plane = config.r_float(m_identifier.c_str(), "far_plane");
    fog_color = config.r_fvector3(m_identifier.c_str(), "fog_color");
    fog_density = config.r_float(m_identifier.c_str(), "fog_density");
    fog_distance = config.r_float(m_identifier.c_str(), "fog_distance");
    rain_density = config.r_float(m_identifier.c_str(), "rain_density");
    clamp(rain_density, 0.f, 1.f);
    rain_color = config.r_fvector3(m_identifier.c_str(), "rain_color");
    wind_velocity = config.r_float(m_identifier.c_str(), "wind_velocity");
    wind_direction = deg2rad(config.r_float(m_identifier.c_str(), "wind_direction"));
    ambient = config.r_fvector3(m_identifier.c_str(), "ambient_color");
    hemi_color = config.r_fvector4(m_identifier.c_str(), "hemisphere_color");
    sun_color = config.r_fvector3(m_identifier.c_str(), "sun_color");

    if (environment.m_static_sun_movement || environment.m_dynamic_sun_movement)
    {
        sun_dir.set(1.f, 1.f, 1.f);
    }
    else
    {
        sun_dir.setHP(deg2rad(config.r_float(m_identifier.c_str(), "sun_altitude")), deg2rad(config.r_float(m_identifier.c_str(), "sun_longitude")));
        R_ASSERT(_valid(sun_dir));
    }

    VERIFY(sun_dir.y < 0, "Invalid sun direction settings while loading");

    lens_flare_id = environment.eff_LensFlare->AppendDef(environment, config.r_string(m_identifier.c_str(), "sun"));
    tb_id = environment.eff_Thunderbolt->AppendDef(environment, environment.m_thunderbolt_collections_config, environment.m_thunderbolts_config, config.r_string(m_identifier.c_str(), "thunderbolt_collection"));
    bolt_period = (tb_id.size()) ? config.r_float(m_identifier.c_str(), "thunderbolt_period") : 0.f;
    bolt_duration = (tb_id.size()) ? config.r_float(m_identifier.c_str(), "thunderbolt_duration") : 0.f;
    env_ambient = config.line_exist(m_identifier.c_str(), "ambient") ? environment.AppendEnvAmb(config.r_string(m_identifier.c_str(), "ambient")) : nullptr;

    if (config.line_exist(m_identifier.c_str(), "sun_shafts_intensity"))
        m_fSunShaftsIntensity = config.r_float(m_identifier.c_str(), "sun_shafts_intensity");
    else
        m_fSunShaftsIntensity = 0.f;

    if (config.line_exist(m_identifier.c_str(), "water_intensity"))
        m_fWaterIntensity = config.r_float(m_identifier.c_str(), "water_intensity");

    constexpr float def_min_TAI = 0.01f, def_max_TAI = 0.07f;
    const float def_TAI = def_min_TAI + (rain_density * (def_max_TAI - def_min_TAI)); //Если не прописано, дефолт будет рассчитываться от силы дождя.
    m_fTreeAmplitudeIntensity = READ_IF_EXISTS((&config), r_float, m_identifier.c_str(), "tree_amplitude_intensity", def_TAI);

	bloom_threshold = config.line_exist(m_identifier.c_str(), "bloom_threshold") ? config.r_float(m_identifier.c_str(), "bloom_threshold") : 3.5f;
    bloom_exposure = config.line_exist(m_identifier.c_str(), "bloom_exposure") ? config.r_float(m_identifier.c_str(), "bloom_exposure") : 3.0f;
    bloom_sky_intensity = config.line_exist(m_identifier.c_str(), "bloom_sky_intensity") ? config.r_float(m_identifier.c_str(), "bloom_sky_intensity") : 0.6f;

    C_CHECK(clouds_color);
    C_CHECK(sky_color);
    C_CHECK(fog_color);
    C_CHECK(rain_color);
    C_CHECK(ambient);
    C_CHECK(hemi_color);
    C_CHECK(sun_color);

    on_device_create();
}

void CEnvDescriptor::load_shoc(CEnvironment& environment, LPCSTR exec_tm, LPCSTR S)
{
    Ivector3 tm{};
    sscanf(exec_tm, "%d:%d:%d", &tm.x, &tm.y, &tm.z);
    R_ASSERT((tm.x >= 0) && (tm.x < 24) && (tm.y >= 0) && (tm.y < 60) && (tm.z >= 0) && (tm.z < 60), "Incorrect weather time", S);
    load_shoc(tm.x * 3600.f + tm.y * 60.f + tm.z, S, environment);
}

void CEnvDescriptor::load_shoc(float exec_tm, LPCSTR S, CEnvironment& environment)
{
    m_identifier = S;
    exec_time = exec_tm;
    exec_time_loaded = exec_time;
    string_path st, st_env;
    xr_strcpy(st, pSettings->r_string(m_identifier.c_str(), "sky_texture"));
    strconcat(sizeof(st_env), st_env, st, "#small");
    sky_texture_name = st;
    sky_texture_env_name = st_env;
    clouds_texture_name = pSettings->r_string(m_identifier.c_str(), "clouds_texture");
    LPCSTR cldclr = pSettings->r_string(m_identifier.c_str(), "clouds_color");
    float multiplier = 0;
    sscanf(cldclr, "%f,%f,%f,%f,%f", &clouds_color.x, &clouds_color.y, &clouds_color.z, &clouds_color.w, &multiplier);
    if (!fis_zero(multiplier))
    {
        float save = clouds_color.w;
        clouds_color.mul(.5f * multiplier);
        clouds_color.w = save;
    }

    sky_color = pSettings->r_fvector3(m_identifier.c_str(), "sky_color");
    sky_color.mul(0.5f);

    if (pSettings->line_exist(m_identifier.c_str(), "sky_rotation"))
        sky_rotation = deg2rad(pSettings->r_float(m_identifier.c_str(), "sky_rotation"));
    else
        sky_rotation = 0;
    far_plane = pSettings->r_float(m_identifier.c_str(), "far_plane");
    fog_color = pSettings->r_fvector3(m_identifier.c_str(), "fog_color");
    fog_density = pSettings->r_float(m_identifier.c_str(), "fog_density");
    fog_distance = pSettings->r_float(m_identifier.c_str(), "fog_distance");
    rain_density = pSettings->r_float(m_identifier.c_str(), "rain_density");
    clamp(rain_density, 0.f, 1.f);
    rain_color = pSettings->r_fvector3(m_identifier.c_str(), "rain_color");
    wind_velocity = pSettings->r_float(m_identifier.c_str(), "wind_velocity");
    wind_direction = deg2rad(pSettings->r_float(m_identifier.c_str(), "wind_direction"));
    ambient = pSettings->r_fvector3(m_identifier.c_str(), "ambient");
    hemi_color = pSettings->r_fvector4(m_identifier.c_str(), "hemi_color");
    hemi_color.w = 1.f;
    sun_color = pSettings->r_fvector3(m_identifier.c_str(), "sun_color");
    Fvector2 sund = pSettings->r_fvector2(m_identifier.c_str(), "sun_dir");
    sun_dir.setHP(deg2rad(sund.y), deg2rad(sund.x));
    VERIFY(sun_dir.y < 0, "Invalid sun direction settings while loading");

    lens_flare_id = environment.eff_LensFlare->AppendDef(environment, pSettings->r_string(m_identifier.c_str(), "flares"));
    tb_id = environment.eff_Thunderbolt->AppendDef_shoc(environment, pSettings, pSettings->r_string(m_identifier.c_str(), "thunderbolt"));
    bolt_period = (tb_id.size()) ? pSettings->r_float(m_identifier.c_str(), "bolt_period") : 0.f;
    bolt_duration = (tb_id.size()) ? pSettings->r_float(m_identifier.c_str(), "bolt_duration") : 0.f;
    env_ambient = pSettings->line_exist(m_identifier.c_str(), "env_ambient") ? environment.AppendEnvAmb(pSettings->r_string(m_identifier.c_str(), "env_ambient")) : nullptr;

    if (pSettings->line_exist(m_identifier.c_str(), "sun_shafts_intensity"))
        m_fSunShaftsIntensity = pSettings->r_float(m_identifier.c_str(), "sun_shafts_intensity");
    else
        m_fSunShaftsIntensity = 0.3f;

    if (pSettings->line_exist(m_identifier.c_str(), "water_intensity"))
        m_fWaterIntensity = pSettings->r_float(m_identifier.c_str(), "water_intensity");

    constexpr float def_min_TAI = 0.01f, def_max_TAI = 0.07f;
    const float def_TAI = def_min_TAI + (rain_density * (def_max_TAI - def_min_TAI)); //Если не прописано, дефолт будет рассчитываться от силы дождя.
    m_fTreeAmplitudeIntensity = READ_IF_EXISTS(pSettings, r_float, m_identifier.c_str(), "tree_amplitude_intensity", def_TAI);

    C_CHECK(clouds_color);
    C_CHECK(sky_color);
    C_CHECK(fog_color);
    C_CHECK(rain_color);
    C_CHECK(ambient);
    C_CHECK(hemi_color);
    C_CHECK(sun_color);

    on_device_create();
}

void CEnvDescriptor::on_device_create() { m_pDescriptor->OnDeviceCreate(*this); }

void CEnvDescriptor::on_device_destroy() { m_pDescriptor->OnDeviceDestroy(); }

void CEnvDescriptor::on_prepare() { m_pDescriptor->OnPrepare(*this); }

void CEnvDescriptor::on_unload() { m_pDescriptor->OnUnload(*this); }

void CEnvDescriptor::setEnvAmbient(LPCSTR sect, CEnvironment* parent) { env_ambient = parent->AppendEnvAmb(sect); }

//-----------------------------------------------------------------------------
// Environment Mixer
//-----------------------------------------------------------------------------
CEnvDescriptorMixer::CEnvDescriptorMixer(shared_str const& identifier) : CEnvDescriptor(identifier) {}

void CEnvDescriptorMixer::destroy()
{
    m_pDescriptorMixer->Destroy();

    //	Reuse existing code
    on_device_destroy();
}

void CEnvDescriptorMixer::clear() { m_pDescriptorMixer->Clear(); }

void CEnvDescriptorMixer::lerp(CEnvironment* env, CEnvDescriptor& A, CEnvDescriptor& B, float f, CEnvModifier& Mdf, float modifier_power)
{
    float modif_power = 1.f / (modifier_power + 1); // the environment itself
    float fi = 1 - f;

    m_pDescriptorMixer->lerp(&*A.m_pDescriptor, &*B.m_pDescriptor);

    weight = f;

    clouds_color.lerp(A.clouds_color, B.clouds_color, f);

    sky_rotation = (fi * A.sky_rotation + f * B.sky_rotation);

    //.	far_plane				=	(fi*A.far_plane + f*B.far_plane + Mdf.far_plane)*psVisDistance*modif_power;
    if (Mdf.use_flags.test(eViewDist))
        far_plane = (fi * A.far_plane + f * B.far_plane + Mdf.far_plane) * psVisDistance * modif_power;
    else
        far_plane = (fi * A.far_plane + f * B.far_plane) * psVisDistance;

    //.	fog_color.lerp			(A.fog_color,B.fog_color,f).add(Mdf.fog_color).mul(modif_power);
    fog_color.lerp(A.fog_color, B.fog_color, f);
    if (Mdf.use_flags.test(eFogColor))
        fog_color.add(Mdf.fog_color).mul(modif_power);

    //.	fog_density				=	(fi*A.fog_density + f*B.fog_density + Mdf.fog_density)*modif_power;
    fog_density = (fi * A.fog_density + f * B.fog_density);
    if (Mdf.use_flags.test(eFogDensity))
    {
        fog_density += Mdf.fog_density;
        fog_density *= modif_power;
    }

    fog_distance = (fi * A.fog_distance + f * B.fog_distance);
    clamp(fog_distance, 1.f, far_plane - 10);
    fog_near = (1.0f - fog_density) * 0.85f * fog_distance;
    fog_far = 0.99f * fog_distance;

    rain_density = fi * A.rain_density + f * B.rain_density;
    rain_color.lerp(A.rain_color, B.rain_color, f);
    bolt_period = fi * A.bolt_period + f * B.bolt_period;
    bolt_duration = fi * A.bolt_duration + f * B.bolt_duration;

    // wind
    wind_velocity_old = wind_velocity;
    wind_velocity = fi * A.wind_velocity + f * B.wind_velocity;

    wind_direction_old = wind_direction;
    wind_direction = fi * A.wind_direction + f * B.wind_direction;

    if (ps_r_sunshafts_intensity > 0.f)
        m_fSunShaftsIntensity = ps_r_sunshafts_intensity;
    else
        m_fSunShaftsIntensity = fi * A.m_fSunShaftsIntensity + f * B.m_fSunShaftsIntensity;

    m_fWaterIntensity = fi * A.m_fWaterIntensity + f * B.m_fWaterIntensity;

    m_fTreeAmplitudeIntensity_old = m_fTreeAmplitudeIntensity;
    m_fTreeAmplitudeIntensity = fi * A.m_fTreeAmplitudeIntensity + f * B.m_fTreeAmplitudeIntensity;

    bloom_threshold = fi * A.bloom_threshold + f * B.bloom_threshold;
    bloom_exposure = fi * A.bloom_exposure + f * B.bloom_exposure;
    bloom_sky_intensity = fi * A.bloom_sky_intensity + f * B.bloom_sky_intensity;

    // colors
    sky_color.lerp(A.sky_color, B.sky_color, f);
    if (Mdf.use_flags.test(eSkyColor))
        sky_color.add(Mdf.sky_color).mul(modif_power);

    ambient.lerp(A.ambient, B.ambient, f);
    if (Mdf.use_flags.test(eAmbientColor))
        ambient.add(Mdf.ambient).mul(modif_power);

    hemi_color.lerp(A.hemi_color, B.hemi_color, f);

    hemi_color_unmodded = hemi_color;

    if (Mdf.use_flags.test(eHemiColor))
    {
        hemi_color.x += Mdf.hemi_color.x;
        hemi_color.y += Mdf.hemi_color.y;
        hemi_color.z += Mdf.hemi_color.z;
        hemi_color.x *= modif_power;
        hemi_color.y *= modif_power;
        hemi_color.z *= modif_power;
    }

    sun_color.lerp(A.sun_color, B.sun_color, f);

	if (rain_density > 0.f)
        env->wetness_factor += (rain_density * puddles_wetting) / 10000.f;
    else
        env->wetness_factor -= 0.0001f * puddles_drying;

    clamp(env->wetness_factor, 0.f, 1.f);

    R_ASSERT(_valid(A.sun_dir));
    R_ASSERT(_valid(B.sun_dir));
    sun_dir.lerp(A.sun_dir, B.sun_dir, f).normalize();
    R_ASSERT(_valid(sun_dir));

    VERIFY(sun_dir.y < 0, "Invalid sun direction settings while lerp");
}

//-----------------------------------------------------------------------------
// Environment IO
//-----------------------------------------------------------------------------
CEnvAmbient* CEnvironment::AppendEnvAmb(const shared_str& sect)
{
    for (auto* it : Ambients)
        if (it->name().equal(sect))
            return it;

    auto& Amb = Ambients.emplace_back(xr_new<CEnvAmbient>());

    if (USED_COP_WEATHER)
    {
        Amb->load(*m_ambients_config, *m_sound_channels_config, *m_effects_config, sect);
    }
    else
    {
        Amb->load_shoc(sect);
    }
    return Amb;
}

void CEnvironment::mods_load()
{
    Modifiers.clear();

    string_path path;

    if (FS.exist(path, fsgame::level, fsgame::level_files::level_env_mod_ltx))
    {
        CInifile ltXfile = CInifile(path);

        for (const auto& it : ltXfile.sections())
        {
            CEnvModifier E;
            if (E.loadIni(ltXfile, it.first.c_str()))
                Modifiers.push_back(E);
        }
    }
    else if (FS.exist(path, fsgame::level, fsgame::level_files::level_env_mod))
    {
        IReader* fs = FS.r_open(path);

        {
            u32 id = 0;
            u32 ver = 0x0015;
            u32 sz;

            while (0 != (sz = fs->find_chunk(id)))
            {
                if (id == 0 && sz == sizeof(u32))
                {
                    ver = fs->r_u32();
                }
                else
                {
                    CEnvModifier E;
                    if (E.load(fs, ver))
                    {
                        Modifiers.push_back(E);
                    }
                }
                id++;
            }
        }

        FS.r_close(fs);

        if (!Modifiers.empty() && bLevelEnvModExport)
        {
            FS.update_path(path, fsgame::level, fsgame::level_files::level_env_mod_ltx);

            CInifile ltXfile{path, 0, 0, 1};
            u32 id{};

            for (auto& mod : Modifiers)
            {
                mod.saveIni(ltXfile, std::to_string(id++).c_str());
            }
        }
    }

    if (USED_COP_WEATHER)
        load_level_specific_ambients();
}

void CEnvironment::mods_unload() { Modifiers.clear(); }

void CEnvironment::load_level_specific_ambients()
{
    const shared_str level_name = g_pGameLevel->name();

    string_path path;
    strconcat(sizeof(path), path, "environment\\ambients\\", level_name.c_str(), ".ltx");

    string_path full_path;
    CInifile* level_ambients = xr_new<CInifile>(FS.update_path(full_path, fsgame::game_configs, path), TRUE, TRUE, FALSE);

    for (const auto& ambient : Ambients)
    {
        shared_str section_name = ambient->name();

        // choose a source ini file
        CInifile* source = (level_ambients && level_ambients->section_exist(section_name)) ? level_ambients : m_ambients_config;

        // check and reload if needed
        if (xr_strcmp(ambient->get_ambients_config_filename().c_str(), source->fname()))
        {
            ambient->destroy();
            ambient->load(*source, *m_sound_channels_config, *m_effects_config, section_name);
        }
        else
        {
#pragma todo("SIMP: Пересмотреть это! Пока убрал метод CreateAmbientParticle, в xrGame по этой части правки не вносились.")//
            // Все партиклы были уничтожены в `Disconnect`, поэтому нужно их
            // пересоздать.
            //for (auto& eff : ambient->effects())
            //    g_pGamePersistent->CreateAmbientParticle(eff);
        }
    }

    xr_delete(level_ambients);
}

CEnvDescriptor* CEnvironment::create_descriptor(shared_str const& identifier, CInifile* config)
{
    CEnvDescriptor* result = xr_new<CEnvDescriptor>(identifier);
    if (config)
        result->load(*this, *config);
    return result;
}

CEnvDescriptor* CEnvironment::create_descriptor_shoc(LPCSTR exec_tm, LPCSTR S)
{
    CEnvDescriptor* result = xr_new<CEnvDescriptor>();
    result->load_shoc(*this, exec_tm, S);

    return result;
}

void CEnvironment::load_sun()
{
    if (!m_static_sun_movement)
        return;

    int i = 0;
    do
    {
        char sun_identifier[10];
        sprintf(sun_identifier, i >= 10 ? "%d:00:00" : "0%d:00:00", i);

        float sun_alt = m_sun_pos_config->r_float(sun_identifier, "sun_altitude");
        float sun_long = m_sun_pos_config->r_float(sun_identifier, "sun_longitude");

        R_ASSERT(_valid(sun_alt));
        R_ASSERT(_valid(sun_long));
        sun_hp[i].set(sun_alt, sun_long);
        i++;
    } while (i < 24);
}

void CEnvironment::load_weathers()
{
    if (!WeatherCycles.empty())
        return;

    if (FS.path_exist(fsgame::game_weathers))
    {
        auto file_list = FS.file_list_open(fsgame::game_weathers, "");

        for (const char* file : *file_list)
        {
            const size_t length = strlen(file);
            ASSERT_FMT(length >= 4 && !strcmp(".ltx", file + (length - 4)), "Something strange with file [%s]", file);
            string256 identifier{};
            strncpy_s(identifier, file, length - 4);

            string_path file_name;
            FS.update_path(file_name, fsgame::game_weathers, file);
            CInifile config(file_name);

            EnvVec& env = WeatherCycles[identifier];
            auto& sections = config.sections();
            env.reserve(sections.size());

            for (const auto& pair : sections)
                env.push_back(create_descriptor(pair.second->Name, &config));
        }

        FS.file_list_close(file_list);
    }
    else
    {
        LPCSTR first_weather{};
        const u32 weather_count = pSettings->line_count("weathers");
        for (u32 w_idx{}; w_idx < weather_count; w_idx++)
        {
            LPCSTR weather, sect_w;
            if (pSettings->r_line("weathers", w_idx, &weather, &sect_w))
            {
                if (!first_weather)
                    first_weather = weather;
                u32 env_count = pSettings->line_count(sect_w);
                for (u32 env_idx{}; env_idx < env_count; env_idx++)
                {
                    LPCSTR exec_tm, sect_e;
                    if (pSettings->r_line(sect_w, env_idx, &exec_tm, &sect_e))
                        WeatherCycles[weather].push_back(create_descriptor_shoc(exec_tm, sect_e));
                }
            }
        }
    }

    // sorting weather envs
    for (auto& [k, v] : WeatherCycles)
    {
        R_ASSERT(v.size() > 1, "Environment in weather must >=2", k.c_str());
        std::sort(v.begin(), v.end(), sort_env_etl_pred);
    }

    R_ASSERT(!WeatherCycles.empty(), "Empty weathers.");

    SetWeather((*WeatherCycles.begin()).first.c_str());
}

void CEnvironment::load_weather_effects()
{
    if (!WeatherFXs.empty())
        return;

    if (USED_COP_WEATHER)
    {
        auto file_list = FS.file_list_open(fsgame::game_weather_effects, "");

        for (const char* file : *file_list)
        {
            const size_t length = strlen(file);
            ASSERT_FMT(length >= 4 && !strcmp(".ltx", file + (length - 4)), "Something strange with file [%s]", file);
            string256 identifier{};
            strncpy_s(identifier, file, length - 4);

            string_path file_name;
            FS.update_path(file_name, fsgame::game_weather_effects, file);
            CInifile config(file_name);
            auto& sections = config.sections();

            EnvVec& env = WeatherFXs[identifier];
            env.reserve(sections.size() + 2);
            env.push_back(create_descriptor("00:00:00", nullptr));

            for (const auto& pair : sections)
                env.push_back(create_descriptor(pair.second->Name, &config));

            env.emplace_back(create_descriptor("24:00:00", nullptr))->exec_time_loaded = DAY_LENGTH;
        }

        FS.file_list_close(file_list);
    }
    else
    {
        u32 line_count = pSettings->line_count("weather_effects");
        for (u32 w_idx{}; w_idx < line_count; w_idx++)
        {
            LPCSTR weather, sect_w;
            if (pSettings->r_line("weather_effects", w_idx, &weather, &sect_w))
            {
                EnvVec& env = WeatherFXs[weather];
                env.emplace_back(xr_new<CEnvDescriptor>())->exec_time_loaded = 0;
                u32 env_count = pSettings->line_count(sect_w);
                LPCSTR exec_tm, sect_e;
                for (u32 env_idx{}; env_idx < env_count; env_idx++)
                    if (pSettings->r_line(sect_w, env_idx, &exec_tm, &sect_e))
                        env.push_back(create_descriptor_shoc(exec_tm, sect_e));

                env.emplace_back(xr_new<CEnvDescriptor>())->exec_time_loaded = DAY_LENGTH;
            }
        }
    }

    // sorting weather envs
    for (auto& [k, v] : WeatherFXs)
    {
        R_ASSERT(v.size() > 1, "Environment in weather must >=2", k.c_str());
        std::sort(v.begin(), v.end(), sort_env_etl_pred);
    }
}

void CEnvironment::load()
{
    if (!CurrentEnv)
        create_mixer();

    if (!eff_Rain)
        eff_Rain = xr_new<CEffect_Rain>();
    if (!eff_LensFlare)
        eff_LensFlare = xr_new<CLensFlare>();
    if (!eff_Thunderbolt)
        eff_Thunderbolt = xr_new<CEffect_Thunderbolt>();

    load_weathers();
    load_weather_effects();
    load_sun();
}

void CEnvironment::unload()
{
    EnvsMapIt _I, _E;
    // clear weathers
    _I = WeatherCycles.begin();
    _E = WeatherCycles.end();
    for (; _I != _E; ++_I)
    {
        for (auto& it : _I->second)
            xr_delete(it);
    }

    WeatherCycles.clear();
    // clear weather effect
    _I = WeatherFXs.begin();
    _E = WeatherFXs.end();
    for (; _I != _E; ++_I)
    {
        for (auto& it : _I->second)
            xr_delete(it);
    }
    WeatherFXs.clear();
    // clear ambient
    for (auto& Ambient : Ambients)
        xr_delete(Ambient);

    Ambients.clear();
    // misc
    xr_delete(eff_Rain);
    xr_delete(eff_LensFlare);
    xr_delete(eff_Thunderbolt);
    CurrentWeather = nullptr;
    CurrentWeatherName = nullptr;
    CurrentEnv->clear();

    Invalidate();
}

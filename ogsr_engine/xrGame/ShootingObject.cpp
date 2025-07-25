//////////////////////////////////////////////////////////////////////
// ShootingObject.cpp:  интерфейс для семейства стреляющих объектов
//						(оружие и осколочные гранаты)
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ShootingObject.h"

#include "ParticlesObject.h"
#include "WeaponAmmo.h"

#include "actor.h"
#include "game_cl_base.h"
#include "level.h"
#include "level_bullet_manager.h"
#include "clsid_game.h"
#include "game_cl_single.h"

#define HIT_POWER_EPSILON 0.05f
#define WALLMARK_SIZE 0.04f

CShootingObject::CShootingObject(void)
{
    fTime = 0;
    fTimeToFire = 0;
    // fHitPower						= 0.0f;
    fvHitPower.set(0.0f, 0.0f, 0.0f, 0.0f);
    m_fStartBulletSpeed = 1000.f;

    m_iCurrentParentID = 0xFFFF;

    m_fPredBulletTime = 0.0f;
    m_bUseAimBullet = false;
    m_fTimeToAim = 0.0f;

    // particles
    m_sFlameParticlesCurrent = m_sFlameParticles = NULL;
    m_sSmokeParticlesCurrent = m_sSmokeParticles = NULL;
    m_sShellParticles = NULL;
    m_bForcedParticlesHudMode = false;
    m_bParticlesHudMode = false;

    bWorking = false;

    light_render = 0;

    reinit();
}
CShootingObject::~CShootingObject(void) {}

void CShootingObject::reinit() { m_pFlameParticles = NULL; }

void CShootingObject::Load(LPCSTR section)
{
    if (pSettings->line_exist(section, "light_disabled"))
    {
        m_bLightShotEnabled = !pSettings->r_bool(section, "light_disabled");
    }
    else
        m_bLightShotEnabled = true;

    //время затрачиваемое на выстрел
    fTimeToFire = pSettings->r_float(section, "rpm");
    VERIFY(fTimeToFire > 0.f);
    // Alundaio: Two-shot burst rpm; used for Abakan/AN-94
    fTimeToFire2 = READ_IF_EXISTS(pSettings, r_float, section, "rpm_mode_2", fTimeToFire);
    VERIFY(fTimeToFire2 > 0.f);
    fTimeToFire = 60.f / fTimeToFire;
    fTimeToFire2 = 60.f / fTimeToFire2;

    // Cycle down RPM after first 2 shots; used for Abakan/AN-94
    bCycleDown = !!READ_IF_EXISTS(pSettings, r_bool, section, "cycle_down", false);
    // Alundaio: END

    m_bForcedParticlesHudMode = !!pSettings->line_exist(section, "forced_particle_hud_mode");
    if (m_bForcedParticlesHudMode)
        m_bParticlesHudMode = !!pSettings->r_bool(section, "forced_particle_hud_mode");

    LoadFireParams(section, "");
    LoadLights(section, "");
    LoadShellParticles(section, "");
    LoadFlameParticles(section, "");
}

void CShootingObject::Light_Create()
{
    // lights
    light_render = ::Render->light_create();
    light_render->set_moveable(true);
}

void CShootingObject::Light_Destroy() { light_render.destroy(); }

void CShootingObject::LoadFireParams(LPCSTR section, LPCSTR prefix)
{
    string256 full_name;
    string32 buffer;
    shared_str s_sHitPower;
    //базовая дисперсия оружия
    fireDispersionBase = pSettings->r_float(section, "fire_dispersion_base");
    fireDispersionBase = deg2rad(fireDispersionBase);
    constDeviation.pitch = READ_IF_EXISTS(pSettings, r_float, section, "const_deviation_pitch", 0);
    constDeviation.yaw = READ_IF_EXISTS(pSettings, r_float, section, "const_deviation_yaw", 0);

    LPCSTR hit_type = READ_IF_EXISTS(pSettings, r_string, section, "hit_type", "fire_wound");
    m_eHitType = ALife::g_tfString2HitType(hit_type); // поддержка произвольного хита оружия
    //сила выстрела и его мощьность
    s_sHitPower = pSettings->r_string_wb(section, strconcat(sizeof(full_name), full_name, prefix, "hit_power")); //читаем строку силы хита пули оружия
    fvHitPower[egdMaster] = (float)atof(_GetItem(*s_sHitPower, 0, buffer)); //первый параметр - это хит для уровня игры мастер

    fvHitPower[egdVeteran] = fvHitPower[egdMaster]; //изначально параметры для других уровней
    fvHitPower[egdStalker] = fvHitPower[egdMaster]; //сложности
    fvHitPower[egdNovice] = fvHitPower[egdMaster]; //такие же

    int num_game_diff_param = _GetItemCount(*s_sHitPower); //узнаём колличество параметров для хитов
    if (num_game_diff_param > 1) //если задан второй параметр хита
    {
        fvHitPower[egdVeteran] = (float)atof(_GetItem(*s_sHitPower, 1, buffer)); //то вычитываем его для уровня ветерана
    }
    if (num_game_diff_param > 2) //если задан третий параметр хита
    {
        fvHitPower[egdStalker] = (float)atof(_GetItem(*s_sHitPower, 2, buffer)); //то вычитываем его для уровня сталкера
    }
    if (num_game_diff_param > 3) //если задан четвёртый параметр хита
    {
        fvHitPower[egdNovice] = (float)atof(_GetItem(*s_sHitPower, 3, buffer)); //то вычитываем его для уровня новичка
    }

    // fHitPower			= pSettings->r_float	(section,strconcat(full_name, prefix, "hit_power"));
    fHitImpulse = pSettings->r_float(section, strconcat(sizeof(full_name), full_name, prefix, "hit_impulse"));
    //максимальное расстояние полета пули
    fireDistance = pSettings->r_float(section, strconcat(sizeof(full_name), full_name, prefix, "fire_distance"));
    //начальная скорость пули
    m_fStartBulletSpeed = pSettings->r_float(section, strconcat(sizeof(full_name), full_name, prefix, "bullet_speed"));
    m_bUseAimBullet = pSettings->r_bool(section, strconcat(sizeof(full_name), full_name, prefix, "use_aim_bullet"));
    if (m_bUseAimBullet)
    {
        m_fTimeToAim = pSettings->r_float(section, strconcat(sizeof(full_name), full_name, prefix, "time_to_aim"));
    }
}

void CShootingObject::LoadLights(LPCSTR section, LPCSTR prefix)
{
    string256 full_name;
    // light
    if (m_bLightShotEnabled)
    {
        Fvector clr = pSettings->r_fvector3(section, strconcat(sizeof(full_name), full_name, prefix, "light_color"));
        light_base_color.set(clr.x, clr.y, clr.z, 1);
        light_base_range = pSettings->r_float(section, strconcat(sizeof(full_name), full_name, prefix, "light_range"));
        light_var_color = pSettings->r_float(section, strconcat(sizeof(full_name), full_name, prefix, "light_var_color"));
        light_var_range = pSettings->r_float(section, strconcat(sizeof(full_name), full_name, prefix, "light_var_range"));
        light_lifetime = pSettings->r_float(section, strconcat(sizeof(full_name), full_name, prefix, "light_time"));
        light_time = -1.f;
    }
}

void CShootingObject::Light_Start()
{
    if (!light_render)
        Light_Create();

    if (Device.dwFrame != light_start_frame)
    {
        light_start_frame = Device.dwFrame;
        light_time = light_lifetime;

        light_build_color.set(
            Random.randFs(light_var_color, light_base_color.r), 
            Random.randFs(light_var_color, light_base_color.g),
            Random.randFs(light_var_color, light_base_color.b), 
            1);
        light_build_range = Random.randFs(light_var_range, light_base_range);
    }
}

void CShootingObject::Light_Update(const Fvector& P)
{
    if (Device.dwFrame != light_update_frame)
    {
        light_update_frame = Device.dwFrame;
        const float light_scale = light_time / light_lifetime;

        //R_ASSERT(light_render);
        light_render->set_position(P);
        light_render->set_color(light_build_color.r * light_scale, light_build_color.g * light_scale, light_build_color.b * light_scale);
        light_render->set_range(light_build_range * light_scale);

        light_render->set_shadow(!Core.Features.test(xrCore::Feature::npc_simplified_shooting) || ParentIsActor());

        if (!light_render->get_active())
        {
            light_render->set_active(true);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// Particles
//////////////////////////////////////////////////////////////////////////

void CShootingObject::StartParticles(CParticlesObject*& pParticles, LPCSTR particles_name, const Fvector& pos, const Fvector& vel, bool auto_remove_flag)
{
    if (!particles_name)
        return;

    if (pParticles != NULL)
    {
        UpdateParticles(pParticles, pos, vel);
        return;
    }

    pParticles = CParticlesObject::Create(particles_name, (BOOL)auto_remove_flag);

    UpdateParticles(pParticles, pos, vel);
    BOOL hudMode = IsHudModeNow() && m_bParticlesHudMode;
    pParticles->Play(hudMode);
}

void CShootingObject::StopParticles(CParticlesObject*& pParticles)
{
    if (pParticles == NULL)
        return;

    pParticles->Stop();
    CParticlesObject::Destroy(pParticles);
}

void CShootingObject::UpdateParticles(CParticlesObject*& pParticles, const Fvector& pos, const Fvector& vel)
{
    if (!pParticles)
        return;

    Fmatrix particles_pos;
    particles_pos.set(get_ParticlesXFORM());
    particles_pos.c.set(pos);

    pParticles->SetXFORM(particles_pos);

    if (!pParticles->IsAutoRemove() && !pParticles->IsLooped() && !pParticles->PSI_alive())
    {
        pParticles->Stop();
        CParticlesObject::Destroy(pParticles);
    }
}

void CShootingObject::LoadShellParticles(LPCSTR section, LPCSTR prefix)
{
    string256 full_name;
    strconcat(sizeof(full_name), full_name, prefix, "shell_particles");

    if (pSettings->line_exist(section, full_name))
    {
        m_sShellParticles = pSettings->r_string(section, full_name);
        vLoadedShellPoint = pSettings->r_fvector3(section, strconcat(sizeof(full_name), full_name, prefix, "shell_point"));
    }
}

void CShootingObject::LoadFlameParticles(LPCSTR section, LPCSTR prefix)
{
    string256 full_name;

    // flames
    strconcat(sizeof(full_name), full_name, prefix, "flame_particles");
    if (pSettings->line_exist(section, full_name))
        m_sFlameParticles = pSettings->r_string(section, full_name);

    strconcat(sizeof(full_name), full_name, prefix, "smoke_particles");
    if (pSettings->line_exist(section, full_name))
        m_sSmokeParticles = pSettings->r_string(section, full_name);

    //текущие партиклы
    m_sFlameParticlesCurrent = m_sFlameParticles;
    m_sSmokeParticlesCurrent = m_sSmokeParticles;
}

#include "weapon.h"
#include "script_callback_ex.h"
#include "script_game_object.h"
#include "game_object_space.h"
void CShootingObject::OnShellDrop(const Fvector& play_pos, const Fvector& parent_vel)
{
    if (ParentIsActor())
    {
        auto wpn = smart_cast<CGameObject*>(this);
        if (wpn)
            Actor()->callback(GameObject::eOnWpnShellDrop)(wpn->lua_game_object(), play_pos, parent_vel);
    }
    else if (Core.Features.test(xrCore::Feature::npc_simplified_shooting))
        return;

    if (!m_sShellParticles)
        return;
    if (Device.vCameraPosition.distance_to_sqr(play_pos) > 2 * 2)
        return;

    CParticlesObject* pShellParticles = CParticlesObject::Create(*m_sShellParticles, TRUE);

    Fmatrix particles_pos;
    particles_pos.set(get_ParticlesXFORM());
    particles_pos.c.set(play_pos);

    pShellParticles->UpdateParent(particles_pos, parent_vel);
    BOOL hudMode = IsHudModeNow() && m_bParticlesHudMode;
    pShellParticles->Play(hudMode);
}

//партиклы дыма
void CShootingObject::StartSmokeParticles(const Fvector& play_pos, const Fvector& parent_vel)
{
    if (!ParentIsActor() && Core.Features.test(xrCore::Feature::npc_simplified_shooting))
        return;
    CParticlesObject* pSmokeParticles = NULL;
    StartParticles(pSmokeParticles, *m_sSmokeParticlesCurrent, play_pos, parent_vel, true);
}

void CShootingObject::StartFlameParticles()
{
    if (0 == m_sFlameParticlesCurrent.size())
        return;

    //если партиклы циклические
    if (m_pFlameParticles && m_pFlameParticles->IsLooped() && m_pFlameParticles->IsPlaying())
    {
        UpdateFlameParticles();
        return;
    }

    StopFlameParticles();
    m_pFlameParticles = CParticlesObject::Create(*m_sFlameParticlesCurrent, FALSE);
    UpdateFlameParticles();
    BOOL hudMode = IsHudModeNow() && m_bParticlesHudMode;
    m_pFlameParticles->Play(hudMode);
}
void CShootingObject::StopFlameParticles()
{
    if (0 == m_sFlameParticlesCurrent.size())
        return;
    if (m_pFlameParticles == NULL)
        return;

    m_pFlameParticles->SetAutoRemove(true);
    m_pFlameParticles->Stop();
    m_pFlameParticles = NULL;
}

void CShootingObject::UpdateFlameParticles()
{
    if (0 == m_sFlameParticlesCurrent.size())
        return;
    if (!m_pFlameParticles)
        return;

    Fmatrix pos;
    pos.set(get_ParticlesXFORM());
    pos.c.set(get_CurrentFirePoint());

    m_pFlameParticles->SetXFORM(pos);

    if (!m_pFlameParticles->IsLooped() && !m_pFlameParticles->IsPlaying() && !m_pFlameParticles->PSI_alive())
    {
        m_pFlameParticles->Stop();
        CParticlesObject::Destroy(m_pFlameParticles);
    }
}

//подсветка от выстрела
void CShootingObject::UpdateLight()
{
    if (light_render && light_time > 0)
    {
        light_time -= Device.fTimeDelta;
        if (light_time <= 0)
            StopLight();
    }
}

void CShootingObject::StopLight()
{
    if (light_render)
    {
        light_render->set_active(false);
    }
}

void CShootingObject::RenderLight()
{
    if (light_render && light_time > 0)
    {
        Light_Update(get_CurrentFirePoint());
    }
}

bool CShootingObject::SendHitAllowed(CObject* pUser)
{
    if (Game().IsServerControlHits())
        return true;

    {
        if (pUser->CLS_ID == CLSID_OBJECT_ACTOR)
        {
            if (Level().CurrentControlEntity() != pUser)
            {
                return false;
            }
        }
        return true;
    }
};

void CShootingObject::FireBullet(const Fvector& pos, const Fvector& shot_dir, float fire_disp, const CCartridge& cartridge, u16 parent_id, u16 weapon_id, bool send_hit)
{
    Fvector dir;
    dir.random_dir(shot_dir, fire_disp);

    if (!Core.Features.test(xrCore::Feature::npc_simplified_shooting) || ParentIsActor())
        if (!fis_zero(constDeviation.pitch) || !fis_zero(constDeviation.yaw))
        {
            // WARN: при больших значениях девиации стрелок может отсрелить себе голову!
            float dir_yaw, dir_pitch;
            dir.getHP(dir_yaw, dir_pitch);
            dir_pitch += constDeviation.pitch;
            dir_yaw += constDeviation.yaw;
            dir.setHP(dir_yaw, dir_pitch);
        }

    m_iCurrentParentID = parent_id;

    bool aim_bullet;
    if (m_bUseAimBullet)
    {
        if (ParentIsActor())
        {
            if (m_fPredBulletTime == 0.0)
            {
                aim_bullet = true;
            }
            else
            {
                if ((Device.fTimeGlobal - m_fPredBulletTime) >= m_fTimeToAim)
                {
                    aim_bullet = true;
                }
                else
                {
                    aim_bullet = false;
                }
            }
        }
        else
        {
            aim_bullet = false;
        }
    }
    else
    {
        aim_bullet = false;
    }
    m_fPredBulletTime = Device.fTimeGlobal;

    float l_fHitPower;
    if (ParentIsActor()) //если из оружия стреляет актёр(игрок)
    {
        l_fHitPower = fvHitPower[g_SingleGameDifficulty];
    }
    else
    {
        l_fHitPower = fvHitPower[egdMaster];
    }

    Level().BulletManager().AddBullet(pos, dir, m_fStartBulletSpeed, l_fHitPower, fHitImpulse, parent_id, weapon_id, m_eHitType, fireDistance, cartridge, send_hit, aim_bullet);
}

void CShootingObject::FireStart() { bWorking = true; }
void CShootingObject::FireEnd() { bWorking = false; }
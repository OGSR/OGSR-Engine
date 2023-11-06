// Level_Bullet_Manager.cpp:	для обеспечения полета пули по траектории
//								все пули и осколки передаются сюда
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Level.h"
#include "Level_Bullet_Manager.h"
#include "game_cl_base.h"
#include "Actor.h"
#include "gamepersistent.h"
#include "Weapon.h"
#include "mt_config.h"

#ifdef DEBUG
#include "debug_renderer.h"
#endif

#define HIT_POWER_EPSILON 0.05f
#define WALLMARK_SIZE 0.04f

float CBulletManager::m_fMinBulletSpeed = 2.f;

SBullet::SBullet() : m_on_bullet_hit(false) {}

SBullet::~SBullet() {}

void SBullet::Init(const Fvector& position, const Fvector& direction, float starting_speed, float power, float impulse, u16 sender_id, u16 sendersweapon_id,
                   ALife::EHitType e_hit_type, float maximum_distance, const CCartridge& cartridge, bool SendHit)
{
    flags._storage = 0;
    pos = position;
    speed = max_speed = starting_speed * cartridge.m_kSpeed;
    VERIFY(speed > 0);

    VERIFY(direction.magnitude() > 0);
    dir.normalize(direction);

    hit_power = power * cartridge.m_kHit;
    hit_impulse = impulse * cartridge.m_kImpulse;

    max_dist = maximum_distance * cartridge.m_kDist;
    fly_dist = 0;

    parent_id = sender_id;
    flags.allow_sendhit = SendHit;
    weapon_id = sendersweapon_id;
    hit_type = e_hit_type;

    pierce = cartridge.m_kPierce;
    ap = cartridge.m_kAP;
    air_resistance = cartridge.m_kAirRes;
    wallmark_size = cartridge.fWallmarkSize;
    m_u8ColorID = cartridge.m_u8ColorID;

    bullet_material_idx = cartridge.bullet_material_idx;
    VERIFY(u16(-1) != bullet_material_idx);

    flags.allow_tracer = !!cartridge.m_flags.test(CCartridge::cfTracer);
    flags.allow_ricochet = !!cartridge.m_flags.test(CCartridge::cfRicochet);
    flags.explosive = !!cartridge.m_flags.test(CCartridge::cfExplosive);
    flags.skipped_frame = 0;

    targetID = 0;
    m_ExplodeParticles = cartridge.m_ExplodeParticles;
}

//////////////////////////////////////////////////////////
//

CBulletManager::CBulletManager()
{
    m_Bullets.reserve(1000);
    m_dwTimeRemainder = 0;
}

CBulletManager::~CBulletManager()
{
    m_Bullets.clear();
    m_WhineSounds.clear();
    m_Events.clear();
}

#define BULLET_MANAGER_SECTION "bullet_manager"

void CBulletManager::Load()
{
    m_fTracerWidth = pSettings->r_float(BULLET_MANAGER_SECTION, "tracer_width");
    m_fTracerLengthMax = pSettings->r_float(BULLET_MANAGER_SECTION, "tracer_length_max");
    m_fTracerLengthMin = pSettings->r_float(BULLET_MANAGER_SECTION, "tracer_length_min");

    m_fGravityConst = pSettings->r_float(BULLET_MANAGER_SECTION, "gravity_const");
    m_dwStepTime = pSettings->r_u32(BULLET_MANAGER_SECTION, "time_step");
    m_fMinBulletSpeed = pSettings->r_float(BULLET_MANAGER_SECTION, "min_bullet_speed");
    m_fCollisionEnergyMin = pSettings->r_float(BULLET_MANAGER_SECTION, "collision_energy_min");
    m_fCollisionEnergyMax = pSettings->r_float(BULLET_MANAGER_SECTION, "collision_energy_max");

    m_fHPMaxDist = pSettings->r_float(BULLET_MANAGER_SECTION, "hit_probability_max_dist");

    LPCSTR whine_sounds = pSettings->r_string(BULLET_MANAGER_SECTION, "whine_sounds");
    int cnt = _GetItemCount(whine_sounds);
    xr_string tmp;
    for (int k = 0; k < cnt; ++k)
    {
        m_WhineSounds.emplace_back();
        m_WhineSounds.back().create(_GetItem(whine_sounds, k, tmp), st_Effect, sg_SourceType);
    }

    LPCSTR explode_particles = pSettings->r_string(BULLET_MANAGER_SECTION, "explode_particles");
    cnt = _GetItemCount(explode_particles);
    for (int k = 0; k < cnt; ++k)
        m_ExplodeParticles.emplace_back(_GetItem(explode_particles, k, tmp));
}

void CBulletManager::PlayExplodePS(const Fmatrix& xf, RStringVec& m_ExplodeParticles)
{
    if (!m_ExplodeParticles.empty())
    {
        const shared_str& ps_name = m_ExplodeParticles[Random.randI(0, m_ExplodeParticles.size())];

        CParticlesObject* ps = CParticlesObject::Create(*ps_name, TRUE);
        ps->UpdateParent(xf, zero_vel);
        GamePersistent().ps_needtoplay.push_back(ps);
    }
}

void CBulletManager::PlayWhineSound(SBullet* bullet, CObject* object, const Fvector& pos)
{
    if (m_WhineSounds.empty())
        return;
    if (bullet->m_whine_snd._feedback() != NULL)
        return;
    if (bullet->hit_type != ALife::eHitTypeFireWound)
        return;

    bullet->m_whine_snd = m_WhineSounds[Random.randI(0, m_WhineSounds.size())];
    bullet->m_whine_snd.play_at_pos(object, pos);
}

void CBulletManager::Clear() //Вызывается при загрузке и дестрое уровня
{
    m_Bullets.clear();
    m_Events.clear();
}

SBullet& CBulletManager::AddBullet(const Fvector& position, const Fvector& direction, float starting_speed, float power, float impulse, u16 sender_id, u16 sendersweapon_id,
                                   ALife::EHitType e_hit_type, float maximum_distance, const CCartridge& cartridge, bool SendHit, bool AimBullet)
{
    VERIFY(u16(-1) != cartridge.bullet_material_idx);
    //	u32 CurID = Level().CurrentControlEntity()->ID();
    //	u32 OwnerID = sender_id;
    m_Bullets.emplace_back();
    SBullet& bullet = m_Bullets.back();
    bullet.Init(position, direction, starting_speed, power, impulse, sender_id, sendersweapon_id, e_hit_type, maximum_distance, cartridge, SendHit);
    bullet.frame_num = Device.dwFrame;
    bullet.flags.aim_bullet = AimBullet;
    return bullet;
}

void CBulletManager::UpdateWorkload()
{
    u32 delta_time = Device.dwTimeDelta + m_dwTimeRemainder;
    u32 step_num = delta_time / m_dwStepTime;
    m_dwTimeRemainder = delta_time % m_dwStepTime;

    rq_storage.r_clear();
    rq_spatial.clear();

    int k = m_Bullets.size() - 1;

    for (; k >= 0; k--)
    {
        SBullet& bullet = m_Bullets[k];
        //для пули пущенной на этом же кадре считаем только 1 шаг
        //(хотя по теории вообще ничего считать на надо)
        //который пропустим на следующем кадре,
        //это делается для того чтоб при скачках FPS не промазать
        //с 2х метров
        u32 cur_step_num = step_num;

        u32 frames_pass = Device.dwFrame - bullet.frame_num;
        if (frames_pass == 0)
            cur_step_num = 1;
        else if (frames_pass == 1 && step_num > 0)
            cur_step_num -= 1;

        // calculate bullet
        for (u32 i = 0; i < cur_step_num; i++)
        {
            if (!CalcBullet(rq_storage, rq_spatial, &bullet, m_dwStepTime))
            {
                collide::rq_result res;
                RegisterEvent(EVENT_REMOVE, FALSE, &bullet, Fvector().set(0, 0, 0), res, (u16)k);
                break;
            }
        }
    }
}

bool CBulletManager::CalcBullet(collide::rq_results& rq_storage, xr_vector<ISpatial*>& rq_spatial, SBullet* bullet, u32 delta_time)
{
    VERIFY(bullet);

    float delta_time_sec = float(delta_time) / 1000.f;
    float range = bullet->speed * delta_time_sec;

    float max_range = bullet->max_dist - bullet->fly_dist;
    if (range > max_range)
        range = max_range;

    //запомнить текущую скорость пули, т.к. в
    // RayQuery() она может поменяться из-за рикошетов
    //и столкновений с объектами
    Fvector cur_dir = bullet->dir;
    bullet_test_callback_data bullet_data;
    bullet_data.pBullet = bullet;
    bullet_data.bStopTracing = true;

    bullet->flags.ricochet_was = 0;

    collide::ray_defs RD(bullet->pos, bullet->dir, range, CDB::OPT_CULL, collide::rqtBoth);
    BOOL result = FALSE;
    VERIFY(!fis_zero(RD.dir.square_magnitude()));
    result = Level().ObjectSpace.RayQuery(rq_storage, RD, firetrace_callback, &bullet_data, test_callback, NULL);

    if (result && bullet_data.bStopTracing)
    {
        range = (rq_storage.r_begin() + rq_storage.r_count() - 1)->range;
    }
    range = std::max(EPS_L, range);

    bullet->flags.skipped_frame = (Device.dwFrame >= bullet->frame_num);

    if (!bullet->flags.ricochet_was)
    {
        //изменить положение пули
        bullet->pos.mad(bullet->pos, cur_dir, range);
        bullet->fly_dist += range;

        if (bullet->fly_dist >= bullet->max_dist)
            return false;

        Fbox level_box = Level().ObjectSpace.GetBoundingVolume();

        /*		if(!level_box.contains(bullet->pos))
                    return false;
        */
        if (!((bullet->pos.x >= level_box.x1) && (bullet->pos.x <= level_box.x2) && (bullet->pos.y >= level_box.y1) &&
              //			 (bullet->pos.y<=level_box.y2) &&
              (bullet->pos.z >= level_box.z1) && (bullet->pos.z <= level_box.z2)))
            return false;

        //изменить скорость и направление ее полета
        //с учетом гравитации
        bullet->dir.mul(bullet->speed);

        Fvector air_resistance = bullet->dir;
        air_resistance.mul(-bullet->air_resistance * delta_time_sec);
        ///		Msg("Speed - %f; ar - %f, %f", bullet->dir.magnitude(), air_resistance.magnitude(), air_resistance.magnitude()/bullet->dir.magnitude()*100);

        bullet->dir.add(air_resistance);
        bullet->dir.y -= m_fGravityConst * delta_time_sec;

        bullet->speed = bullet->dir.magnitude();
        VERIFY(_valid(bullet->speed));
        VERIFY(!fis_zero(bullet->speed));
        //вместо normalize(),	 чтоб не считать 2 раза magnitude()
#pragma todo("а как насчет bullet->speed==0")
        bullet->dir.x /= bullet->speed;
        bullet->dir.y /= bullet->speed;
        bullet->dir.z /= bullet->speed;
    }

    if (bullet->speed < m_fMinBulletSpeed)
        return false;

    return true;
}

#ifdef DEBUG
BOOL g_bDrawBulletHit = FALSE;
#endif

float SqrDistancePointToSegment(const Fvector& pt, const Fvector& orig, const Fvector& dir)
{
    Fvector diff;
    diff.sub(pt, orig);
    float fT = diff.dotproduct(dir);

    if (fT <= 0.0f)
    {
        fT = 0.0f;
    }
    else
    {
        float fSqrLen = dir.square_magnitude();
        if (fT >= fSqrLen)
        {
            fT = 1.0f;
            diff.sub(dir);
        }
        else
        {
            fT /= fSqrLen;
            diff.sub(Fvector().mul(dir, fT));
        }
    }

    return diff.square_magnitude();
}

void CBulletManager::Render()
{
#ifdef DEBUG
    // 0-рикошет
    // 1-застрявание пули в материале
    // 2-пробивание материала
    if (g_bDrawBulletHit)
    {
        extern FvectorVec g_hit[];
        FvectorIt it;
        u32 C[3] = {0xffff0000, 0xff00ff00, 0xff0000ff};
        RCache.set_xform_world(Fidentity);
        for (int i = 0; i < 3; ++i)
            for (it = g_hit[i].begin(); it != g_hit[i].end(); ++it)
            {
                Level().debug_renderer().draw_aabb(*it, 0.01f, 0.01f, 0.01f, C[i]);
            }
    }
#endif

    if (m_BulletsRendered.empty())
        return;

    u32 bullet_num = m_BulletsRendered.size();

    UIRender->StartPrimitive((u32)bullet_num * 12, IUIRender::ptTriList, IUIRender::pttLIT);

    for (auto& bullet : m_BulletsRendered)
    {
        if (!bullet.flags.allow_tracer)
            continue;
        if (!bullet.flags.skipped_frame)
            continue;

        float length = bullet.speed * float(m_dwStepTime) / 1000.f; // dist.magnitude();

        if (length < m_fTracerLengthMin)
            continue;

        if (length > m_fTracerLengthMax)
            length = m_fTracerLengthMax;

        float width = m_fTracerWidth;
        float dist2segSqr = SqrDistancePointToSegment(Device.vCameraPosition, bullet.pos, Fvector().mul(bullet.dir, length));
        //---------------------------------------------
        float MaxDistSqr = 1.0f;
        float MinDistSqr = 0.09f;
        if (dist2segSqr < MaxDistSqr)
        {
            if (dist2segSqr < MinDistSqr)
                dist2segSqr = MinDistSqr;

            width *= _sqrt(dist2segSqr / MaxDistSqr); //*MaxDistWidth/0.08f;
        }
        if (Device.vCameraPosition.distance_to_sqr(bullet.pos) < (length * length))
        {
            length = Device.vCameraPosition.distance_to(bullet.pos) - 0.3f;
        }
        /*
        //---------------------------------------------
        Fvector vT, v0, v1;
        vT.mad(Device.vCameraPosition, Device.vCameraDirection, _sqrt(dist2segSqr));
        v0.mad(vT, Device.vCameraTop, width*.5f);
        v1.mad(vT, Device.vCameraTop, -width*.5f);
        Fvector v0r, v1r;
        Device.mFullTransform.transform(v0r, v0);
        Device.mFullTransform.transform(v1r, v1);
        float ViewWidth = v1r.distance_to(v0r);
*/
        //		float dist = _sqrt(dist2segSqr);
        //		Msg("dist - [%f]; ViewWidth - %f, [%f]", dist, ViewWidth, ViewWidth*float(Device.dwHeight));
        //		Msg("dist - [%f]", dist);
        //---------------------------------------------

        Fvector center;
        center.mad(bullet.pos, bullet.dir, -length * .5f);
        tracers.Render(bullet.pos, center, bullet.dir, length, width, bullet.m_u8ColorID);
    }

    UIRender->CacheSetCullMode(IUIRender::cmNONE);
    UIRender->CacheSetXformWorld(Fidentity);
    UIRender->SetShader(*tracers.sh_Tracer);
    UIRender->FlushPrimitive();
    UIRender->CacheSetCullMode(IUIRender::cmCCW);
}

void CBulletManager::CommitRenderSet() // @ the end of frame
{
    m_BulletsRendered = m_Bullets;
    // Msg("!![%s] size of m_BulletsRendered: [%u], m_Bullets: [%u], m_events: [%u]", __FUNCTION__, m_BulletsRendered.size(), m_Bullets.size(), m_Events.size());
    if (g_mt_config.test(mtBullets))
        Device.add_to_seq_parallel(fastdelegate::MakeDelegate(this, &CBulletManager::UpdateWorkload));
    else
        CBulletManager::UpdateWorkload();
}
void CBulletManager::CommitEvents() // @ the start of frame
{
    for (auto& E : m_Events)
    {
        switch (E.Type)
        {
        case EVENT_HIT: {
            if (E.dynamic)
                DynamicObjectHit(E);
            else
                StaticObjectHit(E);
            if (E.bullet.isOnBulletHit())
            {
                CObject* O = Level().Objects.net_Find(E.bullet.weapon_id);
                if (O)
                {
                    CWeapon* W = smart_cast<CWeapon*>(O);
                    if (W)
                        W->OnBulletHit();
                }
            }
        }
        break;
        case EVENT_REMOVE: {
            m_Bullets[E.tgt_material] = m_Bullets.back();
            m_Bullets.pop_back();
        }
        break;
        }
    }
    m_Events.clear();

    if (m_Bullets.empty())
        m_dwTimeRemainder = 0;
}

void CBulletManager::RegisterEvent(EventType Type, BOOL _dynamic, SBullet* bullet, const Fvector& end_point, collide::rq_result& R, u16 tgt_material)
{
    m_Events.emplace_back();
    _event& E = m_Events.back();
    E.Type = Type;
    E.bullet = *bullet;

    switch (Type)
    {
    case EVENT_HIT: {
        E.dynamic = _dynamic;
        E.result = ObjectHit(bullet, end_point, R, tgt_material, E.normal);
        E.point = end_point;
        E.R = R;
        E.tgt_material = tgt_material;
        if (_dynamic)
        {
            //	E.Repeated = (R.O->ID() == E.bullet.targetID);
            //	bullet->targetID = R.O->ID();

            E.Repeated = (R.O->ID() == E.bullet.targetID);
            bullet->targetID = R.O->ID();
        }
    }
    break;
    case EVENT_REMOVE: {
        E.tgt_material = tgt_material;
    }
    break;
    }
}

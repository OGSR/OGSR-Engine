////////////////////////////////////////////////////////////////////////////
//	Module 		: movement_manager_physic.cpp
//	Created 	: 03.12.2003
//  Modified 	: 03.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Movement manager : physic character movement
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "movement_manager.h"
#include "PHMovementControl.h"
#include "detail_path_manager.h"
#include "level.h"
#include "custommonster.h"
#include "IColisiondamageInfo.h"
#include "profiler.h"

// Lain: added
#include "steering_behaviour.h"

#ifdef DEBUG
#include "PHDebug.h"
#define DBG_PH_MOVE_CONDITIONS(c) c

#else
#define DBG_PH_MOVE_CONDITIONS(c)
#endif

#define DISTANCE_PHISICS_ENABLE_CHARACTERS 2.f

float CMovementManager::speed(CPHMovementControl* movement_control) const
{
    VERIFY(movement_control);
    if (fis_zero(m_speed))
        return (0.f);

    if (movement_control->IsCharacterEnabled())
        return (movement_control->GetXZActVelInGoingDir());

    return (m_speed);
}

void CMovementManager::apply_collision_hit(CPHMovementControl* movement_control)
{
    VERIFY(movement_control);
    if (object().g_Alive() && !fsimilar(0.f, movement_control->gcontact_HealthLost))
    {
        const ICollisionDamageInfo* di = movement_control->CollisionDamageInfo();
        VERIFY(di);
        Fvector dir;
        di->HitDir(dir);

        CObject* who = di->DamageInitiator();
        SHit HDS = SHit(movement_control->gcontact_HealthLost, dir, who, movement_control->ContactBone(), di->HitPos(), 0.f, di->HitType(), 0.0f, false);

        float coef = 0.01f;

        if (who)
        {
            // FX: When is damage to himself, but the energy of the hit will be zero.
            if (object().ID() == who->ID())
                coef = 0;
        }

        CGameObject* obj = smart_cast<CGameObject*>(who);

        if (obj && obj->cast_stalker())
            HDS.power *= coef;

        object().Hit(&HDS);
    }
}

bool CMovementManager::move_along_path() const
{
    if (!enabled())
        return (false);

    if (!actual())
        return (false);

    if (detail().path().empty())
        return (false);

    if (detail().completed(object().Position(), true))
        return (false);

    if (detail().curr_travel_point_index() >= detail().path().size() - 1)
        return (false);

    if (fis_zero(old_desirable_speed()))
        return (false);

    return (true);
}

Fvector CMovementManager::path_position(const float& velocity, const Fvector& position, const float& time_delta, u32& current_travel_point, float& dist, float& dist_to_target,
                                        Fvector& dir_to_target)
{
    VERIFY(current_travel_point < (detail().path().size() - 1));

    Fvector dest_position = position;

    // Вычислить пройденную дистанцию, определить целевую позицию на маршруте,
    //			 изменить detail().m_current_travel_point

    float desirable_speed = velocity; // желаемая скорость объекта
    dist = desirable_speed * time_delta; // пройденное расстояние в соостветствие с желаемой скоростью

    // определить целевую точку
    Fvector target;

    // обновить detail().m_current_travel_point в соответствие с текущей позицией
    while (current_travel_point < detail().path().size() - 2)
    {
        float pos_dist_to_cur_point = dest_position.distance_to(detail().path()[current_travel_point].position);
        float pos_dist_to_next_point = dest_position.distance_to(detail().path()[current_travel_point + 1].position);
        float cur_point_dist_to_next_point = detail().path()[current_travel_point].position.distance_to(detail().path()[current_travel_point + 1].position);

        if ((pos_dist_to_cur_point > cur_point_dist_to_next_point) && (pos_dist_to_cur_point > pos_dist_to_next_point))
        {
            ++current_travel_point;
        }
        else
            break;
    }

    target.set(detail().path()[current_travel_point + 1].position);
    // определить направление к целевой точке
    dir_to_target.sub(target, dest_position);

    // дистанция до целевой точки
    dist_to_target = dir_to_target.magnitude();

    while (dist > dist_to_target)
    {
        dest_position.set(target);
        dist -= dist_to_target;

        if (current_travel_point + 1 >= detail().path().size())
        {
            return (dest_position);
        }

        ++current_travel_point;
        if ((current_travel_point + 1) >= detail().path().size())
        {
            dist = 0.f;
            return (dest_position);
        }

        target.set(detail().path()[current_travel_point + 1].position);
        dir_to_target.sub(target, dest_position);
        dist_to_target = dir_to_target.magnitude();
    }

    VERIFY(dist <= dist_to_target);
    return (dest_position);
}

Fvector CMovementManager::path_position(const float& time_to_check)
{
    if (path_completed())
        return (object().Position());

    if (detail().path().empty())
        return (object().Position());

    if (detail().completed(object().Position(), true))
        return (object().Position());

    Fvector dir_to_target;
    float dist_to_target;
    float dist;
    u32 current_travel_point = detail().m_current_travel_point;
    return (path_position(old_desirable_speed(), object().Position(), time_to_check, current_travel_point, dist, dist_to_target, dir_to_target));
}

void CMovementManager::move_along_path(CPHMovementControl* movement_control, Fvector& dest_position, float time_delta)
{
    START_PROFILE("Build Path/Move Along Path")

    VERIFY(movement_control);

    Fvector motion;
    dest_position = object().Position();

    float precision = 0.5f;

    // Если нет движения по пути
    if (!move_along_path())
    {
        m_speed = 0.f;

        DBG_PH_MOVE_CONDITIONS(if (ph_dbg_draw_mask.test(phDbgNeverUseAiPhMove)) {
            movement_control->SetPosition(dest_position);
            if (movement_control->CharacterExist())
                movement_control->DisableCharacter();
        })
        if (movement_control->IsCharacterEnabled())
        {
            movement_control->Calculate(detail().path(), 0.f, detail().m_current_travel_point, precision);
            movement_control->GetPosition(dest_position);
        }

        // проверка на хит
        apply_collision_hit(movement_control);
        return;
    }

    if (!movement_control->CharacterExist())
        return;

    if (time_delta < EPS)
        return;

    float desirable_speed = old_desirable_speed(); // желаемая скорость объекта
    float desirable_dist = desirable_speed * time_delta;
    float dist;

    // position_computation
    Fvector dir_to_target;
    float dist_to_target;
    u32 current_travel_point = detail().m_current_travel_point;
    dest_position = path_position(old_desirable_speed(), object().Position(), time_delta, current_travel_point, dist, dist_to_target, dir_to_target);

    // Lain: added steering behaviour

    if (detail().m_current_travel_point != current_travel_point)
        on_travel_point_change(detail().m_current_travel_point);
    detail().m_current_travel_point = current_travel_point;

    if (dist_to_target < EPS_L)
    {
#pragma todo("Dima to ? : is this correct?")
        if (current_travel_point + 1 < detail().path().size())
            detail().m_current_travel_point = current_travel_point + 1;
        else
            detail().m_current_travel_point = (u32)detail().path().size() - 1;
        m_speed = 0.f;
        return;
    }

    // Физика устанавливает новую позицию
    Device.Statistic->Physics.Begin();

    // получить физ. объекты в радиусе
    m_nearest_objects.clear();
    Level().ObjectSpace.GetNearest(m_nearest_objects, dest_position, DISTANCE_PHISICS_ENABLE_CHARACTERS + (movement_control->IsCharacterEnabled() ? 0.5f : 0.f), &object());

    // установить позицию
    VERIFY(dist >= 0.f);
    VERIFY(dist_to_target >= 0.f);
    motion.mul(dir_to_target, dist / dist_to_target);
    dest_position.add(motion);

    Fvector velocity = dir_to_target;
    velocity.normalize_safe();
    if (velocity.y > 0.9f)
        velocity.y = 0.8f;
    if (velocity.y < -0.9f)
        velocity.y = -0.8f;
    velocity.normalize_safe(); //как не странно, mdir - не нормирован
    velocity.mul(desirable_speed); //*1.25f

    if (!movement_control->PhysicsOnlyMode())
        movement_control->SetCharacterVelocity(velocity);

    if (DBG_PH_MOVE_CONDITIONS(ph_dbg_draw_mask.test(phDbgNeverUseAiPhMove) || !ph_dbg_draw_mask.test(phDbgAlwaysUseAiPhMove) &&) !(m_nearest_objects.empty()))
    { //  физ. объект

        if (DBG_PH_MOVE_CONDITIONS(!ph_dbg_draw_mask.test(phDbgNeverUseAiPhMove) &&) !movement_control->TryPosition(dest_position))
        {
            movement_control->GetPosition(dest_position);
            movement_control->Calculate(detail().path(), desirable_speed, detail().m_current_travel_point, precision);

            // проверка на хит
            apply_collision_hit(movement_control);
        }
        else
        {
            DBG_PH_MOVE_CONDITIONS(if (ph_dbg_draw_mask.test(phDbgNeverUseAiPhMove)) {
                movement_control->SetPosition(dest_position);
                movement_control->DisableCharacter();
            })
            movement_control->b_exect_position = true;
        }
        movement_control->GetPosition(dest_position);
    }
    else // есть физ. объекты
    {
        movement_control->SetPosition(dest_position);
        movement_control->DisableCharacter();
        movement_control->b_exect_position = true;
    }

    // установить скорость
    float real_motion = motion.magnitude() + desirable_dist - dist;
    float real_speed = real_motion / time_delta;

    m_speed = 0.5f * desirable_speed + 0.5f * real_speed;

    // Физика устанавливает позицию в соответствии с нулевой скоростью
    if (detail().completed(dest_position, true))
    {
        if (!movement_control->PhysicsOnlyMode())
        {
            Fvector velocity = {0.f, 0.f, 0.f};
            movement_control->SetVelocity(velocity);
            m_speed = 0.f;
        }
    }

    Device.Statistic->Physics.End();

    STOP_PROFILE
}

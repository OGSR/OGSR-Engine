#pragma once

#include "../../../../Include/xrRender/Kinematics.h"
#include "../../../Actor.h"
#include "../../../inventory.h"
#include "../../../../xr_3da/CameraBase.h"

#include "../../../HUDManager.h"

#define TEMPLATE_SPECIALIZATION template <typename _Object>

#define CStateBloodsuckerVampireExecuteAbstract CStateBloodsuckerVampireExecute<_Object>

//#define VAMPIRE_MIN_DIST		0.5f
//#define VAMPIRE_MAX_DIST		1.f

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireExecuteAbstract::initialize()
{
    inherited::initialize();

    object->CControlledActor::install();

    look_head();

    m_action = eActionPrepare;
    time_vampire_started = 0;

    object->set_visibility_state(CAI_Bloodsucker::full_visibility);

    object->m_hits_before_vampire = 0;
    object->m_sufficient_hits_before_vampire_random = -1 + (rand() % 3);

    HUD().GetUI()->HideGameIndicators();

    Actor()->inventory().SetSlotsBlocked(INV_STATE_BLOCK_ALL, true);

    // Actor()->set_inventory_disabled	(true);

    m_effector_activated = false;
    m_health_loss_activated = false;
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireExecuteAbstract::execute()
{
    if (!object->CControlledActor::is_turning() && !m_effector_activated)
    {
        object->ActivateVampireEffector();
        m_effector_activated = true;
    }

    look_head();

    switch (m_action)
    {
    case eActionPrepare:
        execute_vampire_prepare();
        m_action = eActionContinue;
        break;

    case eActionContinue: execute_vampire_continue(); break;

    case eActionFire:
        execute_vampire_hit();
        m_action = eActionWaitTripleEnd;
        break;

    case eActionWaitTripleEnd:
        if (!object->com_man().ta_is_active())
        {
            m_action = eActionCompleted;
        }

    case eActionCompleted: break;
    }

    object->dir().face_target(object->EnemyMan.get_enemy());

    auto enemy_pos = object->EnemyMan.get_enemy()->Position();
    Fvector const enemy_to_self = enemy_pos.sub(object->Position());
    float const dist_to_enemy = enemy_to_self.magnitude();
    float const vampire_dist = object->get_vampire_distance();

    if (angle_between_vectors(object->Direction(), enemy_to_self) < deg2rad(20.f) && dist_to_enemy > vampire_dist)
    {
        object->set_action(ACT_RUN);
        object->anim().accel_activate(eAT_Aggressive);
        object->anim().accel_set_braking(false);

        u32 const target_vertex = object->EnemyMan.get_enemy()->ai_location().level_vertex_id();
        Fvector const target_pos = ai().level_graph().vertex_position(target_vertex);

        object->path().set_target_point(target_pos, target_vertex);
        object->path().set_rebuild_time(100);
        object->path().set_use_covers(false);
        object->path().set_distance_to_end(vampire_dist);
    }
    else
    {
        object->set_action(ACT_STAND_IDLE);
    }
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireExecuteAbstract::show_hud()
{
    HUD().GetUI()->ShowGameIndicators();

    Actor()->inventory().SetSlotsBlocked(INV_STATE_BLOCK_ALL, false);
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireExecuteAbstract::cleanup()
{
    // Actor()->set_inventory_disabled	(false);

    if (object->com_man().ta_is_active())
        object->com_man().ta_deactivate();

    if (object->CControlledActor::is_controlling())
        object->CControlledActor::release();

    if (m_health_loss_activated)
    {
        const CEntityAlive* enemy = object->EnemyMan.get_enemy();
        if (enemy)
            enemy->conditions().GetChangeValues().m_fV_HealthRestore += object->m_vampire_loss_health_speed;
        m_health_loss_activated = false;
    }

    show_hud();
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireExecuteAbstract::finalize()
{
    inherited::finalize();
    cleanup();
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireExecuteAbstract::critical_finalize()
{
    inherited::critical_finalize();
    cleanup();
}

TEMPLATE_SPECIALIZATION
bool CStateBloodsuckerVampireExecuteAbstract::check_start_conditions()
{
    const CEntityAlive* enemy = object->EnemyMan.get_enemy();

    // проверить дистанцию
    // 	float dist		= object->MeleeChecker.distance_to_enemy	(enemy);
    // 	if ((dist > VAMPIRE_MAX_DIST) || (dist < VAMPIRE_MIN_DIST))	return false;

    if (!object->done_enough_hits_before_vampire())
        return false;

    u32 const vertex_id = ai().level_graph().check_position_in_direction(object->ai_location().level_vertex_id(), object->Position(), enemy->Position());
    if (!ai().level_graph().valid_vertex_id(vertex_id))
        return false;

    if (!object->MeleeChecker.can_start_melee(enemy))
        return false;

    // проверить направление на врага
    if (!object->control().direction().is_face_target(enemy, PI_DIV_2))
        return false;

    if (!object->WantVampire())
        return false;

    // является ли враг актером
    if (!smart_cast<CActor const*>(enemy))
        return false;

    if (object->CControlledActor::is_controlling())
        return false;

    const CActor* actor = smart_cast<const CActor*>(enemy);

    VERIFY(actor);

    if (actor->input_external_handler_installed())
        return false;

    return true;
}

TEMPLATE_SPECIALIZATION
bool CStateBloodsuckerVampireExecuteAbstract::check_completion() { return (m_action == eActionCompleted); }

//////////////////////////////////////////////////////////////////////////

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireExecuteAbstract::execute_vampire_prepare()
{
    object->com_man().ta_activate(object->anim_triple_vampire);
    time_vampire_started = Device.dwTimeGlobal;

    object->sound().play(CAI_Bloodsucker::eVampireGrasp);
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireExecuteAbstract::execute_vampire_continue()
{
    const CEntityAlive* enemy = object->EnemyMan.get_enemy();

    // if (object->Position().distance_to(Actor()->Position()) > 2.f) {
    if (!object->MeleeChecker.can_start_melee(enemy))
    {
        object->com_man().ta_deactivate();
        m_action = eActionCompleted;
        return;
    }

    object->sound().play(CAI_Bloodsucker::eVampireSucking);

    if (!m_health_loss_activated && !fis_zero(object->m_vampire_loss_health_speed))
    {
        enemy->conditions().GetChangeValues().m_fV_HealthRestore -= object->m_vampire_loss_health_speed;
        m_health_loss_activated = true;
    }

    if (time_vampire_started + object->m_vampire_hold_time < Device.dwTimeGlobal)
    {
        m_action = eActionFire;
    }
}

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireExecuteAbstract::execute_vampire_hit()
{
    object->com_man().ta_pointbreak();
    object->sound().play(CAI_Bloodsucker::eVampireHit);
    object->SatisfyVampire();

    const CEntityAlive* enemy = object->EnemyMan.get_enemy();

    if (m_health_loss_activated)
    {
        enemy->conditions().GetChangeValues().m_fV_HealthRestore += object->m_vampire_loss_health_speed;
        m_health_loss_activated = false;
    }

    if (smart_cast<CActor const*>(enemy) && !fis_zero(object->m_vampire_wound))
    {
        IKinematics* pK = smart_cast<IKinematics*>(const_cast<CEntityAlive*>(enemy)->Visual());
        enemy->conditions().AddWound(object->m_vampire_wound, ALife::eHitTypeWound, pK->LL_BoneID("bip01_head"));
    }
}

//////////////////////////////////////////////////////////////////////////

TEMPLATE_SPECIALIZATION
void CStateBloodsuckerVampireExecuteAbstract::look_head()
{
    IKinematics* pK = smart_cast<IKinematics*>(object->Visual());
    Fmatrix bone_transform;
    bone_transform = pK->LL_GetTransform(pK->LL_BoneID("bip01_head"));

    Fmatrix global_transform;
    global_transform.mul_43(object->XFORM(), bone_transform);

    object->CControlledActor::look_point(global_transform.c);
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateBloodsuckerVampireExecuteAbstract

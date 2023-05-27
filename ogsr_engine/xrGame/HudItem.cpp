//////////////////////////////////////////////////////////////////////
// HudItem.cpp: класс родитель для всех предметов имеющих
//				собственный HUD (CWeapon, CMissile etc)
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HudItem.h"
#include "HudSound.h"
#include "physic_item.h"
#include "actor.h"
#include "xrmessages.h"
#include "level.h"
#include "inventory.h"
#include "player_hud.h"
#include "../xr_3da/gamemtllib.h"
#include <array>
#include "HUDManager.h"
#include "Weapon.h"
#include "ActorCondition.h"
#include "Missile.h"
#include "../xr_3da/x_ray.h"

ENGINE_API extern float psHUD_FOV_def;

CHudItem::CHudItem()
{
    m_huditem_flags.zero();
    m_animation_slot = u32(-1);
    RenderHud(TRUE);
    EnableHudInertion(TRUE);
    AllowHudInertion(TRUE);

    m_bobbing = std::make_unique<CWeaponBobbing>(this);

    m_bStopAtEndAnimIsRunning = false;
    m_current_motion_def = nullptr;
    
    m_dwStateTime = 0;
}

DLL_Pure* CHudItem::_construct()
{
    m_object = smart_cast<CPhysicItem*>(this);
    VERIFY(m_object);

    m_item = smart_cast<CInventoryItem*>(this);
    VERIFY(m_item);

    return (m_object);
}

void CHudItem::Load(LPCSTR section)
{
    world_sect = section;

    //загрузить hud, если он нужен
    if (pSettings->line_exist(section, "hud"))
    {
        hud_sect = pSettings->r_string(section, "hud");

        if (pSettings->line_exist(hud_sect, "allow_inertion"))
            EnableHudInertion(pSettings->r_bool(hud_sect, "allow_inertion"));

        if (pSettings->line_exist(hud_sect, "allow_bobbing"))
            allow_bobbing = pSettings->r_bool(hud_sect, "allow_bobbing");

        hud_recalc_koef = READ_IF_EXISTS(pSettings, r_float, hud_sect, "hud_recalc_koef",
                                         1.35f); //На калаше при 1.35 вроде норм смотрится, другим стволам возможно придется подбирать другие значения.
    }

    m_animation_slot = pSettings->r_u32(section, "animation_slot");

    m_nearwall_on = READ_IF_EXISTS(pSettings, r_bool, section, "nearwall_on", IS_OGSR_GA ? true : READ_IF_EXISTS(pSettings, r_bool, "features", "default_nearwall_on", true));

    if (m_nearwall_on)
    {
        // Координаты офсетов для сдвига худа нашел в интернетах :)
        static constexpr std::array<std::tuple<float, float, Fvector, Fvector, float, float>, _CollisionWeaponTypesCount_> CollisionParamsBase{{
            // Min, Max dist, offset, rotate, HudFov, HudFov Aim
            {0.25f, 0.95f, {-0.0615f, -0.4380f, 0.1235f}, {-0.9219f, -0.0972f, 0.2525f}, 0.5f, 0.25f}, //Общие для всех оружий
            {0.25f, 0.70f, {-0.1000f, -0.5537f, 0.0350f}, {-1.0630f, 0.1751f, -0.0600f}, 0.5f, 0.20f}, //Пистолеты
            {0.30f, 1.30f, {-0.0615f, -0.4380f, 0.1235f}, {-0.9219f, -0.0972f, 0.2525f}, 0.5f, 0.25f}, //СВД и прочие длинные снайперки
            {0.35f, 1.85f, {-0.0399f, 0.0929f, -0.0589f}, {0.3908f, 0.0488f, -0.0193f}, 0.5f, 0.25f}, //РПГ
            {0.25f, 0.80f, {0.0015f, -0.5655f, 0.1240f}, {-1.0319f, 0.0678f, 0.0700f}, 0.5f, 0.25f}, //РГ-6
            {0.25f, 0.80f, {-0.0406f, -0.4191f, 0.1718f}, {-0.8981f, -0.1101f, 0.4420f}, 0.5f, 0.25f}, //Гроза
            {0.25f, 0.80f, {-0.0335f, -0.4618f, 0.1098f}, {-0.9119f, -0.0973f, 0.4143f}, 0.5f, 0.25f}, //ФН2000
            {0.25f, 0.60f, {-0.0650f, -0.5170f, 0.0465f}, {-1.0405f, 0.1051f, -0.0350f}, 0.5f, 0.25f}, //БМ-16
            {0.30f, 0.50f, {-0.0025f, -0.4045f, -0.1415f}, {-0.7900f, 0.0100f, 0.f}, 0.5f, 0.25f}, //Болт
            {0.25f, 0.65f, {0.0120f, -0.4780f, -0.1150f}, {-0.6250f, -0.0725f, -0.1950f}, 0.5f, 1.f}, //Детектор
            {0.30f, 0.50f, {-0.0025f, -0.4045f, -0.1415f}, {-0.7900f, 0.0100f, 0.f}, 0.5f, 0.25f}, //Нож, гранаты и прочее
            {0.25f, 0.70f, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, 0.5f, 0.4f}, //Бинокль
        }};
        const size_t type = GetWeaponTypeForCollision();
        const auto& CollisionParams = CollisionParamsBase.at(type);

        // Параметры изменения коллизии когда игрок стоит вплотную к стене
        m_nearwall_hud_offset_speed = READ_IF_EXISTS(pSettings, r_float, section, "nearwall_hud_offset_speed", 1.5f); // Скорость поднятия\опускания ствола
        m_nearwall_dist_min =
            READ_IF_EXISTS(pSettings, r_float, section, "nearwall_dist_min", std::get<0>(CollisionParams)); //Максимальное расстояние, на которое камера ГГ может упереться к стене
        m_nearwall_dist_max = READ_IF_EXISTS(pSettings, r_float, section, "nearwall_dist_max", std::get<1>(CollisionParams)); //Расстояние, ближе которого начинаем поднимать ствол
        m_nearwall_target_hud_offset = READ_IF_EXISTS(pSettings, r_fvector3, section, "nearwall_target_hud_offset", std::get<2>(CollisionParams)); //Максимальный оффсет худа
        m_nearwall_target_hud_rotate = READ_IF_EXISTS(pSettings, r_fvector3, section, "nearwall_target_hud_rotate", std::get<3>(CollisionParams)); //Максимальный поворот худа
        m_nearwall_target_hud_fov = READ_IF_EXISTS(pSettings, r_float, section, "nearwall_target_hud_fov", std::get<4>(CollisionParams));
        m_nearwall_target_aim_hud_fov = READ_IF_EXISTS(pSettings, r_float, section, "nearwall_target_aim_hud_fov", std::get<5>(CollisionParams));
        m_nearwall_speed_mod = READ_IF_EXISTS(pSettings, r_float, section, "nearwall_speed_mod", 10.f);
    }

    m_base_fov = READ_IF_EXISTS(pSettings, r_float, section, "hud_fov", 0.0f);
    m_nearwall_last_hud_fov = m_base_fov > 0.0f ? m_base_fov : psHUD_FOV_def;

    ////////////////////////////////////////////
    m_strafe_offset[0][0] = READ_IF_EXISTS(pSettings, r_fvector3, section, "strafe_hud_offset_pos", (Fvector{0.015f, 0.f, 0.f}));
    m_strafe_offset[1][0] = READ_IF_EXISTS(pSettings, r_fvector3, section, "strafe_hud_offset_rot", (Fvector{0.f, 0.f, 4.5f}));

    m_strafe_offset[0][1] = READ_IF_EXISTS(pSettings, r_fvector3, section, "strafe_aim_hud_offset_pos", (Fvector{0.f, 0.f, 0.f}));
    m_strafe_offset[1][1] = READ_IF_EXISTS(pSettings, r_fvector3, section, "strafe_aim_hud_offset_rot", (Fvector{0.f, 0.f, 2.5f}));

    m_strafe_offset[2][0].set(READ_IF_EXISTS(pSettings, r_bool, section, "strafe_enabled", true), READ_IF_EXISTS(pSettings, r_float, section, "strafe_transition_time", 0.25f),
                              0.f); // normal
    m_strafe_offset[2][1].set(READ_IF_EXISTS(pSettings, r_bool, section, "strafe_aim_enabled", true),
                              READ_IF_EXISTS(pSettings, r_float, section, "strafe_aim_transition_time", 0.15f), 0.f); // aim-GL
    ////////////////////////////////////////////
    ////////////////////////////////////////////
    m_lookout_offset[0][0] = READ_IF_EXISTS(pSettings, r_fvector3, section, "lookout_hud_offset_pos", (Fvector{0.045f, 0.f, 0.f}));
    m_lookout_offset[1][0] = READ_IF_EXISTS(pSettings, r_fvector3, section, "lookout_hud_offset_rot", (Fvector{0.f, 0.f, 10.f}));

    m_lookout_offset[0][1] = READ_IF_EXISTS(pSettings, r_fvector3, section, "lookout_aim_hud_offset_pos", (Fvector{0.f, 0.f, 0.f}));
    m_lookout_offset[1][1] = READ_IF_EXISTS(pSettings, r_fvector3, section, "lookout_aim_hud_offset_rot", (Fvector{0.f, 0.f, 15.f}));

    m_lookout_offset[2][0].set(READ_IF_EXISTS(pSettings, r_bool, section, "lookout_enabled", true), READ_IF_EXISTS(pSettings, r_float, section, "lookout_transition_time", 0.25f),
                               0.f); // normal
    m_lookout_offset[2][1].set(READ_IF_EXISTS(pSettings, r_bool, section, "lookout_aim_enabled", true),
                               READ_IF_EXISTS(pSettings, r_float, section, "lookout_aim_transition_time", 0.15f), 0.f); // aim-GL
    ////////////////////////////////////////////
    ////////////////////////////////////////////
    m_jump_offset[0][0] = READ_IF_EXISTS(pSettings, r_fvector3, section, "jump_hud_offset_pos", (Fvector{0.f, -0.03f, -0.06f}));
    m_jump_offset[1][0] = READ_IF_EXISTS(pSettings, r_fvector3, section, "jump_hud_offset_rot", (Fvector{0.f, -10.f, -10.f}));

    m_jump_offset[0][1] = READ_IF_EXISTS(pSettings, r_fvector3, section, "jump_aim_hud_offset_pos", (Fvector{0.f, 0.03f, 0.01f}));
    m_jump_offset[1][1] = READ_IF_EXISTS(pSettings, r_fvector3, section, "jump_aim_hud_offset_rot", (Fvector{0.f, 2.5f, -3.f}));

    m_jump_offset[2][0].set(READ_IF_EXISTS(pSettings, r_bool, section, "jump_enabled", true), READ_IF_EXISTS(pSettings, r_float, section, "jump_transition_time", 0.5f),
                               0.f); // normal
    m_jump_offset[2][1].set(READ_IF_EXISTS(pSettings, r_bool, section, "jump_aim_enabled", true),
                               READ_IF_EXISTS(pSettings, r_float, section, "jump_aim_transition_time", 0.45f), 0.f); // aim-GL
    ////////////////////////////////////////////
    ////////////////////////////////////////////
    m_fall_offset[0][0] = READ_IF_EXISTS(pSettings, r_fvector3, section, "fall_hud_offset_pos", (Fvector{0.f, -0.03f, 0.03f}));
    m_fall_offset[1][0] = READ_IF_EXISTS(pSettings, r_fvector3, section, "fall_hud_offset_rot", (Fvector{0.f, 6.f, 6.f}));

    m_fall_offset[0][1] = READ_IF_EXISTS(pSettings, r_fvector3, section, "fall_aim_hud_offset_pos", (Fvector{0.f, 0.03f, -0.01f}));
    m_fall_offset[1][1] = READ_IF_EXISTS(pSettings, r_fvector3, section, "fall_aim_hud_offset_rot", (Fvector{0.f, -2.5f, 3.f}));

    m_fall_offset[2][0].set(READ_IF_EXISTS(pSettings, r_bool, section, "fall_enabled", true), READ_IF_EXISTS(pSettings, r_float, section, "fall_transition_time", 1.f),
                            0.f); // normal
    m_fall_offset[2][1].set(READ_IF_EXISTS(pSettings, r_bool, section, "fall_aim_enabled", true), READ_IF_EXISTS(pSettings, r_float, section, "fall_aim_transition_time", 0.7f),
                            0.f); // aim-GL

    ////////////////////////////////////////////

    //Загрузка параметров инерции --#SM+# Begin--
    constexpr float PITCH_OFFSET_R = 0.0f; // Насколько сильно ствол смещается вбок (влево) при вертикальных поворотах камеры
    constexpr float PITCH_OFFSET_N = 0.0f; // Насколько сильно ствол поднимается\опускается при вертикальных поворотах камеры
    constexpr float PITCH_OFFSET_D = 0.02f; // Насколько сильно ствол приближается\отдаляется при вертикальных поворотах камеры
    constexpr float PITCH_LOW_LIMIT = -PI; // Минимальное значение pitch при использовании совместно с PITCH_OFFSET_N
    constexpr float ORIGIN_OFFSET = -0.05f; // Фактор влияния инерции на положение ствола (чем меньше, тем масштабней инерция)
    constexpr float ORIGIN_OFFSET_AIM = -0.03f; // (Для прицеливания)
    constexpr float TENDTO_SPEED = 5.f; // Скорость нормализации положения ствола
    constexpr float TENDTO_SPEED_AIM = 8.f; // (Для прицеливания)

    inertion_data.m_pitch_offset_r = READ_IF_EXISTS(pSettings, r_float, hud_sect, "pitch_offset_right", PITCH_OFFSET_R);
    inertion_data.m_pitch_offset_n = READ_IF_EXISTS(pSettings, r_float, hud_sect, "pitch_offset_up", PITCH_OFFSET_N);
    inertion_data.m_pitch_offset_d = READ_IF_EXISTS(pSettings, r_float, hud_sect, "pitch_offset_forward", PITCH_OFFSET_D);
    inertion_data.m_pitch_low_limit = READ_IF_EXISTS(pSettings, r_float, hud_sect, "pitch_offset_up_low_limit", PITCH_LOW_LIMIT);

    inertion_data.m_origin_offset = READ_IF_EXISTS(pSettings, r_float, hud_sect, "inertion_origin_offset", ORIGIN_OFFSET);
    inertion_data.m_origin_offset_aim = READ_IF_EXISTS(pSettings, r_float, hud_sect, "inertion_zoom_origin_offset", ORIGIN_OFFSET_AIM);
    inertion_data.m_tendto_speed = READ_IF_EXISTS(pSettings, r_float, hud_sect, "inertion_tendto_speed", TENDTO_SPEED);
    inertion_data.m_tendto_speed_aim = READ_IF_EXISTS(pSettings, r_float, hud_sect, "inertion_zoom_tendto_speed", TENDTO_SPEED_AIM);
    //--#SM+# End--
}

void CHudItem::PlaySound(HUD_SOUND& hud_snd, const Fvector& position, bool overlap) { HUD_SOUND::PlaySound(hud_snd, position, object().H_Root(), !!GetHUDmode(), false, overlap); }

void CHudItem::net_Destroy() { m_dwStateTime = 0; }

BOOL CHudItem::net_Spawn(CSE_Abstract* DC) { return TRUE; }

void CHudItem::renderable_Render()
{
    UpdateXForm();
    const bool _hud_render = ::Render->get_HUD() && GetHUDmode();

    if (!(_hud_render && !IsHidden()))
    {
        if (!object().H_Parent() || (!_hud_render && !IsHidden()))
        {
            on_renderable_Render();
        }
        else if (object().H_Parent())
        {
            CInventoryOwner* owner = smart_cast<CInventoryOwner*>(object().H_Parent());
            VERIFY(owner);
            CInventoryItem* self = smart_cast<CInventoryItem*>(this);
            if (owner->attached(self))
                on_renderable_Render();
        }
    }
}

bool CHudItem::Action(s32 cmd, u32 flags) { return false; }

void CHudItem::SwitchState(u32 S)
{
    SetNextState(S); // Very-very important line of code!!! :)

    if (object().Local() && !object().getDestroy())
    {
        // !!! Just single entry for given state !!!
        NET_Packet P;
        object().u_EventGen(P, GE_WPN_STATE_CHANGE, object().ID());
        P.w_u8(u8(S));
        object().u_EventSend(P);
    }
}

void CHudItem::OnEvent(NET_Packet& P, u16 type)
{
    switch (type)
    {
    case GE_WPN_STATE_CHANGE: {
        u8 S;
        P.r_u8(S);
        OnStateSwitch(u32(S), GetState());
    }
    break;
    }
}

void CHudItem::OnStateSwitch(u32 S, u32 oldState)
{
    m_dwStateTime = 0;
    SetState(S);
    if (object().Remote())
        SetNextState(S);

    if (S == eHidden)
    {
        m_nearwall_last_hud_fov = m_base_fov > 0.0f ? m_base_fov : psHUD_FOV_def;
        SprintType = false;
    }
    else if (S == eSprintStart)
        PlayAnimSprintStart();
    else if (S == eSprintEnd)
        PlayAnimSprintEnd();
    else if (S != eIdle)
        SprintType = false;

    g_player_hud->updateMovementLayerState();
}

bool CHudItem::Activate(bool now)
{
    Show(now);
    OnActiveItem();
    return true;
}

void CHudItem::Deactivate(bool now)
{
    Hide(now);
    OnHiddenItem();
}

void CHudItem::UpdateCL()
{
    m_dwStateTime += Device.dwTimeDelta;

    if (m_current_motion_def)
    {
        if (m_bStopAtEndAnimIsRunning)
        {
            const xr_vector<motion_marks>& marks = m_current_motion_def->marks;
            if (!marks.empty())
            {
                float motion_prev_time = ((float)m_dwMotionCurrTm - (float)m_dwMotionStartTm) / 1000.0f;
                float motion_curr_time = ((float)Device.dwTimeGlobal - (float)m_dwMotionStartTm) / 1000.0f;

                xr_vector<motion_marks>::const_iterator it = marks.begin();
                xr_vector<motion_marks>::const_iterator it_e = marks.end();
                for (; it != it_e; ++it)
                {
                    const motion_marks& M = (*it);
                    if (M.is_empty())
                        continue;

                    const motion_marks::interval* Iprev = M.pick_mark(motion_prev_time);
                    const motion_marks::interval* Icurr = M.pick_mark(motion_curr_time);
                    if (Iprev == nullptr && Icurr != nullptr /* || M.is_mark_between(motion_prev_time, motion_curr_time)*/)
                    {
                        OnMotionMark(m_startedMotionState, M);
                    }
                }
            }

            m_dwMotionCurrTm = Device.dwTimeGlobal;
            if (m_dwMotionCurrTm > m_dwMotionEndTm)
            {
                m_current_motion_def = nullptr;
                m_dwMotionStartTm = 0;
                m_dwMotionEndTm = 0;
                m_dwMotionCurrTm = 0;
                m_bStopAtEndAnimIsRunning = false;
                OnAnimationEnd(m_startedMotionState);
            }
        }
    }

    AllowHudBobbing((Core.Features.test(xrCore::Feature::wpn_bobbing) && allow_bobbing) || Actor()->PsyAuraAffect);
}

void CHudItem::OnH_A_Chield() {}

void CHudItem::OnH_B_Chield()
{
    StopCurrentAnimWithoutCallback();

    m_nearwall_last_hud_fov = m_base_fov > 0.0f ? m_base_fov : psHUD_FOV_def;
}

void CHudItem::OnH_B_Independent(bool just_before_destroy)
{
    StopHUDSounds();
    UpdateXForm();

    m_nearwall_last_hud_fov = m_base_fov > 0.0f ? m_base_fov : psHUD_FOV_def;
}

void CHudItem::OnH_A_Independent()
{
    if (HudItemData())
        g_player_hud->detach_item(this);
    StopCurrentAnimWithoutCallback();
}

void CHudItem::on_b_hud_detach() {}

void CHudItem::on_a_hud_attach()
{
    if (m_current_motion_def)
    {
        PlayHUDMotion_noCB(m_current_motion, false);
#ifdef DEBUG
        //		Msg("continue playing [%s][%d]",m_current_motion.c_str(), Device.dwFrame);
#endif // #ifdef DEBUG
    }
    else
    {
#ifdef DEBUG
        //		Msg("no active motion");
#endif // #ifdef DEBUG
    }
}

u32 CHudItem::PlayHUDMotion(const char* M, const bool bMixIn, const u32 state, const bool randomAnim, float speed)
{
    auto Wpn = g_player_hud->attached_item(0);
    auto Det = g_player_hud->attached_item(1);

    if (Det && Det->m_parent_hud_item != this && Wpn && Wpn->m_parent_hud_item == this && (smart_cast<CWeapon*>(this) || smart_cast<CMissile*>(this)) &&
        Det->m_parent_hud_item->GetState() == eIdle)
    {
        if (strstr(M, "anm_") && !strstr(M, "idle"))
        { 
            //с айдловыми анимациями слишком много багов
            string128 det_anm_name;
            xr_strconcat(det_anm_name, "anm_lefthand_", Det->m_parent_hud_item->world_sect.c_str(), "_wpn_", M + 4);
            if (Det->m_parent_hud_item->AnimationExist(det_anm_name))
                Det->m_parent_hud_item->PlayHUDMotion(det_anm_name, true, Det->m_parent_hud_item->GetState());
        }
    }

    // Msg("~~[%s] Playing motion [%s] for [%s]", __FUNCTION__, M.c_str(), HudSection().c_str());
    u32 anim_time = PlayHUDMotion_noCB(M, bMixIn, randomAnim, speed);
    if (anim_time > 0)
    {
        m_bStopAtEndAnimIsRunning = true;
        m_dwMotionStartTm = Device.dwTimeGlobal;
        m_dwMotionCurrTm = m_dwMotionStartTm;
        m_dwMotionEndTm = m_dwMotionStartTm + anim_time;
        m_startedMotionState = state;
    }
    else
        m_bStopAtEndAnimIsRunning = false;

    return anim_time;
}

u32 CHudItem::PlayHUDMotion(std::initializer_list<const char*> Ms, const bool bMixIn, const u32 state, const bool randomAnim, float speed)
{
    for (const auto* M : Ms)
        if (AnimationExist(M))
            return PlayHUDMotion(M, bMixIn, state, randomAnim, speed);
    /*
    xr_string dbg_anim_name;
    for (const auto* M : Ms) {
        dbg_anim_name += M;
        dbg_anim_name += ", ";
    }
    Msg("~~[%s] Motions [%s] not found for [%s]", __FUNCTION__, dbg_anim_name.c_str(), HudSection().c_str());
    */
    return 0;
}

u32 CHudItem::PlayHUDMotion_noCB(const shared_str& motion_name, const bool bMixIn, const bool randomAnim, float speed)
{
    m_current_motion = motion_name;

    if (HudItemData())
    {
        return HudItemData()->anim_play(motion_name, bMixIn, m_current_motion_def, randomAnim, speed);
    }
    else
    {
        return g_player_hud->motion_length(motion_name, HudSection(), m_current_motion_def, speed);
    }
}

void CHudItem::StopCurrentAnimWithoutCallback()
{
    m_dwMotionStartTm = 0;
    m_dwMotionEndTm = 0;
    m_dwMotionCurrTm = 0;
    m_bStopAtEndAnimIsRunning = false;
    m_current_motion_def = nullptr;
    m_dwStateTime = 0;
}

BOOL CHudItem::GetHUDmode()
{
    if (object().H_Parent())
    {
        CActor* A = smart_cast<CActor*>(object().H_Parent());
        return (A && A->HUDview() && HudItemData());
    }
    else
        return FALSE;
}

void CHudItem::PlayAnimIdle()
{
    if (TryPlayAnimIdle())
        return;

    auto wpn = smart_cast<CWeapon*>(this);
    PlayHUDMotion({(wpn && wpn->IsMisfire()) ?
                       "anm_idle_jammed" :
                       ((wpn && ((wpn->GetAmmoElapsed() == 0 && !wpn->IsGrenadeMode()) || (wpn->GetAmmoElapsed2() == 0 && wpn->IsGrenadeMode()))) ? "anm_idle_empty" : "nullptr"),
                   "anim_idle", "anm_idle"},
                  true, GetState());
}

void CHudItem::PlayAnimSprintStart()
{
    auto wpn = smart_cast<CWeapon*>(this);
    string128 guns_sprint_start_anm;
    xr_strconcat(guns_sprint_start_anm, "anm_idle_sprint_start",
                 (wpn && wpn->IsMisfire()) ?
                     "_jammed" :
                     ((wpn && ((wpn->GetAmmoElapsed() == 0 && !wpn->IsGrenadeMode()) || (wpn->GetAmmoElapsed2() == 0 && wpn->IsGrenadeMode()))) ? "_empty" : ""),
                 (wpn && wpn->IsGrenadeLauncherAttached()) ? (wpn && wpn->IsGrenadeMode() ? "_g" : "_w_gl") : "");
    if (AnimationExist(guns_sprint_start_anm))
        PlayHUDMotion(guns_sprint_start_anm, true, GetState());
    else
    {
        SprintType = true;
        SwitchState(eIdle);
    }
}

void CHudItem::PlayAnimSprintEnd()
{
    auto wpn = smart_cast<CWeapon*>(this);
    string128 guns_sprint_end_anm;
    xr_strconcat(guns_sprint_end_anm, "anm_idle_sprint_end",
                 (wpn && wpn->IsMisfire()) ?
                     "_jammed" :
                     ((wpn && ((wpn->GetAmmoElapsed() == 0 && !wpn->IsGrenadeMode()) || (wpn->GetAmmoElapsed2() == 0 && wpn->IsGrenadeMode()))) ? "_empty" : ""),
                 (wpn && wpn->IsGrenadeLauncherAttached()) ? (wpn && wpn->IsGrenadeMode() ? "_g" : "_w_gl") : "");
    if (AnimationExist(guns_sprint_end_anm))
        PlayHUDMotion(guns_sprint_end_anm, true, GetState());
    else
    {
        SprintType = false;
        SwitchState(eIdle);
    }
}

bool CHudItem::TryPlayAnimIdle()
{
    if (!IsZoomed() || !smart_cast<CWeapon*>(this))
    {
        if (auto pActor = smart_cast<CActor*>(object().H_Parent()))
        {
            const u32 State = pActor->get_state();
            if (State & mcSprint)
            {
                if (!SprintType)
                {
                    SwitchState(eSprintStart);
                    return true;
                }
                PlayAnimIdleSprint();
                return true;
            }
            else if (SprintType)
            {
                SwitchState(eSprintEnd);
                return true;
            }
            else if ((State & mcAnyMove) && AnmIdleMovingAllowed())
            {
                if (!(State & mcCrouch))
                {
                    if (State & mcAccel) //Ходьба медленная (SHIFT)
                        PlayAnimIdleMovingSlow();
                    else
                        PlayAnimIdleMoving();
                    return true;
                }
                else if (State & mcAccel) //Ходьба в присяде (CTRL+SHIFT)
                {
                    PlayAnimIdleMovingCrouchSlow();
                    return true;
                }
                else
                {
                    PlayAnimIdleMovingCrouch();
                    return true;
                }
            }
        }
    }
    return false;
}

/*void CHudItem::PlayAnimBore()
{
    PlayHUDMotion({ "anim_idle", "anm_bore" }, true, GetState());
}*/

bool CHudItem::AnimationExist(const char* anim_name) const
{
    if (HudItemData())
    {
        string256 anim_name_r;
        bool is_16x9 = UI()->is_widescreen();
        u16 attach_place_idx = HudItemData()->m_attach_place_idx;
        xr_sprintf(anim_name_r, "%s%s", anim_name, (attach_place_idx == 1 && is_16x9) ? "_16x9" : "");
        player_hud_motion* anm = HudItemData()->find_motion(anim_name_r);
        if (anm)
            return true;
    }
    else // Third person
    {
        const CMotionDef* temp_motion_def;
        if (g_player_hud->motion_length(anim_name, HudSection(), temp_motion_def) > 100)
            return true;
    }
#ifdef DEBUG
    Msg("~ [WARNING] ------ Animation [%s] does not exist in [%s]", anim_name, HudSection().c_str());
#endif
    return false;
}

void CHudItem::PlayAnimIdleMoving()
{
    auto wpn = smart_cast<CWeapon*>(this);
    PlayHUDMotion(
        {(wpn && wpn->IsMisfire()) ?
             "anm_idle_moving_jammed" :
             ((wpn && ((wpn->GetAmmoElapsed() == 0 && !wpn->IsGrenadeMode()) || (wpn->GetAmmoElapsed2() == 0 && wpn->IsGrenadeMode()))) ? "anm_idle_moving_empty" : "nullptr"),
         "anm_idle_moving", "anm_idle", "anim_idle_moving", "anim_idle"},
        true, GetState());
}

void CHudItem::PlayAnimIdleMovingSlow()
{
    auto wpn = smart_cast<CWeapon*>(this);
    PlayHUDMotion(
        {(wpn && wpn->IsMisfire()) ?
             "anm_idle_moving_slow_jammed" :
             ((wpn && ((wpn->GetAmmoElapsed() == 0 && !wpn->IsGrenadeMode()) || (wpn->GetAmmoElapsed2() == 0 && wpn->IsGrenadeMode()))) ? "anm_idle_moving_slow_empty" : "nullptr"),
         "anm_idle_moving_slow", "anm_idle_moving_slow", "anm_idle", "anim_idle_moving", "anim_idle"},
        true, GetState());
}

void CHudItem::PlayAnimIdleMovingCrouch()
{
    auto wpn = smart_cast<CWeapon*>(this);
    PlayHUDMotion({(wpn && wpn->IsMisfire()) ?
                       "anm_idle_moving_crouch_jammed" :
                       ((wpn && ((wpn->GetAmmoElapsed() == 0 && !wpn->IsGrenadeMode()) || (wpn->GetAmmoElapsed2() == 0 && wpn->IsGrenadeMode()))) ? "anm_idle_moving_crouch_empty" :
                                                                                                                                                    "nullptr"),
                   "anm_idle_moving_crouch", "anm_idle_moving", "anm_idle", "anim_idle_moving", "anim_idle"},
                  true, GetState());
}

void CHudItem::PlayAnimIdleMovingCrouchSlow()
{
    auto wpn = smart_cast<CWeapon*>(this);
    PlayHUDMotion({(wpn && wpn->IsMisfire()) ? "anm_idle_moving_crouch_slow_jammed" :
                                               ((wpn && ((wpn->GetAmmoElapsed() == 0 && !wpn->IsGrenadeMode()) || (wpn->GetAmmoElapsed2() == 0 && wpn->IsGrenadeMode()))) ?
                                                    "anm_idle_moving_crouch_slow_empty" :
                                                    "nullptr"),
                   "anm_idle_moving_crouch_slow", "anm_idle_moving_crouch", "anm_idle_moving", "anm_idle", "anim_idle_moving", "anim_idle"},
                  true, GetState());
}

void CHudItem::PlayAnimIdleSprint()
{
    auto wpn = smart_cast<CWeapon*>(this);
    PlayHUDMotion(
        {(wpn && wpn->IsMisfire()) ?
             "anm_idle_sprint_jammed" :
             ((wpn && ((wpn->GetAmmoElapsed() == 0 && !wpn->IsGrenadeMode()) || (wpn->GetAmmoElapsed2() == 0 && wpn->IsGrenadeMode()))) ? "anm_idle_sprint_empty" : "nullptr"),
         "anm_idle_sprint", "anm_idle", "anim_idle_sprint", "anim_idle"},
        true, GetState());
}

bool CHudItem::NeedBlendAnm()
{
    u32 state = GetState();
    return (state != eIdle && state != eHidden);
}

void CHudItem::OnMovementChanged(ACTOR_DEFS::EMoveCommand cmd)
{
    if (GetState() == eIdle && !m_bStopAtEndAnimIsRunning)
    {
        PlayAnimIdle();
        ResetSubStateTime();
    }
}

attachable_hud_item* CHudItem::HudItemData() const
{
    attachable_hud_item* hi = nullptr;
    if (!g_player_hud)
        return hi;

    hi = g_player_hud->attached_item(0);
    if (hi && hi->m_parent_hud_item == this)
        return hi;

    hi = g_player_hud->attached_item(1);
    if (hi && hi->m_parent_hud_item == this)
        return hi;

    return nullptr;
}

bool CHudItem::used_cop_fire_point() const
{
    auto hd = HudItemData();
    return hd && hd->m_measures.useCopFirePoint;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Сделано на базе увиденного в старом скрипте от Shoker.
// Перевёл его в движок и доработал под современные реалии для OGSR Engine: KRodin
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TODO: Желательно бы сделать визуальную настройку всех этих офсетов через hud_adjust
// TODO: Придумать более годный способ получения полиции и направления для коллизии болтов, детекторов, биноклей и ножей. У стволов есть shoot_point - там легко считать от него, в
// отличие от.
// TODO: Придумать какой-то способ получения нужной позиции для рейтрейса. Сейчас сделал не оптиматьно, по сути худ считается два раза за фрейм (не полностью, но такое решение всё
// равно выглядит плохо)
//              Эти костыли для двойного рассчета можно найти по skip_updated_frame, need_update_collision
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CHudItem::CollisionAllowed() const
{ //Если выкл реалистичный прицел или у ствола ТЧ-стайл фейр поинт или ствол в режиме зума - коллизия работать не будет.
    return m_nearwall_on && psHUD_Flags.test(HUD_CROSSHAIR_HARD) && used_cop_fire_point() && m_fZoomRotationFactor < 1.0f;
}

void CHudItem::UpdateCollision(Fmatrix& trans)
{
    if (!CollisionAllowed())
    {
        // Это необходимо обнулять, если ствол сейчас в режиме зума.
        m_nearwall_last_pos.set(0.f, 0.f, 0.f);
        m_nearwall_last_rot.set(0.f, 0.f, 0.f);
        return;
    }

    skip_updated_frame = Device.dwFrame;

    constexpr float DMGR_STEP_COEF = 180.f; //Магическое число без которого всё будет работать плохо. Влияет на плавность.

    collide::rq_result RQ{nullptr, m_nearwall_dist_max * 1.1f, -1};
    const collide::ray_defs RD{GetPositionForCollision(), GetDirectionForCollision(), RQ.range, CDB::OPT_CULL, collide::rqtBoth};
    collide::rq_results RQR;

    Level().ObjectSpace.RayQuery(
        RQR, RD,
        [](collide::rq_result& result, LPVOID params) {
            //Копипаст из  CHUDTarget
            auto RQ = reinterpret_cast<collide::rq_result*>(params);

            if (result.O)
            {
                if (auto* e = smart_cast<CEntityAlive*>(result.O); e && e->g_Alive())
                    return TRUE;

                *RQ = result;
                return FALSE;
            }

            CDB::TRI* T = Level().ObjectSpace.GetStaticTris() + result.element;
            SGameMtl* mtl = GMLib.GetMaterialByIdx(T->material);
            if (mtl->Flags.is(SGameMtl::flPassable))
                return TRUE;

            *RQ = result;
            return FALSE;
        },
        &RQ, nullptr, Level().CurrentEntity());

    saved_rq_range = RQ.range;

    clamp(RQ.range, m_nearwall_dist_min, RQ.range);

    const u32 delta = Device.dwTimeGlobal - m_nearwall_last_call;
    m_nearwall_last_call = Device.dwTimeGlobal;

    float speed_coef{1.f};
    if (delta > 10)
        if (delta > 30)
            speed_coef = 6;
        else
            speed_coef = delta / 5;

    speed_coef = speed_coef * m_nearwall_hud_offset_speed;
    const Fvector pos_steps{fabsf(m_nearwall_target_hud_offset.x / DMGR_STEP_COEF) * speed_coef, fabsf(m_nearwall_target_hud_offset.y / DMGR_STEP_COEF) * speed_coef,
                            fabsf(m_nearwall_target_hud_offset.z / DMGR_STEP_COEF) * speed_coef};
    const Fvector rot_steps{fabsf(m_nearwall_target_hud_rotate.x / DMGR_STEP_COEF) * speed_coef, fabsf(m_nearwall_target_hud_rotate.y / DMGR_STEP_COEF) * speed_coef,
                            fabsf(m_nearwall_target_hud_rotate.z / DMGR_STEP_COEF) * speed_coef};

    float dist_coef{};
    if (RQ.range <= m_nearwall_dist_max)
        dist_coef = 1.f - (RQ.range - m_nearwall_dist_min) / (m_nearwall_dist_max - m_nearwall_dist_min);

    const auto curr_offs = Fvector{}.mul(m_nearwall_target_hud_offset, dist_coef), curr_rot = Fvector{}.mul(m_nearwall_target_hud_rotate, dist_coef);
    Fvector curr_offs_final{}, curr_rot_final{};

    if (curr_offs.x >= m_nearwall_last_pos.x)
    {
        curr_offs_final.x = m_nearwall_last_pos.x + pos_steps.x;
        if (curr_offs_final.x > curr_offs.x)
            curr_offs_final.x = curr_offs.x;
    }
    else if (curr_offs.x < m_nearwall_last_pos.x)
    {
        curr_offs_final.x = m_nearwall_last_pos.x - pos_steps.x;
        if (curr_offs_final.x < curr_offs.x)
            curr_offs_final.x = curr_offs.x;
    }

    if (curr_offs.y >= m_nearwall_last_pos.y)
    {
        curr_offs_final.y = m_nearwall_last_pos.y + pos_steps.y;
        if (curr_offs_final.y > curr_offs.y)
            curr_offs_final.y = curr_offs.y;
    }
    else if (curr_offs.y < m_nearwall_last_pos.y)
    {
        curr_offs_final.y = m_nearwall_last_pos.y - pos_steps.y;
        if (curr_offs_final.y < curr_offs.y)
            curr_offs_final.y = curr_offs.y;
    }

    if (curr_offs.z >= m_nearwall_last_pos.z)
    {
        curr_offs_final.z = m_nearwall_last_pos.z + pos_steps.z;
        if (curr_offs_final.z > curr_offs.z)
            curr_offs_final.z = curr_offs.z;
    }
    else if (curr_offs.z < m_nearwall_last_pos.z)
    {
        curr_offs_final.z = m_nearwall_last_pos.z - pos_steps.z;
        if (curr_offs_final.z < curr_offs.z)
            curr_offs_final.z = curr_offs.z;
    }

    if (curr_rot.x >= m_nearwall_last_rot.x)
    {
        curr_rot_final.x = m_nearwall_last_rot.x + rot_steps.x;
        if (curr_rot_final.x > curr_rot.x)
            curr_rot_final.x = curr_rot.x;
    }
    else if (curr_rot.x < m_nearwall_last_rot.x)
    {
        curr_rot_final.x = m_nearwall_last_rot.x - rot_steps.x;
        if (curr_rot_final.x < curr_rot.x)
            curr_rot_final.x = curr_rot.x;
    }

    if (curr_rot.y >= m_nearwall_last_rot.y)
    {
        curr_rot_final.y = m_nearwall_last_rot.y + rot_steps.y;
        if (curr_rot_final.y > curr_rot.y)
            curr_rot_final.y = curr_rot.y;
    }
    else if (curr_rot.y < m_nearwall_last_rot.y)
    {
        curr_rot_final.y = m_nearwall_last_rot.y - rot_steps.y;
        if (curr_rot_final.y < curr_rot.y)
            curr_rot_final.y = curr_rot.y;
    }

    if (curr_rot.z >= m_nearwall_last_rot.z)
    {
        curr_rot_final.z = m_nearwall_last_rot.z + rot_steps.z;
        if (curr_rot_final.z > curr_rot.z)
            curr_rot_final.z = curr_rot.z;
    }
    else if (curr_rot.z < m_nearwall_last_rot.z)
    {
        curr_rot_final.z = m_nearwall_last_rot.z - rot_steps.z;
        if (curr_rot_final.z < curr_rot.z)
            curr_rot_final.z = curr_rot.z;
    }

    m_nearwall_last_pos = curr_offs_final;
    m_nearwall_last_rot = curr_rot_final;

    //Чтоб был более-менее плавный вход/выход из прицеливания
    curr_offs_final.mul(1.f - m_fZoomRotationFactor);
    curr_rot_final.mul(1.f - m_fZoomRotationFactor);

    Fmatrix hud_rotation;
    hud_rotation.identity();
    hud_rotation.rotateX(curr_rot_final.x);

    Fmatrix hud_rotation_y;
    hud_rotation_y.identity();
    hud_rotation_y.rotateY(curr_rot_final.y);
    hud_rotation.mulA_43(hud_rotation_y);

    hud_rotation_y.identity();
    hud_rotation_y.rotateZ(curr_rot_final.z);
    hud_rotation.mulA_43(hud_rotation_y);

    hud_rotation.translate_over(curr_offs_final);
    trans.mulB_43(hud_rotation);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CHudItem::UpdateInertion(Fmatrix& trans)
{
    if (HudInertionEnabled() && HudInertionAllowed())
    {
        Fmatrix xform;
        Fvector& origin = trans.c;
        xform = trans;

        // calc difference
        Fvector diff_dir;
        diff_dir.sub(xform.k, inert_st_last_dir);

        // clamp by PI_DIV_2
        Fvector last;
        last.normalize_safe(inert_st_last_dir);
        const float dot = last.dotproduct(xform.k);
        if (dot < EPS)
        {
            Fvector v0;
            v0.crossproduct(inert_st_last_dir, xform.k);
            inert_st_last_dir.crossproduct(xform.k, v0);
            diff_dir.sub(xform.k, inert_st_last_dir);
        }

        // tend to forward
        float _tendto_speed, _origin_offset;
        if (GetCurrentHudOffsetIdx() > 0)
        { // Худ в режиме "Прицеливание"
            float factor = GetInertionFactor();
            _tendto_speed = inertion_data.m_tendto_speed_aim - (inertion_data.m_tendto_speed_aim - inertion_data.m_tendto_speed) * factor;
            _origin_offset = inertion_data.m_origin_offset_aim - (inertion_data.m_origin_offset_aim - inertion_data.m_origin_offset) * factor;
        }
        else
        { // Худ в режиме "От бедра"
            _tendto_speed = inertion_data.m_tendto_speed;
            _origin_offset = inertion_data.m_origin_offset;
        }

        // Фактор силы инерции
        const float power_factor = GetInertionPowerFactor();
        _tendto_speed *= power_factor;
        _origin_offset *= power_factor;

        inert_st_last_dir.mad(diff_dir, _tendto_speed * Device.fTimeDelta);
        origin.mad(diff_dir, _origin_offset);

        // pitch compensation
        float pitch = angle_normalize_signed(xform.k.getP());

        pitch *= GetInertionFactor();

        // Отдаление\приближение
        origin.mad(xform.k, -pitch * inertion_data.m_pitch_offset_d);

        // Сдвиг в противоположную часть экрана
        origin.mad(xform.i, -pitch * inertion_data.m_pitch_offset_r);

        // Подьём\опускание
        clamp(pitch, inertion_data.m_pitch_low_limit, PI);
        origin.mad(xform.j, -pitch * inertion_data.m_pitch_offset_n);
    }
}

// Обновление координат текущего худа
void CHudItem::UpdateHudAdditional(Fmatrix& trans, const bool need_update_collision)
{
    if (!need_update_collision)
        return;

    UpdateInertion(trans);

    Fvector summary_offset{}, summary_rotate{};

    attachable_hud_item* hi = HudItemData();
    u8 idx = GetCurrentHudOffsetIdx();
    const bool b_aiming = idx != hud_item_measures::m_hands_offset_type_normal;
    Fvector zr_offs = hi->m_measures.m_hands_offset[hud_item_measures::m_hands_offset_pos][idx];
    Fvector zr_rot = hi->m_measures.m_hands_offset[hud_item_measures::m_hands_offset_rot][idx];

    //============ Поворот ствола во время аима ===========//
    if (b_aiming)
    {
        if (IsZoomed())
            m_fZoomRotationFactor += Device.fTimeDelta / m_fZoomRotateTime;
        else
            m_fZoomRotationFactor -= Device.fTimeDelta / m_fZoomRotateTime;

        clamp(m_fZoomRotationFactor, 0.f, 1.f);

        zr_offs.mul(m_fZoomRotationFactor);
        zr_rot.mul(m_fZoomRotationFactor);

        summary_offset.add(zr_offs);
    }
    //====================================================//

    auto pActor = smart_cast<const CActor*>(object().H_Parent());
    const u32 iMovingState = pActor->MovingState();
    idx = b_aiming ? 1ui8 : 0ui8;

    //============= Боковой стрейф с оружием =============//
    {
        const bool bEnabled = m_strafe_offset[2][idx].x;
        if (!bEnabled)
            goto LOOKOUT_EFFECT;

        // Рассчитываем фактор боковой ходьбы
        float fStrafeMaxTime = m_strafe_offset[2][idx].y; // Макс. время в секундах, за которое мы наклонимся из центрального положения
        if (fStrafeMaxTime <= EPS)
            fStrafeMaxTime = 0.01f;

        const float fStepPerUpd = Device.fTimeDelta / fStrafeMaxTime; // Величина изменение фактора поворота

        if (iMovingState & mcLStrafe)
        { // Движемся влево
            float fVal = (m_fLR_MovingFactor > 0.f ? fStepPerUpd * 3 : fStepPerUpd);
            m_fLR_MovingFactor -= fVal;
        }
        else if (iMovingState & mcRStrafe)
        { // Движемся вправо
            float fVal = (m_fLR_MovingFactor < 0.f ? fStepPerUpd * 3 : fStepPerUpd);
            m_fLR_MovingFactor += fVal;
        }
        else
        { // Двигаемся в любом другом направлении
            if (m_fLR_MovingFactor < 0.0f)
            {
                m_fLR_MovingFactor += fStepPerUpd;
                clamp(m_fLR_MovingFactor, -1.0f, 0.0f);
            }
            else
            {
                m_fLR_MovingFactor -= fStepPerUpd;
                clamp(m_fLR_MovingFactor, 0.0f, 1.0f);
            }
        }

        clamp(m_fLR_MovingFactor, -1.0f, 1.0f); // Фактор боковой ходьбы не должен превышать эти лимиты

        // Смещение позиции худа в стрейфе
        Fvector moving_offs = m_strafe_offset[0][idx]; // pos
        moving_offs.mul(m_fLR_MovingFactor); // Умножаем на фактор стрейфа

        // Поворот худа в стрейфе
        Fvector moving_rot = m_strafe_offset[1][idx]; // rot
        moving_rot.mul(-PI / 180.f); // Преобразуем углы в радианы
        moving_rot.mul(m_fLR_MovingFactor); // Умножаем на фактор стрейфа

        if (idx == 0)
        { // От бедра
            moving_offs.mul(1.f - m_fZoomRotationFactor);
            moving_rot.mul(1.f - m_fZoomRotationFactor);
        }
        else
        { // Во время аима
            moving_offs.mul(m_fZoomRotationFactor);
            moving_rot.mul(m_fZoomRotationFactor);
        }

        summary_offset.add(moving_offs);
        summary_rotate.add(moving_rot);
    }
    //====================================================//

LOOKOUT_EFFECT:
    //=============== Эффекты выглядываний ===============//
    {
        const bool bEnabled = m_lookout_offset[2][idx].x;
        if (!bEnabled)
            goto JUMP_EFFECT;

        float fLookoutMaxTime = m_lookout_offset[2][idx].y; // Макс. время в секундах, за которое мы наклонимся из центрального положения
        if (fLookoutMaxTime <= EPS)
            fLookoutMaxTime = 0.01f;

        const float fStepPerUpdL = Device.fTimeDelta / fLookoutMaxTime; // Величина изменение фактора поворота

        if ((iMovingState & mcLLookout) && !(iMovingState & mcRLookout))
        { // Выглядываем влево
            if (!IsZoomed())
            {
                m_fAimLookout_MovingFactor += fStepPerUpdL;
                m_fLookout_MovingFactor -= fStepPerUpdL;
            }
            else
            {
                m_fAimLookout_MovingFactor -= fStepPerUpdL;
                m_fLookout_MovingFactor += fStepPerUpdL;
            }
        }
        else if ((iMovingState & mcRLookout) && !(iMovingState & mcLLookout))
        { // Выглядываем вправо

            if (!IsZoomed())
            {
                m_fAimLookout_MovingFactor -= fStepPerUpdL;
                m_fLookout_MovingFactor += fStepPerUpdL;
            }
            else
            {
                m_fAimLookout_MovingFactor += fStepPerUpdL;
                m_fLookout_MovingFactor -= fStepPerUpdL;
            }
        }
        else
        { // Двигаемся в любом другом направлении
            if (m_fLookout_MovingFactor < 0.0f)
            {
                m_fLookout_MovingFactor += fStepPerUpdL;
                clamp(m_fLookout_MovingFactor, -1.0f, 0.0f);
            }
            else
            {
                m_fLookout_MovingFactor -= fStepPerUpdL;
                clamp(m_fLookout_MovingFactor, 0.0f, 1.0f);
            }

            if (m_fAimLookout_MovingFactor < 0.0f)
            {
                m_fAimLookout_MovingFactor += fStepPerUpdL;
                clamp(m_fAimLookout_MovingFactor, -1.0f, 0.0f);
            }
            else
            {
                m_fAimLookout_MovingFactor -= fStepPerUpdL;
                clamp(m_fAimLookout_MovingFactor, 0.0f, 1.0f);
            }
        }

        clamp(m_fAimLookout_MovingFactor, -1.0f, 1.0f); // не должен превышать эти лимиты
        clamp(m_fLookout_MovingFactor, -1.0f, 1.0f); // не должен превышать эти лимиты

        float koef{1.f};
        if ((iMovingState & mcCrouch) && (iMovingState & mcAccel))
            koef = 0.5; // во сколько раз менять амплитуду при полном присяде
        else if (iMovingState & mcCrouch)
            koef = 0.75; // во сколько раз менять амплитуду при присяде

        // Смещение позиции худа
        Fvector lookout_offs = m_lookout_offset[0][0]; // pos
        lookout_offs.mul(koef);
        lookout_offs.mul(m_fLookout_MovingFactor); // Умножаем на фактор наклона

        // Поворот худа
        Fvector lookout_rot = m_lookout_offset[1][0]; // rot
        lookout_rot.mul(koef);
        lookout_rot.mul(-PI / 180.f); // Преобразуем углы в радианы
        lookout_rot.mul(m_fLookout_MovingFactor); // Умножаем на фактор наклона

        // Смещение позиции худа
        Fvector aim_lookout_offs = m_lookout_offset[0][1]; // pos
        aim_lookout_offs.mul(koef);
        aim_lookout_offs.mul(m_fAimLookout_MovingFactor); // Умножаем на фактор наклона

        // Поворот худа
        Fvector aim_lookout_rot = m_lookout_offset[1][1]; // rot
        aim_lookout_rot.mul(koef);
        aim_lookout_rot.mul(-PI / 180.f); // Преобразуем углы в радианы
        aim_lookout_rot.mul(m_fAimLookout_MovingFactor); // Умножаем на фактор наклона

        if (!IsZoomed())
        { // От бедра
            aim_lookout_offs.mul(m_fZoomRotationFactor);
            aim_lookout_rot.mul(m_fZoomRotationFactor);
            lookout_offs.mul(1.f - m_fZoomRotationFactor);
            lookout_rot.mul(1.f - m_fZoomRotationFactor);
        }
        else
        { // Во время аима
            aim_lookout_offs.mul(m_fZoomRotationFactor);
            aim_lookout_rot.mul(m_fZoomRotationFactor);
            lookout_offs.mul(1.f - m_fZoomRotationFactor);
            lookout_rot.mul(1.f - m_fZoomRotationFactor);
        }

        summary_offset.add(lookout_offs + aim_lookout_offs);
        summary_rotate.add(lookout_rot + aim_lookout_rot);
    }
    //====================================================//

JUMP_EFFECT:
    //=============== Эффекты прыжка ===============//
    {
        const bool bEnabled = m_jump_offset[2][idx].x;
        if (!bEnabled)
            goto APPLY_EFFECTS;

        float fJumpMaxTime = m_jump_offset[2][idx].y; // Макс. время в секундах, за которое произойдет смещение худа при прыжке
        float fFallMaxTime = m_fall_offset[2][idx].y; // Макс. время в секундах, за которое произойдет смещение худа при падении

        if (fJumpMaxTime <= EPS)
            fJumpMaxTime = 0.01f;

        if (fFallMaxTime <= EPS)
            fFallMaxTime = 0.01f;

        const float fJumpPerUpd = Device.fTimeDelta / fJumpMaxTime; // Величина изменение фактора смещения худа при прыжке
        const float fFallPerUpd = Device.fTimeDelta / fFallMaxTime; // Величина изменение фактора смещения худа при падении

        if (iMovingState & mcJump)
        { // Прыжок
            m_fJump_MovingFactor += fJumpPerUpd;
            m_fFall_MovingFactor -= fFallPerUpd;
        }
        else if (iMovingState & mcFall)
        { // Падание
            m_fJump_MovingFactor -= fJumpPerUpd;
            m_fFall_MovingFactor += fFallPerUpd;
        }
        else
        { // Двигаемся в любом другом направлении
            if (m_fJump_MovingFactor < 0.0f && m_fFall_MovingFactor < 0.0f)
            {
                m_fJump_MovingFactor += fJumpPerUpd * 2.f;
                m_fFall_MovingFactor += fFallPerUpd * 2.f;
            }
            else
            {
                m_fJump_MovingFactor -= fJumpPerUpd * 2.f;
                m_fFall_MovingFactor -= fFallPerUpd * 2.f;
            }
        }

        clamp(m_fJump_MovingFactor, 0.0f, 1.0f); // не должен превышать эти лимиты
        clamp(m_fFall_MovingFactor, 0.0f, 1.0f); // не должен превышать эти лимиты

        // Смещение позиции худа в стрейфе
        Fvector jump_offs = m_jump_offset[0][idx]; // pos
        jump_offs.mul(m_fJump_MovingFactor); // Умножаем на фактор эффекта

        // Поворот худа в стрейфе
        Fvector jump_rot = m_jump_offset[1][idx]; // rot
        jump_rot.mul(-PI / 180.f); // Преобразуем углы в радианы
        jump_rot.mul(m_fJump_MovingFactor); // Умножаем на фактор эффекта

        // Смещение позиции худа в стрейфе
        Fvector fall_offs = m_fall_offset[0][idx]; // pos
        fall_offs.mul(m_fFall_MovingFactor); // Умножаем на фактор эффекта

        // Поворот худа в стрейфе
        Fvector fall_rot = m_fall_offset[1][idx]; // rot
        fall_rot.mul(-PI / 180.f); // Преобразуем углы в радианы
        fall_rot.mul(m_fFall_MovingFactor); // Умножаем на фактор эффекта

        if (idx == 0)
        { // От бедра
            jump_offs.mul(1.f - m_fZoomRotationFactor);
            jump_rot.mul(1.f - m_fZoomRotationFactor);
            fall_offs.mul(1.f - m_fZoomRotationFactor);
            fall_rot.mul(1.f - m_fZoomRotationFactor);
        }
        else
        { // Во время аима
            jump_offs.mul(m_fZoomRotationFactor);
            jump_rot.mul(m_fZoomRotationFactor);
            fall_offs.mul(m_fZoomRotationFactor);
            fall_rot.mul(m_fZoomRotationFactor);
        }

        summary_offset.add(jump_offs + fall_offs);
        summary_rotate.add(jump_rot + fall_rot);
    }
    //====================================================//

APPLY_EFFECTS:
    //================ Применение эффектов ===============//
    {
        // поворот с сохранением смещения by Zander
        Fvector _angle{}, _pos{trans.c};
        trans.getHPB(_angle);
        _angle.add(-summary_rotate);
        // Msg("##[%s] summary_rotate: [%f,%f,%f]", __FUNCTION__, summary_rotate.x, summary_rotate.y, summary_rotate.z);
        trans.setHPB(_angle.x, _angle.y, _angle.z);
        trans.c = _pos;

        Fmatrix hud_rotation;
        hud_rotation.identity();

        if (b_aiming)
        {
            hud_rotation.rotateX(zr_rot.x);

            Fmatrix hud_rotation_y;
            hud_rotation_y.identity();
            hud_rotation_y.rotateY(zr_rot.y);
            hud_rotation.mulA_43(hud_rotation_y);

            hud_rotation_y.identity();
            hud_rotation_y.rotateZ(zr_rot.z);
            hud_rotation.mulA_43(hud_rotation_y);
            // Msg("~~[%s] zr_rot: [%f,%f,%f]", __FUNCTION__, zr_rot.x, zr_rot.y, zr_rot.z);
        }
        // Msg("--[%s] summary_offset: [%f,%f,%f]", __FUNCTION__, summary_offset.x, summary_offset.y, summary_offset.z);
        hud_rotation.translate_over(summary_offset);
        trans.mulB_43(hud_rotation);
    }
    //====================================================//

    UpdateCollision(trans);
}

float CHudItem::GetHudFov()
{
    // Рассчитываем HUD FOV от бедра (с учётом упирания в стены)
    if (m_nearwall_on)
    {
        // Получаем расстояние от камеры до точки в прицеле
        float dist = CollisionAllowed() ? saved_rq_range : HUD().GetCurrentRayQuery().range;

        // Интерполируем расстояние в диапазон от 0 (min) до 1 (max)
        clamp(dist, m_nearwall_dist_min, m_nearwall_dist_max);
        const float fDistanceMod = ((dist - m_nearwall_dist_min) / (m_nearwall_dist_max - m_nearwall_dist_min)); // 0.f ... 1.f

        // Плавно высчитываем итоговый FOV от бедра
        float src = m_nearwall_speed_mod * Device.fTimeDelta;
        clamp(src, 0.f, 1.f);

        const float fTrgFov = (IsZoomed() ? m_nearwall_target_aim_hud_fov : m_nearwall_target_hud_fov) +
            fDistanceMod * ((m_base_fov > 0.0f ? m_base_fov : psHUD_FOV_def) - (IsZoomed() ? m_nearwall_target_aim_hud_fov : m_nearwall_target_hud_fov));
        m_nearwall_last_hud_fov = m_nearwall_last_hud_fov * (1 - src) + fTrgFov * src;
    }

    return m_nearwall_last_hud_fov;
}

constexpr const char* BOBBING_SECT = "wpn_bobbing_effector";
constexpr float SPEED_REMINDER = 5.f;
CHudItem::CWeaponBobbing::CWeaponBobbing(CHudItem* parent) : parent_hud_item(parent)
{
    fTime = 0.f;
    fReminderFactor = 0.f;
    is_limping = false;

    m_fAmplitudeController = READ_IF_EXISTS(pSettings, r_float, BOBBING_SECT, "controller_amplitude", 0.003f);
    m_fAmplitudeRun = READ_IF_EXISTS(pSettings, r_float, BOBBING_SECT, "run_amplitude", 0.0075f);
    m_fAmplitudeWalk = READ_IF_EXISTS(pSettings, r_float, BOBBING_SECT, "walk_amplitude", 0.005f);
    m_fAmplitudeLimp = READ_IF_EXISTS(pSettings, r_float, BOBBING_SECT, "limp_amplitude", 0.011f);

    m_fSpeedRun = READ_IF_EXISTS(pSettings, r_float, BOBBING_SECT, "run_speed", 6.74f);
    m_fSpeedWalk = READ_IF_EXISTS(pSettings, r_float, BOBBING_SECT, "walk_speed", 6.26f);
    m_fSpeedLimp = READ_IF_EXISTS(pSettings, r_float, BOBBING_SECT, "limp_speed", 4.6f);

    m_fCrouchFactor = READ_IF_EXISTS(pSettings, r_float, BOBBING_SECT, "crouch_k", 0.75f);
    m_fZoomFactor = READ_IF_EXISTS(pSettings, r_float, BOBBING_SECT, "zoom_k", 1.f);
    m_fScopeZoomFactor = READ_IF_EXISTS(pSettings, r_float, BOBBING_SECT, "scope_zoom_k", m_fZoomFactor);
}

void CHudItem::CWeaponBobbing::CheckState()
{
    dwMState = Actor()->get_state();
    is_limping = Actor()->conditions().IsLimping();
    m_bZoomMode = Actor()->IsZoomAimingMode();
    fTime += Device.fTimeDelta;
}

void CHudItem::CWeaponBobbing::Update(Fmatrix& m, Fmatrix& m2)
{
    CheckState();

    // mmccxvii: Тряска рук при воздействии контролера
    const bool ControllerCondition = Actor()->PsyAuraAffect;

    if ((dwMState & ACTOR_DEFS::mcAnyMove) || ControllerCondition)

    {
        if (fReminderFactor < 1.f)
            fReminderFactor += SPEED_REMINDER * Device.fTimeDelta;
        else
            fReminderFactor = 1.f;
    }
    else
    {
        if (fReminderFactor > 0.f)
            fReminderFactor -= SPEED_REMINDER * Device.fTimeDelta;
        else
            fReminderFactor = 0.f;
    }

    if (!fsimilar(fReminderFactor, 0))
    {
        float k = (dwMState & ACTOR_DEFS::mcCrouch) ? m_fCrouchFactor : 1.f;
        float k2 = k;

        if (m_bZoomMode)
        {
            float zoom_factor = m_fZoomFactor;

            auto wpn = smart_cast<CWeapon*>(parent_hud_item);
            if (wpn && wpn->IsScopeAttached() && !wpn->IsGrenadeMode())
                zoom_factor = m_fScopeZoomFactor;

            k2 *= zoom_factor;
        }

        float A, ST;

        // mmccxvii: Тряска рук при воздействии контролера
        if (ControllerCondition)
        {
            A = m_fAmplitudeController * k;
            ST = m_fSpeedRun * 10 * fTime * k;
        }
        else if (isActorAccelerated(dwMState, m_bZoomMode))
        {
            A = m_fAmplitudeRun * k2;
            ST = m_fSpeedRun * fTime * k;
        }
        else if (is_limping)
        {
            A = m_fAmplitudeLimp * k2;
            ST = m_fSpeedLimp * fTime * k;
        }
        else
        {
            A = m_fAmplitudeWalk * k2;
            ST = m_fSpeedWalk * fTime * k;
        }

        float _sinA = _abs(_sin(ST) * A) * fReminderFactor;

        // mmccxvii: Тряска рук при воздействии контролера
        if (ControllerCondition)
            _sinA *= ::Random.randF(-1.0f, 1.0f);

        float _cosA = _cos(ST) * A * fReminderFactor;

        // применяем к матрице положения рук

        Fvector dangle;
        Fmatrix R, mR;

        dangle.x = _cosA;
        dangle.z = _cosA;
        dangle.y = _sinA;

        R.setHPB(dangle.x, dangle.y, dangle.z);

        m.c.y += _sinA;

        mR.mul(m, R);

        m.k.set(mR.k);
        m.j.set(mR.j);

        m2.c.y += _sinA;

        mR.mul(m2, R);

        m2.k.set(mR.k);
        m2.j.set(mR.j);
    }
}

void CHudItem::GetBoneOffsetPosDir(const shared_str& bone_name, Fvector& dest_pos, Fvector& dest_dir, const Fvector& offset)
{
    const u16 bone_id = HudItemData()->m_model->LL_BoneID(bone_name);
    ASSERT_FMT(bone_id != BI_NONE, "!![%s] bone [%s] not found in weapon [%s]", __FUNCTION__, bone_name.c_str(), world_sect.c_str());
    Fmatrix& fire_mat = HudItemData()->m_model->LL_GetTransform(bone_id);
    fire_mat.transform_tiny(dest_pos, offset);
    HudItemData()->m_item_transform.transform_tiny(dest_pos);
    dest_pos.add(Device.vCameraPosition);
    dest_dir.set(0.f, 0.f, 1.f);
    HudItemData()->m_item_transform.transform_dir(dest_dir);
}

extern ENGINE_API float psHUD_FOV;

void CHudItem::CorrectDirFromWorldToHud(Fvector& dir)
{
    const float Fov = Device.fFOV;
    const float HudFov = psHUD_FOV <= 1.f ? psHUD_FOV * Device.fFOV : psHUD_FOV;
    const float diff = hud_recalc_koef * Fov / HudFov;
    const auto& CamDir = Device.vCameraDirection;
    dir.sub(CamDir);
    dir.mul(diff);
    dir.add(CamDir);
    dir.normalize();
}

void CHudItem::TimeLockAnimation()
{
    if (GetState() != eDeviceSwitch)
        return;

    string128 anm_time_param;
    xr_strconcat(anm_time_param, "lock_time_end_", m_current_motion.c_str());
    const float time = READ_IF_EXISTS(pSettings, r_float, HudSection(), anm_time_param, 0) * 1000.f; // Читаем с конфига время анимации (например, lock_time_end_anm_reload)
    const float current_time = Device.dwTimeGlobal - m_dwMotionStartTm;
    if (time && current_time >= time)
        DeviceUpdate();
}

void CHudItem::OnAnimationEnd(u32 state)
{
    switch (state)
    {
    case eSprintStart:
        SprintType = true;
        SwitchState(eIdle);
        break;
    case eSprintEnd:
        SprintType = false;
        SwitchState(eIdle);
        break;
    }
}

bool CHudItem::AnmIdleMovingAllowed() const { return !HudBobbingAllowed() || Actor()->PsyAuraAffect; }

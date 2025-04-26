#include "stdafx.h"
#include "WeaponBinocularsVision.h"
#include "WeaponBinoculars.h"
#include "ui\UIFrameWindow.h"
#include "entity_alive.h"
#include "visual_memory_manager.h"
#include "actor.h"
#include "actor_memory.h"
#include "relation_registry.h"
#include "object_broker.h"

#include "game_base_space.h"
#include "Level.h"
#include "game_cl_base.h"
#include "AI/Monsters/BaseMonster/base_monster.h"
#include "..\xr_3da\IGame_Persistent.h"
#include "HUDTarget.h"

#define RECT_SIZE 16

void SBinocVisibleObj::create_default(u32 color)
{
    m_lt.Init("ui\\ui_enemy_frame", 0, 0, RECT_SIZE, RECT_SIZE);
    m_lb.Init("ui\\ui_enemy_frame", 0, 0, RECT_SIZE, RECT_SIZE);
    m_rt.Init("ui\\ui_enemy_frame", 0, 0, RECT_SIZE, RECT_SIZE);
    m_rb.Init("ui\\ui_enemy_frame", 0, 0, RECT_SIZE, RECT_SIZE);

    m_lt.SetOriginalRect(0, 0, RECT_SIZE, RECT_SIZE);
    m_lb.SetOriginalRect(0, RECT_SIZE, RECT_SIZE, RECT_SIZE);
    m_rt.SetOriginalRect(RECT_SIZE, 0, RECT_SIZE, RECT_SIZE);
    m_rb.SetOriginalRect(RECT_SIZE, RECT_SIZE, RECT_SIZE, RECT_SIZE);

    u32 clr = subst_alpha(color, 128);
    m_lt.SetColor(clr);
    m_lb.SetColor(clr);
    m_rt.SetColor(clr);
    m_rb.SetColor(clr);

    cur_rect.set(0, 0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

    m_flags.zero();
}

void SBinocVisibleObj::Draw()
{
    if (m_flags.test(flVisObjNotValid))
        return;

    m_lt.Draw();
    m_lb.Draw();
    m_rt.Draw();
    m_rb.Draw();
}

void SBinocVisibleObj::Update()
{
    m_flags.set(flVisObjNotValid, TRUE);
    if (m_visible_time > Device.dwTimeGlobal)
        return;

    Fbox b = m_object->Visual()->getVisData().box;

    Fmatrix xform;
    xform.mul(Device.mFullTransform, m_object->XFORM());
    Fvector2 mn = {flt_max, flt_max}, mx = {flt_min, flt_min};

    for (u32 k = 0; k < 8; ++k)
    {
        Fvector p;
        b.getpoint(k, p);
        xform.transform(p);
        mn.x = _min(mn.x, p.x);
        mn.y = _min(mn.y, p.y);
        mx.x = _max(mx.x, p.x);
        mx.y = _max(mx.y, p.y);
    }
    static Frect screen_rect = {-1.0f, -1.0f, 1.0f, 1.0f};

    Frect new_rect;
    new_rect.lt = mn;
    new_rect.rb = mx;

    if (FALSE == screen_rect.intersected(new_rect))
        return;
    if (new_rect.in(screen_rect.lt) && new_rect.in(screen_rect.rb))
        return;

    std::swap(mn.y, mx.y);
    mn.x = (1.f + mn.x) / 2.f * UI_BASE_WIDTH;
    mx.x = (1.f + mx.x) / 2.f * UI_BASE_WIDTH;
    mn.y = (1.f - mn.y) / 2.f * UI_BASE_HEIGHT;
    mx.y = (1.f - mx.y) / 2.f * UI_BASE_HEIGHT;

    if (m_flags.is(flTargetLocked))
    {
        cur_rect.lt.set(mn);
        cur_rect.rb.set(mx);
    }
    else
    {
        cur_rect.lt.x += (mn.x - cur_rect.lt.x) * m_upd_speed * Device.fTimeDelta;
        cur_rect.lt.y += (mn.y - cur_rect.lt.y) * m_upd_speed * Device.fTimeDelta;
        cur_rect.rb.x += (mx.x - cur_rect.rb.x) * m_upd_speed * Device.fTimeDelta;
        cur_rect.rb.y += (mx.y - cur_rect.rb.y) * m_upd_speed * Device.fTimeDelta;
        if (mn.similar(cur_rect.lt, 2.f) && mx.similar(cur_rect.rb, 2.f))
        {
            // target locked
            m_flags.set(flTargetLocked, TRUE);
            u32 clr = subst_alpha(m_lt.GetColor(), 255);

            if (auto* pActor = Actor())
            {
                CInventoryOwner* our_inv_owner = smart_cast<CInventoryOwner*>(pActor);
                CInventoryOwner* others_inv_owner = smart_cast<CInventoryOwner*>(m_object);
                CBaseMonster* monster = smart_cast<CBaseMonster*>(m_object);

                if (our_inv_owner && others_inv_owner && !monster)
                {
                    switch (RELATION_REGISTRY().GetRelationType(others_inv_owner, our_inv_owner))
                    {
                    case ALife::eRelationTypeEnemy: clr = C_ON_ENEMY; break;
                    case ALife::eRelationTypeNeutral: clr = C_ON_NEUTRAL; break;
                    case ALife::eRelationTypeFriend: clr = C_ON_FRIEND; break;
                    }
                }
            }

            m_lt.SetColor(clr);
            m_lb.SetColor(clr);
            m_rt.SetColor(clr);
            m_rb.SetColor(clr);
        }
    }

    m_lt.SetWndPos((cur_rect.lt.x) + 2, (cur_rect.lt.y) + 2);
    m_lb.SetWndPos((cur_rect.lt.x) + 2, (cur_rect.rb.y) - 14);
    m_rt.SetWndPos((cur_rect.rb.x) - 14, (cur_rect.lt.y) + 2);
    m_rb.SetWndPos((cur_rect.rb.x) - 14, (cur_rect.rb.y) - 14);

    m_flags.set(flVisObjNotValid, FALSE);
}

CBinocularsVision::CBinocularsVision(CWeaponMagazined* parent)
{
    m_parent = parent;
    Load(m_parent->cNameSect());
}
CBinocularsVision::~CBinocularsVision()
{
    m_snd_found.destroy();
}

void CBinocularsVision::Update()
{
    if (m_parent->Is3dssEnabled())
        return;

    const CActor* pActor = Actor();
    if (!pActor)
        return;

    const CVisualMemoryManager::VISIBLES& vVisibles = pActor->memory().visual().objects();

    for (auto& vis : m_active_objects)
        vis->m_flags.set(flVisObjNotValid, TRUE);

    CVisualMemoryManager::VISIBLES::const_iterator v_it = vVisibles.begin();
    for (; v_it != vVisibles.end(); ++v_it)
    {
        const CObject* _object_ = (*v_it).m_object;
        const CGameObject* GO = smart_cast<const CGameObject*>(_object_);
        if (!pActor->memory().visual().visible_right_now(GO))
            continue;
        if (pActor->memory().visual().visible_transparency_threshold(GO) < m_transparency_threshold)
            continue;

        CObject* object_ = const_cast<CObject*>(_object_);

        CEntityAlive* EA = smart_cast<CEntityAlive*>(object_);
        if (!EA || !EA->g_Alive())
            continue;

        auto found = std::find_if(m_active_objects.begin(), m_active_objects.end(), [object_](const auto& vis) { return (object_ == vis->m_object); });

        if (found != m_active_objects.end())
        {
            (*found)->m_flags.set(flVisObjNotValid, FALSE);
        }
        else
        {
            auto& new_vis_obj = m_active_objects.emplace_back(std::make_unique<SBinocVisibleObj>());
            new_vis_obj->m_flags.set(flVisObjNotValid, FALSE);
            new_vis_obj->m_object = object_;
            new_vis_obj->create_default(m_frame_color.get());
            new_vis_obj->m_upd_speed = m_rotating_speed;
            new_vis_obj->m_visible_time = Device.dwTimeGlobal + m_min_visible_time;
            if (NULL == m_snd_found._feedback())
                m_snd_found.play_at_pos(0, Fvector().set(0, 0, 0), sm_2D);
        }
    }

    m_active_objects.erase(std::remove_if(m_active_objects.begin(), m_active_objects.end(),
                                          [](auto& vis) {
                                              if (vis->m_flags.test(flVisObjNotValid))
                                                  return true;
                                              else
                                              {
                                                  vis->Update();
                                                  return false;
                                              }
                                          }),
                           m_active_objects.end());
}

void CBinocularsVision::Draw()
{
    if (m_parent->Is3dssEnabled())
        return;

    for (auto& vis : m_active_objects)
        vis->Draw();
}

void CBinocularsVision::Load(const shared_str& section)
{
    m_rotating_speed = pSettings->r_float(section, "vis_frame_speed");
    m_frame_color = pSettings->r_fcolor(section, "vis_frame_color");
    m_snd_found.create(pSettings->r_string(section, "found_snd"), st_Effect, sg_SourceType);
    m_min_visible_time = READ_IF_EXISTS(pSettings, r_u32, section, "min_visible_time", 0);
    m_transparency_threshold = READ_IF_EXISTS(pSettings, r_float, section, "transparency_threshold", 0.f);
}

void CBinocularsVision::remove_links(CObject* object)
{
    m_active_objects.erase(std::remove_if(m_active_objects.begin(), m_active_objects.end(), [object](const auto& vis) { return (object == vis->m_object); }),
                           m_active_objects.end());
}

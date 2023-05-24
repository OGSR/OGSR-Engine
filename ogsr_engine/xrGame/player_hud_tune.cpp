#include "stdafx.h"
#include "player_hud.h"
#include "level.h"
#include "debug_renderer.h"
#include "../xr_3da/xr_input.h"
#include "HudManager.h"
#include "HudItem.h"
#include "Weapon.h"
#include <array>

enum HUD_ADJUST_MODE : int
{
    OFF,
    HUD_POS,
    HUD_ROT,
    ITM_POS,
    ITM_ROT,
    FIRE_POINT,
    FIRE_POINT2,
    SHELL_POINT,
    ADJUST_DELTA_POS,
    ADJUST_DELTA_ROT,
    LASETDOT_POS,
    FLASHLIGHT_POS,
    _HUD_ADJUST_MODES_COUNT_
};

static constexpr std::array<std::tuple<int, const char*>, _HUD_ADJUST_MODES_COUNT_> ADJUST_MODES_DB{{
    {DIK_NUMPAD0, ""},
    {DIK_NUMPAD1, "adjusting HUD POSITION"},
    {DIK_NUMPAD2, "adjusting HUD ROTATION"},
    {DIK_NUMPAD3, "adjusting ITEM POSITION"},
    {DIK_NUMPAD4, "adjusting ITEM ROTATION"},
    {DIK_NUMPAD5, "adjusting FIRE POINT"},
    {DIK_NUMPAD6, "adjusting FIRE POINT 2"},
    {DIK_NUMPAD7, "adjusting SHELL POINT"},
    {DIK_NUMPAD8, "adjusting pos STEP"},
    {DIK_NUMPAD9, "adjusting rot STEP"},
    {DIK_1, "adjusting LASER POINT"},
    {DIK_2, "adjusting FLASHLIGHT POINT"},
}};

int g_bHudAdjustMode = OFF;
int g_bHudAdjustItemIdx = 0;
float g_bHudAdjustDeltaPos = 0.0005f;
float g_bHudAdjustDeltaRot = 0.05f;

static bool is_attachable_item_tuning_mode()
{
    return pInput->iGetAsyncKeyState(DIK_LSHIFT) || pInput->iGetAsyncKeyState(DIK_Z) || pInput->iGetAsyncKeyState(DIK_X) || pInput->iGetAsyncKeyState(DIK_C);
}

static void tune_remap(const Ivector& in_values, Ivector& out_values)
{
    if (pInput->iGetAsyncKeyState(DIK_LSHIFT))
    {
        out_values = in_values;
    }
    else if (pInput->iGetAsyncKeyState(DIK_Z))
    { // strict by X
        out_values.x = in_values.y;
        out_values.y = 0;
        out_values.z = 0;
    }
    else if (pInput->iGetAsyncKeyState(DIK_X))
    { // strict by Y
        out_values.x = 0;
        out_values.y = in_values.y;
        out_values.z = 0;
    }
    else if (pInput->iGetAsyncKeyState(DIK_C))
    { // strict by Z
        out_values.x = 0;
        out_values.y = 0;
        out_values.z = in_values.y;
    }
    else
    {
        out_values.set(0, 0, 0);
    }
}

static void calc_cam_diff_pos(const Fmatrix& item_transform, const Fvector& diff, Fvector& res)
{
    Fmatrix cam_m;
    cam_m.i.set(Device.vCameraRight);
    cam_m.j.set(Device.vCameraTop);
    cam_m.k.set(Device.vCameraDirection);
    cam_m.c.set(Device.vCameraPosition);

    Fvector res1;
    cam_m.transform_dir(res1, diff);

    Fmatrix item_transform_i;
    item_transform_i.invert(item_transform);
    item_transform_i.transform_dir(res, res1);
}

void attachable_hud_item::tune(const Ivector& values)
{
    if (!is_attachable_item_tuning_mode())
        return;

    Fvector diff{};

    if (g_bHudAdjustMode == ITM_POS || g_bHudAdjustMode == ITM_ROT)
    {
        if (g_bHudAdjustMode == ITM_POS)
        {
            if (values.x)
                diff.x = (values.x > 0) ? g_bHudAdjustDeltaPos : -g_bHudAdjustDeltaPos;
            if (values.y)
                diff.y = (values.y > 0) ? g_bHudAdjustDeltaPos : -g_bHudAdjustDeltaPos;
            if (values.z)
                diff.z = (values.z < 0) ? g_bHudAdjustDeltaPos : -g_bHudAdjustDeltaPos;

            Fvector d;
            Fmatrix ancor_m;
            m_parent->calc_transform(m_attach_place_idx, Fidentity, ancor_m);
            calc_cam_diff_pos(ancor_m, diff, d);
            m_measures.m_item_attach[0].add(d);
        }
        else if (g_bHudAdjustMode == ITM_ROT)
        {
            if (values.x)
                diff.x = (values.x > 0) ? g_bHudAdjustDeltaRot : -g_bHudAdjustDeltaRot;
            if (values.y)
                diff.y = (values.y > 0) ? g_bHudAdjustDeltaRot : -g_bHudAdjustDeltaRot;
            if (values.z)
                diff.z = (values.z > 0) ? g_bHudAdjustDeltaRot : -g_bHudAdjustDeltaRot;

            Fvector d;
            Fmatrix ancor_m;
            m_parent->calc_transform(m_attach_place_idx, Fidentity, ancor_m);

            calc_cam_diff_pos(m_item_transform, diff, d);
            m_measures.m_item_attach[1].add(d);
        }

        if ((values.x) || (values.y) || (values.z))
        {
            Log("####################################");
            Msg("[%s]", m_sect_name.c_str());
            Msg("item_position = %f,%f,%f", m_measures.m_item_attach[0].x, m_measures.m_item_attach[0].y, m_measures.m_item_attach[0].z);
            Msg("item_orientation = %f,%f,%f", m_measures.m_item_attach[1].x, m_measures.m_item_attach[1].y, m_measures.m_item_attach[1].z);
            Log("####################################");
        }
    }

    if (g_bHudAdjustMode == FIRE_POINT || g_bHudAdjustMode == FIRE_POINT2 || g_bHudAdjustMode == SHELL_POINT || g_bHudAdjustMode == LASETDOT_POS ||
        g_bHudAdjustMode == FLASHLIGHT_POS)
    {
        if (values.x)
            diff.x = (values.x > 0) ? g_bHudAdjustDeltaPos : -g_bHudAdjustDeltaPos;
        if (values.y)
            diff.y = (values.y > 0) ? g_bHudAdjustDeltaPos : -g_bHudAdjustDeltaPos;
        if (values.z)
            diff.z = (values.z > 0) ? g_bHudAdjustDeltaPos : -g_bHudAdjustDeltaPos;

        if (g_bHudAdjustMode == FIRE_POINT)
            m_measures.m_fire_point_offset.add(diff);
        else if (g_bHudAdjustMode == FIRE_POINT2)
            m_measures.m_fire_point2_offset.add(diff);
        else if (g_bHudAdjustMode == SHELL_POINT)
            m_measures.m_shell_point_offset.add(diff);
        else if (g_bHudAdjustMode == LASETDOT_POS)
        {
            if (auto Wpn = smart_cast<CWeapon*>(m_parent_hud_item))
                Wpn->laserdot_attach_offset.add(diff);
        }
        else if (g_bHudAdjustMode == FLASHLIGHT_POS)
        {
            if (auto Wpn = smart_cast<CWeapon*>(m_parent_hud_item))
                Wpn->flashlight_attach_offset.add(diff);
        }

        if ((values.x) || (values.y) || (values.z))
        {
            Log("####################################");
            Msg("[%s]", m_sect_name.c_str());
            Msg("fire_point = %f,%f,%f", m_measures.m_fire_point_offset.x, m_measures.m_fire_point_offset.y, m_measures.m_fire_point_offset.z);
            Msg("fire_point2 = %f,%f,%f", m_measures.m_fire_point2_offset.x, m_measures.m_fire_point2_offset.y, m_measures.m_fire_point2_offset.z);
            Msg("shell_point = %f,%f,%f", m_measures.m_shell_point_offset.x, m_measures.m_shell_point_offset.y, m_measures.m_shell_point_offset.z);
            if (auto Wpn = smart_cast<CWeapon*>(m_parent_hud_item))
            {
                Msg("laserdot_attach_offset = %f,%f,%f", Wpn->laserdot_attach_offset.x, Wpn->laserdot_attach_offset.y, Wpn->laserdot_attach_offset.z);
                Msg("torch_attach_offset = %f,%f,%f", Wpn->flashlight_attach_offset.x, Wpn->flashlight_attach_offset.y, Wpn->flashlight_attach_offset.z);
            }
            Log("####################################");
        }
    }
}

void attachable_hud_item::debug_draw_firedeps()
{
    const bool bForce = g_bHudAdjustMode == ITM_POS || g_bHudAdjustMode == ITM_ROT;

    if (g_bHudAdjustMode == FIRE_POINT || g_bHudAdjustMode == FIRE_POINT2 || g_bHudAdjustMode == SHELL_POINT || g_bHudAdjustMode == LASETDOT_POS ||
        g_bHudAdjustMode == FLASHLIGHT_POS || bForce)
    {
        auto& render = Level().debug_renderer();

        firedeps fd;
        setup_firedeps(fd);

        if (g_bHudAdjustMode == FIRE_POINT || bForce)
        {
            render.draw_aabb(fd.vLastFP, 0.01f, 0.01f, 0.01f, D3DCOLOR_XRGB(255, 0, 0), true);
            render.draw_aabb(fd.vLastShootPoint, 0.01f, 0.01f, 0.01f, D3DCOLOR_XRGB(5, 107, 0), true);
        }
        else if (g_bHudAdjustMode == FIRE_POINT2)
        {
            render.draw_aabb(fd.vLastFP2, 0.01f, 0.01f, 0.01f, D3DCOLOR_XRGB(0, 0, 255), true);
        }
        else if (g_bHudAdjustMode == SHELL_POINT)
        {
            render.draw_aabb(fd.vLastSP, 0.01f, 0.01f, 0.01f, D3DCOLOR_XRGB(0, 255, 0), true);
        }
        else if (g_bHudAdjustMode == LASETDOT_POS)
        {
            if (auto Wpn = smart_cast<CWeapon*>(m_parent_hud_item))
                render.draw_aabb(Wpn->laser_pos, 0.01f, 0.01f, 0.01f, D3DCOLOR_XRGB(125, 0, 0));
        }
        else if (g_bHudAdjustMode == FLASHLIGHT_POS)
        {
            if (auto Wpn = smart_cast<CWeapon*>(m_parent_hud_item))
                render.draw_aabb(Wpn->flashlight_pos, 0.01f, 0.01f, 0.01f, D3DCOLOR_XRGB(0, 56, 125));
        }
    }
}

void player_hud::tune(const Ivector& _values)
{
    Ivector values;
    tune_remap(_values, values);

    const bool is_16x9 = UI()->is_widescreen();

    if (g_bHudAdjustMode == HUD_POS || g_bHudAdjustMode == HUD_ROT)
    {
        Fvector diff{};

        float _curr_dr = g_bHudAdjustDeltaRot;

        if (!m_attached_items[g_bHudAdjustItemIdx])
            return;

        const u8 idx = m_attached_items[g_bHudAdjustItemIdx]->m_parent_hud_item->GetCurrentHudOffsetIdx();
        if (idx)
            _curr_dr /= 20.0f;

        Fvector& pos_ = (idx != 0) ? m_attached_items[g_bHudAdjustItemIdx]->hands_offset_pos() : m_attached_items[g_bHudAdjustItemIdx]->hands_attach_pos();
        Fvector& rot_ = (idx != 0) ? m_attached_items[g_bHudAdjustItemIdx]->hands_offset_rot() : m_attached_items[g_bHudAdjustItemIdx]->hands_attach_rot();

        if (g_bHudAdjustMode == HUD_POS)
        {
            if (values.x)
                diff.x = (values.x > 0) ? g_bHudAdjustDeltaPos : -g_bHudAdjustDeltaPos;
            if (values.y)
                diff.y = (values.y < 0) ? g_bHudAdjustDeltaPos : -g_bHudAdjustDeltaPos;
            if (values.z)
                diff.z = (values.z < 0) ? g_bHudAdjustDeltaPos : -g_bHudAdjustDeltaPos;

            pos_.add(diff);
        }
        else if (g_bHudAdjustMode == HUD_ROT)
        {
            if (values.x)
                diff.y = (values.x > 0) ? _curr_dr : -_curr_dr;
            if (values.y)
                diff.x = (values.y > 0) ? _curr_dr : -_curr_dr;
            if (values.z)
                diff.z = (values.z > 0) ? _curr_dr : -_curr_dr;

            rot_.add(diff);
        }

        if ((values.x) || (values.y) || (values.z))
        {
            if (idx == hud_item_measures::m_hands_offset_type_normal)
            {
                Log("####################################");
                Msg("[%s]", m_attached_items[g_bHudAdjustItemIdx]->m_sect_name.c_str());
                Msg("hands_position%s = %f,%f,%f", is_16x9 ? "_16x9" : "", pos_.x, pos_.y, pos_.z);
                Msg("hands_orientation%s = %f,%f,%f", is_16x9 ? "_16x9" : "", rot_.x, rot_.y, rot_.z);
                Log("####################################");
            }
            else if (idx == hud_item_measures::m_hands_offset_type_aim)
            {
                Log("####################################");
                Msg("[%s]", m_attached_items[g_bHudAdjustItemIdx]->m_sect_name.c_str());
                Msg("aim_hud_offset_pos%s = %f,%f,%f", is_16x9 ? "_16x9" : "", pos_.x, pos_.y, pos_.z);
                Msg("aim_hud_offset_rot%s = %f,%f,%f", is_16x9 ? "_16x9" : "", rot_.x, rot_.y, rot_.z);
                Log("####################################");
            }
            else if (idx == hud_item_measures::m_hands_offset_type_gl)
            {
                Log("####################################");
                Msg("[%s]", m_attached_items[g_bHudAdjustItemIdx]->m_sect_name.c_str());
                Msg("gl_hud_offset_pos%s = %f,%f,%f", is_16x9 ? "_16x9" : "", pos_.x, pos_.y, pos_.z);
                Msg("gl_hud_offset_rot%s	 = %f,%f,%f", is_16x9 ? "_16x9" : "", rot_.x, rot_.y, rot_.z);
                Log("####################################");
            }
            else if (idx == hud_item_measures::m_hands_offset_type_aim_scope)
            {
                Log("####################################");
                Msg("[%s]", m_attached_items[g_bHudAdjustItemIdx]->m_sect_name.c_str());
                Msg("scope_zoom_offset%s = %f,%f,%f", is_16x9 ? "_16x9" : "", pos_.x, pos_.y, pos_.z);
                Msg("scope_zoom_rotate_x%s = %f", is_16x9 ? "_16x9" : "", rot_.x);
                Msg("scope_zoom_rotate_y%s = %f", is_16x9 ? "_16x9" : "", rot_.y);
                Log("####################################");
            }
            else if (idx == hud_item_measures::m_hands_offset_type_gl_scope)
            {
                Log("####################################");
                Msg("[%s]", m_attached_items[g_bHudAdjustItemIdx]->m_sect_name.c_str());
                Msg("scope_grenade_zoom_offset%s = %f,%f,%f", is_16x9 ? "_16x9" : "", pos_.x, pos_.y, pos_.z);
                Msg("scope_grenade_zoom_rotate_x%s = %f", is_16x9 ? "_16x9" : "", rot_.x);
                Msg("scope_grenade_zoom_rotate_y%s = %f", is_16x9 ? "_16x9" : "", rot_.y);
                Log("####################################");
            }
            else if (idx == hud_item_measures::m_hands_offset_type_aim_gl_normal)
            {
                Log("####################################");
                Msg("[%s]", m_attached_items[g_bHudAdjustItemIdx]->m_sect_name.c_str());
                Msg("grenade_normal_zoom_offset%s = %f,%f,%f", is_16x9 ? "_16x9" : "", pos_.x, pos_.y, pos_.z);
                Msg("grenade_normal_zoom_rotate_x%s = %f", is_16x9 ? "_16x9" : "", rot_.x);
                Msg("grenade_normal_zoom_rotate_y%s = %f", is_16x9 ? "_16x9" : "", rot_.y);
                Log("####################################");
            }
            else if (idx == hud_item_measures::m_hands_offset_type_gl_normal_scope)
            {
                Log("####################################");
                Msg("[%s]", m_attached_items[g_bHudAdjustItemIdx]->m_sect_name.c_str());
                Msg("scope_grenade_normal_zoom_offset%s = %f,%f,%f", is_16x9 ? "_16x9" : "", pos_.x, pos_.y, pos_.z);
                Msg("scope_grenade_normal_zoom_rotate_x%s = %f", is_16x9 ? "_16x9" : "", rot_.x);
                Msg("scope_grenade_normal_zoom_rotate_y%s = %f", is_16x9 ? "_16x9" : "", rot_.y);
                Log("####################################");
            }
        }
    }
    else if (g_bHudAdjustMode == ADJUST_DELTA_POS || g_bHudAdjustMode == ADJUST_DELTA_ROT)
    {
        if (g_bHudAdjustMode == ADJUST_DELTA_POS && (values.z))
            g_bHudAdjustDeltaPos += (values.z > 0) ? 0.001f : -0.001f;

        if (g_bHudAdjustMode == ADJUST_DELTA_ROT && (values.z))
            g_bHudAdjustDeltaRot += (values.z > 0) ? 0.1f : -0.1f;
    }
    else if (auto hi = m_attached_items[g_bHudAdjustItemIdx])
        hi->tune(values);
}

void hud_draw_adjust_mode()
{
    if (!g_bHudAdjustMode)
        return;

    const char* _text{};
    if (pInput->iGetAsyncKeyState(DIK_LSHIFT))
        _text =
            "press SHIFT+NUM 0-return|1-hud_pos|2-hud_rot|3-itm_pos|4-itm_rot|5-fire_point|6-fire_point2|7-shell_point|8-pos_step|9-rot_step    ||||||    press "
            "SHIFT+1-laser_point|2-flashlight_point";
    else if (pInput->iGetAsyncKeyState(DIK_LCONTROL))
        _text = "press CTRL+NUM 0-item idx 1|1-item idx 2";
    else
        _text = std::get<1>(ADJUST_MODES_DB.at(g_bHudAdjustMode));

    if (_text)
    {
        CGameFont* F = UI()->Font()->pFontDI;
        F->SetAligment(CGameFont::alCenter);
        F->OutSetI(0.f, -0.8f);
        F->SetColor(D3DCOLOR_XRGB(125, 0, 0));
        F->OutNext(_text);
        F->OutNext("for item: [%d] [%s]", g_bHudAdjustItemIdx,
                   g_player_hud->attached_item(u16(g_bHudAdjustItemIdx)) ? g_player_hud->attached_item(u16(g_bHudAdjustItemIdx))->m_sect_name.c_str() : "NOT FOUND");
        F->OutNext("delta values: dP=[%f], dR=[%f]", g_bHudAdjustDeltaPos, g_bHudAdjustDeltaRot);
        F->OutNext("[Z]-x axis, [X]-y axis, [C]-z axis    ||||||    [<---LEFT/RIGHT--->]-x axis, [UP/DOWN]-y axis, [PageUP/PageDown]-z axis");
    }
}

void hud_adjust_mode_keyb(int dik)
{
    if (!g_bHudAdjustMode) //Включать этот режим только через консоль
        return;

    if (pInput->iGetAsyncKeyState(DIK_LSHIFT))
    {
        int mode{};
        for (const auto& [key, str] : ADJUST_MODES_DB)
        {
            if (key == dik)
            {
                g_bHudAdjustMode = mode;
                return;
            }
            mode++;
        }
    }
    else if (pInput->iGetAsyncKeyState(DIK_LCONTROL))
    {
        if (dik == DIK_NUMPAD0)
            g_bHudAdjustItemIdx = 0;
        else if (dik == DIK_NUMPAD1)
            g_bHudAdjustItemIdx = 1;
    }
}

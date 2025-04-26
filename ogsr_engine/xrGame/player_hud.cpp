#include "StdAfx.h"
#include "player_hud.h"
#include "physic_item.h"
#include "actor.h"
#include "ActorEffector.h"
#include "HudItem.h"
#include "ui_base.h"
#include "level.h"
#include "weapon.h"

player_hud* g_player_hud{};


// Рассчитать стартовую секунду анимации --#SM+#--
float CalculateMotionStartSeconds(float fStartFromTime, float fMotionLength)
{
    R_ASSERT(fStartFromTime >= -1.0f);

    //if (fStartFromTime >= 0.0f)
    //{ 
    //    // Выставляем время в точных значениях
    //    clamp(fStartFromTime, 0.0f, fMotionLength);
    //    return abs(fStartFromTime);
    //}
    //else
    {   // Выставляем время в процентных значениях (от всей длины анимации)
        return (abs(fStartFromTime) * fMotionLength);
    }
}

player_hud_motion* player_hud_motion_container::find_motion(const shared_str& name)
{
    auto it = m_anims.find(name);
    return it != m_anims.end() ? &it->second : nullptr;
}

void player_hud_motion_container::load(bool has_separated_hands, IKinematicsAnimated* model, IKinematicsAnimated* animatedHudItem, const shared_str& sect)
{
    string512 buff;
    MotionID motion_ID;

    for (const auto& [name, anm] : pSettings->r_section(sect).Data)
    {
        if ((strstr(name.c_str(), "anm_") == name.c_str() || strstr(name.c_str(), "anim_") == name.c_str()) 
            && !strstr(name.c_str(), "_speed_k") && !strstr(name.c_str(), "_start_k") && !strstr(name.c_str(), "_stop_k") && !strstr(name.c_str(), "_effector"))
        {
            player_hud_motion pm;

            if (has_separated_hands)
            {
                if (_GetItemCount(anm.c_str()) == 1)
                {
                    pm.m_base_name = anm;
                    pm.m_additional_name = anm;
                }
                else
                {
                    R_ASSERT2(_GetItemCount(anm.c_str()) == 2, anm.c_str());
                    string512 str_item;
                    _GetItem(anm.c_str(), 0, str_item);
                    pm.m_base_name = str_item;

                    _GetItem(anm.c_str(), 1, str_item);
                    pm.m_additional_name = str_item;
                }
            }
            else
            {
                string512 str_item;
                _GetItem(anm.c_str(), 0, str_item);
                pm.m_base_name = str_item;
                pm.m_additional_name = str_item;
            }

            string128 speed_param;
            xr_strconcat(speed_param, name.c_str(), "_speed_k");
            if (pSettings->line_exist(sect, speed_param))
            {
                const float k = pSettings->r_float(sect, speed_param);
                if (!fsimilar(k, 1.f) && k > 0.001f)
                    pm.params.speed_k = k;
            }

            string128 stop_param;
            xr_strconcat(stop_param, name.c_str(), "_stop_k");
            if (pSettings->line_exist(sect, stop_param))
            {
                const float k = pSettings->r_float(sect, stop_param);
                if (k < 1.f && k > 0.001f)
                    pm.params.stop_k = k;
            }

            string128 start_param;
            xr_strconcat(start_param, name.c_str(), "_start_k");
            if (pSettings->line_exist(sect, start_param))
            {
                const float k = pSettings->r_float(sect, start_param);
                if (k < 1.f && k > 0.001f)
                    pm.params.start_k = k;
            }

            IKinematicsAnimated* final_model{};
            if (model && has_separated_hands)
                final_model = model;
            else if (animatedHudItem && !has_separated_hands)
                final_model = animatedHudItem;

            // and load all motions for it

            for (u32 i = 0; i <= 8; ++i)
            {
                if (i == 0)
                    xr_strcpy(buff, pm.m_base_name.c_str());
                else
                    xr_sprintf(buff, "%s%d", pm.m_base_name.c_str(), i);

                {
                    motion_ID = final_model->ID_Cycle_Safe(buff);

                    if (motion_ID.valid())
                    {
                        auto& Anim = pm.m_animations.emplace_back();
                        Anim.mid = motion_ID;
                        Anim.name = buff;

                        string128 eff_param;
                        Anim.eff_name = READ_IF_EXISTS(pSettings, r_string, sect, xr_strconcat(eff_param, name.c_str(), "_effector"), nullptr);
                    }
                }
            }

            if (pm.m_base_name != pm.m_additional_name)
            {
                // and additiona motions for it

                for (u32 i = 0; i <= 8; ++i)
                {
                    if (i == 0)
                        xr_strcpy(buff, pm.m_additional_name.c_str());
                    else
                        xr_sprintf(buff, "%s%d", pm.m_additional_name.c_str(), i);

                    {
                        motion_ID = animatedHudItem->ID_Cycle_Safe(buff);

                        if (motion_ID.valid())
                        {
                            auto& Anim = pm.m_additional_animations.emplace_back();
                            Anim.mid = motion_ID;
                            Anim.name = buff;

                            /*string128 eff_param;
                            Anim.eff_name = READ_IF_EXISTS(pSettings, r_string, sect, xr_strconcat(eff_param, name.c_str(), "_effector"), nullptr);*/
                        }
                    }
                }

                if (pm.m_additional_animations.empty())
                {
                    MsgDbg("additional motion [%s](%s) not found in section [%s], will use main!", pm.m_additional_name.c_str(), name.c_str(), sect.c_str());

                    pm.m_additional_name = pm.m_base_name;
                }
            }

            if (pm.m_animations.empty())
            {
                if (has_separated_hands)
                {
                    FATAL("[%s] motion [%s](%s) not found in section [%s]", __FUNCTION__, pm.m_base_name.c_str(), name.c_str(), sect.c_str());
                }
                else
                {
                    Msg("! [%s] motion [%s](%s) not found in section [%s]", __FUNCTION__, pm.m_base_name.c_str(), name.c_str(), sect.c_str());
                    continue;
                }
            }

            m_anims.emplace(name, std::move(pm));
        }
    }
}

Fvector& attachable_hud_item::hands_attach_pos() { return m_measures.m_hands_attach[0]; }
Fvector& attachable_hud_item::hands_attach_rot() { return m_measures.m_hands_attach[1]; }
Fvector& attachable_hud_item::hands_offset_pos() { return m_measures.m_hands_offset[hud_item_measures::m_hands_offset_pos][m_parent_hud_item->GetCurrentHudOffsetIdx()]; }

Fvector& attachable_hud_item::hands_offset_rot() { return m_measures.m_hands_offset[hud_item_measures::m_hands_offset_rot][m_parent_hud_item->GetCurrentHudOffsetIdx()]; }

void attachable_hud_item::set_bone_visible(const shared_str& bone_name, BOOL bVisibility, BOOL bSilent)
{
    u16 bone_id;
    BOOL bVisibleNow;
    bone_id = m_model->LL_BoneID(bone_name);
    if (bone_id == BI_NONE)
    {
        if (bSilent)
            return;
        FATAL("model [%s] has no bone [%s]", m_visual_name.c_str(), bone_name.c_str());
    }
    bVisibleNow = m_model->LL_GetBoneVisible(bone_id);
    if (bVisibleNow != bVisibility)
        m_model->LL_SetBoneVisible(bone_id, bVisibility, TRUE);
}

void attachable_hud_item::set_bone_visible(const xr_vector<shared_str>& bone_names, BOOL bVisibility, BOOL bSilent)
{
    for (const auto& bone_name : bone_names)
        set_bone_visible(bone_name, bVisibility, bSilent);
}

BOOL attachable_hud_item::get_bone_visible(const shared_str& bone_name)
{
    u16 bone_id = m_model->LL_BoneID(bone_name);
    return m_model->LL_GetBoneVisible(bone_id);
}

bool attachable_hud_item::has_bone(const shared_str& bone_name)
{
    u16 bone_id = m_model->LL_BoneID(bone_name);
    return (bone_id != BI_NONE);
}

void attachable_hud_item::update(bool bForce)
{
    if (!bForce && m_upd_firedeps_frame == Device.dwFrame)
        return;
    bool is_16x9 = UI()->is_widescreen();

    if (!!m_measures.m_prop_flags.test(hud_item_measures::e_16x9_mode_now) != is_16x9)
        m_measures.load(m_sect_name, m_model);

    Fvector ypr = m_measures.m_item_attach[1];
    ypr.mul(PI / 180.f);
    m_attach_offset.setHPB(ypr.x, ypr.y, ypr.z);
    m_attach_offset.translate_over(m_measures.m_item_attach[0]);

    m_parent->calc_transform(m_attach_place_idx, m_attach_offset, m_item_transform);
    m_upd_firedeps_frame = Device.dwFrame;

    IKinematicsAnimated* ka = m_model->dcast_PKinematicsAnimated();
    if (ka)
    {
        ka->UpdateTracks();
        ka->dcast_PKinematics()->CalculateBones_Invalidate();
        ka->dcast_PKinematics()->CalculateBones(TRUE);
    }
}

player_hud_motion* attachable_hud_item::find_motion(const shared_str& name)
{ 
    return m_hand_motions.find_motion(name); 
}

void attachable_hud_item::setup_firedeps(firedeps& fd)
{
    update(false);
    // fire point&direction
    if (m_measures.m_prop_flags.test(hud_item_measures::e_fire_point))
    {
        Fmatrix& fire_mat = m_model->LL_GetTransform(m_measures.m_fire_bone);
        fire_mat.transform_tiny(fd.vLastFP, m_measures.m_fire_point_offset);
        m_item_transform.transform_tiny(fd.vLastFP);
        fd.vLastFP.add(Device.vCameraPosition);

        // KRodin придумал костыль. Из-за того, что fire_point расположен сильно впереди ствола, попробуем точку вылета пули считать от позиции fire_point.z == -0.5, т.е. ближе к
        // актору, чтобы нельзя было стрелять сквозь стены.
        if (m_measures.useCopFirePoint)
        {
            fire_mat.transform_tiny(fd.vLastShootPoint, m_measures.m_shoot_point_offset);
            m_item_transform.transform_tiny(fd.vLastShootPoint);
            fd.vLastShootPoint.add(Device.vCameraPosition);
        }
        else //На ТЧ - стволах fire_point живет от стволов отдельной жизнью, поэтому если пытаться там править координаты - всё плывёт, оставим как есть.
            fd.vLastShootPoint = fd.vLastFP;

        fd.vLastFD.set(0.f, 0.f, 1.f);
        m_item_transform.transform_dir(fd.vLastFD);
        if (m_measures.useCopFirePoint)
            m_parent_hud_item->CorrectDirFromWorldToHud(fd.vLastFD);
        VERIFY(_valid(fd.vLastFD));
        VERIFY(_valid(fd.vLastFD));

        fd.m_FireParticlesXForm.identity();
        fd.m_FireParticlesXForm.k.set(fd.vLastFD);
        Fvector::generate_orthonormal_basis_normalized(fd.m_FireParticlesXForm.k, fd.m_FireParticlesXForm.j, fd.m_FireParticlesXForm.i);
        VERIFY(_valid(fd.m_FireParticlesXForm));
    }

    if (m_measures.m_prop_flags.test(hud_item_measures::e_fire_point2))
    {
        Fmatrix& fire_mat = m_model->LL_GetTransform(m_measures.m_fire_bone2);
        fire_mat.transform_tiny(fd.vLastFP2, m_measures.m_fire_point2_offset);
        m_item_transform.transform_tiny(fd.vLastFP2);
        fd.vLastFP2.add(Device.vCameraPosition);
        VERIFY(_valid(fd.vLastFP2));
        VERIFY(_valid(fd.vLastFP2));
    }

    if (m_measures.m_prop_flags.test(hud_item_measures::e_shell_point))
    {
        Fmatrix& fire_mat = m_model->LL_GetTransform(m_measures.m_shell_bone);
        fire_mat.transform_tiny(fd.vLastSP, m_measures.m_shell_point_offset);
        m_item_transform.transform_tiny(fd.vLastSP);
        fd.vLastSP.add(Device.vCameraPosition);
        VERIFY(_valid(fd.vLastSP));
        VERIFY(_valid(fd.vLastSP));
    }
}

bool attachable_hud_item::need_renderable() { return m_parent_hud_item->need_renderable(); }

void attachable_hud_item::render(u32 context_id, IRenderable* root)
{
    ::Render->add_Visual(context_id, root, m_model->dcast_RenderVisual(), m_item_transform);
    debug_draw_firedeps();
    m_parent_hud_item->render_hud_mode(context_id, root);
}

bool attachable_hud_item::render_item_ui_query() { return m_parent_hud_item->render_item_3d_ui_query(); }

void attachable_hud_item::render_item_ui() { m_parent_hud_item->render_item_3d_ui(); }

void hud_item_measures::load(const shared_str& sect_name, IKinematics* K)
{
    bool is_16x9 = UI()->is_widescreen();
    string64 _prefix;
    xr_sprintf(_prefix, "%s", is_16x9 ? "_16x9" : "");
    string128 val_name, val_name2;

    strconcat(sizeof(val_name), val_name, "hands_position", _prefix);
    if (is_16x9 && !pSettings->line_exist(sect_name, val_name))
        xr_strcpy(val_name, "hands_position");
    m_hands_attach[0] = READ_IF_EXISTS(pSettings, r_fvector3, sect_name, val_name, Fvector{});

    strconcat(sizeof(val_name), val_name, "hands_orientation", _prefix);
    if (is_16x9 && !pSettings->line_exist(sect_name, val_name))
        xr_strcpy(val_name, "hands_orientation");
    m_hands_attach[1] = READ_IF_EXISTS(pSettings, r_fvector3, sect_name, val_name, Fvector{});

    if (!pSettings->line_exist(sect_name, "item_position") && pSettings->line_exist(sect_name, "position"))
        m_item_attach[0] = pSettings->r_fvector3(sect_name, "position");
    else
        m_item_attach[0] = pSettings->r_fvector3(sect_name, "item_position");

    if (!pSettings->line_exist(sect_name, "item_orientation") && pSettings->line_exist(sect_name, "orientation"))
        m_item_attach[1] = pSettings->r_fvector3(sect_name, "orientation");
    else
        m_item_attach[1] = pSettings->r_fvector3(sect_name, "item_orientation");

    shared_str bone_name;
    if (pSettings->line_exist(sect_name, "use_cop_fire_point"))
        useCopFirePoint = !!pSettings->r_bool(sect_name, "use_cop_fire_point");
    else
        useCopFirePoint = !!pSettings->line_exist(sect_name, "item_visual");

    if (!useCopFirePoint) // shoc configs
    {
        m_prop_flags.set(e_fire_point, pSettings->line_exist(sect_name, "fire_bone") && pSettings->line_exist(sect_name, "fire_point"));
        if (m_prop_flags.test(e_fire_point))
        {
            bone_name = pSettings->r_string(sect_name, "fire_bone");
            m_fire_bone = K->LL_BoneID(bone_name);
            ASSERT_FMT(m_fire_bone != BI_NONE, "!![%s] bone [%s] not found in weapon [%s]", __FUNCTION__, bone_name.c_str(), sect_name.c_str());
            m_fire_point_offset = pSettings->r_fvector3(sect_name, "fire_point");
        }
        else
            m_fire_point_offset.set(0.f, 0.f, 0.f);

        m_prop_flags.set(e_fire_point2, pSettings->line_exist(sect_name, "fire_bone") && pSettings->line_exist(sect_name, "fire_point2"));
        if (m_prop_flags.test(e_fire_point2))
        {
            bone_name = pSettings->r_string(sect_name, "fire_bone");
            m_fire_bone2 = K->LL_BoneID(bone_name);
            ASSERT_FMT(m_fire_bone2 != BI_NONE, "!![%s] bone [%s] not found in weapon [%s]", __FUNCTION__, bone_name.c_str(), sect_name.c_str());
            m_fire_point2_offset = pSettings->r_fvector3(sect_name, "fire_point2");
        }
        else if (m_prop_flags.test(e_fire_point))
        {
            m_prop_flags.set(e_fire_point2, true);
            bone_name = pSettings->r_string(sect_name, "fire_bone");
            m_fire_bone2 = K->LL_BoneID(bone_name);
            ASSERT_FMT(m_fire_bone2 != BI_NONE, "!![%s] bone [%s] not found in weapon [%s]", __FUNCTION__, bone_name.c_str(), sect_name.c_str());
            m_fire_point2_offset.set(m_fire_point_offset);
        }
        else
            m_fire_point2_offset.set(0.f, 0.f, 0.f);

        m_prop_flags.set(e_shell_point, pSettings->line_exist(sect_name, "fire_bone") && pSettings->line_exist(sect_name, "shell_point"));
        if (m_prop_flags.test(e_shell_point))
        {
            bone_name = pSettings->r_string(sect_name, "fire_bone");
            m_shell_bone = K->LL_BoneID(bone_name);
            ASSERT_FMT(m_shell_bone != BI_NONE, "!![%s] bone [%s] not found in weapon [%s]", __FUNCTION__, bone_name.c_str(), sect_name.c_str());
            m_shell_point_offset = pSettings->r_fvector3(sect_name, "shell_point");
        }
        else
            m_shell_point_offset.set(0.f, 0.f, 0.f);
    }
    else // cop configs
    {
        m_prop_flags.set(e_fire_point, pSettings->line_exist(sect_name, "fire_bone"));
        if (m_prop_flags.test(e_fire_point))
        {
            bone_name = pSettings->r_string(sect_name, "fire_bone");
            m_fire_bone = K->LL_BoneID(bone_name);
            ASSERT_FMT(m_fire_bone != BI_NONE, "!![%s] bone [%s] not found in weapon [%s]", __FUNCTION__, bone_name.c_str(), sect_name.c_str());
            m_fire_point_offset = pSettings->r_fvector3(sect_name, "fire_point");
            m_shoot_point_offset = READ_IF_EXISTS(pSettings, r_fvector3, sect_name, "shoot_point", (Fvector{m_fire_point_offset.x, m_fire_point_offset.y, -0.5f}));
        }
        else
            m_fire_point_offset.set(0.f, 0.f, 0.f);

        m_prop_flags.set(e_fire_point2, pSettings->line_exist(sect_name, "fire_bone2"));
        if (m_prop_flags.test(e_fire_point2))
        {
            bone_name = pSettings->r_string(sect_name, "fire_bone2");
            m_fire_bone2 = K->LL_BoneID(bone_name);
            ASSERT_FMT(m_fire_bone2 != BI_NONE, "!![%s] bone [%s] not found in weapon [%s]", __FUNCTION__, bone_name.c_str(), sect_name.c_str());
            m_fire_point2_offset = pSettings->r_fvector3(sect_name, "fire_point2");
        }
        else
            m_fire_point2_offset.set(0.f, 0.f, 0.f);

        m_prop_flags.set(e_shell_point, pSettings->line_exist(sect_name, "shell_bone"));
        if (m_prop_flags.test(e_shell_point))
        {
            bone_name = pSettings->r_string(sect_name, "shell_bone");
            m_shell_bone = K->LL_BoneID(bone_name);
            ASSERT_FMT(m_shell_bone != BI_NONE, "!![%s] bone [%s] not found in weapon [%s]", __FUNCTION__, bone_name.c_str(), sect_name.c_str());
            m_shell_point_offset = pSettings->r_fvector3(sect_name, "shell_point");
        }
        else
            m_shell_point_offset.set(0.f, 0.f, 0.f);
    }

    strconcat(sizeof(val_name), val_name, "aim_hud_offset_pos", _prefix);
    if (is_16x9 && !pSettings->line_exist(sect_name, val_name))
        xr_strcpy(val_name, "aim_hud_offset_pos");
    if (!pSettings->line_exist(sect_name, val_name) && pSettings->line_exist(sect_name, "zoom_offset"))
        m_hands_offset[m_hands_offset_pos][m_hands_offset_type_aim] = pSettings->r_fvector3(sect_name, "zoom_offset");
    else
        m_hands_offset[m_hands_offset_pos][m_hands_offset_type_aim] = READ_IF_EXISTS(pSettings, r_fvector3, sect_name, val_name, Fvector{});

    strconcat(sizeof(val_name), val_name, "aim_hud_offset_rot", _prefix);
    if (is_16x9 && !pSettings->line_exist(sect_name, val_name))
        xr_strcpy(val_name, "aim_hud_offset_rot");
    if (!pSettings->line_exist(sect_name, val_name) && pSettings->line_exist(sect_name, "zoom_rotate_x") && pSettings->line_exist(sect_name, "zoom_rotate_y"))
        m_hands_offset[m_hands_offset_rot][m_hands_offset_type_aim] =
            Fvector().set(pSettings->r_float(sect_name, "zoom_rotate_x"), pSettings->r_float(sect_name, "zoom_rotate_y"), 0.f);
    else
        m_hands_offset[m_hands_offset_rot][m_hands_offset_type_aim] = READ_IF_EXISTS(pSettings, r_fvector3, sect_name, val_name, Fvector{});

    strconcat(sizeof(val_name), val_name, "gl_hud_offset_pos", _prefix);
    if (is_16x9 && !pSettings->line_exist(sect_name, val_name))
        xr_strcpy(val_name, "gl_hud_offset_pos");
    if (!pSettings->line_exist(sect_name, val_name) && pSettings->line_exist(sect_name, "grenade_zoom_offset"))
        m_hands_offset[m_hands_offset_pos][m_hands_offset_type_gl] = pSettings->r_fvector3(sect_name, "grenade_zoom_offset");
    else
        m_hands_offset[m_hands_offset_pos][m_hands_offset_type_gl] = READ_IF_EXISTS(pSettings, r_fvector3, sect_name, val_name, Fvector{});

    strconcat(sizeof(val_name), val_name, "gl_hud_offset_rot", _prefix);
    if (is_16x9 && !pSettings->line_exist(sect_name, val_name))
        xr_strcpy(val_name, "gl_hud_offset_rot");
    if (!pSettings->line_exist(sect_name, val_name) && pSettings->line_exist(sect_name, "grenade_zoom_rotate_x") && pSettings->line_exist(sect_name, "grenade_zoom_rotate_y"))
        m_hands_offset[m_hands_offset_rot][m_hands_offset_type_gl] =
            Fvector().set(pSettings->r_float(sect_name, "grenade_zoom_rotate_x"), pSettings->r_float(sect_name, "grenade_zoom_rotate_y"), 0.f);
    else
        m_hands_offset[m_hands_offset_rot][m_hands_offset_type_gl] = READ_IF_EXISTS(pSettings, r_fvector3, sect_name, val_name, Fvector{});

    //ОГСР-специфичные параметры
    xr_strconcat(val_name, "scope_zoom_offset", _prefix);
    if (is_16x9 && !pSettings->line_exist(sect_name, val_name))
        xr_strcpy(val_name, "scope_zoom_offset");
    if (pSettings->line_exist(sect_name, val_name))
        m_hands_offset[m_hands_offset_pos][m_hands_offset_type_aim_scope] = pSettings->r_fvector3(sect_name, val_name);

    xr_strconcat(val_name, "scope_zoom_rotate_x", _prefix);
    xr_strconcat(val_name2, "scope_zoom_rotate_y", _prefix);
    if (is_16x9 && (!pSettings->line_exist(sect_name, val_name) || !pSettings->line_exist(sect_name, val_name2)))
    {
        xr_strcpy(val_name, "scope_zoom_rotate_x");
        xr_strcpy(val_name2, "scope_zoom_rotate_y");
    }
    if (pSettings->line_exist(sect_name, val_name) && pSettings->line_exist(sect_name, val_name2))
        m_hands_offset[m_hands_offset_rot][m_hands_offset_type_aim_scope] = Fvector{pSettings->r_float(sect_name, val_name), pSettings->r_float(sect_name, val_name2)};
    //
    xr_strconcat(val_name, "scope_grenade_zoom_offset", _prefix);
    if (is_16x9 && !pSettings->line_exist(sect_name, val_name))
        xr_strcpy(val_name, "scope_grenade_zoom_offset");
    if (pSettings->line_exist(sect_name, val_name))
        m_hands_offset[m_hands_offset_pos][m_hands_offset_type_gl_scope] = pSettings->r_fvector3(sect_name, val_name);

    xr_strconcat(val_name, "scope_grenade_zoom_rotate_x", _prefix);
    xr_strconcat(val_name2, "scope_grenade_zoom_rotate_y", _prefix);
    if (is_16x9 && (!pSettings->line_exist(sect_name, val_name) || !pSettings->line_exist(sect_name, val_name2)))
    {
        xr_strcpy(val_name, "scope_grenade_zoom_rotate_x");
        xr_strcpy(val_name2, "scope_grenade_zoom_rotate_y");
    }
    if (pSettings->line_exist(sect_name, val_name) && pSettings->line_exist(sect_name, val_name2))
        m_hands_offset[m_hands_offset_rot][m_hands_offset_type_gl_scope] = Fvector{pSettings->r_float(sect_name, val_name), pSettings->r_float(sect_name, val_name2)};
    //
    xr_strconcat(val_name, "grenade_normal_zoom_offset", _prefix);
    if (is_16x9 && !pSettings->line_exist(sect_name, val_name))
        xr_strcpy(val_name, "grenade_normal_zoom_offset");
    if (pSettings->line_exist(sect_name, val_name))
        m_hands_offset[m_hands_offset_pos][m_hands_offset_type_aim_gl_normal] = pSettings->r_fvector3(sect_name, val_name);
    else
        m_hands_offset[m_hands_offset_pos][m_hands_offset_type_aim_gl_normal] = m_hands_offset[m_hands_offset_pos][m_hands_offset_type_aim];

    xr_strconcat(val_name, "grenade_normal_zoom_rotate_x", _prefix);
    xr_strconcat(val_name2, "grenade_normal_zoom_rotate_y", _prefix);
    if (is_16x9 && (!pSettings->line_exist(sect_name, val_name) || !pSettings->line_exist(sect_name, val_name2)))
    {
        xr_strcpy(val_name, "grenade_normal_zoom_rotate_x");
        xr_strcpy(val_name2, "grenade_normal_zoom_rotate_y");
    }
    if (pSettings->line_exist(sect_name, val_name) && pSettings->line_exist(sect_name, val_name2))
        m_hands_offset[m_hands_offset_rot][m_hands_offset_type_aim_gl_normal] = Fvector{pSettings->r_float(sect_name, val_name), pSettings->r_float(sect_name, val_name2)};
    else
        m_hands_offset[m_hands_offset_rot][m_hands_offset_type_aim_gl_normal] = m_hands_offset[m_hands_offset_rot][m_hands_offset_type_aim];
    //
    xr_strconcat(val_name, "scope_grenade_normal_zoom_offset", _prefix);
    if (is_16x9 && !pSettings->line_exist(sect_name, val_name))
        xr_strcpy(val_name, "scope_grenade_normal_zoom_offset");
    if (pSettings->line_exist(sect_name, val_name))
        m_hands_offset[m_hands_offset_pos][m_hands_offset_type_gl_normal_scope] = pSettings->r_fvector3(sect_name, val_name);

    xr_strconcat(val_name, "scope_grenade_normal_zoom_rotate_x", _prefix);
    xr_strconcat(val_name2, "scope_grenade_normal_zoom_rotate_y", _prefix);
    if (is_16x9 && (!pSettings->line_exist(sect_name, val_name) || !pSettings->line_exist(sect_name, val_name2)))
    {
        xr_strcpy(val_name, "scope_grenade_normal_zoom_rotate_x");
        xr_strcpy(val_name2, "scope_grenade_normal_zoom_rotate_y");
    }
    if (pSettings->line_exist(sect_name, val_name) && pSettings->line_exist(sect_name, val_name2))
        m_hands_offset[m_hands_offset_rot][m_hands_offset_type_gl_normal_scope] = Fvector{pSettings->r_float(sect_name, val_name), pSettings->r_float(sect_name, val_name2)};
    //

    if (useCopFirePoint) // cop configs
    {
        R_ASSERT2(pSettings->line_exist(sect_name, "fire_point") == pSettings->line_exist(sect_name, "fire_bone"), sect_name.c_str());
        R_ASSERT2(pSettings->line_exist(sect_name, "fire_point2") == pSettings->line_exist(sect_name, "fire_bone2"), sect_name.c_str());
        R_ASSERT2(pSettings->line_exist(sect_name, "shell_point") == pSettings->line_exist(sect_name, "shell_bone"), sect_name.c_str());
    }

    m_prop_flags.set(e_16x9_mode_now, is_16x9);
}

attachable_hud_item::~attachable_hud_item()
{
    IRenderVisual* v = m_model->dcast_RenderVisual();
    ::Render->model_Delete(v);
    m_model = nullptr;
}

void attachable_hud_item::load(const shared_str& sect_name)
{
    m_sect_name = sect_name;

    // Visual
    if (pSettings->line_exist(sect_name, "visual"))
    {
        m_visual_name = pSettings->r_string(sect_name, "visual");
        m_has_separated_hands = false;
    }
    else
    {
        m_visual_name = pSettings->r_string(sect_name, "item_visual");
        m_has_separated_hands = true;
    }

    ::Render->hud_loading = true;
    m_model = smart_cast<IKinematics*>(::Render->model_Create(m_visual_name.c_str()));
    m_model->dcast_RenderVisual()->MarkAsHot(false);
    ::Render->hud_loading = false;

    m_attach_place_idx = READ_IF_EXISTS(pSettings, r_u16, sect_name, "attach_place_idx", 0);
    m_measures.load(sect_name, m_model);

    IKinematicsAnimated* animatedHudItem = smart_cast<IKinematicsAnimated*>(m_model);
    m_hand_motions.load(m_has_separated_hands, m_parent->Model(), animatedHudItem, sect_name);
}

u32 attachable_hud_item::anim_play(const shared_str& anm_name_b, BOOL bMixIn, const CMotionDef*& md, bool randomAnim, float speed)
{
    R_ASSERT(strstr(anm_name_b.c_str(), "anm_") == anm_name_b.c_str() || strstr(anm_name_b.c_str(), "anim_") == anm_name_b.c_str());
    string256 anim_name_r;
    bool is_16x9 = UI()->is_widescreen();
    xr_sprintf(anim_name_r, "%s%s", anm_name_b.c_str(), ((m_attach_place_idx == 1) && is_16x9) ? "_16x9" : "");

    player_hud_motion* anm = m_hand_motions.find_motion(anim_name_r);
    ASSERT_FMT(anm, "model [%s] has no motion alias defined [%s]", m_visual_name.c_str(), anim_name_r);
    ASSERT_FMT(anm->m_animations.size(), "model [%s] has no motion defined in motion_alias [%s]", m_visual_name.c_str(), anim_name_r);

    u8 rnd_idx = 0;

    if (randomAnim)
        rnd_idx = (u8)Random.randI(anm->m_animations.size());

    const motion_descr& M = anm->m_animations[rnd_idx];

    if (speed == 1.f)
        speed = anm->params.speed_k;

    IKinematicsAnimated* ka = m_model->dcast_PKinematicsAnimated();
    u32 ret = g_player_hud->anim_play(m_attach_place_idx, anm->params, M, bMixIn, md, speed, m_has_separated_hands, ka);

    if (ka)
    {
        MotionID M2;

        if (anm->m_base_name != anm->m_additional_name)
        {
            u8 rnd_idx2 = 0;

            if (randomAnim)
                rnd_idx2 = (u8)Random.randI(anm->m_additional_animations.size());

            motion_descr& additional = anm->m_additional_animations[rnd_idx2];

            if (bDebug)
                Msg("playing item animation [%s]", additional.name.c_str());

            M2 = ka->ID_Cycle_Safe(additional.name);
        }
        else
        {
            shared_str item_anm_name = M.name;

            if (bDebug)
                Msg("playing item animation [%s]", item_anm_name.c_str());

            M2 = ka->ID_Cycle_Safe(item_anm_name);
        }
        
        if (!M2.valid())
            M2 = ka->ID_Cycle_Safe("idle");
        
        R_ASSERT3(M2.valid(), "model has no motion [idle] ", m_visual_name.c_str());

        if (m_has_separated_hands)
        {
            u16 root_id = m_model->LL_GetBoneRoot();
            CBoneInstance& root_binst = m_model->LL_GetBoneInstance(root_id);
            root_binst.set_callback_overwrite(TRUE);
            root_binst.mTransform.identity();
        }

        u16 pc = ka->partitions().count();
        for (u16 pid = 0; pid < pc; ++pid)
        {
            CBlend* B = ka->PlayCycle(pid, M2, bMixIn);
            R_ASSERT(B);
            B->speed *= speed;
            B->timeCurrent = CalculateMotionStartSeconds(anm->params.start_k, B->timeTotal);
        }

        m_model->CalculateBones_Invalidate();
    }

    R_ASSERT2(m_parent_hud_item, "parent hud item is NULL");
    CPhysicItem& parent_object = m_parent_hud_item->object();

    if (parent_object.H_Parent() == Level().CurrentControlEntity())
    {
        CActor* current_actor = smart_cast<CActor*>(Level().CurrentControlEntity());
        VERIFY(current_actor);

        string_path ce_path, anm_name;
        xr_strconcat(anm_name, "camera_effects\\weapon\\", M.eff_name ? M.eff_name : M.name.c_str(), ".anm");
        if (FS.exist(ce_path, "$game_anims$", anm_name))
        {
            current_actor->Cameras().RemoveCamEffector(eCEWeaponAction);

            CAnimatorCamEffector* e = xr_new<CAnimatorCamEffector>();
            e->SetType(eCEWeaponAction);
            e->SetHudAffect(false);
            e->SetCyclic(false);
            e->Start(anm_name);
            current_actor->Cameras().AddCamEffector(e);
        }
    }
    return ret;
}

player_hud::player_hud()
{
    m_transform.identity();
    m_transform_2.identity();

    script_anim_part = u8(-1);
    script_anim_offset_factor = 0.f;
    m_item_pos.identity();
    script_override_arms = false;
    script_override_item = false;

    if (pSettings->section_exist("hud_movement_layers"))
    {
        m_movement_layers.reserve(move_anms_end);

        for (int i = 0; i < move_anms_end; i++)
        {
            movement_layer* anm = xr_new<movement_layer>();

            char temp[20];
            string512 tmp;
            strconcat(sizeof(temp), temp, "movement_layer_", std::to_string(i).c_str());
            R_ASSERT2(pSettings->line_exist("hud_movement_layers", temp), make_string("Missing definition for [hud_movement_layers] %s", temp));
            LPCSTR layer_def = pSettings->r_string("hud_movement_layers", temp);
            R_ASSERT2(_GetItemCount(layer_def) > 0, make_string("Wrong definition for [hud_movement_layers] %s", temp));

            _GetItem(layer_def, 0, tmp);
            anm->Load(tmp);
            _GetItem(layer_def, 1, tmp);
            anm->anm->Speed() = (atof(tmp) ? atof(tmp) : 1.f);
            _GetItem(layer_def, 2, tmp);
            anm->m_power = (atof(tmp) ? atof(tmp) : 1.f);
            m_movement_layers.push_back(anm);
        }
    }
}

player_hud::~player_hud()
{
    if (m_model)
    {
        IRenderVisual* v = m_model->dcast_RenderVisual();
        ::Render->model_Delete(v);
        m_model = nullptr;
    }

    auto it = m_pool.begin();
    auto it_e = m_pool.end();
    for (; it != it_e; ++it)
    {
        attachable_hud_item* a = *it;
        xr_delete(a);
    }
    m_pool.clear();

    delete_data(m_script_layers);
    delete_data(m_movement_layers);
}

void player_hud::load(const shared_str& player_hud_sect, bool force)
{
    if (!force && player_hud_sect == m_sect_name)
        return;

    if (script_override_arms) return;

    const bool b_reload = m_model != nullptr || m_model_2 != nullptr;
    if (m_model)
    {
        IRenderVisual* v = m_model->dcast_RenderVisual();
        ::Render->model_Delete(v);
        m_model = nullptr;
    }
    if (m_model_2)
    {
        IRenderVisual* v = m_model_2->dcast_RenderVisual();
        ::Render->model_Delete(v);
        m_model_2 = nullptr;
    }

    if (!pSettings->line_exist(player_hud_sect, "visual"))
        return;

    m_sect_name = player_hud_sect;
    const char* model_name = pSettings->r_string(player_hud_sect, "visual");

    ::Render->hud_loading = true;
    m_model = smart_cast<IKinematicsAnimated*>(::Render->model_Create(model_name));
    const char* model_name_2 = READ_IF_EXISTS(pSettings, r_string, player_hud_sect, "visual_2", model_name);
    m_model_2 = smart_cast<IKinematicsAnimated*>(::Render->model_Create(model_name_2));
    ::Render->hud_loading = false;

    u16 l_arm = m_model->dcast_PKinematics()->LL_BoneID("l_clavicle");
    ASSERT_FMT(l_arm != BI_NONE, "[%s]: bone [%s] not found in sect [%s] visual [%s]", __FUNCTION__, "l_clavicle", m_sect_name.c_str(), model_name);
    u16 r_arm = m_model_2->dcast_PKinematics()->LL_BoneID("r_clavicle");
    ASSERT_FMT(r_arm != BI_NONE, "[%s]: bone [%s] not found in sect [%s] visual [%s]", __FUNCTION__, "r_clavicle", m_sect_name.c_str(), model_name_2);

    u16 r_finger0 = m_model->dcast_PKinematics()->LL_BoneID("r_finger0");
    ASSERT_FMT(r_finger0 != BI_NONE, "[%s]: bone [%s] not found in sect [%s] visual [%s]", __FUNCTION__, "r_finger0", m_sect_name.c_str(), model_name);
    u16 r_finger01 = m_model->dcast_PKinematics()->LL_BoneID("r_finger01");
    ASSERT_FMT(r_finger01 != BI_NONE, "[%s]: bone [%s] not found in sect [%s] visual [%s]", __FUNCTION__, "r_finger01", m_sect_name.c_str(), model_name);
    u16 r_finger02 = m_model->dcast_PKinematics()->LL_BoneID("r_finger02");
    ASSERT_FMT(r_finger02 != BI_NONE, "[%s]: bone [%s] not found in sect [%s] visual [%s]", __FUNCTION__, "r_finger02", m_sect_name.c_str(), model_name);

    m_model->dcast_PKinematics()->LL_GetBoneInstance(r_finger0).set_callback(bctCustom, Thumb0Callback, this);
    m_model->dcast_PKinematics()->LL_GetBoneInstance(r_finger01).set_callback(bctCustom, Thumb01Callback, this);
    m_model->dcast_PKinematics()->LL_GetBoneInstance(r_finger02).set_callback(bctCustom, Thumb02Callback, this);

    // hides the unused arm meshes
    m_model->dcast_PKinematics()->LL_SetBoneVisible(l_arm, FALSE, TRUE);
    m_model_2->dcast_PKinematics()->LL_SetBoneVisible(r_arm, FALSE, TRUE);

    m_ancors.clear();
    for (const auto& [key, _bone] : pSettings->r_section(player_hud_sect).Data)
    {
        if (strstr(key.c_str(), "ancor_") == key.c_str())
        {
            m_ancors.push_back(m_model->dcast_PKinematics()->LL_BoneID(_bone));
        }
    }

    //	Msg("hands visual changed to[%s] [%s] [%s]", model_name.c_str(), b_reload?"R":"", m_attached_items[0]?"Y":"");

    if (!b_reload)
    {
        m_model->PlayCycle("hand_idle_doun");
        m_model_2->PlayCycle("hand_idle_doun");
    }
    else
    {
        if (m_attached_items[1])
            m_attached_items[1]->m_parent_hud_item->on_a_hud_attach();

        if (m_attached_items[0])
            m_attached_items[0]->m_parent_hud_item->on_a_hud_attach();
    }
    m_model->dcast_PKinematics()->CalculateBones_Invalidate();
    m_model->dcast_PKinematics()->CalculateBones(TRUE);
    m_model_2->dcast_PKinematics()->CalculateBones_Invalidate();
    m_model_2->dcast_PKinematics()->CalculateBones(TRUE);

    m_model->dcast_RenderVisual()->MarkAsHot(true);
    m_model_2->dcast_RenderVisual()->MarkAsHot(true);
}

bool player_hud::render_item_ui_query()
{
    bool res = false;
    if (m_attached_items[0])
        res |= m_attached_items[0]->render_item_ui_query();

    if (m_attached_items[1])
        res |= m_attached_items[1]->render_item_ui_query();

    return res;
}

void player_hud::render_item_ui()
{
    IUIRender::ePointType bk = UI()->m_currentPointType;
    UI()->m_currentPointType = IUIRender::pttLIT;
    UIRender->CacheSetCullMode(IUIRender::cmNONE);

    if (m_attached_items[0])
        m_attached_items[0]->render_item_ui();

    if (m_attached_items[1])
        m_attached_items[1]->render_item_ui();

    UIRender->CacheSetCullMode(IUIRender::cmCCW);
    UI()->m_currentPointType = bk;
}

void player_hud::render_hud(u32 context_id, IRenderable* root)
{
    //if (!m_attached_items[0] && !m_attached_items[1])
    //    return;

	bool b_r0 = ((m_attached_items[0] && m_attached_items[0]->need_renderable()) || script_anim_part == 0 || script_anim_part == 2);
    bool b_r1 = ((m_attached_items[1] && m_attached_items[1]->need_renderable()) || script_anim_part == 1 || script_anim_part == 2);

    if (!b_r0 && !b_r1)
        return;

    bool b_has_hands =
        (m_attached_items[0] && m_attached_items[0]->m_has_separated_hands) || (m_attached_items[1] && m_attached_items[1]->m_has_separated_hands) || script_anim_item_model;

    if (b_has_hands || script_anim_part != u8(-1))
    {
        ::Render->add_Visual(context_id, root, m_model->dcast_RenderVisual(), m_transform);

        ::Render->add_Visual(context_id, root, m_model_2->dcast_RenderVisual(), m_transform_2);
    }

    if (!script_override_item) // можно скрывать предметы в руках во время скриптовой анимаии, но выглядит кривовато
    {
        if (m_attached_items[0])
            m_attached_items[0]->render(context_id, root);

        if (m_attached_items[1])
            m_attached_items[1]->render(context_id, root);
    }

    if (b_has_hands)
    {
        if (script_anim_item_model)
        {
            ::Render->add_Visual(context_id, root, script_anim_item_model->dcast_RenderVisual(), m_item_pos);
        }
    }
}

#include "../xr_3da/motion.h"

u32 player_hud::motion_length(const shared_str& anim_name, const shared_str& hud_name, const CMotionDef*& md, float speed)
{
    attachable_hud_item* pi = create_hud_item(hud_name);
    player_hud_motion* pm = pi->find_motion(anim_name);
    if (!pm)
        return 100; // ms TEMPORARY
    ASSERT_FMT(pm, "hudItem model [%s] has no motion with alias [%s]", hud_name.c_str(), anim_name.c_str());
    return motion_length(pm->params, pm->m_animations[0], md, pi->m_has_separated_hands ? m_model : smart_cast<IKinematicsAnimated*>(pi->m_model), speed == 1.f ? pm->params.speed_k : speed);
}

u32 player_hud::motion_length(const motion_params& P, const motion_descr& M, const CMotionDef*& md, IKinematicsAnimated* itemModel, float speed)
{
    IKinematicsAnimated* model = itemModel;

    // Msg("~~[%s] model->LL_GetMotionDef [%s] [%s], hasHands = [%u]", __FUNCTION__, M.name.c_str(), model->dcast_RenderVisual()->getDebugName().c_str(), hasHands);

    md = model->LL_GetMotionDef(M.mid);
    VERIFY(md);
    if (md->flags & esmStopAtEnd)
    {
        CMotion* motion = model->LL_GetRootMotion(M.mid);

        auto fStartFromTime = CalculateMotionStartSeconds(P.start_k, motion->GetLength());

        if (speed >= 0.0f)
            return iFloor(0.5f + 1000.f * (motion->GetLength() - fStartFromTime) / (md->Speed() * speed) * P.stop_k);
        else
            return iFloor(0.5f + 1000.f * (fStartFromTime) / (md->Speed() * abs(speed)) * P.stop_k);
    }
    return 0;
}

void player_hud::update(const Fmatrix& cam_trans)
{
    //Костыли для правильной работы системы коллизии худа. Это всё плохо и надо будет как-то переделать в будущем. Здесь два апдейта худа подряд делаются для того, чтобы менеджер
    //коллизи мог получить координаты ствола в обычном режиме, из которых уже будет делаться рейтрейс. skip_updated_frame тоже к этому относится.
    static bool need_update_collision{};
    need_update_collision = !need_update_collision;
    bool need_update_collision_local = need_update_collision;
    if (need_update_collision)
        this->update(cam_trans);

    Fmatrix trans = cam_trans;
    Fmatrix trans_b = cam_trans;
    
    auto attach_pos = [this](size_t part) {
        if (m_attached_items[part])
            return m_attached_items[part]->hands_attach_pos();
        else if (m_attached_items[!part])
            return m_attached_items[!part]->hands_attach_pos();
        else
            return Fvector{};
    };
    auto attach_rot = [this](size_t part) {
        if (m_attached_items[part])
            return m_attached_items[part]->hands_attach_rot();
        else if (m_attached_items[!part])
            return m_attached_items[!part]->hands_attach_rot();
        else
            return Fvector{};
    };

    Fvector m1pos = attach_pos(0);
    Fvector m1rot = attach_rot(0);

    Fvector m2pos = attach_pos(1);
    Fvector m2rot = attach_rot(1);

    Fmatrix trans_2 = trans;

    if (m_attached_items[0])
        m_attached_items[0]->m_parent_hud_item->UpdateHudAdditional(trans, need_update_collision_local);

    if (m_attached_items[1])
        m_attached_items[1]->m_parent_hud_item->UpdateHudAdditional(trans_2, need_update_collision_local);
    else
        trans_2 = trans;

    {
        // override hand offset for single hand animation
        if (script_anim_part == 2 || (script_anim_part && !m_attached_items[0] && !m_attached_items[1]))
        {
            m1pos = script_anim_offset[0];
            m2pos = script_anim_offset[0];
            m1rot = script_anim_offset[1];
            m2rot = script_anim_offset[1];

            trans = trans_b;
            trans_2 = trans_b;
        }
        else if (script_anim_offset_factor != 0.f)
        {
            Fvector& hand_pos = script_anim_part == 0 ? m1pos : m2pos;
            Fvector& hand_rot = script_anim_part == 0 ? m1rot : m2rot;

            hand_pos.lerp(script_anim_part == 0 ? m1pos : m2pos, script_anim_offset[0], script_anim_offset_factor);
            hand_rot.lerp(script_anim_part == 0 ? m1rot : m2rot, script_anim_offset[1], script_anim_offset_factor);

            if (script_anim_part == 0)
            {
                trans_b.inertion(trans, script_anim_offset_factor);
                trans = trans_b;
            }
            else
            {
                trans_b.inertion(trans_2, script_anim_offset_factor);
                trans_2 = trans_b;
            }
        }
    }

    // override hand offset for single hand animation
    m1rot.mul(PI / 180.f);
    m_attach_offset.setHPB(m1rot.x, m1rot.y, m1rot.z);
    m_attach_offset.translate_over(m1pos);

    m2rot.mul(PI / 180.f);
    m_attach_offset_2.setHPB(m2rot.x, m2rot.y, m2rot.z);
    m_attach_offset_2.translate_over(m2pos);

    if (need_update_collision_local)
    {
        if (m_attached_items[0] && m_attached_items[0]->m_parent_hud_item->HudBobbingAllowed())
        {
            // m_bobbing привазан к айтему только что б получить zoom factor. Зумится может только основной предмет в руках потому можно считать только по нему
            m_attached_items[0]->m_parent_hud_item->m_bobbing->Update(m_attach_offset, m_attach_offset_2);
        }
    }

    m_transform.mul(trans, m_attach_offset);
    m_transform_2.mul(trans_2, m_attach_offset_2);

    bool hasHands = (m_attached_items[0] && m_attached_items[0]->m_has_separated_hands) || (m_attached_items[1] && m_attached_items[1]->m_has_separated_hands);
    if (hasHands || script_anim_item_model)
    {
        m_model->UpdateTracks();
        m_model->dcast_PKinematics()->CalculateBones_Invalidate();
        m_model->dcast_PKinematics()->CalculateBones(TRUE);

        m_model_2->UpdateTracks();
        m_model_2->dcast_PKinematics()->CalculateBones_Invalidate();
        m_model_2->dcast_PKinematics()->CalculateBones(TRUE);       
    }

    for (script_layer* anm : m_script_layers)
    {
        if (!anm || !anm->anm || (!anm->active && anm->blend_amount == 0.f))
            continue;

        if (need_update_collision_local)
        {
            if (anm->active)
                anm->blend_amount += Device.fTimeDelta / .4f;
            else
                anm->blend_amount -= Device.fTimeDelta / .4f;

            clamp(anm->blend_amount, 0.f, 1.f);

            if (anm->blend_amount > 0.f)
            {
                if (anm->anm->bLoop || anm->anm->m_MParam.t < anm->anm->m_MParam.max_t)
                    anm->anm->Update(Device.fTimeDelta);
                else
                    anm->Stop(false);
            }
            else
            {
                anm->Stop(true);
                continue;
            }
        }

        Fmatrix blend = anm->XFORM();

        if (anm->m_part == 0 || anm->m_part == 2)
            m_transform.mulB_43(blend);

        if (anm->m_part == 1 || anm->m_part == 2)
            m_transform_2.mulB_43(blend);
    }

    bool need_blend[2];
    need_blend[0] = ((script_anim_part == 0 || script_anim_part == 2) || (m_attached_items[0] && m_attached_items[0]->m_parent_hud_item->NeedBlendAnm()));
    need_blend[1] = ((script_anim_part == 1 || script_anim_part == 2) || (m_attached_items[1] && m_attached_items[1]->m_parent_hud_item->NeedBlendAnm()));

    for (movement_layer* anm : m_movement_layers)
    {
        if (!anm || !anm->anm || (!anm->active && anm->blend_amount[0] == 0.f && anm->blend_amount[1] == 0.f))
            continue;

        if (need_update_collision_local)
        {
            if (anm->active && (need_blend[0] || need_blend[1]))
            {
                if (need_blend[0])
                {
                    anm->blend_amount[0] += Device.fTimeDelta / .4f;

                    if (!m_attached_items[1])
                        anm->blend_amount[1] += Device.fTimeDelta / .4f;
                    else if (!need_blend[1])
                        anm->blend_amount[1] -= Device.fTimeDelta / .4f;
                }

                if (need_blend[1])
                {
                    anm->blend_amount[1] += Device.fTimeDelta / .4f;

                    if (!m_attached_items[0])
                        anm->blend_amount[0] += Device.fTimeDelta / .4f;
                    else if (!need_blend[0])
                        anm->blend_amount[0] -= Device.fTimeDelta / .4f;
                }
            }
            else
            {
                anm->blend_amount[0] -= Device.fTimeDelta / .4f;
                anm->blend_amount[1] -= Device.fTimeDelta / .4f;
            }

            clamp(anm->blend_amount[0], 0.f, 1.f);
            clamp(anm->blend_amount[1], 0.f, 1.f);

            if (anm->blend_amount[0] == 0.f && anm->blend_amount[1] == 0.f)
            {
                anm->Stop(true);
                continue;
            }

            anm->anm->Update(Device.fTimeDelta);
        }

        if (anm->blend_amount[0] == anm->blend_amount[1])
        {
            Fmatrix blend = anm->XFORM(0);
            m_transform.mulB_43(blend);
            m_transform_2.mulB_43(blend);
        }
        else
        {
            if (anm->blend_amount[0] > 0.f)
                m_transform.mulB_43(anm->XFORM(0));

            if (anm->blend_amount[1] > 0.f)
                m_transform_2.mulB_43(anm->XFORM(1));
        }
    }


    if (m_attached_items[0])
        m_attached_items[0]->update(true);

    if (m_attached_items[1])
        m_attached_items[1]->update(true);

    if (script_anim_item_attached && script_anim_item_model)
        update_script_item();

    {
        // single hand offset smoothing + syncing back to other hand animation on end
        if (script_anim_part != u8(-1))
        {
            if (need_update_collision_local)
                script_anim_offset_factor += Device.fTimeDelta * 2.5f;

            if (m_bStopAtEndAnimIsRunning && Device.dwTimeGlobal >= script_anim_end)
                script_anim_stop();
        }
        else if (need_update_collision_local)
            script_anim_offset_factor -= Device.fTimeDelta * 5.f;

        clamp(script_anim_offset_factor, 0.f, 1.f);
    }
}

u32 player_hud::anim_play(u16 part, const motion_params& P, const motion_descr& M, BOOL bMixIn, const CMotionDef*& md, float speed, bool hasHands, IKinematicsAnimated* itemModel)
{
    // Msg("~~[%s] model->LL_GetMotionDef [%s] [%s] attached_item(0): [%p], hasHands = [%u]", __FUNCTION__, M.name.c_str(), itemModel ?
    // itemModel->dcast_RenderVisual()->getDebugName().c_str() : "", attached_item(0), hasHands); Msg("~~[%s] model->LL_GetMotionDef [%s] [%s], hasHands = [%u]", __FUNCTION__,
    // M.name.c_str(), itemModel ? itemModel->dcast_RenderVisual()->getDebugName().c_str() : "", hasHands);

    if (hasHands)
    {
        u16 part_id = u16(-1);
        if (attached_item(0) && attached_item(1))
            part_id = m_model->partitions().part_id((part == 0) ? "right_hand" : "left_hand");

        if (script_anim_part != u8(-1))
        {
            if (script_anim_part != 2)
                part_id = script_anim_part == 0 ? 1 : 0;
            else
                return 0;
        }

        if (part_id == u16(-1))
        {
            for (u8 pid = 0; pid < 3; pid++)
            {
                if (pid == 0 || pid == 2)
                {
                    CBlend* B = m_model->PlayCycle(pid, M.mid, bMixIn);
                    R_ASSERT(B);
                    B->speed *= speed;
                    B->timeCurrent = CalculateMotionStartSeconds(P.start_k, B->timeTotal);
                }
                if (pid == 0 || pid == 1)
                {
                    CBlend* B = m_model_2->PlayCycle(pid, M.mid, bMixIn);
                    R_ASSERT(B);
                    B->speed *= speed;
                    B->timeCurrent = CalculateMotionStartSeconds(P.start_k, B->timeTotal);
                }
            }

            m_model->dcast_PKinematics()->CalculateBones_Invalidate();
            m_model_2->dcast_PKinematics()->CalculateBones_Invalidate();
        }
        else if (part_id == 0 || part_id == 2)
        {
            for (u8 pid = 0; pid < 3; pid++)
            {
                if (pid != 1)
                {
                    CBlend* B = m_model->PlayCycle(pid, M.mid, bMixIn);
                    R_ASSERT(B);
                    B->speed *= speed;
                    B->timeCurrent = CalculateMotionStartSeconds(P.start_k, B->timeTotal);
                }
            }

            m_model->dcast_PKinematics()->CalculateBones_Invalidate();
        }
        else if (part_id == 1)
        {
            for (u8 pid = 0; pid < 3; pid++)
            {
                if (pid != 2)
                {
                    CBlend* B = m_model_2->PlayCycle(pid, M.mid, bMixIn);
                    R_ASSERT(B);
                    B->speed *= speed;
                    B->timeCurrent = CalculateMotionStartSeconds(P.start_k, B->timeTotal);
                }
            }

            m_model_2->dcast_PKinematics()->CalculateBones_Invalidate();
        }
    }

    return motion_length(P, M, md, hasHands ? m_model : itemModel, speed);
}

attachable_hud_item* player_hud::create_hud_item(const shared_str& sect)
{
    auto it = m_pool.begin();
    auto it_e = m_pool.end();
    for (; it != it_e; ++it)
    {
        attachable_hud_item* itm = *it;
        if (itm->m_sect_name == sect)
            return itm;
    }
    attachable_hud_item* res = xr_new<attachable_hud_item>(this);
    res->load(sect);
    m_pool.push_back(res);

    return res;
}

bool player_hud::allow_activation(CHudItem* item)
{
    if (m_attached_items[1])
        return m_attached_items[1]->m_parent_hud_item->CheckCompatibility(item);
    else
        return true;
}

void player_hud::attach_item(CHudItem* item)
{
    attachable_hud_item* pi = create_hud_item(item->HudSection());
    int item_idx = pi->m_attach_place_idx;

    if (m_attached_items[item_idx] != pi || pi->m_parent_hud_item != item)
    {
        if (m_attached_items[item_idx])
            m_attached_items[item_idx]->m_parent_hud_item->on_b_hud_detach();

        m_attached_items[item_idx] = pi;
        pi->m_parent_hud_item = item;

        if (item_idx == 0 && m_attached_items[1])
            m_attached_items[1]->m_parent_hud_item->CheckCompatibility(item);

        item->on_a_hud_attach();

        updateMovementLayerState();
    }
    pi->m_parent_hud_item = item;
}

void player_hud::detach_item_idx(u16 idx)
{
    if (!attached_item(idx))
        return;

    const bool hasHands = attached_item(idx)->m_has_separated_hands;

    m_attached_items[idx]->m_parent_hud_item->on_b_hud_detach();

    m_attached_items[idx]->m_parent_hud_item = nullptr;
    m_attached_items[idx] = nullptr;

    if (hasHands)
    {
        if (idx == 1)
        {
            if (m_attached_items[0])
                re_sync_anim(2);
            else
            {
                m_model_2->PlayCycle("hand_idle_doun");
            }
        }
        else if (idx == 0)
        {
            if (m_attached_items[1])
            {
                // fix for a rare case where the right hand stays visible on screen after detaching the right hand's attached item
                player_hud_motion* pm = m_attached_items[1]->find_motion("anm_idle");
                if (pm)
                {
                    const motion_descr& M = pm->m_animations[0];
                    m_model->PlayCycle(0, M.mid, false);
                    m_model->PlayCycle(2, M.mid, false);
                }
            }
            else
            {
                m_model->PlayCycle("hand_idle_doun");
            }
        }

        if (!m_attached_items[0] && !m_attached_items[1])
        {
            m_model->PlayCycle("hand_idle_doun");
            m_model_2->PlayCycle("hand_idle_doun");
        }
    }

    // KRodin: закомментировал этот кусок, не понятно для чего он может быть нужен.
    /*else if (idx == 0 && attached_item(1))
    {
        OnMovementChanged(mcAnyMove);
    }*/
}

void player_hud::detach_item(CHudItem* item)
{
    if (nullptr == item->HudItemData())
        return;

    u16 item_idx = item->HudItemData()->m_attach_place_idx;
    if (m_attached_items[item_idx] == item->HudItemData())
        detach_item_idx(item_idx);
}

void player_hud::calc_transform(u16 attach_slot_idx, const Fmatrix& offset, Fmatrix& result)
{
    bool hasHands = m_attached_items[attach_slot_idx] && m_attached_items[attach_slot_idx]->m_has_separated_hands;

    if (hasHands || script_anim_item_model)
    {
        IKinematics* kin = (attach_slot_idx == 0) ? m_model->dcast_PKinematics() : m_model_2->dcast_PKinematics();
        Fmatrix ancor_m = kin->LL_GetTransform(m_ancors.at(attach_slot_idx));
        result.mul((attach_slot_idx == 0) ? m_transform : m_transform_2, ancor_m);
        result.mulB_43(offset);
    }
    else
    {
        result.set(m_transform);
        result.mulB_43(offset);
    }
}

void player_hud::OnMovementChanged(ACTOR_DEFS::EMoveCommand cmd)
{
    if (cmd == 0)
    {
        if (m_attached_items[0])
        {
            if (m_attached_items[0]->m_parent_hud_item->GetState() == CHUDState::eIdle)
                m_attached_items[0]->m_parent_hud_item->PlayAnimIdle();
        }
        if (m_attached_items[1])
        {
            if (m_attached_items[1]->m_parent_hud_item->GetState() == CHUDState::eIdle)
                m_attached_items[1]->m_parent_hud_item->PlayAnimIdle();
        }
    }
    else
    {
        if (m_attached_items[0])
            m_attached_items[0]->m_parent_hud_item->OnMovementChanged(cmd);

        if (m_attached_items[1])
            m_attached_items[1]->m_parent_hud_item->OnMovementChanged(cmd);
    }

    updateMovementLayerState();
}

// sync anim of other part to selected part (1 = sync to left hand anim; 2 = sync to right hand anim)
void player_hud::re_sync_anim(u8 part)
{
    u32 bc = part == 1 ? m_model_2->LL_PartBlendsCount(part) : m_model->LL_PartBlendsCount(part);
    for (u32 bidx = 0; bidx < bc; ++bidx)
    {
        CBlend* BR = part == 1 ? m_model_2->LL_PartBlend(part, bidx) : m_model->LL_PartBlend(part, bidx);
        if (!BR)
            continue;

        MotionID M = BR->motionID;

        u16 pc = m_model->partitions().count(); // same on both armatures
        for (u16 pid = 0; pid < pc; ++pid)
        {
            if (pid == 0)
            {
                CBlend* B = m_model->PlayCycle(0, M, TRUE);
                B->timeCurrent = BR->timeCurrent;
                B->speed = BR->speed;
                B = m_model_2->PlayCycle(0, M, TRUE);
                B->timeCurrent = BR->timeCurrent;
                B->speed = BR->speed;
            }
            else if (pid != part)
            {
                CBlend* B = part == 1 ? m_model->PlayCycle(pid, M, TRUE) : m_model_2->PlayCycle(pid, M, TRUE);
                B->timeCurrent = BR->timeCurrent;
                B->speed = BR->speed;
            }
        }
    }
}

void player_hud::GetLHandBoneOffsetPosDir(const shared_str& bone_name, Fvector& dest_pos, Fvector& dest_dir, const Fvector& offset)
{
    const u16 bone_id = m_model_2->dcast_PKinematics()->LL_BoneID(bone_name);
    ASSERT_FMT(bone_id != BI_NONE, "!![%s] bone [%s] not found in weapon [%s]", __FUNCTION__, bone_name.c_str(), m_sect_name.c_str());
    Fmatrix& fire_mat = m_model_2->dcast_PKinematics()->LL_GetTransform(bone_id);
    fire_mat.transform_tiny(dest_pos, offset);
    m_transform_2.transform_tiny(dest_pos);
    dest_pos.add(Device.vCameraPosition);
    dest_dir.set(0.f, 0.f, 1.f);
    m_transform_2.transform_dir(dest_dir);
}

void player_hud::Thumb0Callback(CBoneInstance* B)
{
    player_hud* P = static_cast<player_hud*>(B->callback_param());

    Fvector& target = P->target_thumb0rot;
    Fvector& current = P->thumb0rot;

    if (!target.similar(current))
    {
        Fvector diff[2];
        diff[0] = target;
        diff[0].sub(current);
        diff[0].mul(Device.fTimeDelta / .1f);
        current.add(diff[0]);
    }
    else
        current.set(target);

    Fmatrix rotation;
    rotation.identity();
    rotation.rotateX(current.x);

    Fmatrix rotation_y;
    rotation_y.identity();
    rotation_y.rotateY(current.y);
    rotation.mulA_43(rotation_y);

    rotation_y.identity();
    rotation_y.rotateZ(current.z);
    rotation.mulA_43(rotation_y);

    B->mTransform.mulB_43(rotation);
}

void player_hud::Thumb01Callback(CBoneInstance* B)
{
    player_hud* P = static_cast<player_hud*>(B->callback_param());

    Fvector& target = P->target_thumb01rot;
    Fvector& current = P->thumb01rot;

    if (!target.similar(current))
    {
        Fvector diff[2];
        diff[0] = target;
        diff[0].sub(current);
        diff[0].mul(Device.fTimeDelta / .1f);
        current.add(diff[0]);
    }
    else
        current.set(target);

    Fmatrix rotation;
    rotation.identity();
    rotation.rotateX(current.x);

    Fmatrix rotation_y;
    rotation_y.identity();
    rotation_y.rotateY(current.y);
    rotation.mulA_43(rotation_y);

    rotation_y.identity();
    rotation_y.rotateZ(current.z);
    rotation.mulA_43(rotation_y);

    B->mTransform.mulB_43(rotation);
}

void player_hud::Thumb02Callback(CBoneInstance* B)
{
    player_hud* P = static_cast<player_hud*>(B->callback_param());

    Fvector& target = P->target_thumb02rot;
    Fvector& current = P->thumb02rot;

    if (!target.similar(current))
    {
        Fvector diff[2];
        diff[0] = target;
        diff[0].sub(current);
        diff[0].mul(Device.fTimeDelta / .1f);
        current.add(diff[0]);
    }
    else
        current.set(target);

    Fmatrix rotation;
    rotation.identity();
    rotation.rotateX(current.x);

    Fmatrix rotation_y;
    rotation_y.identity();
    rotation_y.rotateY(current.y);
    rotation.mulA_43(rotation_y);

    rotation_y.identity();
    rotation_y.rotateZ(current.z);
    rotation.mulA_43(rotation_y);

    B->mTransform.mulB_43(rotation);
}

bool player_hud::allow_script_anim()
{
    if (m_attached_items[0] && (m_attached_items[0]->m_parent_hud_item->IsPending() || m_attached_items[0]->m_parent_hud_item->GetState() == CHudItem::EHudStates::eBore))
        return false;
    else if (m_attached_items[1] && (m_attached_items[1]->m_parent_hud_item->IsPending() || m_attached_items[1]->m_parent_hud_item->GetState() == CHudItem::EHudStates::eBore))
        return false;
    else if (script_anim_part != u8(-1))
        return false;

    return true;
}

void player_hud::load_script(LPCSTR section)
{
    script_override_arms = false;
    load(section, true);
    script_override_arms = true;
}

u32 player_hud::script_anim_play(u8 hand, LPCSTR hud_section, LPCSTR anm_name, bool bMixIn, float speed, bool bOverride_item)
{
    if (!pSettings->section_exist(hud_section))
    {
        Msg("! script motion section [%s] does not exist", hud_section);
        m_bStopAtEndAnimIsRunning = true;
        script_anim_end = Device.dwTimeGlobal;

        return 0;
    }

    xr_string pos = "hands_position";
    xr_string rot = "hands_orientation";

    if (UI()->is_widescreen())
    {
        pos.append("_16x9");
        rot.append("_16x9");
    }

    Fvector def = {0.f, 0.f, 0.f};
    Fvector offs = READ_IF_EXISTS(pSettings, r_fvector3, hud_section, pos.c_str(), def);
    Fvector rrot = READ_IF_EXISTS(pSettings, r_fvector3, hud_section, rot.c_str(), def);

    if (pSettings->line_exist(hud_section, "item_visual"))
    {
        ::Render->hud_loading = true;
        script_anim_item_model = ::Render->model_Create(pSettings->r_string(hud_section, "item_visual"))->dcast_PKinematicsAnimated();
        ::Render->hud_loading = false;

        item_pos[0] = READ_IF_EXISTS(pSettings, r_fvector3, hud_section, "item_position", def);
        item_pos[1] = READ_IF_EXISTS(pSettings, r_fvector3, hud_section, "item_orientation", def);
        script_anim_item_attached = READ_IF_EXISTS(pSettings, r_bool, hud_section, "item_attached", true);
        m_attach_idx = READ_IF_EXISTS(pSettings, r_u8, hud_section, "attach_place_idx", 0);

        if (!script_anim_item_attached)
        {
            Fmatrix attach_offs;
            Fvector ypr = item_pos[1];
            ypr.mul(PI / 180.f);
            attach_offs.setHPB(ypr.x, ypr.y, ypr.z);
            attach_offs.translate_over(item_pos[0]);
            m_item_pos = attach_offs;
        }
    }
    else
        script_anim_item_model = nullptr;

    script_anim_offset[0] = offs;
    script_anim_offset[1] = rrot;
    script_anim_part = hand;

    player_hud_motion_container* pm = get_hand_motions(hud_section, script_anim_item_model);
    player_hud_motion* phm = pm->find_motion(anm_name);

    if (!phm)
    {
        Msg("! script motion [%s] not found in section [%s]", anm_name, hud_section);
        m_bStopAtEndAnimIsRunning = true;
        script_anim_end = Device.dwTimeGlobal;

        return 0;
    }

    const motion_descr& M = phm->m_animations[Random.randI(phm->m_animations.size())];

    if (script_anim_item_model)
    {
        MotionID M2;

        if (phm->m_base_name != phm->m_additional_name)
        {
            u8 rnd_idx2 = 0;

            if (false) // randomAnim
                rnd_idx2 = (u8)Random.randI(phm->m_additional_animations.size());

            motion_descr& additional = phm->m_additional_animations[rnd_idx2];

            if (bDebug)
                Msg("playing item animation [%s]", additional.name.c_str());

            M2 = script_anim_item_model->ID_Cycle_Safe(additional.name);
        }
        else
        {
            shared_str item_anm_name = M.name;

            if (bDebug)
                Msg("playing item animation [%s]", item_anm_name.c_str());

            M2 = script_anim_item_model->ID_Cycle_Safe(item_anm_name);
        }

        if (!M2.valid())
            M2 = script_anim_item_model->ID_Cycle_Safe("idle");

        R_ASSERT(M2.valid(), "model %s has no motion [idle] ", pSettings->r_string(hud_section, "item_visual"));

        u16 root_id = script_anim_item_model->dcast_PKinematics()->LL_GetBoneRoot();
        CBoneInstance& root_binst = script_anim_item_model->dcast_PKinematics()->LL_GetBoneInstance(root_id);
        root_binst.set_callback_overwrite(TRUE);
        root_binst.mTransform.identity();

        u16 pc = script_anim_item_model->partitions().count();
        for (u16 pid = 0; pid < pc; ++pid)
        {
            CBlend* B = script_anim_item_model->PlayCycle(pid, M2, bMixIn);
            R_ASSERT(B);
            B->speed *= speed;
            B->timeCurrent = CalculateMotionStartSeconds(phm->params.start_k, B->timeTotal);
        }

        script_anim_item_model->dcast_PKinematics()->CalculateBones_Invalidate();
    }

    if (hand == 0) // right hand
    {
        CBlend* B = m_model->PlayCycle(0, M.mid, bMixIn);
        B->speed *= speed;
        B->timeCurrent = CalculateMotionStartSeconds(phm->params.start_k, B->timeTotal);
        B = m_model->PlayCycle(2, M.mid, bMixIn);
        B->speed *= speed;
        B->timeCurrent = CalculateMotionStartSeconds(phm->params.start_k, B->timeTotal);
    }
    else if (hand == 1) // left hand
    {
        CBlend* B = m_model_2->PlayCycle(0, M.mid, bMixIn);
        B->speed *= speed;
        B->timeCurrent = CalculateMotionStartSeconds(phm->params.start_k, B->timeTotal);
        B = m_model_2->PlayCycle(1, M.mid, bMixIn);
        B->speed *= speed;
        B->timeCurrent = CalculateMotionStartSeconds(phm->params.start_k, B->timeTotal);
    }
    else if (hand == 2) // both hands
    {
        CBlend* B = m_model->PlayCycle(0, M.mid, bMixIn);
        B->speed *= speed;
        B->timeCurrent = CalculateMotionStartSeconds(phm->params.start_k, B->timeTotal);
        B = m_model_2->PlayCycle(0, M.mid, bMixIn);
        B->speed *= speed;
        B->timeCurrent = CalculateMotionStartSeconds(phm->params.start_k, B->timeTotal);
        B = m_model->PlayCycle(2, M.mid, bMixIn);
        B->speed *= speed;
        B->timeCurrent = CalculateMotionStartSeconds(phm->params.start_k, B->timeTotal);
        B = m_model_2->PlayCycle(1, M.mid, bMixIn);
        B->speed *= speed;
        B->timeCurrent = CalculateMotionStartSeconds(phm->params.start_k, B->timeTotal);
    }

    const CMotionDef* md;
    u32 length = motion_length(phm->params, M, md, m_model, speed);

    if (length > 0)
    {
        script_override_item = bOverride_item;

        m_bStopAtEndAnimIsRunning = true;
        script_anim_end = Device.dwTimeGlobal + length;
    }
    else
        m_bStopAtEndAnimIsRunning = false;

    updateMovementLayerState();

    return length;
}

void player_hud::script_anim_stop()
{
    u8 part = script_anim_part;
    script_anim_part = u8(-1);
    script_anim_item_model = nullptr;
    script_override_item = false;

    updateMovementLayerState();

    if (part != 2 && !m_attached_items[part])
        re_sync_anim(part + 1);
    else
        OnMovementChanged((ACTOR_DEFS::EMoveCommand)0);
}

u32 player_hud::motion_length_script(LPCSTR hud_section, LPCSTR anm_name, float speed)
{
    if (!pSettings->section_exist(hud_section))
    {
        Msg("! script motion section [%s] does not exist", hud_section);
        return 0;
    }

    IKinematicsAnimated* animatedHudItem = NULL;

    if (pSettings->line_exist(hud_section, "item_visual"))
    {
        ::Render->hud_loading = true;
        animatedHudItem = ::Render->model_Create(pSettings->r_string(hud_section, "item_visual"))->dcast_PKinematicsAnimated();
        ::Render->hud_loading = false;
    }

    player_hud_motion_container* pm = get_hand_motions(hud_section, animatedHudItem);
    if (!pm)
        return 0;

    player_hud_motion* phm = pm->find_motion(anm_name);
    if (!phm)
    {
        Msg("! script motion [%s] not found in section [%s]", anm_name, hud_section);
        return 0;
    }

    const CMotionDef* md;
    return motion_length(phm->params, phm->m_animations[0], md, m_model, speed);
}

void player_hud::update_script_item()
{
    Fvector ypr = item_pos[1];
    ypr.mul(PI / 180.f);
    m_attach_offset.setHPB(ypr.x, ypr.y, ypr.z);
    m_attach_offset.translate_over(item_pos[0]);

    calc_transform(m_attach_idx, m_attach_offset, m_item_pos);

    if (script_anim_item_model)
    {
        script_anim_item_model->UpdateTracks();
        script_anim_item_model->dcast_PKinematics()->CalculateBones_Invalidate();
        script_anim_item_model->dcast_PKinematics()->CalculateBones(TRUE);
    }
}

void player_hud::updateMovementLayerState()
{
    if (!m_movement_layers.size())
        return;

    CActor* pActor = Actor();

    if (!pActor)
        return;

    for (movement_layer* anm : m_movement_layers)
    {
        anm->Stop(false);
    }

    bool need_blend = (script_anim_part != u8(-1) 
            || (m_attached_items[0] && m_attached_items[0]->m_parent_hud_item->NeedBlendAnm()) 
            || (m_attached_items[1] && m_attached_items[1]->m_parent_hud_item->NeedBlendAnm()));

    if (pActor->AnyMove() && need_blend)
    {
        CEntity::SEntityState state;
        pActor->g_State(state);

        CWeapon* wep = nullptr;

        if (m_attached_items[0] && m_attached_items[0]->m_parent_hud_item->object().cast_weapon())
            wep = m_attached_items[0]->m_parent_hud_item->object().cast_weapon();

        if (wep && wep->IsZoomed())
            state.bCrouch ? m_movement_layers[eAimCrouch]->Play() : m_movement_layers[eAimWalk]->Play();
        else if (state.bCrouch)
            m_movement_layers[eCrouch]->Play();
        else if (state.bSprint)
            m_movement_layers[eSprint]->Play();
        else if (!isActorAccelerated(pActor->MovingState(), false))
            m_movement_layers[eWalk]->Play();
        else
            m_movement_layers[eRun]->Play();
    }
}


float player_hud::PlayBlendAnm(LPCSTR name, u8 part, float speed, float power, bool bLooped, bool no_restart)
{
    for (script_layer* anm : m_script_layers)
    {
        if (!xr_strcmp(*anm->m_name, name))
        {
            if (!no_restart)
            {
                anm->anm->Stop();
                anm->blend_amount = 0.f;
                anm->blend.identity();
            }

            if (!anm->anm->IsPlaying())
                anm->anm->Play(bLooped);

            anm->anm->bLoop = bLooped;
            anm->m_part = part;
            anm->anm->Speed() = speed;
            anm->m_power = power;
            anm->active = true;
            return (anm->anm->m_MParam.max_t - anm->anm->m_MParam.t) / anm->anm->Speed();
        }
    }

    script_layer* anm = xr_new<script_layer>(name, part, speed, power, bLooped);
    m_script_layers.push_back(anm);
    return (anm->anm->m_MParam.max_t - anm->anm->m_MParam.t) / anm->anm->Speed();
}

void player_hud::StopBlendAnm(LPCSTR name, bool bForce)
{
    for (script_layer* anm : m_script_layers)
    {
        if (!xr_strcmp(*anm->m_name, name))
        {
            anm->Stop(bForce);
            return;
        }
    }
}

void player_hud::StopAllBlendAnms(bool bForce)
{
    for (script_layer* anm : m_script_layers)
    {
        anm->Stop(bForce);
    }
}

float player_hud::SetBlendAnmTime(LPCSTR name, float time)
{
    for (script_layer* anm : m_script_layers)
    {
        if (!xr_strcmp(*anm->m_name, name))
        {
            if (!anm->anm->IsPlaying())
                return 0;

            float speed = (anm->anm->m_MParam.max_t - anm->anm->m_MParam.t) / time;
            anm->anm->Speed() = speed;
            return speed;
        }
    }

    return 0;
}


player_hud_motion_container* player_hud::get_hand_motions(LPCSTR section, IKinematicsAnimated* animatedHudItem)
{
    for (hand_motions* phm : m_hand_motions)
    {
        if (phm->section == section)
            return &phm->pm;
    }

    hand_motions* res = xr_new<hand_motions>();
    res->section = section;
    res->pm.load(true, m_model, animatedHudItem, section);
    m_hand_motions.push_back(res);

    return &res->pm;
}


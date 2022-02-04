#include "StdAfx.h"
#include "player_hud.h"
#include "HudItem.h"
#include "ui_base.h"
#include "Actor.h"
#include "physic_item.h"
#include "ActorEffector.h"
#include "../xr_3da/IGame_Persistent.h"
#include "Weapon.h"
#include "Actor.h"
#include "ActorCondition.h"

player_hud* g_player_hud = nullptr;

// --#SM+# Begin--
constexpr float PITCH_OFFSET_R = 0.0f;		// Насколько сильно ствол смещается вбок (влево) при вертикальных поворотах камеры
constexpr float PITCH_OFFSET_N = 0.0f;		// Насколько сильно ствол поднимается\опускается при вертикальных поворотах камеры
constexpr float PITCH_OFFSET_D = 0.02f;	// Насколько сильно ствол приближается\отдаляется при вертикальных поворотах камеры
constexpr float PITCH_LOW_LIMIT = -PI;		// Минимальное значение pitch при использовании совместно с PITCH_OFFSET_N
constexpr float ORIGIN_OFFSET = -0.05f;	// Фактор влияния инерции на положение ствола (чем меньше, тем масштабней инерция)
constexpr float ORIGIN_OFFSET_AIM = -0.03f;	// (Для прицеливания)
constexpr float TENDTO_SPEED = 5.f;		// Скорость нормализации положения ствола
constexpr float TENDTO_SPEED_AIM = 8.f;		// (Для прицеливания)
// --#SM+# End--

player_hud_motion* player_hud_motion_container::find_motion(const shared_str& name)
{
	auto it = m_anims.find(name);
	return it != m_anims.end() ? &it->second : nullptr;
}

void player_hud_motion_container::load(attachable_hud_item* parent, IKinematicsAnimated* model, IKinematicsAnimated* animatedHudItem, const shared_str& sect)
{
	string512 buff;
	MotionID motion_ID;

	for (const auto& [name, anm] : pSettings->r_section(sect).Data)
	{
		if (
			(strstr(name.c_str(), "anm_") == name.c_str() || strstr(name.c_str(), "anim_") == name.c_str())
			&& !strstr(name.c_str(), "_speed_k") && !strstr(name.c_str(), "_stop_k") && !strstr(name.c_str(), "_effector")
			)
		{
			player_hud_motion pm;

			if (parent->m_has_separated_hands)
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

			// and load all motions for it

			for (u32 i = 0; i <= 8; ++i)
			{
				if (i == 0)
					xr_strcpy(buff, pm.m_base_name.c_str());
				else
					xr_sprintf(buff, "%s%d", pm.m_base_name.c_str(), i);

				IKinematicsAnimated* final_model{};
				if (model && parent->m_has_separated_hands)
					final_model = model;
				else if (animatedHudItem && !parent->m_has_separated_hands)
					final_model = animatedHudItem;

				{
					motion_ID = final_model->ID_Cycle_Safe(buff);

					if (motion_ID.valid())
					{
						auto& Anim = pm.m_animations.emplace_back();
						Anim.mid = motion_ID;
						Anim.name = buff;

						string_path speed_param;
						xr_strconcat(speed_param, name.c_str(), "_speed_k");
						if (pSettings->line_exist(sect, speed_param)) {
							const float k = pSettings->r_float(sect, speed_param);
							if (!fsimilar(k, 1.f))
								Anim.speed_k = k;
						}

						string_path stop_param;
						xr_strconcat(stop_param, name.c_str(), "_stop_k");
						if (pSettings->line_exist(sect, stop_param)) {
							const float k = pSettings->r_float(sect, stop_param);
							if (k < 1.f)
								Anim.stop_k = k;
						}

						string_path eff_param;
						Anim.eff_name = READ_IF_EXISTS(pSettings, r_string, sect, xr_strconcat(eff_param, name.c_str(), "_effector"), nullptr);
					}
				}
			}

			if (pm.m_animations.empty())
			{
				if (parent->m_has_separated_hands) {
					FATAL("[%s] motion [%s](%s) not found in section [%s]", __FUNCTION__, pm.m_base_name.c_str(), name.c_str(), sect.c_str());
				}
				else {
					Msg("! [%s] motion [%s](%s) not found in section [%s]", __FUNCTION__, pm.m_base_name.c_str(), name.c_str(), sect.c_str());
					continue;
				}
			}

			m_anims.emplace(std::move(name), std::move(pm));
		}
	}
}

Fvector& attachable_hud_item::hands_attach_pos() { return m_measures.m_hands_attach[0]; }
Fvector& attachable_hud_item::hands_attach_rot() { return m_measures.m_hands_attach[1]; }
Fvector& attachable_hud_item::hands_offset_pos()
{
	return m_measures.m_hands_offset[hud_item_measures::m_hands_offset_pos][m_parent_hud_item->GetCurrentHudOffsetIdx()];
}

Fvector& attachable_hud_item::hands_offset_rot()
{
	return m_measures.m_hands_offset[hud_item_measures::m_hands_offset_rot][m_parent_hud_item->GetCurrentHudOffsetIdx()];
}

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

void attachable_hud_item::set_bone_visible(const std::vector<shared_str>& bone_names, BOOL bVisibility, BOOL bSilent)
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

void attachable_hud_item::update_hud_additional(Fmatrix& trans)
{
	if (m_parent_hud_item)
	{
		m_parent_hud_item->UpdateHudAdditonal(trans);
	}
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

		//KRodin придумал костыль. Из-за того, что fire_point расположен сильно впереди ствола, попробуем точку вылета пули считать от позиции fire_point.z == -0.5, т.е. ближе к актору, чтобы нельзя было стрелять сквозь стены.
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
		if (auto Wpn = smart_cast<CWeapon*>(m_parent_hud_item); Wpn && m_measures.useCopFirePoint)
			Wpn->CorrectDirFromWorldToHud(fd.vLastFD);
		VERIFY(_valid(fd.vLastFD));
		VERIFY(_valid(fd.vLastFD));

		fd.m_FireParticlesXForm.identity();
		fd.m_FireParticlesXForm.k.set(fd.vLastFD);
		Fvector::generate_orthonormal_basis_normalized(
			fd.m_FireParticlesXForm.k, fd.m_FireParticlesXForm.j, fd.m_FireParticlesXForm.i);
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

bool attachable_hud_item::need_renderable()
{
	return m_parent_hud_item->need_renderable();
}

void attachable_hud_item::render()
{
	::Render->set_Transform(&m_item_transform);
	::Render->add_Visual(m_model->dcast_RenderVisual());
	debug_draw_firedeps();
	m_parent_hud_item->render_hud_mode();
}

bool attachable_hud_item::render_item_ui_query()
{
	return m_parent_hud_item->render_item_3d_ui_query();
}

void attachable_hud_item::render_item_ui()
{
	m_parent_hud_item->render_item_3d_ui();
}

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
			m_shoot_point_offset = READ_IF_EXISTS(pSettings, r_fvector3, sect_name, "shoot_point", (Fvector{ m_fire_point_offset.x, m_fire_point_offset.y, -0.5f }));
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
		m_hands_offset[m_hands_offset_rot][m_hands_offset_type_aim] = Fvector().set(pSettings->r_float(sect_name, "zoom_rotate_x"), pSettings->r_float(sect_name, "zoom_rotate_y"), 0.f);
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
		m_hands_offset[m_hands_offset_rot][m_hands_offset_type_gl] = Fvector().set(pSettings->r_float(sect_name, "grenade_zoom_rotate_x"), pSettings->r_float(sect_name, "grenade_zoom_rotate_y"), 0.f);
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
	if (is_16x9 && (!pSettings->line_exist(sect_name, val_name) || !pSettings->line_exist(sect_name, val_name2))) {
		xr_strcpy(val_name, "scope_zoom_rotate_x");
		xr_strcpy(val_name2, "scope_zoom_rotate_y");
	}
	if (pSettings->line_exist(sect_name, val_name) && pSettings->line_exist(sect_name, val_name2))
		m_hands_offset[m_hands_offset_rot][m_hands_offset_type_aim_scope] = Fvector{ pSettings->r_float(sect_name, val_name), pSettings->r_float(sect_name, val_name2) };
	//
	xr_strconcat(val_name, "scope_grenade_zoom_offset", _prefix);
	if (is_16x9 && !pSettings->line_exist(sect_name, val_name))
		xr_strcpy(val_name, "scope_grenade_zoom_offset");
	if (pSettings->line_exist(sect_name, val_name))
		m_hands_offset[m_hands_offset_pos][m_hands_offset_type_gl_scope] = pSettings->r_fvector3(sect_name, val_name);

	xr_strconcat(val_name, "scope_grenade_zoom_rotate_x", _prefix);
	xr_strconcat(val_name2, "scope_grenade_zoom_rotate_y", _prefix);
	if (is_16x9 && (!pSettings->line_exist(sect_name, val_name) || !pSettings->line_exist(sect_name, val_name2))) {
		xr_strcpy(val_name, "scope_grenade_zoom_rotate_x");
		xr_strcpy(val_name2, "scope_grenade_zoom_rotate_y");
	}
	if (pSettings->line_exist(sect_name, val_name) && pSettings->line_exist(sect_name, val_name2))
		m_hands_offset[m_hands_offset_rot][m_hands_offset_type_gl_scope] = Fvector{ pSettings->r_float(sect_name, val_name), pSettings->r_float(sect_name, val_name2) };
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
	if (is_16x9 && (!pSettings->line_exist(sect_name, val_name) || !pSettings->line_exist(sect_name, val_name2))) {
		xr_strcpy(val_name, "grenade_normal_zoom_rotate_x");
		xr_strcpy(val_name2, "grenade_normal_zoom_rotate_y");
	}
	if (pSettings->line_exist(sect_name, val_name) && pSettings->line_exist(sect_name, val_name2))
		m_hands_offset[m_hands_offset_rot][m_hands_offset_type_aim_gl_normal] = Fvector{ pSettings->r_float(sect_name, val_name), pSettings->r_float(sect_name, val_name2) };
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
	if (is_16x9 && (!pSettings->line_exist(sect_name, val_name) || !pSettings->line_exist(sect_name, val_name2))) {
		xr_strcpy(val_name, "scope_grenade_normal_zoom_rotate_x");
		xr_strcpy(val_name2, "scope_grenade_normal_zoom_rotate_y");
	}
	if (pSettings->line_exist(sect_name, val_name) && pSettings->line_exist(sect_name, val_name2))
		m_hands_offset[m_hands_offset_rot][m_hands_offset_type_gl_normal_scope] = Fvector{ pSettings->r_float(sect_name, val_name), pSettings->r_float(sect_name, val_name2) };
	//

	if (useCopFirePoint) // cop configs
	{
		R_ASSERT2(pSettings->line_exist(sect_name, "fire_point") == pSettings->line_exist(sect_name, "fire_bone"),
			sect_name.c_str());
		R_ASSERT2(pSettings->line_exist(sect_name, "fire_point2") == pSettings->line_exist(sect_name, "fire_bone2"),
			sect_name.c_str());
		R_ASSERT2(pSettings->line_exist(sect_name, "shell_point") == pSettings->line_exist(sect_name, "shell_bone"),
			sect_name.c_str());
	}

	m_prop_flags.set(e_16x9_mode_now, is_16x9);

	//Загрузка параметров инерции --#SM+# Begin--
	m_inertion_params.m_pitch_offset_r = READ_IF_EXISTS(pSettings, r_float, sect_name, "pitch_offset_right", PITCH_OFFSET_R);
	m_inertion_params.m_pitch_offset_n = READ_IF_EXISTS(pSettings, r_float, sect_name, "pitch_offset_up", PITCH_OFFSET_N);
	m_inertion_params.m_pitch_offset_d = READ_IF_EXISTS(pSettings, r_float, sect_name, "pitch_offset_forward", PITCH_OFFSET_D);
	m_inertion_params.m_pitch_low_limit = READ_IF_EXISTS(pSettings, r_float, sect_name, "pitch_offset_up_low_limit", PITCH_LOW_LIMIT);

	m_inertion_params.m_origin_offset = READ_IF_EXISTS(pSettings, r_float, sect_name, "inertion_origin_offset", ORIGIN_OFFSET);
	m_inertion_params.m_origin_offset_aim = READ_IF_EXISTS(pSettings, r_float, sect_name, "inertion_zoom_origin_offset", ORIGIN_OFFSET_AIM);
	m_inertion_params.m_tendto_speed = READ_IF_EXISTS(pSettings, r_float, sect_name, "inertion_tendto_speed", TENDTO_SPEED);
	m_inertion_params.m_tendto_speed_aim = READ_IF_EXISTS(pSettings, r_float, sect_name, "inertion_zoom_tendto_speed", TENDTO_SPEED_AIM);
	//--#SM+# End--
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

	m_model = smart_cast<IKinematics*>(::Render->model_Create(m_visual_name.c_str()));

	m_attach_place_idx = READ_IF_EXISTS(pSettings, r_u16, sect_name, "attach_place_idx", 0);
	m_measures.load(sect_name, m_model);
}

u32 attachable_hud_item::anim_play(const shared_str& anm_name_b, BOOL bMixIn, const CMotionDef*& md, u8& rnd_idx, bool randomAnim)
{
	R_ASSERT(strstr(anm_name_b.c_str(), "anm_") == anm_name_b.c_str() || strstr(anm_name_b.c_str(), "anim_") == anm_name_b.c_str());
	string256 anim_name_r;
	bool is_16x9 = UI()->is_widescreen();
	xr_sprintf(anim_name_r, "%s%s", anm_name_b.c_str(), ((m_attach_place_idx == 1) && is_16x9) ? "_16x9" : "");

	player_hud_motion* anm = m_hand_motions.find_motion(anim_name_r);
	ASSERT_FMT(anm, "model [%s] has no motion alias defined [%s]", m_visual_name.c_str(), anim_name_r);
	ASSERT_FMT(anm->m_animations.size(), "model [%s] has no motion defined in motion_alias [%s]", m_visual_name.c_str(), anim_name_r);

	if (randomAnim)
		rnd_idx = (u8)Random.randI(anm->m_animations.size());
	const motion_descr& M = anm->m_animations[rnd_idx];

	IKinematicsAnimated* ka = m_model->dcast_PKinematicsAnimated();
	u32 ret = g_player_hud->anim_play(m_attach_place_idx, M, bMixIn, md, M.speed_k, m_has_separated_hands, ka);

	if (ka)
	{
		shared_str item_anm_name;
		if (anm->m_base_name != anm->m_additional_name)
			item_anm_name = anm->m_additional_name;
		else
			item_anm_name = M.name;

		MotionID M2 = ka->ID_Cycle_Safe(item_anm_name);
		if (!M2.valid())
			M2 = ka->ID_Cycle_Safe("idle");
		else if (bDebug)
			Msg("playing item animation [%s]", item_anm_name.c_str());

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
			B->speed *= M.speed_k;
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
	if (Core.Features.test(xrCore::Feature::wpn_bobbing))
		m_bobbing = xr_new<CWeaponBobbing>();
}

player_hud::~player_hud()
{
	if (m_model)
	{
		IRenderVisual* v = m_model->dcast_RenderVisual();
		::Render->model_Delete(v);
		m_model = nullptr;
	}

	xr_vector<attachable_hud_item*>::iterator it = m_pool.begin();
	xr_vector<attachable_hud_item*>::iterator it_e = m_pool.end();
	for (; it != it_e; ++it)
	{
		attachable_hud_item* a = *it;
		xr_delete(a);
	}
	m_pool.clear();
	if (m_bobbing)
		xr_delete(m_bobbing);
}

void player_hud::load(const shared_str& player_hud_sect)
{
	if (player_hud_sect == m_sect_name)
		return;

	bool b_reload = (m_model != nullptr);
	if (m_model)
	{
		IRenderVisual* v = m_model->dcast_RenderVisual();
		::Render->model_Delete(v);
		m_model = nullptr;
	}

	if (!pSettings->line_exist(player_hud_sect, "visual"))
		return;

	m_sect_name = player_hud_sect;
	const shared_str& model_name = pSettings->r_string(player_hud_sect, "visual");
	m_model = smart_cast<IKinematicsAnimated*>(::Render->model_Create(model_name.c_str()));

	CInifile::Sect& _sect = pSettings->r_section(player_hud_sect);
	auto _b = _sect.Data.cbegin();
	auto _e = _sect.Data.cend();
	for (; _b != _e; ++_b)
	{
		if (strstr(_b->first.c_str(), "ancor_") == _b->first.c_str())
		{
			const shared_str& _bone = _b->second;
			m_ancors.push_back(m_model->dcast_PKinematics()->LL_BoneID(_bone));
		}
	}

	//	Msg("hands visual changed to[%s] [%s] [%s]", model_name.c_str(), b_reload?"R":"", m_attached_items[0]?"Y":"");

	if (!b_reload)
	{
		m_model->PlayCycle("hand_idle_doun");
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
	if (m_attached_items[0])
		m_attached_items[0]->render_item_ui();

	if (m_attached_items[1])
		m_attached_items[1]->render_item_ui();
}

void player_hud::render_hud()
{
	if (!m_attached_items[0] && !m_attached_items[1])
		return;

	bool b_r0 = (m_attached_items[0] && m_attached_items[0]->need_renderable());
	bool b_r1 = (m_attached_items[1] && m_attached_items[1]->need_renderable());
	bool b_has_hands = (m_attached_items[0] && m_attached_items[0]->m_has_separated_hands) || (m_attached_items[1] && m_attached_items[1]->m_has_separated_hands);

	if (!b_r0 && !b_r1)
		return;

	if (m_model && b_has_hands)
	{
		::Render->set_Transform(&m_transform);
		::Render->add_Visual(m_model->dcast_RenderVisual());
	}

	if (m_attached_items[0])
		m_attached_items[0]->render();

	if (m_attached_items[1])
		m_attached_items[1]->render();
}

#include "../xr_3da/motion.h"

u32 player_hud::motion_length(const shared_str& anim_name, const shared_str& hud_name, const CMotionDef*& md)
{
	attachable_hud_item* pi = create_hud_item(hud_name);
	player_hud_motion* pm = pi->m_hand_motions.find_motion(anim_name);
	if (!pm)
		return 100; // ms TEMPORARY
	ASSERT_FMT(pm, "hudItem model [%s] has no motion with alias [%s]", hud_name.c_str(), anim_name.c_str());
	return motion_length(pm->m_animations[0], md, pm->m_animations[0].speed_k, pi->m_has_separated_hands, smart_cast<IKinematicsAnimated*>(pi->m_model), pi);
}

u32 player_hud::motion_length(const motion_descr& M, const CMotionDef*& md, float speed, bool hasHands, IKinematicsAnimated* itemModel, attachable_hud_item* pi)
{
	if (pi)
		hasHands = pi->m_has_separated_hands;

	IKinematicsAnimated* model = m_model;
	if ((!model || !hasHands) && itemModel)
		model = itemModel;
	//Msg("~~[%s] model->LL_GetMotionDef [%s] [%s], hasHands = [%u]", __FUNCTION__, M.name.c_str(), model->dcast_RenderVisual()->getDebugName().c_str(), hasHands);
	md = model->LL_GetMotionDef(M.mid);
	VERIFY(md);
	if (md->flags & esmStopAtEnd)
	{
		CMotion* motion = model->LL_GetRootMotion(M.mid);
		return iFloor(0.5f + 1000.f * motion->GetLength() / (md->Speed() * speed) * M.stop_k);
	}
	return 0;
}

void player_hud::update(const Fmatrix& cam_trans)
{
	Fmatrix trans = cam_trans;
	update_inertion(trans);
	update_additional(trans);

	Fvector ypr;
	if (m_attached_items[0])
		ypr = m_attached_items[0]->hands_attach_rot();
	else if (m_attached_items[1])
		ypr = m_attached_items[1]->hands_attach_rot();
	else
		ypr = Fvector().set(0.f, 0.f, 0.f);

	ypr.mul(PI / 180.f);
	m_attach_offset.setHPB(ypr.x, ypr.y, ypr.z);

	Fvector tmp;
	if (m_attached_items[0])
		tmp = m_attached_items[0]->hands_attach_pos();
	else if (m_attached_items[1])
		tmp = m_attached_items[1]->hands_attach_pos();
	else
		tmp = Fvector().set(0.f, 0.f, 0.f);

	m_attach_offset.translate_over(tmp);

	if (bobbing_allowed())
		m_bobbing->Update(m_attach_offset, m_attached_items[0]);

	m_transform.mul(trans, m_attach_offset);
	// insert inertion here

	if (m_model)
	{
		m_model->UpdateTracks();
		m_model->dcast_PKinematics()->CalculateBones_Invalidate();
		m_model->dcast_PKinematics()->CalculateBones(TRUE);
	}

	if (m_attached_items[0])
		m_attached_items[0]->update(true);

	if (m_attached_items[1])
		m_attached_items[1]->update(true);
}

u32 player_hud::anim_play(u16 part, const motion_descr& M, BOOL bMixIn, const CMotionDef*& md, float speed, bool hasHands, IKinematicsAnimated* itemModel)
{
	//Msg("~~[%s] model->LL_GetMotionDef [%s] [%s] attached_item(0): [%p], hasHands = [%u]", __FUNCTION__, M.name.c_str(), itemModel ? itemModel->dcast_RenderVisual()->getDebugName().c_str() : "", attached_item(0), hasHands);
	//Msg("~~[%s] model->LL_GetMotionDef [%s] [%s], hasHands = [%u]", __FUNCTION__, M.name.c_str(), itemModel ? itemModel->dcast_RenderVisual()->getDebugName().c_str() : "", hasHands);

	if (m_model && hasHands)
	{
		u16 part_id = u16(-1);
		if (attached_item(0) && attached_item(1))
			part_id = m_model->partitions().part_id((part == 0) ? "right_hand" : "left_hand");

		u16 pc = m_model->partitions().count();
		for (u16 pid = 0; pid < pc; ++pid)
		{
			if (pid == 0 || pid == part_id || part_id == u16(-1))
			{
				CBlend* B = m_model->PlayCycle(pid, M.mid, bMixIn);
				R_ASSERT(B);
				B->speed *= speed;
			}
		}
		m_model->dcast_PKinematics()->CalculateBones_Invalidate();
	}

	return motion_length(M, md, speed, hasHands, itemModel);
}

void player_hud::update_additional(Fmatrix& trans)
{
	if (m_attached_items[0])
		m_attached_items[0]->update_hud_additional(trans);

	if (m_attached_items[1])
		m_attached_items[1]->update_hud_additional(trans);
}

void player_hud::update_inertion(Fmatrix& trans)
{
	if (inertion_allowed())
	{
		attachable_hud_item* pMainHud = m_attached_items[0];

		Fmatrix xform;
		Fvector& origin = trans.c;
		xform = trans;

		static Fvector st_last_dir = { 0, 0, 0 };

		// load params
		hud_item_measures::inertion_params inertion_data;
		if (pMainHud != nullptr)
		{ // Загружаем параметры инерции из основного худа
			inertion_data.m_pitch_offset_r = pMainHud->m_measures.m_inertion_params.m_pitch_offset_r;
			inertion_data.m_pitch_offset_n = pMainHud->m_measures.m_inertion_params.m_pitch_offset_n;
			inertion_data.m_pitch_offset_d = pMainHud->m_measures.m_inertion_params.m_pitch_offset_d;
			inertion_data.m_pitch_low_limit = pMainHud->m_measures.m_inertion_params.m_pitch_low_limit;
			inertion_data.m_origin_offset = pMainHud->m_measures.m_inertion_params.m_origin_offset;
			inertion_data.m_origin_offset_aim = pMainHud->m_measures.m_inertion_params.m_origin_offset_aim;
			inertion_data.m_tendto_speed = pMainHud->m_measures.m_inertion_params.m_tendto_speed;
			inertion_data.m_tendto_speed_aim = pMainHud->m_measures.m_inertion_params.m_tendto_speed_aim;
		}
		else
		{ // Загружаем дефолтные параметры инерции
			inertion_data.m_pitch_offset_r = PITCH_OFFSET_R;
			inertion_data.m_pitch_offset_n = PITCH_OFFSET_N;
			inertion_data.m_pitch_offset_d = PITCH_OFFSET_D;
			inertion_data.m_pitch_low_limit = PITCH_LOW_LIMIT;
			inertion_data.m_origin_offset = ORIGIN_OFFSET;
			inertion_data.m_origin_offset_aim = ORIGIN_OFFSET_AIM;
			inertion_data.m_tendto_speed = TENDTO_SPEED;
			inertion_data.m_tendto_speed_aim = TENDTO_SPEED_AIM;
		}

		// calc difference
		Fvector diff_dir;
		diff_dir.sub(xform.k, st_last_dir);

		// clamp by PI_DIV_2
		Fvector last;
		last.normalize_safe(st_last_dir);
		float dot = last.dotproduct(xform.k);
		if (dot < EPS)
		{
			Fvector v0;
			v0.crossproduct(st_last_dir, xform.k);
			st_last_dir.crossproduct(xform.k, v0);
			diff_dir.sub(xform.k, st_last_dir);
		}

		// tend to forward
		float _tendto_speed, _origin_offset;
		if (pMainHud != nullptr && pMainHud->m_parent_hud_item->GetCurrentHudOffsetIdx() > 0)
		{ // Худ в режиме "Прицеливание"
			float factor = pMainHud->m_parent_hud_item->GetInertionFactor();
			_tendto_speed = inertion_data.m_tendto_speed_aim - (inertion_data.m_tendto_speed_aim - inertion_data.m_tendto_speed) * factor;
			_origin_offset =
				inertion_data.m_origin_offset_aim - (inertion_data.m_origin_offset_aim - inertion_data.m_origin_offset) * factor;
		}
		else
		{ // Худ в режиме "От бедра"
			_tendto_speed = inertion_data.m_tendto_speed;
			_origin_offset = inertion_data.m_origin_offset;
		}

		// Фактор силы инерции
		if (pMainHud != nullptr)
		{
			float power_factor = pMainHud->m_parent_hud_item->GetInertionPowerFactor();
			_tendto_speed *= power_factor;
			_origin_offset *= power_factor;
		}

		st_last_dir.mad(diff_dir, _tendto_speed * Device.fTimeDelta);
		origin.mad(diff_dir, _origin_offset);

		// pitch compensation
		float pitch = angle_normalize_signed(xform.k.getP());

		if (pMainHud != nullptr)
			pitch *= pMainHud->m_parent_hud_item->GetInertionFactor();

		// Отдаление\приближение
		origin.mad(xform.k, -pitch * inertion_data.m_pitch_offset_d);

		// Сдвиг в противоположную часть экрана
		origin.mad(xform.i, -pitch * inertion_data.m_pitch_offset_r);

		// Подьём\опускание
		clamp(pitch, inertion_data.m_pitch_low_limit, PI);
		origin.mad(xform.j, -pitch * inertion_data.m_pitch_offset_n);
	}
}

attachable_hud_item* player_hud::create_hud_item(const shared_str& sect)
{
	xr_vector<attachable_hud_item*>::iterator it = m_pool.begin();
	xr_vector<attachable_hud_item*>::iterator it_e = m_pool.end();
	for (; it != it_e; ++it)
	{
		attachable_hud_item* itm = *it;
		if (itm->m_sect_name == sect)
			return itm;
	}
	attachable_hud_item* res = xr_new<attachable_hud_item>(this);
	res->load(sect);
	IKinematicsAnimated* animatedHudItem = smart_cast<IKinematicsAnimated*>(res->m_model);
	res->m_hand_motions.load(res, m_model, animatedHudItem, sect);
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
	}
	pi->m_parent_hud_item = item;
}

void player_hud::detach_item_idx(u16 idx)
{
	if (nullptr == attached_item(idx))
		return;

	m_attached_items[idx]->m_parent_hud_item->on_b_hud_detach();

	m_attached_items[idx]->m_parent_hud_item = nullptr;
	m_attached_items[idx] = nullptr;

	if (idx == 1 && attached_item(0) && m_model)
	{
		u16 part_idR = m_model->partitions().part_id("right_hand");
		u32 bc = m_model->LL_PartBlendsCount(part_idR);
		for (u32 bidx = 0; bidx < bc; ++bidx)
		{
			CBlend* BR = m_model->LL_PartBlend(part_idR, bidx);
			if (!BR)
				continue;

			MotionID M = BR->motionID;

			u16 pc = m_model->partitions().count();
			for (u16 pid = 0; pid < pc; ++pid)
			{
				if (pid != part_idR)
				{
					CBlend* B = m_model->PlayCycle(pid, M, TRUE); // this can destroy BR calling UpdateTracks !
					if (BR->blend_state() != CBlend::eFREE_SLOT)
					{
						u16 bop = B->bone_or_part;
						*B = *BR;
						B->bone_or_part = bop;
					}
				}
			}
		}
	}
	//KRodin: закомментировал этот кусок, не понятно для чего он может быть нужен.
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
	bool hasHands = (m_attached_items[0] && m_attached_items[0]->m_has_separated_hands) || (m_attached_items[1] && m_attached_items[1]->m_has_separated_hands);
	if (m_model && hasHands)
	{
		Fmatrix ancor_m = m_model->dcast_PKinematics()->LL_GetTransform(m_ancors[attach_slot_idx]);
		result.mul(m_transform, ancor_m);
		result.mulB_43(offset);
	}
	else
	{
		result.set(m_transform);
		result.mulB_43(offset);
	}
}

bool player_hud::inertion_allowed()
{
	attachable_hud_item* hi = m_attached_items[0];
	if (hi)
	{
		bool res = (hi->m_parent_hud_item->HudInertionEnabled() && hi->m_parent_hud_item->HudInertionAllowed());
		return res;
	}
	return true;
}

bool player_hud::bobbing_allowed()
{
	attachable_hud_item* hi = m_attached_items[0];
	if (hi)
	{
		return hi->m_parent_hud_item->HudBobbingAllowed();
	}
	return Core.Features.test(xrCore::Feature::wpn_bobbing);
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
}

constexpr const char* BOBBING_SECT = "wpn_bobbing_effector";
constexpr float SPEED_REMINDER = 5.f;

CWeaponBobbing::CWeaponBobbing()
{
	Load();
}

void CWeaponBobbing::Load()
{
	fTime = 0.f;
	fReminderFactor = 0.f;
	is_limping = false;

	m_fAmplitudeRun = pSettings->r_float(BOBBING_SECT, "run_amplitude");
	m_fAmplitudeWalk = pSettings->r_float(BOBBING_SECT, "walk_amplitude");
	m_fAmplitudeLimp = pSettings->r_float(BOBBING_SECT, "limp_amplitude");

	m_fSpeedRun = pSettings->r_float(BOBBING_SECT, "run_speed");
	m_fSpeedWalk = pSettings->r_float(BOBBING_SECT, "walk_speed");
	m_fSpeedLimp = pSettings->r_float(BOBBING_SECT, "limp_speed");

	m_fCrouchFactor = READ_IF_EXISTS(pSettings, r_float, BOBBING_SECT, "crouch_k", 0.75f);
	m_fZoomFactor = READ_IF_EXISTS(pSettings, r_float, BOBBING_SECT, "zoom_k", 1.f);
	m_fScopeZoomFactor = READ_IF_EXISTS(pSettings, r_float, BOBBING_SECT, "scope_zoom_k", m_fZoomFactor);
}

void CWeaponBobbing::CheckState()
{
	dwMState = Actor()->get_state();
	is_limping = Actor()->conditions().IsLimping();
	m_bZoomMode = Actor()->IsZoomAimingMode();
	fTime += Device.fTimeDelta;
}

void CWeaponBobbing::Update(Fmatrix& m, attachable_hud_item* hi)
{
	CheckState();
	if (dwMState & ACTOR_DEFS::mcAnyMove)
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
		Fvector dangle;
		Fmatrix R, mR;
		float k = (dwMState & ACTOR_DEFS::mcCrouch) ? m_fCrouchFactor : 1.f;
		float k2 = k;

		if (m_bZoomMode)
		{
			float zoom_factor = m_fZoomFactor;
			if (hi)
			{
				CWeapon* wpn = smart_cast<CWeapon*>(hi->m_parent_hud_item);
				if (wpn && wpn->IsScopeAttached() && !wpn->IsGrenadeMode())
					zoom_factor = m_fScopeZoomFactor;
			}
			k2 *= zoom_factor;
		}

		float A, ST;

		if (isActorAccelerated(dwMState, m_bZoomMode))
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
		float _cosA = _cos(ST) * A * fReminderFactor;

		m.c.y += _sinA;
		dangle.x = _cosA;
		dangle.z = _cosA;
		dangle.y = _sinA;


		R.setHPB(dangle.x, dangle.y, dangle.z);

		mR.mul(m, R);

		m.k.set(mR.k);
		m.j.set(mR.j);
	}
}

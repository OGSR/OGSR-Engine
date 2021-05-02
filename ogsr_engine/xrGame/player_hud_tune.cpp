#include "stdafx.h"
#include "player_hud.h"
#include "level.h"
#include "debug_renderer.h"
#include "../xr_3da/xr_input.h"
#include "HudManager.h"
#include "HudItem.h"
#include "../xr_3da/Effector.h"
#include "../xr_3da/CameraManager.h"
#include "../xr_3da/FDemoRecord.h"
#include "ui_base.h"
#include "debug_renderer.h"

int g_bHudAdjustMode = 0; // g_bHudAdjustMode = 0-return 1-hud_pos 2-hud_rot 3-itm_pos 4-itm_rot 5-fire_point 6-fire_2_point 7-shell_point";
int g_bHudAdjustItemIdx = 0;
float g_bHudAdjustDeltaPos = 0.0005f;
float g_bHudAdjustDeltaRot = 0.05f;

bool is_attachable_item_tuning_mode()
{
	return	pInput->iGetAsyncKeyState(DIK_LSHIFT) ||
		pInput->iGetAsyncKeyState(DIK_Z) ||
		pInput->iGetAsyncKeyState(DIK_X) ||
		pInput->iGetAsyncKeyState(DIK_C);
}

void tune_remap(const Ivector& in_values, Ivector& out_values)
{
	if (pInput->iGetAsyncKeyState(DIK_LSHIFT))
	{
		out_values = in_values;
	}
	else
		if (pInput->iGetAsyncKeyState(DIK_Z))
		{ //strict by X
			out_values.x = in_values.y;
			out_values.y = 0;
			out_values.z = 0;
		}
		else
			if (pInput->iGetAsyncKeyState(DIK_X))
			{ //strict by Y
				out_values.x = 0;
				out_values.y = in_values.y;
				out_values.z = 0;
			}
			else
				if (pInput->iGetAsyncKeyState(DIK_C))
				{ //strict by Z
					out_values.x = 0;
					out_values.y = 0;
					out_values.z = in_values.y;
				}
				else
				{
					out_values.set(0, 0, 0);
				}
}

void calc_cam_diff_pos(Fmatrix item_transform, Fvector diff, Fvector& res)
{
	Fmatrix							cam_m;
	cam_m.i.set(Device.vCameraRight);
	cam_m.j.set(Device.vCameraTop);
	cam_m.k.set(Device.vCameraDirection);
	cam_m.c.set(Device.vCameraPosition);


	Fvector							res1;
	cam_m.transform_dir(res1, diff);

	Fmatrix							item_transform_i;
	item_transform_i.invert(item_transform);
	item_transform_i.transform_dir(res, res1);
}

void calc_cam_diff_rot(Fmatrix item_transform, Fvector diff, Fvector& res)
{
	Fmatrix							cam_m;
	cam_m.i.set(Device.vCameraRight);
	cam_m.j.set(Device.vCameraTop);
	cam_m.k.set(Device.vCameraDirection);
	cam_m.c.set(Device.vCameraPosition);

	Fmatrix							R;
	R.identity();
	if (!fis_zero(diff.x))
	{
		R.rotation(cam_m.i, diff.x);
	}
	else
		if (!fis_zero(diff.y))
		{
			R.rotation(cam_m.j, diff.y);
		}
		else
			if (!fis_zero(diff.z))
			{
				R.rotation(cam_m.k, diff.z);
			};

	Fmatrix					item_transform_i;
	item_transform_i.invert(item_transform);
	R.mulB_43(item_transform);
	R.mulA_43(item_transform_i);

	R.getHPB(res);

	res.mul(180.0f / PI);
}

void attachable_hud_item::tune(Ivector values)
{
	if (!is_attachable_item_tuning_mode())
		return;

	Fvector					diff;
	diff.set(0, 0, 0);

	if (g_bHudAdjustMode == 3 || g_bHudAdjustMode == 4)
	{
		if (g_bHudAdjustMode == 3)
		{
			if (values.x)	diff.x = (values.x > 0) ? g_bHudAdjustDeltaPos : -g_bHudAdjustDeltaPos;
			if (values.y)	diff.y = (values.y > 0) ? g_bHudAdjustDeltaPos : -g_bHudAdjustDeltaPos;
			if (values.z)	diff.z = (values.z < 0) ? g_bHudAdjustDeltaPos : -g_bHudAdjustDeltaPos;

			Fvector							d;
			Fmatrix							ancor_m;
			m_parent->calc_transform(m_attach_place_idx, Fidentity, ancor_m);
			calc_cam_diff_pos(ancor_m, diff, d);
			m_measures.m_item_attach[0].add(d);
		}
		else
			if (g_bHudAdjustMode == 4)
			{
				if (values.x)	diff.x = (values.x > 0) ? g_bHudAdjustDeltaRot : -g_bHudAdjustDeltaRot;
				if (values.y)	diff.y = (values.y > 0) ? g_bHudAdjustDeltaRot : -g_bHudAdjustDeltaRot;
				if (values.z)	diff.z = (values.z > 0) ? g_bHudAdjustDeltaRot : -g_bHudAdjustDeltaRot;

				Fvector							d;
				Fmatrix							ancor_m;
				m_parent->calc_transform(m_attach_place_idx, Fidentity, ancor_m);

				calc_cam_diff_pos(m_item_transform, diff, d);
				m_measures.m_item_attach[1].add(d);
			}

		if ((values.x) || (values.y) || (values.z))
		{
			Msg("[%s]", m_sect_name.c_str());
			Msg("item_position				= %f,%f,%f", m_measures.m_item_attach[0].x, m_measures.m_item_attach[0].y, m_measures.m_item_attach[0].z);
			Msg("item_orientation			= %f,%f,%f", m_measures.m_item_attach[1].x, m_measures.m_item_attach[1].y, m_measures.m_item_attach[1].z);
			Log("-----------");
		}
	}

	if (g_bHudAdjustMode == 5 || g_bHudAdjustMode == 6 || g_bHudAdjustMode == 7)
	{
		if (values.x)	diff.x = (values.x > 0) ? g_bHudAdjustDeltaPos : -g_bHudAdjustDeltaPos;
		if (values.y)	diff.y = (values.y > 0) ? g_bHudAdjustDeltaPos : -g_bHudAdjustDeltaPos;
		if (values.z)	diff.z = (values.z > 0) ? g_bHudAdjustDeltaPos : -g_bHudAdjustDeltaPos;

		if (g_bHudAdjustMode == 5)
		{
			m_measures.m_fire_point_offset.add(diff);
		}
		if (g_bHudAdjustMode == 6)
		{
			m_measures.m_fire_point2_offset.add(diff);
		}
		if (g_bHudAdjustMode == 7)
		{
			m_measures.m_shell_point_offset.add(diff);
		}
		if ((values.x) || (values.y) || (values.z))
		{
			Msg("[%s]", m_sect_name.c_str());
			Msg("fire_point				= %f,%f,%f", m_measures.m_fire_point_offset.x, m_measures.m_fire_point_offset.y, m_measures.m_fire_point_offset.z);
			Msg("fire_point2			= %f,%f,%f", m_measures.m_fire_point2_offset.x, m_measures.m_fire_point2_offset.y, m_measures.m_fire_point2_offset.z);
			Msg("shell_point			= %f,%f,%f", m_measures.m_shell_point_offset.x, m_measures.m_shell_point_offset.y, m_measures.m_shell_point_offset.z);
			Log("-----------");
		}
	}
}

void attachable_hud_item::debug_draw_firedeps()
{
	bool bForce = (g_bHudAdjustMode == 3 || g_bHudAdjustMode == 4);

	if (g_bHudAdjustMode == 5 || g_bHudAdjustMode == 6 || g_bHudAdjustMode == 7 || bForce)
	{
		CDebugRenderer& render = Level().debug_renderer();

		firedeps			fd;
		setup_firedeps(fd);

		if (g_bHudAdjustMode == 5 || bForce)
			render.draw_aabb(fd.vLastFP, 0.005f, 0.005f, 0.005f, D3DCOLOR_XRGB(255, 0, 0));

		if (g_bHudAdjustMode == 6)
			render.draw_aabb(fd.vLastFP2, 0.005f, 0.005f, 0.005f, D3DCOLOR_XRGB(0, 0, 255));

		if (g_bHudAdjustMode == 7)
			render.draw_aabb(fd.vLastSP, 0.005f, 0.005f, 0.005f, D3DCOLOR_XRGB(0, 255, 0));
	}
}


void player_hud::tune(Ivector _values)
{
	Ivector				values;
	tune_remap(_values, values);

	bool is_16x9 = UI()->is_widescreen();

	if (g_bHudAdjustMode == 1 || g_bHudAdjustMode == 2)
	{
		Fvector diff;
		diff.set(0, 0, 0);

		float _curr_dr = g_bHudAdjustDeltaRot;

		if (m_attached_items[g_bHudAdjustItemIdx] == nullptr)
			return;

		u8 idx = m_attached_items[g_bHudAdjustItemIdx]->m_parent_hud_item->GetCurrentHudOffsetIdx();
		if (idx)
			_curr_dr /= 20.0f;

		Fvector& pos_ = (idx != 0) ? m_attached_items[g_bHudAdjustItemIdx]->hands_offset_pos() : m_attached_items[g_bHudAdjustItemIdx]->hands_attach_pos();
		Fvector& rot_ = (idx != 0) ? m_attached_items[g_bHudAdjustItemIdx]->hands_offset_rot() : m_attached_items[g_bHudAdjustItemIdx]->hands_attach_rot();

		if (g_bHudAdjustMode == 1)
		{
			if (values.x)	diff.x = (values.x > 0) ? g_bHudAdjustDeltaPos : -g_bHudAdjustDeltaPos;
			if (values.y)	diff.y = (values.y < 0) ? g_bHudAdjustDeltaPos : -g_bHudAdjustDeltaPos;
			if (values.z)	diff.z = (values.z < 0) ? g_bHudAdjustDeltaPos : -g_bHudAdjustDeltaPos;

			pos_.add(diff);
		}

		if (g_bHudAdjustMode == 2)
		{
			if (values.x)	diff.y = (values.x > 0) ? _curr_dr : -_curr_dr;
			if (values.y)	diff.x = (values.y > 0) ? _curr_dr : -_curr_dr;
			if (values.z)	diff.z = (values.z > 0) ? _curr_dr : -_curr_dr;

			rot_.add(diff);
		}
		if ((values.x) || (values.y) || (values.z))
		{
			if (idx == 0)
			{
				Msg("[%s]", m_attached_items[g_bHudAdjustItemIdx]->m_sect_name.c_str());
				Msg("hands_position%s				= %f,%f,%f", (is_16x9) ? "_16x9" : "", pos_.x, pos_.y, pos_.z);
				Msg("hands_orientation%s			= %f,%f,%f", (is_16x9) ? "_16x9" : "", rot_.x, rot_.y, rot_.z);
				Log("-----------");
			}
			else
				if (idx == 1)
				{
					Msg("[%s]", m_attached_items[g_bHudAdjustItemIdx]->m_sect_name.c_str());
					Msg("aim_hud_offset_pos%s				= %f,%f,%f", (is_16x9) ? "_16x9" : "", pos_.x, pos_.y, pos_.z);
					Msg("aim_hud_offset_rot%s				= %f,%f,%f", (is_16x9) ? "_16x9" : "", rot_.x, rot_.y, rot_.z);
					Log("-----------");
				}
				else
					if (idx == 2)
					{
						Msg("[%s]", m_attached_items[g_bHudAdjustItemIdx]->m_sect_name.c_str());
						Msg("gl_hud_offset_pos%s				= %f,%f,%f", (is_16x9) ? "_16x9" : "", pos_.x, pos_.y, pos_.z);
						Msg("gl_hud_offset_rot%s				= %f,%f,%f", (is_16x9) ? "_16x9" : "", rot_.x, rot_.y, rot_.z);
						Log("-----------");
					}
		}
	}
	else
		if (g_bHudAdjustMode == 8 || g_bHudAdjustMode == 9)
		{
			if (g_bHudAdjustMode == 8 && (values.z))
				g_bHudAdjustDeltaPos += (values.z > 0) ? 0.001f : -0.001f;

			if (g_bHudAdjustMode == 9 && (values.z))
				g_bHudAdjustDeltaRot += (values.z > 0) ? 0.1f : -0.1f;
		}
		else
		{
			attachable_hud_item* hi = m_attached_items[g_bHudAdjustItemIdx];
			if (!hi)	return;
			hi->tune(values);
		}
}

void hud_draw_adjust_mode()
{
	if (!g_bHudAdjustMode)
		return;

	LPCSTR _text = NULL;
	if (pInput->iGetAsyncKeyState(DIK_LSHIFT))
	{
		_text = "press SHIFT+NUM 0-return|1-hud_pos|2-hud_rot|3-itm_pos|4-itm_rot|5-fire_point|6-fire_2_point|7-shell_point|8-pos_step|9-rot_step";
	}
	else if (pInput->iGetAsyncKeyState(DIK_LCONTROL))
	{
		_text = "press CTRL+NUM 0-item idx 1|1-item idx 2";
	}
	else {
		switch (g_bHudAdjustMode)
		{
		case 1:
			_text = "adjusting HUD POSITION";
			break;
		case 2:
			_text = "adjusting HUD ROTATION";
			break;
		case 3:
			_text = "adjusting ITEM POSITION";
			break;
		case 4:
			_text = "adjusting ITEM ROTATION";
			break;
		case 5:
			_text = "adjusting FIRE POINT";
			break;
		case 6:
			_text = "adjusting FIRE 2 POINT";
			break;
		case 7:
			_text = "adjusting SHELL POINT";
			break;
		case 8:
			_text = "adjusting pos STEP";
			break;
		case 9:
			_text = "adjusting rot STEP";
			break;

		};
	}

	if (_text)
	{
		CGameFont* F = UI()->Font()->pFontDI;
		F->SetAligment(CGameFont::alCenter);
		F->OutSetI(0.f, -0.8f);
		F->SetColor(0xffffffff);
		F->OutNext(_text);
		F->OutNext("for item [%d]%s", g_bHudAdjustItemIdx, g_player_hud->attached_item(u16(g_bHudAdjustItemIdx)) ? "" : " [NOT FOUND!]");
		F->OutNext("delta values dP=%f dR=%f", g_bHudAdjustDeltaPos, g_bHudAdjustDeltaRot);
		F->OutNext("[Z]-x axis [X]-y axis [C]-z axis");
	}
}

void hud_adjust_mode_keyb(int dik)
{
	if (pInput->iGetAsyncKeyState(DIK_LSHIFT))
	{
		if (dik == DIK_NUMPAD0)
			g_bHudAdjustMode = 0;
		if (dik == DIK_NUMPAD1)
			g_bHudAdjustMode = 1;
		if (dik == DIK_NUMPAD2)
			g_bHudAdjustMode = 2;
		if (dik == DIK_NUMPAD3)
			g_bHudAdjustMode = 3;
		if (dik == DIK_NUMPAD4)
			g_bHudAdjustMode = 4;
		if (dik == DIK_NUMPAD5)
			g_bHudAdjustMode = 5;
		if (dik == DIK_NUMPAD6)
			g_bHudAdjustMode = 6;
		if (dik == DIK_NUMPAD7)
			g_bHudAdjustMode = 7;
		if (dik == DIK_NUMPAD8)
			g_bHudAdjustMode = 8;
		if (dik == DIK_NUMPAD9)
			g_bHudAdjustMode = 9;
	}
	if (pInput->iGetAsyncKeyState(DIK_LCONTROL))
	{
		if (dik == DIK_NUMPAD0)
			g_bHudAdjustItemIdx = 0;
		if (dik == DIK_NUMPAD1)
			g_bHudAdjustItemIdx = 1;
	}
}

//////////////////////////////////////////////////////////////////////
// HudItem.cpp: класс родитель для всех предметов имеющих
//				собственный HUD (CWeapon, CMissile etc)
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HudItem.h"
#include "HudSound.h"
#include "physic_item.h"
#include "actor.h"
#include "actoreffector.h"
#include "xrmessages.h"
#include "level.h"
#include "inventory.h"
#include "../xr_3da/camerabase.h"
#include "../Include/xrRender/Kinematics.h"
#include "player_hud.h"

CHudItem::CHudItem(void)
{
	m_huditem_flags.zero();
	m_animation_slot	= u32(-1);
	RenderHud			(TRUE);
	EnableHudInertion	(TRUE);
	AllowHudInertion	(TRUE);
	AllowHudBobbing(Core.Features.test(xrCore::Feature::wpn_bobbing));

	m_bStopAtEndAnimIsRunning = false;
	m_current_motion_def = nullptr;
	m_started_rnd_anim_idx = u8(-1);
	m_dwStateTime		= 0;
}

CHudItem::~CHudItem(void)
{
}

DLL_Pure *CHudItem::_construct	()
{
	m_object			= smart_cast<CPhysicItem*>(this);
	VERIFY				(m_object);

	m_item				= smart_cast<CInventoryItem*>(this);
	VERIFY				(m_item);

	return				(m_object);
}

void CHudItem::Load(LPCSTR section)
{
	//загрузить hud, если он нужен
	if (pSettings->line_exist(section, "hud"))
	{
		hud_sect		= pSettings->r_string(section, "hud");

		if (pSettings->line_exist(hud_sect, "allow_inertion"))
			EnableHudInertion(pSettings->r_bool(hud_sect, "allow_inertion"));

		if (pSettings->line_exist(hud_sect, "allow_bobbing"))
			AllowHudBobbing(pSettings->r_bool(hud_sect, "allow_bobbing"));
	}

	m_animation_slot	= pSettings->r_u32(section,"animation_slot");
}

void CHudItem::PlaySound(HUD_SOUND& hud_snd, const Fvector& position, bool overlap)
{
	Fvector pos = GetHUDmode() ? Fvector().sub(position, ::Sound->listener_position()) : position;
	HUD_SOUND::PlaySound(hud_snd, pos, object().H_Root(), !!GetHUDmode(), false, overlap);
}

void CHudItem::net_Destroy()
{
	m_dwStateTime = 0;
}

BOOL CHudItem::net_Spawn(CSE_Abstract* DC)
{
	return TRUE;
}

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

bool CHudItem::Action(s32 cmd, u32 flags) 
{
	return false;
}

void CHudItem::SwitchState(u32 S)
{
	if (OnClient()) 
		return;

	SetNextState( S );	// Very-very important line of code!!! :)

	if (object().Local() && !object().getDestroy())	
	{
		// !!! Just single entry for given state !!!
		NET_Packet		P;
		object().u_EventGen		(P,GE_WPN_STATE_CHANGE,object().ID());
		P.w_u8			(u8(S));
		object().u_EventSend		(P);
	}
}

void CHudItem::OnEvent		(NET_Packet& P, u16 type)
{
	switch (type)
	{
	case GE_WPN_STATE_CHANGE:
		{
			u8				S;
			P.r_u8			(S);
			OnStateSwitch	(u32(S), GetState());
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
}

bool CHudItem::Activate( bool now )
{
	Show( now );
	OnActiveItem ();
	return true;
}

void CHudItem::Deactivate( bool now )
{
	Hide( now );
	OnHiddenItem ();
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
}

void CHudItem::OnH_A_Chield()
{
}

void CHudItem::OnH_B_Chield()
{
	StopCurrentAnimWithoutCallback();
}

void CHudItem::OnH_B_Independent(bool just_before_destroy)
{
	StopHUDSounds();
	UpdateXForm();
}

void CHudItem::OnH_A_Independent()
{
	if (HudItemData())
		g_player_hud->detach_item(this);
	StopCurrentAnimWithoutCallback();
}

void CHudItem::on_b_hud_detach()
{ 
}

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

u32 CHudItem::PlayHUDMotion(const char* M, const bool bMixIn, const u32 state, const bool randomAnim)
{
	//Msg("~~[%s] Playing motion [%s] for [%s]", __FUNCTION__, M.c_str(), HudSection().c_str());
	u32 anim_time = PlayHUDMotion_noCB(M, bMixIn, randomAnim);
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

u32 CHudItem::PlayHUDMotion(std::initializer_list<const char*> Ms, const bool bMixIn, const u32 state, const bool randomAnim)
{
	for (const auto* M : Ms)
		if (AnimationExist(M))
			return PlayHUDMotion(M, bMixIn, state, randomAnim);

	std::string dbg_anim_name;
	for (const auto* M : Ms) {
		dbg_anim_name += M;
		dbg_anim_name += ", ";
	}
	//Msg("~~[%s] Motions [%s] not found for [%s]", __FUNCTION__, dbg_anim_name.c_str(), HudSection().c_str());

	return 0;
}

u32 CHudItem::PlayHUDMotion_noCB(const shared_str& motion_name, const bool bMixIn, const bool randomAnim)
{
	m_current_motion = motion_name;
	m_started_rnd_anim_idx = 0;

	if (HudItemData())
	{
		return HudItemData()->anim_play(motion_name, bMixIn, m_current_motion_def, m_started_rnd_anim_idx, randomAnim);
	}
	else
	{
		return g_player_hud->motion_length(motion_name, HudSection(), m_current_motion_def);
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

	PlayHUDMotion({ "anim_idle", "anm_idle" }, true, GetState());
}

bool CHudItem::TryPlayAnimIdle()
{
	if (!IsZoomed())
	{
		if (auto pActor = smart_cast<CActor*>(object().H_Parent()))
		{
			const u32 State = pActor->get_state();
			if (State & mcSprint)
			{
				PlayAnimIdleSprint();
				return true;
			}
			else if (!HudBobbingAllowed())
			{
				if (State & mcAnyMove)
				{
					if (!(State & mcCrouch))
					{
						if (State & mcAccel) //Ходьба медленная (SHIFT)
							PlayAnimIdleMovingSlow();
						else
							PlayAnimIdleMoving();
						return true;
					}
					else if(State & mcAccel) //Ходьба в присяде (CTRL+SHIFT)
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
		player_hud_motion* anm = HudItemData()->m_hand_motions.find_motion(anim_name_r);
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
	PlayHUDMotion({ "anm_idle_moving", "anm_idle", "anim_idle_moving", "anim_idle" }, true, GetState());
}

void CHudItem::PlayAnimIdleMovingSlow()
{
	PlayHUDMotion({ "anm_idle_moving_slow", "anm_idle_moving", "anm_idle", "anim_idle_moving", "anim_idle" }, true, GetState());
}

void CHudItem::PlayAnimIdleMovingCrouch()
{
	PlayHUDMotion({ "anm_idle_moving_crouch", "anm_idle_moving", "anm_idle", "anim_idle_moving", "anim_idle" }, true, GetState());
}

void CHudItem::PlayAnimIdleMovingCrouchSlow()
{
	PlayHUDMotion({ "anm_idle_moving_crouch_slow", "anm_idle_moving_crouch", "anm_idle_moving", "anm_idle", "anim_idle_moving", "anim_idle" }, true, GetState());
}

void CHudItem::PlayAnimIdleSprint()
{
	PlayHUDMotion({ "anm_idle_sprint", "anm_idle", "anim_idle_sprint", "anim_idle" }, true, GetState());
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

#include "stdafx.h"
#include "actor_mp_client.h"
#include "inventory.h"
#include "../camerabase.h"

void CActorMP::net_Import	( NET_Packet &P)
{
	net_update			N;

	m_state_holder.read	(P);

	if (m_i_am_dead)
		return;

	if (OnClient())
		SetfHealth		(m_state_holder.state().health);

	if (OnClient())
		SetfRadiation	(m_state_holder.state().radiation*100.0f);

	u8					ActiveSlot = m_state_holder.state().inventory_active_slot;
	if (OnClient()) 
	{
//.		if (ActiveSlot >= SLOTS_TOTAL) inventory().SetActiveSlot(NO_ACTIVE_SLOT);
		if (ActiveSlot >= SLOTS_TOTAL) inventory().Activate(NO_ACTIVE_SLOT);
		else 
		{
			if (inventory().GetActiveSlot() != u32(ActiveSlot) )
				inventory().Activate(u32(ActiveSlot), eImportUpdate);
		};
	}

	N.mstate			= m_state_holder.state().body_state_flags;

	N.dwTimeStamp		= m_state_holder.state().time;
	N.p_pos				= m_state_holder.state().position;

	N.o_model			= m_state_holder.state().model_yaw;
	N.o_torso.yaw		= m_state_holder.state().camera_yaw;
	N.o_torso.pitch		= m_state_holder.state().camera_pitch;
	N.o_torso.roll		= m_state_holder.state().camera_roll;

	if (N.o_torso.roll > PI)
		N.o_torso.roll	-= PI_MUL_2;

	{
		if (Level().IsDemoPlay() || OnServer() || Remote())
		{
			unaffected_r_torso.yaw		= N.o_torso.yaw;
			unaffected_r_torso.pitch	= N.o_torso.pitch;
			unaffected_r_torso.roll		= N.o_torso.roll;

			cam_Active()->yaw	= -N.o_torso.yaw;
			cam_Active()->pitch = N.o_torso.pitch;
		};
	};

	//CSE_ALifeCreatureActor
	N.p_accel				= m_state_holder.state().logic_acceleration;

	process_packet			(N);

	net_update_A			N_A;
	m_States.clear			();

	N_A.State.enabled		= m_state_holder.state().physics_state_enabled;
	N_A.State.angular_vel	= m_state_holder.state().physics_angular_velocity;
	N_A.State.linear_vel	= m_state_holder.state().physics_linear_velocity;
	N_A.State.force			= m_state_holder.state().physics_force;
	N_A.State.torque		= m_state_holder.state().physics_torque;
	N_A.State.position		= m_state_holder.state().physics_position;
	N_A.State.quaternion	= m_state_holder.state().physics_quaternion;

	// interpolcation
	postprocess_packet		(N_A);
}

void CActorMP::postprocess_packet	(net_update_A &N_A)
{

	if (!NET.empty())
		N_A.dwTimeStamp		= NET.back().dwTimeStamp;
	else
		N_A.dwTimeStamp		= Level().timeServer();

	N_A.State.previous_position	= N_A.State.position;
	N_A.State.previous_quaternion = N_A.State.quaternion;

	if (Local() && OnClient() || !g_Alive()) return;

	{
		//-----------------------------------------------
		if (!NET_A.empty() && N_A.dwTimeStamp < NET_A.back().dwTimeStamp) return;
		if (!NET_A.empty() && N_A.dwTimeStamp == NET_A.back().dwTimeStamp)
		{
			NET_A.back() = N_A;
		}
		else
		{
			NET_A.push_back			(N_A);
			if (NET_A.size()>5) NET_A.pop_front();
		};

		if (!NET_A.empty()) m_bInterpolate = true;
	};

	Level().AddObject_To_Objects4CrPr	(this);
	CrPr_SetActivated					(false);
	CrPr_SetActivationStep				(0);
}

void CActorMP::process_packet		(net_update &N)
{
	if (Local() && OnClient())
		return;

	if (!NET.empty() && (N.dwTimeStamp < NET.back().dwTimeStamp))
		return;

	if (g_Alive()) {
		setVisible		((BOOL)!HUDview	());
		setEnabled		(TRUE);
	};

	if (!NET.empty() && (N.dwTimeStamp == NET.back().dwTimeStamp)) {
		NET.back()		= N;
		return;
	}

	NET.push_back		(N);

	if (NET.size() > 5)
		NET.pop_front	();
}

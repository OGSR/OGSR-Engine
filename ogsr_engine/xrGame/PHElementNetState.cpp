#include "StdAfx.h"
#include "physicsshell.h"
#include "phinterpolation.h"
#include "phelement.h"
#include "phobject.h"
#include "phworld.h"
#include "phshell.h"

void CPHElement::get_State(SPHNetState& state)
{
	GetGlobalPositionDynamic(&state.position);
	getQuaternion(state.quaternion);
	m_body_interpolation.GetPosition(state.previous_position,0);
	m_body_interpolation.GetRotation(state.previous_quaternion,0);
	get_LinearVel(state.linear_vel);
	get_AngularVel(state.angular_vel);
	getForce(state.force);
	getTorque(state.torque);
	if(!isActive()) 
	{
		state.enabled=false;
		return;
	}
	state.enabled=!!dBodyIsEnabled(m_body);
}
void CPHElement::set_State(const SPHNetState& state)
{
	//bUpdate=true;
	m_flags.set(flUpdate,TRUE);
	SetGlobalPositionDynamic(state.position);
	setQuaternion(state.quaternion);
	m_body_interpolation.SetPosition(state.previous_position,0);
	m_body_interpolation.SetRotation(state.previous_quaternion,0);
	m_body_interpolation.SetPosition(state.position,1);
	m_body_interpolation.SetRotation(state.quaternion,1);
	set_LinearVel(state.linear_vel);
	set_AngularVel(state.angular_vel);
	setForce(state.force);
	setTorque(state.torque);
	if(!isActive()) return;
	if(state.enabled&& !dBodyIsEnabled(m_body)) 
	{
		dBodyEnable(m_body);
		m_shell->EnableObject(0);
	}
	if(!state.enabled && dBodyIsEnabled(m_body)) 
	{
		m_shell->DisableObject();
		Disable();
	}
	CPHDisablingFull::Reinit();
	m_flags.set(flUpdate,TRUE);
}

#include "stdafx.h"
#ifdef DEBUG
#include "ode_include.h"
#include "../StatGraph.h"
#include "PHDebug.h"
#endif
#include "alife_space.h"
#include "hit.h"
#include "PHDestroyable.h"
#include "car.h"
#include "../skeletoncustom.h"
#include "PHWorld.h"
extern CPHWorld*	ph_world;
CCar::SCarSound::SCarSound(CCar* car)
{
	volume                 =1.f;
	pcar=car;
	relative_pos.set(0.f,0.5f,-1.f);
}

CCar::SCarSound::~SCarSound()
{
	
}
void CCar::SCarSound::Init()
{
	CInifile* ini=smart_cast<CKinematics*>(pcar->Visual())->LL_UserData();
	if (ini->section_exist("car_sound") && ini->line_exist("car_sound","snd_volume"))
	{
		volume  			= ini->r_float("car_sound","snd_volume");

		snd_engine.create		(ini->r_string("car_sound","snd_name"),st_Effect,sg_SourceType);//
		snd_engine_start.create	(READ_IF_EXISTS(ini,r_string,"car_sound","engine_start","car\\test_car_start"),st_Effect,sg_SourceType);
		snd_engine_stop.create	(READ_IF_EXISTS(ini,r_string,"car_sound","engine_stop","car\\test_car_stop"),st_Effect,sg_SourceType);
		float fengine_start_delay=READ_IF_EXISTS(ini,r_float,"car_sound","engine_sound_start_dellay",0.25f);
		engine_start_delay=iFloor((snd_engine_start._handle() ? snd_engine_start._handle()->length_ms() : 1.f)*fengine_start_delay);
		if(ini->line_exist("car_sound","relative_pos"))
		{
			relative_pos.set(ini->r_fvector3("car_sound","relative_pos"));
		}
		if(ini->line_exist("car_sound","transmission_switch"))
		{
			snd_transmission.create(ini->r_string("car_sound","transmission_switch"),st_Effect,sg_SourceType);
		}
	
	
	} else {
		Msg					("! Car doesn't contain sound params");
	}
	eCarSound=sndOff;
}
void CCar::SCarSound::SetSoundPosition(ref_sound &snd)
{
	VERIFY(!ph_world->Processing());
	if (snd._feedback())
	{
		Fvector pos;
		pcar->XFORM().transform_tiny(pos,relative_pos);
		snd.set_position		(pos);
	}
}
void CCar::SCarSound::UpdateStarting()
{	
	VERIFY(!ph_world->Processing());
	SetSoundPosition(snd_engine_start);

	if(snd_engine._feedback())
	{
			UpdateDrive();
	} else
	{
		
		if(time_state_start+engine_start_delay<Device.dwTimeGlobal)
		{
			snd_engine.play(pcar,sm_Looped);
			UpdateDrive();
		}
	}

	if(!snd_engine_start._feedback())Drive();
}
void CCar::SCarSound::UpdateStoping()
{
	VERIFY(!ph_world->Processing());
	SetSoundPosition(snd_engine_stop);
	if(!snd_engine_stop._feedback())SwitchOff();
}
void CCar::SCarSound::UpdateStalling()
{
	SetSoundPosition(snd_engine_stop);
	if(!snd_engine_stop._feedback())SwitchOff();
}
void CCar::SCarSound::UpdateDrive()
{
VERIFY(!ph_world->Processing());
float		scale							= 0.5f+0.5f*pcar->m_current_rpm/pcar->m_torque_rpm; clamp(scale,0.5f,1.25f);
			snd_engine.set_frequency		(scale);
			SetSoundPosition(snd_engine);
}
void CCar::SCarSound::SwitchState(ESoundState new_state)
{
	eCarSound=new_state;
	time_state_start=Device.dwTimeGlobal;
}
void CCar::SCarSound::Update()
{
	VERIFY(!ph_world->Processing());
	if(eCarSound==sndOff) return;
	
	switch (eCarSound)
	{
	case sndStarting	:UpdateStarting	()	;	break;
	case sndDrive		:UpdateDrive	()	;	break;
	case sndStalling	:UpdateStalling	()	;	break;
	case sndStoping		:UpdateStalling	()	;	break;
	}
	

}

void CCar::SCarSound::SwitchOn()
{
	pcar->processing_activate();
}
void CCar::SCarSound::Destroy()
{
	SwitchOff();
	snd_engine.destroy	();
	snd_transmission.destroy();
	snd_engine_stop.destroy();
	snd_engine_start.destroy();
}

void CCar::SCarSound::SwitchOff()
{
	eCarSound=sndOff;
	pcar->processing_deactivate();
}

void CCar::SCarSound::Start()
{
	VERIFY(!ph_world->Processing());
	if(eCarSound==sndOff) SwitchOn();
	SwitchState(sndStarting);
	snd_engine_start.play(pcar);
	SetSoundPosition(snd_engine_start);
}

void CCar::SCarSound::Stall()
{
	VERIFY(!ph_world->Processing());
	if(eCarSound==sndOff)return;
	SwitchState(sndStalling);
	snd_engine.stop_deffered();
	snd_engine_stop.play(pcar);
	SetSoundPosition(snd_engine_stop);
}

void CCar::SCarSound::Stop()
{
	VERIFY(!ph_world->Processing());
	if(eCarSound==sndOff)return;
	SwitchState(sndStoping);
	snd_engine.stop_deffered();
	snd_engine_stop.play(pcar);
	SetSoundPosition(snd_engine_stop);
}

void CCar::SCarSound::Drive()
{
	VERIFY(!ph_world->Processing());
	if(eCarSound==sndOff) SwitchOn();
	SwitchState(sndDrive);
	if(!snd_engine._feedback())snd_engine.play(pcar,sm_Looped);
	SetSoundPosition(snd_engine);
}
void CCar::SCarSound::TransmissionSwitch()
{
	VERIFY(!ph_world->Processing());
	if(snd_transmission._handle()&&eCarSound!=sndOff)
	{
		snd_transmission.play(pcar);
		SetSoundPosition(snd_transmission);
	}
}


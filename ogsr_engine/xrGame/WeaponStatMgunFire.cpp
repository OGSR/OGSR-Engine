#include "stdafx.h"
#include "WeaponStatMgun.h"
#include "level.h"
#include "entity_alive.h"
#include "hudsound.h"
#include "actor.h"
#include "actorEffector.h"
#include "EffectorShot.h"

const Fvector&	CWeaponStatMgun::get_CurrentFirePoint()
{
	return m_fire_pos;
}

const Fmatrix&	CWeaponStatMgun::get_ParticlesXFORM	()						
{
	return m_fire_bone_xform;
}

void CWeaponStatMgun::FireStart()
{
	m_dAngle.set(0.0f,0.0f);
	inheritedShooting::FireStart();
}

void CWeaponStatMgun::FireEnd()	
{
	m_dAngle.set(0.0f,0.0f);
	inheritedShooting::FireEnd();
	StopFlameParticles	();
	RemoveShotEffector ();
}

void CWeaponStatMgun::UpdateFire()
{
	fTime -= Device.fTimeDelta;
	

	inheritedShooting::UpdateFlameParticles();
	inheritedShooting::UpdateLight();

	if(!IsWorking()){
		if(fTime<0) fTime = 0.f;
		return;
	}

	if(fTime<=0){
		OnShot();
		fTime += fTimeToFire;
	}else{
		angle_lerp		(m_dAngle.x,0.f,5.f,Device.fTimeDelta);
		angle_lerp		(m_dAngle.y,0.f,5.f,Device.fTimeDelta);
	}
}


void CWeaponStatMgun::OnShot()
{
	VERIFY(Owner());

	FireBullet				(	m_fire_pos, m_fire_dir, fireDispersionBase, *m_Ammo, 
								Owner()->ID(),ID(), SendHitAllowed(Owner()));

	StartShotParticles		();
	
	if(m_bLightShotEnabled) 
		Light_Start			();

	StartFlameParticles		();
	StartSmokeParticles		(m_fire_pos, zero_vel);
	OnShellDrop				(m_fire_pos, zero_vel);

	bool b_hud_mode =			(Level().CurrentEntity() == smart_cast<CObject*>(Owner()));
	HUD_SOUND::PlaySound	(sndShot, m_fire_pos, Owner(), b_hud_mode);

	AddShotEffector			();
	m_dAngle.set			(	::Random.randF(-fireDispersionBase,fireDispersionBase),
								::Random.randF(-fireDispersionBase,fireDispersionBase));
}

void CWeaponStatMgun::AddShotEffector				()
{
	if(OwnerActor())
	{
		CCameraShotEffector* S	= smart_cast<CCameraShotEffector*>(OwnerActor()->Cameras().GetCamEffector(eCEShot)); 
		if (!S)	S				= (CCameraShotEffector*)OwnerActor()->Cameras().AddCamEffector(xr_new<CCameraShotEffector> (camMaxAngle,camRelaxSpeed, 0.25f, 0.01f, 0.7f));
		R_ASSERT				(S);
		S->Shot					(0.01f);
	}
}

void  CWeaponStatMgun::RemoveShotEffector	()
{
	if(OwnerActor())
		OwnerActor()->Cameras().RemoveCamEffector	(eCEShot);
}
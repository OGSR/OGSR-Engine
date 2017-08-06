#include "stdafx.h"
#include "EffectorFall.h"
#include "CameraEffector.h"

#define FALL_SPEED 3.5f
#define FALL_MAXDIST 0.15f

CEffectorFall::CEffectorFall(float power,float life_time) : CEffectorCam(eCEFall, life_time)
{
	fPower	= (power>1)?1:((power<0)?0:power*power);
	fPhase	= 0;
}

CEffectorFall::~CEffectorFall()
{
}

BOOL CEffectorFall::Process(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect)
{
	fPhase+=FALL_SPEED*Device.fTimeDelta;
	if (fPhase<1)	p.y-=FALL_MAXDIST*fPower*_sin(M_PI*fPhase+M_PI);
	else			fLifeTime=-1;
	return TRUE;
}

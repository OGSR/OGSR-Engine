#pragma once

#include "../../../../effectorPP.h"
#include "../../../CameraEffector.h"
#include "../../../../cameramanager.h"

//////////////////////////////////////////////////////////////////////////
// Vampire Postprocess Effector
//////////////////////////////////////////////////////////////////////////
class CVampirePPEffector : public CEffectorPP {
	typedef CEffectorPP inherited;	

	SPPInfo state;			//current state
	float	m_total;		// total PP time

public:
					CVampirePPEffector		(const SPPInfo &ppi, float life_time);
	virtual	BOOL	Process					(SPPInfo& pp);
};

//////////////////////////////////////////////////////////////////////////
// Vampire Camera Effector
//////////////////////////////////////////////////////////////////////////
class CVampireCameraEffector : public CEffectorCam {
	typedef CEffectorCam inherited;	

	float	m_time_total;
	Fvector	dangle_target;
	Fvector dangle_current;

	float	m_dist;
	Fvector m_direction;

public:
					CVampireCameraEffector	(float time, const Fvector &src, const Fvector &tgt);
	virtual	BOOL	Process					(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect);
};




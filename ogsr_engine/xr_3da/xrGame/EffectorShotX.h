#pragma once

#include "EffectorShot.h"

class CCameraShotEffectorX : public CCameraShotEffector
{	
	typedef			CCameraShotEffector	inherited;
public:
					CCameraShotEffectorX(float max_angle, float relax_time, float max_angle_horz, float step_angle_horz, float angle_frac = 0.7f);
	virtual			~CCameraShotEffectorX();

	virtual	BOOL	Process				(Fvector &delta_p, Fvector &delta_d, Fvector &delta_n, float& fFov, float& fFar, float& fAspect);
	virtual	void	GetDeltaAngle		(Fvector& delta_angle);
	virtual	void	Shot				(float angle);
	virtual	void	Clear				();
protected:
	virtual void	UpdateActorCamera	(float dPitch, float dYaw);
};
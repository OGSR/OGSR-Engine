#pragma once

#include "EffectorShot.h"

//Этот класс в ЗП удалён и по видимому не используется.
class CCameraShotEffectorX : public CCameraShotEffector
{
    typedef CCameraShotEffector inherited;

public:
    CCameraShotEffectorX(float max_angle, float relax_time, float max_angle_horz, float step_angle_horz, float angle_frac = 0.7f);
    virtual ~CCameraShotEffectorX();

    virtual BOOL ProcessCam(SCamEffectorInfo& info);
    virtual void GetDeltaAngle(Fvector& delta_angle);
    virtual void Shot(float angle);
    virtual void Clear();

protected:
    virtual void UpdateActorCamera(float dPitch, float dYaw);
};
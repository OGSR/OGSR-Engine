#pragma once

#include "../xr_3da/Effector.h"

// приседание после падения
class CEffectorFall : public CEffectorCam
{
    float fPower;
    float fPhase;

public:
    CEffectorFall(float power, float life_time = 1);
    virtual BOOL ProcessCam(SCamEffectorInfo& info);
};

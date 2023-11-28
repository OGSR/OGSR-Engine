// EffectorZoomInertion.h: инерция(покачивания) оружия в режиме
//						   приближения
//////////////////////////////////////////////////////////////////////

#pragma once

#include "CameraEffector.h"
#include "../xr_3da/cameramanager.h"
#include "WeaponMagazined.h"
#include "script_export_space.h"

class CEffectorZoomInertion : public CEffectorCam
{
public:
    //коэффициент скорости "покачивания" прицела
    float m_fFloatSpeed;
    float m_fDispRadius;

    float m_fEpsilon;
    Fvector m_vCurrentPoint;
    Fvector m_vLastPoint;
    Fvector m_vTargetPoint;
    Fvector m_vTargetVel;

    Fvector m_vOldCameraDir;

    u32 m_dwTimePassed;

    //параметры настройки эффектора
    float m_fCameraMoveEpsilon;
    float m_fDispMin;
    float m_fSpeedMin;
    float m_fZoomAimingDispK;
    float m_fZoomAimingSpeedK;
    //время через которое эффектор меняет направление движения
    u32 m_dwDeltaTime;

    void CalcNextPoint();
    void LoadParams(LPCSTR Section, LPCSTR Prefix);

public:
    CEffectorZoomInertion();
    virtual ~CEffectorZoomInertion();

    void Load();
    void SetParams(float disp);

    virtual BOOL ProcessCam(SCamEffectorInfo& info);
    virtual void Init(CWeaponMagazined* pWeapon);

    virtual CEffectorZoomInertion* cast_effector_zoom_inertion() { return this; }

    DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CEffectorZoomInertion)
#undef script_type_list
#define script_type_list save_type_list(CEffectorZoomInertion)

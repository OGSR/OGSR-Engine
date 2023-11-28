// EffectorZoomInertion.cpp: инерция(покачивания) оружия в режиме
//							 приближения
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EffectorZoomInertion.h"

#include "Actor.h"
#include "ActorEffector.h"

constexpr const char* EFFECTOR_ZOOM_SECTION = "zoom_inertion_effector";

bool external_zoom_osc = false; // alpet: флажок внешнего рассчета колебаний прицела (из скриптов).

void switch_zoom_osc(bool bExternal) { external_zoom_osc = bExternal; }

CEffectorZoomInertion* FindEffectorZoomInertion()
{
    CActor* actor = Actor();
    if (!actor)
        return NULL;
    const CEffectorCam* eff = actor->Cameras().GetCamEffector(eCEZoom);
    if (eff)
        return (CEffectorZoomInertion*)(eff);
    return NULL;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffectorZoomInertion::CEffectorZoomInertion() : CEffectorCam(eCEZoom, 100000.f)
{
    Load();
    m_dwTimePassed = 0;
}

CEffectorZoomInertion::~CEffectorZoomInertion() {}

void CEffectorZoomInertion::LoadParams(LPCSTR Section, LPCSTR Prefix)
{
    string256 full_name;
    m_fCameraMoveEpsilon = READ_IF_EXISTS(pSettings, r_float, Section, strconcat(sizeof(full_name), full_name, Prefix, "camera_move_epsilon"),
                                          pSettings->r_float(EFFECTOR_ZOOM_SECTION, "camera_move_epsilon"));
    m_fDispMin = READ_IF_EXISTS(pSettings, r_float, Section, strconcat(sizeof(full_name), full_name, Prefix, "disp_min"), pSettings->r_float(EFFECTOR_ZOOM_SECTION, "disp_min"));
    m_fSpeedMin = READ_IF_EXISTS(pSettings, r_float, Section, strconcat(sizeof(full_name), full_name, Prefix, "speed_min"), pSettings->r_float(EFFECTOR_ZOOM_SECTION, "speed_min"));
    m_fZoomAimingDispK = READ_IF_EXISTS(pSettings, r_float, Section, strconcat(sizeof(full_name), full_name, Prefix, "zoom_aim_disp_k"),
                                        pSettings->r_float(EFFECTOR_ZOOM_SECTION, "zoom_aim_disp_k"));
    m_fZoomAimingSpeedK = READ_IF_EXISTS(pSettings, r_float, Section, strconcat(sizeof(full_name), full_name, Prefix, "zoom_aim_speed_k"),
                                         pSettings->r_float(EFFECTOR_ZOOM_SECTION, "zoom_aim_speed_k"));
    m_dwDeltaTime = READ_IF_EXISTS(pSettings, r_u32, Section, strconcat(sizeof(full_name), full_name, Prefix, "delta_time"), pSettings->r_u32(EFFECTOR_ZOOM_SECTION, "delta_time"));
};

void CEffectorZoomInertion::Load()
{
    LoadParams(EFFECTOR_ZOOM_SECTION, "");

    m_dwTimePassed = 0;

    m_fFloatSpeed = m_fSpeedMin;
    m_fDispRadius = m_fDispMin;

    m_fEpsilon = 2 * m_fFloatSpeed;

    m_vTargetVel.set(0.f, 0.f, 0.f);
    m_vCurrentPoint.set(0.f, 0.f, 0.f);
    m_vTargetPoint.set(0.f, 0.f, 0.f);
    m_vLastPoint.set(0.f, 0.f, 0.f);
}

void CEffectorZoomInertion::Init(CWeaponMagazined* pWeapon)
{
    if (!pWeapon)
        return;

    LoadParams(*pWeapon->cNameSect(), "ezi_");
};

void CEffectorZoomInertion::SetParams(float disp)
{
    float old_disp = m_fDispRadius;

    m_fDispRadius = disp * m_fZoomAimingDispK;
    if (m_fDispRadius < m_fDispMin)
        m_fDispRadius = m_fDispMin;

    m_fFloatSpeed = disp * m_fZoomAimingSpeedK;
    if (m_fFloatSpeed < m_fSpeedMin)
        m_fFloatSpeed = m_fSpeedMin;

    //для того, чтоб сразу прошел пересчет направления
    //движения прицела
    if (!fis_zero(old_disp - m_fDispRadius, EPS))
        m_fEpsilon = 2 * m_fDispRadius;
}

void CEffectorZoomInertion::CalcNextPoint()
{
    //	m_fEpsilon = 2*m_fFloatSpeed;

    float half_disp_radius = m_fDispRadius / 2.f;
    m_vTargetPoint.x = ::Random.randF(-half_disp_radius, half_disp_radius);
    m_vTargetPoint.y = ::Random.randF(-half_disp_radius, half_disp_radius);

    m_vTargetVel.sub(m_vTargetPoint, m_vLastPoint);
};

BOOL CEffectorZoomInertion::ProcessCam(SCamEffectorInfo& info)
{
    bool camera_moved = false;

    //определяем двигал ли прицелом актер
    if (!info.d.similar(m_vOldCameraDir, m_fCameraMoveEpsilon))
        camera_moved = true;

    /*
    Fvector dir;
    dir.sub(m_vCurrentPoint,m_vTargetPoint);*/

    ///	if(dir.magnitude()<m_fEpsilon || m_dwTimePassed>m_dwDeltaTime)
    //	if (m_dwTimePassed>m_dwDeltaTime)
    if (m_dwTimePassed == 0)
    {
        m_vLastPoint.set(m_vCurrentPoint);
        CalcNextPoint();
    }
    else
    {
        while (m_dwTimePassed > m_dwDeltaTime)
        {
            m_dwTimePassed -= m_dwDeltaTime;

            m_vLastPoint.set(m_vTargetPoint);
            CalcNextPoint();
        };
    }

    m_vCurrentPoint.lerp(m_vLastPoint, m_vTargetPoint, float(m_dwTimePassed) / m_dwDeltaTime);

    m_vOldCameraDir = info.d;

    if (!camera_moved)
        info.d.add(m_vCurrentPoint);

    m_dwTimePassed += Device.dwTimeDelta;

    return TRUE;
}

using namespace luabind;

Fvector get_current_point(CEffectorZoomInertion* E) { return E->m_vCurrentPoint; }
Fvector get_last_point(CEffectorZoomInertion* E) { return E->m_vLastPoint; }
Fvector get_target_point(CEffectorZoomInertion* E) { return E->m_vTargetPoint; }

void set_current_point(CEffectorZoomInertion* E, const Fvector src) { E->m_vCurrentPoint.set(src); }
void set_last_point(CEffectorZoomInertion* E, const Fvector src) { E->m_vLastPoint.set(src); }
void set_target_point(CEffectorZoomInertion* E, const Fvector src) { E->m_vTargetPoint.set(src); }

#pragma optimize("s", on)
void CEffectorZoomInertion::script_register(lua_State* L)
{
    module(L)[

        class_<CEffectorZoomInertion>("CEffectorZoomInertion")
            .def_readwrite("float_speed", &CEffectorZoomInertion::m_fFloatSpeed)
            .def_readwrite("disp_radius", &CEffectorZoomInertion::m_fDispRadius)
            .def_readwrite("epsilon", &CEffectorZoomInertion::m_fEpsilon)
            .property("current_point", &get_current_point, &set_current_point)
            .property("last_point", &get_last_point, &set_last_point)
            .property("target_point", &get_target_point, &set_target_point)
            .def_readwrite("target_vel", &CEffectorZoomInertion::m_vTargetVel)
            .def_readwrite("time_passed", &CEffectorZoomInertion::m_dwTimePassed)
            // settings for real-time modify
            .def_readwrite("camera_move_epsilon", &CEffectorZoomInertion::m_fCameraMoveEpsilon)
            .def_readwrite("disp_min", &CEffectorZoomInertion::m_fDispMin)
            .def_readwrite("speed_min", &CEffectorZoomInertion::m_fSpeedMin)
            .def_readwrite("zoom_aim_disp_k", &CEffectorZoomInertion::m_fZoomAimingDispK)
            .def_readwrite("zoom_aim_speed_k", &CEffectorZoomInertion::m_fZoomAimingSpeedK)
            .def_readwrite("delta_time", &CEffectorZoomInertion::m_dwDeltaTime)

        // */
        ,
        def("find_effector_zi", &FindEffectorZoomInertion), def("switch_zoom_osc", &switch_zoom_osc)];
}

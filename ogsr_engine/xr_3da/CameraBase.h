// CameraBase.h: interface for the CCameraBase class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "CameraDefs.h"

// refs
class CObject;

class ENGINE_API CCameraBase
{
protected:
    CObject* parent;

public:
    BOOL bClampYaw, bClampPitch, bClampRoll;
    float yaw, pitch, roll;

    enum
    {
        flRelativeLink = (1 << 0),
        flPositionRigid = (1 << 1),
        flDirectionRigid = (1 << 2),
    };
    Flags32 m_Flags;

    ECameraStyle style;
    Fvector2 lim_yaw, lim_pitch, lim_roll;
    Fvector rot_speed;

    Fvector vPosition;
    Fvector vDirection;
    Fvector vNormal;
    float f_fov;
    float f_aspect;

    IC Fvector Position() const { return vPosition; }
    IC Fvector Direction() const { return vDirection; }
    IC Fvector Up() const { return vNormal; }
    IC Fvector Right() const { return Fvector().crossproduct(vNormal, vDirection); }
    IC float Fov() const { return f_fov; }
    IC float Aspect() const { return f_aspect; }

    int tag;

    Fvector HPB() const { return Fvector().set(yaw, pitch, roll); }

public:
    CCameraBase(CObject* p, u32 flags);
    virtual ~CCameraBase();
    virtual void Load(LPCSTR section);
    void SetParent(CObject* p)
    {
        parent = p;
        VERIFY(p);
    }
    virtual void OnActivate(CCameraBase* old_cam) { ; }
    virtual void OnDeactivate() { ; }
    virtual void Move(int cmd, float val = 0, float factor = 1.0f) { ; }
    virtual void Update(Fvector& point, Fvector& noise_angle) { ; }
    virtual void Get(Fvector& P, Fvector& D, Fvector& N)
    {
        P.set(vPosition);
        D.set(vDirection);
        N.set(vNormal);
    }
    virtual void Set(const Fvector& P, const Fvector& D, const Fvector& N)
    {
        vPosition.set(P);
        vDirection.set(D);
        vNormal.set(N);
    }
    virtual void Set(float Y, float P, float R)
    {
        yaw = Y;
        pitch = P;
        roll = R;
    }

    virtual float GetWorldYaw() { return 0; };
    virtual float GetWorldPitch() { return 0; };

    virtual float CheckLimYaw();
    virtual float CheckLimPitch();
    virtual float CheckLimRoll();

    IC void SetLimYaw(Fvector2 _lim_yaw) { lim_yaw = _lim_yaw; };
    IC void SetLimPitch(Fvector2 _lim_pitch) { lim_pitch = _lim_pitch; };
    IC void SetRotSpeed(Fvector _rot_speed) { rot_speed = _rot_speed; };
    IC void SetYaw(float _yaw) { yaw = _yaw; };
    IC Fvector2 GetLimYaw() const { return lim_yaw; };
    IC Fvector2 GetLimPitch() const { return lim_pitch; };
    IC Fvector GetRotSpeed() const { return rot_speed; };
    IC float GetYaw() const { return yaw; };
};

template <typename T>
IC void tviewport_size(CRenderDeviceBase& D, float _viewport_near, const T& cam_info, float& h_w, float& h_h)
{
    h_h = _viewport_near * tan(deg2rad(cam_info.Fov()) / 2.f);
    VERIFY(_valid(h_h), make_string("invalide viewporrt params fov: %f ", cam_info.Fov()));
    float aspect = D.fASPECT; // cam_info.Aspect();
    VERIFY(aspect > EPS);
    h_w = h_h / aspect;
}

template <typename T>
IC void viewport_size(float _viewport_near, const T& cam_info, float& h_w, float& h_h)
{
    tviewport_size<T>(Device, _viewport_near, cam_info, h_w, h_h);
}

#include "..\COMMON_AI\script_export_space.h"

typedef class_exporter<CCameraBase> CCameraScript;
add_to_type_list(CCameraScript)
#undef script_type_list
#define script_type_list save_type_list(CCameraScript)

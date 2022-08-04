#pragma once
#ifndef PH_CAPTURE_H
#define PH_CAPTURE_H

#include "phobject.h"
#include "gameobject.h"
#include "physicsshellholder.h"

enum EPHCaptureState
{
    cstPulling,
    cstCaptured,
    cstReleased
};

class CPHCharacter;

class CPHCapture : public CPHUpdateObject
{
public:
    CPHCapture(CPHCharacter*, CPhysicsShellHolder*, LPCSTR = nullptr, bool = false);
    CPHCapture(CPHCharacter*, CPhysicsShellHolder*, u16, LPCSTR = nullptr, bool = false);
    virtual ~CPHCapture();

    bool Failed() { return b_failed; };
    void Release();
    void net_Relcase(CObject* O);

public:
    bool m_hard_mode;
    EPHCaptureState e_state;
    float m_capture_force;
    float m_capture_distance;
    float m_pull_distance;
    u32 m_capture_time;
    float m_pull_force;

protected:
    LPCSTR m_capture_section;
    CPHCharacter* m_character;
    CPhysicsElement* m_taget_element;
    CPhysicsShellHolder* m_taget_object;
    dJointID m_joint;
    dJointID m_ajoint;
    dJointFeedback m_joint_feedback;
    Fvector m_capture_pos;
    float m_back_force;
    u32 m_time_start;
    CBoneInstance* m_capture_bone;
    dBodyID m_body;
    CPHIsland m_island;
    bool b_failed;
    bool b_collide;
    bool b_disabled;
    bool b_character_feedback;

private:
    void PullingUpdate();
    void CapturedUpdate();
    void ReleasedUpdate();
    void ReleaseInCallBack();
    void Init(CInifile* ini);
    void Deactivate();
    void CreateBody();
    bool Invalid() { return !m_taget_object->PPhysicsShell() || !m_taget_object->PPhysicsShell()->isActive() || !m_character->b_exist; };

    static void object_contactCallbackFun(bool& do_colide, bool bo1, dContact& c, SGameMtl* /*material_1*/, SGameMtl* /*material_2*/);

    ///////////CPHObject/////////////////////////////
    virtual void PhDataUpdate(dReal step);
    virtual void PhTune(dReal step);

    Fvector GetCapturePosition();

public:
    CPhysicsShellHolder* taget_object() const { return m_taget_object; };
};
#endif

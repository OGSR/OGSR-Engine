/////////////////////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "phcharacter.h"
#include "Physics.h"
#include "ExtendedGeom.h"
#include "PHCapture.h"
#include "Entity.h"
#include "inventory_item.h"
#include "../Include/xrRender/Kinematics.h"
#include "Actor.h"
#include "Inventory.h"
#include "ai/stalker/ai_stalker.h"
#include "ai/monsters/BaseMonster/base_monster.h"
extern class CPHWorld* ph_world;
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
CPHCapture::CPHCapture(CPHCharacter* a_character, CPhysicsShellHolder* a_taget_object, LPCSTR capture_bone, bool hard_mode)
{
    CPHUpdateObject::Activate();

    m_joint = NULL;
    m_ajoint = NULL;
    m_body = NULL;
    m_taget_object = NULL;
    m_character = NULL;
    b_failed = false;
    b_disabled = false;
    b_character_feedback = false;
    e_state = cstPulling;
    m_hard_mode = hard_mode;

    if (!a_taget_object || !a_taget_object->m_pPhysicsShell || !a_taget_object->m_pPhysicsShell->isActive() || smart_cast<CInventoryItem*>(a_taget_object))
    {
        m_taget_object = NULL;
        b_failed = true;
        return;
    }

    if (!a_character || !a_character->b_exist)
    {
        m_taget_object = NULL;
        b_failed = true;
        return;
    }
    m_taget_object = a_taget_object;
    m_character = a_character;

    CObject* capturer_object = smart_cast<CObject*>(m_character->PhysicsRefObject());

    if (!capturer_object)
    {
        m_taget_object = NULL;
        b_failed = true;
        return;
    }

    IKinematics* p_kinematics = smart_cast<IKinematics*>(capturer_object->Visual());

    if (!p_kinematics)
    {
        m_taget_object = NULL;
        b_failed = true;
        return;
    }

    CInifile* ini = nullptr;
    if (smart_cast<CActor*>(m_character->PhysicsRefObject()) && pSettings->section_exist("actor_capture"))
    {
        ini = pSettings;
        m_capture_section = "actor_capture";
    }
    else if (smart_cast<CAI_Stalker*>(m_character->PhysicsRefObject()) && pSettings->section_exist("stalker_capture"))
    {
        ini = pSettings;
        m_capture_section = "stalker_capture";
    }
    else
    {
        ini = p_kinematics->LL_UserData();
        m_capture_section = "capture";
    }

    if (!ini)
    {
        m_taget_object = NULL;
        b_failed = true;
        return;
    }

    if (!ini->section_exist(m_capture_section))
    {
        m_taget_object = NULL;
        b_failed = true;
        return;
    }

    u16 capture_bone_id = p_kinematics->LL_BoneID(capture_bone ? capture_bone : ini->r_string(m_capture_section, "bone"));
    R_ASSERT2(capture_bone_id != BI_NONE, "wrong capture bone");
    m_capture_bone = &p_kinematics->LL_GetBoneInstance(capture_bone_id);

    m_taget_element = m_taget_object->m_pPhysicsShell->NearestToPoint(GetCapturePosition());

    Init(ini);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
CPHCapture::CPHCapture(CPHCharacter* a_character, CPhysicsShellHolder* a_taget_object, u16 a_taget_element, LPCSTR capture_bone, bool hard_mode)
{
    CPHUpdateObject::Activate();
    m_joint = NULL;
    m_ajoint = NULL;
    m_body = NULL;
    b_failed = false;
    b_disabled = false;
    e_state = cstPulling;
    b_character_feedback = false;
    m_taget_object = NULL;
    m_character = NULL;
    m_hard_mode = hard_mode;
    if (!a_taget_object || !a_taget_object->m_pPhysicsShell || !a_taget_object->m_pPhysicsShell->isActive() || smart_cast<CInventoryItem*>(a_taget_object))
    {
        m_taget_object = NULL;
        b_failed = true;
        return;
    }

    if (!a_character || !a_character->b_exist)
    {
        m_taget_object = NULL;
        b_failed = true;
        return;
    }
    m_taget_object = a_taget_object;
    m_character = a_character;

    CObject* capturer_object = smart_cast<CObject*>(m_character->PhysicsRefObject());

    if (!capturer_object)
    {
        m_taget_object = NULL;
        b_failed = true;
        return;
    }

    IKinematics* p_kinematics = smart_cast<IKinematics*>(capturer_object->Visual());

    if (!p_kinematics)
    {
        m_taget_object = NULL;
        b_failed = true;
        return;
    }

    CInifile* ini = nullptr;
    if (smart_cast<CActor*>(m_character->PhysicsRefObject()) && pSettings->section_exist("actor_capture"))
    {
        ini = pSettings;
        m_capture_section = "actor_capture";
    }
    else if (smart_cast<CAI_Stalker*>(m_character->PhysicsRefObject()) && pSettings->section_exist("stalker_capture"))
    {
        ini = pSettings;
        m_capture_section = "stalker_capture";
    }
    else
    {
        ini = p_kinematics->LL_UserData();
        m_capture_section = "capture";
    }

    if (!ini)
    {
        m_taget_object = NULL;
        b_failed = true;
        return;
    }

    if (a_taget_element == BI_NONE)
    {
        m_taget_object = NULL;
        b_failed = true;
        return;
    }

    if (!ini->section_exist(m_capture_section))
    {
        m_taget_object = NULL;
        b_failed = true;
        return;
    }

    u16 capture_bone_id = p_kinematics->LL_BoneID(capture_bone ? capture_bone : ini->r_string(m_capture_section, "bone"));
    R_ASSERT2(capture_bone_id != BI_NONE, "wrong capture bone");
    m_capture_bone = &p_kinematics->LL_GetBoneInstance(capture_bone_id);

    IRenderVisual* V = m_taget_object->Visual();

    if (!V)
    {
        m_taget_object = NULL;
        b_failed = true;
        return;
    }

    IKinematics* K = smart_cast<IKinematics*>(V);

    if (!K)
    {
        m_taget_object = NULL;
        b_failed = true;
        return;
    }

    CBoneInstance& tag_bone = K->LL_GetBoneInstance(a_taget_element);

    if (!tag_bone.callback_param())
    {
        m_taget_object = NULL;
        b_failed = true;
        return;
    }

    m_taget_element = (CPhysicsElement*)tag_bone.callback_param();

    if (!m_taget_element)
    {
        m_taget_object = NULL;
        b_failed = true;
        return;
    }

    Init(ini);
}

void CPHCapture::Init(CInifile* ini)
{
    Fvector dir;
    Fvector capture_bone_position = GetCapturePosition();
    b_character_feedback = true;

    m_taget_element->GetGlobalPositionDynamic(&dir);
    dir.sub(capture_bone_position, dir);

    m_pull_distance = ini->r_float(m_capture_section, "pull_distance");
    if (!m_hard_mode && dir.magnitude() > m_pull_distance)
    {
        m_taget_object = NULL;
        b_failed = true;
        return;
    }

    m_capture_distance = ini->r_float(m_capture_section, "distance");
    m_capture_force = ini->r_float(m_capture_section, "capture_force");
    m_capture_time = ini->r_u32(m_capture_section, "time_limit") * 1000;

    float pull_force_factor = READ_IF_EXISTS(ini, r_float, m_capture_section, "pull_force_factor", 4.f);
    auto ps = m_taget_object->PPhysicsShell();
    m_pull_force = pull_force_factor * ph_world->Gravity() * ps->getMass();

    m_time_start = Device.dwTimeGlobal;

    float pulling_vel_scale = ini->r_float(m_capture_section, "velocity_scale");
    m_taget_element->set_DynamicLimits(default_l_limit * pulling_vel_scale, default_w_limit * pulling_vel_scale);
    // m_taget_element->PhysicsShell()->set_ObjectContactCallback(object_contactCallbackFun);
    m_character->SetObjectContactCallback(object_contactCallbackFun);
    m_island.Init();
    CActor* A = smart_cast<CActor*>(m_character->PhysicsRefObject());
    if (A)
    {
        A->SetWeaponHideState(INV_STATE_BLOCK_ALL, true, true);
        m_hard_mode = true;
    }
    else if (!m_hard_mode)
        m_hard_mode = false;

    ps->applyForce(0, m_pull_force, 0);
}

void CPHCapture::Release()
{
    if (b_failed)
        return;
    if (e_state == cstReleased)
        return;
    if (m_joint)
    {
        m_island.RemoveJoint(m_joint);

        dJointDestroy(m_joint);
    }
    m_joint = NULL;
    if (m_ajoint)
    {
        m_island.RemoveJoint(m_ajoint);
        dJointDestroy(m_ajoint);
    }
    m_ajoint = NULL;
    if (m_body)
    {
        m_island.RemoveBody(m_body);
        dBodyDestroy(m_body);
    }
    m_body = NULL;

    if (e_state == cstPulling && m_taget_element && !m_taget_object->getDestroy() && m_taget_object->PPhysicsShell() && m_taget_object->PPhysicsShell()->isActive())
    {
        m_taget_element->set_DynamicLimits();
    }

    b_failed = false;
    b_collide = true;
    CActor* A = smart_cast<CActor*>(m_character->PhysicsRefObject());
    if (A)
    {
        if (e_state == cstCaptured && !m_taget_object->getDestroy() && m_taget_object->PPhysicsShell() && m_taget_object->PPhysicsShell()->isActive())
        {
            Fvector dir = {0, -1, 0};
            m_taget_object->PPhysicsShell()->applyImpulse(dir, 0.5f * m_taget_object->PPhysicsShell()->getMass());
        }
        A->SetWeaponHideState(INV_STATE_BLOCK_ALL, false);
        //.		A->inventory().setSlotsBlocked(false);
    }
    e_state = cstReleased;
}

void CPHCapture::Deactivate()
{
    Release();
    // if(m_taget_object&&m_taget_element&&!m_taget_object->getDestroy()&&m_taget_object->m_pPhysicsShell&&m_taget_object->m_pPhysicsShell->isActive())
    //{
    //	m_taget_element->set_ObjectContactCallback(0);

    //}
    if (m_character)
        m_character->SetObjectContactCallback(0);
    CPHUpdateObject::Deactivate();
    m_character = NULL;
    m_taget_object = NULL;
    m_taget_element = NULL;
}

#include "stdafx.h"
#include "physicobject.h"
#include "PhysicsShell.h"
#include "Physics.h"
#include "xrserver_objects_alife.h"
#include "..\Include/xrRender/Kinematics.h"
#include "..\Include/xrRender/KinematicsAnimated.h"
#include "../xr_3da/xr_collide_form.h"
#include "game_object_space.h"

#ifdef ANIMATED_PHYSICS_OBJECT_SUPPORT
#include "PhysicsShellAnimator.h"
#endif

CPhysicObject::CPhysicObject(void)
{
    m_type = epotBox;
    m_mass = 10.f;
    m_collision_hit_callback = NULL;
}

CPhysicObject::~CPhysicObject(void) {}

BOOL CPhysicObject::net_Spawn(CSE_Abstract* DC)
{
    CSE_Abstract* e = (CSE_Abstract*)(DC);
    CSE_ALifeObjectPhysic* po = smart_cast<CSE_ALifeObjectPhysic*>(e);
    R_ASSERT(po);
    m_type = EPOType(po->type);
    m_mass = po->mass;
    m_collision_hit_callback = NULL;
    inherited::net_Spawn(DC);
    xr_delete(collidable.model);
    switch (m_type)
    {
    case epotBox:
    case epotFixedChain:
    case epotFreeChain:
    case epotSkeleton: collidable.model = xr_new<CCF_Skeleton>(this); break;

    default: NODEFAULT;
    }

    CPHSkeleton::Spawn(e);
    setVisible(TRUE);
    setEnabled(TRUE);

    if (!PPhysicsShell()->isBreakable() && !CScriptBinder::object() && !CPHSkeleton::IsRemoving())
        SheduleUnregister();

#ifdef ANIMATED_PHYSICS_OBJECT_SUPPORT
    if (PPhysicsShell()->Animated())
    {
        processing_activate();
    }
#endif

    PHObjectPositionUpdate();

    // приложить небольшую силу для того, чтобы объект начал падать
    PPhysicsShell()->applyImpulse(Fvector().set(0.f, -1.0f, 0.f), 0.5f * PPhysicsShell()->getMass());

    return TRUE;
}

void CPhysicObject::SpawnInitPhysics(CSE_Abstract* D)
{
    CreatePhysicsShell(D);
    RunStartupAnim(D);
}
void CPhysicObject::RunStartupAnim(CSE_Abstract* D)
{
    if (Visual() && smart_cast<IKinematics*>(Visual()))
    {
        //		CSE_PHSkeleton	*po	= smart_cast<CSE_PHSkeleton*>(D);
        IKinematicsAnimated* PKinematicsAnimated = NULL;
        R_ASSERT(Visual() && smart_cast<IKinematics*>(Visual()));
        PKinematicsAnimated = smart_cast<IKinematicsAnimated*>(Visual());
        if (PKinematicsAnimated)
        {
            CSE_Visual* visual = smart_cast<CSE_Visual*>(D);
            R_ASSERT(visual);
            R_ASSERT2(*visual->startup_animation, "no startup animation");
            PKinematicsAnimated->PlayCycle(*visual->startup_animation);
        }
        smart_cast<IKinematics*>(Visual())->CalculateBones_Invalidate();
        smart_cast<IKinematics*>(Visual())->CalculateBones();
    }
}
void CPhysicObject::net_Destroy()
{
#ifdef ANIMATED_PHYSICS_OBJECT_SUPPORT
    if (PPhysicsShell()->Animated())
    {
        processing_deactivate();
    }
#endif

    inherited::net_Destroy();
    CPHSkeleton::RespawnInit();
}

void CPhysicObject::net_Save(NET_Packet& P)
{
    inherited::net_Save(P);
    CPHSkeleton::SaveNetState(P);
}
void CPhysicObject::CreatePhysicsShell(CSE_Abstract* e)
{
    CSE_ALifeObjectPhysic* po = smart_cast<CSE_ALifeObjectPhysic*>(e);
    CreateBody(po);
}

void CPhysicObject::CreateSkeleton(CSE_ALifeObjectPhysic* po)
{
    if (m_pPhysicsShell)
        return;
    if (!Visual())
        return;
    LPCSTR fixed_bones = *po->fixed_bones;
    m_pPhysicsShell = P_build_Shell(this, !po->_flags.test(CSE_PHSkeleton::flActive), fixed_bones);
    ApplySpawnIniToPhysicShell(&po->spawn_ini(), m_pPhysicsShell, fixed_bones[0] != '\0');
    ApplySpawnIniToPhysicShell(smart_cast<IKinematics*>(Visual())->LL_UserData(), m_pPhysicsShell, fixed_bones[0] != '\0');
}

void CPhysicObject::Load(LPCSTR section)
{
    inherited::Load(section);
    CPHSkeleton::Load(section);
}

void CPhysicObject::shedule_Update(u32 dt)
{
    inherited::shedule_Update(dt);
    CPHSkeleton::Update(dt);
}
void CPhysicObject::UpdateCL()
{
    inherited::UpdateCL();

#ifdef ANIMATED_PHYSICS_OBJECT_SUPPORT
    //Если наш физический объект анимированный, то
    //двигаем объект за анимацией
    if (m_pPhysicsShell->PPhysicsShellAnimator())
    {
        m_pPhysicsShell->PPhysicsShellAnimator()->OnFrame();
    }
#endif

    PHObjectPositionUpdate();
}
void CPhysicObject::PHObjectPositionUpdate()
{
    if (m_pPhysicsShell)
    {
        if (m_type == epotBox)
        {
            m_pPhysicsShell->Update();
            XFORM().set(m_pPhysicsShell->mXFORM);
        }
        else
            m_pPhysicsShell->InterpolateGlobalTransform(&XFORM());
    }
}

void CPhysicObject::AddElement(CPhysicsElement* root_e, int id)
{
    IKinematics* K = smart_cast<IKinematics*>(Visual());

    CPhysicsElement* E = P_create_Element();
    CBoneInstance& B = K->LL_GetBoneInstance(u16(id));
    E->mXFORM.set(K->LL_GetTransform(u16(id)));
    Fobb bb = K->LL_GetBox(u16(id));

    if (bb.m_halfsize.magnitude() < 0.05f)
    {
        bb.m_halfsize.add(0.05f);
    }
    E->add_Box(bb);
    E->setMass(10.f);
    E->set_ParentElement(root_e);
    B.set_callback(bctPhysics, m_pPhysicsShell->GetBonesCallback(), E);
    m_pPhysicsShell->add_Element(E);
    if (!(m_type == epotFreeChain && root_e == 0))
    {
        CPhysicsJoint* J = P_create_Joint(CPhysicsJoint::full_control, root_e, E);
        J->SetAnchorVsSecondElement(0, 0, 0);
        J->SetAxisDirVsSecondElement(1, 0, 0, 0);
        J->SetAxisDirVsSecondElement(0, 1, 0, 2);
        J->SetLimits(-M_PI / 2, M_PI / 2, 0);
        J->SetLimits(-M_PI / 2, M_PI / 2, 1);
        J->SetLimits(-M_PI / 2, M_PI / 2, 2);
        m_pPhysicsShell->add_Joint(J);
    }

    CBoneData& BD = K->LL_GetData(u16(id));
    for (vecBonesIt it = BD.children.begin(); BD.children.end() != it; ++it)
    {
        AddElement(E, (*it)->GetSelfID());
    }
}

void CPhysicObject::CreateBody(CSE_ALifeObjectPhysic* po)
{
    if (m_pPhysicsShell)
        return;
    IKinematics* pKinematics = smart_cast<IKinematics*>(Visual());
    switch (m_type)
    {
    case epotBox: {
        m_pPhysicsShell = P_build_SimpleShell(this, m_mass, !po->_flags.test(CSE_ALifeObjectPhysic::flActive));
        {
            // для SimpleShell нет привязки к костям и вообще не работают параметры из секцции collide
            // но объекты с таким type есть в all.spawn например визуалы сталкеров на Арене. я так и не нашел как оно раньше могло работать
            // пока сделаем фикс прямо тут

            if (*po->fixed_bones)
            {
                m_pPhysicsShell->get_ElementByStoreOrder(0)->Fix();

                CInifile& ini = po->spawn_ini();

                if (ini.line_exist("collide", "ignore_static"))
                {
                    m_pPhysicsShell->SetIgnoreStatic();
                }

                if (ini.line_exist("collide", "ignore_dynamic"))
                {
                    m_pPhysicsShell->SetIgnoreDynamic();
                }                
            }
        }
    }
    break;
    case epotFixedChain:
    case epotFreeChain: {
        m_pPhysicsShell = P_create_Shell();
        m_pPhysicsShell->set_Kinematics(pKinematics);
        AddElement(0, pKinematics->LL_GetBoneRoot()); //-V595
        m_pPhysicsShell->setMass1(m_mass);
    }
    break;

    case epotSkeleton: {
        // pKinematics->LL_SetBoneRoot(0);
        CreateSkeleton(po);
    }
    break;

    default: {
    }
    break;
    }

    m_pPhysicsShell->mXFORM.set(XFORM());
    m_pPhysicsShell->SetAirResistance(0.001f, 0.02f);
    if (pKinematics)
    {
        SAllDDOParams disable_params;
        disable_params.Load(pKinematics->LL_UserData());
        m_pPhysicsShell->set_DisableParams(disable_params);
    }
    // m_pPhysicsShell->SetAirResistance(0.002f, 0.3f);
}

BOOL CPhysicObject::net_SaveRelevant()
{
    return TRUE; //! m_flags.test(CSE_ALifeObjectPhysic::flSpawnCopy);
}

BOOL CPhysicObject::UsedAI_Locations() { return (FALSE); }

void CPhysicObject::InitServerObject(CSE_Abstract* D)
{
    CPHSkeleton::InitServerObject(D);
    CSE_ALifeObjectPhysic* l_tpALifePhysicObject = smart_cast<CSE_ALifeObjectPhysic*>(D);
    if (!l_tpALifePhysicObject)
        return;
    l_tpALifePhysicObject->type = u32(m_type);
}
ICollisionHitCallback* CPhysicObject::get_collision_hit_callback() { return m_collision_hit_callback; }
void CPhysicObject::set_collision_hit_callback(ICollisionHitCallback* cc)
{
    xr_delete(m_collision_hit_callback);
    m_collision_hit_callback = cc;
}

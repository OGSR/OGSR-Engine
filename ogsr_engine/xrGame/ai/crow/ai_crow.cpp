////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_crow.cpp
//	Created 	: 13.05.2002
//  Modified 	: 13.05.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Crow"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../../physicsshell.h"
#include "ai_crow.h"
#include "../../hudmanager.h"
#include "../../level.h"
#include "../../../Include/xrRender/Kinematics.h"
#include "xrserver_objects_alife_monsters.h"

void CAI_Crow::SAnim::Load(IKinematicsAnimated* visual, LPCSTR prefix)
{
    const MotionID& M = visual->ID_Cycle_Safe(prefix);
    if (M)
        m_Animations.push_back(M);
    for (int i = 0; (i < MAX_ANIM_COUNT) && (m_Animations.size() < MAX_ANIM_COUNT); ++i)
    {
        string128 sh_anim;
        sprintf_s(sh_anim, "%s_%d", prefix, i);
        const MotionID& M = visual->ID_Cycle_Safe(sh_anim);
        if (M)
            m_Animations.push_back(M);
    }
    R_ASSERT(m_Animations.size());
}

void CAI_Crow::SSound::Load(LPCSTR prefix)
{
    string_path fn;
    if (FS.exist(fn, "$game_sounds$", prefix, ".ogg"))
    {
        m_Sounds.push_back(ref_sound());
        ::Sound->create(m_Sounds.back(), prefix, st_Effect, sg_SourceType);
    }
    for (int i = 0; (i < MAX_SND_COUNT) && (m_Sounds.size() < MAX_SND_COUNT); ++i)
    {
        string64 name;
        sprintf_s(name, "%s_%d", prefix, i);
        if (FS.exist(fn, "$game_sounds$", name, ".ogg"))
        {
            m_Sounds.push_back(ref_sound());
            ::Sound->create(m_Sounds.back(), name, st_Effect, sg_SourceType);
        }
    }
    R_ASSERT(m_Sounds.size());
}

void CAI_Crow::SSound::SetPosition(const Fvector& pos)
{
    for (int i = 0; i < (int)m_Sounds.size(); ++i)
        if (m_Sounds[i]._feedback())
            m_Sounds[i].set_position(pos);
}

void CAI_Crow::SSound::Unload()
{
    for (int i = 0; i < (int)m_Sounds.size(); ++i)
        ::Sound->destroy(m_Sounds[i]);
}

void cb_OnHitEndPlaying(CBlend* B) { ((CAI_Crow*)B->CallbackParam)->OnHitEndPlaying(B); }

void CAI_Crow::OnHitEndPlaying(CBlend* /**B/**/) { bPlayDeathIdle = true; }

CAI_Crow::CAI_Crow() { init(); }

CAI_Crow::~CAI_Crow()
{
    // removing all data no more being neded
    m_Sounds.m_idle.Unload();
}

void CAI_Crow::init()
{
    st_current = eUndef;
    st_target = eFlyIdle;
    vGoalDir.set(10.0f * (Random.randF() - Random.randF()), 10.0f * (Random.randF() - Random.randF()), 10.0f * (Random.randF() - Random.randF()));
    vCurrentDir.set(0, 0, 1);
    vHPB.set(0, 0, 0);
    fDHeading = 0;
    fGoalChangeDelta = 10.f;
    fGoalChangeTime = 0.f;
    fSpeed = 5.f;
    fASpeed = 0.2f;
    fMinHeight = 40.f;
    vVarGoal.set(10.f, 10.f, 100.f);
    fIdleSoundDelta = 10.f;
    fIdleSoundTime = fIdleSoundDelta;
    bPlayDeathIdle = false;
}

void CAI_Crow::Load(LPCSTR section)
{
    inherited::Load(section);
    //////////////////////////////////////////////////////////////////////////
    ISpatial* self = smart_cast<ISpatial*>(this);
    if (self)
    {
        self->spatial.type &= ~STYPE_VISIBLEFORAI;
        self->spatial.type &= ~STYPE_REACTTOSOUND;
    }
    //////////////////////////////////////////////////////////////////////////

    // sounds
    m_Sounds.m_idle.Load("monsters\\crow\\idle");
    // play defaut

    fSpeed = pSettings->r_float(section, "speed");
    fASpeed = pSettings->r_float(section, "angular_speed");
    fGoalChangeDelta = pSettings->r_float(section, "goal_change_delta");
    fMinHeight = pSettings->r_float(section, "min_height");
    vVarGoal = pSettings->r_fvector3(section, "goal_variability");
    fIdleSoundDelta = pSettings->r_float(section, "idle_sound_delta");
    fIdleSoundTime = fIdleSoundDelta + fIdleSoundDelta * Random.randF(-.5f, .5f);
}

BOOL CAI_Crow::net_Spawn(CSE_Abstract* DC)
{
    BOOL R = inherited::net_Spawn(DC);
    setVisible(TRUE);
    setEnabled(TRUE);

    // animations
    IKinematicsAnimated* M = smart_cast<IKinematicsAnimated*>(Visual());
    R_ASSERT(M);
    m_Anims.m_death.Load(M, "norm_death");
    m_Anims.m_death_dead.Load(M, "norm_death_dead");
    m_Anims.m_death_idle.Load(M, "norm_death_idle");
    m_Anims.m_fly.Load(M, "norm_fly_fwd");
    m_Anims.m_idle.Load(M, "norm_idle");

    // Crow fixes by Sin! (Gunslinger mod)  --#SM+#--
    // [bug] баг - кто-то забыл проинициализировать, из-за чего при неудачном стечении обстоятельств вороны вне сектора обзора не летают... Исправим.
    o_workload_frame = 0;
    o_workload_rframe = 0;

    // без этого уже сбитые вороны "воскресают" при перезагрузке игры и начинают вести себя некорректно (начинают летать, при попадании не падают на землю) --#SM+#--
    if (GetfHealth() > 0)
    {
        st_current = ECrowStates::eFlyIdle;
        st_target = ECrowStates::eFlyIdle;

        // disable UpdateCL, enable only on HIT
        processing_deactivate();
    }
    else
    {
        st_current = ECrowStates::eDeathFall;
        st_target = ECrowStates::eDeathDead;

        // turn on physic
        processing_activate();
        CreateSkeleton();
    }

    renderable.visual->ignore_optimization = true;

    return R;
}

void CAI_Crow::net_Destroy()
{
    inherited::net_Destroy();

    m_Anims.m_death.m_Animations.clear();
    m_Anims.m_death_dead.m_Animations.clear();
    m_Anims.m_death_idle.m_Animations.clear();
    m_Anims.m_fly.m_Animations.clear();
    m_Anims.m_idle.m_Animations.clear();
}

// crow update
void CAI_Crow::switch2_FlyUp() { smart_cast<IKinematicsAnimated*>(Visual())->PlayCycle(m_Anims.m_fly.GetRandom()); }
void CAI_Crow::switch2_FlyIdle() { smart_cast<IKinematicsAnimated*>(Visual())->PlayCycle(m_Anims.m_idle.GetRandom()); }
void CAI_Crow::switch2_DeathDead()
{
    // AI need to pickup this
    ISpatial* self = smart_cast<ISpatial*>(this);
    if (self)
        self->spatial.type |= STYPE_VISIBLEFORAI;
    //
    smart_cast<IKinematicsAnimated*>(Visual())->PlayCycle(m_Anims.m_death_dead.GetRandom());
}
void CAI_Crow::switch2_DeathFall()
{
    Fvector V;
    V.mul(XFORM().k, fSpeed);
    //	m_PhysicMovementControl->SetVelocity(V);
    smart_cast<IKinematicsAnimated*>(Visual())->PlayCycle(m_Anims.m_death.GetRandom(), TRUE, cb_OnHitEndPlaying, this);
}

void CAI_Crow::state_Flying(float fdt)
{
    // Update position and orientation of the planes
    float fAT = fASpeed * fdt;
    Fvector& vDirection = XFORM().k;

    // Tweak orientation based on last position and goal
    Fvector vOffset;
    vOffset.sub(vGoalDir, Position());

    // First, tweak the pitch
    if (vOffset.y > 1.0)
    { // We're too low
        vHPB.y += fAT;
        if (vHPB.y > 0.8f)
            vHPB.y = 0.8f;
    }
    else if (vOffset.y < -1.0)
    { // We're too high
        vHPB.y -= fAT;
        if (vHPB.y < -0.8f)
            vHPB.y = -0.8f;
    }
    else // Add damping
        vHPB.y *= 0.95f;

    // Now figure out yaw changes
    vOffset.y = 0.0f;
    vDirection.y = 0.0f;

    vDirection.normalize();
    vOffset.normalize();

    float fDot = vDirection.dotproduct(vOffset);
    fDot = (1.0f - fDot) / 2.0f * fAT * 10.0f;

    vOffset.crossproduct(vOffset, vDirection);

    if (vOffset.y > 0.01f)
        fDHeading = (fDHeading * 9.0f + fDot) * 0.1f;
    else if (vOffset.y < 0.01f)
        fDHeading = (fDHeading * 9.0f - fDot) * 0.1f;

    vHPB.x += fDHeading;
    vHPB.z = -fDHeading * 9.0f;

    // Update position
    vOldPosition.set(Position());
    XFORM().setHPB(vHPB.x, vHPB.y, vHPB.z);
    Position().mad(vOldPosition, vDirection, fSpeed * fdt);
}

static Fvector vV = {0, 0, 0};
void CAI_Crow::state_DeathFall()
{
    Fvector tAcceleration;
    tAcceleration.set(0, -10.f, 0);
    if (m_pPhysicsShell)
    {
        Fvector velocity;
        m_pPhysicsShell->get_LinearVel(velocity);
        if (velocity.y > -0.001f)
            st_target = eDeathDead;
    }
    if (bPlayDeathIdle)
    {
        smart_cast<IKinematicsAnimated*>(Visual())->PlayCycle(m_Anims.m_death_idle.GetRandom());
        bPlayDeathIdle = false;
    }
}

void CAI_Crow::Die(CObject* who)
{
    inherited::Die(who);
    processing_activate(); // enable UpdateCL for dead crows - especially for physics support
                           // and do it especially before Creating physics shell or it definitely throws processing enable/disable calls: underflow
    CreateSkeleton();
};
void CAI_Crow::UpdateWorkload(float fdt)
{
    if (o_workload_frame == Device.dwFrame)
        return;
    o_workload_frame = Device.dwFrame;
    switch (st_current)
    {
    case eFlyIdle:
    case eFlyUp: state_Flying(fdt); break;
    case eDeathFall: state_DeathFall(); break;
    }
}
void CAI_Crow::UpdateCL()
{
    inherited::UpdateCL();
    if (m_pPhysicsShell)
    {
        m_pPhysicsShell->Update();
        XFORM().set(m_pPhysicsShell->mXFORM);
    }
}
void CAI_Crow::renderable_Render(u32 context_id, IRenderable* root)
{
    if (o_workload_rframe != Device.dwFrame)
    {
        // CAI_Crow::renderable_Render вызывается только для ВИДИМЫХ В ЭТОМ КАДРЕ объектов --#SM+#--
        // из-за этого вороны, видимые в мире, но не видимые в зуме, замедляются - fDeltaTime для кадров линзы не учитывается
        UpdateWorkload(Device.fTimeDelta * (Device.dwFrame - o_workload_frame));
        o_workload_rframe = Device.dwFrame;
    }

    inherited::renderable_Render(context_id, root);
}
void CAI_Crow::shedule_Update(u32 DT)
{
    float fDT = float(DT) / 1000.F;
    spatial.type &= ~STYPE_VISIBLEFORAI;

    inherited::shedule_Update(DT);

    if (st_target != st_current)
    {
        switch (st_target)
        {
        case eFlyUp: switch2_FlyUp(); break;
        case eFlyIdle: switch2_FlyIdle(); break;
        case eDeathFall: switch2_DeathFall(); break;
        case eDeathDead: switch2_DeathDead(); break;
        }
        st_current = st_target;
    }

    switch (st_current)
    {
    case eFlyIdle:
        if (Position().y > vOldPosition.y)
            st_target = eFlyUp;
        break;
    case eFlyUp:
        if (Position().y <= vOldPosition.y)
            st_target = eFlyIdle;
        break;
    case eDeathFall: state_DeathFall(); break;
    }
    if ((eDeathFall != st_current) && (eDeathDead != st_current))
    {
        // At random times, change the direction (goal) of the plane
        if (fGoalChangeTime <= 0)
        {
            fGoalChangeTime += fGoalChangeDelta + fGoalChangeDelta * Random.randF(-0.5f, 0.5f);
            Fvector vP;
            vP.set(Device.vCameraPosition.x, Device.vCameraPosition.y + fMinHeight, Device.vCameraPosition.z);
            vGoalDir.x = vP.x + vVarGoal.x * Random.randF(-0.5f, 0.5f);
            vGoalDir.y = vP.y + vVarGoal.y * Random.randF(-0.5f, 0.5f);
            vGoalDir.z = vP.z + vVarGoal.z * Random.randF(-0.5f, 0.5f);
        }
        fGoalChangeTime -= fDT;
        // sounds
        if (fIdleSoundTime <= 0)
        {
            fIdleSoundTime = fIdleSoundDelta + fIdleSoundDelta * Random.randF(-0.5f, 0.5f);
            // if (st_current==eFlyIdle)
            ::Sound->play_at_pos(m_Sounds.m_idle.GetRandom(), H_Root(), Position());
        }
        fIdleSoundTime -= fDT;
    }
    m_Sounds.m_idle.SetPosition(Position());

    // work
    if (o_workload_rframe >= (Device.dwFrame - 2)) //--#SM+#--
        ;
    else
        UpdateWorkload(fDT);
}

void CAI_Crow::net_Export(CSE_Abstract* E)
{
    R_ASSERT(Local());

    CSE_ALifeCreatureAbstract* creature = smart_cast<CSE_ALifeCreatureAbstract*>(E);
    creature->fHealth = GetfHealth();
    creature->timestamp = Level().timeServer();
    creature->flags = 0;
    creature->o_Position = Position();
    float yaw, pitch, bank;
    XFORM().getHPB(yaw, pitch, bank);
    creature->o_model = yaw;
    creature->o_torso.yaw = yaw;
    creature->o_torso.pitch = pitch;
    creature->o_torso.roll = 0;
    creature->s_team = u8(g_Team());
    creature->s_squad = u8(g_Squad());
    creature->s_group = u8(g_Group());
}

//---------------------------------------------------------------------
void CAI_Crow::HitSignal(float /**HitAmount/**/, Fvector& /**local_dir/**/, CObject* who, s16 /**element/**/)
{
    SetfHealth(0);
    if (eDeathDead != st_current)
    {
        st_target = eDeathFall;
    }
    else
        smart_cast<IKinematicsAnimated*>(Visual())->PlayCycle(m_Anims.m_death_dead.GetRandom());
}

void CAI_Crow::CreateSkeleton()
{
    m_pPhysicsShell = P_build_SimpleShell(this, 0.3f, false);
    m_pPhysicsShell->SetMaterial(smart_cast<IKinematics*>(Visual())->LL_GetData(smart_cast<IKinematics*>(Visual())->LL_GetBoneRoot()).game_mtl_idx);
}

void CAI_Crow::Hit(SHit* pHDS)
{
    SHit HDS = *pHDS;
    HDS.impulse /= 100.f;
    inherited::Hit(&HDS);
}

BOOL CAI_Crow::UsedAI_Locations() { return (FALSE); }

void CAI_Crow::create_physic_shell()
{
    // do not delete!!!
}

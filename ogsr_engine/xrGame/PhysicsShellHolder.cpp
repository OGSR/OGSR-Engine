#include "stdafx.h"
#include "PhysicsShellHolder.h"
#include "PhysicsShell.h"
#include "xrMessages.h"
#include "ph_shell_interface.h"
#include "../Include/xrRender/Kinematics.h"
#include "script_callback_ex.h"
#include "Level.h"
#include "PHCommander.h"
#include "PHScriptCall.h"
#include "CustomRocket.h"
#include "Grenade.h"
#include "phworld.h"
#include "phactivationshape.h"
#include "phvalide.h"
#include "PHElement.h"
#include "PHMovementControl.h"
#include "CharacterPhysicsSupport.h"
#include "Actor.h"

CPhysicsShellHolder::CPhysicsShellHolder() { init(); }

CPhysicsShellHolder::~CPhysicsShellHolder()
{
    xr_delete(m_ph_sound_player);
}

void CPhysicsShellHolder::net_Destroy()
{
    //удалить партиклы из ParticlePlayer
    CParticlesPlayer::net_DestroyParticles();
    CCharacterPhysicsSupport* char_support = character_physics_support();
    if (char_support)
        char_support->destroy_imotion();
    inherited::net_Destroy();
    b_sheduled = false;
    deactivate_physics_shell();
    xr_delete(m_pPhysicsShell);
}

/*static*/ enum EEnableState
{
    stEnable = 0,
    stDisable,
    stNotDefitnite
};
static u8 st_enable_state = (u8)stNotDefitnite;
BOOL CPhysicsShellHolder::net_Spawn(CSE_Abstract* DC)
{
    CParticlesPlayer::net_SpawnParticles();
    st_enable_state = (u8)stNotDefitnite;
    b_sheduled = true;
    BOOL ret = inherited::net_Spawn(DC); // load
                                         // create_physic_shell			();
    if (PPhysicsShell() && PPhysicsShell()->isFullActive())
    {
        PPhysicsShell()->GetGlobalTransformDynamic(&XFORM());
        switch (EEnableState(st_enable_state))
        {
        case stEnable: PPhysicsShell()->Enable(); break;
        case stDisable: PPhysicsShell()->Disable(); break;
        case stNotDefitnite:; break;
        }
        ApplySpawnIniToPhysicShell(pSettings, PPhysicsShell(), false);
        st_enable_state = (u8)stNotDefitnite;
    }
    return ret;
}

void CPhysicsShellHolder::Load(LPCSTR section)
{
    CGameObject::Load(section);
    m_collide_snd_dist = READ_IF_EXISTS(pSettings, r_fvector2, section, "collide_snd_dist", Fvector2().set(-1.f, -1.f));
}

void CPhysicsShellHolder::PHHit(SHit& H)
{
    if (H.impulse > 0)
        if (m_pPhysicsShell)
            m_pPhysicsShell->applyHit(H.p_in_bone_space, H.dir, H.impulse, H.boneID, H.type());
}

// void	CPhysicsShellHolder::Hit(float P, Fvector &dir, CObject* who, s16 element,
//						 Fvector p_in_object_space, float impulse, ALife::EHitType hit_type)
void CPhysicsShellHolder::Hit(SHit* pHDS) { PHHit(*pHDS); }

void CPhysicsShellHolder::create_physic_shell()
{
    VERIFY(!m_pPhysicsShell);
    IPhysicShellCreator* shell_creator = smart_cast<IPhysicShellCreator*>(this);
    if (shell_creator)
        shell_creator->CreatePhysicsShell();
}

void CPhysicsShellHolder::init()
{
    m_pPhysicsShell = NULL;
    b_sheduled = false;
    m_activation_speed_is_overriden = false;
    m_ph_sound_player = xr_new<CPHSoundPlayer>(this);
    m_collide_snd_dist = {-1.f, -1.f};
}
void CPhysicsShellHolder::correct_spawn_pos()
{
    VERIFY(PPhysicsShell());

    Fvector size;
    Fvector c;
    get_box(PPhysicsShell(), XFORM(), size, c);

    CPHActivationShape activation_shape;
    activation_shape.Create(c, size, this);
    activation_shape.set_rotation(XFORM());
    PPhysicsShell()->DisableCollision();
    activation_shape.Activate(size, 1, 1.f, M_PI / 8.f);
    ////	VERIFY								(valid_pos(activation_shape.Position(),phBoundaries));
    //	if (!valid_pos(activation_shape.Position(),phBoundaries)) {
    //		CPHActivationShape				activation_shape;
    //		activation_shape.Create			(c,size,this);
    //		activation_shape.set_rotation	(XFORM());
    //		activation_shape.Activate		(size,1,1.f,M_PI/8.f);
    ////		VERIFY							(valid_pos(activation_shape.Position(),phBoundaries));
    //	}

    PPhysicsShell()->EnableCollision();

    Fvector ap = activation_shape.Position();
#ifdef DEBUG
    if (!valid_pos(ap, phBoundaries))
    {
        Msg("not valid position	%f,%f,%f", ap.x, ap.y, ap.z);
        Msg("size	%f,%f,%f", size.x, size.y, size.z);
        Msg("Object: %s", Name());
        Msg("Visual: %s", *(cNameVisual()));
        Msg("Object	pos	%f,%f,%f", Position().x, Position().y, Position().z);
    }
#endif // DEBUG
    VERIFY(valid_pos(activation_shape.Position(), phBoundaries));

    Fmatrix trans;
    trans.identity();
    trans.c.sub(ap, c);
    PPhysicsShell()->TransformPosition(trans);
    PPhysicsShell()->GetGlobalTransformDynamic(&XFORM());
    activation_shape.Destroy();
}

void CPhysicsShellHolder::activate_physic_shell()
{
    VERIFY(!m_pPhysicsShell);
    create_physic_shell();
    Fvector l_fw, l_up;
    l_fw.set(XFORM().k);
    l_up.set(XFORM().j);
    l_fw.mul(2.f);
    l_up.mul(2.f);

    Fmatrix l_p1, l_p2;
    l_p1.set(XFORM());
    l_p2.set(XFORM());
    l_fw.mul(2.f);
    l_p2.c.add(l_fw);

    m_pPhysicsShell->Activate(l_p1, 0, l_p2);
    if (H_Parent() && H_Parent()->Visual())
    {
        smart_cast<IKinematics*>(H_Parent()->Visual())->CalculateBones_Invalidate();
        smart_cast<IKinematics*>(H_Parent()->Visual())->CalculateBones();
        Fvector dir = H_Parent()->Direction();
        dir.y = 0;
        l_fw.set(normalize(dir) * 2.f);
    }
    smart_cast<IKinematics*>(Visual())->CalculateBones_Invalidate();
    smart_cast<IKinematics*>(Visual())->CalculateBones();
    //	XFORM().set					(l_p1);
    correct_spawn_pos();

    Fvector overriden_vel;
    if (ActivationSpeedOverriden(overriden_vel, true))
    {
        m_pPhysicsShell->set_LinearVel(overriden_vel);
    }
    else
    {
        m_pPhysicsShell->set_LinearVel(l_fw);
    }
    m_pPhysicsShell->GetGlobalTransformDynamic(&XFORM());
}

void CPhysicsShellHolder::setup_physic_shell()
{
    VERIFY(!m_pPhysicsShell);
    create_physic_shell();
    m_pPhysicsShell->Activate(XFORM(), 0, XFORM());
    smart_cast<IKinematics*>(Visual())->CalculateBones_Invalidate();
    smart_cast<IKinematics*>(Visual())->CalculateBones();
    m_pPhysicsShell->GetGlobalTransformDynamic(&XFORM());
}

void CPhysicsShellHolder::deactivate_physics_shell()
{
    CParticlesPlayer::DestroyParticles(); //удалить партиклы из ParticlePlayer

    if (m_pPhysicsShell)
        m_pPhysicsShell->Deactivate();
    xr_delete(m_pPhysicsShell);
}
void CPhysicsShellHolder::PHSetMaterial(u16 m)
{
    if (m_pPhysicsShell)
        m_pPhysicsShell->SetMaterial(m);
}

void CPhysicsShellHolder::PHSetMaterial(LPCSTR m)
{
    if (m_pPhysicsShell)
        m_pPhysicsShell->SetMaterial(m);
}

IPhysicsShell* CPhysicsShellHolder::physics_shell() const
{
    if (m_pPhysicsShell)
        return m_pPhysicsShell;
    const CCharacterPhysicsSupport* char_support = character_physics_support();
    if (!char_support || !char_support->animation_collision())
        return 0;
    return char_support->animation_collision()->shell();
}

IPhysicsElement* CPhysicsShellHolder::physics_character() const
{
    const CCharacterPhysicsSupport* char_support = character_physics_support();
    if (!char_support)
        return 0;
    const CPHMovementControl* mov = character_physics_support()->movement();
    VERIFY(mov);
    return mov->IElement();
}

const IObjectPhysicsCollision* CPhysicsShellHolder::physics_collision()
{
    /*CCharacterPhysicsSupport* char_support = character_physics_support();
    if (char_support)
        char_support->create_animation_collision();*/

    return this;
}

void CPhysicsShellHolder::PHGetLinearVell(Fvector& velocity)
{
    if (!m_pPhysicsShell)
    {
        velocity.set(0, 0, 0);
        return;
    }
    m_pPhysicsShell->get_LinearVel(velocity);
}

void CPhysicsShellHolder::PHSetLinearVell(Fvector& velocity)
{
    if (!m_pPhysicsShell)
    {
        return;
    }
    m_pPhysicsShell->set_LinearVel(velocity);
}

f32 CPhysicsShellHolder::GetMass() { return m_pPhysicsShell ? m_pPhysicsShell->getMass() : 0; }

u16 CPhysicsShellHolder::PHGetSyncItemsNumber()
{
    if (m_pPhysicsShell)
        return m_pPhysicsShell->get_ElementsNumber();
    else
        return 0;
}

CPHSynchronize* CPhysicsShellHolder::PHGetSyncItem(u16 item)
{
    if (m_pPhysicsShell)
        return m_pPhysicsShell->get_ElementSync(item);
    else
        return 0;
}
void CPhysicsShellHolder::PHUnFreeze()
{
    if (m_pPhysicsShell)
        m_pPhysicsShell->UnFreeze();
}

void CPhysicsShellHolder::PHFreeze()
{
    if (m_pPhysicsShell)
        m_pPhysicsShell->Freeze();
}

void CPhysicsShellHolder::OnChangeVisual()
{
    inherited::OnChangeVisual();
    if (0 == renderable.visual)
    {
        CCharacterPhysicsSupport* char_support = character_physics_support();
        if (char_support)
            char_support->destroy_imotion();

        if (m_pPhysicsShell)
            m_pPhysicsShell->Deactivate();
        xr_delete(m_pPhysicsShell);
        VERIFY(0 == m_pPhysicsShell);
    }
}

void CPhysicsShellHolder::UpdateCL()
{
    inherited::UpdateCL();
    //обновить присоединенные партиклы
    UpdateParticles();
}
float CPhysicsShellHolder::EffectiveGravity() { return ph_world->Gravity(); }

void CPhysicsShellHolder::save(NET_Packet& output_packet)
{
    inherited::save(output_packet);
    u8 enable_state = (u8)stNotDefitnite;
    if (PPhysicsShell() && PPhysicsShell()->isActive())
    {
        enable_state = u8(PPhysicsShell()->isEnabled() ? stEnable : stDisable);
    }
    output_packet.w_u8(enable_state);
}

void CPhysicsShellHolder::load(IReader& input_packet)
{
    inherited::load(input_packet);
    st_enable_state = input_packet.r_u8();
}

void CPhysicsShellHolder::PHSaveState(NET_Packet& P)
{
    // Msg("!!Called [CPhysicsShellHolder::PHSaveState]");
    // CPhysicsShell* pPhysicsShell=PPhysicsShell();
    IKinematics* K = smart_cast<IKinematics*>(Visual());
    // Flags8 lflags;
    // if(pPhysicsShell&&pPhysicsShell->isActive())			lflags.set(CSE_PHSkeleton::flActive,pPhysicsShell->isEnabled());

    //	P.w_u8 (lflags.get());
    VisMask _vm;
    if (K)
    {
        _vm = K->LL_GetBonesVisible();
        P.w_u64(_vm._visimask.flags);
        P.w_u16(K->LL_GetBoneRoot());
    }
    else
    {
        P.w_u64(u64(-1));
        P.w_u16(0);
    }
    /////////////////////////////
    Fvector min, max;

    min.set(flt_max, flt_max, flt_max);
    max.set(-flt_max, -flt_max, -flt_max);
    /////////////////////////////////////

    u16 bones_number = PHGetSyncItemsNumber();
    for (u16 i = 0; i < bones_number; i++)
    {
        SPHNetState state;
        PHGetSyncItem(i)->get_State(state);
        Fvector& p = state.position;
        if (p.x < min.x)
            min.x = p.x;
        if (p.y < min.y)
            min.y = p.y;
        if (p.z < min.z)
            min.z = p.z;

        if (p.x > max.x)
            max.x = p.x;
        if (p.y > max.y)
            max.y = p.y;
        if (p.z > max.z)
            max.z = p.z;
    }

    min.sub(2.f * EPS_L);
    max.add(2.f * EPS_L);

    VERIFY(!min.similar(max));
    P.w_vec3(min);
    P.w_vec3(max);

    P.w_u16(bones_number);
    if (bones_number > 64)
    {
        Msg("!![CPhysicsShellHolder::PHSaveState] bones_number is [%u]!", bones_number);
        P.w_u64(K ? _vm._visimask_ex.flags : u64(-1));
    }

    for (u16 i = 0; i < bones_number; i++)
    {
        SPHNetState state;
        PHGetSyncItem(i)->get_State(state);
        state.net_Save(P, min, max);
    }
}

void CPhysicsShellHolder::PHLoadState(IReader& P)
{
    // Msg("!!Called [CPhysicsShellHolder::PHLoadState]");

    //	Flags8 lflags;
    IKinematics* K = smart_cast<IKinematics*>(Visual());
    //	P.r_u8 (lflags.flags);
    u64 _low = 0;
    u64 _high = 0;
    if (K)
    {
        _low = P.r_u64();
        K->LL_SetBoneRoot(P.r_u16());
    }
    else //Скорее всего K есть всегда, но на всякий случай.
    {
        P.r_u64();
        P.r_u16();
    }
    Fvector min = P.r_vec3();
    Fvector max = P.r_vec3();

    VERIFY(!min.similar(max));

    u16 bones_number = P.r_u16();
    if (bones_number > 64)
    {
        Msg("!![CPhysicsShellHolder::PHLoadState] bones_number is [%u]!", bones_number);
        _high = P.r_u64();
    }
    VisMask _vm(_low, _high);
    K->LL_SetBonesVisible(_vm);

    for (u16 i = 0; i < bones_number; i++)
    {
        SPHNetState state;
        state.net_Load(P, min, max);
        PHGetSyncItem(i)->set_State(state);
    }
}

bool CPhysicsShellHolder::register_schedule() const { return (b_sheduled); }

bool CPhysicsShellHolder::ActorCanCapture() const
{
    if (!m_pPhysicsShell || hasFixedBones() || Actor()->is_actor_climb() || Actor()->is_actor_climbing())
        return false;
    if (pSettings->line_exist("ph_capture_visuals", cNameVisual().c_str()))
        return true;
    
    std::string p{cNameVisual().c_str()};
    while (xr_string_utils::SplitFilename(p))
    {
        if (pSettings->line_exist("ph_capture_visuals", p.c_str()))
            return true;
    }
    return false;
}

bool CPhysicsShellHolder::hasFixedBones() const
{
    for (const auto it : m_pPhysicsShell->Elements())
        if (it->isFixed())
            return true;
    return false;
}

CPHCapture* CPhysicsShellHolder::PHCapture()
{
    CCharacterPhysicsSupport* ph_sup = character_physics_support();
    if (!ph_sup)
        return nullptr;
    CPHMovementControl* mov = ph_sup->movement();
    if (!mov)
        return nullptr;
    return mov->PHCapture();
}

bool CPhysicsShellHolder::IsInventoryItem() { return !!cast_inventory_item(); }

bool CPhysicsShellHolder::IsActor() { return !!cast_actor(); }

bool CPhysicsShellHolder::IsStalker() { return !!cast_stalker(); }

void CPhysicsShellHolder::MovementCollisionEnable(bool enable)
{
    VERIFY(character_physics_support());
    VERIFY(character_physics_support()->movement());
    character_physics_support()->movement()->CollisionEnable(enable);
}

bool CPhysicsShellHolder::ActivationSpeedOverriden(Fvector& dest, bool clear_override)
{
    if (m_activation_speed_is_overriden)
    {
        if (clear_override)
            m_activation_speed_is_overriden = false;

        dest = m_overriden_activation_speed;
        return true;
    }

    return false;
}

void CPhysicsShellHolder::SetActivationSpeedOverride(Fvector const& speed)
{
    m_overriden_activation_speed = speed;
    m_activation_speed_is_overriden = true;
}

Fvector2 CPhysicsShellHolder::CollideSndDist() const { return m_collide_snd_dist; }

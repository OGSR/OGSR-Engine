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
#include "iActivationShape.h"
#include "CharacterPhysicsSupport.h"
#include "Actor.h"
CPhysicsShellHolder::CPhysicsShellHolder()
{
	init();
}
CPhysicsShellHolder::	~CPhysicsShellHolder						()
{
	VERIFY ( !m_pPhysicsShell );
	destroy_physics_shell( m_pPhysicsShell );
}
const CPhysicsShellHolder*CPhysicsShellHolder::physics_collision	()
{
	CCharacterPhysicsSupport	*char_support = character_physics_support();
	if( char_support )
		char_support->create_animation_collision();
	return this;
}

void CPhysicsShellHolder::net_Destroy()
{
	//remove calls
	CPHSriptReqGObjComparer cmpr(this);
	Level().ph_commander_scripts().remove_calls(&cmpr);
	//удалить партиклы из ParticlePlayer
	CParticlesPlayer::net_DestroyParticles		();
	CCharacterPhysicsSupport	*char_support = character_physics_support();
	if( char_support )
		char_support->destroy_imotion();
	inherited::net_Destroy						();
	b_sheduled									=	false;

	deactivate_physics_shell						();
	xr_delete									(m_pPhysicsShell);
}

/*static*/ enum EEnableState
{
	stEnable			=0	,
	stDisable				,
	stNotDefitnite		
};
static u8 st_enable_state=(u8)stNotDefitnite;
BOOL CPhysicsShellHolder::net_Spawn				(CSE_Abstract*	DC)
{
	CParticlesPlayer::net_SpawnParticles		();
	st_enable_state=(u8)stNotDefitnite;
	b_sheduled									=	true;
	BOOL ret=inherited::net_Spawn				(DC);//load
		//create_physic_shell			();
	if(PPhysicsShell()&&PPhysicsShell()->isFullActive())
	{
		PPhysicsShell()->GetGlobalTransformDynamic(&XFORM());
		PPhysicsShell()->mXFORM = XFORM();
		switch (EEnableState(st_enable_state))
		{
		case stEnable		:	PPhysicsShell()->Enable()	;break;
		case stDisable		:	PPhysicsShell()->Disable()	;break;
		case stNotDefitnite	:								;break;
		}
		ApplySpawnIniToPhysicShell(pSettings,PPhysicsShell(),false);


		st_enable_state=(u8)stNotDefitnite;
	}
	return ret;
}

void	CPhysicsShellHolder::PHHit( SHit &H )
{
	if ( H.phys_impulse() >0 )
		if(m_pPhysicsShell) m_pPhysicsShell->applyHit(H.bone_space_position(),H.direction(),H.phys_impulse(),H.bone(),H.type());
}

//void	CPhysicsShellHolder::Hit(float P, Fvector &dir, CObject* who, s16 element,
//						 Fvector p_in_object_space, float impulse, ALife::EHitType hit_type)
void	CPhysicsShellHolder::Hit					(SHit* pHDS)
{
	bool const is_special_burn_hit_2_self	=	(pHDS->who == this) && (pHDS->boneID == BI_NONE) && 
												(pHDS->hit_type == ALife::eHitTypeBurn);
	if ( !is_special_burn_hit_2_self )
	{
		PHHit(*pHDS);
	}
}

void CPhysicsShellHolder::create_physic_shell	()
{
	VERIFY(!m_pPhysicsShell);
	IPhysicShellCreator *shell_creator = smart_cast<IPhysicShellCreator*>(this);
	if (shell_creator)
		shell_creator->CreatePhysicsShell();
}

void CPhysicsShellHolder::init			()
{
	m_pPhysicsShell				=	NULL		;
	b_sheduled					=	false		;
	m_activation_speed_is_overriden = false;
}
bool	 CPhysicsShellHolder::has_shell_collision_place( const CPhysicsShellHolder* obj ) const
{
	if(character_physics_support())
			return character_physics_support()->has_shell_collision_place( obj );
	return false;
}
void	CPhysicsShellHolder::on_child_shell_activate ( CPhysicsShellHolder* obj )
{
	
	if(character_physics_support())
		character_physics_support()->on_child_shell_activate ( obj );

}

void CPhysicsShellHolder::correct_spawn_pos()
{
	VERIFY								(PPhysicsShell());
	
	if( H_Parent() )
	{
		CPhysicsShellHolder	* P = smart_cast<CPhysicsShellHolder*>(H_Parent());
		if( P && P->has_shell_collision_place(this) )
			return;
	}

	Fvector								size;
	Fvector								c;
	get_box								(PPhysicsShell(),XFORM(),size,c);

	R_ASSERT_FORMAT(_valid(c),			"object: %s model: %s ", cName().c_str(), cNameVisual().c_str());
	R_ASSERT_FORMAT(_valid(size),		"object: %s model: %s ", cName().c_str(), cNameVisual().c_str());
	R_ASSERT_FORMAT(_valid(XFORM()),	"object: %s model: %s ", cName().c_str(), cNameVisual().c_str());
	PPhysicsShell()->DisableCollision	();

	Fvector								ap = Fvector().set(0,0,0);
	ActivateShapePhysShellHolder		( this, XFORM(), size, c, ap );

////	VERIFY								(valid_pos(activation_shape.Position(),phBoundaries));
//	if (!valid_pos(activation_shape.Position(),phBoundaries)) {
//		CPHActivationShape				activation_shape;
//		activation_shape.Create			(c,size,this);
//		activation_shape.set_rotation	(XFORM());
//		activation_shape.Activate		(size,1,1.f,M_PI/8.f);
////		VERIFY							(valid_pos(activation_shape.Position(),phBoundaries));
//	}
	
	PPhysicsShell()->EnableCollision	();

	


	
	Fmatrix								trans;
	trans.identity						();
	trans.c.sub							(ap,c);
	PPhysicsShell()->TransformPosition	(trans, mh_clear );
	PPhysicsShell()->GetGlobalTransformDynamic(&XFORM());

}

void CPhysicsShellHolder::activate_physic_shell()
{
	VERIFY						(!m_pPhysicsShell);
	create_physic_shell			();
	Fvector						l_fw, l_up;
	l_fw.set					(XFORM().k);
	l_up.set					(XFORM().j);
	l_fw.mul					(2.f);
	l_up.mul					(2.f);

	Fmatrix						l_p1, l_p2;
	l_p1.set					(XFORM());
	l_p2.set					(XFORM());
	l_fw.mul					(2.f);
	l_p2.c.add					(l_fw);

	m_pPhysicsShell->Activate	(l_p1, 0, l_p2);
	if(H_Parent()&&H_Parent()->Visual())
	{
		smart_cast<IKinematics*>(H_Parent()->Visual())->CalculateBones_Invalidate	();
		smart_cast<IKinematics*>(H_Parent()->Visual())->CalculateBones	(TRUE);
		Fvector dir = H_Parent()->Direction();
		if ( dir.y < 0.f )
		   dir.y = -dir.y;
		l_fw.set( normalize( dir ) * 2.f );
	}
	smart_cast<IKinematics*>(Visual())->CalculateBones_Invalidate	();
	smart_cast<IKinematics*>(Visual())->CalculateBones(TRUE);

//	XFORM().set					(l_p1);
	correct_spawn_pos();

	Fvector overriden_vel;
	if ( ActivationSpeedOverriden (overriden_vel, true) )
	{
		m_pPhysicsShell->set_LinearVel(overriden_vel);
	}
	else
	{
		m_pPhysicsShell->set_LinearVel(l_fw);
	}
	m_pPhysicsShell->GetGlobalTransformDynamic(&XFORM());

	if(H_Parent()&&H_Parent()->Visual())
	{
		smart_cast<CKinematics*>(H_Parent()->Visual())->CalculateBones_Invalidate	();
		smart_cast<CKinematics*>(H_Parent()->Visual())->CalculateBones	(TRUE);
	}
	CPhysicsShellHolder* P = smart_cast<CPhysicsShellHolder*>( H_Parent() );
	if( P )
		P->on_child_shell_activate( this );
}

void CPhysicsShellHolder::setup_physic_shell	()
{
	VERIFY						(!m_pPhysicsShell);
	create_physic_shell			();
	m_pPhysicsShell->Activate	(XFORM(),0,XFORM());
	smart_cast<IKinematics*>(Visual())->CalculateBones_Invalidate	();
	smart_cast<IKinematics*>(Visual())->CalculateBones(TRUE);
		
	ApplySpawnIniToPhysicShell(spawn_ini(),PPhysicsShell(),false);
	correct_spawn_pos();
	m_pPhysicsShell->GetGlobalTransformDynamic(&XFORM());

}

void CPhysicsShellHolder::deactivate_physics_shell()
{
	//удалить партиклы из ParticlePlayer
	CParticlesPlayer::DestroyParticles();
	destroy_physics_shell( m_pPhysicsShell );
}
void CPhysicsShellHolder::PHSetMaterial(u16 m)
{
	if(m_pPhysicsShell)
		m_pPhysicsShell->SetMaterial(m);
}

void CPhysicsShellHolder::PHSetMaterial(LPCSTR m)
{
	if(m_pPhysicsShell)
		m_pPhysicsShell->SetMaterial(m);
}

void CPhysicsShellHolder::PHGetLinearVell		(Fvector& velocity)
{
	if(!m_pPhysicsShell)
	{
		velocity.set(0,0,0);
		return;
	}
	m_pPhysicsShell->get_LinearVel(velocity);
}

void CPhysicsShellHolder::PHSetLinearVell(Fvector& velocity)
{
	if(!m_pPhysicsShell)
	{
		return;
	}
	m_pPhysicsShell->set_LinearVel(velocity);
}



f32 CPhysicsShellHolder::GetMass()
{
	return m_pPhysicsShell ? m_pPhysicsShell->getMass() : 0;
}

u16	CPhysicsShellHolder::PHGetSyncItemsNumber()
{
	if(m_pPhysicsShell)	return m_pPhysicsShell->get_ElementsNumber();
	else				return 0;
}

CPHSynchronize*	CPhysicsShellHolder::PHGetSyncItem	(u16 item)
{
	if(m_pPhysicsShell) return m_pPhysicsShell->get_ElementSync(item);
	else				return 0;
}
void	CPhysicsShellHolder::PHUnFreeze	()
{
	if(m_pPhysicsShell) m_pPhysicsShell->UnFreeze();
}


void	CPhysicsShellHolder::PHFreeze()
{
	if(m_pPhysicsShell) m_pPhysicsShell->Freeze();
}

void CPhysicsShellHolder::OnChangeVisual()
{
	inherited::OnChangeVisual();

	if (nullptr==renderable.visual) 
	{
		CCharacterPhysicsSupport	*char_support = character_physics_support();
		if( char_support )
			char_support->destroy_imotion();
		
		VERIFY( !character_physics_support() || !character_physics_support()->interactive_motion());
		if(m_pPhysicsShell)m_pPhysicsShell->Deactivate();
		xr_delete(m_pPhysicsShell);
		VERIFY(nullptr==m_pPhysicsShell);
	}
}

void CPhysicsShellHolder::UpdateCL	()
{
	inherited::UpdateCL	();
	//обновить присоединенные партиклы
	UpdateParticles		();
}
float CPhysicsShellHolder::EffectiveGravity()
{
	return ph_world->Gravity();
}

void		CPhysicsShellHolder::	save				(NET_Packet &output_packet)
{
	inherited::save(output_packet);
	u8 enable_state=(u8)stNotDefitnite;
	if(PPhysicsShell()&&PPhysicsShell()->isActive())
	{
		enable_state=u8(PPhysicsShell()->isEnabled() ? stEnable:stDisable);
	}
	output_packet.w_u8(enable_state);
}

void		CPhysicsShellHolder::	load				(IReader &input_packet)
{
	inherited::load(input_packet);
	st_enable_state=input_packet.r_u8();

}

void CPhysicsShellHolder::PHSaveState(NET_Packet &P)
{
	//Msg("!!Called [CPhysicsShellHolder::PHSaveState]");
	//CPhysicsShell* pPhysicsShell=PPhysicsShell();
	IKinematics* K	=smart_cast<IKinematics*>(Visual());
	//Flags8 lflags;
	//if(pPhysicsShell&&pPhysicsShell->isActive())			lflags.set(CSE_PHSkeleton::flActive,pPhysicsShell->isEnabled());

//	P.w_u8 (lflags.get());
	VisMask _vm;
	if(K)
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
	Fvector min,max;

	min.set(flt_max,flt_max,flt_max);
	max.set(-flt_max,-flt_max,-flt_max);
	/////////////////////////////////////

	u16 bones_number=PHGetSyncItemsNumber();
	for(u16 i=0;i<bones_number;i++)
	{
		SPHNetState state;
		PHGetSyncItem(i)->get_State(state);
		Fvector& p=state.position;
		if(p.x<min.x)min.x=p.x;
		if(p.y<min.y)min.y=p.y;
		if(p.z<min.z)min.z=p.z;

		if(p.x>max.x)max.x=p.x;
		if(p.y>max.y)max.y=p.y;
		if(p.z>max.z)max.z=p.z;
	}

	min.sub(2.f*EPS_L);
	max.add(2.f*EPS_L);

	VERIFY(!min.similar(max));
	P.w_vec3(min);
	P.w_vec3(max);

	P.w_u16(bones_number);
	if (bones_number > 64)
	{
		Msg("!![CPhysicsShellHolder::PHSaveState] bones_number is [%u]!", bones_number);
		P.w_u64(K ? _vm._visimask_ex.flags : u64(-1));
	}

	for(u16 i=0;i<bones_number;i++)
	{
		SPHNetState state;
		PHGetSyncItem(i)->get_State(state);
		state.net_Save(P,min,max);
	}
}

void CPhysicsShellHolder::PHLoadState(IReader &P)
{
	//Msg("!!Called [CPhysicsShellHolder::PHLoadState]");

//	Flags8 lflags;
	IKinematics* K=smart_cast<IKinematics*>(Visual());
//	P.r_u8 (lflags.flags);
	u64 _low = 0;
	u64 _high = 0;
	if(K)
	{
		_low = P.r_u64();
		K->LL_SetBoneRoot(P.r_u16());
	}
	else //Скорее всего K есть всегда, но на всякий случай.
	{
		P.r_u64();
		P.r_u16();
	}
	Fvector min=P.r_vec3();
	Fvector max=P.r_vec3();
	
	VERIFY(!min.similar(max));

	u16 bones_number=P.r_u16();
	if (bones_number > 64)
	{
		Msg("!![CPhysicsShellHolder::PHLoadState] bones_number is [%u]!", bones_number);
		_high = P.r_u64();
	}
	VisMask _vm(_low, _high);
	K->LL_SetBonesVisible(_vm);

	for(u16 i=0;i<bones_number;i++)
	{
		SPHNetState state;
		state.net_Load(P,min,max);
		PHGetSyncItem(i)->set_State(state);
	}
}

bool CPhysicsShellHolder::register_schedule	() const
{
	return					(b_sheduled);
}

Fmatrix& CPhysicsShellHolder::ObjectXFORM()
{
	return XFORM();
}

Fvector& CPhysicsShellHolder::ObjectPosition()
{
	return Position();
}

LPCSTR CPhysicsShellHolder::ObjectName()const
{
	return cName().c_str();
}

LPCSTR CPhysicsShellHolder::ObjectNameVisual()const
{
	return cNameVisual().c_str();
}

LPCSTR CPhysicsShellHolder::ObjectNameSect()const
{
	return cNameSect().c_str();
}

bool CPhysicsShellHolder::ObjectGetDestroy()const
{
	return !!getDestroy();
}

ICollisionHitCallback* CPhysicsShellHolder::ObjectGetCollisionHitCallback()
{
	return get_collision_hit_callback ();
}

u16	CPhysicsShellHolder::ObjectID()const
{
	return ID();
}

ICollisionForm* CPhysicsShellHolder::ObjectCollisionModel()
{
	return collidable.model;
}


CKinematics	*CPhysicsShellHolder::ObjectKinematics()
{
	VERIFY(Visual());
	return Visual()->dcast_PKinematics();
}

#include <filesystem>

bool CPhysicsShellHolder::ActorCanCapture() const {
  if ( !m_pPhysicsShell || hasFixedBones() || Actor()->is_actor_climb() || Actor()->is_actor_climbing()) return false;
  if ( pSettings->line_exist( "ph_capture_visuals", cNameVisual().c_str() ) )
    return true;
  std::filesystem::path p = cNameVisual().c_str();
  while ( p.has_parent_path() ) {
    p = p.parent_path();
    if ( pSettings->line_exist( "ph_capture_visuals", p.string().c_str() ) )
      return true;
  }
  return false;
}

bool CPhysicsShellHolder::hasFixedBones() const {
  for ( const auto it : m_pPhysicsShell->Elements() )
    if ( it->isFixed() ) return true;
  return false;
}

CPhysicsShell*& CPhysicsShellHolder::ObjectPPhysicsShell()
{
	return PPhysicsShell();
}

bool CPhysicsShellHolder::has_parent_object()
{
	return !!H_Parent();
}

CPHCapture*	CPhysicsShellHolder::PHCapture()
{
	CCharacterPhysicsSupport* ph_sup = character_physics_support();
	if( !ph_sup )
		return nullptr;
	CPHMovementControl	*mov = ph_sup->movement();
	if( !mov )
		return nullptr;
	return mov->PHCapture();
}

bool CPhysicsShellHolder::ActivationSpeedOverriden(Fvector& dest, bool clear_override)
{
	if (m_activation_speed_is_overriden)
	{
		if (clear_override)
			m_activation_speed_is_overriden	= false;

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

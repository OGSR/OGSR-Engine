#include "stdafx.h"
#include "alife_space.h"
#include "hit.h"
#include "PHDestroyable.h"
#include "CharacterPhysicsSupport.h"
#include "PHMovementControl.h"
#include "CustomMonster.h"
#include "PhysicsShell.h"
#include "../skeletonanimated.h"
#include "Actor.h"
#include "CustomZone.h"
#include "Extendedgeom.h"
#include "Physics.h"
#include "level.h"
#include "PHActivationShape.h"
#include "IKLimbsController.h"
#include "PHCapture.h"
#include "PHCollideValidator.h"
#include "ai/stalker/ai_stalker.h"
#include "interactive_motion.h"
#include "animation_movement_controller.h"
//const float default_hinge_friction = 5.f;//gray_wolf comment
#ifdef DEBUG
#	include "PHDebug.h"
#endif // DEBUG

#include "../device.h"

#ifdef PRIQUEL
#	define USE_SMART_HITS
#	define USE_IK
#endif // PRIQUEL

void  NodynamicsCollide(bool& do_colide,bool bo1,dContact& c,SGameMtl * /*material_1*/,SGameMtl * /*material_2*/)
{
	dBodyID body1=dGeomGetBody(c.geom.g1);
	dBodyID body2=dGeomGetBody(c.geom.g2);
	if(!body1||!body2||(dGeomUserDataHasCallback(c.geom.g1,NodynamicsCollide)&&dGeomUserDataHasCallback(c.geom.g2,NodynamicsCollide)))return;
	do_colide=false; 
}

void  OnCharacterContactInDeath(bool& do_colide,bool bo1,dContact& c,SGameMtl * /*material_1*/,SGameMtl * /*material_2*/)
{
	dSurfaceParameters		&surface=c.surface;
	CCharacterPhysicsSupport* l_character_physic_support=0;
	if (bo1)
	{
		l_character_physic_support=(CCharacterPhysicsSupport*)retrieveGeomUserData(c.geom.g1)->callback_data;
	}
	else
	{
		l_character_physic_support=(CCharacterPhysicsSupport*)retrieveGeomUserData(c.geom.g2)->callback_data;
	}

	surface.mu=l_character_physic_support->m_curr_skin_friction_in_death;
}

IC bool is_imotion(interactive_motion *im)
{
	return im && im->is_enabled();
}

CCharacterPhysicsSupport::~CCharacterPhysicsSupport()
{
	if(m_flags.test(fl_skeleton_in_shell))
	{
		if(m_physics_skeleton)m_physics_skeleton->Deactivate();
		xr_delete(m_physics_skeleton);//!b_skeleton_in_shell
	}
	xr_delete(m_PhysicMovementControl);
	VERIFY(!m_interactive_motion);
}

CCharacterPhysicsSupport::CCharacterPhysicsSupport(EType atype,CEntityAlive* aentity) 
:	  m_pPhysicsShell(aentity->PPhysicsShell()),
	  m_EntityAlife(*aentity),
	  mXFORM(aentity->XFORM()),
	  m_ph_sound_player(aentity),
	  m_interactive_motion(0)
{
	m_PhysicMovementControl=xr_new<CPHMovementControl>(aentity);
	m_flags.assign(0);
	m_eType=atype;
	m_eState=esAlive;
	//b_death_anim_on					= false;
	m_flags.set(fl_death_anim_on,FALSE);
	m_pPhysicsShell					=	NULL;
	//m_saved_impulse					= 0.f;
	m_physics_skeleton				=	NULL;
	//b_skeleton_in_shell				= false;
	m_flags.set(fl_skeleton_in_shell,FALSE);
	m_shot_up_factor				=0.f;
	m_after_death_velocity_factor	=1.f;
	m_ik_controller					=	NULL;
	m_BonceDamageFactor				=1.f;
	m_collision_hit_callback		=	NULL;
	m_Pred_Time						= 0.0;
	m_was_wounded					= false;
	switch(atype)
	{
	case etActor:
		m_PhysicMovementControl->AllocateCharacterObject(CPHMovementControl::actor);
		m_PhysicMovementControl->SetRestrictionType(CPHCharacter::rtActor);
		break;
	case etStalker:
		m_PhysicMovementControl->AllocateCharacterObject(CPHMovementControl::ai);
		m_PhysicMovementControl->SetRestrictionType(CPHCharacter::rtStalker);
		m_PhysicMovementControl->SetActorMovable(false);
		break;
	case etBitting:
		m_PhysicMovementControl->AllocateCharacterObject(CPHMovementControl::ai);
	}
};

void CCharacterPhysicsSupport::SetRemoved()
{
	m_eState=esRemoved;
	if(m_flags.test(fl_skeleton_in_shell))//b_skeleton_in_shell
	{
		if(m_pPhysicsShell->isEnabled())
		{
			m_EntityAlife.processing_deactivate();
		}
		if(m_pPhysicsShell)m_pPhysicsShell->Deactivate();
		xr_delete(m_pPhysicsShell);
	}
	else
	{
		if(m_physics_skeleton)m_physics_skeleton->Deactivate();
		xr_delete(m_physics_skeleton);
		m_EntityAlife.processing_deactivate();
	}
	
}

void CCharacterPhysicsSupport::in_Load(LPCSTR section)
{

	skel_airr_ang_factor			= pSettings->r_float(section,"ph_skeleton_airr_ang_factor");
	skel_airr_lin_factor			= pSettings->r_float(section,"ph_skeleton_airr_lin_factor");
	hinge_force_factor1				= pSettings->r_float(section,"ph_skeleton_hinger_factor1");
	skel_ddelay						= pSettings->r_float(section,"ph_skeleton_ddelay");
	skel_remain_time				= skel_ddelay;
	skel_fatal_impulse_factor		= pSettings->r_float(section,"ph_skel_fatal_impulse_factor");
	//gray_wolf>„итаем из ltx параметры дл€ поддержки измен€ющегос€ трени€ у персонажей во врем€ смерти
	skeleton_skin_ddelay			= pSettings->r_float(section,"ph_skeleton_skin_ddelay");
	skeleton_skin_remain_time		= skeleton_skin_ddelay;
	skeleton_skin_friction_start	= pSettings->r_float(section,"ph_skeleton_skin_friction_start");
	skeleton_skin_friction_end		= pSettings->r_float(section,"ph_skeleton_skin_friction_end");
	character_have_wounded_state	= pSettings->r_bool(section,"ph_character_have_wounded_state");
	skeleton_skin_ddelay_after_wound= pSettings->r_float(section,"ph_skeleton_skin_ddelay_after_wound");
	skeleton_skin_remain_time_after_wound= skeleton_skin_ddelay_after_wound;
	pelvis_factor_low_pose_detect= pSettings->r_float(section,"ph_pelvis_factor_low_pose_detect");
	
	//gray_wolf<
	if(pSettings->line_exist(section,"ph_skel_shot_up_factor")) m_shot_up_factor=pSettings->r_float(section,"ph_skel_shot_up_factor");
	if(pSettings->line_exist(section,"ph_after_death_velocity_factor")) m_after_death_velocity_factor=pSettings->r_float(section,"ph_after_death_velocity_factor");
	m_flags.set(fl_specific_bonce_demager,TRUE);
	if(pSettings->line_exist(section,"bonce_damage_factor"))
	{
		
		m_BonceDamageFactor=pSettings->r_float(section,"bonce_damage_factor_for_objects");
	}else
	{
		m_BonceDamageFactor=pSettings->r_float("collision_damage","bonce_damage_factor_for_objects");
	}
	CPHDestroyable::Load(section);
}

void CCharacterPhysicsSupport::in_NetSpawn(CSE_Abstract* e)
{
	
	if(m_EntityAlife.use_simplified_visual	())
	{
		m_flags.set(fl_death_anim_on,TRUE);
		CKinematics*	ka=smart_cast<CKinematics*>(m_EntityAlife.Visual());
		VERIFY(ka);
		ka->CalculateBones_Invalidate();
		ka->CalculateBones();
		CollisionCorrectObjPos(m_EntityAlife.Position());
		m_pPhysicsShell		= P_build_Shell(&m_EntityAlife,false);
		ka->CalculateBones_Invalidate();
		ka->CalculateBones();
		return;
	}

	CPHDestroyable::Init();//this zerows colbacks !!;
	CKinematicsAnimated*ka= smart_cast<CKinematicsAnimated*>(m_EntityAlife.Visual());
	if(!m_EntityAlife.g_Alive())
	{
		
		if(m_eType==etStalker)
			ka->PlayCycle("waunded_1_idle_0");
		else
			ka->PlayCycle("death_init");

	}else if( !m_EntityAlife.animation_movement_controlled( ) )
	{
	
		ka->PlayCycle( "death_init" );///непон€тно зачем это вообще надо запускать
									  ///этот хак нужен, потому что некоторым монстрам 
									  ///анимаци€ после спона, может быть вообще не назначена
	}
	ka->CalculateBones_Invalidate();
	ka->CalculateBones();
	
	CPHSkeleton::Spawn(e);
	movement()->EnableCharacter();
	movement()->SetPosition(m_EntityAlife.Position());
	movement()->SetVelocity	(0,0,0);
	if(m_eType!=etActor)
	{
		m_flags.set(fl_specific_bonce_demager,TRUE);
		m_BonceDamageFactor=1.f;
	}
	if(Type() == etStalker)
	{
		m_hit_animations.SetupHitMotions(*smart_cast<CKinematicsAnimated*>(m_EntityAlife.Visual()));
	}
	anim_mov_state.init();

	anim_mov_state.active = m_EntityAlife.animation_movement_controlled();
}


void CCharacterPhysicsSupport::CreateCharacter( )
{
	//if( m_eType == etBitting )
		//return;
	if( m_PhysicMovementControl->CharacterExist( ) )return;
	CollisionCorrectObjPos( m_EntityAlife.Position( ), true );
	m_PhysicMovementControl->CreateCharacter( );
	m_PhysicMovementControl->SetPhysicsRefObject( &m_EntityAlife );
	m_PhysicMovementControl->SetPosition	( m_EntityAlife.Position( ) );
}
void CCharacterPhysicsSupport::SpawnInitPhysics(CSE_Abstract* e)
{
	//if(!m_physics_skeleton)CreateSkeleton(m_physics_skeleton);

	if(m_EntityAlife.g_Alive())
	{
#ifdef DEBUG
		if(ph_dbg_draw_mask1.test(ph_m1_DbgTrackObject)&&stricmp(PH_DBG_ObjectTrack(),*m_EntityAlife.cName())==0)
		{
			Msg("CCharacterPhysicsSupport::SpawnInitPhysics obj %s before collision correction %f,%f,%f",PH_DBG_ObjectTrack(),m_EntityAlife.Position().x,m_EntityAlife.Position().y,m_EntityAlife.Position().z);
		}
#endif
#ifdef	USE_IK
		if( etStalker == m_eType || etActor == m_eType )
				CreateIKController( );
#endif
		if( !m_EntityAlife.animation_movement_controlled( ) )
			CreateCharacter( );
#ifdef DEBUG  
		if( ph_dbg_draw_mask1.test( ph_m1_DbgTrackObject ) && stricmp( PH_DBG_ObjectTrack( ), *m_EntityAlife.cName()) == 0 )
		{
			Msg( "CCharacterPhysicsSupport::SpawnInitPhysics obj %s after collision correction %f,%f,%f", PH_DBG_ObjectTrack(),m_EntityAlife.Position( ).x, m_EntityAlife.Position().y, m_EntityAlife.Position().z );
		}
#endif
		//m_PhysicMovementControl.SetMaterial( )
	}
	else
	{
		ActivateShell( NULL );
	}
}
void CCharacterPhysicsSupport::in_NetDestroy( )
{
	m_PhysicMovementControl->DestroyCharacter( );

	if( m_physics_skeleton )
	{
		m_physics_skeleton->Deactivate( );
		xr_delete( m_physics_skeleton );
	}
	if(m_pPhysicsShell)
	{
		m_pPhysicsShell->Deactivate();
		xr_delete(m_pPhysicsShell);
	}

	m_flags.set( fl_skeleton_in_shell, FALSE );
	CPHSkeleton::RespawnInit( );
	CPHDestroyable::RespawnInit( );
	m_eState = esAlive;
	xr_delete( m_interactive_motion );
	DestroyIKController( );
}

void	CCharacterPhysicsSupport::in_NetSave( NET_Packet& P )
{
	
	CPHSkeleton::SaveNetState( P );
}

void CCharacterPhysicsSupport::in_Init( )
{
	
	//b_death_anim_on					= false;
	//m_pPhysicsShell					= NULL;
	//m_saved_impulse					= 0.f;
}

void CCharacterPhysicsSupport::in_shedule_Update( u32 DT )
{
	//CPHSkeleton::Update(DT);
	if( !m_EntityAlife.use_simplified_visual	( ) )
		CPHDestroyable::SheduleUpdate( DT );
	else	if( m_pPhysicsShell&&m_pPhysicsShell->isFullActive( ) && !m_pPhysicsShell->isEnabled( ) )
	{
		m_EntityAlife.deactivate_physics_shell( );
	}
	movement( )->in_shedule_Update( DT );

#if	0
	if( anim_mov_state.active )
	{
		DBG_OpenCashedDraw( );
		DBG_DrawMatrix( mXFORM, 0.5f );
		DBG_ClosedCashedDraw( 5000 );
	}
#endif

}

#ifdef DEBUG
	string64 sdbg_stalker_death_anim = "none";
	LPSTR dbg_stalker_death_anim = sdbg_stalker_death_anim;
	BOOL  b_death_anim_velocity = TRUE;
#endif
const float cmp_angle = M_PI/10.f;
const float cmp_ldisp = 0.1f;
IC bool cmp(const Fmatrix &f0, const Fmatrix &f1 )
{
	Fmatrix if0;if0.invert(f0);
	Fmatrix cm;cm.mul_43(if0,f1);
	
	Fvector ax;float ang;
	Fquaternion q;
	q.set(cm);
	q.get_axis_angle(ax,ang);

	return ang < cmp_angle && cm.c.square_magnitude() < cmp_ldisp* cmp_ldisp ;
}

bool is_similar(const Fmatrix &m0,const Fmatrix &m1,float param)
{
	Fmatrix tmp1;tmp1.invert(m0);
	Fmatrix tmp2;tmp2.mul(tmp1,m1);
	Fvector ax;float ang;
	Fquaternion q;
	q.set(tmp2);
	q.get_axis_angle(ax,ang);
	return _abs(ang)<M_PI/2.f;
	/*
	return  fsimilar(tmp2._11,1.f,param)&&
			fsimilar(tmp2._22,1.f,param)&&
			fsimilar(tmp2._33,1.f,param)&&
			fsimilar(tmp2._41,0.f,param)&&
			fsimilar(tmp2._42,0.f,param)&&
			fsimilar(tmp2._43,0.f,param);
			*/
	/*
			fsimilar(tmp2._12,0.f,param)&&
			fsimilar(tmp2._13,0.f,param)&&
			fsimilar(tmp2._21,0.f,param)&&
			fsimilar(tmp2._23,0.f,param)&&
			fsimilar(tmp2._31,0.f,param)&&
			fsimilar(tmp2._32,0.f,param)&&
			fsimilar(tmp2._41,0.f,param)&&
			fsimilar(tmp2._42,0.f,param)&&
			fsimilar(tmp2._43,0.f,param);
			*/
}

void CCharacterPhysicsSupport::KillHit(CObject *who, ALife::EHitType hit_type, float &impulse)
{
	TestForWounded();
	Fmatrix prev_pose;prev_pose.set(mXFORM);
	ActivateShell(who);
#ifdef DEBUG
	if(Type() == etStalker && xr_strcmp(dbg_stalker_death_anim, "none") != 0)
	{	
		if(cmp(prev_pose,mXFORM))
		{
			xr_delete(m_interactive_motion);
			if(b_death_anim_velocity)
				m_interactive_motion = xr_new<imotion_velocity>();
			else
				m_interactive_motion = xr_new<imotion_position>();
			m_interactive_motion->setup(dbg_stalker_death_anim, m_pPhysicsShell);
		}
	}
#endif

	if(is_imotion(m_interactive_motion))
				m_interactive_motion->play(m_pPhysicsShell);

	if (!m_was_wounded)
	{
		impulse*=(hit_type==ALife::eHitTypeExplosion ? 1.f : skel_fatal_impulse_factor);
	}
	if(!is_imotion(m_interactive_motion))
		m_flags.set(fl_block_hit,TRUE);
}

void CCharacterPhysicsSupport::in_Hit(float P,Fvector &dir, CObject *who,s16 element,Fvector p_in_object_space, float impulse,ALife::EHitType hit_type ,bool is_killing)
{
	if(m_EntityAlife.use_simplified_visual	())	return;
	if(m_flags.test(fl_block_hit))
	{
		VERIFY(!m_EntityAlife.g_Alive());
		if(Device.dwTimeGlobal-m_EntityAlife.GetLevelDeathTime()>=2000)
			m_flags.set(fl_block_hit,FALSE);
		else return;
	}

	is_killing=is_killing||(m_eState==esAlive&&!m_EntityAlife.g_Alive());
	if(m_EntityAlife.g_Alive()&&is_killing&&hit_type==ALife::eHitTypeExplosion&&P>70.f)
		CPHDestroyable::Destroy();

	if((!m_EntityAlife.g_Alive()||is_killing)&&!fis_zero(m_shot_up_factor)&&hit_type!=ALife::eHitTypeExplosion)
	{
		dir.y+=m_shot_up_factor;
		dir.normalize();
	}

	if(!m_pPhysicsShell&&is_killing)
	{
		KillHit(who, hit_type, impulse);
	}

	if(!(m_pPhysicsShell&&m_pPhysicsShell->isActive()))
	{
		if(!is_killing&&m_EntityAlife.g_Alive())
			m_PhysicMovementControl->ApplyHit(dir,impulse,hit_type);

#ifdef USE_SMART_HITS
		if(Type()==etStalker)
		{
				m_hit_animations.PlayHitMotion(dir,p_in_object_space,element,m_EntityAlife);
		}
#endif // USE_SMART_HITS

	}else 
		m_pPhysicsShell->applyHit(p_in_object_space,dir,impulse,element,hit_type);
}


IC		void	CCharacterPhysicsSupport::						UpdateDeathAnims				()
{
	VERIFY(m_pPhysicsShell->isFullActive());

	if(!m_flags.test(fl_death_anim_on) && !is_imotion(m_interactive_motion))//!m_flags.test(fl_use_death_motion)//!b_death_anim_on&&m_pPhysicsShell->isFullActive()
	{
		smart_cast<CKinematicsAnimated*>(m_EntityAlife.Visual())->PlayCycle("death_init");
		m_flags.set(fl_death_anim_on,TRUE);
	}
}


void CCharacterPhysicsSupport::in_UpdateCL( )
{
	if( m_eState==esRemoved )
	{
		return;
	}
	CalculateTimeDelta( );
	if( m_pPhysicsShell )
	{
		VERIFY( m_pPhysicsShell->isFullActive( ) );
		m_pPhysicsShell->SetRagDoll( );//“еперь шела относитьс€ к классу объектов cbClassRagDoll
		
		if( !is_imotion(m_interactive_motion ) )//!m_flags.test(fl_use_death_motion)
			m_pPhysicsShell->InterpolateGlobalTransform( &mXFORM );
		else
			m_interactive_motion->update( m_pPhysicsShell );
	
		UpdateDeathAnims();
		UpdateFrictionAndJointResistanse( );
	} else if ( !m_EntityAlife.g_Alive( ) && !m_EntityAlife.use_simplified_visual( ) )
	{
		ActivateShell( NULL );
		m_PhysicMovementControl->DestroyCharacter( );
	} else if( ik_controller( ) )
		ik_controller( )->Update();


#ifdef DEBUG
	if(Type()==etStalker && ph_dbg_draw_mask1.test(phDbgHitAnims))
	{
		Fmatrix m;
		m_hit_animations.GetBaseMatrix(m,m_EntityAlife);
		DBG_DrawMatrix(m,1.5f);
/*
		CKinematicsAnimated	*K = smart_cast<CKinematicsAnimated*>(m_EntityAlife.Visual());
		u16 hb = K->LL_BoneID("bip01_head");
		u16 pb = K->LL_GetBoneRoot();
		u16 nb = K->LL_BoneID("bip01_neck");
		u16 eb = K->LL_BoneID("eye_right");
		Fmatrix &mh  = K->LL_GetTransform(hb);
		Fmatrix &mp  = K->LL_GetTransform(pb);
		Fmatrix &me	 = K->LL_GetTransform(eb);
		Fmatrix &mn	 = K->LL_GetTransform(nb);
		float d = DET(mh);
		if(Fvector().sub(mh.c,mp.c).magnitude() < 0.3f||d<0.7 )//|| Fvector().sub(me.c,mn.c) < 0.5
		{
			
			K->CalculateBones_Invalidate();
			K->CalculateBones();
			;
		}
*/
	}
#endif
}

void CCharacterPhysicsSupport::CreateSkeleton(CPhysicsShell* &pShell)
{

	R_ASSERT2(!pShell,"pShell already initialized!!");
	if (!m_EntityAlife.Visual())
		return;
#ifdef DEBUG
	CTimer t;t.Start();
#endif	
	pShell		= P_create_Shell();
	pShell->preBuild_FromKinematics(smart_cast<CKinematics*>(m_EntityAlife.Visual()));
	pShell->mXFORM.set(mXFORM);
	pShell->SetAirResistance(skel_airr_lin_factor,skel_airr_ang_factor);
	pShell->SmoothElementsInertia(0.3f);
	pShell->set_PhysicsRefObject(&m_EntityAlife);
	SAllDDOParams disable_params;
	disable_params.Load(smart_cast<CKinematics*>(m_EntityAlife.Visual())->LL_UserData());
	pShell->set_DisableParams(disable_params);

	pShell->Build();
#ifdef DEBUG	
	Msg("shell for %s[%d] created in %f ms",*m_EntityAlife.cName(),m_EntityAlife.ID(),t.GetElapsed_sec()*1000.f);
#endif
}
void CCharacterPhysicsSupport::CreateSkeleton()
{
	if(m_pPhysicsShell) return;
	Fvector velocity;
	m_PhysicMovementControl->GetCharacterVelocity(velocity);
	m_PhysicMovementControl->GetDeathPosition	(m_EntityAlife.Position());
	m_PhysicMovementControl->DestroyCharacter();
	if (!m_EntityAlife.Visual())
		return;
	m_pPhysicsShell		= P_create_Shell();
	m_pPhysicsShell->build_FromKinematics(smart_cast<CKinematics*>(m_EntityAlife.Visual()));
	m_pPhysicsShell->mXFORM.set(mXFORM);
	m_pPhysicsShell->SetAirResistance(skel_airr_lin_factor,
		skel_airr_ang_factor);
	m_pPhysicsShell->set_PhysicsRefObject(&m_EntityAlife);
	SAllDDOParams disable_params;
	disable_params.Load(smart_cast<CKinematics*>(m_EntityAlife.Visual())->LL_UserData());
	m_pPhysicsShell->set_DisableParams(disable_params);
	m_pPhysicsShell->set_JointResistance(0.f);
	m_pPhysicsShell->Activate(true);
	velocity.mul(1.25f*m_after_death_velocity_factor);
	m_pPhysicsShell->set_LinearVel(velocity);
	smart_cast<CKinematics*>(m_EntityAlife.Visual())->CalculateBones();
	//b_death_anim_on=false;
	m_flags.set(fl_death_anim_on,FALSE);
	m_eState=esDead;
}
bool CCharacterPhysicsSupport::DoCharacterShellCollide()
{
	if(m_eType==etStalker)
	{
		CAI_Stalker*	OBJ=smart_cast<CAI_Stalker*>(&m_EntityAlife);
		VERIFY			(OBJ);
		return			!OBJ->wounded();
	}
	return true;
}
void CCharacterPhysicsSupport::CollisionCorrectObjPos(const Fvector& start_from,bool	character_create/*=false*/)
{
	Fvector shift;shift.sub(start_from,m_EntityAlife.Position());

	Fbox box;
	if(character_create)box.set(movement()->Box());
	else	box.set(m_EntityAlife.BoundingBox());
	Fvector vbox;Fvector activation_pos;
	box.get_CD(activation_pos,vbox);shift.add(activation_pos);vbox.mul(2.f);
	activation_pos.add(shift,m_EntityAlife.Position());
	CPHActivationShape activation_shape;
	activation_shape.Create(activation_pos,vbox,&m_EntityAlife);
	if(!DoCharacterShellCollide()&&!character_create)
	{
		CPHCollideValidator::SetCharacterClassNotCollide(activation_shape);
	}
	activation_shape.Activate(vbox,1,1.f,M_PI/8.f);
	m_EntityAlife.Position().sub(activation_shape.Position(),shift);
	activation_shape.Destroy();
}

void CCharacterPhysicsSupport::set_movement_position( const Fvector &pos )
{
	VERIFY( movement() );

	CollisionCorrectObjPos( pos, true );
	
	movement()->SetPosition( m_EntityAlife.Position() );
}

void CCharacterPhysicsSupport::ActivateShell			( CObject* who )
{
	DestroyIKController( );
	CKinematics* K=smart_cast<CKinematics*>( m_EntityAlife.Visual( ) );

	//animation movement controller issues
	bool	anim_mov_ctrl =m_EntityAlife.animation_movement_controlled( );
	CBoneInstance	&BR = K->LL_GetBoneInstance( K->LL_GetBoneRoot( ) );
	Fmatrix start_xform; start_xform.identity( );
	CBlend *anim_mov_blend = 0;
	//float	blend_time = 0;
	if( anim_mov_ctrl )
	{
		m_EntityAlife.animation_movement( )->ObjStartXform( start_xform );
		anim_mov_blend = m_EntityAlife.animation_movement( )->ControlBlend( );
		/*
		VERIFY( anim_mov_blend->blend != CBlend::eFREE_SLOT );
		anim_mov_blend->timeCurrent -= 2 * Device.fTimeDelta * anim_mov_blend->speed;
		blend_time = anim_mov_blend->timeCurrent;
		anim_mov_blend->playing = true;

		K->CalculateBones_Invalidate( );
		K->CalculateBones	();
		anim_mov_blend->playing = false;
		*/
		m_EntityAlife.destroy_anim_mov_ctrl( );
		BR.Callback_overwrite = TRUE;
	}
	//

	if( !m_physics_skeleton ) 
		CreateSkeleton( m_physics_skeleton );

	if( m_eType == etActor )
	{
		CActor* A=smart_cast<CActor*>( &m_EntityAlife );
		R_ASSERT2( A, "not an actor has actor type" );
		if( A->Holder( ) ) return;
		if( m_eState==esRemoved )return;
	}
	
//////////////////////this needs to evaluate object box//////////////////////////////////////////////////////
	for( u16 I = K->LL_BoneCount( )-1; I!=u16(-1); --I )
				K->LL_GetBoneInstance( I ).reset_callback( );

	if( anim_mov_ctrl )	//we do not whant to move by long animation in root 
			BR.Callback_overwrite = TRUE;

	K->CalculateBones_Invalidate();
	K->CalculateBones	();
////////////////////////////////////////////////////////////////////////////
	if( m_pPhysicsShell ) return;
	Fvector velocity;
	m_PhysicMovementControl->GetCharacterVelocity		( velocity );
	velocity.mul( 1.3f );
	Fvector dp, start;start.set( m_EntityAlife.Position( ) );
	if( !m_PhysicMovementControl->CharacterExist( ) )
		dp.set( m_EntityAlife.Position( ) );
	else m_PhysicMovementControl->GetDeathPosition( dp );
	m_PhysicMovementControl->DestroyCharacter( );

	CollisionCorrectObjPos( dp );

	//shell create
	R_ASSERT2(m_physics_skeleton,"No skeleton created!!");
	m_pPhysicsShell=m_physics_skeleton;
	m_physics_skeleton=NULL;
	m_pPhysicsShell->set_Kinematics(K);
	m_pPhysicsShell->RunSimulation();
	m_pPhysicsShell->mXFORM.set(mXFORM);
	m_pPhysicsShell->SetCallbacks(m_pPhysicsShell->GetBonesCallback());
	//

	if(anim_mov_ctrl) //we do not whant to move by long animation in root 
			BR.Callback_overwrite = TRUE;

	//set shell params
	if(!smart_cast<CCustomZone*>(who))
	{
		velocity.mul(1.25f*m_after_death_velocity_factor);
	}
	if(!DoCharacterShellCollide())
	{
		m_pPhysicsShell->DisableCharacterCollision();
	}
	m_pPhysicsShell->set_LinearVel(velocity);
	K->CalculateBones_Invalidate();
	K->CalculateBones	();
	m_flags.set(fl_death_anim_on,FALSE);
	m_eState=esDead;
	m_flags.set(fl_skeleton_in_shell,TRUE);
	
// KD:  оллизи€ с трупами
/*	if(IsGameTypeSingle())
	{
		m_pPhysicsShell->SetPrefereExactIntegration	();//use exact integration for ragdolls in single
		m_pPhysicsShell->SetRemoveCharacterCollLADisable();
	}
	else
	{
		m_pPhysicsShell->SetIgnoreDynamic();
	}*/
	m_pPhysicsShell->SetIgnoreSmall();
	//end seting params



	//fly back after correction
	FlyTo(Fvector().sub(start,m_EntityAlife.Position()));
	//

	//actualize
	m_pPhysicsShell->GetGlobalTransformDynamic(&mXFORM);
	//

	m_pPhysicsShell->add_ObjectContactCallback(OnCharacterContactInDeath);
	m_pPhysicsShell->set_CallbackData((void*)this);
//

	if(anim_mov_ctrl && anim_mov_blend && anim_mov_blend->blend != CBlend::eFREE_SLOT &&  anim_mov_blend->timeCurrent + Device.fTimeDelta*anim_mov_blend->speed < anim_mov_blend->timeTotal-SAMPLE_SPF-EPS)//.
	{
		const Fmatrix sv_xform = mXFORM;
		mXFORM.set( start_xform );
		//anim_mov_blend->blendPower = 1;
		anim_mov_blend->timeCurrent  += Device.fTimeDelta * anim_mov_blend->speed;
		m_pPhysicsShell->AnimToVelocityState( Device.fTimeDelta, 2 * default_l_limit, 10.f * default_w_limit );
		mXFORM.set( sv_xform );
	}

}
void CCharacterPhysicsSupport::in_ChangeVisual()
{
	
	if(!m_physics_skeleton&&!m_pPhysicsShell) return;

	if(m_pPhysicsShell)
	{
		VERIFY(m_eType!=etStalker);
		if(m_physics_skeleton)
		{
			m_EntityAlife.processing_deactivate()	;
			m_physics_skeleton->Deactivate()		;
			xr_delete(m_physics_skeleton)			;
		}
		CreateSkeleton(m_physics_skeleton);
		if(m_pPhysicsShell)m_pPhysicsShell->Deactivate();
		xr_delete(m_pPhysicsShell);
		ActivateShell(NULL);
	}
	if(m_ik_controller)
	{
		DestroyIKController();
		CreateIKController();
	}
}

bool CCharacterPhysicsSupport::CanRemoveObject()
{
	if(m_eType==etActor)
	{
		return false;
	}
	else
	{
		return !m_EntityAlife.IsPlaying();
	}
}

void CCharacterPhysicsSupport::PHGetLinearVell(Fvector &velocity)
{
	if(m_pPhysicsShell&&m_pPhysicsShell->isActive())
	{
		m_pPhysicsShell->get_LinearVel(velocity);
	}
	else
		movement()->GetCharacterVelocity(velocity);
		
}

void CCharacterPhysicsSupport::CreateIKController()
{

	VERIFY(!m_ik_controller);
	m_ik_controller=xr_new<CIKLimbsController>();
	m_ik_controller->Create(&m_EntityAlife);
	
}
void CCharacterPhysicsSupport::DestroyIKController()
{
	if(!m_ik_controller)return;
	m_ik_controller->Destroy(&m_EntityAlife);
	xr_delete(m_ik_controller);
}

void		 CCharacterPhysicsSupport::in_NetRelcase(CObject* O)																													
{
	CPHCapture* c=m_PhysicMovementControl->PHCapture();
	if(c)
	{
		c->net_Relcase(O);
	}
}
 
bool CCharacterPhysicsSupport::set_collision_hit_callback(SCollisionHitCallback* cc)
{
	if(!cc)
	{
		m_collision_hit_callback=NULL;
		return true;
	}
	if(m_pPhysicsShell)
	{
		VERIFY2(cc->m_collision_hit_callback!=0,"No callback function");
		m_collision_hit_callback=cc;
		return true;
	}else return false;
}
SCollisionHitCallback * CCharacterPhysicsSupport::get_collision_hit_callback()
{
	return m_collision_hit_callback;
}

void	StaticEnvironmentCB (bool& do_colide,bool bo1,dContact& c,SGameMtl* material_1,SGameMtl* material_2)
{
	dJointID contact_joint	= dJointCreateContact(0, ContactGroup, &c);

	if(bo1)
	{
		((CPHIsland*)(retrieveGeomUserData(c.geom.g1)->callback_data))->DActiveIsland()->ConnectJoint(contact_joint);
		dJointAttach			(contact_joint, dGeomGetBody(c.geom.g1), 0);
	}
	else
	{
		((CPHIsland*)(retrieveGeomUserData(c.geom.g2)->callback_data))->DActiveIsland()->ConnectJoint(contact_joint);
		dJointAttach			(contact_joint, 0, dGeomGetBody(c.geom.g2));
	}
	do_colide=false;
}

void						CCharacterPhysicsSupport::FlyTo(const	Fvector &disp)
{
		VERIFY(m_pPhysicsShell);
		float ammount=disp.magnitude();
		if(fis_zero(ammount,EPS_L))	return;
		ph_world->Freeze();
		bool g=m_pPhysicsShell->get_ApplyByGravity();
		m_pPhysicsShell->set_ApplyByGravity(false);
		m_pPhysicsShell->add_ObjectContactCallback(StaticEnvironmentCB);
		void*	cd=m_pPhysicsShell->get_CallbackData();
		m_pPhysicsShell->set_CallbackData(m_pPhysicsShell->PIsland());
		m_pPhysicsShell->UnFreeze();
		Fvector vel;vel.set(disp);
		const	u16	steps_num=10;
		const	float	fsteps_num=steps_num;
		vel.mul(1.f/fsteps_num/fixed_step);


		for(u16	i=0;steps_num>i;++i)
		{
			m_pPhysicsShell->set_LinearVel(vel);
			ph_world->Step();
		}
		//u16 step_num=disp.magnitude()/fixed_step;
		m_pPhysicsShell->set_ApplyByGravity(g);
		m_pPhysicsShell->set_CallbackData(cd);
		m_pPhysicsShell->remove_ObjectContactCallback(StaticEnvironmentCB);
		ph_world->UnFreeze();
}

void CCharacterPhysicsSupport::TestForWounded()
{
	m_was_wounded=false;
	if (!character_have_wounded_state)
	{
		return;
	}
	
	CKinematics* CKA=smart_cast<CKinematics*>(m_EntityAlife.Visual());
	CKA->CalculateBones();
	CBoneInstance CBI=CKA->LL_GetBoneInstance(0);
	Fmatrix position_matrix;
	position_matrix.mul(mXFORM,CBI.mTransform);
	
	xrXRC						xrc;
	xrc.ray_options				(0);
	xrc.ray_query(Level().ObjectSpace.GetStaticModel(),position_matrix.c,Fvector().set(0.0f,-1.0f,0.0f),pelvis_factor_low_pose_detect);
		
	if (xrc.r_count())
	{
		m_was_wounded=true;
	}
};

void CCharacterPhysicsSupport::UpdateFrictionAndJointResistanse()
{
	//ѕреобразование skel_ddelay из кадров в секунды и линейное нарастание сопротивлени€ в джоинтах со временем от момента смерти 

	if(skel_remain_time!=0)
	{
		skel_remain_time-=m_time_delta;
	};
	if (skel_remain_time<0)
	{
		skel_remain_time=0;
	};
			
	float curr_joint_resistance=hinge_force_factor1-
		(skel_remain_time*hinge_force_factor1)/skel_ddelay;
	m_pPhysicsShell->set_JointResistance(curr_joint_resistance);




	if(skeleton_skin_remain_time!=0)
	{
		skeleton_skin_remain_time-=m_time_delta;
	}
	if (skeleton_skin_remain_time<0)
	{
		skeleton_skin_remain_time=0;
	}

	if(skeleton_skin_remain_time_after_wound!=0)
	{
		skeleton_skin_remain_time_after_wound-=m_time_delta;
	};
	if (skeleton_skin_remain_time_after_wound<0)
	{
		skeleton_skin_remain_time_after_wound=0;
	};

	float ddelay,remain;
	if (m_was_wounded)
	{
		ddelay=skeleton_skin_ddelay_after_wound;
		remain=skeleton_skin_remain_time_after_wound;
	}
	else
	{
		ddelay=skeleton_skin_ddelay;
		remain=skeleton_skin_remain_time;
	}

	m_curr_skin_friction_in_death=skeleton_skin_friction_end+
		(remain/ddelay)*(skeleton_skin_friction_start-skeleton_skin_friction_end);	

	
};

void CCharacterPhysicsSupport::CalculateTimeDelta()
{
	if (m_Pred_Time==0.0)
	{
		m_time_delta=0;
	}
	else
	{
		m_time_delta=Device.fTimeGlobal-m_Pred_Time;					
	}
	m_Pred_Time=Device.fTimeGlobal;
};

void CCharacterPhysicsSupport::on_create_anim_mov_ctrl	()
{
	VERIFY( !anim_mov_state.active );
	anim_mov_state.character_exist = m_PhysicMovementControl->CharacterExist(); 
	if(anim_mov_state.character_exist)
		m_PhysicMovementControl->DestroyCharacter();
	anim_mov_state.active = true;
}

void CCharacterPhysicsSupport::on_destroy_anim_mov_ctrl	()
{
	VERIFY( anim_mov_state.active );
	if( anim_mov_state.character_exist )
						CreateCharacter();
	anim_mov_state.active = false;
}

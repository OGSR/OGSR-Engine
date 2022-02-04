#include "stdafx.h"
#include "phantom.h"
#include "../../../xr_3da/NET_Server_Trash/net_utils.h"
#include "level.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "../../../xr_3da/motion.h"
#include "../../../Include\xrRender\KinematicsAnimated.h"

CPhantom::CPhantom()
{
	fSpeed				= 4.f;
	fASpeed				= 1.7f;
	vHP.set				(0,0);
	fContactHit			= 0.f;
	m_CurState			= stInvalid;
	m_TgtState			= stInvalid;
}

CPhantom::~CPhantom()
{
}

//---------------------------------------------------------------------
void CPhantom::Load( LPCSTR section )
{
	inherited::Load		(section);
	//////////////////////////////////////////////////////////////////////////
	ISpatial* self		= smart_cast<ISpatial*> (this);
	if (self) {
		self->spatial.type &=~STYPE_VISIBLEFORAI;
		self->spatial.type &=~STYPE_REACTTOSOUND;
	}
	//////////////////////////////////////////////////////////////////////////
	fSpeed							= pSettings->r_float(section,"speed");
	fASpeed							= pSettings->r_float(section,"angular_speed");
	fContactHit						= pSettings->r_float(section,"contact_hit");

	LPCSTR snd_name			= 0;
	m_state_data[stBirth].particles	= pSettings->r_string(section,"particles_birth");
	snd_name						= pSettings->r_string(section,"sound_birth");
	if (snd_name&&snd_name[0])		m_state_data[stBirth].sound.create(snd_name,st_Effect,sg_SourceType);

	m_state_data[stFly].particles	= pSettings->r_string(section,"particles_fly");
	snd_name						= pSettings->r_string(section,"sound_fly");
	if (snd_name&&snd_name[0])		m_state_data[stFly].sound.create(snd_name,st_Effect,sg_SourceType);

	m_state_data[stContact].particles= pSettings->r_string(section,"particles_contact");
	snd_name						= pSettings->r_string(section,"sound_contact");
	if (snd_name&&snd_name[0])		m_state_data[stContact].sound.create(snd_name,st_Effect,sg_SourceType);

	m_state_data[stShoot].particles	= pSettings->r_string(section,"particles_shoot");
	snd_name						= pSettings->r_string(section,"sound_shoot");
	if (snd_name&&snd_name[0])		m_state_data[stShoot].sound.create(snd_name,st_Effect,sg_SourceType);
}
BOOL CPhantom::net_Spawn(CSE_Abstract* DC)
{
	CSE_ALifeCreaturePhantom*	OBJ	= smart_cast<CSE_ALifeCreaturePhantom*>(DC); VERIFY(OBJ);
	
	// select visual at first
	LPCSTR vis_name = OBJ->get_visual();
	if (!(vis_name&&vis_name[0])){
		LPCSTR visuals	= pSettings->r_string(cNameSect(),"visuals");
		u32 cnt			= _GetItemCount(visuals);
		string256 tmp;
		OBJ->set_visual	(_GetItem(visuals,Random.randI(cnt),tmp));
		// inform server
		NET_Packet		P;
		u_EventGen		(P, GE_CHANGE_VISUAL, OBJ->ID);
		P.w_stringZ		(tmp);
		u_EventSend		(P);
	}

	SwitchToState		(stBirth);			// initial state (changed on load method in inherited::)

	// inherited
	if (!inherited::net_Spawn(DC)) return FALSE;
	
	m_enemy			= Level().CurrentEntity();
	VERIFY			(m_enemy);

	// default init 
	m_fly_particles		= 0;
	SetfHealth			(0.001f);

	// orientate to enemy
	XFORM().k.sub	(m_enemy->Position(),Position()).normalize();
	XFORM().j.set	(0,1,0);
	XFORM().i.crossproduct	(XFORM().j,XFORM().k);
	XFORM().k.getHP	(vHP.x,vHP.y);

	// set animation
	IKinematicsAnimated *K			= smart_cast<IKinematicsAnimated*>(Visual());
	m_state_data[stBirth].motion	= K->ID_Cycle("birth_0");	
	m_state_data[stFly].motion		= K->ID_Cycle("fly_0");
	m_state_data[stContact].motion	= K->ID_Cycle("contact_0"); 
	m_state_data[stShoot].motion	= K->ID_Cycle("shoot_0");	

	VERIFY(K->LL_GetMotionDef(m_state_data[stBirth].motion)->flags&esmStopAtEnd);
	VERIFY(K->LL_GetMotionDef(m_state_data[stContact].motion)->flags&esmStopAtEnd);
	VERIFY(K->LL_GetMotionDef(m_state_data[stShoot].motion)->flags&esmStopAtEnd);

	// set state
	SwitchToState_internal(m_TgtState);

	setVisible		(m_CurState>stIdle?TRUE:FALSE);
	setEnabled		(TRUE);

	return			TRUE;
}
void CPhantom::net_Destroy	()
{
	inherited::net_Destroy	();

	// stop looped
	SStateData& sdata			= m_state_data[stFly];
	sdata.sound.stop			();
	CParticlesObject::Destroy	(m_fly_particles);
}

//---------------------------------------------------------------------
// Animation Callbacks
void CPhantom::animation_end_callback(CBlend* B)
{
	CPhantom *phantom				= (CPhantom*)B->CallbackParam;
	switch (phantom->m_CurState){
	case stBirth:	phantom->SwitchToState(stFly);	break;
	case stContact:	phantom->SwitchToState(stIdle);	break;
	case stShoot:	phantom->SwitchToState(stIdle);	break;
	}
}
//---------------------------------------------------------------------
void CPhantom::SwitchToState_internal(EState new_state)
{
	if (new_state!=m_CurState){
		IKinematicsAnimated *K	= smart_cast<IKinematicsAnimated*>(Visual());
		Fmatrix	xform			= XFORM_center	();
		UpdateEvent				= 0;
		// after event
		switch (m_CurState){
		case stBirth:		break;
		case stFly:			break;
		case stContact:{
			SStateData& sdata	= m_state_data[m_CurState];
			PlayParticles		(sdata.particles.c_str(),FALSE,xform);
			Fvector vE,vP;
			m_enemy->Center		(vE);
			Center				(vP);
			if (vP.distance_to_sqr(vE)<_sqr(Radius())){ 
				// hit enemy
				PsyHit			(m_enemy,fContactHit);
			}
			}break;
		case stShoot:{
			SStateData& sdata	= m_state_data[m_CurState];
			PlayParticles		(sdata.particles.c_str(),FALSE,xform);
		}break;
		case stIdle:		break;
		}
		// before event
		switch (new_state){
		case stBirth:{
			SStateData& sdata	= m_state_data[new_state];
			PlayParticles		(sdata.particles.c_str(),TRUE,xform);
			sdata.sound.play_at_pos(0,xform.c);
			K->PlayCycle		(sdata.motion, TRUE, animation_end_callback, this);
		}break;
		case stFly:{
			UpdateEvent.bind	(this,&CPhantom::OnFlyState);
			SStateData& sdata	= m_state_data[new_state];
			m_fly_particles		= PlayParticles(sdata.particles.c_str(),FALSE,xform);
			sdata.sound.play_at_pos(0,xform.c,sm_Looped);
			K->PlayCycle		(sdata.motion);
		}break;
		case stContact:{
			UpdateEvent.bind	(this,&CPhantom::OnDeadState);	
			SStateData& sdata	= m_state_data[new_state];
			sdata.sound.play_at_pos(0,xform.c);
			K->PlayCycle		(sdata.motion, TRUE, animation_end_callback, this);
		}break;
		case stShoot:{
			UpdateEvent.bind	(this,&CPhantom::OnDeadState);	
			SStateData& sdata	= m_state_data[new_state];
			PlayParticles		(sdata.particles.c_str(),TRUE,xform);
			sdata.sound.play_at_pos(0,xform.c);
			K->PlayCycle		(sdata.motion, TRUE, animation_end_callback, this);
		}break;
		case stIdle:{
			UpdateEvent.bind	(this,&CPhantom::OnIdleState);	
			SStateData& sdata	= m_state_data[m_CurState];
			sdata.sound.stop	();
			CParticlesObject::Destroy(m_fly_particles);
		}break;
		}
		m_CurState				= new_state;
	}
}

void CPhantom::OnIdleState()
{
		DestroyObject		();
}

void CPhantom::OnFlyState()
{
	UpdateFlyMedia			();
	if (g_Alive()){
		Fvector vE,vP;
		m_enemy->Center		(vE);
		Center				(vP);
		if (vP.distance_to_sqr(vE)<_sqr(Radius()+m_enemy->Radius())){
			SwitchToState	(stContact);
//			Hit				(1000.f,Fvector().set(0,0,1),this,-1,Fvector().set(0,0,0),100.f,ALife::eHitTypeFireWound);
			SHit HDS(1000.f,Fvector().set(0,0,1),this,BI_NONE,Fvector().set(0,0,0),100.f,ALife::eHitTypeFireWound);
			Hit(&HDS);
		}
	}
}
void CPhantom::OnDeadState() 
{
	UpdateFlyMedia	();
}
void CPhantom::UpdateFlyMedia()
{
	UpdatePosition	(m_enemy->Position());
	Fmatrix	xform			= XFORM_center();
	// update particles
	if (m_fly_particles){		
		Fvector		vel;
		vel.sub		(m_enemy->Position(),Position()).normalize_safe().mul(fSpeed);
		m_fly_particles->UpdateParent(xform,vel);
	}
	// update sound
	if (m_state_data[stFly].sound._feedback()) m_state_data[stFly].sound.set_position(xform.c);
}
//---------------------------------------------------------------------


void CPhantom::shedule_Update(u32 DT)
{
	spatial.type &=~STYPE_VISIBLEFORAI;

	inherited::shedule_Update(DT);

	IKinematicsAnimated *K	= smart_cast<IKinematicsAnimated*>(Visual());
	K->UpdateTracks			();
}

void CPhantom::UpdateCL()
{
	inherited::UpdateCL();

	if (!UpdateEvent.empty())	UpdateEvent();
	if (m_TgtState!=m_CurState)	SwitchToState_internal(m_TgtState);
}
//---------------------------------------------------------------------
//void CPhantom::Hit	(float P, Fvector &dir, CObject* who, s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type)
void	CPhantom::Hit							(SHit* pHDS)
{
	if (m_TgtState==stFly)	SwitchToState(stShoot);
	if (g_Alive()){
		SetfHealth		(-1.f);
//		inherited::Hit	(P,dir,who,element,p_in_object_space,impulse/100.f, hit_type);
		inherited::Hit	(pHDS);
	}
}
//---------------------------------------------------------------------
Fmatrix	CPhantom::XFORM_center()
{
	Fvector			center;
	Center			(center);
	Fmatrix	xform	= XFORM();
	return			xform.translate_over(center);
}

CParticlesObject* CPhantom::PlayParticles(const shared_str& name, BOOL bAutoRemove, const Fmatrix& xform)
{
	CParticlesObject* ps = CParticlesObject::Create(name.c_str(),bAutoRemove);
	ps->UpdateParent	(xform, zero_vel);
	ps->Play			();
	return bAutoRemove?0:ps;
}

//---------------------------------------------------------------------
void CPhantom::UpdatePosition(const Fvector& tgt_pos) 
{
	float			tgt_h,tgt_p;
	Fvector			tgt_dir,cur_dir;
	tgt_dir.sub		(tgt_pos,Position());
	tgt_dir.getHP	(tgt_h,tgt_p);

	angle_lerp		(vHP.x,tgt_h,fASpeed,Device.fTimeDelta);
	angle_lerp		(vHP.y,tgt_p,fASpeed,Device.fTimeDelta);

	cur_dir.setHP	(vHP.x,vHP.y);

	Fvector prev_pos=Position();
	XFORM().rotateY (-vHP.x);
	Position().mad	(prev_pos,cur_dir,fSpeed*Device.fTimeDelta);
}

void CPhantom::PsyHit(const CObject *object, float value) 
{
	NET_Packet			P;
	SHit				HS;
	HS.GenHeader		(GE_HIT, object->ID());				//				//	u_EventGen		(P,GE_HIT, object->ID());				
	HS.whoID			= (ID());					// own			//	P.w_u16			(object->ID());							
	HS.weaponID			= (ID());					// own			//	P.w_u16			(object->ID());							
	HS.dir				= (Fvector().set(0.f,1.f,0.f));		// direction	//	P.w_dir			(Fvector().set(0.f,1.f,0.f));			
	HS.power			= (value);							// hit value	//	P.w_float		(value);								
	HS.boneID			= (BI_NONE);						// bone			//	P.w_s16			(BI_NONE);								
	HS.p_in_bone_space	= (Fvector().set(0.f,0.f,0.f));						//	P.w_vec3		(Fvector().set(0.f,0.f,0.f));			
	HS.impulse			= (0.f);											//	P.w_float		(0.f);									
	HS.hit_type			= (ALife::eHitTypeTelepatic);						//	P.w_u16			(u16(ALife::eHitTypeTelepatic));
	HS.Write_Packet		(P);
	
	u_EventSend			(P);
}

//---------------------------------------------------------------------
// Core events
void CPhantom::save(NET_Packet &output_packet)
{
	output_packet.w_s32	(s32(m_CurState));
}
void CPhantom::load(IReader &input_packet)
{
	SwitchToState	(EState(input_packet.r_s32()));
}

void CPhantom::net_Export( CSE_Abstract* E ) {
  R_ASSERT( Local() );

  CSE_ALifeCreatureAbstract* creature = smart_cast<CSE_ALifeCreatureAbstract*>( E );
  creature->fHealth       = GetfHealth();
  creature->timestamp     = Level().timeServer();
  creature->flags         = 0;
  creature->o_Position    = Position();
  float yaw, pitch, bank;
  XFORM().getHPB( yaw, pitch, bank);
  creature->o_model       = yaw;
  creature->o_torso.yaw   = yaw;
  creature->o_torso.pitch = pitch;
  creature->o_torso.roll  = 0;
  creature->s_team        = u8( g_Team() );
  creature->s_squad       = u8( g_Squad() );
  creature->s_group       = u8( g_Group() );
}

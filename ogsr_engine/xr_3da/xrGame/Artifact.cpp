#include "stdafx.h"
#include "artifact.h"
#include "PhysicsShell.h"
#include "PhysicsShellHolder.h"
#include "game_cl_base.h"
#include "../skeletonanimated.h"
#include "inventory.h"
#include "level.h"
#include "ai_object_location.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "phworld.h"
#include "restriction_space.h"
#include "../IGame_Persistent.h"
#include "level_graph.h"
#include "ai_space.h"

#define	FASTMODE_DISTANCE (50.f)	//distance to camera from sphere, when zone switches to fast update sequence

#define CHOOSE_MAX(x,inst_x,y,inst_y,z,inst_z)\
	if(x>y)\
		if(x>z){inst_x;}\
		else{inst_z;}\
	else\
		if(y>z){inst_y;}\
		else{inst_z;}

struct SArtefactActivation{
	enum EActivationStates		{eNone=0, eStarting, eFlying, eBeforeSpawn, eSpawnZone, eMax};
	struct SStateDef{
		float		m_time;
		shared_str	m_snd;
		Fcolor		m_light_color;
		float		m_light_range;
		shared_str	m_particle;
		shared_str	m_animation;
		
					SStateDef	():m_time(0.0f){};
		void		Load		(LPCSTR section, LPCSTR name);
	};

	SArtefactActivation			(CArtefact* af, u32 owner_id);
	~SArtefactActivation		();
	CArtefact*					m_af;
	svector<SStateDef,eMax>		m_activation_states;
	EActivationStates			m_cur_activation_state;
	float						m_cur_state_time;

	ref_light					m_light;
	ref_sound					m_snd;
	
	u32							m_owner_id;

	void						UpdateActivation				();
	void						Load							();
	void						Start							();
	void						ChangeEffects					();
	void						UpdateEffects					();
	void						SpawnAnomaly					();
	void						PhDataUpdate					(dReal step);
};


CArtefact::CArtefact(void) 
{
	shedule.t_min				= 20;
	shedule.t_max				= 50;
	m_sParticlesName			= NULL;
	m_pTrailLight				= NULL;
	m_activationObj				= NULL;
}


CArtefact::~CArtefact(void) 
{}

void CArtefact::Load(LPCSTR section) 
{
	inherited::Load			(section);


	if (pSettings->line_exist(section, "particles"))
		m_sParticlesName	= pSettings->r_string(section, "particles");

	m_bLightsEnabled		= !!pSettings->r_bool(section, "lights_enabled");
	if(m_bLightsEnabled){
		sscanf(pSettings->r_string(section,"trail_light_color"), "%f,%f,%f", 
			&m_TrailLightColor.r, &m_TrailLightColor.g, &m_TrailLightColor.b);
		m_fTrailLightRange	= pSettings->r_float(section,"trail_light_range");
	}


	{
		m_fHealthRestoreSpeed = pSettings->r_float		(section,"health_restore_speed"		);
		m_fRadiationRestoreSpeed = pSettings->r_float	(section,"radiation_restore_speed"	);
		m_fSatietyRestoreSpeed = pSettings->r_float		(section,"satiety_restore_speed"	);
		m_fPowerRestoreSpeed = pSettings->r_float		(section,"power_restore_speed"		);
		m_fBleedingRestoreSpeed = pSettings->r_float	(section,"bleeding_restore_speed"	);
		if(pSettings->section_exist(/**cNameSect(), */pSettings->r_string(section,"hit_absorbation_sect")))
			m_ArtefactHitImmunities.LoadImmunities(pSettings->r_string(section,"hit_absorbation_sect"),pSettings);
	}
	m_bCanSpawnZone = !!pSettings->line_exist("artefact_spawn_zones", section);


	animGet				(m_anim_idle,					pSettings->r_string(*hud_sect,"anim_idle"));
	animGet				(m_anim_idle_sprint,			pSettings->r_string(*hud_sect,"anim_idle_sprint"));
	animGet				(m_anim_hide,					pSettings->r_string(*hud_sect,"anim_hide"));
	animGet				(m_anim_show,					pSettings->r_string(*hud_sect,"anim_show"));
	animGet				(m_anim_activate,				pSettings->r_string(*hud_sect,"anim_activate"));
}

BOOL CArtefact::net_Spawn(CSE_Abstract* DC) 
{
	BOOL result = inherited::net_Spawn(DC);
	if (*m_sParticlesName) 
	{Fvector dir;
		dir.set(0,1,0);
		CParticlesPlayer::StartParticles(m_sParticlesName,dir,ID(),-1, false);
	}

	VERIFY(m_pTrailLight == NULL);
	m_pTrailLight = ::Render->light_create();
	m_pTrailLight->set_shadow(true);

	StartLights();
	/////////////////////////////////////////
	m_CarringBoneID = u16(-1);
	/////////////////////////////////////////
	CKinematicsAnimated	*K=smart_cast<CKinematicsAnimated*>(Visual());
	if(K)K->PlayCycle("idle");
	
	o_fastmode					= FALSE	;		// start initially with fast-mode enabled
	o_render_frame				= 0		;
	SetState					(eHidden);

	return result;	
}

void CArtefact::net_Destroy() 
{
/*
	if (*m_sParticlesName) 
		CParticlesPlayer::StopParticles(m_sParticlesName, BI_NONE, true);
*/
	inherited::net_Destroy		();

	StopLights					();
	m_pTrailLight.destroy		();
	CPHUpdateObject::Deactivate	();
	xr_delete					(m_activationObj);
}

void CArtefact::OnH_A_Chield() 
{
	inherited::OnH_A_Chield		();

	StopLights();
	if (GameID() == GAME_SINGLE)
	{
		if (*m_sParticlesName) 
		{	
			CParticlesPlayer::StopParticles(m_sParticlesName, BI_NONE, true);
		}
	}
	else
	{
		CKinematics* K	= smart_cast<CKinematics*>(H_Parent()->Visual());
		if (K)
			m_CarringBoneID			= K->LL_BoneID("bip01_head");
		else
			m_CarringBoneID = u16(-1);
	}
}

void CArtefact::OnH_B_Independent(bool just_before_destroy) 
{
	VERIFY(!ph_world->Processing());
	inherited::OnH_B_Independent(just_before_destroy);

	StartLights();
	if (*m_sParticlesName) 
	{
		Fvector dir;
		dir.set(0,1,0);
		CParticlesPlayer::StartParticles(m_sParticlesName,dir,ID(),-1, false);
	}
}

// called only in "fast-mode"
void CArtefact::UpdateCL		() 
{
	inherited::UpdateCL			();
	
	if (o_fastmode || m_activationObj)
		UpdateWorkload			(Device.dwTimeDelta);	
}

void CArtefact::UpdateWorkload		(u32 dt) 
{
	VERIFY(!ph_world->Processing());
	// particles - velocity
	Fvector vel = {0, 0, 0};
	if (H_Parent()) 
	{
		CPhysicsShellHolder* pPhysicsShellHolder = smart_cast<CPhysicsShellHolder*>(H_Parent());
		if(pPhysicsShellHolder) pPhysicsShellHolder->PHGetLinearVell(vel);
	}
	CParticlesPlayer::SetParentVel	(vel);

	// 
	UpdateLights					();
	if(m_activationObj)	{
		CPHUpdateObject::Activate			();
		m_activationObj->UpdateActivation	();
		return	;
	}

	// custom-logic
	UpdateCLChild					();
}

void CArtefact::shedule_Update		(u32 dt) 
{
	inherited::shedule_Update		(dt);

	//////////////////////////////////////////////////////////////////////////
	// check "fast-mode" border
	if (H_Parent())			o_switch_2_slow	();
	else					{
		Fvector	center;			Center(center);
		BOOL	rendering		= (Device.dwFrame==o_render_frame);
		float	cam_distance	= Device.vCameraPosition.distance_to(center)-Radius();
		if (rendering || (cam_distance < FASTMODE_DISTANCE))	o_switch_2_fast	();
		else													o_switch_2_slow	();
	}
	if (!o_fastmode)		UpdateWorkload	(dt);
}


void CArtefact::create_physic_shell	()
{
	///create_box2sphere_physic_shell	();
	m_pPhysicsShell=P_build_Shell(this,false);
	m_pPhysicsShell->Deactivate();
}

void CArtefact::StartLights()
{
	VERIFY(!ph_world->Processing());
	if(!m_bLightsEnabled) return;

	//включить световую подсветку от двигателя
	m_pTrailLight->set_color(m_TrailLightColor.r, 
		m_TrailLightColor.g, 
		m_TrailLightColor.b);

	m_pTrailLight->set_range(m_fTrailLightRange);
	m_pTrailLight->set_position(Position()); 
	m_pTrailLight->set_active(true);
}

void CArtefact::StopLights()
{
	VERIFY(!ph_world->Processing());
	if(!m_bLightsEnabled) return;
	m_pTrailLight->set_active(false);
}

void CArtefact::UpdateLights()
{
	VERIFY(!ph_world->Processing());
	if(!m_bLightsEnabled || !m_pTrailLight->get_active()) return;
	m_pTrailLight->set_position(Position());
}

void CArtefact::ActivateArtefact	()
{
	VERIFY(m_bCanSpawnZone);
	VERIFY( H_Parent() );
	m_activationObj = xr_new<SArtefactActivation>(this,H_Parent()->ID());
	m_activationObj->Start();

}

void CArtefact::PhDataUpdate	(dReal step)
{
	if(m_activationObj)
		m_activationObj->PhDataUpdate			(step);
}

bool CArtefact::CanTake() const
{
	if(!inherited::CanTake())return false;
	return (m_activationObj==NULL);
}

void CArtefact::Hide()
{
	SwitchState(eHiding);
}

void CArtefact::Show()
{
	SwitchState(eShowing);
}
#include "inventoryOwner.h"
#include "Entity_alive.h"
void CArtefact::UpdateXForm()
{
	if (Device.dwFrame!=dwXF_Frame)
	{
		dwXF_Frame			= Device.dwFrame;

		if (0==H_Parent())	return;

		// Get access to entity and its visual
		CEntityAlive*		E		= smart_cast<CEntityAlive*>(H_Parent());
        
		if(!E)				return	;

		const CInventoryOwner	*parent = smart_cast<const CInventoryOwner*>(E);
		if (parent && parent->use_simplified_visual())
			return;

		VERIFY				(E);
		CKinematics*		V		= smart_cast<CKinematics*>	(E->Visual());
		VERIFY				(V);

		// Get matrices
		int					boneL,boneR,boneR2;
		E->g_WeaponBones	(boneL,boneR,boneR2);

		boneL = boneR2;

		V->CalculateBones	();
		Fmatrix& mL			= V->LL_GetTransform(u16(boneL));
		Fmatrix& mR			= V->LL_GetTransform(u16(boneR));

		// Calculate
		Fmatrix				mRes;
		Fvector				R,D,N;
		D.sub				(mL.c,mR.c);	D.normalize_safe();
		R.crossproduct		(mR.j,D);		R.normalize_safe();
		N.crossproduct		(D,R);			N.normalize_safe();
		mRes.set			(R,N,D,mR.c);
		mRes.mulA_43		(E->XFORM());
//		UpdatePosition		(mRes);
		XFORM().mul			(mRes,offset());
	}
}
#include "xr_level_controller.h"
bool CArtefact::Action(s32 cmd, u32 flags) 
{
	switch (cmd)
	{
	case kWPN_FIRE:
		{
			if (flags&CMD_START && m_bCanSpawnZone){
				SwitchState(eActivating);
				return true;
			}
			if (flags&CMD_STOP && m_bCanSpawnZone && GetState()==eActivating)
			{
				SwitchState(eIdle);
				return true;
			}
		}break;
	default:
		break;
	}
	return inherited::Action(cmd,flags);
}

void CArtefact::onMovementChanged	(ACTOR_DEFS::EMoveCommand cmd)
{
	if( (cmd == ACTOR_DEFS::mcSprint)&&(GetState()==eIdle)  )
		PlayAnimIdle		();
}

void CArtefact::OnStateSwitch		(u32 S)
{
	inherited::OnStateSwitch	(S);
	switch(S){
	case eShowing:
		{
			m_pHUD->animPlay(random_anim(m_anim_show),		FALSE, this, S);
		}break;
	case eHiding:
		{
			m_pHUD->animPlay(random_anim(m_anim_hide),		FALSE, this, S);
		}break;
	case eActivating:
		{
			m_pHUD->animPlay(random_anim(m_anim_activate),	FALSE, this, S);
		}break;
	case eIdle:
		{
			PlayAnimIdle();
		}break;
	};
}

void CArtefact::PlayAnimIdle()
{
	m_pHUD->animPlay(random_anim(m_anim_idle),		FALSE, NULL, eIdle);
}

void CArtefact::OnAnimationEnd		(u32 state)
{
	switch (state)
	{
	case eHiding:
		{
			SwitchState(eHidden);
//.			if(m_pCurrentInventory->GetNextActiveSlot()!=NO_ACTIVE_SLOT)
//.				m_pCurrentInventory->Activate(m_pCurrentInventory->GetPrevActiveSlot());
		}break;
	case eShowing:
		{
			SwitchState(eIdle);
		}break;
	case eActivating:
		{
			if(Local()){
				SwitchState		(eHiding);
				NET_Packet		P;
				u_EventGen		(P, GEG_PLAYER_ACTIVATEARTEFACT, H_Parent()->ID());
				P.w_u16			(ID());
				u_EventSend		(P);	
			}
		}break;
	};
}



u16	CArtefact::bone_count_to_synchronize	() const
{
	return CInventoryItem::object().PHGetSyncItemsNumber();
}



//---SArtefactActivation----
SArtefactActivation::SArtefactActivation(CArtefact* af,u32 owner_id)
{
	m_af			= af;
	Load			();
	m_light			= ::Render->light_create();
	m_light->set_shadow(true);
	m_owner_id		= owner_id;
}
SArtefactActivation::~SArtefactActivation()
{
	m_light.destroy();

}

void SArtefactActivation::Load()
{
	for(int i=0; i<(int)eMax; ++i)
		m_activation_states.push_back(SStateDef());

	LPCSTR activation_seq = pSettings->r_string(*m_af->cNameSect(),"artefact_activation_seq");


	m_activation_states[(int)eStarting].Load(activation_seq,	"starting");
	m_activation_states[(int)eFlying].Load(activation_seq,		"flying");
	m_activation_states[(int)eBeforeSpawn].Load(activation_seq,	"idle_before_spawning");
	m_activation_states[(int)eSpawnZone].Load(activation_seq,	"spawning");

}

void SArtefactActivation::Start()
{
	VERIFY(!ph_world->Processing());
	m_af->StopLights				();
	m_cur_activation_state			= eStarting;
	m_cur_state_time				= 0.0f;
	
	m_af->processing_activate();

	NET_Packet						P;
	CGameObject::u_EventGen			(P,GE_OWNERSHIP_REJECT, m_af->H_Parent()->ID());
	P.w_u16							(m_af->ID());
	if (OnServer())
		CGameObject::u_EventSend		(P);
	m_light->set_active				(true);
	ChangeEffects					();
}

void SArtefactActivation::UpdateActivation()
{
	VERIFY(!ph_world->Processing());
	m_cur_state_time				+=	Device.fTimeDelta;
	if(m_cur_state_time				>=	m_activation_states[int(m_cur_activation_state)].m_time){
		m_cur_activation_state		=	(EActivationStates)(int)(m_cur_activation_state+1);
		
		if(m_cur_activation_state == eMax){
			m_cur_activation_state = eNone;

			m_af->processing_deactivate			();
			m_af->CPHUpdateObject::Deactivate	();
			m_af->DestroyObject();
		}

		m_cur_state_time	= 0.0f;
		ChangeEffects				();


	if(m_cur_activation_state==eSpawnZone && OnServer())
		SpawnAnomaly	();

	}
	UpdateEffects				();

}

void SArtefactActivation::PhDataUpdate(dReal step)
{
	if (m_cur_activation_state==eFlying) {
		Fvector dir	= {0, -1.f, 0};
		if(Level().ObjectSpace.RayTest(m_af->Position(), dir, 1.0f, collide::rqtBoth,NULL,m_af) ){
			dir.y = ph_world->Gravity()*1.1f; 
			m_af->m_pPhysicsShell->applyGravityAccel(dir);
		}
	}

}
void SArtefactActivation::ChangeEffects()
{
	VERIFY(!ph_world->Processing());
	SStateDef& state_def = m_activation_states[(int)m_cur_activation_state];
	
	if(m_snd._feedback())
		m_snd.stop();
	
	if(state_def.m_snd.size()){
		m_snd.create			(*state_def.m_snd,st_Effect,sg_SourceType);
		m_snd.play_at_pos		(m_af,	m_af->Position());
	};

	m_light->set_range		(	state_def.m_light_range);
	m_light->set_color		(	state_def.m_light_color.r,
								state_def.m_light_color.g,
								state_def.m_light_color.b);
	
	if(state_def.m_particle.size()){
		Fvector dir;
		dir.set(0,1,0);

		m_af->CParticlesPlayer::StartParticles(	state_def.m_particle,
												dir,
												m_af->ID(),
												iFloor(state_def.m_time*1000) );
	};
	if(state_def.m_animation.size()){
		CKinematicsAnimated	*K=smart_cast<CKinematicsAnimated*>(m_af->Visual());
		if(K)K->PlayCycle(*state_def.m_animation);
	}

}

void SArtefactActivation::UpdateEffects()
{
	VERIFY(!ph_world->Processing());
	if(m_snd._feedback())
		m_snd.set_position( m_af->Position() );
	
	m_light->set_position(m_af->Position());
}

void SArtefactActivation::SpawnAnomaly()
{
	VERIFY(!ph_world->Processing());
	string128 tmp;
	LPCSTR str			= pSettings->r_string("artefact_spawn_zones",*m_af->cNameSect());
	VERIFY3(3==_GetItemCount(str),"Bad record format in artefact_spawn_zones",str);
	float zone_radius	= (float)atof(_GetItem(str,1,tmp));
	float zone_power	= (float)atof(_GetItem(str,2,tmp));
	LPCSTR zone_sect	= _GetItem(str,0,tmp); //must be last call of _GetItem... (LPCSTR !!!)

		Fvector pos;
		m_af->Center(pos);
		u32 lvid = m_af->UsedAI_Locations() ? m_af->ai_location().level_vertex_id() : ai().level_graph().vertex(pos);
		CSE_Abstract		*object = Level().spawn_item(	zone_sect,
															pos,
															(g_dedicated_server)?u32(-1): lvid,
															0xffff,
															true
		);
		CSE_ALifeAnomalousZone*		AlifeZone = smart_cast<CSE_ALifeAnomalousZone*>(object);
		VERIFY(AlifeZone);
		CShapeData::shape_def		_shape;
		_shape.data.sphere.P.set	(0.0f,0.0f,0.0f);
		_shape.data.sphere.R		= zone_radius;
		_shape.type					= CShapeData::cfSphere;
		AlifeZone->assign_shapes	(&_shape,1);
		AlifeZone->m_maxPower		= zone_power;
		AlifeZone->m_owner_id		= m_owner_id;
		AlifeZone->m_space_restrictor_type	= RestrictionSpace::eRestrictorTypeNone;

		NET_Packet					P;
		object->Spawn_Write			(P,TRUE);
		Level().Send				(P,net_flags(TRUE));
		F_entity_Destroy			(object);
//. #ifdef DEBUG
		Msg("artefact [%s] spawned a zone [%s] at [%f]", *m_af->cName(), zone_sect, Device.fTimeGlobal);
//. #endif
}

shared_str clear_brackets(LPCSTR src)
{
	if	(0==src)					return	shared_str(0);
	
	if( NULL == strchr(src,'"') )	return	shared_str(src);

	string512						_original;	
	strcpy_s						(_original,src);
	u32			_len				= xr_strlen(_original);
	if	(0==_len)					return	shared_str("");
	if	('"'==_original[_len-1])	_original[_len-1]=0;					// skip end
	if	('"'==_original[0])			return	shared_str(&_original[0] + 1);	// skip begin
	return									shared_str(_original);

}
void SArtefactActivation::SStateDef::Load(LPCSTR section, LPCSTR name)
{
	LPCSTR str = pSettings->r_string(section,name);
	VERIFY(_GetItemCount(str)==8);


	string128 tmp;

	m_time			= (float)atof(		_GetItem(str,0,tmp) );
	
	m_snd			= clear_brackets(	_GetItem(str,1,tmp) )	;

	m_light_color.r = (float)atof(		_GetItem(str,2,tmp) );
	m_light_color.g = (float)atof(		_GetItem(str,3,tmp) );
	m_light_color.b = (float)atof(		_GetItem(str,4,tmp) );

	m_light_range	= (float)atof(		_GetItem(str,5,tmp) );

	m_particle		= clear_brackets(	_GetItem(str,6,tmp) );
	m_animation		= clear_brackets(	_GetItem(str,7,tmp) );

}
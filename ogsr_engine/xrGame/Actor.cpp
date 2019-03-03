#include "stdafx.h"
#include "Actor_Flags.h"
#include "hudmanager.h"
#ifdef DEBUG
#	include "ode_include.h"
#	include "../xr_3da/StatGraph.h"
#	include "PHDebug.h"
#endif // DEBUG
#include "alife_space.h"
#include "hit.h"
#include "PHDestroyable.h"
#include "Car.h"
#include "xrserver_objects_alife_monsters.h"
#include "CameraLook.h"
#include "CameraFirstEye.h"
#include "effectorfall.h"
#include "EffectorBobbing.h"
#include "clsid_game.h"
#include "ActorEffector.h"
#include "EffectorZoomInertion.h"
#include "SleepEffector.h"
#include "character_info.h"
#include "CustomOutfit.h"
#include "actorcondition.h"
#include "UIGameCustom.h"
#include "game_cl_base_weapon_usage_statistic.h"

// breakpoints
#include "../xr_3da/xr_input.h"

//
#include "Actor.h"
#include "ActorAnimation.h"
#include "actor_anim_defs.h"
#include "HudItem.h"
#include "ai_sounds.h"
#include "ai_space.h"
#include "trade.h"
#include "inventory.h"
#include "Physics.h"
#include "level.h"
#include "GamePersistent.h"
#include "game_cl_base.h"
#include "game_cl_single.h"
#include "xrmessages.h"
#include "string_table.h"
#include "usablescriptobject.h"
#include "cl_intersect.h"
#include "ExtendedGeom.h"
#include "alife_registry_wrappers.h"
#include "..\xr_3da\skeletonanimated.h"
#include "artifact.h"
#include "CharacterPhysicsSupport.h"
#include "material_manager.h"
#include "IColisiondamageInfo.h"
#include "ui/UIMainIngameWnd.h"
#include "ui/UIArtefactPanel.h"
#include "map_manager.h"
#include "GameTaskManager.h"
#include "actor_memory.h"
#include "Script_Game_Object.h"
#include "Game_Object_Space.h"
#include "script_callback_ex.h"
#include "InventoryBox.h"
#include "location_manager.h"
#include "PHCapture.h"

// Tip for action for object we're looking at
constexpr const char* m_sCarCharacterUseAction        = "car_character_use";
constexpr const char* m_sCharacterUseAction           = "character_use";
constexpr const char* m_sDeadCharacterUseAction       = "dead_character_use";
constexpr const char* m_sDeadCharacterUseOrDragAction = "dead_character_use_or_drag";
constexpr const char* m_sInventoryItemUseAction       = "inventory_item_use";
constexpr const char* m_sInventoryItemUseOrDragAction = "inventory_item_use_or_drag";
constexpr const char* m_sGameObjectDragAction         = "game_object_drag";

const u32		patch_frames	= 50;
const float		respawn_delay	= 1.f;
const float		respawn_auto	= 7.f;

static float IReceived = 0;
static float ICoincidenced = 0;


//skeleton
static Fbox		bbStandBox;
static Fbox		bbCrouchBox;
static Fvector	vFootCenter;
static Fvector	vFootExt;

Flags32			psActorFlags={ AF_3D_SCOPES };

static bool updated;

CActor::CActor() : CEntityAlive(),current_ik_cam_shift(0)
{
	encyclopedia_registry	= xr_new<CEncyclopediaRegistryWrapper	>();
	game_news_registry		= xr_new<CGameNewsRegistryWrapper		>();
	// Cameras
	cameras[eacFirstEye]	= xr_new<CCameraFirstEye>				(this);
	cameras[eacFirstEye]->Load("actor_firsteye_cam");

	if constexpr ( true /*strstr(Core.Params,"-psp")*/ )
		psActorFlags.set(AF_PSP, TRUE);
	else
		psActorFlags.set(AF_PSP, FALSE);

	if( psActorFlags.test(AF_PSP) )
	{
		cameras[eacLookAt]		= xr_new<CCameraLook2>				(this);
		cameras[eacLookAt]->Load("actor_look_cam_psp");
	}else
	{
		cameras[eacLookAt]		= xr_new<CCameraLook>				(this);
		cameras[eacLookAt]->Load("actor_look_cam");
	}
	cameras[eacFreeLook]	= xr_new<CCameraLook>					(this);
	cameras[eacFreeLook]->Load("actor_free_cam");

	cam_active				= eacFirstEye;
	fPrevCamPos				= 0.0f;
	vPrevCamDir.set			(0.f,0.f,1.f);
	fCurAVelocity			= 0.0f;
	// эффекторы
	pCamBobbing				= 0;
	m_pSleepEffector		= NULL;
	m_pSleepEffectorPP		= NULL;


	r_torso.yaw				= 0;
	r_torso.pitch			= 0;
	r_torso.roll			= 0;
	r_torso_tgt_roll		= 0;
	r_model_yaw				= 0;
	r_model_yaw_delta		= 0;
	r_model_yaw_dest		= 0;

	b_DropActivated			= 0;
	f_DropPower				= 0.f;

	m_fRunFactor			= 2.f;
	m_fCrouchFactor			= 0.2f;
	m_fClimbFactor			= 1.f;
	m_fCamHeightFactor		= 0.87f;

	m_fFallTime				=	s_fFallTime;
	m_bAnimTorsoPlayed		=	false;

	m_pPhysicsShell			=	NULL;



	m_holder				=	NULL;
	m_holderID				=	u16(-1);


#ifdef DEBUG
	Device.seqRender.Add	(this,REG_PRIORITY_LOW);
#endif

	//разрешить использование пояса в inventory
	inventory().SetBeltUseful(true);

	m_pPersonWeLookingAt	= NULL;
	m_pVehicleWeLookingAt	= NULL;
	m_pObjectWeLookingAt	= NULL;
	m_bPickupMode			= false;

	pStatGraph				= NULL;

	m_pActorEffector		= NULL;

	m_bZoomAimingMode		= false;

	m_sDefaultObjAction		= nullptr;

	m_fSprintFactor			= 4.f;

	hFriendlyIndicator.create(FVF::F_LIT,RCache.Vertex.Buffer(),RCache.QuadIB);

	m_pUsableObject			= NULL;


	m_anims					= xr_new<SActorMotions>();
	m_vehicle_anims			= xr_new<SActorVehicleAnims>();
	m_entity_condition		= NULL;
	m_iLastHitterID			= u16(-1);
	m_iLastHittingWeaponID	= u16(-1);
	m_game_task_manager		= NULL;
	m_statistic_manager		= NULL;
	//-----------------------------------------------------------------------------------
	m_memory				= xr_new<CActorMemory>(this);
	m_bOutBorder			= false;
	hit_probability			= 1.f;
	m_feel_touch_characters = 0;
	//-----------------------------------------------------------------------------------
	m_dwILastUpdateTime		= 0;

	m_location_manager		= xr_new<CLocationManager>(this);

	m_fDrugPsyProtectionCoeff	= 1.f;
	m_fDrugRadProtectionCoeff	= 1.f;
	
	m_loaded_ph_box_id = 0;
	updated = false;

	// Alex ADD: for smooth crouch fix
	CurrentHeight = 0.f;

        hit_slowmo = 0.f;
}


CActor::~CActor()
{
	xr_delete				(m_location_manager);

	xr_delete				(m_memory);

	xr_delete				(encyclopedia_registry);
	xr_delete				(game_news_registry);
#ifdef DEBUG
	Device.seqRender.Remove(this);
#endif
	//xr_delete(Weapons);
	for (int i=0; i<eacMaxCam; ++i) xr_delete(cameras[i]);

	m_HeavyBreathSnd.destroy();
	m_BloodSnd.destroy		();

	xr_delete				(m_pActorEffector);

	xr_delete				(m_pSleepEffector);

	hFriendlyIndicator.destroy();

	xr_delete				(m_pPhysics_support);

	xr_delete				(m_anims);
	xr_delete				(m_vehicle_anims);
}

void CActor::reinit	()
{
	character_physics_support()->movement()->CreateCharacter		();
	character_physics_support()->movement()->SetPhysicsRefObject	(this);
	CEntityAlive::reinit						();
	CInventoryOwner::reinit						();

	character_physics_support()->in_Init		();
	material().reinit							();

	m_pUsableObject								= NULL;
	memory().reinit							();
	
	set_input_external_handler					(0);
	m_time_lock_accel							= 0;
}

void CActor::reload	(LPCSTR section)
{
	CEntityAlive::reload		(section);
	CInventoryOwner::reload		(section);
	material().reload			(section);
	CStepManager::reload		(section);
	memory().reload			(section);
	m_location_manager->reload	(section);
}

void CActor::Load	(LPCSTR section )
{
	// Msg						("Loading actor: %s",section);
	inherited::Load				(section);
	material().Load				(section);
	CInventoryOwner::Load		(section);
	m_location_manager->Load	(section);

	OnDifficultyChanged		();
	//////////////////////////////////////////////////////////////////////////
	ISpatial*		self			=	smart_cast<ISpatial*> (this);
	if (self)	{
		self->spatial.type	|=	STYPE_VISIBLEFORAI;
		self->spatial.type	&= ~STYPE_REACTTOSOUND;
	}
	//////////////////////////////////////////////////////////////////////////

	// m_PhysicMovementControl: General
	//m_PhysicMovementControl->SetParent		(this);
	Fbox	bb;Fvector	vBOX_center,vBOX_size;
	// m_PhysicMovementControl: BOX
	vBOX_center= pSettings->r_fvector3	(section,"ph_box2_center"	);
	vBOX_size	= pSettings->r_fvector3	(section,"ph_box2_size"		);
	bb.set	(vBOX_center,vBOX_center); bb.grow(vBOX_size);
	character_physics_support()->movement()->SetBox		(2,bb);

	if ( pSettings->line_exist( section, "ph_box4_center" ) && pSettings->line_exist( section, "ph_box4_size" ) ) {
	  vBOX_center = pSettings->r_fvector3( section, "ph_box4_center" );
	  vBOX_size   = pSettings->r_fvector3( section, "ph_box4_size"   );
	  bb.set( vBOX_center, vBOX_center ); bb.grow( vBOX_size );
	  character_physics_support()->movement()->SetBox( 4, bb );
	}
	else
	  character_physics_support()->movement()->SetBox( 4, bb );

	// m_PhysicMovementControl: BOX
	vBOX_center= pSettings->r_fvector3	(section,"ph_box1_center"	);
	vBOX_size	= pSettings->r_fvector3	(section,"ph_box1_size"		);
	bb.set	(vBOX_center,vBOX_center); bb.grow(vBOX_size);
	character_physics_support()->movement()->SetBox		(1,bb);

	if ( pSettings->line_exist( section, "ph_box3_center" ) && pSettings->line_exist( section, "ph_box3_size" ) ) {
	  vBOX_center = pSettings->r_fvector3( section, "ph_box3_center" );
	  vBOX_size   = pSettings->r_fvector3( section, "ph_box3_size"   );
	  bb.set( vBOX_center, vBOX_center ); bb.grow( vBOX_size );
	  character_physics_support()->movement()->SetBox( 3, bb );
	}
	else
	  character_physics_support()->movement()->SetBox( 3, bb );

	// m_PhysicMovementControl: BOX
	vBOX_center= pSettings->r_fvector3	(section,"ph_box0_center"	);
	vBOX_size	= pSettings->r_fvector3	(section,"ph_box0_size"		);
	bb.set	(vBOX_center,vBOX_center); bb.grow(vBOX_size);
	character_physics_support()->movement()->SetBox		(0,bb);

	//// m_PhysicMovementControl: Foots
	//Fvector	vFOOT_center= pSettings->r_fvector3	(section,"ph_foot_center"	);
	//Fvector	vFOOT_size	= pSettings->r_fvector3	(section,"ph_foot_size"		);
	//bb.set	(vFOOT_center,vFOOT_center); bb.grow(vFOOT_size);
	////m_PhysicMovementControl->SetFoots	(vFOOT_center,vFOOT_size);

	// m_PhysicMovementControl: Crash speed and mass
	float	cs_min		= pSettings->r_float	(section,"ph_crash_speed_min"	);
	float	cs_max		= pSettings->r_float	(section,"ph_crash_speed_max"	);
	float	mass		= pSettings->r_float	(section,"ph_mass"				);
	character_physics_support()->movement()->SetCrashSpeeds	(cs_min,cs_max);
	character_physics_support()->movement()->SetMass		(mass);
	if(pSettings->line_exist(section,"stalker_restrictor_radius"))
		character_physics_support()->movement()->SetActorRestrictorRadius(CPHCharacter::rtStalker,pSettings->r_float(section,"stalker_restrictor_radius"));
	if(pSettings->line_exist(section,"stalker_small_restrictor_radius"))
		character_physics_support()->movement()->SetActorRestrictorRadius(CPHCharacter::rtStalkerSmall,pSettings->r_float(section,"stalker_small_restrictor_radius"));
	if(pSettings->line_exist(section,"medium_monster_restrictor_radius"))
		character_physics_support()->movement()->SetActorRestrictorRadius(CPHCharacter::rtMonsterMedium,pSettings->r_float(section,"medium_monster_restrictor_radius"));
	character_physics_support()->movement()->Load(section);

	

	m_fWalkAccel				= pSettings->r_float(section,"walk_accel");	
	m_fJumpSpeed				= pSettings->r_float(section,"jump_speed");
	m_fRunFactor				= pSettings->r_float(section,"run_coef");
	m_fRunBackFactor			= pSettings->r_float(section,"run_back_coef");
	m_fWalkBackFactor			= pSettings->r_float(section,"walk_back_coef");
	m_fCrouchFactor				= pSettings->r_float(section,"crouch_coef");
	m_fClimbFactor				= pSettings->r_float(section,"climb_coef");
	m_fSprintFactor				= pSettings->r_float(section,"sprint_koef");

	m_fWalk_StrafeFactor		= READ_IF_EXISTS(pSettings, r_float, section, "walk_strafe_coef", 1.0f);
	m_fRun_StrafeFactor			= READ_IF_EXISTS(pSettings, r_float, section, "run_strafe_coef", 1.0f);

	m_hit_slowmo_jump = READ_IF_EXISTS( pSettings, r_bool, section, "hit_slowmo_jump", false );

	m_fExoFactor = 1.0f;

	m_fCamHeightFactor			= pSettings->r_float(section,"camera_height_factor");
	character_physics_support()->movement()		->SetJumpUpVelocity(m_fJumpSpeed);
	float AirControlParam		= pSettings->r_float	(section,"air_control_param"	);
	character_physics_support()->movement()		->SetAirControlParam(AirControlParam);

	m_fPickupInfoRadius	= pSettings->r_float(section,"pickup_info_radius");
	m_fSleepTimeFactor	= pSettings->r_float(section,"sleep_time_factor");

	character_physics_support()->in_Load		(section);
	
	//загрузить параметры эффектора
//	LoadShootingEffector	("shooting_effector");
	LoadSleepEffector		("sleep_effector");

	//загрузить параметры смещения firepoint
	m_vMissileOffset	= pSettings->r_fvector3(section,"missile_throw_offset");

	//Weapons				= xr_new<CWeaponList> (this);

	LPCSTR hit_snd_sect = pSettings->r_string(section,"hit_sounds");
	for(int hit_type=0; hit_type<(int)ALife::eHitTypeMax; ++hit_type)
	{
		LPCSTR hit_name = ALife::g_cafHitType2String((ALife::EHitType)hit_type);
		LPCSTR hit_snds = READ_IF_EXISTS(pSettings, r_string, hit_snd_sect, hit_name, "");
		int cnt = _GetItemCount(hit_snds);
		string128		tmp;
		VERIFY			(cnt!=0);
		for(int i=0; i<cnt;++i)
		{
			sndHit[hit_type].push_back		(ref_sound());
			sndHit[hit_type].back().create	(_GetItem(hit_snds,i,tmp),st_Effect,sg_SourceType);
		}
		char buf[256];

		::Sound->create		(sndDie[0],			strconcat(sizeof(buf),buf,*cName(),"\\die0"), st_Effect,SOUND_TYPE_MONSTER_DYING);
		::Sound->create		(sndDie[1],			strconcat(sizeof(buf),buf,*cName(),"\\die1"), st_Effect,SOUND_TYPE_MONSTER_DYING);
		::Sound->create		(sndDie[2],			strconcat(sizeof(buf),buf,*cName(),"\\die2"), st_Effect,SOUND_TYPE_MONSTER_DYING);
		::Sound->create		(sndDie[3],			strconcat(sizeof(buf),buf,*cName(),"\\die3"), st_Effect,SOUND_TYPE_MONSTER_DYING);

		m_HeavyBreathSnd.create	(pSettings->r_string(section,"heavy_breath_snd"), st_Effect,SOUND_TYPE_MONSTER_INJURING);
		m_BloodSnd.create		(pSettings->r_string(section,"heavy_blood_snd"), st_Effect,SOUND_TYPE_MONSTER_INJURING);
	}
	if (this == Level().CurrentEntity()) //--#SM+#-- Сбрасываем режим рендеринга в дефолтный [reset some render flags]
	{
		g_pGamePersistent->m_pGShaderConstants.m_blender_mode.set(0.f, 0.f, 0.f, 0.f);
	}

	// KRodin: это, мне кажется, лишнее.
	//if( psActorFlags.test(AF_PSP) )
	//	cam_Set(eacLookAt);
	//else
		cam_Set(eacFirstEye);

	// sheduler
	shedule.t_min				= shedule.t_max = 1;

	// настройки дисперсии стрельбы
	m_fDispBase					= pSettings->r_float		(section,"disp_base"		 );
	m_fDispBase					= deg2rad(m_fDispBase);

	m_fDispAim					= pSettings->r_float		(section,"disp_aim"		 );
	m_fDispAim					= deg2rad(m_fDispAim);

	m_fDispVelFactor			= pSettings->r_float		(section,"disp_vel_factor"	 );
	m_fDispAccelFactor			= pSettings->r_float		(section,"disp_accel_factor" );
	m_fDispCrouchFactor			= pSettings->r_float		(section,"disp_crouch_factor");
	m_fDispCrouchNoAccelFactor	= pSettings->r_float		(section,"disp_crouch_no_acc_factor");

	LPCSTR							default_outfit = READ_IF_EXISTS(pSettings,r_string,section,"default_outfit",0);
	SetDefaultVisualOutfit			(default_outfit);

	invincibility_fire_shield_1st	= READ_IF_EXISTS(pSettings,r_string,section,"Invincibility_Shield_1st",0);
	invincibility_fire_shield_3rd	= READ_IF_EXISTS(pSettings,r_string,section,"Invincibility_Shield_3rd",0);
//-----------------------------------------
	m_AutoPickUp_AABB				= READ_IF_EXISTS(pSettings,r_fvector3,section,"AutoPickUp_AABB",Fvector().set(0.02f, 0.02f, 0.02f));
	m_AutoPickUp_AABB_Offset		= READ_IF_EXISTS(pSettings,r_fvector3,section,"AutoPickUp_AABB_offs",Fvector().set(0, 0, 0));

	CStringTable string_table;
	//---------------------------------------------------------------------
	m_sHeadShotParticle	= READ_IF_EXISTS(pSettings,r_string,section,"HeadShotParticle",0);

	if ( pSettings->line_exist( section, "lookout_angle" ) ) {
	  m_fLookoutAngle = pSettings->r_float( section, "lookout_angle" );
	  m_fLookoutAngle = deg2rad( m_fLookoutAngle );
	}
	else
	  m_fLookoutAngle = ACTOR_LOOKOUT_ANGLE;

	// Alex ADD: for smooth crouch fix
	CurrentHeight = CameraHeight();	
}

void CActor::PHHit(SHit& H)
{
	m_pPhysics_support->in_Hit(H,!g_Alive());
}

struct playing_pred
{
	IC	bool	operator()			(ref_sound &s)
	{
		return	(NULL != s._feedback() );
	}
};

void	CActor::Hit							(SHit* pHDS)
{
	pHDS->aim_bullet = false;

	SHit HDS = *pHDS;
	if( HDS.hit_type<ALife::eHitTypeBurn || HDS.hit_type >= ALife::eHitTypeMax )
	{
		string256	err;
		sprintf_s		(err, "Unknown/unregistered hit type [%d]", HDS.hit_type);
		R_ASSERT2	(0, err );
	
	}
#ifdef DEBUG
	if(ph_dbg_draw_mask.test(phDbgCharacterControl)) 
	{
		DBG_OpenCashedDraw();
		Fvector to;to.add(Position(),Fvector().mul(HDS.dir,HDS.phys_impulse()));
		DBG_DrawLine(Position(),to,D3DCOLOR_XRGB(124,124,0));
		DBG_ClosedCashedDraw(500);
	}
#endif // DEBUG

	bool bPlaySound = true;
	if (!g_Alive()) bPlaySound = false;

	if (HDS.hit_type == ALife::eHitTypeRadiation)
		HDS.power *= m_fDrugRadProtectionCoeff;

	if (HDS.hit_type == ALife::eHitTypeTelepatic)
		HDS.power *= m_fDrugPsyProtectionCoeff;

	if(	!sndHit[HDS.hit_type].empty()			&& 
		(ALife::eHitTypeTelepatic != HDS.hit_type))
	{
		ref_sound& S = sndHit[HDS.hit_type][Random.randI(sndHit[HDS.hit_type].size())];
		bool b_snd_hit_playing = sndHit[HDS.hit_type].end() != std::find_if(sndHit[HDS.hit_type].begin(), sndHit[HDS.hit_type].end(), playing_pred());

		if(ALife::eHitTypeExplosion == HDS.hit_type)
		{
			if (this == Level().CurrentControlEntity())
			{
				S.set_volume(10.0f);
				if(!m_sndShockEffector){
					m_sndShockEffector = xr_new<SndShockEffector>();
					m_sndShockEffector->Start(this, float(S._handle()->length_ms()), HDS.damage() );
				}
			}
			else
				bPlaySound = false;
		}
		if (bPlaySound && !b_snd_hit_playing) 
		{
			Fvector point		= Position();
			point.y				+= CameraHeight();
			S.play_at_pos		(this, point);
		};
	}

	
	//slow actor, only when he gets hit
	hit_slowmo = conditions().HitSlowmo(pHDS);

	//---------------------------------------------------------------
	if (Level().CurrentViewEntity() == this && HDS.hit_type == ALife::eHitTypeFireWound)
	{
		CObject* pLastHitter = Level().Objects.net_Find(m_iLastHitterID);
		CObject* pLastHittingWeapon = Level().Objects.net_Find(m_iLastHittingWeaponID);
		HitSector(pLastHitter, pLastHittingWeapon);
	};

	if ((mstate_real&mcSprint) && Level().CurrentControlEntity() == this && conditions().DisableSprint(pHDS))
	{
		bool const is_special_burn_hit_2_self = (pHDS->who == this) && (pHDS->boneID == BI_NONE) &&
			(pHDS->hit_type == ALife::eHitTypeBurn);
		if (!is_special_burn_hit_2_self)
			mstate_wishful &= ~mcSprint;
	}

	HitMark(HDS.damage(), HDS.dir, HDS.who, HDS.bone(), HDS.p_in_bone_space, HDS.impulse, HDS.hit_type);

			float hit_power	= HitArtefactsOnBelt(HDS.damage(), HDS.hit_type);

			if (GodMode())
			{
				HDS.power = 0.0f;
//				inherited::Hit(0.f,dir,who,element,position_in_bone_space,impulse, hit_type);
				inherited::Hit(&HDS);
				return;
			}
			else 
			{
				//inherited::Hit		(hit_power,dir,who,element,position_in_bone_space, impulse, hit_type);
				HDS.power = hit_power;
				inherited::Hit(&HDS);
			}
}

void CActor::HitMark	(float P, 
						 Fvector dir,			
						 CObject* who, 
						 s16 element, 
						 Fvector position_in_bone_space, 
						 float impulse,  
						 ALife::EHitType hit_type)
{
	// hit marker
	if ( (hit_type==ALife::eHitTypeFireWound||hit_type==ALife::eHitTypeWound_2) && g_Alive() && Local() && /*(this!=who) && */(Level().CurrentEntity()==this) )	
	{
		HUD().Hit(0, P, dir);

	{
		CEffectorCam* ce = Cameras().GetCamEffector((ECamEffectorType)effFireHit);
		if(!ce)
			{
			int id						= -1;
			Fvector						cam_pos,cam_dir,cam_norm;
			cam_Active()->Get			(cam_pos,cam_dir,cam_norm);
			cam_dir.normalize_safe		();
			dir.normalize_safe			();

			float ang_diff				= angle_difference	(cam_dir.getH(), dir.getH());
			Fvector						cp;
			cp.crossproduct				(cam_dir,dir);
			bool bUp					=(cp.y>0.0f);

			Fvector cross;
			cross.crossproduct			(cam_dir, dir);
			VERIFY						(ang_diff>=0.0f && ang_diff<=PI);

			float _s1 = PI_DIV_8;
			float _s2 = _s1+PI_DIV_4;
			float _s3 = _s2+PI_DIV_4;
			float _s4 = _s3+PI_DIV_4;

			if(ang_diff<=_s1){
				id = 2;
			}else
			if(ang_diff>_s1 && ang_diff<=_s2){
				id = (bUp)?5:7;
			}else
			if(ang_diff>_s2 && ang_diff<=_s3){
				id = (bUp)?3:1;
			}else
			if(ang_diff>_s3 && ang_diff<=_s4){
				id = (bUp)?4:6;
			}else
			if(ang_diff>_s4){
				id = 0;
			}else{
				VERIFY(0);
			}

			string64 sect_name;
			sprintf_s(sect_name,"effector_fire_hit_%d",id);
			AddEffector(this, effFireHit, sect_name, P/100.0f);
			}
		}
	}

}

void CActor::HitSignal(float perc, Fvector& vLocalDir, CObject* who, s16 element)
{
	if (g_Alive()) 
	{

		// stop-motion
		if (character_physics_support()->movement()->Environment()==CPHMovementControl::peOnGround || character_physics_support()->movement()->Environment()==CPHMovementControl::peAtWall)
		{
			Fvector zeroV;
			zeroV.set			(0,0,0);
			character_physics_support()->movement()->SetVelocity(zeroV);
		}
		
		// check damage bone
		Fvector D;
		XFORM().transform_dir(D,vLocalDir);

		float	yaw, pitch;
		D.getHP(yaw,pitch);
		CKinematicsAnimated *tpKinematics = smart_cast<CKinematicsAnimated*>(Visual());
		VERIFY(tpKinematics);
#pragma todo("Dima to Dima : forward-back bone impulse direction has been determined incorrectly!")
		MotionID motion_ID = m_anims->m_normal.m_damage[iFloor(tpKinematics->LL_GetBoneInstance(element).get_param(1) + (angle_difference(r_model_yaw + r_model_yaw_delta,yaw) <= PI_DIV_2 ? 0 : 1))];
		float power_factor = perc/100.f; clamp(power_factor,0.f,1.f);
		VERIFY(motion_ID.valid());
		tpKinematics->PlayFX(motion_ID,power_factor);
		callback(GameObject::eHit)(
			lua_game_object(),
			perc,
			vLocalDir,
			smart_cast<const CGameObject*>(who)->lua_game_object(),
			element
			);
	}
}
void start_tutorial(LPCSTR name);
void CActor::Die(CObject* who)
{
	inherited::Die		(who);

	if (OnServer())
	{	
		xr_vector<CInventorySlot>::iterator I = inventory().m_slots.begin();
		xr_vector<CInventorySlot>::iterator E = inventory().m_slots.end();


		for (u32 slot_idx=0 ; I != E; ++I,++slot_idx)
		{
			if (slot_idx == inventory().GetActiveSlot()) 
			{
				if((*I).m_pIItem)
				{
					(*I).m_pIItem->SetDropManual(TRUE);
				}
			continue;
			}
			else
			{
				CCustomOutfit *pOutfit = smart_cast<CCustomOutfit *> ((*I).m_pIItem);
				if (pOutfit) continue;
			};
			if((*I).m_pIItem) 
				inventory().Ruck((*I).m_pIItem);
		};


		///!!! чистка пояса
		TIItemContainer &l_blist = inventory().m_belt;
		while (!l_blist.empty())	
			inventory().Ruck(l_blist.front());
	}

	cam_Set					(eacFreeLook);
	mstate_wishful	&=		~mcAnyMove;
	mstate_real		&=		~mcAnyMove;

		::Sound->play_at_pos	(sndDie[Random.randI(SND_DIE_COUNT)],this,Position());

		m_HeavyBreathSnd.stop	();
		m_BloodSnd.stop			();		

	start_tutorial		("game_over");
	xr_delete				(m_sndShockEffector);
}

void	CActor::SwitchOutBorder(bool new_border_state)
{
	if(new_border_state)
	{
		callback(GameObject::eExitLevelBorder)(lua_game_object());
	}
	else 
	{
//.		Msg("enter level border");
		callback(GameObject::eEnterLevelBorder)(lua_game_object());
	}
	m_bOutBorder=new_border_state;
}

void CActor::g_Physics(Fvector& _accel, float jump, float dt)
{
	// Correct accel
	Fvector						accel;
	accel.set					(_accel);
	hit_slowmo					-=	dt;
	if (hit_slowmo<0)			hit_slowmo = 0.f;

	accel.mul					(1.f-hit_slowmo);
	
	if(g_Alive())
	{
	if(mstate_real&mcClimb&&!cameras[eacFirstEye]->bClampYaw)accel.set(0.f,0.f,0.f);
	character_physics_support()->movement()->Calculate			(accel,cameras[cam_active]->vDirection,0,jump,dt,false);
	bool new_border_state=character_physics_support()->movement()->isOutBorder();
	if(m_bOutBorder!=new_border_state && Level().CurrentControlEntity() == this)
	{
		SwitchOutBorder(new_border_state);
	}
	character_physics_support()->movement()->GetPosition		(Position());
	character_physics_support()->movement()->bSleep				=false;
	}

	if (Local() && g_Alive()) {
		if (character_physics_support()->movement()->gcontact_Was)
			Cameras().AddCamEffector		(xr_new<CEffectorFall> (character_physics_support()->movement()->gcontact_Power));
		if (!fis_zero(character_physics_support()->movement()->gcontact_HealthLost))	{
			ICollisionDamageInfo* di=character_physics_support()->movement()->CollisionDamageInfo();
			bool b_hit_initiated = di->GetAndResetInitiated();
			Fvector hdir;di->HitDir(hdir);
			SetHitInfo(this, NULL, 0, Fvector().set(0, 0, 0), hdir);
			//				Hit	(m_PhysicMovementControl->gcontact_HealthLost,hdir,di->DamageInitiator(),m_PhysicMovementControl->ContactBone(),di->HitPos(),0.f,ALife::eHitTypeStrike);//s16(6 + 2*::Random.randI(0,2))
			if (Level().CurrentControlEntity() == this)
			{
				SHit HDS = SHit(character_physics_support()->movement()->gcontact_HealthLost,hdir,di->DamageInitiator(),character_physics_support()->movement()->ContactBone(),di->HitPos(),0.f,di->HitType(), 0.0f, b_hit_initiated);
//				Hit(&HDS);

				NET_Packet	l_P;
				HDS.GenHeader(GE_HIT, ID());
				HDS.whoID = di->DamageInitiator()->ID();
				HDS.weaponID = di->DamageInitiator()->ID();
				HDS.Write_Packet(l_P);

				u_EventSend	(l_P);
			}
		}
	}
}
float g_fov = 67.5f; //75.0f - SWM

float CActor::currentFOV()
{
	const auto pWeapon = smart_cast<CWeapon*>(inventory().ActiveItem());	

	if (
		eacFirstEye == cam_active
		&& pWeapon && pWeapon->IsZoomed()
		&& (!pWeapon->ZoomTexture() || (!pWeapon->IsRotatingToZoom() && pWeapon->ZoomTexture()))
		)
		if (Core.Features.test(xrCore::Feature::ogse_wpn_zoom_system))
			return float(atan(tan(g_fov * (0.5 * PI / 180)) / pWeapon->GetZoomFactor()) / (0.5 * PI / 180));
		else
			return pWeapon->GetZoomFactor() * 0.75f;
	else
		return g_fov;
}

void CActor::UpdateCL	()
{
	if(m_feel_touch_characters>0)
	{
		for(xr_vector<CObject*>::iterator it = feel_touch.begin(); it != feel_touch.end(); it++)
		{
			CPhysicsShellHolder	*sh = smart_cast<CPhysicsShellHolder*>(*it);
			if(sh&&sh->character_physics_support())
			{
				sh->character_physics_support()->movement()->UpdateObjectBox(character_physics_support()->movement()->PHCharacter());
			}
		}
	}
	if(m_holder)
		m_holder->UpdateEx( currentFOV() );

	m_snd_noise -= 0.3f*Device.fTimeDelta;

	VERIFY2								(_valid(renderable.xform),*cName());
	inherited::UpdateCL();
	VERIFY2								(_valid(renderable.xform),*cName());
	m_pPhysics_support->in_UpdateCL	();
	VERIFY2								(_valid(renderable.xform),*cName());

	if (g_Alive()) 
		PickupModeUpdate	();	

	PickupModeUpdate_COD();

	m_bZoomAimingMode = false;
	CWeapon* pWeapon = smart_cast<CWeapon*>(inventory().ActiveItem());	

	Device.Statistic->TEST1.Begin		();
	cam_Update(float(Device.dwTimeDelta)/1000.0f, currentFOV());
	Device.Statistic->TEST1.End		();

	if(Level().CurrentEntity() && this->ID()==Level().CurrentEntity()->ID() )
	{
		psHUD_Flags.set( HUD_CROSSHAIR_RT2, true );
		psHUD_Flags.set( HUD_DRAW_RT, true );
	}
	if(pWeapon )
	{
		if(pWeapon->IsZoomed())
		{
			float full_fire_disp = pWeapon->GetFireDispersion(true);

			CEffectorZoomInertion* S = smart_cast<CEffectorZoomInertion*>	(Cameras().GetCamEffector(eCEZoom));
			if(S) S->SetParams(full_fire_disp);

			m_bZoomAimingMode = true;
		}

		if(Level().CurrentEntity() && this->ID()==Level().CurrentEntity()->ID() )
		{
			float fire_disp_full = pWeapon->GetFireDispersion(true);

			if (!Device.m_SecondViewport.IsSVPFrame()) //--#SM+#-- +SecondVP+ Чтобы перекрестие не скакало из за смены FOV (Sin!) [fix for crosshair shaking while SecondVP]
				HUD().SetCrosshairDisp(fire_disp_full, 0.02f);

			HUD().ShowCrosshair(!psHUD_Flags.test(HUD_CROSSHAIR_BUILD) && pWeapon->use_crosshair());

			psHUD_Flags.set( HUD_CROSSHAIR_RT2, pWeapon->show_crosshair() );
			psHUD_Flags.set( HUD_DRAW_RT,		pWeapon->show_indicators() );

			// Обновляем двойной рендер от оружия [Update SecondVP with weapon data]
			pWeapon->UpdateSecondVP();	//--#SM+#-- +SecondVP+
			
			// Обновляем информацию об оружии в шейдерах
			g_pGamePersistent->m_pGShaderConstants.hud_params.x = pWeapon->GetZRotatingFactor(); //--#SM+#--
			g_pGamePersistent->m_pGShaderConstants.hud_params.y = pWeapon->GetSecondVP_FovFactor(); //--#SM+#--
		}

	}
	else
	{
		if(Level().CurrentEntity() && this->ID()==Level().CurrentEntity()->ID() )
		{
			HUD().SetCrosshairDisp(0.f);
			HUD().ShowCrosshair(false);

			// Очищаем информацию об оружии в шейдерах
			g_pGamePersistent->m_pGShaderConstants.hud_params.set(0.f, 0.f, 0.f, 0.f); //--#SM+#--

			// Отключаем второй вьюпорт  [Turn off SecondVP]
			// + CWeapon::UpdateSecondVP();
			Device.m_SecondViewport.SetSVPActive(false); //--#SM+#-- +SecondVP+
		}
	}

	UpdateDefferedMessages();

	if (g_Alive()) 
		CStepManager::update();

	spatial.type |=STYPE_REACTTOSOUND;

	if(m_sndShockEffector)
	{
		if (this == Level().CurrentViewEntity())
		{
			m_sndShockEffector->Update();

			if(!m_sndShockEffector->InWork() || !g_Alive())
				xr_delete(m_sndShockEffector);
		}
		else
			xr_delete(m_sndShockEffector);
	}
}

#define TASKS_UPDATE_TIME 500u
float	NET_Jump = 0;
void CActor::shedule_Update	(u32 DT)
{
	setSVU(OnServer());

	//установить режим показа HUD для текущего активного слота
	CHudItem* pHudItem = smart_cast<CHudItem*>(inventory().ActiveItem());	
	if(pHudItem) 
		pHudItem->SetHUDmode(HUDview());

	//обновление инвентаря
	if ( !updated )
          inventory().RestoreBeltOrder();
	UpdateInventoryOwner			(DT);

	static u32 tasks_update_time = 0;
	if ( tasks_update_time > TASKS_UPDATE_TIME ) {
	  tasks_update_time = 0;
	  GameTaskManager().UpdateTasks();
	}
	else {
	  tasks_update_time += DT;
	}

	if(m_holder || !getEnabled() || !Ready())
	{
		m_sDefaultObjAction = nullptr;
		inherited::shedule_Update		(DT);
	
/*		if (OnServer())
		{
			Check_Weapon_ShowHideState();
		};	
*/
		return;
	}

	// 
	clamp					(DT,0u,100u);
	float	dt	 			=  float(DT)/1000.f;

	// Check controls, create accel, prelimitary setup "mstate_real"
	
	//----------- for E3 -----------------------------
//	if (Local() && (OnClient() || Level().CurrentEntity()==this))
	if (Level().CurrentControlEntity() == this && (!Level().IsDemoPlay() || Level().IsServerDemo()))
	//------------------------------------------------
	{
		g_cl_CheckControls		(mstate_wishful,NET_SavedAccel,NET_Jump,dt);
		{
			/*
			if (mstate_real & mcJump)
			{
				NET_Packet	P;
				u_EventGen(P, GE_ACTOR_JUMPING, ID());
				P.w_sdir(NET_SavedAccel);
				P.w_float(NET_Jump);
				u_EventSend(P);
			}
			*/
		}
		g_cl_Orientate			(mstate_real,dt);
		g_Orientate				(mstate_real,dt);

		g_Physics				(NET_SavedAccel,NET_Jump,dt);
		
		g_cl_ValidateMState		(dt,mstate_wishful);
		g_SetAnimation			(mstate_real);
		
		// Check for game-contacts
		Fvector C; float R;		
		//m_PhysicMovementControl->GetBoundingSphere	(C,R);
		
		Center(C);
		R=Radius();
		feel_touch_update		(C,R);

		// Dropping
		if (b_DropActivated)	{
			f_DropPower			+= dt*0.1f;
			clamp				(f_DropPower,0.f,1.f);
		} else {
			f_DropPower			= 0.f;
		}
		if (!Level().IsDemoPlay())
		{		
		//-----------------------------------------------------
		mstate_wishful &=~mcAccel;
		mstate_wishful &=~mcLStrafe;
		mstate_wishful &=~mcRStrafe;
		mstate_wishful &=~mcLLookout;
		mstate_wishful &=~mcRLookout;
		mstate_wishful &=~mcFwd;
		mstate_wishful &=~mcBack;
		extern bool g_bAutoClearCrouch;
		if (g_bAutoClearCrouch)
			mstate_wishful &=~mcCrouch;
		//-----------------------------------------------------
		}
	}
	else 
	{
		make_Interpolation();
	
		if (NET.size())
		{
			
//			NET_SavedAccel = NET_Last.p_accel;
//			mstate_real = mstate_wishful = NET_Last.mstate;

			g_sv_Orientate				(mstate_real,dt			);
			g_Orientate					(mstate_real,dt			);
			g_Physics					(NET_SavedAccel,NET_Jump,dt	);			
			if (!m_bInInterpolation)
				g_cl_ValidateMState			(dt,mstate_wishful);
			g_SetAnimation				(mstate_real);

			if (NET_Last.mstate & mcCrouch)
			{
				if (isActorAccelerated(mstate_real, IsZoomAimingMode()))
					character_physics_support()->movement()->ActivateBox(1, true);
				else
					character_physics_support()->movement()->ActivateBox(2, true);
			}
			else 
				character_physics_support()->movement()->ActivateBox(0, true);
		}	
		mstate_old = mstate_real;
	}

	if (this == Level().CurrentViewEntity())
	{
		UpdateMotionIcon		(mstate_real);
	};

	NET_Jump = 0;


	inherited::shedule_Update	(DT);

	//эффектор включаемый при ходьбе
	if (!pCamBobbing)
	{
		pCamBobbing = xr_new<CEffectorBobbing>	();
		Cameras().AddCamEffector			(pCamBobbing);
	}
	pCamBobbing->SetState						(mstate_real, conditions().IsLimping(), IsZoomAimingMode());

	//звук тяжелого дыхания при уталости и хромании
	if(this==Level().CurrentControlEntity())
	{
		if(conditions().IsLimping() && g_Alive())
		{
			if(!m_HeavyBreathSnd._feedback())
			{
				m_HeavyBreathSnd.play_at_pos(this, Fvector().set(0,ACTOR_HEIGHT,0), sm_Looped | sm_2D);
			}else{
				m_HeavyBreathSnd.set_position(Fvector().set(0,ACTOR_HEIGHT,0));
			}
		}else if(m_HeavyBreathSnd._feedback()){
			m_HeavyBreathSnd.stop		();
		}

		float bs = conditions().BleedingSpeed();
		if(bs>0.6f)
		{
			Fvector snd_pos;
			snd_pos.set(0,ACTOR_HEIGHT,0);
			if(!m_BloodSnd._feedback())
				m_BloodSnd.play_at_pos(this, snd_pos, sm_Looped | sm_2D);
			else
				m_BloodSnd.set_position(snd_pos);

			float v = bs+0.25f;

			m_BloodSnd.set_volume	(v);
		}else{
			if(m_BloodSnd._feedback())
				m_BloodSnd.stop();
		}

		if(!g_Alive()&&m_BloodSnd._feedback())
				m_BloodSnd.stop();
	}
	
	//если в режиме HUD, то сама модель актера не рисуется
	if(!character_physics_support()->IsRemoved())
										setVisible				(!HUDview	());
	//что актер видит перед собой
	collide::rq_result& RQ = HUD().GetCurrentRayQuery();
	

	if(!input_external_handler_installed() && RQ.O && RQ.range<inventory().GetTakeDist()) 
	{
		m_pObjectWeLookingAt  = smart_cast<CGameObject*>(RQ.O);
		m_pUsableObject	      = smart_cast<CUsableScriptObject*>(RQ.O);
		m_pInvBoxWeLookingAt  = smart_cast<IInventoryBox*>(RQ.O);
		inventory().m_pTarget = smart_cast<PIItem>(RQ.O);
		m_pPersonWeLookingAt  = smart_cast<CInventoryOwner*>(RQ.O);
		m_pVehicleWeLookingAt = smart_cast<CHolderCustom*>(RQ.O);
		auto pEntityAlive     = smart_cast<CEntityAlive*>(RQ.O);
		auto ph_shell_holder  = smart_cast<CPhysicsShellHolder*>(RQ.O);

		bool b_allow_drag = ph_shell_holder && ph_shell_holder->ActorCanCapture();

		if (HUD().GetUI()->MainInputReceiver() || m_holder || character_physics_support()->movement()->PHCapture())
		{
			m_sDefaultObjAction = nullptr;
		}
		else if (m_pUsableObject && m_pUsableObject->tip_text())
		{
			m_sDefaultObjAction = CStringTable().translate(m_pUsableObject->tip_text()).c_str();
		}
		else if (pEntityAlive)
		{
			if (m_pPersonWeLookingAt && pEntityAlive->g_Alive())
			{
				m_sDefaultObjAction = m_sCharacterUseAction;
			}
			else if (!pEntityAlive->g_Alive())
			{
				if (b_allow_drag)
					m_sDefaultObjAction = m_sDeadCharacterUseOrDragAction;
				else
					m_sDefaultObjAction = m_sDeadCharacterUseAction;
			}
		}
		else if (m_pVehicleWeLookingAt)
		{
			m_sDefaultObjAction = m_sCarCharacterUseAction;
		}
		else if (inventory().m_pTarget && inventory().m_pTarget->CanTake())
		{
			if (b_allow_drag)
				m_sDefaultObjAction = m_sInventoryItemUseOrDragAction;
			else
				m_sDefaultObjAction = m_sInventoryItemUseAction;
		}
		else if (b_allow_drag)
		{
			m_sDefaultObjAction = m_sGameObjectDragAction;
		}
		else
		{
			m_sDefaultObjAction = nullptr;
		}
	}
	else
	{
		inventory().m_pTarget	= NULL;
		m_pPersonWeLookingAt	= NULL;
		m_sDefaultObjAction		= nullptr;
		m_pUsableObject			= NULL;
		m_pObjectWeLookingAt	= NULL;
		m_pVehicleWeLookingAt	= NULL;
		m_pInvBoxWeLookingAt	= NULL;
	}

//	UpdateSleep									();

	//для свойст артефактов, находящихся на поясе
	UpdateArtefactsOnBelt						();
	m_pPhysics_support->in_shedule_Update		(DT);

	updated = true;
};
#include "debug_renderer.h"
void CActor::renderable_Render	()
{
	inherited::renderable_Render			();
	if ((cam_active==eacFirstEye &&									// first eye cam
		::Render->get_generation() == ::Render->GENERATION_R2 &&	// R2
		::Render->active_phase() ==	1)								// shadow map rendering on R2
		
		||

		!(IsFocused() &&
		(cam_active==eacFirstEye) &&
		((!m_holder) || (m_holder && m_holder->allowWeapon() && m_holder->HUDView())))
		)
		CInventoryOwner::renderable_Render	();
}

BOOL CActor::renderable_ShadowGenerate	() 
{
	if(m_holder)
		return FALSE;
	
	return inherited::renderable_ShadowGenerate();
}



void CActor::g_PerformDrop	( )
{
	b_DropActivated			= FALSE;

	PIItem pItem			= inventory().ActiveItem();
	if (0==pItem)			return;

	u32 s					= inventory().GetActiveSlot();
	if(inventory().m_slots[s].m_bPersistent)	return;

	pItem->SetDropManual	(TRUE);
}


#ifdef DEBUG
extern	BOOL	g_ShowAnimationInfo		;
#endif // DEBUG
// HUD
void CActor::OnHUDDraw	(CCustomHUD* /**hud/**/)
{
	CHudItem* pHudItem = smart_cast<CHudItem*>(inventory().ActiveItem());
	if (pHudItem && pHudItem->GetHUDmode())
//	if(inventory().ActiveItem()  ) 
	{
		inventory().ActiveItem()->renderable_Render();
	}

#if 0//ndef NDEBUG
	if (Level().CurrentControlEntity() == this && g_ShowAnimationInfo)
	{
		string128 buf;
		HUD().Font().pFontStat->SetColor	(0xffffffff);
		HUD().Font().pFontStat->OutSet		(170,530);
		HUD().Font().pFontStat->OutNext	("Position:      [%3.2f, %3.2f, %3.2f]",VPUSH(Position()));
		HUD().Font().pFontStat->OutNext	("Velocity:      [%3.2f, %3.2f, %3.2f]",VPUSH(m_PhysicMovementControl->GetVelocity()));
		HUD().Font().pFontStat->OutNext	("Vel Magnitude: [%3.2f]",m_PhysicMovementControl->GetVelocityMagnitude());
		HUD().Font().pFontStat->OutNext	("Vel Actual:    [%3.2f]",m_PhysicMovementControl->GetVelocityActual());
		switch (m_PhysicMovementControl->Environment())
		{
		case CPHMovementControl::peOnGround:	strcpy(buf,"ground");			break;
		case CPHMovementControl::peInAir:		strcpy(buf,"air");				break;
		case CPHMovementControl::peAtWall:		strcpy(buf,"wall");				break;
		}
		HUD().Font().pFontStat->OutNext	(buf);

		if (IReceived != 0)
		{
			float Size = 0;
			Size = HUD().Font().pFontStat->GetSize();
			HUD().Font().pFontStat->SetSize(Size*2);
			HUD().Font().pFontStat->SetColor	(0xffff0000);
			HUD().Font().pFontStat->OutNext ("Input :		[%3.2f]", ICoincidenced/IReceived * 100.0f);
			HUD().Font().pFontStat->SetSize(Size);
		};
	};
#endif
}

void CActor::RenderIndicator			(Fvector dpos, float r1, float r2, ref_shader IndShader)
{
	if (!g_Alive()) return;

	u32			dwOffset = 0,dwCount = 0;
	FVF::LIT* pv_start				= (FVF::LIT*)RCache.Vertex.Lock(4,hFriendlyIndicator->vb_stride,dwOffset);
	FVF::LIT* pv					= pv_start;
	// base rect

	CBoneInstance& BI = smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(u16(m_head));
	Fmatrix M;
	smart_cast<CKinematics*>(Visual())->CalculateBones	();
	M.mul						(XFORM(),BI.mTransform);

	Fvector pos = M.c; pos.add(dpos);
	const Fvector& T        = Device.vCameraTop;
	const Fvector& R        = Device.vCameraRight;
	Fvector Vr, Vt;
	Vr.x            = R.x*r1;
	Vr.y            = R.y*r1;
	Vr.z            = R.z*r1;
	Vt.x            = T.x*r2;
	Vt.y            = T.y*r2;
	Vt.z            = T.z*r2;

	Fvector         a,b,c,d;
	a.sub           (Vt,Vr);
	b.add           (Vt,Vr);
	c.invert        (a);
	d.invert        (b);
	pv->set         (d.x+pos.x,d.y+pos.y,d.z+pos.z, 0xffffffff, 0.f,1.f);        pv++;
	pv->set         (a.x+pos.x,a.y+pos.y,a.z+pos.z, 0xffffffff, 0.f,0.f);        pv++;
	pv->set         (c.x+pos.x,c.y+pos.y,c.z+pos.z, 0xffffffff, 1.f,1.f);        pv++;
	pv->set         (b.x+pos.x,b.y+pos.y,b.z+pos.z, 0xffffffff, 1.f,0.f);        pv++;
	// render	
	dwCount 				= u32(pv-pv_start);
	RCache.Vertex.Unlock	(dwCount,hFriendlyIndicator->vb_stride);

	RCache.set_xform_world		(Fidentity);
	RCache.set_Shader			(IndShader);
	RCache.set_Geometry			(hFriendlyIndicator);
	RCache.Render	   			(D3DPT_TRIANGLESTRIP,dwOffset,0, dwCount, 0, 2);
};

static float mid_size = 0.097f;
static float fontsize = 15.0f;
static float upsize	= 0.33f;
void CActor::RenderText				(LPCSTR Text, Fvector dpos, float* pdup, u32 color)
{
	if (!g_Alive()) return;
	
	CBoneInstance& BI = smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(u16(m_head));
	Fmatrix M;
	smart_cast<CKinematics*>(Visual())->CalculateBones	();
	M.mul						(XFORM(),BI.mTransform);
	//------------------------------------------------
	Fvector v0, v1;
	v0.set(M.c); v1.set(M.c);
	Fvector T        = Device.vCameraTop;
	v1.add(T);

	Fvector v0r, v1r;
	Device.mFullTransform.transform(v0r,v0);
	Device.mFullTransform.transform(v1r,v1);
	float size = v1r.distance_to(v0r);
	CGameFont* pFont = HUD().Font().pFontArial14;
	if (!pFont) return;
//	float OldFontSize = pFont->GetHeight	();	
	float delta_up = 0.0f;
	if (size < mid_size) delta_up = upsize;
	else delta_up = upsize*(mid_size/size);
	dpos.y += delta_up;
	if (size > mid_size) size = mid_size;
//	float NewFontSize = size/mid_size * fontsize;
	//------------------------------------------------
	M.c.y += dpos.y;

	Fvector4 v_res;	
	Device.mFullTransform.transform(v_res,M.c);

	if (v_res.z < 0 || v_res.w < 0)	return;
	if (v_res.x < -1.f || v_res.x > 1.f || v_res.y<-1.f || v_res.y>1.f) return;

	float x = (1.f + v_res.x)/2.f * (Device.dwWidth);
	float y = (1.f - v_res.y)/2.f * (Device.dwHeight);

	pFont->SetAligment	(CGameFont::alCenter);
	pFont->SetColor		(color);
//	pFont->SetHeight	(NewFontSize);
	pFont->Out			(x,y,Text);
	//-------------------------------------------------
//	pFont->SetHeight(OldFontSize);
	*pdup = delta_up;
};

void CActor::SetPhPosition(const Fmatrix &transform)
{
	if(!m_pPhysicsShell){ 
		character_physics_support()->movement()->SetPosition(transform.c);
	}
	//else m_phSkeleton->S
}

void CActor::ForceTransform(const Fmatrix& m)
{
	if(!g_Alive())				return;
	XFORM().set					(m);
	if(character_physics_support()->movement()->CharacterExist()) character_physics_support()->movement()->EnableCharacter	();
	character_physics_support()->set_movement_position( m.c );
	character_physics_support()->movement()->SetVelocity		(0,0,0);
}

ENGINE_API extern float		psHUD_FOV;
float CActor::Radius()const
{ 
	float R		= inherited::Radius();
	CWeapon* W	= smart_cast<CWeapon*>(inventory().ActiveItem());
	if (W) R	+= W->Radius();
	//	if (HUDview()) R *= 1.f/psHUD_FOV;
	return R;
}

bool		CActor::use_bolts				() const
{
	return CInventoryOwner::use_bolts();
};

int		g_iCorpseRemove = 1;

bool  CActor::NeedToDestroyObject() const
{
	if(g_Alive())				return false;
	if(g_iCorpseRemove == -1)	return false;
	if(g_iCorpseRemove == 0 && m_bAllowDeathRemove) return true;
	return (TimePassedAfterDeath()>m_dwBodyRemoveTime && m_bAllowDeathRemove);
}

ALife::_TIME_ID	 CActor::TimePassedAfterDeath()	const
{
	if(!g_Alive())
		return Level().timeServer() - GetLevelDeathTime();
	else
		return 0;
}


void CActor::OnItemTake			(CInventoryItem *inventory_item)
{
	CInventoryOwner::OnItemTake(inventory_item);
	if (OnClient()) return;
}

void CActor::OnItemDrop			(CInventoryItem *inventory_item)
{
	CInventoryOwner::OnItemDrop(inventory_item);

	if ( inventory_item->m_eItemPlace == eItemPlaceBelt )
		UpdateArtefactPanel();
}


void CActor::OnItemDropUpdate ()
{
	CInventoryOwner::OnItemDropUpdate		();

	TIItemContainer::iterator				I = inventory().m_all.begin();
	TIItemContainer::iterator				E = inventory().m_all.end();
	
	for ( ; I != E; ++I)
		if( !(*I)->IsInvalid() && !attached(*I))
			attach(*I);
}


void CActor::OnItemRuck		(CInventoryItem *inventory_item, EItemPlace previous_place)
{
	CInventoryOwner::OnItemRuck(inventory_item, previous_place);

	if ( previous_place == eItemPlaceBelt )
		UpdateArtefactPanel();
}
void CActor::OnItemBelt		(CInventoryItem *inventory_item, EItemPlace previous_place)
{
	CInventoryOwner::OnItemBelt(inventory_item, previous_place);

	UpdateArtefactPanel();
}

void CActor::OnItemSlot(CInventoryItem* inventory_item, EItemPlace previous_place)
{
	CInventoryOwner::OnItemSlot(inventory_item, previous_place);

	if (previous_place == eItemPlaceBelt)
		UpdateArtefactPanel();
}


void CActor::UpdateArtefactPanel()
{
	if (Level().CurrentViewEntity() && Level().CurrentViewEntity() == this) //Оно надо вообще без мультиплеера?
		HUD().GetUI()->UIMainIngameWnd->m_artefactPanel->InitIcons(inventory().m_belt);
}

void CActor::ApplyArtefactEffects(ActorRestoreParams& r, CArtefact*	artefact)
{
  float k = ( Core.Features.test(xrCore::Feature::af_zero_condition) && fis_zero( artefact->GetCondition() ) ) ? 0.f : 1.f;

  r.BleedingRestoreSpeed += artefact->m_fBleedingRestoreSpeed * k;
  r.HealthRestoreSpeed += artefact->m_fHealthRestoreSpeed * k;
  r.PowerRestoreSpeed += artefact->m_fPowerRestoreSpeed * k;

  if (Core.Features.test(xrCore::Feature::af_satiety))
    r.SatietyRestoreSpeed += artefact->m_fSatietyRestoreSpeed * k;

  if (Core.Features.test(xrCore::Feature::af_psy_health)) {
	  if (Core.Features.test(xrCore::Feature::objects_radioactive)) {
		  if (artefact->PsyHealthRestoreSpeed() > 0)
			  r.PsyHealthRestoreSpeed += artefact->PsyHealthRestoreSpeed() * k;
	  }
	  else {
		  r.PsyHealthRestoreSpeed += artefact->PsyHealthRestoreSpeed() * k;
	  }
  }
  if (Core.Features.test(xrCore::Feature::objects_radioactive)) {
	  if (artefact->RadiationRestoreSpeed() < 0)
		  r.RadiationRestoreSpeed += artefact->RadiationRestoreSpeed() * k;
  }
  else {

  if ( artefact->RadiationRestoreSpeed() > 0 && Core.Features.test( xrCore::Feature::af_radiation_immunity_mod ) ) {
    float new_rs = HitArtefactsOnBelt( artefact->RadiationRestoreSpeed(), ALife::eHitTypeRadiation, true );
    if ( new_rs > artefact->RadiationRestoreSpeed() )
      r.RadiationRestoreSpeed += new_rs * k;
    else
      r.RadiationRestoreSpeed += artefact->RadiationRestoreSpeed() * k;
  }
  else
    r.RadiationRestoreSpeed += artefact->RadiationRestoreSpeed() * k;

  }
}

ActorRestoreParams CActor::ActiveArtefactsOnBelt()
{
	ActorRestoreParams r;

	Memory.mem_fill(&r, 0, sizeof(r));

	for (TIItemContainer::iterator it = inventory().m_belt.begin();
		inventory().m_belt.end() != it; ++it)
	{
		CArtefact*	artefact = smart_cast<CArtefact*>(*it);
		if (artefact)
		{
      ApplyArtefactEffects(r, artefact);
		}
	}

    if (Core.Features.test(xrCore::Feature::objects_radioactive)) {

	auto &map_all = inventory().m_all;
	for (TIItemContainer::iterator it = map_all.begin(); map_all.end() != it; ++it) {
		CInventoryItem *obj = smart_cast<CInventoryItem*>(*it);

		float k = ( Core.Features.test(xrCore::Feature::af_zero_condition) && fis_zero( obj->GetCondition() ) ) ? 0.f : 1.f;

		if (Core.Features.test(xrCore::Feature::af_psy_health)) {
			// только уменьшение пси здоровоья
			if (obj->PsyHealthRestoreSpeed() < 0) {
				r.PsyHealthRestoreSpeed += obj->PsyHealthRestoreSpeed() * k;
			}
		}
		// только увеличение радиации
		if ( obj->RadiationRestoreSpeed() > 0 ) {
		  if ( Core.Features.test( xrCore::Feature::af_radiation_immunity_mod ) ) {
		    float new_rs = HitArtefactsOnBelt( obj->RadiationRestoreSpeed(), ALife::eHitTypeRadiation, true );
		    if ( new_rs > obj->RadiationRestoreSpeed() )
		      r.RadiationRestoreSpeed += new_rs * k;
		    else
		      r.RadiationRestoreSpeed += obj->RadiationRestoreSpeed() * k;
		  }
		  else
		    r.RadiationRestoreSpeed += obj->RadiationRestoreSpeed() * k;
		}
	}

	}

  if ( Core.Features.test(xrCore::Feature::outfit_af) ) {

  PIItem helm = inventory().m_slots[HELMET_SLOT].m_pIItem;
  if (helm)
  {
    CArtefact*	helmet = smart_cast<CArtefact*>(helm);
    if (helmet)
    {
      ApplyArtefactEffects(r, helmet);
    }
  }

	PIItem outfit_item = inventory().m_slots[OUTFIT_SLOT].m_pIItem;
	if (outfit_item) {
		CCustomOutfit *outfit = smart_cast<CCustomOutfit*>(outfit_item);
		if (outfit) {
			float k = ( Core.Features.test(xrCore::Feature::af_zero_condition) && fis_zero( outfit->GetCondition() ) ) ? 0.f : 1.f;

			r.BleedingRestoreSpeed += outfit->m_fBleedingRestoreSpeed * k;
			r.HealthRestoreSpeed += outfit->m_fHealthRestoreSpeed * k;
			r.PowerRestoreSpeed += outfit->m_fPowerRestoreSpeed * k;

			if (Core.Features.test(xrCore::Feature::af_satiety))
				r.SatietyRestoreSpeed += outfit->m_fSatietyRestoreSpeed * k;

			if (Core.Features.test(xrCore::Feature::af_psy_health)) {
				if (Core.Features.test(xrCore::Feature::objects_radioactive)) {
					if (outfit->PsyHealthRestoreSpeed() > 0)
						r.PsyHealthRestoreSpeed += outfit->PsyHealthRestoreSpeed() * k;
				}
				else {
					r.PsyHealthRestoreSpeed += outfit->PsyHealthRestoreSpeed() * k;
				}
			}

			if (Core.Features.test(xrCore::Feature::objects_radioactive)) {
				if (outfit->RadiationRestoreSpeed() < 0)
					r.RadiationRestoreSpeed += outfit->RadiationRestoreSpeed() * k;
			}
			else {
				r.RadiationRestoreSpeed += outfit->RadiationRestoreSpeed() * k;
			}
		}
	}

  }

  return r;
}

#define ARTEFACTS_UPDATE_TIME 0.100f

void CActor::UpdateArtefactsOnBelt()
{
	static float update_time = 0;

	float f_update_time = 0;

	if(update_time<ARTEFACTS_UPDATE_TIME)
	{
		update_time += conditions().fdelta_time();
		return;
	}
	else
	{
		f_update_time	= update_time;
		update_time		= 0.0f;
	}

	auto effects = ActiveArtefactsOnBelt();

	if (!fis_zero(effects.BleedingRestoreSpeed))
		conditions().ChangeBleeding(effects.BleedingRestoreSpeed * f_update_time);

	if (!fis_zero(effects.HealthRestoreSpeed))
		conditions().ChangeHealth(effects.HealthRestoreSpeed * f_update_time);

	if (!fis_zero(effects.PowerRestoreSpeed))
		conditions().ChangePower(effects.PowerRestoreSpeed * f_update_time);

	if (!fis_zero(effects.SatietyRestoreSpeed))
		conditions().ChangeSatiety(effects.SatietyRestoreSpeed * f_update_time);

	if (!fis_zero(effects.PsyHealthRestoreSpeed))
		conditions().ChangePsyHealth(effects.PsyHealthRestoreSpeed * f_update_time);

	if (!fis_zero(effects.RadiationRestoreSpeed))
		conditions().ChangeRadiation(effects.RadiationRestoreSpeed * f_update_time);

	callback( GameObject::eUpdateArtefactsOnBelt )( f_update_time );
}

float	CActor::HitArtefactsOnBelt( float hit_power, ALife::EHitType hit_type, bool belt_only ) {
	float res_hit_power_k		= 1.0f;
	float _af_count				= 0.0f;
	for(TIItemContainer::iterator it = inventory().m_belt.begin(); 
		inventory().m_belt.end() != it; ++it) 
	{
		CArtefact*	artefact = smart_cast<CArtefact*>(*it);
		if(artefact){
			if ( !Core.Features.test(xrCore::Feature::af_zero_condition) || !fis_zero( artefact->GetCondition() ) ) {
				res_hit_power_k	+= artefact->m_ArtefactHitImmunities.AffectHit( 1.0f, hit_type );
				_af_count += 1.0f;
			}
		}
	}
	// учет иммунитета от шлема
	PIItem helm = inventory().m_slots[HELMET_SLOT].m_pIItem;
	if ( helm && !belt_only ) {
		CArtefact*	helmet = smart_cast<CArtefact*>(helm);
		if (helmet)
		{
			if ( !Core.Features.test(xrCore::Feature::af_zero_condition) || !fis_zero( helmet->GetCondition() ) ) {
				res_hit_power_k += helmet->m_ArtefactHitImmunities.AffectHit( 1.0f, hit_type );
				_af_count += 1.0f;
			}
		}
	}

	res_hit_power_k -= _af_count;

	return res_hit_power_k > 0 ? res_hit_power_k * hit_power : 0;
}

void	CActor::SetZoomRndSeed		(s32 Seed)
{
	if (0 != Seed) m_ZoomRndSeed = Seed;
	else m_ZoomRndSeed = s32(Level().timeServer_Async());
};

void	CActor::SetShotRndSeed		(s32 Seed)
{
	if (0 != Seed) m_ShotRndSeed = Seed;
	else m_ShotRndSeed = s32(Level().timeServer_Async());
};

Fvector CActor::GetMissileOffset	() const
{
	return m_vMissileOffset;
}

void CActor::SetMissileOffset		(const Fvector &vNewOffset)
{
	m_vMissileOffset.set(vNewOffset);
}

void CActor::spawn_supplies			()
{
	inherited::spawn_supplies		();
	CInventoryOwner::spawn_supplies	();
}


void CActor::AnimTorsoPlayCallBack(CBlend* B)
{
	CActor* actor		= (CActor*)B->CallbackParam;
	actor->m_bAnimTorsoPlayed = FALSE;
}

void CActor::SetActorVisibility(u16 who, float value)
{
	CUIMotionIcon		&motion_icon	= HUD().GetUI()->UIMainIngameWnd->MotionIcon();
	motion_icon.SetActorVisibility		(who, value);
}

void CActor::UpdateMotionIcon(u32 mstate_rl)
{
	CUIMotionIcon		&motion_icon=HUD().GetUI()->UIMainIngameWnd->MotionIcon();
	if(mstate_rl&mcClimb)
	{
		motion_icon.ShowState(CUIMotionIcon::stClimb);
	}
	else
	{
		if(mstate_rl&mcCrouch)
		{
			if (!isActorAccelerated(mstate_rl, IsZoomAimingMode()))
				motion_icon.ShowState(CUIMotionIcon::stCreep);
			else
				motion_icon.ShowState(CUIMotionIcon::stCrouch);
		}
		else
		if(mstate_rl&mcSprint)
				motion_icon.ShowState(CUIMotionIcon::stSprint);
		else
		if(mstate_rl&mcAnyMove && isActorAccelerated(mstate_rl, IsZoomAimingMode()))
			motion_icon.ShowState(CUIMotionIcon::stRun);
		else
			motion_icon.ShowState(CUIMotionIcon::stNormal);
	}

/*
						stNormal, --
						stCrouch, --
						stCreep,  --
						stClimb,  --
						stRun,    --
						stSprint, --
*/
}


CPHDestroyable*	CActor::ph_destroyable	()
{
	return smart_cast<CPHDestroyable*>(character_physics_support());
}

CEntityConditionSimple *CActor::create_entity_condition	(CEntityConditionSimple* ec)
{
	if(!ec)
		m_entity_condition		= xr_new<CActorCondition>(this);
	else
		m_entity_condition		= smart_cast<CActorCondition*>(ec);
	
	return		(inherited::create_entity_condition(m_entity_condition));
}

DLL_Pure *CActor::_construct			()
{
	m_pPhysics_support				=	xr_new<CCharacterPhysicsSupport>(CCharacterPhysicsSupport::etActor,this);
	CEntityAlive::_construct		();
	CInventoryOwner::_construct		();
	CStepManager::_construct		();
	
	return							(this);
}

bool CActor::use_center_to_aim			() const
{
	return							(!(mstate_real&mcCrouch));
}



bool CActor::can_attach			(const CInventoryItem *inventory_item) const
{
	const CAttachableItem	*item = smart_cast<const CAttachableItem*>(inventory_item);
	if (!item || /*!item->enabled() ||*/ !item->can_be_attached())
		return			(false);

	//можно ли присоединять объекты такого типа
	if( m_attach_item_sections.end() == std::find(m_attach_item_sections.begin(),m_attach_item_sections.end(),inventory_item->object().cNameSect()) )
		return false;

	//если уже есть присоединненый объет такого типа 
	if(attached(inventory_item->object().cNameSect()))
		return false;

	return true;
}

void CActor::OnDifficultyChanged	()
{
	// immunities
	VERIFY(g_SingleGameDifficulty>=egdNovice && g_SingleGameDifficulty<=egdMaster); 
	LPCSTR diff_name				= get_token_name(difficulty_type_token, g_SingleGameDifficulty);
	string128						tmp;
	strconcat						(sizeof(tmp),tmp,"actor_immunities_",diff_name);
	conditions().LoadImmunities		(tmp,pSettings);
	// hit probability
	strconcat						(sizeof(tmp),tmp,"hit_probability_",diff_name);
	hit_probability					= pSettings->r_float(*cNameSect(),tmp);
}

CVisualMemoryManager	*CActor::visual_memory	() const
{
	return							(&memory().visual());
}

float		CActor::GetCarryWeight() const
{
	float add = 0;
	CPHCapture* capture = character_physics_support()->movement()->PHCapture();
	if (capture && capture->taget_object())
	{
		CPhysicsShellHolder *obj = capture->taget_object();
		CInventoryOwner *io = smart_cast<CInventoryOwner *> (obj);
		if (io)
			add += io->GetCarryWeight();

		add += READ_IF_EXISTS(pSettings, r_float, *obj->cNameSect(), "ph_mass", 0) * 0.1f;
	}
	
	return CInventoryOwner::GetCarryWeight() + add;
}

float		CActor::GetMass				()
{
	return g_Alive()?character_physics_support()->movement()->GetMass():m_pPhysicsShell?m_pPhysicsShell->getMass():0; 
}

bool CActor::is_on_ground()
{
	return (character_physics_support()->movement()->Environment() != CPHMovementControl::peInAir);
}

CCustomOutfit* CActor::GetOutfit() const
{
	PIItem _of	= inventory().m_slots[OUTFIT_SLOT].m_pIItem;
	return _of?smart_cast<CCustomOutfit*>(_of):NULL;
}


void CActor::block_action(EGameActions cmd)
{
	if (m_blocked_actions.find(cmd) == m_blocked_actions.end() )
	{
		m_blocked_actions[cmd] = true;
	}
}

void CActor::unblock_action(EGameActions cmd)
{
	auto iter = m_blocked_actions.find(cmd);
	if (iter != m_blocked_actions.end())
	{
		m_blocked_actions.erase(iter);
	}
}

bool CActor::is_actor_normal() {
	return mstate_real & (mcAnyAction | mcLookout | mcCrouch | mcClimb | mcSprint) ? false : true;
}

bool CActor::is_actor_crouch() {
	return mstate_real & (mcAnyAction | mcAccel | mcClimb | mcSprint)
		? false
		: mstate_real & mcCrouch ? true : false;
}

bool CActor::is_actor_creep() {
	return mstate_real & (mcAnyAction | mcClimb | mcSprint)
		? false
		: (mstate_real & mcCrouch && mstate_real & mcAccel)
			? true
			: false;
}

bool CActor::is_actor_climb() {
	return mstate_real & (mcAnyAction | mcCrouch | mcSprint)
		? false
		: mstate_real & mcClimb ? true : false;
}

bool CActor::is_actor_walking() {
	bool run = false;
	if (mstate_real&mcAccel)
		run = psActorFlags.test(AF_ALWAYSRUN) ? false : true;
	else
		run = psActorFlags.test(AF_ALWAYSRUN) ? true : false;
	return ((mstate_real & (mcJump | mcFall | mcLanding | mcLanding2 | mcCrouch | mcClimb | mcSprint)) || run)
		? false
		: mstate_real & mcAnyMove ? true : false;
}

bool CActor::is_actor_running() {
	bool run = false;
	if (mstate_real&mcAccel)
		run = psActorFlags.test(AF_ALWAYSRUN) ? false : true;
	else
		run = psActorFlags.test(AF_ALWAYSRUN) ? true : false;
	return mstate_real & (mcJump | mcFall | mcLanding | mcLanding2 | mcLookout | mcCrouch | mcClimb | mcSprint)
		? false
		: (mstate_real & mcAnyMove && run ) ? true : false;
}

bool CActor::is_actor_sprinting() {
	return mstate_real & (mcJump | mcFall | mcLanding | mcLanding2 | mcLookout | mcCrouch | mcAccel | mcClimb | mcLStrafe | mcRStrafe)
		? false
		: (mstate_real & mcFwd && mstate_real & mcSprint )
			? true
			: false;
}

bool CActor::is_actor_crouching() {
	return mstate_real & (mcJump | mcFall | mcLanding | mcLanding2 | mcAccel | mcClimb)
		? false
		: (mstate_real & mcAnyMove && mstate_real & mcCrouch)
			? true
			: false;
}

bool CActor::is_actor_creeping() {
	return mstate_real & (mcJump | mcFall | mcLanding | mcLanding2 | mcClimb)
		? false
		: (mstate_real & mcAnyMove && mstate_real & mcCrouch && mstate_real & mcAccel)
			? true
			: false;
}

bool CActor::is_actor_climbing() {
	return mstate_real & (mcJump | mcFall | mcLanding | mcLanding2)
		? false
		: (mstate_real & mcAnyMove && mstate_real & mcClimb)
			? true
			: false;
}

bool CActor::is_actor_moving() {
	return mstate_real & mcAnyAction ? true : false;
}


void CActor::RepackAmmo() {
  xr_vector<CWeaponAmmo*> _ammo;
  // заполняем массив неполными пачками
  for ( PIItem& _pIItem : inventory().m_ruck ) {
    CWeaponAmmo* pAmmo = smart_cast<CWeaponAmmo*>( _pIItem );
    if ( pAmmo && pAmmo->m_boxCurr < pAmmo->m_boxSize )
      _ammo.push_back( pAmmo );
  }
  while ( !_ammo.empty() ) {
    shared_str asect = _ammo[ 0 ]->cNameSect(); // текущая секция
    u16 box_size     = _ammo[ 0 ]->m_boxSize; // размер пачки
    u32 cnt          = 0;
    u16 cart_cnt     = 0;
    // считаем кол=во патронов текущей секции
    for ( CWeaponAmmo* ammo : _ammo ) {
      if ( asect == ammo->cNameSect() ) {
        cnt = cnt + ammo->m_boxCurr;
        cart_cnt++;
      }
    }
    // если больше одной неполной пачки, то перепаковываем
    if ( cart_cnt > 1 ) {
      for ( CWeaponAmmo* ammo : _ammo ) {
        if ( asect == ammo->cNameSect() ) {
          if ( cnt > 0 ) {
            if ( cnt > box_size ) {
              ammo->m_boxCurr = box_size;
              cnt             = cnt - box_size;
            } else {
              ammo->m_boxCurr = ( u16 )cnt;
              cnt             = 0;
            }
          } else {
            ammo->DestroyObject();
          }
        }
      }
    }
    //чистим массив от обработанных пачек
    _ammo.erase( std::remove_if( _ammo.begin(), _ammo.end(), [asect]( CWeaponAmmo* a ) { return a->cNameSect() == asect; } ), _ammo.end() );
  }
}

bool CActor::unlimited_ammo()
{
	return !!psActorFlags.test(AF_UNLIMITEDAMMO);
}

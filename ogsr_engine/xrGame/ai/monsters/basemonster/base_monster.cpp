#include "stdafx.h"
#include "base_monster.h"
#include "../../../PhysicsShell.h"
#include "../../../hit.h"
#include "../../../PHDestroyable.h"
#include "../../../CharacterPhysicsSupport.h"
#include "game_level_cross_table.h"
#include "game_graph.h"
#include "../../../phmovementcontrol.h"
#include "../ai_monster_squad_manager.h"
#include "xrserver_objects_alife_monsters.h"
#include "../corpse_cover.h"
#include "../../../cover_evaluators.h"
#include "../../../seniority_hierarchy_holder.h"
#include "../../../team_hierarchy_holder.h"
#include "../../../squad_hierarchy_holder.h"
#include "../../../group_hierarchy_holder.h"
#include "../../../phdestroyable.h"
#include "../../../../Include/xrRender/KinematicsAnimated.h"
#include "../../../../Include/xrRender/Kinematics.h"
#include "../../../detail_path_manager.h"
#include "../../../hudmanager.h"
#include "../../../memory_manager.h"
#include "../../../visual_memory_manager.h"
#include "../monster_velocity_space.h"
#include "../../../entitycondition.h"
#include "../../../sound_player.h"
#include "../../../level.h"
#include "../../../ui/UIMainIngameWnd.h"
#include "../state_manager.h"
#include "../controlled_entity.h"
#include "../control_animation_base.h"
#include "../control_direction_base.h"
#include "../control_movement_base.h"
#include "../control_path_builder_base.h"
#include "../monster_cover_manager.h"
#include "../monster_home.h"
#include "../../../inventory.h"
#include "../../../xrserver.h"
#include "../ai_monster_squad.h"
#include "../../../actor.h"
#include "../../../ai_object_location.h"
#include "../../../ai_space.h"
#include "script_engine.h"
#include "../anti_aim_ability.h"
#include "../../../game_object_space.h"

CBaseMonster::CBaseMonster() :	m_psy_aura(this, "psy"), 
								m_fire_aura(this, "fire"), 
								m_radiation_aura(this, "radiation"), 
								m_base_aura(this, "base")
{
	m_pPhysics_support=xr_new<CCharacterPhysicsSupport>(CCharacterPhysicsSupport::etBitting,this);
	
	m_pPhysics_support				->in_Init();

	// Components external init 
	
	m_control_manager				= xr_new<CControl_Manager>(this);


	EnemyMemory.init_external		(this, 20000);
	SoundMemory.init_external		(this, 20000);
	CorpseMemory.init_external		(this, 20000);
	HitMemory.init_external			(this, 50000);

	EnemyMan.init_external			(this);
	CorpseMan.init_external			(this);

	// Инициализация параметров анимации	

	StateMan						= 0;

	MeleeChecker.init_external		(this);
	Morale.init_external			(this);

	m_controlled					= 0;

	
	control().add					(&m_com_manager,  ControlCom::eControlCustom);
	
	m_com_manager.add_ability		(ControlCom::eControlSequencer);
	m_com_manager.add_ability		(ControlCom::eControlTripleAnimation);

	CoverMan						= xr_new<CMonsterCoverManager>(this);

	Home							= xr_new<CMonsterHome>(this);

	com_man().add_ability			(ControlCom::eComCriticalWound);

	EatedCorpse								=	NULL;

	m_steer_manager							=	NULL;
	m_grouping_behaviour					=	NULL;

	m_last_grouping_behaviour_update_tick	=	0;
	m_feel_enemy_who_made_sound_max_distance = 0;
	m_feel_enemy_who_just_hit_max_distance   = 0;
	m_feel_enemy_max_distance                = 0;

	m_anti_aim								=	nullptr;
	m_head_bone_name						=	"bip01_head";

	m_first_tick_enemy_inaccessible			=	0;
	m_last_tick_enemy_inaccessible			=	0;
	m_first_tick_object_not_at_home			=	0;
}


CBaseMonster::~CBaseMonster()
{
	xr_delete(m_anti_aim);
	xr_delete(m_steer_manager);
	xr_delete(m_pPhysics_support);
	xr_delete(m_corpse_cover_evaluator);
	xr_delete(m_enemy_cover_evaluator);
	xr_delete(m_cover_evaluator_close_point);
	
	xr_delete(m_control_manager);

	xr_delete(m_anim_base);
	xr_delete(m_move_base);
	xr_delete(m_path_base);
	xr_delete(m_dir_base);

	xr_delete(CoverMan);
	xr_delete(Home);
}

void CBaseMonster::update_pos_by_grouping_behaviour ()
{
	if ( !m_grouping_behaviour )
	{
		return;
	}

	Fvector acc = get_steer_manager()->calc_acceleration();

	acc.y = 0; // remove vertical component

	if ( !m_last_grouping_behaviour_update_tick )
	{
		m_last_grouping_behaviour_update_tick = Device.dwTimeGlobal;
	}	

	const float dt = 0.001f * (Device.dwTimeGlobal - m_last_grouping_behaviour_update_tick);
	
	m_last_grouping_behaviour_update_tick = Device.dwTimeGlobal;

	const Fvector old_pos  = Position();
	Fvector       offs     = acc*dt;
	const float   offs_mag = magnitude(offs);

	if ( offs_mag < 0.000001f )
	{
		// too little force applied, ignore it and save cpu
		return;
	}

	// this control maximum offset
	// higher values allow stronger forces, but can lead to jingling
	const float max_offs = 0.005f;
	if ( offs_mag > max_offs )
	{
		offs.set_length(0.005f);
	}

	Fvector   new_pos    = old_pos + offs;


	const u32 old_vertex = ai_location().level_vertex_id();
	u32       new_vertex = ai().level_graph().check_position_in_direction(old_vertex, old_pos, new_pos);

	if ( !ai().level_graph().valid_vertex_id(new_vertex) )
	{
		// aiming out of ai-map, ignore
		return;
	}

	// use physics simulation to slide along obstacles
	character_physics_support()->movement()->VirtualMoveTo(new_pos, new_pos);

	if ( !ai().level_graph().valid_vertex_position(new_pos) )
	{
		// aiming out of ai-map, ignore
		return;
	}

	new_vertex = ai().level_graph().check_position_in_direction(old_vertex, old_pos, new_pos);

	if ( !ai().level_graph().valid_vertex_id(new_vertex) )
	{
		return;
	}

	// finally, new position is valid on the ai-map, we can use it
	character_physics_support()->movement()->SetPosition(new_pos);
	Position() = new_pos;
	ai_location().level_vertex(new_vertex);
}

bool   accessible_epsilon (CBaseMonster * const object, Fvector const pos, float epsilon)
{
	Fvector const offsets[]			=	{	Fvector().set( 0.f,			0.f,	0.f),
											Fvector().set(- epsilon, 	0.f,  	0.f),
											Fvector().set(+ epsilon, 	0.f,  	0.f),
											Fvector().set( 0.f,			0.f, 	- epsilon),
											Fvector().set( 0.f,			0.f, 	+ epsilon)	};
	
	for ( u32 i=0; i<sizeof(offsets)/sizeof(offsets[0]); ++i )
	{
		if ( object->movement().restrictions().accessible(pos + offsets[i]) )
			return						true;
	}

	return								false;
}

static
bool enemy_inaccessible (CBaseMonster * const object)
{
	if ( object->getDestroy() ) return false;
	CEntityAlive const * enemy		=	object->EnemyMan.get_enemy();
	if ( !enemy || enemy->getDestroy() )
		return							false;

	Fvector const enemy_pos			=	enemy->Position();
	Fvector const enemy_vert_pos	=	ai().level_graph().vertex_position(enemy->ai_location().level_vertex_id());
	
	float const xz_dist_to_vertex	=	enemy_vert_pos.distance_to_xz(enemy_pos);
	float const y_dist_to_vertex	=	_abs(enemy_vert_pos.y - enemy_pos.y);

	if ( xz_dist_to_vertex > 0.5f && y_dist_to_vertex > 3.f )
		return							true;

	if ( xz_dist_to_vertex >= 1.2f || y_dist_to_vertex >= 1.2f )
		return							true;

	if ( !object->Home->at_home(enemy_pos) )
		return							true;

	if ( !accessible_epsilon(object, enemy_pos, 1.5f) )
		return							true;

	if ( !ai().level_graph().valid_vertex_position(enemy_pos) )
		return							true;
	
	if ( !ai().level_graph().valid_vertex_id(enemy->ai_location().level_vertex_id()) )
		return							true;
	
	return								false;
}

bool CBaseMonster::enemy_accessible ()
{
	if ( !m_first_tick_enemy_inaccessible )
		return							true;

	if ( EnemyMan.get_enemy() )
	{
		u32 const enemy_vertex		=	EnemyMan.get_enemy()->ai_location().level_vertex_id();
		if ( ai_location().level_vertex_id() == enemy_vertex )
			return						false;
	}

	if ( Device.dwTimeGlobal < m_first_tick_enemy_inaccessible + 3000 )
		return							true;

	return								false;
}

bool CBaseMonster::at_home ()
{
	return										!m_first_tick_object_not_at_home ||
												(Device.dwTimeGlobal < m_first_tick_object_not_at_home + 4000);
}

void CBaseMonster::update_enemy_accessible_and_at_home_info	()
{
	if ( !Home->at_home() )
	{
		if ( !m_first_tick_object_not_at_home )
			m_first_tick_object_not_at_home	=	Device.dwTimeGlobal;
	}
	else
		m_first_tick_object_not_at_home		=	0;

	if ( !EnemyMan.get_enemy() )
	{
		m_first_tick_enemy_inaccessible		=	0;
		m_last_tick_enemy_inaccessible		=	0;
		return;
	}

	if ( ::enemy_inaccessible(this) )
	{
		if ( !m_first_tick_enemy_inaccessible )
			m_first_tick_enemy_inaccessible	=	Device.dwTimeGlobal;

		m_last_tick_enemy_inaccessible		=	Device.dwTimeGlobal;
	}
	else
	{
		if ( m_last_tick_enemy_inaccessible && Device.dwTimeGlobal - m_last_tick_enemy_inaccessible > 3000 )
		{
			m_first_tick_enemy_inaccessible	=	0;
			m_last_tick_enemy_inaccessible	=	0;
		}
	}
}

void CBaseMonster::UpdateCL()
{
	if ( EatedCorpse && !CorpseMemory.is_valid_corpse(EatedCorpse) )
	{
		EatedCorpse = NULL;
	}

	inherited::UpdateCL();
	
	if (g_Alive()) {
		update_enemy_accessible_and_at_home_info();
		CStepManager::update				();

		update_pos_by_grouping_behaviour();
	}

	control().update_frame();

	m_pPhysics_support->in_UpdateCL();
}

void CBaseMonster::shedule_Update(u32 dt)
{
	inherited::shedule_Update	(dt);

	// update_eyes_visibility		();

	if ( m_anti_aim )
	{
		m_anti_aim->update_schedule();
	}

	m_psy_aura.update_schedule();
	m_fire_aura.update_schedule();
	m_base_aura.update_schedule();
	m_radiation_aura.update_schedule();

	control().update_schedule	();

	Morale.update_schedule		(dt);
	
	m_pPhysics_support->in_shedule_Update(dt);

#ifdef DEBUG	
	show_debug_info();
#endif
}


//////////////////////////////////////////////////////////////////////
// Other functions
//////////////////////////////////////////////////////////////////////


void CBaseMonster::Die(CObject* who)
{
	if (StateMan) StateMan->critical_finalize();

	m_psy_aura.on_monster_death();
	m_radiation_aura.on_monster_death();
	m_fire_aura.on_monster_death();
	m_base_aura.on_monster_death();

	if ( m_anti_aim )
	{
		m_anti_aim->on_monster_death ();
	}

	inherited::Die(who);
        sound().clear_playing_sounds();

	if (is_special_killer(who))
		sound().play			(MonsterSound::eMonsterSoundDieInAnomaly);
	else
		sound().play			(MonsterSound::eMonsterSoundDie);

	monster_squad().remove_member	((u8)g_Team(),(u8)g_Squad(),(u8)g_Group(),this);

	if ( m_grouping_behaviour )
	{
		m_grouping_behaviour->set_squad(NULL);
	}
	
	if (m_controlled)			m_controlled->on_die();
}


//void CBaseMonster::Hit(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse, ALife::EHitType hit_type)
void	CBaseMonster::Hit							(SHit* pHDS)
{
	if (ignore_collision_hit && (pHDS->hit_type == ALife::eHitTypeStrike)) return;
	
	if (invulnerable())
		return;

	SHit HDS = *pHDS;
	callback( GameObject::entity_alive_before_hit )( &HDS );
	if ( HDS.ignore_flag )
	  return;

	if (g_Alive())
		if (!critically_wounded()) 
			update_critical_wounded( HDS.boneID, HDS.power );
	


//	inherited::Hit(P,dir,who,element,p_in_object_space,impulse,hit_type);
	inherited::Hit( &HDS );
}

void CBaseMonster::PHHit(SHit& H)
{
	m_pPhysics_support->in_Hit(H);
}

CPHDestroyable*	CBaseMonster::	ph_destroyable	()
{
	return smart_cast<CPHDestroyable*>(character_physics_support());
}

bool CBaseMonster::useful(const CItemManager *manager, const CGameObject *object) const
{
	const Fvector& object_pos = object->Position();
	if (!movement().restrictions().accessible(object_pos))
	{
		return false;
	}

	// Lain: added (temp?) guard due to bug http://tiger/bugz/view.php?id=15983
	// sometimes accessible(object->Position())) returns true
	// but accessible(ai_location().level_vertex_id()) crashes 
	// because level_vertex_id is not valid, so this code syncs vertex_id with position
	if ( !ai().level_graph().valid_vertex_id(object->ai_location().level_vertex_id()) )
	{
		u32 vertex_id = ai().level_graph().vertex_id(object_pos);
		if ( !ai().level_graph().valid_vertex_id(vertex_id) )
		{
			return false;
		}
		object->ai_location().level_vertex(vertex_id);
	}

	if ( !movement().restrictions().accessible(object->ai_location().level_vertex_id()) )
	{
		return false;
	}

	const CEntityAlive *pCorpse = smart_cast<const CEntityAlive *>(object); 
	if ( !pCorpse ) 
	{
		return false;
	}
	
	if ( !pCorpse->g_Alive() )
	{
		return true;
	}

	return false;
}

float CBaseMonster::evaluate(const CItemManager *manager, const CGameObject *object) const
{
	return (0.f);
}

//////////////////////////////////////////////////////////////////////////

void CBaseMonster::ChangeTeam(int team, int squad, int group)
{
	if ((team == g_Team()) && (squad == g_Squad()) && (group == g_Group())) return;

#ifdef DEBUG
	if (!g_Alive()) {
		ai().script_engine().print_stack	();
		VERIFY2								(g_Alive(),"you are trying to change team of a dead entity");
	}
#endif // DEBUG

	// remove from current team
	monster_squad().remove_member	((u8)g_Team(),(u8)g_Squad(),(u8)g_Group(),this);
	inherited::ChangeTeam			(team,squad,group);
	monster_squad().register_member	((u8)g_Team(),(u8)g_Squad(),(u8)g_Group(), this);

	if ( m_grouping_behaviour )
	{
		m_grouping_behaviour->set_squad( monster_squad().get_squad(this) );
	}	
}


void CBaseMonster::SetTurnAnimation(bool turn_left)
{
	(turn_left) ? anim().SetCurAnim(eAnimStandTurnLeft) : anim().SetCurAnim(eAnimStandTurnRight);
}

void CBaseMonster::set_state_sound(u32 type, bool once)
{
	if (once) {
	
		sound().play(type);
	
	} else {

		// handle situation, when monster want to play attack sound for the first time
		if ((type == MonsterSound::eMonsterSoundAggressive) && 
			(m_prev_sound_type != MonsterSound::eMonsterSoundAggressive)) {
			
			sound().play(MonsterSound::eMonsterSoundAttackHit);

		} else {
			// get count of monsters in squad
			u8 objects_count = monster_squad().get_squad(this)->get_count(this, 20.f);

			// include myself
			objects_count++;
			VERIFY(objects_count > 0);

			u32 delay = 0;
			switch (type) {
			case MonsterSound::eMonsterSoundIdle : 
				// check distance to actor

				if (Actor()->Position().distance_to(Position()) > db().m_fDistantIdleSndRange) {
					delay = u32(float(db().m_dwDistantIdleSndDelay) * _sqrt(float(objects_count)));
					type  = MonsterSound::eMonsterSoundIdleDistant;
				} else {
					delay = u32(float(db().m_dwIdleSndDelay) * _sqrt(float(objects_count)));
				}
				
				break;
			case MonsterSound::eMonsterSoundEat:
				delay = u32(float(db().m_dwEatSndDelay) * _sqrt(float(objects_count)));
				break;
			case MonsterSound::eMonsterSoundAggressive:
			case MonsterSound::eMonsterSoundPanic:
				delay = u32(float(db().m_dwAttackSndDelay) * _sqrt(float(objects_count)));
				break;
			}

			sound().play(type, 0, 0, delay);
		} 
	}

	m_prev_sound_type	= type;
}

BOOL CBaseMonster::feel_touch_on_contact	(CObject *O)
{
	return		(inherited::feel_touch_on_contact(O));
}

BOOL CBaseMonster::feel_touch_contact(CObject *O)
{
	return inherited::feel_touch_contact(O);
}

void CBaseMonster::TranslateActionToPathParams()
{
	bool bEnablePath = true;
	u32 vel_mask = 0;
	u32 des_mask = 0;

	EAction action	=	anim().m_tAction;
	switch (action) 
	{
	case ACT_STAND_IDLE: 
	case ACT_SIT_IDLE:	 
	case ACT_LIE_IDLE:
	case ACT_EAT:
	case ACT_SLEEP:
	case ACT_REST:
		//jump
	//case ACT_JUMP:
	case ACT_LOOK_AROUND:
		bEnablePath = false;
		break;
	case ACT_ATTACK:
		if ( !m_attack_on_move_params.enabled )
		{
			bEnablePath = false;
		}
		else
		{
			if (m_bDamaged) {
				vel_mask = MonsterMovement::eVelocityParamsRunDamaged;
				des_mask = MonsterMovement::eVelocityParameterRunDamaged;
			} else {
				vel_mask = MonsterMovement::eVelocityParamsRun;
				des_mask = MonsterMovement::eVelocityParameterRunNormal;
			}
		}
		break;

	case ACT_HOME_WALK_GROWL:
		vel_mask = MonsterMovement::eVelocityParamsWalkGrowl;
		des_mask = MonsterMovement::eVelocityParameterWalkGrowl;
		break;

	case ACT_HOME_WALK_SMELLING:
		vel_mask = MonsterMovement::eVelocityParamsWalkSmelling;
		des_mask = MonsterMovement::eVelocityParameterWalkSmelling;
		break;
	case ACT_WALK_FWD:
		if (m_bDamaged) {
			vel_mask = MonsterMovement::eVelocityParamsWalkDamaged;
			des_mask = MonsterMovement::eVelocityParameterWalkDamaged;
		} else {
			vel_mask = MonsterMovement::eVelocityParamsWalk;
			des_mask = MonsterMovement::eVelocityParameterWalkNormal;
		}
		break;
	case ACT_WALK_BKWD:
		break;
	case ACT_RUN:
		if (m_bDamaged) {
			vel_mask = MonsterMovement::eVelocityParamsRunDamaged;
			des_mask = MonsterMovement::eVelocityParameterRunDamaged;
		} else {
			vel_mask = MonsterMovement::eVelocityParamsRun;
			des_mask = MonsterMovement::eVelocityParameterRunNormal;
		}
		break;
	case ACT_DRAG:
		vel_mask = MonsterMovement::eVelocityParamsDrag;
		des_mask = MonsterMovement::eVelocityParameterDrag;

		anim().SetSpecParams(ASP_MOVE_BKWD);

		break;
	case ACT_STEAL:
		vel_mask = MonsterMovement::eVelocityParamsSteal;
		des_mask = MonsterMovement::eVelocityParameterSteal;
		break;
	}

	if (state_invisible) {
		vel_mask = MonsterMovement::eVelocityParamsInvisible;
		des_mask = MonsterMovement::eVelocityParameterInvisible;
	}

	if (m_force_real_speed) vel_mask = des_mask;

	if (bEnablePath) {
		path().set_velocity_mask	(vel_mask);
		path().set_desirable_mask	(des_mask);
		path().enable_path			();
	} else {
		path().disable_path			();
	}
}

u32 CBaseMonster::get_attack_rebuild_time()
{
	float dist = EnemyMan.get_enemy()->Position().distance_to(Position());
	return (100 + u32(20.f * dist));
}

void CBaseMonster::on_kill_enemy(const CEntity *obj)
{
	const CEntityAlive *entity	= smart_cast<const CEntityAlive *>(obj);
	
	// добавить в список трупов	
	CorpseMemory.add_corpse		(entity);
	
	// удалить всю информацию о хитах
	HitMemory.remove_hit_info	(entity);

	// удалить всю информацию о звуках
	SoundMemory.clear			();
}

CMovementManager *CBaseMonster::create_movement_manager	()
{
	m_movement_manager = xr_new<CControlPathBuilder>(this);

	control().add					(m_movement_manager, ControlCom::eControlPath);
	control().install_path_manager	(m_movement_manager);
	control().set_base_controller	(m_path_base, ControlCom::eControlPath);

	return			(m_movement_manager);
}

DLL_Pure *CBaseMonster::_construct	()
{
	create_base_controls			();

	control().add					(m_anim_base, ControlCom::eControlAnimationBase);
	control().add					(m_move_base, ControlCom::eControlMovementBase);
	control().add					(m_path_base, ControlCom::eControlPathBase);
	control().add					(m_dir_base,  ControlCom::eControlDirBase);

	control().set_base_controller	(m_anim_base, ControlCom::eControlAnimation);
	control().set_base_controller	(m_move_base, ControlCom::eControlMovement);
	control().set_base_controller	(m_dir_base,  ControlCom::eControlDir);
	
	inherited::_construct		();
	CStepManager::_construct	();
	CInventoryOwner::_construct	();
	return						(this);
}

void CBaseMonster::net_Relcase(CObject *O)
{
	inherited::net_Relcase(O);

	StateMan->remove_links			(O);

	com_man().remove_links			(O);

	// TODO: do not clear, remove only object O
	if (g_Alive()) {
		EnemyMemory.remove_links	(O);
		SoundMemory.remove_links	(O);
		HitMemory.remove_hit_info	(O);

		EnemyMan.remove_links		(O);
		CorpseMan.remove_links		(O);

		UpdateMemory				();
		
		monster_squad().remove_links(O);
	}
	CorpseMemory.remove_links		(O);
	m_pPhysics_support->in_NetRelcase(O);
}
	
void CBaseMonster::create_base_controls()
{
	m_anim_base		= xr_new<CControlAnimationBase>		();
	m_move_base		= xr_new<CControlMovementBase>		();
	m_path_base		= xr_new<CControlPathBuilderBase>	();
	m_dir_base		= xr_new<CControlDirectionBase>		();
}

void CBaseMonster::set_action(EAction action)
{
	anim().m_tAction		= action;
}

CParticlesObject* CBaseMonster::PlayParticles(const shared_str& name, const Fvector &position, const Fvector &dir, BOOL auto_remove, BOOL xformed)
{
	CParticlesObject* ps = CParticlesObject::Create(name.c_str(),auto_remove);
	
	// вычислить позицию и направленность партикла
	Fmatrix	matrix; 

	matrix.identity			();
	matrix.k.set			(dir);
	Fvector::generate_orthonormal_basis_normalized(matrix.k,matrix.j,matrix.i);
	matrix.translate_over	(position);
	
	(xformed) ?				ps->SetXFORM (matrix) : ps->UpdateParent(matrix,zero_vel); 
	ps->Play				();

	return ps;
}

void CBaseMonster::on_restrictions_change()
{
	inherited::on_restrictions_change();

	if (StateMan) StateMan->reinit();
}

void CBaseMonster::load_effector(LPCSTR section, LPCSTR line, SAttackEffector &effector)
{
	LPCSTR ppi_section = pSettings->r_string(section, line);
	effector.ppi.duality.h			= pSettings->r_float(ppi_section,"duality_h");
	effector.ppi.duality.v			= pSettings->r_float(ppi_section,"duality_v");
	effector.ppi.gray				= pSettings->r_float(ppi_section,"gray");
	effector.ppi.blur				= pSettings->r_float(ppi_section,"blur");
	effector.ppi.noise.intensity	= pSettings->r_float(ppi_section,"noise_intensity");
	effector.ppi.noise.grain		= pSettings->r_float(ppi_section,"noise_grain");
	effector.ppi.noise.fps			= pSettings->r_float(ppi_section,"noise_fps");
	VERIFY(!fis_zero(effector.ppi.noise.fps));

	sscanf(pSettings->r_string(ppi_section,"color_base"),	"%f,%f,%f", &effector.ppi.color_base.r,	&effector.ppi.color_base.g,	&effector.ppi.color_base.b);
	sscanf(pSettings->r_string(ppi_section,"color_gray"),	"%f,%f,%f", &effector.ppi.color_gray.r,	&effector.ppi.color_gray.g,	&effector.ppi.color_gray.b);
	sscanf(pSettings->r_string(ppi_section,"color_add"),	"%f,%f,%f", &effector.ppi.color_add.r,	&effector.ppi.color_add.g,	&effector.ppi.color_add.b);

	effector.time				= pSettings->r_float(ppi_section,"time");
	effector.time_attack		= pSettings->r_float(ppi_section,"time_attack");
	effector.time_release		= pSettings->r_float(ppi_section,"time_release");

	effector.ce_time			= pSettings->r_float(ppi_section,"ce_time");
	effector.ce_amplitude		= pSettings->r_float(ppi_section,"ce_amplitude");
	effector.ce_period_number	= pSettings->r_float(ppi_section,"ce_period_number");
	effector.ce_power			= pSettings->r_float(ppi_section,"ce_power");
}

bool CBaseMonster::check_start_conditions(ControlCom::EControlType type)
{
	if ( !StateMan->check_control_start_conditions(type) )
	{
		return					false;
	}

	if ( type == ControlCom::eControlRotationJump )
	{
		EMonsterState state	=	StateMan->get_state_type();
		
		if ( !is_state(state, eStateAttack_Run) && 
			 !is_state(state, eStateAttack_RunAttack) ) 
		{
			return false;
		}
	} 
	if ( type == ControlCom::eControlMeleeJump ) 
	{
		EMonsterState state	=	StateMan->get_state_type();

		if (!is_state(state, eStateAttack_Run) && 
			!is_state(state, eStateAttack_Melee) &&
			!is_state(state, eStateAttack_RunAttack) ) 
		{
			return				false;
		}
	}

	return						true;
}

void CBaseMonster::OnEvent(NET_Packet& P, u16 type)
{
	inherited::OnEvent			(P,type);
	CInventoryOwner::OnEvent	(P,type);

	u16			id;
	switch (type){
	case GE_TRADE_BUY:
	case GE_OWNERSHIP_TAKE:
	case GE_TRANSFER_TAKE:
		{
			P.r_u16		(id);
			CObject		*O	= Level().Objects.net_Find	(id);
			VERIFY		(O);

			CGameObject			*GO = smart_cast<CGameObject*>(O);
			CInventoryItem		*pIItem = smart_cast<CInventoryItem*>(GO);
			VERIFY				(inventory().CanTakeItem(pIItem));
			pIItem->m_eItemPlace = eItemPlaceRuck;

			O->H_SetParent		(this);
			inventory().Take	(GO, true, true);
		break;
		}
	case GE_TRADE_SELL:
	case GE_OWNERSHIP_REJECT:
	case GE_TRANSFER_REJECT:
		{
			P.r_u16		(id);
			CObject* O	= Level().Objects.net_Find	(id);
			VERIFY		(O);

			bool just_before_destroy	= !P.r_eof() && P.r_u8();
			bool dont_create_shell = (type == GE_TRADE_SELL) || (type == GE_TRANSFER_REJECT) || just_before_destroy;

			O->SetTmpPreDestroy				(just_before_destroy);
			if (inventory().DropItem(smart_cast<CGameObject*>(O)) && !O->getDestroy()) 
			{
				O->H_SetParent	(0, dont_create_shell);
				feel_touch_deny	(O,2000);
			}
		}
		break;

	case GE_KILL_SOMEONE:
		P.r_u16		(id);
		CObject* O	= Level().Objects.net_Find	(id);

		if (O)  {
			CEntity *pEntity = smart_cast<CEntity*>(O);
			if (pEntity) on_kill_enemy(pEntity);
		}
			
		break;
	}
}

// Lain: added
bool   CBaseMonster::check_eated_corpse_draggable()
{
	const CEntity* p_corpse = EatedCorpse;
	if ( !p_corpse || !p_corpse->Visual() )
	{
		return false;
	}
	
	if ( IKinematics* K = p_corpse->Visual()->dcast_PKinematics() )
	{
/*
		if ( CInifile* ini = K->LL_UserData() )
		{
			return ini->section_exist("capture_used_bones") && ini->line_exist("capture_used_bones", "bones");
		}
*/
		return true;
	}

	return false;	
}

//-------------------------------------------------------------------
// CBaseMonster's  Atack on Move
//-------------------------------------------------------------------

bool   CBaseMonster::can_attack_on_move()
{
	return m_attack_on_move_params.enabled;
}

float   CBaseMonster::get_attack_on_move_max_go_close_time()
{
	return m_attack_on_move_params.max_go_close_time;
}

float   CBaseMonster::get_attack_on_move_far_radius()
{
	float radius	=	m_attack_on_move_params.far_radius;
	clamp				(radius, 0.f, 100.f);
	return				radius;
}

float   CBaseMonster::get_attack_on_move_attack_radius()
{
	return m_attack_on_move_params.attack_radius;
}

float   CBaseMonster::get_attack_on_move_update_side_period()
{
	return m_attack_on_move_params.update_side_period;
}

float   CBaseMonster::get_attack_on_move_prediction_factor()
{
	return m_attack_on_move_params.prediction_factor;
}

float   CBaseMonster::get_attack_on_move_prepare_radius()
{
	return m_attack_on_move_params.prepare_radius;
}

float   CBaseMonster::get_attack_on_move_prepare_time()
{
	return m_attack_on_move_params.prepare_time;
}

float   CBaseMonster::get_psy_influence ()
{
	if ( g_Alive() || m_psy_aura.enable_for_dead() )
	  return m_psy_aura.calculate();
	return 0.f;
}

float   CBaseMonster::get_radiation_influence ()
{
	if ( g_Alive() || m_radiation_aura.enable_for_dead() )
	  return m_radiation_aura.calculate();
	return 0.f;
}

float   CBaseMonster::get_fire_influence ()
{
	if ( g_Alive() || m_fire_aura.enable_for_dead() )
	  return m_fire_aura.calculate();
	return 0.f;
}

void   CBaseMonster::play_detector_sound()
{
	m_psy_aura.play_detector_sound();
	m_radiation_aura.play_detector_sound();
	m_fire_aura.play_detector_sound();
}

bool CBaseMonster::is_jumping()
{
	return m_com_manager.is_jumping();
}

void CBaseMonster::update_eyes_visibility ()
{
	if ( !m_left_eye_bone_name )
	{
		return;
	}

	IKinematics* const skeleton	=	smart_cast<IKinematics*>(Visual());
	if ( !skeleton )
	{
		return;
	}

	u16 const left_eye_bone_id	=	skeleton->LL_BoneID(m_left_eye_bone_name);
	u16 const right_eye_bone_id	=	skeleton->LL_BoneID(m_right_eye_bone_name);

	R_ASSERT						(left_eye_bone_id != u16(-1) && right_eye_bone_id != u16(-1));

	bool eyes_visible			=	!g_Alive() || get_screen_space_coverage_diagonal() > 0.05f;

	bool const was_visible		=	!!skeleton->LL_GetBoneVisible	(left_eye_bone_id);
	skeleton->LL_SetBoneVisible		(left_eye_bone_id, eyes_visible, true);
	skeleton->LL_SetBoneVisible		(right_eye_bone_id, eyes_visible, true);

	if ( !was_visible && eyes_visible )
	{
		skeleton->CalculateBones_Invalidate();
		skeleton->CalculateBones		();
	}
}

float CBaseMonster::get_screen_space_coverage_diagonal()
{
	Fbox		b		= Visual()->getVisData().box;

	Fmatrix				xform;
	xform.mul			(Device.mFullTransform,XFORM());
	Fvector2	mn		={flt_max,flt_max},mx={flt_min,flt_min};

	for (u32 k=0; k<8; ++k)
	{
		Fvector p;
		b.getpoint		(k,p);
		xform.transform	(p);
		mn.x			= std::min(mn.x,p.x);
		mn.y			= std::min(mn.y,p.y);
		mx.x			= std::max(mx.x,p.x);
		mx.y			= std::max(mx.y,p.y);
	}

	float const width	=	mx.x - mn.x;
	float const height	=	mx.y - mn.y;

	float const	average_diagonal	=	_sqrt(width * height);
	return				average_diagonal;
}

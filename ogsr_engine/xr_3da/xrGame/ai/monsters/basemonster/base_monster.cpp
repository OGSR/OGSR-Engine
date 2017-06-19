#include "stdafx.h"
#include "base_monster.h"
#include "../../../PhysicsShell.h"
#include "../../../hit.h"
#include "../../../PHDestroyable.h"
#include "../../../CharacterPhysicsSupport.h"
#include "../../../game_level_cross_table.h"
#include "../../../game_graph.h"
#include "../../../phmovementcontrol.h"
#include "../ai_monster_squad_manager.h"
#include "../../../xrserver_objects_alife_monsters.h"
#include "../corpse_cover.h"
#include "../../../cover_evaluators.h"
#include "../../../seniority_hierarchy_holder.h"
#include "../../../team_hierarchy_holder.h"
#include "../../../squad_hierarchy_holder.h"
#include "../../../group_hierarchy_holder.h"
#include "../../../phdestroyable.h"
#include "../../../../skeletoncustom.h"
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
#include "../anomaly_detector.h"
#include "../monster_cover_manager.h"
#include "../monster_home.h"
#include "../../../inventory.h"
#include "../../../xrserver.h"
#include "../ai_monster_squad.h"
#include "../../../actor.h"
#include "../../../ai_object_location.h"
#include "../../../ai_space.h"
#include "../../../script_engine.h"

CBaseMonster::CBaseMonster()
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


	m_anomaly_detector				= xr_new<CAnomalyDetector>(this);
	CoverMan						= xr_new<CMonsterCoverManager>(this);

	Home							= xr_new<CMonsterHome>(this);

	com_man().add_ability			(ControlCom::eComCriticalWound);
}


CBaseMonster::~CBaseMonster()
{
	xr_delete(m_pPhysics_support);
	xr_delete(m_corpse_cover_evaluator);
	xr_delete(m_enemy_cover_evaluator);
	xr_delete(m_cover_evaluator_close_point);
	
	xr_delete(m_control_manager);

	xr_delete(m_anim_base);
	xr_delete(m_move_base);
	xr_delete(m_path_base);
	xr_delete(m_dir_base);

	xr_delete(m_anomaly_detector);
	xr_delete(CoverMan);
	xr_delete(Home);
}

void CBaseMonster::UpdateCL()
{
	inherited::UpdateCL();
	
	if (g_Alive()) {
		CStepManager::update				();
	}

	control().update_frame();

	m_pPhysics_support->in_UpdateCL();
}

void CBaseMonster::shedule_Update(u32 dt)
{
	inherited::shedule_Update	(dt);
	control().update_schedule	();

	Morale.update_schedule		(dt);

	m_anomaly_detector->update_schedule();
	
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

	inherited::Die(who);

	if (is_special_killer(who))
		sound().play			(MonsterSound::eMonsterSoundDieInAnomaly);
	else
		sound().play			(MonsterSound::eMonsterSoundDie);

	monster_squad().remove_member	((u8)g_Team(),(u8)g_Squad(),(u8)g_Group(),this);
	
	if (m_controlled)			m_controlled->on_die();
}


//void CBaseMonster::Hit(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse, ALife::EHitType hit_type)
void	CBaseMonster::Hit							(SHit* pHDS)
{
	if (ignore_collision_hit && (pHDS->hit_type == ALife::eHitTypeStrike)) return;
	
	if (invulnerable())
		return;

	if (g_Alive())
		if (!critically_wounded()) 
			update_critical_wounded(pHDS->boneID,pHDS->power);
	


//	inherited::Hit(P,dir,who,element,p_in_object_space,impulse,hit_type);
	inherited::Hit(pHDS);
}

void CBaseMonster::PHHit(float P,Fvector &dir, CObject *who,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type /*=ALife::eHitTypeWound*/)
{
	m_pPhysics_support->in_Hit(P,dir,who,element,p_in_object_space,impulse,hit_type);
}

CPHDestroyable*	CBaseMonster::	ph_destroyable	()
{
	return smart_cast<CPHDestroyable*>(character_physics_support());
}

bool CBaseMonster::useful(const CItemManager *manager, const CGameObject *object) const
{
	if (!movement().restrictions().accessible(object->Position()))
		return				(false);

	if (!movement().restrictions().accessible(object->ai_location().level_vertex_id()))
		return				(false);

	const CEntityAlive *pCorpse = smart_cast<const CEntityAlive *>(object); 
	if (!pCorpse) return false;
	
	if (!pCorpse->g_Alive()) return true;
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
	m_anomaly_detector->on_contact(O);
	return inherited::feel_touch_contact(O);
}

void CBaseMonster::TranslateActionToPathParams()
{
	bool bEnablePath = true;
	u32 vel_mask = 0;
	u32 des_mask = 0;

	switch (anim().m_tAction) {
	case ACT_STAND_IDLE: 
	case ACT_SIT_IDLE:	 
	case ACT_LIE_IDLE:
	case ACT_EAT:
	case ACT_SLEEP:
	case ACT_REST:
	case ACT_LOOK_AROUND:
	case ACT_ATTACK:
		bEnablePath = false;
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
	return (100 + u32(50.f * dist));
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

	// TODO: do not clear, remove only object O
	if (g_Alive()) {
		EnemyMemory.remove_links	(O);
		SoundMemory.remove_links	(O);
		CorpseMemory.remove_links	(O);
		HitMemory.remove_hit_info	(O);

		EnemyMan.reinit				();
		CorpseMan.reinit			();

		UpdateMemory				();
		
		monster_squad().remove_links(O);
	}
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
	if (type == ControlCom::eControlRotationJump) {
		EMonsterState state = StateMan->get_state_type();
		if (state != eStateAttack_Run) return false;
	} if (type == ControlCom::eControlMeleeJump) {
		EMonsterState state = StateMan->get_state_type();
		if (!is_state(state, eStateAttack_Run) && !is_state(state, eStateAttack_Melee)) return false;
	}
	return true;
}

void CBaseMonster::OnEvent(NET_Packet& P, u16 type)
{
	inherited::OnEvent			(P,type);
	CInventoryOwner::OnEvent	(P,type);

	u16			id;
	switch (type){
	case GE_OWNERSHIP_TAKE:
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
		{
			P.r_u16		(id);
			CObject* O	= Level().Objects.net_Find	(id);
			VERIFY		(O);

			bool just_before_destroy	= !P.r_eof() && P.r_u8();
			O->SetTmpPreDestroy				(just_before_destroy);
			if (inventory().DropItem(smart_cast<CGameObject*>(O)) && !O->getDestroy()) 
			{
				O->H_SetParent	(0,just_before_destroy);
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



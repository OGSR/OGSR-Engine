#include "pch_script.h"
#include "base_monster.h"
#include "../../../script_entity_action.h"
#include "../../../phmovementcontrol.h"
#include "../../../sight_manager.h"
#include "../../../detail_path_manager.h"
#include "../../../ai_object_location.h"
#include "../../../sound_player.h"
#include "../../../ai_monster_space.h"
#include "../state_manager.h"

#include "../../../ai_debug.h"
#include "../../../level.h"
#include "../../../level_debug.h"

#include "../control_animation_base.h"
#include "../control_path_builder_base.h"

#include "../../../patrol_path_manager.h"
#include "../../../patrol_path_manager_space.h"


using namespace MonsterSpace;
using namespace MonsterSound;

//////////////////////////////////////////////////////////////////////////
bool CBaseMonster::bfAssignMovement (CScriptEntityAction *tpEntityAction)
{
	CScriptMovementAction	&l_tMovementAction	= tpEntityAction->m_tMovementAction;
	
	// check if completed
	if (l_tMovementAction.m_bCompleted)	return(false);

	// check if alive
	CEntityAlive *entity_alive = smart_cast<CEntityAlive*>(this);
	if (entity_alive && !entity_alive->g_Alive()) {
		l_tMovementAction.m_bCompleted = true;
		return				(false);
	}

	if (control().path_builder().detail().time_path_built() >= tpEntityAction->m_tActionCondition.m_tStartTime) {
		if ((l_tMovementAction.m_fDistToEnd > 0) && control().path_builder().is_path_end(l_tMovementAction.m_fDistToEnd))  {
			l_tMovementAction.m_bCompleted = true;
			
		}
		if (control().path_builder().actual_all() && control().path_builder().path_completed()) {
			l_tMovementAction.m_bCompleted = true;
			return false;
		}
	}

	// translate script.action into anim().action
	switch (l_tMovementAction.m_tMoveAction) {
	case eMA_WalkFwd:	anim().m_tAction = ACT_WALK_FWD;		break;
	case eMA_WalkBkwd:	anim().m_tAction = ACT_WALK_BKWD;	break;
	case eMA_Run:		anim().m_tAction = ACT_RUN;			break;
	case eMA_Drag:		anim().m_tAction = ACT_DRAG;			break;
	case eMA_Steal:		anim().m_tAction = ACT_STEAL;		break;
	}

	m_force_real_speed = (l_tMovementAction.m_tSpeedParam == eSP_ForceSpeed);

	switch (l_tMovementAction.m_tGoalType) {
		
		case CScriptMovementAction::eGoalTypeObject : {
			CGameObject		*l_tpGameObject = smart_cast<CGameObject*>(l_tMovementAction.m_tpObjectToGo);
			path().set_target_point	(l_tpGameObject->Position(), l_tpGameObject->ai_location().level_vertex_id());
			break;
													  }
		case CScriptMovementAction::eGoalTypePatrolPath : 
			path().set_patrol_path_type							();
			control().path_builder().set_path_type				(MovementManager::ePathTypePatrolPath);
			control().path_builder().patrol().set_path			(l_tMovementAction.m_path,l_tMovementAction.m_path_name);
			control().path_builder().patrol().set_start_type	(l_tMovementAction.m_tPatrolPathStart);
			control().path_builder().patrol().set_route_type	(l_tMovementAction.m_tPatrolPathStop);
			control().path_builder().patrol().set_random		(l_tMovementAction.m_bRandom);
			if (l_tMovementAction.m_previous_patrol_point != u32(-1)) {
				control().path_builder().patrol().set_previous_point(l_tMovementAction.m_previous_patrol_point);
			}
			break;

		case CScriptMovementAction::eGoalTypePathPosition :
		case CScriptMovementAction::eGoalTypeNoPathPosition :
			path().set_target_point	(l_tMovementAction.m_tDestinationPosition);
			break;
		case CScriptMovementAction::eGoalTypePathNodePosition :
			path().set_target_point	(l_tMovementAction.m_tDestinationPosition, l_tMovementAction.m_tNodeID);
			break;
	}
	
	return	(true);
}

///////////////////////////////////////////////////////////////////////////
bool CBaseMonster::bfAssignObject(CScriptEntityAction *tpEntityAction)
{
	if (!inherited::bfAssignObject(tpEntityAction))
		return	(false);

//	CScriptObjectAction	&l_tObjectAction = tpEntityAction->m_tObjectAction;
//	if (!l_tObjectAction.m_tpObject)
//		return	(false == (l_tObjectAction.m_bCompleted = true));
//
//	CEntityAlive	*l_tpEntity		= smart_cast<CEntityAlive*>(l_tObjectAction.m_tpObject);
//	if (!l_tpEntity) return	(false == (l_tObjectAction.m_bCompleted = true));
//
//	switch (l_tObjectAction.m_tGoalType) {
//		case eObjectActionTake: 
//			m_PhysicMovementControl->PHCaptureObject(l_tpEntity);
//			break;
//		case eObjectActionDrop: 
//			m_PhysicMovementControl->PHReleaseObject();
//			break;
//	}
//	
//	l_tObjectAction.m_bCompleted = true;
	return	(true);
}


bool CBaseMonster::bfAssignWatch(CScriptEntityAction *tpEntityAction)
{
	if (!inherited::bfAssignWatch(tpEntityAction))
		return		(false);
	
	// Инициализировать action
	anim().m_tAction = ACT_STAND_IDLE;

	CScriptWatchAction	&l_tWatchAction = tpEntityAction->m_tWatchAction;
	if (l_tWatchAction.completed()) return false;

	Fvector new_pos;
	switch (l_tWatchAction.m_tWatchType) {
		case SightManager::eSightTypePosition:
			LookPosition(l_tWatchAction.m_tWatchVector);
			break;
		case SightManager::eSightTypeDirection:
			new_pos.mad(Position(), l_tWatchAction.m_tWatchVector, 2.f);
			LookPosition(new_pos);
			break;
	}


	if (!control().direction().is_turning())
		l_tWatchAction.m_bCompleted = true;
	else
		l_tWatchAction.m_bCompleted = false;
	
	return		(!l_tWatchAction.m_bCompleted);
}

bool CBaseMonster::bfAssignAnimation(CScriptEntityAction *tpEntityAction)
{
	if (!inherited::bfAssignAnimation(tpEntityAction))
		return			(false);

	CScriptAnimationAction	&l_tAnimAction	= tpEntityAction->m_tAnimationAction;
	if (l_tAnimAction.completed()) return false;
	
	// translate animation.action into anim().action
	switch (l_tAnimAction.m_tAnimAction) {
	case eAA_StandIdle:		anim().m_tAction = ACT_STAND_IDLE;	break;
	case eAA_SitIdle:		anim().m_tAction = ACT_SIT_IDLE;		break;
	case eAA_LieIdle:		anim().m_tAction = ACT_LIE_IDLE;		break;
	case eAA_Eat:			anim().m_tAction = ACT_EAT;			break;
	case eAA_Sleep:			anim().m_tAction = ACT_SLEEP;		break;
	case eAA_Rest:			anim().m_tAction = ACT_REST;			break;
	case eAA_Attack:		anim().m_tAction = ACT_ATTACK;		break;
	case eAA_LookAround:	anim().m_tAction = ACT_LOOK_AROUND;	break;
	}

	return				(true);
}

bool CBaseMonster::bfAssignSound(CScriptEntityAction *tpEntityAction)
{
	CScriptSoundAction	&l_tAction = tpEntityAction->m_tSoundAction;
	if (l_tAction.completed()) return false;

	if (l_tAction.m_monster_sound == MonsterSound::eMonsterSoundDummy) {
		if (!inherited::bfAssignSound(tpEntityAction))
			return			(false);
	}

	switch (l_tAction.m_monster_sound) {
	case	eMonsterSoundIdle:			sound().play(eMonsterSoundIdle,			0, 0, (l_tAction.m_monster_sound_delay == int(-1)) ? db().m_dwIdleSndDelay		: l_tAction.m_monster_sound_delay);		break;
	case 	eMonsterSoundEat:			sound().play(eMonsterSoundEat,			0, 0, (l_tAction.m_monster_sound_delay == int(-1)) ? db().m_dwEatSndDelay		: l_tAction.m_monster_sound_delay);		break;
	case 	eMonsterSoundAggressive:	sound().play(eMonsterSoundAggressive,	0, 0, (l_tAction.m_monster_sound_delay == int(-1)) ? db().m_dwAttackSndDelay	: l_tAction.m_monster_sound_delay);		break;
	case	eMonsterSoundAttackHit:		sound().play(eMonsterSoundAttackHit);	break;
	case	eMonsterSoundTakeDamage:	sound().play(eMonsterSoundTakeDamage);	break;
	case	eMonsterSoundDie:			sound().play(eMonsterSoundDie);			break;
	case	eMonsterSoundThreaten:		sound().play(eMonsterSoundThreaten,		0, 0, (l_tAction.m_monster_sound_delay == int(-1)) ? db().m_dwAttackSndDelay : l_tAction.m_monster_sound_delay);		break;
	case	eMonsterSoundSteal:			sound().play(eMonsterSoundSteal,		0, 0, (l_tAction.m_monster_sound_delay == int(-1)) ? db().m_dwAttackSndDelay : l_tAction.m_monster_sound_delay);		break;
	case	eMonsterSoundPanic:			sound().play(eMonsterSoundPanic,		0, 0, (l_tAction.m_monster_sound_delay == int(-1)) ? db().m_dwAttackSndDelay : l_tAction.m_monster_sound_delay);		break;
	}

	return				(true);
}

bool CBaseMonster::bfAssignMonsterAction(CScriptEntityAction *tpEntityAction)
{
	if (!inherited::bfAssignMonsterAction(tpEntityAction)) return false;
	
	CScriptMonsterAction	&l_tAction = tpEntityAction->m_tMonsterAction;	
	if (l_tAction.completed()) return false;

	CEntityAlive *pE = smart_cast<CEntityAlive *>(l_tAction.m_tObject);

	switch(l_tAction.m_tAction) {
		case eGA_Rest:		
			StateMan->force_script_state(eStateRest);	
			break;
		case eGA_Eat:		
			if (pE && !pE->getDestroy() && !pE->g_Alive()){
				CorpseMan.force_corpse(pE);
				StateMan->force_script_state(eStateEat);	
			} else StateMan->force_script_state(eStateRest);	

			break;
		case eGA_Attack:
			if (pE && !pE->getDestroy() && pE->g_Alive()){
				EnemyMan.force_enemy(pE);
				StateMan->force_script_state(eStateAttack);
			} else StateMan->force_script_state(eStateRest);

			break;
		case eGA_Panic:		
			if (pE && !pE->getDestroy() && pE->g_Alive()){
				EnemyMan.force_enemy			(pE);
				StateMan->force_script_state	(eStatePanic);
			} else StateMan->force_script_state	(eStateRest);	
			break;
	}

	m_script_state_must_execute = true;
	return (!l_tAction.m_bCompleted);
}



void CBaseMonster::ProcessScripts()
{
	if (!g_Alive()) return;
	if (m_script_processing_active) return;
	
	m_script_processing_active = true;

	//movement().Update_Initialize			();
	
	// Выполнить скриптовые actions
	m_script_state_must_execute					= false;
	inherited::ProcessScripts					();

	Device.dwTimeGlobal							= Device.dwTimeGlobal;

	// обновить мир (память, враги, объекты)
	UpdateMemory								();
	
	anim().accel_deactivate					();

	// если из скрипта выбрано действие по универсальной схеме, выполнить его
	if (m_script_state_must_execute) 	
		StateMan->execute_script_state			();		
	
	TranslateActionToPathParams					();

	// обновить путь
	//movement().Update_Execute			();

	//anim().Update							();
	
	// установить текущую скорость
	//movement().Update_Finalize			();

	// Удалить все враги и объекты, которые были принудительно установлены
	// во время выполнения скриптового действия
	if (m_script_state_must_execute) {
		EnemyMan.unforce_enemy();
		CorpseMan.unforce_corpse();
	}

	m_force_real_speed			= false;
	m_script_processing_active	= false;

#ifdef DEBUG	
	if (psAI_Flags.test(aiMonsterDebug)) {
		DBG().object_info(this,this).remove_item (u32(0));
		DBG().object_info(this,this).remove_item (u32(1));
		DBG().object_info(this,this).add_item	 (*cName(), D3DCOLOR_XRGB(255,0,0), 0);
		DBG().object_info(this,this).add_item	 ("Under script", D3DCOLOR_XRGB(255,0,0), 1);
	} else {
		DBG().object_info(this,this).clear		 ();
	}
#endif

}

CEntity *CBaseMonster::GetCurrentEnemy()
{
	CEntity *enemy = 0;
	
	if (EnemyMan.get_enemy()) 
		enemy = const_cast<CEntity *>(smart_cast<const CEntity*>(EnemyMan.get_enemy()));

	if (!enemy || enemy->getDestroy() || !enemy->g_Alive()) enemy = 0;

	return (enemy);
}

CEntity *CBaseMonster::GetCurrentCorpse()
{
	CEntity *corpse = 0;

	if (CorpseMan.get_corpse()) 
		corpse = const_cast<CEntity *>(smart_cast<const CEntity*>(CorpseMan.get_corpse()));

	if (!corpse || corpse->getDestroy() || corpse->g_Alive()) corpse = 0;

	return (corpse);
}

void CBaseMonster::SetScriptControl(const bool bScriptControl, shared_str caScriptName)
{
	if (StateMan) StateMan->critical_finalize();

	CScriptEntity::SetScriptControl(bScriptControl, caScriptName);
}

int	CBaseMonster::get_enemy_strength()
{
	if (EnemyMan.get_enemy()) {
		switch (EnemyMan.get_danger_type()) {
			case eVeryStrong	: 	return (4);
			case eStrong		: 	return (3);
			case eNormal		: 	return (2);
			case eWeak			:	return (1);
		}
	}
	
	return (0);
}

void CBaseMonster::vfFinishAction(CScriptEntityAction *tpEntityAction)
{
	inherited::vfFinishAction(tpEntityAction);
}


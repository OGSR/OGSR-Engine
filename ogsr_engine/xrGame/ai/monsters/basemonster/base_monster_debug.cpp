#include "stdafx.h"
#include "base_monster.h"
#include "../../../level.h"
#include "../../../level_debug.h"
#include "../../../entitycondition.h"
#include "../../../ai_debug.h"
#include "../state_defs.h"
#include "../state_manager.h"
#include "../../../phmovementcontrol.h"
#include "../../../characterphysicssupport.h"
#include "../../../actor.h"

#ifdef DEBUG
CBaseMonster::SDebugInfo CBaseMonster::show_debug_info()
{
	if (!g_Alive()) return SDebugInfo();

	if (m_show_debug_info == 0) {
		DBG().text(this).clear();
		return SDebugInfo();
	}

	float y				= 200;
	float x				= (m_show_debug_info == 1) ? 40.f : float(::Render->getTarget()->get_width() / 2) + 40.f;
	const float delta_y	= 12;

	string256	text;

	u32			color			= D3DCOLOR_XRGB(0,255,0);
	u32			delimiter_color	= D3DCOLOR_XRGB(0,0,255);

	DBG().text(this).clear	 ();
	DBG().text(this).add_item("---------------------------------------", x, y+=delta_y, delimiter_color);

	sprintf_s(text, "-- Monster : [%s]  Current Time = [%u]", *cName(), Device.dwTimeGlobal);
	DBG().text(this).add_item(text, x, y+=delta_y, color);
	DBG().text(this).add_item("-----------   PROPERTIES   ------------", x, y+=delta_y, delimiter_color);

	sprintf_s(text, "Health = [%f]", conditions().GetHealth());
	DBG().text(this).add_item(text,										 x, y+=delta_y, color);

	sprintf_s(text, "Morale = [%f]", Morale.get_morale());
	DBG().text(this).add_item(text,										 x, y+=delta_y, color);


	DBG().text(this).add_item("-----------   MEMORY   ----------------", x, y+=delta_y, delimiter_color);

	if (EnemyMan.get_enemy()) {
		sprintf_s(text, "Current Enemy = [%s]", *EnemyMan.get_enemy()->cName());
	} else 
		sprintf_s(text, "Current Enemy = [NONE]");
	DBG().text(this).add_item(text,										 x, y+=delta_y, color);
	
	if (EnemyMan.get_enemy()) {
		sprintf_s(text, "SeeEnemy[%u] EnemySeeMe[%u] TimeLastSeen[%u]", EnemyMan.see_enemy_now(),EnemyMan.enemy_see_me_now(),EnemyMan.get_enemy_time_last_seen());
		DBG().text(this).add_item(text,									x, y+=delta_y, color);
	}

	if (CorpseMan.get_corpse()) {
		sprintf_s(text, "Current Corpse = [%s] Satiety = [%.2f]", *CorpseMan.get_corpse()->cName(), GetSatiety());
	} else 
		sprintf_s(text, "Current Corpse = [NONE] Satiety = [%.2f]", GetSatiety());

	DBG().text(this).add_item(text,										 x, y+=delta_y, color);

	// Sound
	if (SoundMemory.IsRememberSound()) {
		SoundElem	sound_elem;
		bool		dangerous_sound;
		SoundMemory.GetSound(sound_elem, dangerous_sound);

		string128	s_type;

		switch(sound_elem.type){
			case WEAPON_SHOOTING:			strcpy(s_type,"WEAPON_SHOOTING"); break;
			case MONSTER_ATTACKING:			strcpy(s_type,"MONSTER_ATTACKING"); break;
			case WEAPON_BULLET_RICOCHET:	strcpy(s_type,"WEAPON_BULLET_RICOCHET"); break;
			case WEAPON_RECHARGING:			strcpy(s_type,"WEAPON_RECHARGING"); break;

			case WEAPON_TAKING:				strcpy(s_type,"WEAPON_TAKING"); break;
			case WEAPON_HIDING:				strcpy(s_type,"WEAPON_HIDING"); break;
			case WEAPON_CHANGING:			strcpy(s_type,"WEAPON_CHANGING"); break;
			case WEAPON_EMPTY_CLICKING:		strcpy(s_type,"WEAPON_EMPTY_CLICKING"); break;

			case MONSTER_DYING:				strcpy(s_type,"MONSTER_DYING"); break;
			case MONSTER_INJURING:			strcpy(s_type,"MONSTER_INJURING"); break;
			case MONSTER_WALKING:			strcpy(s_type,"MONSTER_WALKING"); break;
			case MONSTER_JUMPING:			strcpy(s_type,"MONSTER_JUMPING"); break;
			case MONSTER_FALLING:			strcpy(s_type,"MONSTER_FALLING"); break;
			case MONSTER_TALKING:			strcpy(s_type,"MONSTER_TALKING"); break;

			case DOOR_OPENING:				strcpy(s_type,"DOOR_OPENING"); break;
			case DOOR_CLOSING:				strcpy(s_type,"DOOR_CLOSING"); break;
			case OBJECT_BREAKING:			strcpy(s_type,"OBJECT_BREAKING"); break;
			case OBJECT_FALLING:			strcpy(s_type,"OBJECT_FALLING"); break;
			case NONE_DANGEROUS_SOUND:		strcpy(s_type,"NONE_DANGEROUS_SOUND"); break;
		}

		if (sound_elem.who)
			sprintf_s(text,"Sound: type[%s] time[%u] power[%.3f] val[%i] src[+]", s_type, sound_elem.time, sound_elem.power, sound_elem.value);
		else 
			sprintf_s(text,"Sound: type[%s] time[%u] power[%.3f] val[%i] src[?]", s_type, sound_elem.time, sound_elem.power, sound_elem.value);


	} else 
		sprintf_s(text, "Sound: NONE");

	DBG().text(this).add_item(text,										 x, y+=delta_y, color);

	// Hit
	if (HitMemory.is_hit()) {
		if (HitMemory.get_last_hit_object()) {
			sprintf_s(text,"Hit Info: object=[%s] time=[%u]", *(HitMemory.get_last_hit_object()->cName()), HitMemory.get_last_hit_time());
		} else {
			sprintf_s(text,"Hit Info: object=[NONE] time=[%u]", HitMemory.get_last_hit_time());
		}
	} else 
		sprintf_s(text, "Hit Info: NONE");

	DBG().text(this).add_item(text,										 x, y+=delta_y, color);

	DBG().text(this).add_item("-----------   MOVEMENT   ------------", x, y+=delta_y, delimiter_color);

	sprintf_s(text, "Actual = [%u] Enabled = [%u]",			 control().path_builder().actual(), control().path_builder().enabled());
	DBG().text(this).add_item(text,										x, y+=delta_y, color);
	
	sprintf_s(text, "Speed: Linear = [%.3f] Angular = [%.3f]", control().movement().velocity_current(), 0.f);
	DBG().text(this).add_item(text,										x, y+=delta_y, color);
	
	DBG().text(this).add_item("------- Attack Distances -------------", x, y+=delta_y, delimiter_color);
	sprintf_s(text, "MinDist[%.3f] MaxDist[%.3f] As_Step[%.3f] As_MinDist[%.3f]", 
		MeleeChecker.get_min_distance(),
		MeleeChecker.get_max_distance(),
		MeleeChecker.dbg_as_step(),
		MeleeChecker.dbg_as_min_dist()
	);
	DBG().text(this).add_item(text,										x, y+=delta_y, color);


	if (EnemyMan.get_enemy()) {
		sprintf_s(text, "Current Enemy = [%s]", *EnemyMan.get_enemy()->cName());
	} else 
		sprintf_s(text, "Current Enemy = [NONE]");
	DBG().text(this).add_item(text,										 x, y+=delta_y, color);



	return SDebugInfo(x, y, delta_y, color, delimiter_color);
}

void CBaseMonster::debug_fsm()
{
	if (!g_Alive()) return;

	if (!psAI_Flags.test(aiMonsterDebug)) {
		DBG().object_info(this,this).clear ();
		return;
	}
	
	EMonsterState state = StateMan->get_state_type();
	
	string128 st;

	switch (state) {
		case eStateRest_WalkGraphPoint:					sprintf_s(st,"Rest :: Walk Graph");			break;
		case eStateRest_Idle:							sprintf_s(st,"Rest :: Idle");					break;
		case eStateRest_Fun:							sprintf_s(st,"Rest :: Fun");					break;
		case eStateRest_Sleep:							sprintf_s(st,"Rest :: Sleep");				break;
		case eStateRest_MoveToHomePoint:				sprintf_s(st,"Rest :: MoveToHomePoint");		break;
		case eStateRest_WalkToCover:					sprintf_s(st,"Rest :: WalkToCover");			break;
		case eStateRest_LookOpenPlace:					sprintf_s(st,"Rest :: LookOpenPlace");		break;

		case eStateEat_CorpseApproachRun:				sprintf_s(st,"Eat :: Corpse Approach Run");	break;
		case eStateEat_CorpseApproachWalk:				sprintf_s(st,"Eat :: Corpse Approach Walk");	break;
		case eStateEat_CheckCorpse:						sprintf_s(st,"Eat :: Check Corpse");			break;
		case eStateEat_Eat:								sprintf_s(st,"Eat :: Eating");				break;
		case eStateEat_WalkAway:						sprintf_s(st,"Eat :: Walk Away");				break;
		case eStateEat_Rest:							sprintf_s(st,"Eat :: Rest After Meal");		break;
		case eStateEat_Drag:							sprintf_s(st,"Eat :: Drag");					break;
		
		case eStateAttack_Run:							sprintf_s(st,"Attack :: Run");				break;
		case eStateAttack_Melee:						sprintf_s(st,"Attack :: Melee");				break;
		case eStateAttack_RunAttack:					sprintf_s(st,"Attack :: Run Attack");			break;
		case eStateAttack_RunAway:						sprintf_s(st,"Attack :: Run Away");			break;
		case eStateAttack_FindEnemy:					sprintf_s(st,"Attack :: Find Enemy");			break;
		case eStateAttack_Steal:						sprintf_s(st,"Attack :: Steal");				break;
		case eStateAttack_AttackHidden:					sprintf_s(st,"Attack :: Attack Hidden");		break;
		
		case eStateAttackCamp_Hide:						sprintf_s(st,"Attack Camp:: Hide");			break;
		case eStateAttackCamp_Camp:						sprintf_s(st,"Attack Camp:: Camp");			break;
		case eStateAttackCamp_StealOut:					sprintf_s(st,"Attack Camp:: Steal Out");		break;

		case eStateAttack_HideInCover:					sprintf_s(st,"Attack :: Hide In Cover");		break;
		case eStateAttack_MoveOut:						sprintf_s(st,"Attack :: Move Out From Cover");break;
		case eStateAttack_CampInCover:					sprintf_s(st,"Attack :: Camp In Cover");		break;

		case eStateAttack_Psy:							sprintf_s(st,"Attack :: Psy");				break;
		case eStateAttack_MoveToHomePoint:				sprintf_s(st,"Attack :: Move To Home Point");	break;
		case eStateAttack_HomePoint_Hide:				sprintf_s(st,"Attack :: Home Point :: Hide");	break;
		case eStateAttack_HomePoint_Camp:				sprintf_s(st,"Attack :: Home Point :: Camp");	break;
		case eStateAttack_HomePoint_LookOpenPlace:		sprintf_s(st,"Attack :: Home Point :: Look Open Place");	break;
		
		case eStatePanic_Run:							sprintf_s(st,"Panic :: Run Away");				break;
		case eStatePanic_FaceUnprotectedArea:			sprintf_s(st,"Panic :: Face Unprotected Area");	break;
		case eStatePanic_HomePoint_Hide:				sprintf_s(st,"Panic :: Home Point :: Hide");		break;
		case eStatePanic_HomePoint_LookOpenPlace:		sprintf_s(st,"Panic :: Home Point :: Look Open Place");	break;
		case eStatePanic_HomePoint_Camp:				sprintf_s(st,"Panic :: Home Point :: Camp");		break;

		case eStateHitted_Hide:							sprintf_s(st,"Hitted :: Hide");					break;
		case eStateHitted_MoveOut:						sprintf_s(st,"Hitted :: MoveOut");				break;
		case eStateHitted_Home:							sprintf_s(st,"Hitted :: Home");				break;

		case eStateHearDangerousSound_Hide:				sprintf_s(st,"Dangerous Snd :: Hide");			break;
		case eStateHearDangerousSound_FaceOpenPlace:	sprintf_s(st,"Dangerous Snd :: FaceOpenPlace");	break;
		case eStateHearDangerousSound_StandScared:		sprintf_s(st,"Dangerous Snd :: StandScared");		break;
		case eStateHearDangerousSound_Home:				sprintf_s(st,"Dangerous Snd :: Home");			break;

		case eStateHearInterestingSound_MoveToDest:		sprintf_s(st,"Interesting Snd :: MoveToDest");	break;
		case eStateHearInterestingSound_LookAround:		sprintf_s(st,"Interesting Snd :: LookAround");	break;
		
		case eStateHearHelpSound:						sprintf_s(st,"Hear Help Sound");	break;
		case eStateHearHelpSound_MoveToDest:			sprintf_s(st,"Hear Help Sound :: MoveToDest");	break;
		case eStateHearHelpSound_LookAround:			sprintf_s(st,"Hear Help Sound :: LookAround");	break;

		case eStateControlled_Follow_Wait:				sprintf_s(st,"Controlled :: Follow : Wait");			break;
		case eStateControlled_Follow_WalkToObject:		sprintf_s(st,"Controlled :: Follow : WalkToObject");	break;
		case eStateControlled_Attack:					sprintf_s(st,"Controlled :: Attack");					break;
		case eStateThreaten:							sprintf_s(st,"Threaten :: ");							break;
		case eStateFindEnemy_Run:						sprintf_s(st,"Find Enemy :: Run");							break;
		case eStateFindEnemy_LookAround_MoveToPoint:	sprintf_s(st,"Find Enemy :: Look Around : Move To Point");	break;
		case eStateFindEnemy_LookAround_LookAround:		sprintf_s(st,"Find Enemy :: Look Around : Look Around");		break;
		case eStateFindEnemy_LookAround_TurnToPoint:	sprintf_s(st,"Find Enemy :: Look Around : Turn To Point");	break;
		case eStateFindEnemy_Angry:						sprintf_s(st,"Find Enemy :: Angry");							break;
		case eStateFindEnemy_WalkAround:				sprintf_s(st,"Find Enemy :: Walk Around");					break;
		case eStateSquad_Rest_Idle:						sprintf_s(st,"Squad :: Rest : Idle");					break;
		case eStateSquad_Rest_WalkAroundLeader:			sprintf_s(st,"Squad :: Rest : WalkAroundLeader");		break;
		case eStateSquad_RestFollow_Idle:				sprintf_s(st,"Squad :: Follow Leader : Idle");		break;
		case eStateSquad_RestFollow_WalkToPoint:		sprintf_s(st,"Squad :: Follow Leader : WalkToPoint");	break;
		case eStateCustom_Vampire:						sprintf_s(st,"Attack :: Vampire");					break;
		case eStateVampire_ApproachEnemy:				sprintf_s(st,"Vampire :: Approach to enemy");			break;
		case eStateVampire_Execute:						sprintf_s(st,"Vampire :: Hit");						break;
		case eStateVampire_RunAway:						sprintf_s(st,"Vampire :: Run Away");					break;
		case eStateVampire_Hide:						sprintf_s(st,"Vampire :: Hide");						break;
		case eStatePredator:							sprintf_s(st,"Predator");								break;
		case eStatePredator_MoveToCover:				sprintf_s(st,"Predator :: MoveToCover");				break;
		case eStatePredator_LookOpenPlace:				sprintf_s(st,"Predator :: Look Open Place");			break;
		case eStatePredator_Camp:						sprintf_s(st,"Predator :: Camp");						break;
		case eStateBurerAttack_Tele:					sprintf_s(st,"Attack :: Telekinesis");			break;
		case eStateBurerAttack_Gravi:					sprintf_s(st,"Attack :: Gravi Wave");				break;
		case eStateBurerAttack_RunAround:				sprintf_s(st,"Attack :: Run Around");			break;
		case eStateBurerAttack_FaceEnemy:				sprintf_s(st,"Attack :: Face Enemy");			break;
		case eStateBurerAttack_Melee:					sprintf_s(st,"Attack :: Melee");				break;
		case eStateBurerScanning:						sprintf_s(st,"Attack :: Scanning");			break;
		case eStateCustomMoveToRestrictor:				sprintf_s(st,"Moving To Restrictor :: Position not accessible");	break;
		case eStateSmartTerrainTask:					sprintf_s(st,"ALIFE");	break;
		case eStateSmartTerrainTaskGamePathWalk:		sprintf_s(st,"ALIFE :: Game Path Walk");	break;
		case eStateSmartTerrainTaskLevelPathWalk:		sprintf_s(st,"ALIFE :: Level Path Walk");	break;
		case eStateSmartTerrainTaskWaitCapture:			sprintf_s(st,"ALIFE :: Wait till smart terrain will capture me");	break;
		case eStateUnknown:								sprintf_s(st,"Unknown State :: ");			break;
		default:										sprintf_s(st,"Undefined State ::");			break;
	}
	
	DBG().object_info(this,this).remove_item (u32(0));
	DBG().object_info(this,this).remove_item (u32(1));
	DBG().object_info(this,this).remove_item (u32(2));

	DBG().object_info(this,this).add_item	 (*cName(), D3DCOLOR_XRGB(255,0,0), 0);
	DBG().object_info(this,this).add_item	 (st, D3DCOLOR_XRGB(255,0,0), 1);
	
	sprintf_s(st, "Team[%u]Squad[%u]Group[%u]", g_Team(), g_Squad(), g_Group());
	DBG().object_info(this,this).add_item	 (st, D3DCOLOR_XRGB(255,0,0), 2);

	CEntityAlive *entity = smart_cast<CEntityAlive *>(Level().CurrentEntity());
	if (entity && entity->character_physics_support()->movement()) {
		sprintf_s(st,"VELOCITY [%f,%f,%f] Value[%f]",VPUSH(entity->character_physics_support()->movement()->GetVelocity()),entity->character_physics_support()->movement()->GetVelocityActual());
		DBG().text(this).clear();
		DBG().text(this).add_item(st,200,100,COLOR_GREEN,100);
	}
}


#endif

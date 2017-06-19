////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_debug.cpp
//	Created 	: 05.07.2005
//  Modified 	: 05.07.2005
//	Author		: Dmitriy Iassenev
//	Description : Debug functions for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"

#ifdef DEBUG
#include "ai_stalker.h"
#include "../../hudmanager.h"
#include "../../memory_manager.h"
#include "../../visual_memory_manager.h"
#include "../../sound_memory_manager.h"
#include "../../hit_memory_manager.h"
#include "../../enemy_manager.h"
#include "../../danger_manager.h"
#include "../../item_manager.h"
#include "../../actor.h"
#include "../../stalker_planner.h"
#include "../../script_game_object.h"
#include "../../stalker_animation_manager.h"
#include "../../weapon.h"
#include "../../sound_player.h"
#include "../../inventory.h"
#include "../../object_handler_planner.h"
#include "../../stalker_movement_manager.h"
#include "../../movement_manager_space.h"
#include "../../patrol_path_manager.h"
#include "../../level_path_manager.h"
#include "../../game_path_manager.h"
#include "../../detail_path_manager.h"
#include "../../sight_manager.h"
#include "../../ai_object_location.h"
#include "../../entitycondition.h"
#include "../ai_monsters_misc.h"
#include "../../agent_manager.h"
#include "../../agent_member_manager.h"
#include "../../agent_enemy_manager.h"
#include "../../agent_corpse_manager.h"
#include "../../agent_location_manager.h"
#include "../../cover_point.h"
#include "../../../camerabase.h"
#include "../../clsid_game.h"
#include "../../mt_config.h"
#include "../../weaponmagazined.h"
#include "../../object_handler_space.h"
#include "../../debug_renderer.h"
#include "../../CharacterPhysicsSupport.h"

CActor *g_debug_actor = 0;

void try_change_current_entity()
{
	CActor								*actor = smart_cast<CActor*>(Level().CurrentEntity());
	VERIFY								(actor);
	g_debug_actor						= actor;

	CFrustum							frustum;
	frustum.CreateFromMatrix			(Device.mFullTransform,FRUSTUM_P_LRTB|FRUSTUM_P_FAR);

	typedef xr_vector<ISpatial*>		OBJECTS;
	OBJECTS								ISpatialResult;
	g_SpatialSpace->q_frustum			(ISpatialResult, 0, STYPE_COLLIDEABLE, frustum);

	float								maxlen = 1000.0f;
	CAI_Stalker*						nearest_agent = 0;

	OBJECTS::const_iterator				I = ISpatialResult.begin();
	OBJECTS::const_iterator				E = ISpatialResult.end();
	for ( ; I != E; ++I) {
		CAI_Stalker						*current = smart_cast<CAI_Stalker*>(*I);
		if (!current)					continue;
		if (Level().CurrentEntity()==current) continue;

		Fvector							A, B, tmp; 
		current->Center					(A);

		tmp.sub							(A, actor->cam_Active()->vPosition);
		B.mad							(
			actor->cam_Active()->vPosition,
			actor->cam_Active()->vDirection,
			tmp.dotproduct(
				actor->cam_Active()->vDirection
			)
		);
		float len = B.distance_to_sqr(A);
		if (len > 1) continue;

		if (maxlen>len && !current->getDestroy())
		{
			maxlen = len;
			nearest_agent = current;
		};
	}

	if (!nearest_agent)
		return;

	Level().SetEntity		(nearest_agent);
	actor->inventory().Items_SetCurrentEntityHud(false);
	
	Engine.Sheduler.Unregister	(actor);
	Engine.Sheduler.Register	(actor);
	
	Engine.Sheduler.Unregister	(nearest_agent);
	Engine.Sheduler.Register	(nearest_agent, TRUE);
}

void restore_actor()
{
	VERIFY		(g_debug_actor);
	VERIFY		(Level().CurrentEntity()->CLS_ID != CLSID_OBJECT_ACTOR);

	Engine.Sheduler.Unregister	(Level().CurrentEntity());
	Engine.Sheduler.Register	(Level().CurrentEntity());

	Level().SetEntity			(g_debug_actor);

	Engine.Sheduler.Unregister	(g_debug_actor);
	Engine.Sheduler.Register	(g_debug_actor, TRUE);

	g_debug_actor->inventory().Items_SetCurrentEntityHud(true);

	CHudItem* pHudItem = smart_cast<CHudItem*>(g_debug_actor->inventory().ActiveItem());
	if (pHudItem) 
	{
		pHudItem->OnStateSwitch(pHudItem->GetState());
	}
}

template <typename planner_type>
void draw_planner						(const planner_type &brain, LPCSTR start_indent, LPCSTR indent, LPCSTR planner_id)
{
	planner_type						&_brain = const_cast<planner_type&>(brain);
	if (brain.solution().empty())
		return;

	CScriptActionPlannerAction			*planner = smart_cast<CScriptActionPlannerAction*>(&_brain.action(brain.solution().front()));
	if (planner)
		draw_planner					(*planner,start_indent,indent,_brain.action2string(brain.solution().front()));

	HUD().Font().pFontStat->OutNext	("%s ",start_indent);
	HUD().Font().pFontStat->OutNext	("%splanner %s",start_indent,planner_id);
	HUD().Font().pFontStat->OutNext	("%s%sevaluators  : %d",start_indent,indent,brain.evaluators().size());
	HUD().Font().pFontStat->OutNext	("%s%soperators   : %d",start_indent,indent,brain.operators().size());
	HUD().Font().pFontStat->OutNext	("%s%sselected    : %s",start_indent,indent,_brain.action2string(brain.solution().front()));
	// solution
	HUD().Font().pFontStat->OutNext	("%s%ssolution",start_indent,indent);
	for (int i=0; i<(int)brain.solution().size(); ++i)
		HUD().Font().pFontStat->OutNext("%s%s%s%s",start_indent,indent,indent,_brain.action2string(brain.solution()[i]));
	// current
	HUD().Font().pFontStat->OutNext	("%s%scurrent world state",start_indent,indent);
	planner_type::EVALUATORS::const_iterator	I = brain.evaluators().begin();
	planner_type::EVALUATORS::const_iterator	E = brain.evaluators().end();
	for ( ; I != E; ++I) {
		xr_vector<planner_type::COperatorCondition>::const_iterator J = std::lower_bound(brain.current_state().conditions().begin(),brain.current_state().conditions().end(),planner_type::CWorldProperty((*I).first,false));
		char				temp = '?';
		if ((J != brain.current_state().conditions().end()) && ((*J).condition() == (*I).first)) {
			temp			= (*J).value() ? '+' : '-';
			HUD().Font().pFontStat->OutNext	("%s%s%s    %5c : [%d][%s]",start_indent,indent,indent,temp,(*I).first,_brain.property2string((*I).first));
		}
	}
	// goal
	HUD().Font().pFontStat->OutNext	("%s%starget world state",start_indent,indent);
	I = brain.evaluators().begin();
	for ( ; I != E; ++I) {
		xr_vector<planner_type::COperatorCondition>::const_iterator J = std::lower_bound(brain.target_state().conditions().begin(),brain.target_state().conditions().end(),planner_type::CWorldProperty((*I).first,false));
		char				temp = '?';
		if ((J != brain.target_state().conditions().end()) && ((*J).condition() == (*I).first)) {
			temp			= (*J).value() ? '+' : '-';
			HUD().Font().pFontStat->OutNext	("%s%s%s    %5c : [%d][%s]",start_indent,indent,indent,temp,(*I).first,_brain.property2string((*I).first));
		}
	}
}

LPCSTR animation_name(CAI_Stalker *self, const MotionID &animation)
{
	if (!animation)
		return			("");
	CKinematicsAnimated	*skeleton_animated = smart_cast<CKinematicsAnimated*>(self->Visual());
	VERIFY				(skeleton_animated);
	LPCSTR				name = skeleton_animated->LL_MotionDefName_dbg(animation).first;
	return				(name);
}

void draw_restrictions(const shared_str &restrictions, LPCSTR start_indent, LPCSTR indent, LPCSTR header)
{
	HUD().Font().pFontStat->OutNext	("%s%s%s",start_indent,indent,header);
	string256	temp;
	for (u32 i=0, n=_GetItemCount(*restrictions); i<n; ++i)
		HUD().Font().pFontStat->OutNext("%s%s%s%s",start_indent,indent,indent,_GetItem(*restrictions,i,temp));
}

LPCSTR movement_type(const MonsterSpace::EMovementType &movement_type)
{
	switch (movement_type) {
		case MonsterSpace::eMovementTypeStand :
			return		("stand");
		case MonsterSpace::eMovementTypeWalk  :
			return		("walk");
		case MonsterSpace::eMovementTypeRun   :
			return		("run");
		default			: NODEFAULT;
	}
	return				("invalid");
}

LPCSTR danger_type(const CDangerObject::EDangerType &danger_type)
{
	switch (danger_type) {
		case CDangerObject::eDangerTypeBulletRicochet		: return	("bullet ricochet");
		case CDangerObject::eDangerTypeAttackSound			: return	("attack sound");
		case CDangerObject::eDangerTypeEntityAttacked		: return	("entity attacked");
		case CDangerObject::eDangerTypeEntityDeath			: return	("entity death");
		case CDangerObject::eDangerTypeFreshEntityCorpse	: return	("fresh entity corpse");
		case CDangerObject::eDangerTypeAttacked				: return	("I am attacked");
		case CDangerObject::eDangerTypeGrenade				: return	("greande nearby");
		case CDangerObject::eDangerTypeEnemySound			: return	("enemy sound");
		default												: NODEFAULT;
	};
	return				("");
}

void CAI_Stalker::debug_planner			(const script_planner *planner)
{
	m_debug_planner						= planner;
}

void CAI_Stalker::OnHUDDraw				(CCustomHUD *hud)
{
	inherited::OnHUDDraw				(hud);

	if (!psAI_Flags.test(aiStalker))
		return;

	CActor								*actor = smart_cast<CActor*>(Level().Objects.net_Find(0));
	if (!actor) {
		if (!g_debug_actor)
			return;

		actor							= g_debug_actor;
	}

	float								up_indent = 40.f;
	LPCSTR								indent = "  ";

	HUD().Font().pFontStat->SetColor	(D3DCOLOR_XRGB(0,255,0));
	HUD().Font().pFontStat->OutSet		(0,up_indent);
	// memory
	HUD().Font().pFontStat->OutNext	("memory");
	HUD().Font().pFontStat->OutNext	("%sname          : %s",indent,*cName());
	HUD().Font().pFontStat->OutNext	("%sid            : %d",indent,ID());
	HUD().Font().pFontStat->OutNext	("%shealth        : %f",indent,conditions().health());
	HUD().Font().pFontStat->OutNext	("%swounded       : %c",indent,wounded() ? '+' : '-');
	// visual
	HUD().Font().pFontStat->OutNext	("%svisual",indent);
	
	float								object_range, object_fov;
	update_range_fov					(object_range,object_fov,eye_range,deg2rad(eye_fov));
	HUD().Font().pFontStat->OutNext	("%s%seye range   : %f",indent,indent,object_range);
	HUD().Font().pFontStat->OutNext	("%s%sFOV         : %f",indent,indent,rad2deg(object_fov));
	if (g_Alive()) {
		HUD().Font().pFontStat->OutNext	("%s%sobjects     : %d",indent,indent,memory().visual().objects().size());
		HUD().Font().pFontStat->OutNext	("%s%snot yet     : %d",indent,indent,memory().visual().not_yet_visible_objects().size());
		HUD().Font().pFontStat->OutNext	("%s%sin frustum  : %d",indent,indent,memory().visual().raw_objects().size());
		if (memory().visual().visible_now(actor))
			HUD().Font().pFontStat->OutNext("%s%sactor       : visible",indent,indent);
		else {
			MemorySpace::CNotYetVisibleObject	*object = memory().visual().not_yet_visible_object(actor);
			if (object && !fis_zero(object->m_value))
				HUD().Font().pFontStat->OutNext("%s%sactor       : not yet visible : %f",indent,indent,object->m_value);
			else
				HUD().Font().pFontStat->OutNext("%s%sactor       : not visible",indent,indent);
		}
		// sound
		HUD().Font().pFontStat->OutNext	("%ssound",indent);
		HUD().Font().pFontStat->OutNext	("%s%sobjects     : %d",indent,indent,memory().sound().objects().size());
#ifdef USE_SELECTED_SOUND
		if (memory().sound().sound()) {
			HUD().Font().pFontStat->OutNext	("%s%sselected",indent,indent);
			HUD().Font().pFontStat->OutNext	("%s%s%stype",indent,indent,indent);
			HUD().Font().pFontStat->OutNext	("%s%s%spower     : %f",indent,indent,indent,memory().sound().sound()->m_power);
			HUD().Font().pFontStat->OutNext	("%s%s%sobject    : %s",indent,indent,indent,memory().sound().sound()->m_object ? *memory().sound().sound()->m_object->cName() : "unknown");
			if (g_Alive() && memory().sound().sound()->m_object)
				HUD().Font().pFontStat->OutNext("%s%s%svisible   : %s",indent,indent,indent,memory().visual().visible_now(memory().sound().sound()->m_object) ? "+" : "-");
		}
#endif
		// hit
		HUD().Font().pFontStat->OutNext	("%shit",indent);
		HUD().Font().pFontStat->OutNext	("%s%sobjects     : %d",indent,indent,memory().hit().objects().size());
		ALife::_OBJECT_ID					object_id = memory().hit().last_hit_object_id();
		HUD().Font().pFontStat->OutNext	("%s%slast hit object id   : %d",indent,indent,object_id);
		CObject								*object = (object_id == ALife::_OBJECT_ID(-1)) ? 0 : Level().Objects.net_Find(object_id);
		HUD().Font().pFontStat->OutNext	("%s%slast hit object name : %s",indent,indent,object ? *object->cName() : "");
#ifdef USE_SELECTED_HIT
		if (memory().hit().hit()) {
			HUD().Font().pFontStat->OutNext	("%s%sselected",indent,indent);
			HUD().Font().pFontStat->OutNext	("%s%s%spower     : %f",indent,indent,indent,memory().hit().hit()->m_amount);
			HUD().Font().pFontStat->OutNext	("%s%s%sobject    : %s",indent,indent,indent,memory().hit().hit()->m_object ? *memory().hit().hit()->m_object->cName() : "unknown");
			if (g_Alive() && memory().hit().hit()->m_object)
				HUD().Font().pFontStat->OutNext("%s%s%svisible   : %s",indent,indent,indent,memory().visual().visible_now(memory().hit().hit()->m_object) ? "+" : "-");
		}
#endif
	}
	// enemy
	HUD().Font().pFontStat->OutNext	("%senemy",indent);
	if (inventory().ActiveItem()) {
		HUD().Font().pFontStat->OutNext("%s%scan kill member   : %s",indent,indent,can_kill_member() ? "+" : "-");
		HUD().Font().pFontStat->OutNext("%s%scan kill enemy    : %s",indent,indent,can_kill_enemy() ? "+" : "-");
		HUD().Font().pFontStat->OutNext("%s%spick distance     : %f",indent,indent,pick_distance());
		HUD().Font().pFontStat->OutNext("%s%sfire make sense   : %s",indent,indent,fire_make_sense() ? "+" : "-");
		HUD().Font().pFontStat->OutNext("%s%sactor is enemy    : %c",indent,indent,is_relation_enemy(actor) ? '+' : '-');
		HUD().Font().pFontStat->OutNext("%s%sis enemy to actor : %c",indent,indent,actor->is_relation_enemy(this) ? '+' : '-');
	}

	HUD().Font().pFontStat->OutNext	("%s%sobjects     : %d",indent,indent,memory().enemy().objects().size());
	if (g_Alive()) {
		CEnemyManager::OBJECTS::const_iterator	I = memory().enemy().objects().begin();
		CEnemyManager::OBJECTS::const_iterator	E = memory().enemy().objects().end();
		for ( ; I != E; ++I)
			HUD().Font().pFontStat->OutNext	("%s%s%s%s : %s",indent,indent,indent,*(*I)->cName(),memory().visual().visible_now(*I) ? "visible" : "invisible");
	}

	if (memory().enemy().selected()) {
		HUD().Font().pFontStat->OutNext	("%s%sselected",indent,indent);
		
		float								fuzzy = 0.f;
		xr_vector<feel_visible_Item>::iterator I=feel_visible.begin(),E=feel_visible.end();
		for (; I!=E; I++)
			if (I->O->ID() == memory().enemy().selected()->ID()) {
				fuzzy						= I->fuzzy;
				break;
			}

		if (g_Alive()) {
			if (!g_mt_config.test(mtAiVision))
				VERIFY						(!memory().visual().visible_now(memory().enemy().selected()) || (fuzzy > 0.f));
			HUD().Font().pFontStat->OutNext("%s%s%svisible   : %s %f",indent,indent,indent,memory().visual().visible_now(memory().enemy().selected()) ? "+" : "-",fuzzy);
		}
		HUD().Font().pFontStat->OutNext	("%s%s%sobject    : %s",indent,indent,indent,*memory().enemy().selected()->cName());
		if (g_Alive()) {
			float								interval = (1.f - panic_threshold())*.25f, left = -1.f, right = -1.f;
			LPCSTR								description = "invalid";
			u32									result = dwfChooseAction(
				2000,
				1.f - interval,
				1.f - 2*interval,
				1.f - 3*interval,
				panic_threshold(),
				g_Team(),
				g_Squad(),
				g_Group(),
				0,
				1,
				2,
				3,
				4,
				this,
				300.f
			);
			switch (result) {
				case 0 : {
					description					= "attack";
					left						= 1.f;
					right						= 1.f - 1.f*interval;
					break;
				}
				case 1 : {
					description					= "careful attack";
					left						= 1.f - 1.f*interval;
					right						= 1.f - 2.f*interval;
					break;
				}
				case 2 : {
					description					= "defend";
					left						= 1.f - 2.f*interval;
					right						= 1.f - 3.f*interval;
					break;
				}
				case 3 : {
					description					= "retreat";
					left						= 1.f - 3*interval;
					right						= panic_threshold();
					break;
				}
				case 4 : {
					description					= "panic";
					left						= panic_threshold();
					right						= 0.f;
					break;
				}
				default : NODEFAULT;
			}
			HUD().Font().pFontStat->OutNext	("%s%s%svictory   : [%5.2f%%,%5.2f%%] -> %s",indent,indent,indent,100.f*right,100.f*left,description);
		}
	}
	// danger
	HUD().Font().pFontStat->OutNext	("%sdanger",indent);
	HUD().Font().pFontStat->OutNext	("%s%sobjects     : %d",indent,indent,memory().danger().objects().size());
	if (memory().danger().selected() && memory().danger().selected()->object()) {
		HUD().Font().pFontStat->OutNext	("%s%sselected",indent,indent);
		HUD().Font().pFontStat->OutNext	("%s%s%stype      : %s",indent,indent,indent,danger_type(memory().danger().selected()->type()));
		HUD().Font().pFontStat->OutNext	("%s%s%stime      : %.3f (%.3f)",indent,indent,indent,float(memory().danger().selected()->time())/1000.f,float(Device.dwTimeGlobal - memory().danger().selected()->time())/1000.f);
		HUD().Font().pFontStat->OutNext	("%s%s%sinitiator : %s",indent,indent,indent,*memory().danger().selected()->object()->cName());
		if (g_Alive() && memory().danger().selected()->object())
			HUD().Font().pFontStat->OutNext("%s%s%svisible   : %s",indent,indent,indent,memory().visual().visible_now(memory().danger().selected()->object()) ? "+" : "-");

		if (memory().danger().selected()->dependent_object() && !!memory().danger().selected()->dependent_object()->cName()) {
			HUD().Font().pFontStat->OutNext("%s%s%sdependent : %s",indent,indent,indent,*memory().danger().selected()->dependent_object()->cName());
			if (g_Alive())
				HUD().Font().pFontStat->OutNext("%s%s%svisible   : %s",indent,indent,indent,memory().visual().visible_now(smart_cast<const CGameObject*>(memory().danger().selected()->dependent_object())) ? "+" : "-");
		}
	}

	HUD().Font().pFontStat->OutNext	("%sanomalies",indent);
	HUD().Font().pFontStat->OutNext	("%s%sundetected  : %s",indent,indent,undetected_anomaly() ? "+" : "-");
	HUD().Font().pFontStat->OutNext	("%s%sinside      : %s",indent,indent,inside_anomaly() ? "+" : "-");

	// agent manager
	HUD().Font().pFontStat->OutNext	(" ");
	HUD().Font().pFontStat->OutNext	("agent manager");
	if (g_Alive()) {
	HUD().Font().pFontStat->OutNext	("%smembers           : %d",indent,agent_manager().member().members().size());
	HUD().Font().pFontStat->OutNext	("%senemies           : %d",indent,agent_manager().enemy().enemies().size());
	HUD().Font().pFontStat->OutNext	("%scorpses           : %d",indent,agent_manager().corpse().corpses().size());
	HUD().Font().pFontStat->OutNext	("%sdanger locations  : %d",indent,agent_manager().location().locations().size());
	HUD().Font().pFontStat->OutNext	("%smembers in combat : %d",indent,agent_manager().member().combat_members().size());
	if (g_Alive())
		HUD().Font().pFontStat->OutNext("%sI am in combat    : %s",indent,agent_manager().member().registered_in_combat(this) ? "+" : "-");
	HUD().Font().pFontStat->OutNext	("%smembers in detour : %d",indent,agent_manager().member().in_detour());
	if (g_Alive())
		HUD().Font().pFontStat->OutNext("%sI am in detour    : %s",indent,agent_manager().member().member(this).detour() ? "+" : "-");

		if (g_Alive()) {
			if (agent_manager().member().member(this).cover())
				HUD().Font().pFontStat->OutNext("%scover         : [%f][%f][%f]",indent,VPUSH(agent_manager().member().member(this).cover()->position()));

			if (agent_manager().member().member(this).member_death_reaction().m_processing)
				HUD().Font().pFontStat->OutNext("%react on death : %s",indent,*agent_manager().member().member(this).member_death_reaction().m_member->cName());

			if (agent_manager().member().member(this).grenade_reaction().m_processing)
				HUD().Font().pFontStat->OutNext("%react on grenade : %s",indent,agent_manager().member().member(this).grenade_reaction().m_game_object ? *agent_manager().member().member(this).grenade_reaction().m_game_object->cName() : "unknown");
		}
	}

	// objects
	HUD().Font().pFontStat->OutNext	(" ");
	HUD().Font().pFontStat->OutNext	("%sobjects",indent);
	HUD().Font().pFontStat->OutNext	("%s%sobjects             : %d",indent,indent,inventory().m_all.size());
	HUD().Font().pFontStat->OutNext	("%s%sactive item         : %s",indent,indent,inventory().ActiveItem() ? *inventory().ActiveItem()->object().cName() : "");
	HUD().Font().pFontStat->OutNext	("%s%sbest weapon         : %s",indent,indent,best_weapon() ? *best_weapon()->object().cName() : "");
	HUD().Font().pFontStat->OutNext	("%s%sitem to kill        : %s",indent,indent,item_to_kill() ? *m_best_item_to_kill->object().cName() : "");
	HUD().Font().pFontStat->OutNext	("%s%sitem can kill       : %s",indent,indent,item_can_kill() ? "+" : "-");
	HUD().Font().pFontStat->OutNext	("%s%smemory item to kill : %s",indent,indent,remember_item_to_kill() ? *m_best_found_item_to_kill->object().cName() : "");
	HUD().Font().pFontStat->OutNext	("%s%smemory ammo         : %s",indent,indent,remember_ammo() ? *m_best_found_ammo->object().cName() : "");
	HUD().Font().pFontStat->OutNext	("%s%sinfinite ammo       : %s",indent,indent,m_infinite_ammo ? "+" : "-");
	HUD().Font().pFontStat->OutNext	("%s%sitem to spawn       : %s",indent,indent,item_to_spawn().size() ? *item_to_spawn() : "no item to spawn");
	HUD().Font().pFontStat->OutNext	("%s%sammo in box to spawn: %d",indent,indent,item_to_spawn().size() ? ammo_in_box_to_spawn() : 0);
	
	CWeaponMagazined					*weapon = smart_cast<CWeaponMagazined*>(inventory().ActiveItem());
	if (weapon) {
		CObjectHandlerPlanner			&planner = CObjectHandler::planner();
		HUD().Font().pFontStat->OutNext("%s%squeue size          : %d",indent,indent,weapon->GetQueueSize());
		HUD().Font().pFontStat->OutNext("%s%squeue interval      : %d",
			indent,
			indent,
			planner.action(
				planner.uid(
					weapon->ID(),
					ObjectHandlerSpace::eWorldOperatorQueueWait1
				)
			).inertia_time()
		);
	}
	
	if (inventory().ActiveItem()) {
		HUD().Font().pFontStat->OutNext	("%s%sactive item",indent,indent);
		HUD().Font().pFontStat->OutNext	("%s%s%sobject         : %s",indent,indent,indent,inventory().ActiveItem() ? *inventory().ActiveItem()->object().cName() : "");
		CWeapon	*weapon = smart_cast<CWeapon*>(inventory().ActiveItem());
		if (weapon) {
			HUD().Font().pFontStat->OutNext("%s%s%sstrapped       : %s",indent,indent,indent,weapon_strapped(weapon) ? "+" : "-");
			HUD().Font().pFontStat->OutNext("%s%s%sunstrapped     : %s",indent,indent,indent,weapon_unstrapped(weapon) ? "+" : "-");
			HUD().Font().pFontStat->OutNext("%s%s%sammo           : %d",indent,indent,indent,weapon->GetAmmoElapsed());
			HUD().Font().pFontStat->OutNext("%s%s%smagazine       : %d",indent,indent,indent,weapon->GetAmmoMagSize());
			HUD().Font().pFontStat->OutNext("%s%s%stotal ammo     : %d",indent,indent,indent,weapon->GetAmmoCurrent());
		}
	}

	string256							temp;

	const CObjectHandlerPlanner			&objects = planner();
	strconcat							(sizeof(temp),temp,indent,indent);
	draw_planner						(objects,temp,indent,"root");

	HUD().Font().pFontStat->OutSet		(330,up_indent);
	
	// brain
	HUD().Font().pFontStat->OutNext	("brain");
	
	// actions
	draw_planner						(this->brain(),indent,indent,"root");

	// debug planner
	if (m_debug_planner)
		draw_planner					(*m_debug_planner,indent,indent,"debug_planner");
	
	HUD().Font().pFontStat->OutSet		(640,up_indent);
	// brain
	HUD().Font().pFontStat->OutNext	("controls");
	// animations
	HUD().Font().pFontStat->OutNext	("%sanimations",indent);

	HUD().Font().pFontStat->OutNext	("%s%shead        : [%s][%f]",indent,indent,
		animation_name(this,animation().head().animation()),
		animation().head().blend() ? animation().head().blend()->timeCurrent : 0.f
	);
	HUD().Font().pFontStat->OutNext	("%s%storso       : [%s][%f]",indent,indent,
		animation_name(this,animation().torso().animation()),
		animation().torso().blend() ? animation().torso().blend()->timeCurrent : 0.f
	);
	HUD().Font().pFontStat->OutNext	("%s%slegs        : [%s][%f]",indent,indent,
		animation_name(this,animation().legs().animation()),
		animation().legs().blend() ? animation().legs().blend()->timeCurrent : 0.f
	);
	HUD().Font().pFontStat->OutNext	("%s%sglobal      : [%s][%f]",indent,indent,
		animation_name(this,animation().global().animation()),
		animation().global().blend() ? animation().global().blend()->timeCurrent : 0.f
	);
	HUD().Font().pFontStat->OutNext	("%s%sscript      : [%s][%f]",indent,indent,
		animation_name(this,animation().script().animation()),
		animation().script().blend() ? animation().script().blend()->timeCurrent : 0.f
	);

	// movement
	HUD().Font().pFontStat->OutNext	(" ");
	HUD().Font().pFontStat->OutNext	("%smovement",indent);
	HUD().Font().pFontStat->OutNext	("%s%senabled         : %s",indent,indent,movement().enabled() ? "+" : "-");

	LPCSTR								mental_state = "invalid";
	switch (movement().mental_state()) {
		case MonsterSpace::eMentalStateFree : {
			mental_state				= "free";
			break;
		}
		case MonsterSpace::eMentalStateDanger : {
			mental_state				= "danger";
			break;
		}
		case MonsterSpace::eMentalStatePanic : {
			mental_state				= "panic";
			break;
		}
		default : NODEFAULT;
	}
	HUD().Font().pFontStat->OutNext	("%s%smental state    : %s",indent,indent,mental_state);

	LPCSTR								body_state = "invalid";
	switch (movement().body_state()) {
		case MonsterSpace::eBodyStateStand : {
			body_state					= "stand";
			break;
		}
		case MonsterSpace::eBodyStateCrouch : {
			body_state					= "crouch";
			break;
		}
		default : NODEFAULT;
	}
	HUD().Font().pFontStat->OutNext	("%s%sbody state      : %s",indent,indent,body_state);
	HUD().Font().pFontStat->OutNext	("%s%smovement type   : %s (current)",indent,indent,movement_type(movement().movement_type()));
	HUD().Font().pFontStat->OutNext	("%s%smovement type   : %s (target)",indent,indent, movement_type(movement().target_movement_type()));

	LPCSTR						path_type = "invalid";
	switch (movement().path_type()) {
		case MovementManager::ePathTypeGamePath : {
			path_type			= "game path";
			break;
		}
		case MovementManager::ePathTypeLevelPath : {
			path_type			= "level path";
			break;
		}
		case MovementManager::ePathTypePatrolPath : {
			path_type			= "patrol path";
			break;
		}
		case MovementManager::ePathTypeNoPath : {
			path_type			= "no path";
			break;
		}
		default : NODEFAULT;
	}
	HUD().Font().pFontStat->OutNext	("%s%spath type       : %s",indent,indent,path_type);
	HUD().Font().pFontStat->OutNext	("%s%sposition        : [%f][%f][%f]",indent,indent,VPUSH(Position()));
	HUD().Font().pFontStat->OutNext	("%s%slevel vertex id : %d",indent,indent,ai_location().level_vertex_id());
	HUD().Font().pFontStat->OutNext	("%s%sgame vertex id  : %d",indent,indent,ai_location().game_vertex_id());
	HUD().Font().pFontStat->OutNext	("%s%shead current    : [%f][%f]",indent,indent,movement().head_orientation().current.yaw,movement().head_orientation().current.pitch);
	HUD().Font().pFontStat->OutNext	("%s%shead target     : [%f][%f]",indent,indent,movement().head_orientation().target.yaw,movement().head_orientation().target.pitch);
	HUD().Font().pFontStat->OutNext	("%s%sbody current    : [%f][%f]",indent,indent,movement().body_orientation().current.yaw,movement().body_orientation().current.pitch);
	HUD().Font().pFontStat->OutNext	("%s%sbody target     : [%f][%f]",indent,indent,movement().body_orientation().target.yaw,movement().body_orientation().target.pitch);
	
	if (movement().path_type() == MovementManager::ePathTypePatrolPath) {
		HUD().Font().pFontStat->OutNext("%s%spatrol",indent,indent);
		HUD().Font().pFontStat->OutNext("%s%s%spath          : %s",indent,indent,indent,*movement().patrol().path_name());
		HUD().Font().pFontStat->OutNext("%s%s%scompleted     : %s",indent,indent,indent,movement().patrol().completed() ? "+" : "-");
		HUD().Font().pFontStat->OutNext("%s%s%scurrent point : %d",indent,indent,indent,movement().patrol().get_current_point_index());
		if	(
				movement().patrol().get_path()
				&&
				movement().patrol().get_path()->vertex(movement().patrol().get_current_point_index())
			)
			HUD().Font().pFontStat->OutNext("%s%s%sextrapolate   : %s",indent,indent,indent,movement().patrol().extrapolate_path() ? "+" : "-");
		else
			HUD().Font().pFontStat->OutNext("%s%s%sextrapolate   : unknown",indent,indent,indent);
	}

	if (movement().path_type() == MovementManager::ePathTypeGamePath) {
		HUD().Font().pFontStat->OutNext("%s%sgame",indent,indent);
		HUD().Font().pFontStat->OutNext("%s%s%scompleted     : %s",indent,indent,indent,movement().game_path().completed() ? "+" : "-");
		HUD().Font().pFontStat->OutNext("%s%s%spath size     : %d",indent,indent,indent,movement().game_path().path().size());
		HUD().Font().pFontStat->OutNext("%s%s%scurrent point : %d",indent,indent,indent,movement().game_path().intermediate_index());
	}
	
	HUD().Font().pFontStat->OutNext	("%s%slevel",indent,indent);
	HUD().Font().pFontStat->OutNext	("%s%s%spath size     : %d",indent,indent,indent,movement().level_path().path().size());
	HUD().Font().pFontStat->OutNext	("%s%s%sstart vertex  : %d",indent,indent,indent,movement().level_path().path().empty() ? -1 : movement().level_path().path().front());
	HUD().Font().pFontStat->OutNext	("%s%s%sdest vertex   : %d",indent,indent,indent,movement().level_path().path().empty() ? -1 : movement().level_path().path().back());

	HUD().Font().pFontStat->OutNext	("%s%sdetail",indent,indent);
	HUD().Font().pFontStat->OutNext	("%s%s%svelocities    : %d",indent,indent,indent,movement().detail().velocities().size());
	HUD().Font().pFontStat->OutNext	("%s%s%sextrapolate   : %f",indent,indent,indent,movement().detail().extrapolate_length());
	HUD().Font().pFontStat->OutNext	("%s%s%spath size     : %d",indent,indent,indent,movement().detail().path().size());
	if (!movement().detail().path().empty()) {
		HUD().Font().pFontStat->OutNext	("%s%s%sstart point   : [%f][%f][%f]",indent,indent,indent,VPUSH(movement().detail().path().front().position));
		HUD().Font().pFontStat->OutNext	("%s%s%sdest point    : [%f][%f][%f]",indent,indent,indent,VPUSH(movement().detail().path().back().position));
		HUD().Font().pFontStat->OutNext	("%s%s%scurrent point",indent,indent,indent);
		HUD().Font().pFontStat->OutNext	("%s%s%s%sindex     : %d",indent,indent,indent,indent,movement().detail().curr_travel_point_index());
		HUD().Font().pFontStat->OutNext	("%s%s%s%sposition  : [%f][%f][%f]",indent,indent,indent,indent,VPUSH(movement().detail().path()[movement().detail().curr_travel_point_index()].position));
		CDetailPathManager::STravelParams	current_velocity = movement().detail().velocity(movement().detail().path()[movement().detail().curr_travel_point_index()].velocity);
		HUD().Font().pFontStat->OutNext	("%s%s%s%slinear    : %f",    indent,indent,indent,indent,current_velocity.linear_velocity);
		HUD().Font().pFontStat->OutNext	("%s%s%s%sangular   : %f deg",indent,indent,indent,indent,rad2deg(current_velocity.angular_velocity));
		HUD().Font().pFontStat->OutNext	("%s%s%s%sangular(R): %f deg",indent,indent,indent,indent,rad2deg(current_velocity.real_angular_velocity));
		HUD().Font().pFontStat->OutNext	("%s%s%sspeed(calc)   : %f",indent,indent,indent,movement().speed());
		HUD().Font().pFontStat->OutNext	("%s%s%sspeed(physics): %f",indent,indent,indent,movement().speed(character_physics_support()->movement()));
	}

	if (movement().detail().use_dest_orientation())
		HUD().Font().pFontStat->OutNext("%s%s%sorientation   : + [%f][%f][%f]",indent,indent,indent,VPUSH(movement().detail().dest_direction()));
	else
		HUD().Font().pFontStat->OutNext("%s%s%sorientation   : -",indent,indent,indent);

	if	(
			movement().restrictions().out_restrictions().size() ||
			movement().restrictions().in_restrictions().size() ||
			movement().restrictions().base_out_restrictions().size() ||
			movement().restrictions().base_in_restrictions().size()
		) {
		HUD().Font().pFontStat->OutNext	("%s%srestrictions",indent,indent);
		strconcat							(sizeof(temp),temp,indent,indent,indent);
		draw_restrictions					(movement().restrictions().out_restrictions(),temp,indent,"out");
		draw_restrictions					(movement().restrictions().in_restrictions(),temp,indent,"in");
		draw_restrictions					(movement().restrictions().base_out_restrictions(),temp,indent,"base out");
		draw_restrictions					(movement().restrictions().base_in_restrictions(),temp,indent,"base in");
	}

	// sounds
	HUD().Font().pFontStat->OutNext	(" ");
	HUD().Font().pFontStat->OutNext	("%ssounds",indent);
	HUD().Font().pFontStat->OutNext	("%s%scollections : %d",indent,indent,sound().objects().size());
	
	{
		u32			object_count = 0;
		CSoundPlayer::SOUND_COLLECTIONS::const_iterator	I = sound().objects().begin();
		CSoundPlayer::SOUND_COLLECTIONS::const_iterator	E = sound().objects().end();
		for ( ; I != E; ++I)
			object_count	+= (*I).second.second->m_sounds.size();
		HUD().Font().pFontStat->OutNext("%s%sobjects     : %d",indent,indent,object_count);
	}
	{
		xr_vector<CSoundPlayer::CSoundSingle>::const_iterator	I = sound().playing_sounds().begin();
		xr_vector<CSoundPlayer::CSoundSingle>::const_iterator	E = sound().playing_sounds().end();
		for ( ; I != E; ++I)
			HUD().Font().pFontStat->OutNext(
				"%s%s%s[%s]%s",
				indent,
				indent,
				indent,
				(Device.dwTimeGlobal < (*I).m_start_time)
				?
				"not yet started"
				:
				(
					(*I).m_sound->_feedback()
					?
					"playing"
					:
					"already played"
				),
				(*I).m_sound->_handle() ? (*I).m_sound->_handle()->file_name() : "no source"
			);
	}

	// sight
	HUD().Font().pFontStat->OutNext	(" ");
	HUD().Font().pFontStat->OutNext	("%ssight",indent);

	LPCSTR								sight_type = "invalid";
	switch (sight().current_action().sight_type()) {
		case SightManager::eSightTypeCurrentDirection : {
			sight_type					= "current direction";
			break;
		}
		case SightManager::eSightTypePathDirection : {
			sight_type					= "path direction";
			break;
		}
		case SightManager::eSightTypeDirection : {
			sight_type					= "direction";
			break;
		}
		case SightManager::eSightTypePosition : {
			sight_type					= "position";
			break;
		}
		case SightManager::eSightTypeObject : {
			sight_type					= "object";
			break;
		}
		case SightManager::eSightTypeCover : {
			sight_type					= "cover";
			break;
		}
		case SightManager::eSightTypeSearch : {
			sight_type					= "search";
			break;
		}
		case SightManager::eSightTypeLookOver : {
			sight_type					= "look over";
			break;
		}
		case SightManager::eSightTypeCoverLookOver : {
			sight_type					= "cover look over";
			break;
		}
		case SightManager::eSightTypeFireObject : {
			sight_type					= "fire object";
			break;
		}
		default : NODEFAULT;
	}

	HUD().Font().pFontStat->OutNext	("%s%stype            : %s",indent,indent,sight_type);
	HUD().Font().pFontStat->OutNext	("%s%suse torso       : %s",indent,indent,sight().current_action().use_torso_look() ? "+" : "-");
	
	switch (sight().current_action().sight_type()) {
		case SightManager::eSightTypeCurrentDirection : {
			break;
		}
		case SightManager::eSightTypePathDirection : {
			break;
		}
		case SightManager::eSightTypeDirection : {
			HUD().Font().pFontStat->OutNext	("%s%sdirection       : [%f][%f][%f]",indent,indent,VPUSH(sight().current_action().vector3d()));
			break;
		}
		case SightManager::eSightTypePosition : {
			HUD().Font().pFontStat->OutNext	("%s%sposition        : [%f][%f][%f]",indent,indent,VPUSH(sight().current_action().vector3d()));
			break;
		}
		case SightManager::eSightTypeObject : {
			HUD().Font().pFontStat->OutNext	("%s%sobject          : %s",indent,indent,*sight().current_action().object().cName());
			HUD().Font().pFontStat->OutNext	("%s%sposition        : [%f][%f][%f]",indent,indent,VPUSH(sight().current_action().object().Position()));
			break;
		}
		case SightManager::eSightTypeCover : {
			sight_type					= "cover";
			break;
		}
		case SightManager::eSightTypeSearch : {
			sight_type					= "search";
			break;
		}
		case SightManager::eSightTypeLookOver : {
			sight_type					= "look over";
			break;
		}
		case SightManager::eSightTypeCoverLookOver : {
			sight_type					= "cover look over";
			break;
		}
		case SightManager::eSightTypeFireObject : {
			HUD().Font().pFontStat->OutNext	("%s%sobject          : %s",indent,indent,*sight().current_action().object().cName());
			HUD().Font().pFontStat->OutNext	("%s%sposition        : [%f][%f][%f]",indent,indent,VPUSH(sight().current_action().object().Position()));
			HUD().Font().pFontStat->OutNext	("%s%svisible point   : %s",indent,indent,false ? "-" : "+");
			break;
		}
		default : NODEFAULT;
	}
}

void CAI_Stalker::OnRender			()
{
	if (inventory().ActiveItem()) {
		Fvector					position, direction, temp;
		g_fireParams			(0,position,direction);
		temp					= direction;
		temp.mul				(1.f);
		temp.add				(position);
		Level().debug_renderer().draw_line		(Fidentity,position,temp,D3DCOLOR_XRGB(0*255,255,0*255));
	}

	if (IsMyCamera()) {
		if (!g_Alive())
			return;

		if (!memory().enemy().selected() || !memory().visual().visible_now(memory().enemy().selected()))
			return;

		xr_vector<CObject*>		objects;
		feel_vision_get			(objects);
		if (std::find(objects.begin(),objects.end(),memory().enemy().selected()) != objects.end()) {
			Fvector				position = feel_vision_get_vispoint(const_cast<CEntityAlive*>(memory().enemy().selected()));
			Level().debug_renderer().draw_aabb	(position,.05f,.05f,.05f,D3DCOLOR_XRGB(0*255,255,0*255));
			return;
		}

		return;
	}

	inherited::OnRender		();

	{
		Fvector					c0 = Position(),c1,t0 = Position(),t1;
		c0.y					+= 2.f;
		c1.setHP				(-movement().m_body.current.yaw,-movement().m_body.current.pitch);
		c1.add					(c0);
		Level().debug_renderer().draw_line		(Fidentity,c0,c1,D3DCOLOR_XRGB(0,255,0));
		
		t0.y					+= 2.f;
		t1.setHP				(-movement().m_body.target.yaw,-movement().m_body.target.pitch);
		t1.add					(t0);
		Level().debug_renderer().draw_line		(Fidentity,t0,t1,D3DCOLOR_XRGB(255,0,0));
	}

	if (memory().danger().selected() && ai().level_graph().valid_vertex_position(memory().danger().selected()->position())) {
		Fvector						position = memory().danger().selected()->position();
		u32							level_vertex_id = ai().level_graph().vertex_id(position);
		float						half_size = ai().level_graph().header().cell_size()*.5f;
		position.y					+= 1.f;
		Level().debug_renderer().draw_aabb	(position,half_size - .01f,1.f,ai().level_graph().header().cell_size()*.5f-.01f,D3DCOLOR_XRGB(0*255,255,0*255));

		if (ai().level_graph().valid_vertex_id(level_vertex_id)) {
			LevelGraph::CVertex			*v = ai().level_graph().vertex(level_vertex_id);
			Fvector						direction;
			float						best_value = -1.f;

			for (u32 i=0, j = 0; i<36; ++i) {
				float				value = ai().level_graph().cover_in_direction(float(10*i)/180.f*PI,v);
				direction.setHP		(float(10*i)/180.f*PI,0);
				direction.normalize	();
				direction.mul		(value*half_size);
				direction.add		(position);
				direction.y			= position.y;
				Level().debug_renderer().draw_line	(Fidentity,position,direction,D3DCOLOR_XRGB(0,0,255));
				value				= ai().level_graph().compute_square(float(10*i)/180.f*PI,PI/2.f,v);
				if (value > best_value) {
					best_value		= value;
					j				= i;
				}
			}

			direction.set		(position.x - half_size*float(v->cover(0))/15.f,position.y,position.z);
			Level().debug_renderer().draw_line(Fidentity,position,direction,D3DCOLOR_XRGB(255,0,0));

			direction.set		(position.x,position.y,position.z + half_size*float(v->cover(1))/15.f);
			Level().debug_renderer().draw_line(Fidentity,position,direction,D3DCOLOR_XRGB(255,0,0));

			direction.set		(position.x + half_size*float(v->cover(2))/15.f,position.y,position.z);
			Level().debug_renderer().draw_line(Fidentity,position,direction,D3DCOLOR_XRGB(255,0,0));

			direction.set		(position.x,position.y,position.z - half_size*float(v->cover(3))/15.f);
			Level().debug_renderer().draw_line(Fidentity,position,direction,D3DCOLOR_XRGB(255,0,0));

			float				value = ai().level_graph().cover_in_direction(float(10*j)/180.f*PI,v);
			direction.setHP		(float(10*j)/180.f*PI,0);
			direction.normalize	();
			direction.mul		(value*half_size);
			direction.add		(position);
			direction.y			= position.y;
			Level().debug_renderer().draw_line	(Fidentity,position,direction,D3DCOLOR_XRGB(0,0,0));
		}
	}
}

void CAI_Stalker::dbg_draw_vision	()
{
	VERIFY						(!!psAI_Flags.is(aiVision));

	if (!smart_cast<CGameObject*>(Level().CurrentEntity()))
		return;

	Fvector						shift;
	shift.set					(0.f,2.5f,0.f);

	Fmatrix						res;
	res.mul						(Device.mFullTransform,XFORM());

	Fvector4					v_res;

	res.transform				(v_res,shift);

	if (v_res.z < 0 || v_res.w < 0)
		return;

	if (v_res.x < -1.f || v_res.x > 1.f || v_res.y<-1.f || v_res.y>1.f)
		return;

	float						x = (1.f + v_res.x)/2.f * (Device.dwWidth);
	float						y = (1.f - v_res.y)/2.f * (Device.dwHeight);

	CNotYetVisibleObject		*object = memory().visual().not_yet_visible_object(smart_cast<CGameObject*>(Level().CurrentEntity()));
	string64					out_text;
	sprintf_s						(out_text,"%.2f",object ? object->m_value : 0.f);

	HUD().Font().pFontMedium->SetColor	(D3DCOLOR_RGBA(255,0,0,95));
	HUD().Font().pFontMedium->OutSet	(x,y);
	HUD().Font().pFontMedium->OutNext	(out_text);
}

typedef xr_vector<Fvector>	COLLIDE_POINTS;

class ray_query_param	{
public:
	CCustomMonster			*m_holder;
	float					m_power;
	float					m_power_threshold;
	float					m_pick_distance;
	Fvector					m_start_position;
	Fvector					m_direction;
	COLLIDE_POINTS			*m_points;

public:
	IC				ray_query_param		(CCustomMonster *holder, float power_threshold, float distance, const Fvector &start_position, const Fvector &direction, COLLIDE_POINTS &points)
	{
		m_holder			= holder;
		m_power				= 1.f;
		m_power_threshold	= power_threshold;
		m_pick_distance		= distance;
		m_start_position	= start_position;
		m_direction			= direction;
		m_points			= &points;
	}
};

BOOL _ray_query_callback	(collide::rq_result& result, LPVOID params)
{
	ray_query_param						*param = (ray_query_param*)params;
	param->m_points->push_back			(
		Fvector().mad(
			param->m_start_position,
			param->m_direction,
			result.range
		)
	);
	
	float								power = param->m_holder->feel_vision_mtl_transp(result.O,result.element);
	param->m_power						*= power;
	if (param->m_power > param->m_power_threshold)
		return							(true);

	param->m_pick_distance				= result.range;
	return								(false);
}

void fill_points			(CCustomMonster *self, const Fvector &position, const Fvector &direction, float distance, collide::rq_results& rq_storage, COLLIDE_POINTS &points, float &pick_distance)
{
	VERIFY							(!fis_zero(direction.square_magnitude()));

	collide::ray_defs				ray_defs(position,direction,distance,CDB::OPT_CULL,collide::rqtBoth);
	VERIFY							(!fis_zero(ray_defs.dir.square_magnitude()));
	
	ray_query_param					params(self,self->memory().visual().transparency_threshold(),distance,position,direction,points);

	Level().ObjectSpace.RayQuery	(rq_storage,ray_defs,_ray_query_callback,&params,NULL,self);

	pick_distance					= params.m_pick_distance;
}

void draw_visiblity_rays	(CCustomMonster *self, const CObject *object, collide::rq_results& rq_storage)
{
	typedef Feel::Vision::feel_visible_Item		feel_visible_Item;
	typedef xr_vector<feel_visible_Item>		VISIBLE_ITEMS;

	feel_visible_Item		*item = 0;
	{
		VISIBLE_ITEMS::iterator	I = self->feel_visible.begin();
		VISIBLE_ITEMS::iterator	E = self->feel_visible.end();
		for ( ; I!=E; ++I) {
			if ((*I).O == object) {
				item		= &*I;
				break;
			}
		}
	}
	
	if (!item)
		return;

	Fvector					start_position = self->eye_matrix.c;
	Fvector					dest_position = item->cp_LAST;
	Fvector					direction = Fvector().sub(dest_position,start_position);
	float					distance = direction.magnitude();
	direction.normalize		();
	float					pick_distance = flt_max;
	rq_storage.r_clear		();
	COLLIDE_POINTS			points;
	points.push_back		(start_position);
	fill_points				(
		self,
		start_position,
		direction,
		distance,
		rq_storage,
		points,
		pick_distance
	);

//	VERIFY					(fsimilar(pick_distance,distance));
	if (fsimilar(pick_distance,distance) && !dest_position.similar(points.back()))
		points.push_back	(dest_position);

	VERIFY					(points.size() > 1);
	
	Fvector					size = Fvector().set(.05f,.05f,.05f);
	Level().debug_renderer().draw_aabb	(points.front(),size.x,size.y,size.z,D3DCOLOR_XRGB(0,0,255));

	{
		COLLIDE_POINTS::const_iterator	I = points.begin() + 1;
		COLLIDE_POINTS::const_iterator	E = points.end();
		for ( ; I != E; ++I) {
			Level().debug_renderer().draw_line	(Fidentity,*(I-1),*I,D3DCOLOR_XRGB(0,255,0));
			Level().debug_renderer().draw_aabb	(*I,size.x,size.y,size.z,D3DCOLOR_XRGB(0,255,0));
		}
	}

	Level().debug_renderer().draw_aabb	(points.back(),size.x,size.y,size.z,D3DCOLOR_XRGB(255,0,0));
}

void CAI_Stalker::dbg_draw_visibility_rays	()
{
	if (!g_Alive())
		return;

	const CEntityAlive		*enemy = memory().enemy().selected();
	if (enemy) {
		if (memory().visual().visible_now(enemy)) {
			collide::rq_results	rq_storage;
			draw_visiblity_rays	(this,enemy,rq_storage);
		}
	}
}

#endif // DEBUG
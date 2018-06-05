////////////////////////////////////////////////////////////////////////////
//	Module 		: level_script.cpp
//	Created 	: 28.06.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Level script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "level.h"
#include "actor.h"
#include "script_game_object.h"
#include "patrol_path_storage.h"
#include "xrServer.h"
#include "client_spawn_manager.h"
#include "..\xr_3da\IGame_Persistent.h"
#include "game_cl_base.h"
#include "ui/UIDialogWnd.h"
#include "date_time.h"
#include "ai_space.h"
#include "level_graph.h"
#include "PHCommander.h"
#include "PHScriptCall.h"
#include "HUDManager.h"
#include "script_engine.h"
#include "game_cl_single.h"
#include "alife_simulator.h"
#include "alife_time_manager.h"
#include "map_manager.h"
#include "map_location.h"
#include "phworld.h"
#include "../xr_3da/r2_shader_exports.h"
#include "../xr_3da/xr_collide_defs.h"
#include "script_rq_result.h"
#include "monster_community.h"

using namespace luabind;

LPCSTR command_line	()
{
	return		(Core.Params);
}

#ifdef DEBUG
void check_object(CScriptGameObject *object)
{
	try {
		Msg	("check_object %s",object->Name());
	}
	catch(...) {
		object = object;
	}
}

CScriptGameObject *tpfGetActor()
{
	static bool first_time = true;
	if (first_time)
		ai().script_engine().script_log(eLuaMessageTypeError,"Do not use level.actor function!");
	first_time = false;
	
	CActor *l_tpActor = smart_cast<CActor*>(Level().CurrentEntity());
	if (l_tpActor)
		return	(smart_cast<CGameObject*>(l_tpActor)->lua_game_object());
	else
		return	(0);
}

CScriptGameObject *get_object_by_name(LPCSTR caObjectName)
{
	static bool first_time = true;
	if (first_time)
		ai().script_engine().script_log(eLuaMessageTypeError,"Do not use level.object function!");
	first_time = false;
	
	CGameObject		*l_tpGameObject	= smart_cast<CGameObject*>(Level().Objects.FindObjectByName(caObjectName));
	if (l_tpGameObject)
		return		(l_tpGameObject->lua_game_object());
	else
		return		(0);
}
#endif

CScriptGameObject *get_object_by_id(u32 id)
{
	CGameObject* pGameObject = smart_cast<CGameObject*>(Level().Objects.net_Find(id));
	if(!pGameObject)
		return NULL;

	return pGameObject->lua_game_object();
}

LPCSTR get_weather	()
{
	return			(*g_pGamePersistent->Environment().GetWeather());
}

void set_weather	(LPCSTR weather_name, bool forced)
{
	return			(g_pGamePersistent->Environment().SetWeather(weather_name,forced));
}

bool set_weather_fx	(LPCSTR weather_name)
{
	return			(g_pGamePersistent->Environment().SetWeatherFX(weather_name));
}

bool is_wfx_playing	()
{
	return			(g_pGamePersistent->Environment().IsWFXPlaying());
}

void set_time_factor(float time_factor)
{
	if (!OnServer())
		return;

	Level().Server->game->SetGameTimeFactor(time_factor);
}

float get_time_factor()
{
	return			(Level().GetGameTimeFactor());
}

void set_game_difficulty(ESingleGameDifficulty dif)
{
	g_SingleGameDifficulty		= dif;
	game_cl_Single* game		= smart_cast<game_cl_Single*>(Level().game); VERIFY(game);
	game->OnDifficultyChanged	();
}
ESingleGameDifficulty get_game_difficulty()
{
	return g_SingleGameDifficulty;
}

u32 get_time_days()
{
	u32 year = 0, month = 0, day = 0, hours = 0, mins = 0, secs = 0, milisecs = 0;
	split_time(Level().GetGameTime(), year, month, day, hours, mins, secs, milisecs);
	return			day;
}

u32 get_time_hours()
{
	u32 year = 0, month = 0, day = 0, hours = 0, mins = 0, secs = 0, milisecs = 0;
	split_time(Level().GetGameTime(), year, month, day, hours, mins, secs, milisecs);
	return			hours;
}

u32 get_time_minutes()
{
	u32 year = 0, month = 0, day = 0, hours = 0, mins = 0, secs = 0, milisecs = 0;
	split_time(Level().GetGameTime(), year, month, day, hours, mins, secs, milisecs);
	return			mins;
}

float cover_in_direction(u32 level_vertex_id, const Fvector &direction)
{
	float			y,p;
	direction.getHP	(y,p);
	return			(ai().level_graph().cover_in_direction(y,level_vertex_id));
}

float rain_factor()
{
	return			(g_pGamePersistent->Environment().CurrentEnv.rain_density);
}

u32	vertex_in_direction(u32 level_vertex_id, Fvector direction, float max_distance)
{
	direction.normalize_safe();
	direction.mul	(max_distance);
	Fvector			start_position = ai().level_graph().vertex_position(level_vertex_id);
	Fvector			finish_position = Fvector(start_position).add(direction);
	u32				result = u32(-1);
	ai().level_graph().farthest_vertex_in_direction(level_vertex_id,start_position,finish_position,result,0);
	return			(ai().level_graph().valid_vertex_id(result) ? result : level_vertex_id);
}

Fvector vertex_position(u32 level_vertex_id)
{
	return			(ai().level_graph().vertex_position(level_vertex_id));
}

void map_add_object_spot(u16 id, LPCSTR spot_type, LPCSTR text)
{
	CMapLocation* ml = Level().MapManager().AddMapLocation(spot_type,id);
	if( xr_strlen(text) )
			ml->SetHint(text);
}

void map_add_object_spot_ser(u16 id, LPCSTR spot_type, LPCSTR text)
{
	CMapLocation* ml = Level().MapManager().AddMapLocation(spot_type,id);
	if( xr_strlen(text) )
			ml->SetHint(text);

	ml->SetSerializable(true);
}

void map_change_spot_hint(u16 id, LPCSTR spot_type, LPCSTR text)
{
	CMapLocation* ml	= Level().MapManager().GetMapLocation(spot_type, id);
	if(!ml)				return;
	ml->SetHint			(text);
}

void map_remove_object_spot(u16 id, LPCSTR spot_type)
{
	Level().MapManager().RemoveMapLocation(spot_type, id);
}

u16 map_has_object_spot(u16 id, LPCSTR spot_type)
{
	return Level().MapManager().HasMapLocation(spot_type, id);
}

bool patrol_path_exists(LPCSTR patrol_path)
{
	return		(!!ai().patrol_paths().path(patrol_path,true));
}

LPCSTR get_name()
{
	return		(*Level().name());
}

void prefetch_sound	(LPCSTR name)
{
	Level().PrefetchSound(name);
}


CClientSpawnManager	&get_client_spawn_manager()
{
	return		(Level().client_spawn_manager());
}

void start_stop_menu(CUIDialogWnd* pDialog, bool bDoHideIndicators)
{
	HUD().GetUI()->StartStopMenu(pDialog,bDoHideIndicators);
}


void add_dialog_to_render(CUIDialogWnd* pDialog)
{
	HUD().GetUI()->AddDialogToRender(pDialog);
}

void remove_dialog_to_render(CUIDialogWnd* pDialog)
{
	HUD().GetUI()->RemoveDialogToRender(pDialog);
}

CUIDialogWnd* main_input_receiver()
{
	return HUD().GetUI()->MainInputReceiver();
}

#include "UIGameCustom.h"
#include "ui/UIInventoryWnd.h"
#include "ui/UITradeWnd.h"
#include "ui/UITalkWnd.h"
#include "uigamesp.h"
#include "HUDManager.h"
#include "HUDTarget.h"
CUIWindow* GetInventoryWindow()
{
	CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if (!pGameSP) return nullptr;
	return (CUIWindow*)pGameSP->InventoryMenu;
}
CUIWindow* GetTradeWindow()
{
	CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if (!pGameSP) return nullptr;
	return (CUIWindow*)pGameSP->TalkMenu->GetTradeWnd();
}
CUIWindow* GetTalkWindow()
{
	CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if (!pGameSP) return nullptr;
	return (CUIWindow*)pGameSP->TalkMenu;
}
CGameObject* GetSecondTalker()
{
	CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if (!pGameSP) return nullptr;
	CUITalkWnd* wnd = pGameSP->TalkMenu;
	if (wnd == nullptr) return nullptr;
	return smart_cast<CGameObject*>(wnd->GetSecondTalker());
}

CUIWindow* GetUIChangeLevelWnd()
{
	CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if (!pGameSP) return nullptr;
	return (CUIWindow*)pGameSP->UIChangeLevelWnd;
}

script_rq_result PerformRayQuery(Fvector start, Fvector dir, float range, collide::rq_target tgt, CScriptGameObject* ignore)
{
	collide::rq_result RQ;
	script_rq_result res;
	CObject *obj = nullptr;
	if (ignore)
		obj = smart_cast<CObject *>(&(ignore->object()));
	if (Level().ObjectSpace.RayPick(start, dir, range, tgt, RQ, obj))
	{
		res.set_result(RQ);
	}
	return res;
}

/*void DisableActorCallbacks(u32 _fl)
{
	psCallbackFlags.set(_fl, true);
}*/

float GetTargetDist()
{
	return ((CHUDManager *)g_hud)->GetTarget()->GetDist();
}

CScriptGameObject *GetTargetObj()
{
	CObject *obj = ((CHUDManager *)g_hud)->GetTarget()->GetObj();
	if (!obj)
		return nullptr;
	return smart_cast<CGameObject *>(obj)->lua_game_object();
}

void hide_indicators()
{
	HUD().GetUI()->UIGame()->HideShownDialogs();

	HUD().GetUI()->HideGameIndicators();
	HUD().GetUI()->HideCrosshair();
}

void show_indicators()
{
	HUD().GetUI()->ShowGameIndicators();
	HUD().GetUI()->ShowCrosshair();
}


bool is_level_present()
{
	return (!!g_pGameLevel);
}

CPHCall* add_call(const luabind::functor<bool> &condition,const luabind::functor<void> &action)
{
	luabind::functor<bool>		_condition = condition;
	luabind::functor<void>		_action = action;
	CPHScriptCondition	* c=xr_new<CPHScriptCondition>(_condition);
	CPHScriptAction		* a=xr_new<CPHScriptAction>(_action);
	return Level().ph_commander_scripts().add_call(c,a);
}

void remove_call(const luabind::functor<bool> &condition,const luabind::functor<void> &action)
{
	CPHScriptCondition	c(condition);
	CPHScriptAction		a(action);
	Level().ph_commander_scripts().remove_call(&c,&a);
}

CPHCall* add_call(const luabind::object &lua_object, LPCSTR condition,LPCSTR action)
{
//	try{	
//		CPHScriptObjectCondition	*c=xr_new<CPHScriptObjectCondition>(lua_object,condition);
//		CPHScriptObjectAction		*a=xr_new<CPHScriptObjectAction>(lua_object,action);
		luabind::functor<bool>		_condition = object_cast<luabind::functor<bool> >(lua_object[condition]);
		luabind::functor<void>		_action = object_cast<luabind::functor<void> >(lua_object[action]);
		CPHScriptObjectConditionN	*c=xr_new<CPHScriptObjectConditionN>(lua_object,_condition);
		CPHScriptObjectActionN		*a=xr_new<CPHScriptObjectActionN>(lua_object,_action);
		return Level().ph_commander_scripts().add_call_unique(c,c,a,a);
//	}
//	catch(...)
//	{
//		Msg("add_call excepted!!");
//	}
}

void remove_call(const luabind::object &lua_object, LPCSTR condition,LPCSTR action)
{
	CPHScriptObjectCondition	c(lua_object,condition);
	CPHScriptObjectAction		a(lua_object,action);
	Level().ph_commander_scripts().remove_call(&c,&a);
}

CPHCall* add_call(const luabind::object &lua_object, const luabind::functor<bool> &condition,const luabind::functor<void> &action)
{

	CPHScriptObjectConditionN	*c=xr_new<CPHScriptObjectConditionN>(lua_object,condition);
	CPHScriptObjectActionN		*a=xr_new<CPHScriptObjectActionN>(lua_object,action);
	return Level().ph_commander_scripts().add_call(c,a);
}

void remove_call(const luabind::object &lua_object, const luabind::functor<bool> &condition,const luabind::functor<void> &action)
{
	CPHScriptObjectConditionN	c(lua_object,condition);
	CPHScriptObjectActionN		a(lua_object,action);
	Level().ph_commander_scripts().remove_call(&c,&a);
}

void remove_calls_for_object(const luabind::object &lua_object)
{
	CPHSriptReqObjComparer c(lua_object);
	Level().ph_commander_scripts().remove_calls(&c);
}

CPHWorld* physics_world()
{
	return	ph_world;
}
CEnvironment *environment()
{
	return		(g_pGamePersistent->pEnvironment);
}

CEnvDescriptor *current_environment(CEnvironment *self)
{
	return		(&self->CurrentEnv);
}
extern bool g_bDisableAllInput;
void disable_input()
{
	g_bDisableAllInput = true;
}
void enable_input()
{
	g_bDisableAllInput = false;
}

void spawn_phantom(const Fvector &position)
{
	Level().spawn_item("m_phantom", position, u32(-1), u16(-1), false);
}

Fbox get_bounding_volume()
{
	return Level().ObjectSpace.GetBoundingVolume();
}

void iterate_sounds					(LPCSTR prefix, u32 max_count, const CScriptCallbackEx<void> &callback)
{
	for (int j=0, N = _GetItemCount(prefix); j<N; ++j) {
		string_path					fn, s;
		LPSTR						S = (LPSTR)&s;
		_GetItem					(prefix,j,S);
		if (FS.exist(fn,"$game_sounds$",S,".ogg"))
			callback				(prefix);

		for (u32 i=0; i<max_count; ++i)
		{
			string_path					name;
			sprintf_s					(name,"%s%d",S,i);
			if (FS.exist(fn,"$game_sounds$",name,".ogg"))
				callback			(name);
		}
	}
}

void iterate_sounds1				(LPCSTR prefix, u32 max_count, luabind::functor<void> functor)
{
	CScriptCallbackEx<void>		temp;
	temp.set					(functor);
	iterate_sounds				(prefix,max_count,temp);
}

void iterate_sounds2				(LPCSTR prefix, u32 max_count, luabind::object object, luabind::functor<void> functor)
{
	CScriptCallbackEx<void>		temp;
	temp.set					(functor,object);
	iterate_sounds				(prefix,max_count,temp);
}

#include "actoreffector.h"
float add_cam_effector(LPCSTR fn, int id, bool cyclic, LPCSTR cb_func)
{
	CAnimatorCamEffectorScriptCB* e		= xr_new<CAnimatorCamEffectorScriptCB>(cb_func);
	e->SetType					((ECamEffectorType)id);
	e->SetCyclic				(cyclic);
	e->Start					(fn);
	Actor()->Cameras().AddCamEffector(e);
	return						e->GetAnimatorLength();
}

float add_cam_effector2(LPCSTR fn, int id, bool cyclic, LPCSTR cb_func)
{
	CAnimatorCamEffectorScriptCB* e		= xr_new<CAnimatorCamEffectorScriptCB>(cb_func);
	e->m_bAbsolutePositioning	= true;
	e->SetType					((ECamEffectorType)id);
	e->SetCyclic				(cyclic);
	e->Start					(fn);
	Actor()->Cameras().AddCamEffector(e);
	return						e->GetAnimatorLength();
}

void remove_cam_effector(int id)
{
	Actor()->Cameras().RemoveCamEffector((ECamEffectorType)id );
}
		
float get_snd_volume()
{
	return psSoundVFactor;
}

void set_snd_volume(float v)
{
	psSoundVFactor = v;
	clamp(psSoundVFactor,0.0f,1.0f);
}
#include "actor_statistic_mgr.h"
void add_actor_points(LPCSTR sect, LPCSTR detail_key, int cnt, int pts)
{
	return Actor()->StatisticMgr().AddPoints(sect, detail_key, cnt, pts);
}

void add_actor_points_str(LPCSTR sect, LPCSTR detail_key, LPCSTR str_value)
{
	return Actor()->StatisticMgr().AddPoints(sect, detail_key, str_value);
}

int get_actor_points(LPCSTR sect)
{
	return Actor()->StatisticMgr().GetSectionPoints(sect);
}
extern int get_actor_ranking();
extern void add_human_to_top_list		(u16 id);
extern void remove_human_from_top_list	(u16 id);



#include "ActorEffector.h"
void add_complex_effector(LPCSTR section, int id)
{
	AddEffector(Actor(),id, section);
}

void remove_complex_effector(int id)
{
	RemoveEffector(Actor(),id);
}

#include "postprocessanimator.h"
void add_pp_effector(LPCSTR fn, int id, bool cyclic)
{
	CPostprocessAnimator* pp		= xr_new<CPostprocessAnimator>(id, cyclic);
	pp->Load						(fn);
	Actor()->Cameras().AddPPEffector	(pp);
}

void remove_pp_effector(int id)
{
	CPostprocessAnimator*	pp	= smart_cast<CPostprocessAnimator*>(Actor()->Cameras().GetPPEffector((EEffectorPPType)id));

	if(pp) pp->Stop(1.0f);

}

void set_pp_effector_factor(int id, float f, float f_sp)
{
	CPostprocessAnimator*	pp	= smart_cast<CPostprocessAnimator*>(Actor()->Cameras().GetPPEffector((EEffectorPPType)id));

	if(pp) pp->SetDesiredFactor(f,f_sp);
}

void set_pp_effector_factor2(int id, float f)
{
	CPostprocessAnimator*	pp	= smart_cast<CPostprocessAnimator*>(Actor()->Cameras().GetPPEffector((EEffectorPPType)id));

	if(pp) pp->SetCurrentFactor(f);
}

#include "relation_registry.h"

 int g_community_goodwill(LPCSTR _community, int _entity_id)
 {
	 CHARACTER_COMMUNITY c;
	 c.set					(_community);

 	return RELATION_REGISTRY().GetCommunityGoodwill(c.index(), u16(_entity_id));
 }

void g_set_community_goodwill(LPCSTR _community, int _entity_id, int val)
{
	 CHARACTER_COMMUNITY	c;
	 c.set					(_community);
	RELATION_REGISTRY().SetCommunityGoodwill(c.index(), u16(_entity_id), val);
}

void g_change_community_goodwill(LPCSTR _community, int _entity_id, int val)
{
	 CHARACTER_COMMUNITY	c;
	 c.set					(_community);
	RELATION_REGISTRY().ChangeCommunityGoodwill(c.index(), u16(_entity_id), val);
}
int g_get_personal_goodwill(int _who_id, int _to_whom_id)
{
	return RELATION_REGISTRY().GetGoodwill(u16(_who_id), u16(_to_whom_id));
}
void g_set_personal_goodwill(int _who_id, int _to_whom_id, int _amount)
{
	RELATION_REGISTRY().SetGoodwill(u16(_who_id), u16(_to_whom_id), _amount);
}
void g_change_personal_goodwill(int _who_id, int _to_whom_id, int _amount)
{
	CHARACTER_GOODWILL gw = RELATION_REGISTRY().GetGoodwill(u16(_who_id), u16(_to_whom_id));
	RELATION_REGISTRY().SetGoodwill(u16(_who_id), u16(_to_whom_id), gw + _amount);
}
void g_clear_personal_goodwill(int _who_id, int _to_whom_id)
{
	RELATION_REGISTRY().ClearGoodwill(u16(_who_id), u16(_to_whom_id));
}
void g_clear_personal_relations( int _who_id )
{
	RELATION_REGISTRY().ClearRelations( u16( _who_id ) );
}
void set_ignore_game_state_update()
{
	Game().m_need_to_update = false;
}

void SetEnvDescData(LPCSTR section_1, LPCSTR section_2, float exec_time_1, float exec_time_2)
{
	// в общем, какой тут смысл. Скрипт берет на себя все, кроме установки текущей погоды.
	// устанавливается погода двумя дескрипторами, соответственно, чтоб не тащить за собой кучу всего, 
	// единомоментно загружен минимум погодных дескрипторов. Остальные подгружаются по необходимости.
	CEnvironment &env = g_pGamePersistent->Environment(); 
														  
	env.SetWeather("ogse_script_weather", true);

	CEnvDescriptor *env_desc0 = (*env.CurrentWeather)[0];
	env_desc0->load(exec_time_1, section_1, &env);
	CEnvDescriptor *env_desc1 = (*env.CurrentWeather)[1];
	env_desc1->load(exec_time_2, section_2, &env);

	env.ForceReselectEnvs();
}

void g_set_artefact_position(const u32 i, const float x, const float y, const float z)
{
	Fvector pos;
	pos.set(x, y, z);

	if (fis_zero(pos.x) && fis_zero(pos.y) && fis_zero(pos.z))
	{
		Fvector2 res;
		shader_exports->set_artefact_position(i, res.set(0.f, 0.f));
	}
	else
	{
		Device.mView.transform_tiny(pos);
		Fvector2 res;
		shader_exports->set_artefact_position(i, res.set(pos.x, pos.z));
	}

}
void g_set_anomaly_position(const u32 i, const float x, const float y, const float z)
{
	Fvector pos;
	pos.set(x, y, z);

	if (fis_zero(pos.x) && fis_zero(pos.y) && fis_zero(pos.z))
	{
		Fvector2 res;
		shader_exports->set_anomaly_position(i, res.set(0.f, 0.f));
	}
	else
	{
		Device.mView.transform_tiny(pos);
		Fvector2 res;
		shader_exports->set_anomaly_position(i, res.set(pos.x, pos.z));
	}
}
void g_set_detector_params(int _one, int _two)
{
	shader_exports->set_detector_params(Ivector2().set(_one, _two));
}
void g_set_input_language(bool _is_alternate)
{
	g_alternate_lang = _is_alternate;
}
bool g_get_input_language()
{
	return g_alternate_lang;
}
#include "game_sv_single.h"
void AdvanceGameTime(u32 _ms)
{
	game_sv_Single			*game = smart_cast<game_sv_Single*>(Level().Server->game);
	R_ASSERT(game);
	game->alife().time_manager().advance_game_time(_ms);

	Level().game->SetGameTimeFactor(ai().get_alife() ? ai().alife().time().game_time() : Level().GetGameTime(), Level().game->GetGameTimeFactor());
}

//
void send_event_key_press(int dik) //Нажатие клавиши
{
	Level().IR_OnKeyboardPress(dik);
}
void send_event_key_release(int dik) //Отпускание клавиши
{
	Level().IR_OnKeyboardRelease(dik);
}
void send_event_key_hold(int dik) //Удержание клавиши.
{
	Level().IR_OnKeyboardHold(dik);
}
void send_event_mouse_wheel(int vol) //Вращение колеса мыши
{
	Level().IR_OnMouseWheel(vol);
}
//

// Real Wolf 07.07.2014
u32 vertex_id(const Fvector &vec)
{
	return ai().level_graph().vertex_id(vec);
}

u32 vertex_id( u32 node, const Fvector &vec ) {
 return ai().level_graph().vertex( node, vec );
}

void update_inventory_window() {
  HUD().GetUI()->UIGame()->ReInitShownUI();
}

void update_inventory_weight() {
  CUIGameSP* pGameSP = smart_cast<CUIGameSP*>( HUD().GetUI()->UIGame() );
  if ( pGameSP)
    pGameSP->InventoryMenu->UpdateWeight();
}


void change_level( GameGraph::_GRAPH_ID game_vertex_id, u32 level_vertex_id, Fvector pos, Fvector dir ) {
  NET_Packet p;
  p.w_begin( M_CHANGE_LEVEL );
  p.w( &game_vertex_id, sizeof( game_vertex_id ) );
  p.w( &level_vertex_id, sizeof( level_vertex_id ) );
  p.w_vec3( pos );
  p.w_vec3( dir );
  Level().Send( p, net_flags( TRUE ) );
}


void setEFXPreset( LPCSTR preset ) {
  ::Sound->setEFXPreset( preset );
}

void unsetEFXPreset() {
  ::Sound->unsetEFXPreset();
}


void setEFXEAXPreset( LPCSTR preset ) {
  ::Sound->setEFXEAXPreset( preset );
}

void unsetEFXEAXPreset() {
  ::Sound->unsetEFXEAXPreset();
}


void set_cam_inert( float v ) {
  psCamInert = v;
  clamp( psCamInert, 0.0f, 1.0f );
}


void set_monster_relation( LPCSTR from, LPCSTR to, int rel ) {
  MONSTER_COMMUNITY::set_relation( (MONSTER_COMMUNITY_ID)from, (MONSTER_COMMUNITY_ID)to, rel );
}


void patrol_path_add( LPCSTR patrol_path, CPatrolPath* path ) {
  ai().patrol_paths_raw().add_path( shared_str( patrol_path ), path );
}


void patrol_path_remove( LPCSTR patrol_path ) {
  ai().patrol_paths_raw().remove_path( shared_str( patrol_path ) );
}


//
float set_blender_mode_main(float blender_num = 0.f) //--#SM+#--
{
	g_pGamePersistent->m_pGShaderConstants.m_blender_mode.x = blender_num;
	return g_pGamePersistent->m_pGShaderConstants.m_blender_mode.x;
}

float get_blender_mode_main() //--#SM+#--
{
	return g_pGamePersistent->m_pGShaderConstants.m_blender_mode.x;
}

float set_blender_mode_second(float blender_num = 0.f) //--#SM+#--
{
	g_pGamePersistent->m_pGShaderConstants.m_blender_mode.y = blender_num;
	return g_pGamePersistent->m_pGShaderConstants.m_blender_mode.y;
}

float get_blender_mode_second() //--#SM+#--
{
	return g_pGamePersistent->m_pGShaderConstants.m_blender_mode.y;
}

Fmatrix get_shader_params() //--#SM+#--
{
	return g_pGamePersistent->m_pGShaderConstants.m_script_params;
}

void set_shader_params(const Fmatrix& m_params) //--#SM+#--
{
	g_pGamePersistent->m_pGShaderConstants.m_script_params = m_params;
}
//


bool valid_vertex_id( u32 level_vertex_id ) {
  return ai().level_graph().valid_vertex_id( level_vertex_id );
}


u32 vertex_count() {
  return ai().level_graph().header().vertex_count();
}


#pragma optimize("s",on)
void CLevel::script_register(lua_State *L)
{
	module(L)
	[
	class_<CEnvDescriptor>( "CEnvDescriptor" )
          .def_readwrite( "fog_density", &CEnvDescriptor::fog_density)
          .def_readwrite( "far_plane",   &CEnvDescriptor::far_plane)
          .def_readwrite( "sun_dir",     &CEnvDescriptor::sun_dir )
          .def( "load",	           ( void( CEnvDescriptor::* ) ( float, LPCSTR, CEnvironment* ) ) &CEnvDescriptor::load )
          .def( "set_env_ambient", &CEnvDescriptor::setEnvAmbient ),

	class_<CEnvironment>( "CEnvironment" )
          .def( "current",           current_environment )
          .def( "ForceReselectEnvs", &CEnvironment::ForceReselectEnvs )
          .def( "getCurrentWeather", &CEnvironment::getCurrentWeather ),

	class_<CPHCall>( "CPHCall" )
          .def( "set_pause", &CPHCall::setPause )
	],

	module(L,"level")
	[
		// obsolete\deprecated
		def("object_by_id",						get_object_by_id),
#ifdef DEBUG
		def("debug_object",						get_object_by_name),
		def("debug_actor",						tpfGetActor),
		def("check_object",						check_object),
#endif
		
		def("get_weather",						get_weather),
		def("set_weather",						set_weather),
		def("set_weather_fx",					set_weather_fx),
		def("is_wfx_playing",					is_wfx_playing),

		def("environment",						environment),
		
		def("set_time_factor",					set_time_factor),
		def("get_time_factor",					get_time_factor),

		def("set_game_difficulty",				set_game_difficulty),
		def("get_game_difficulty",				get_game_difficulty),
		
		def("get_time_days",					get_time_days),
		def("get_time_hours",					get_time_hours),
		def("get_time_minutes",					get_time_minutes),

		def("cover_in_direction",				cover_in_direction),
		def("vertex_in_direction",				vertex_in_direction),
		def("rain_factor",						rain_factor),
		def("patrol_path_exists",				patrol_path_exists),
		def("vertex_position",					vertex_position),
		def("name",								get_name),
		def("prefetch_sound",					prefetch_sound),

		def("client_spawn_manager",				get_client_spawn_manager),

		def("map_add_object_spot_ser",			map_add_object_spot_ser),
		def("map_add_object_spot",				map_add_object_spot),
		def("map_remove_object_spot",			map_remove_object_spot),
		def("map_has_object_spot",				map_has_object_spot),
		def("map_change_spot_hint",				map_change_spot_hint),

		def("start_stop_menu",					start_stop_menu),
		def("add_dialog_to_render",				add_dialog_to_render),
		def("remove_dialog_to_render",			remove_dialog_to_render),
		def("main_input_receiver",				main_input_receiver),
		def("hide_indicators",					hide_indicators),
		def("show_indicators",					show_indicators),
		def("add_call",							((CPHCall* (*) (const luabind::functor<bool> &,const luabind::functor<void> &)) &add_call)),
		def("add_call",							((CPHCall* (*) (const luabind::object &,const luabind::functor<bool> &,const luabind::functor<void> &)) &add_call)),
		def("add_call",							((CPHCall* (*) (const luabind::object &, LPCSTR, LPCSTR)) &add_call)),
		def("remove_call",						((void (*) (const luabind::functor<bool> &,const luabind::functor<void> &)) &remove_call)),
		def("remove_call",						((void (*) (const luabind::object &,const luabind::functor<bool> &,const luabind::functor<void> &)) &remove_call)),
		def("remove_call",						((void (*) (const luabind::object &, LPCSTR, LPCSTR)) &remove_call)),
		def("remove_calls_for_object",			remove_calls_for_object),
		def("present",							is_level_present),
		def("disable_input",					disable_input),
		def("enable_input",						enable_input),
		def("spawn_phantom",					spawn_phantom),

		def("get_bounding_volume",				get_bounding_volume),

		def("iterate_sounds",					&iterate_sounds1),
		def("iterate_sounds",					&iterate_sounds2),
		def("physics_world",					&physics_world),
		def("get_snd_volume",					&get_snd_volume),
		def("set_snd_volume",					&set_snd_volume),
		def("add_cam_effector",					&add_cam_effector),
		def("add_cam_effector2",				&add_cam_effector2),
		def("remove_cam_effector",				&remove_cam_effector),
		def("add_pp_effector",					&add_pp_effector),
		def("set_pp_effector_factor",			&set_pp_effector_factor),
		def("set_pp_effector_factor",			&set_pp_effector_factor2),
		def("remove_pp_effector",				&remove_pp_effector),

		def("add_complex_effector",				&add_complex_effector),
		def("remove_complex_effector",			&remove_complex_effector),
		
		def("game_id",							&GameID),
		def("set_ignore_game_state_update",		&set_ignore_game_state_update),

		def("get_inventory_wnd",				&GetInventoryWindow),
		def("get_talk_wnd",						&GetTalkWindow),
		def("get_trade_wnd",					&GetTradeWindow),
		def("get_second_talker",				&GetSecondTalker),
		def("get_change_level_wnd",				&GetUIChangeLevelWnd),

		def("ray_query",						&PerformRayQuery),

		// Real Wolf 07.07.2014
		def( "vertex_id", ( ( u32(*) ( const Fvector& ) ) &vertex_id ) ),
		def( "vertex_id", ( ( u32(*) ( u32, const Fvector& ) ) &vertex_id ) ),

		def("advance_game_time",				&AdvanceGameTime),
		
		def("get_target_dist",					&GetTargetDist),
		def("get_target_obj",					&GetTargetObj),
		//
		def("send_event_key_press", &send_event_key_press),
		def("send_event_key_release", &send_event_key_release),
		def("send_event_key_hold", &send_event_key_hold),
		def("send_event_mouse_wheel", &send_event_mouse_wheel),

		def( "change_level", &change_level ),
		def( "set_efx_preset", &setEFXPreset ),
		def( "unset_efx_preset", &unsetEFXPreset ),
		def( "set_efx_eax_preset", &setEFXEAXPreset ),
		def( "unset_efx_eax_preset", &unsetEFXEAXPreset ),
		def( "set_cam_inert", &set_cam_inert ),
		def( "set_monster_relation", &set_monster_relation ),
		def( "patrol_path_add", &patrol_path_add ),
		def( "patrol_path_remove", &patrol_path_remove ),
		def( "valid_vertex_id",	valid_vertex_id ),
		def( "vertex_count",	vertex_count ),
		//--#SM+# Begin --
		def("set_blender_mode_main", &set_blender_mode_main),
		def("get_blender_mode_main", &get_blender_mode_main),
		def("set_blender_mode_second", &set_blender_mode_second),
		def("get_blender_mode_second", &get_blender_mode_second),
		def("set_shader_params", &set_shader_params),
		def("get_shader_params", &get_shader_params)
		//--#SM+# End --
	],
	
	module(L,"actor_stats")
	[
		def("add_points",						&add_actor_points),
		def("add_points_str",					&add_actor_points_str),
		def("get_points",						&get_actor_points),
		def("add_to_ranking",					&add_human_to_top_list),
		def("remove_from_ranking",				&remove_human_from_top_list),
		def("get_actor_ranking",				&get_actor_ranking)
	];

	module(L)
	[
		def("command_line",						&command_line)
	];

	module(L,"relation_registry")
	[
		def("community_goodwill",				&g_community_goodwill),
		def("set_community_goodwill",			&g_set_community_goodwill),
		def("change_community_goodwill",		&g_change_community_goodwill),
		def("get_personal_goodwill",			&g_get_personal_goodwill),
		def("set_personal_goodwill",			&g_set_personal_goodwill),
		def("change_personal_goodwill",			&g_change_personal_goodwill),
		def("clear_personal_goodwill",			&g_clear_personal_goodwill),
		def("clear_personal_relations",			&g_clear_personal_relations)
	];
	//установка параметров для шейдеров из скриптов
	module(L)
		[
			def("set_artefact_slot", &g_set_artefact_position),
			def("set_anomaly_slot", &g_set_anomaly_position),
			def("set_detector_mode", &g_set_detector_params),
			def("SetEnvDescData", &SetEnvDescData),
			def("set_input_language", &g_set_input_language),
			def("get_input_language", &g_get_input_language),
			def("update_inventory_window", &update_inventory_window),
			def("update_inventory_weight", &update_inventory_weight),

			class_<enum_exporter<collide::rq_target> >("rq_target")
			.enum_("rq_target")
			[
				value("rqtNone", int(collide::rqtNone)),
				value("rqtObject", int(collide::rqtObject)),
				value("rqtStatic", int(collide::rqtStatic)),
				value("rqtShape", int(collide::rqtShape)),
				value("rqtObstacle", int(collide::rqtObstacle)),
				value("rqtBoth", int(collide::rqtBoth)),
				value("rqtDyn", int(collide::rqtDyn))
			]
		];
}

////////////////////////////////////////////////////////////////////////////
//	Module 		: level_script.cpp
//	Created 	: 28.06.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Level script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Level.h"
#include "Actor.h"
#include "script_game_object.h"
#include "patrol_path_storage.h"
#include "xrServer.h"
#include "client_spawn_manager.h"
#include "../xr_3da/IGame_Persistent.h"
#include "game_cl_base.h"
#include "UIGameCustom.h"
#include "ui/UIDialogWnd.h"
#include "date_time.h"
#include "ai_space.h"
#include "level_graph.h"
#include "PHCommander.h"
#include "PHScriptCall.h"
#include "script_engine.h"
#include "game_cl_single.h"
#include "game_sv_single.h"
#include "map_manager.h"
#include "map_spot.h"
#include "map_location.h"
#include "physics_world_scripted.h"
#include "alife_simulator.h"
#include "alife_time_manager.h"
#include "ui/UIGameTutorial.h"
#include "string_table.h"
#include "ui/UIInventoryUtilities.h"
#include "alife_object_registry.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "ui/UIActorMenu.h"
#include "Inventory.h"
#include "Weapon.h"
#include "HUDManager.h"
#include "GamePersistent.h"
#include "level_sounds.h"
#include "player_hud.h"

using namespace luabind;

LPCSTR command_line() { return (Core.Params); }
bool IsDynamicMusic() { return !!psActorFlags.test(AF_DYNAMIC_MUSIC); }

bool IsImportantSave() { return !!psActorFlags.test(AF_IMPORTANT_SAVE); }

#ifdef DEBUG
void check_object(CScriptGameObject* object)
{
    try
    {
        Msg("check_object %s", object->Name());
    }
    catch (...)
    {
        object = object;
    }
}

CScriptGameObject* tpfGetActor()
{
    static bool first_time = true;
    if (first_time)
        ai().script_engine().script_log(eLuaMessageTypeError, "Do not use level.actor function!");
    first_time = false;

    CActor* l_tpActor = smart_cast<CActor*>(Level().CurrentEntity());
    if (l_tpActor)
        return (smart_cast<CGameObject*>(l_tpActor)->lua_game_object());
    else
        return (0);
}

CScriptGameObject* get_object_by_name(LPCSTR caObjectName)
{
    static bool first_time = true;
    if (first_time)
        ai().script_engine().script_log(eLuaMessageTypeError, "Do not use level.object function!");
    first_time = false;

    CGameObject* l_tpGameObject = smart_cast<CGameObject*>(Level().Objects.FindObjectByName(caObjectName));
    if (l_tpGameObject)
        return (l_tpGameObject->lua_game_object());
    else
        return (0);
}
#endif

CScriptGameObject* get_object_by_id(u16 id)
{
    CGameObject* pGameObject = smart_cast<CGameObject*>(Level().Objects.net_Find(id));
    if (!pGameObject)
        return nullptr;

    return pGameObject->lua_game_object();
}

LPCSTR get_weather() { return (*g_pGamePersistent->Environment().GetWeather()); }

LPCSTR get_weather_prev() { return (*g_pGamePersistent->Environment().GetPrevWeather()); }

u32 get_weather_last_shift() { return g_pGamePersistent->Environment().GetWeatherLastShift(); }

extern bool editor_override_weather;

void set_weather(LPCSTR weather_name, bool forced)
{
    if (editor_override_weather)
        return;

    // KRodin: ТЧ погоду всегда надо обновлять форсировано, иначе она почему-то не всегда корректно обновляется. А для ЗП погоды так делать нельзя - будут очень резкие переходы!
    if (!g_pGamePersistent->Environment().USED_COP_WEATHER)
        forced = true;

    g_pGamePersistent->Environment().SetWeather(weather_name, forced);
}

void set_weather_next(LPCSTR weather_name)
{
    if (editor_override_weather)
        return;

    g_pGamePersistent->Environment().SetWeatherNext(weather_name);
}

bool set_weather_fx(LPCSTR weather_name)
{
    if (editor_override_weather)
        return false;

    return (g_pGamePersistent->Environment().SetWeatherFX(weather_name));
}

bool start_weather_fx_from_time(LPCSTR weather_name, float time)
{
    if (editor_override_weather)
        return false;

    return g_pGamePersistent->Environment().SetWeatherFXFromTime(weather_name, time);
}

bool is_wfx_playing() { return (g_pGamePersistent->Environment().IsWeatherFXPlaying()); }

float get_wfx_time() { return (g_pGamePersistent->Environment().wfx_time); }

void stop_weather_fx() { g_pGamePersistent->Environment().StopWeatherFX(); }

void set_time_factor(float time_factor)
{
    Level().Server->game->SetGameTimeFactor(time_factor);
    GamePersistent().Environment().SetGameTime(Level().GetEnvironmentGameDayTimeSec(), Level().game->GetEnvironmentGameTimeFactor());
}

float get_time_factor() { return (Level().GetGameTimeFactor()); }

void set_game_difficulty(ESingleGameDifficulty dif)
{
    R_ASSERT(dif < egdCount);
    g_SingleGameDifficulty = dif;
    game_cl_Single* game = smart_cast<game_cl_Single*>(Level().game);
    VERIFY(game);
    game->OnDifficultyChanged();
}
ESingleGameDifficulty get_game_difficulty() { return g_SingleGameDifficulty; }

u32 get_time_days()
{
    u32 year = 0, month = 0, day = 0, hours = 0, mins = 0, secs = 0, milisecs = 0;
    split_time((g_pGameLevel && Level().game) ? Level().GetGameTime() : ai().alife().time_manager().game_time(), year, month, day, hours, mins, secs, milisecs);
    return day;
}

u32 get_time_hours()
{
    u32 year = 0, month = 0, day = 0, hours = 0, mins = 0, secs = 0, milisecs = 0;
    split_time((g_pGameLevel && Level().game) ? Level().GetGameTime() : ai().alife().time_manager().game_time(), year, month, day, hours, mins, secs, milisecs);
    return hours;
}

u32 get_time_minutes()
{
    u32 year = 0, month = 0, day = 0, hours = 0, mins = 0, secs = 0, milisecs = 0;
    split_time((g_pGameLevel && Level().game) ? Level().GetGameTime() : ai().alife().time_manager().game_time(), year, month, day, hours, mins, secs, milisecs);
    return mins;
}

void change_game_time(u32 days, u32 hours, u32 mins)
{
    game_sv_Single* tpGame = smart_cast<game_sv_Single*>(Level().Server->game);
    if (tpGame && ai().get_alife())
    {
        u32 value = days * 86400 + hours * 3600 + mins * 60;
        float fValue = static_cast<float>(value);
        value *= 1000; // msec
        g_pGamePersistent->Environment().ChangeGameTime(fValue);
        tpGame->alife().time_manager().change_game_time(value);
    }
}

float high_cover_in_direction(u32 level_vertex_id, const Fvector& direction)
{
    float y, p;
    direction.getHP(y, p);
    return (ai().level_graph().high_cover_in_direction(y, level_vertex_id));
}

float low_cover_in_direction(u32 level_vertex_id, const Fvector& direction)
{
    float y, p;
    direction.getHP(y, p);
    return (ai().level_graph().low_cover_in_direction(y, level_vertex_id));
}

float rain_factor() { return (g_pGamePersistent->Environment().CurrentEnv->rain_density); }

float rain_hemi()
{
    CObject* E = g_pGameLevel->CurrentViewEntity();
    if (E && E->renderable_ROS())
    {
        const float* hemi_cube = E->renderable_ROS()->get_luminocity_hemi_cube();
        float hemi_val = _max(hemi_cube[0], hemi_cube[1]);
        hemi_val = _max(hemi_val, hemi_cube[2]);
        hemi_val = _max(hemi_val, hemi_cube[3]);
        hemi_val = _max(hemi_val, hemi_cube[5]);
        return hemi_val;
    }
    return 0.f;
}

u32 vertex_in_direction(u32 level_vertex_id, Fvector direction, float max_distance)
{
    direction.normalize_safe();
    direction.mul(max_distance);
    Fvector start_position = ai().level_graph().vertex_position(level_vertex_id);
    Fvector finish_position = Fvector(start_position).add(direction);
    u32 result = u32(-1);
    ai().level_graph().farthest_vertex_in_direction(level_vertex_id, start_position, finish_position, result, 0);
    return (ai().level_graph().valid_vertex_id(result) ? result : level_vertex_id);
}

Fvector vertex_position(u32 level_vertex_id) { return (ai().level_graph().vertex_position(level_vertex_id)); }

void map_add_object_spot(u16 id, LPCSTR spot_type, LPCSTR text)
{
    CMapLocation* ml = Level().MapManager().AddMapLocation(spot_type, id);
    if (xr_strlen(text))
    {
        ml->SetHint(text);
    }
}

void map_add_object_spot_ser(u16 id, LPCSTR spot_type, LPCSTR text)
{
    CMapLocation* ml = Level().MapManager().AddMapLocation(spot_type, id);
    if (xr_strlen(text))
        ml->SetHint(text);

    ml->SetSerializable(true);
}

void map_change_spot_hint(u16 id, LPCSTR spot_type, LPCSTR text)
{
    CMapLocation* ml = Level().MapManager().GetMapLocation(spot_type, id);
    if (!ml)
        return;
    ml->SetHint(text);
}

void map_remove_object_spot(u16 id, LPCSTR spot_type) { Level().MapManager().RemoveMapLocation(spot_type, id); }

u16 map_has_object_spot(u16 id, LPCSTR spot_type) { return Level().MapManager().HasMapLocation(spot_type, id); }

bool patrol_path_exists(LPCSTR patrol_path) { return (!!ai().patrol_paths().path(patrol_path, true)); }

LPCSTR get_name() { return (*Level().name()); }

void prefetch_sound(LPCSTR name) { Level().PrefetchSound(name); }

CClientSpawnManager& get_client_spawn_manager() { return (Level().client_spawn_manager()); }

static void start_stop_menu(CUIDialogWnd* pDialog, bool bDoHideIndicators)
{
    if (pDialog->IsShown())
        pDialog->HideDialog();
    else
        pDialog->ShowDialog(bDoHideIndicators);
}

void add_dialog_to_render(CUIDialogWnd* pDialog) { CurrentGameUI()->AddDialogToRender(pDialog); }

void remove_dialog_to_render(CUIDialogWnd* pDialog) { CurrentGameUI()->RemoveDialogToRender(pDialog); }

static CUIDialogWnd* main_input_receiver() { return HUD().GetGameUI()->TopInputReceiver(); }

void hide_indicators()
{
    if (CurrentGameUI())
    {
        CurrentGameUI()->HideShownDialogs();
        CurrentGameUI()->ShowGameIndicators(false);
        CurrentGameUI()->ShowCrosshair(false);
    }
    psActorFlags.set(AF_GODMODE_RT, TRUE);
}

void hide_indicators_safe()
{
    if (CurrentGameUI())
    {
        CurrentGameUI()->ShowGameIndicators(false);
        CurrentGameUI()->ShowCrosshair(false);

        CurrentGameUI()->OnExternalHideIndicators();
    }
    psActorFlags.set(AF_GODMODE_RT, TRUE);
}

void show_indicators()
{
    if (CurrentGameUI())
    {
        CurrentGameUI()->ShowGameIndicators(true);
        CurrentGameUI()->ShowCrosshair(true);
    }
    psActorFlags.set(AF_GODMODE_RT, FALSE);
}

static void show_weapon(const bool b)
{
    // Simp: пусть так будет что ли
    HUD().SetRenderable(b);
}

bool is_level_present() { return (!!g_pGameLevel); }

CPHCommanderScripts::CPHCallScript* add_call(const luabind::functor<bool>& condition, const luabind::functor<void>& action)
{
    luabind::functor<bool> _condition = condition;
    luabind::functor<void> _action = action;
    CPHScriptCondition* c = xr_new<CPHScriptCondition>(_condition);
    CPHScriptAction* a = xr_new<CPHScriptAction>(_action);
    return Level().ph_commander_scripts().add_call(c, a);
}

void remove_call(const luabind::functor<bool>& condition, const luabind::functor<void>& action)
{
    CPHScriptCondition c(condition);
    CPHScriptAction a(action);
    Level().ph_commander_scripts().remove_call(&c, &a);
}

CPHCommanderScripts::CPHCallScript* add_call(const luabind::object& lua_object, LPCSTR condition, LPCSTR action)
{
    luabind::functor<bool> _condition = object_cast<luabind::functor<bool>>(lua_object[condition]);
    luabind::functor<void> _action = object_cast<luabind::functor<void>>(lua_object[action]);
    CPHScriptObjectConditionN* c = xr_new<CPHScriptObjectConditionN>(lua_object, _condition);
    CPHScriptObjectActionN* a = xr_new<CPHScriptObjectActionN>(lua_object, _action);
    return Level().ph_commander_scripts().add_call_unique(c, c, a, a);
}

void remove_call(const luabind::object& lua_object, LPCSTR condition, LPCSTR action)
{
    CPHScriptObjectCondition c(lua_object, condition);
    CPHScriptObjectAction a(lua_object, action);
    Level().ph_commander_scripts().remove_call(&c, &a);
}

CPHCommanderScripts::CPHCallScript* add_call(const luabind::object& lua_object, const luabind::functor<bool>& condition, const luabind::functor<void>& action)
{
    CPHScriptObjectConditionN* c = xr_new<CPHScriptObjectConditionN>(lua_object, condition);
    CPHScriptObjectActionN* a = xr_new<CPHScriptObjectActionN>(lua_object, action);
    return Level().ph_commander_scripts().add_call(c, a);
}

void remove_call(const luabind::object& lua_object, const luabind::functor<bool>& condition, const luabind::functor<void>& action)
{
    CPHScriptObjectConditionN c(lua_object, condition);
    CPHScriptObjectActionN a(lua_object, action);
    Level().ph_commander_scripts().remove_call(&c, &a);
}

void remove_calls_for_object(const luabind::object& lua_object)
{
    CPHSriptReqObjComparer c(lua_object);
    Level().ph_commander_scripts().remove_calls(&c);
}

cphysics_world_scripted* physics_world_scripted() { return get_script_wrapper<cphysics_world_scripted>(*physics_world()); }

extern bool g_bDisableAllInput;
void disable_input()
{
    g_bDisableAllInput = true;
    if (Actor())
        Actor()->PickupModeOff();
#ifdef DEBUG
    Msg("input disabled");
#endif // #ifdef DEBUG
}
void enable_input()
{
    g_bDisableAllInput = false;
#ifdef DEBUG
    Msg("input enabled");
#endif // #ifdef DEBUG
}

void spawn_phantom(const Fvector& position) { Level().spawn_item("m_phantom", position, u32(-1), u16(-1), false); }

Fbox get_bounding_volume() { return Level().ObjectSpace.GetBoundingVolume(); }

void iterate_sounds(LPCSTR prefix, u32 max_count, const CScriptCallbackEx<void>& callback)
{
    for (int j = 0, N = _GetItemCount(prefix); j < N; ++j)
    {
        string_path fn, s;
        LPSTR S = (LPSTR)&s;
        _GetItem(prefix, j, s);
        if (FS.exist(fn, "$game_sounds$", S, ".ogg"))
            callback(prefix);

        for (u32 i = 0; i < max_count; ++i)
        {
            string_path name;
            xr_sprintf(name, "%s%d", S, i);
            if (FS.exist(fn, "$game_sounds$", name, ".ogg"))
                callback(name);
        }
    }
}

void iterate_sounds1(LPCSTR prefix, u32 max_count, luabind::functor<void> functor)
{
    CScriptCallbackEx<void> temp;
    temp.set(functor);
    iterate_sounds(prefix, max_count, temp);
}

void iterate_sounds2(LPCSTR prefix, u32 max_count, luabind::object object, luabind::functor<void> functor)
{
    CScriptCallbackEx<void> temp;
    temp.set(functor, object);
    iterate_sounds(prefix, max_count, temp);
}

#include "ActorEffector.h"
float add_cam_effector(LPCSTR fn, int id, bool cyclic, LPCSTR cb_func)
{
    CAnimatorCamEffectorScriptCB* e = xr_new<CAnimatorCamEffectorScriptCB>(cb_func);
    e->SetType((ECamEffectorType)id);
    e->SetCyclic(cyclic);
    e->Start(fn);
    Actor()->Cameras().AddCamEffector(e);
    return e->GetAnimatorLength();
}

float add_cam_effector2(LPCSTR fn, int id, bool cyclic, LPCSTR cb_func, float cam_fov)
{
    CAnimatorCamEffectorScriptCB* e = xr_new<CAnimatorCamEffectorScriptCB>(cb_func);
    e->m_bAbsolutePositioning = true;
    e->m_fov = cam_fov;
    e->SetType((ECamEffectorType)id);
    e->SetCyclic(cyclic);
    e->Start(fn);
    Actor()->Cameras().AddCamEffector(e);
    return e->GetAnimatorLength();
}

void remove_cam_effector(int id) { Actor()->Cameras().RemoveCamEffector((ECamEffectorType)id); }

float get_snd_volume() { return psSoundVFactor; }

void set_snd_volume(float v)
{
    psSoundVFactor = v;
    clamp(psSoundVFactor, 0.0f, 1.0f);
}
#include "actor_statistic_mgr.h"
void add_actor_points(LPCSTR sect, LPCSTR detail_key, int cnt, int pts) { return Actor()->StatisticMgr().AddPoints(sect, detail_key, cnt, pts); }

void add_actor_points_str(LPCSTR sect, LPCSTR detail_key, LPCSTR str_value) { return Actor()->StatisticMgr().AddPoints(sect, detail_key, str_value); }

int get_actor_points(LPCSTR sect) { return Actor()->StatisticMgr().GetSectionPoints(sect); }

#include "ActorEffector.h"
void add_complex_effector(LPCSTR section, int id) { AddEffector(Actor(), id, section); }

void remove_complex_effector(int id) { RemoveEffector(Actor(), id); }

#include "PostprocessAnimator.h"
void add_pp_effector(LPCSTR fn, int id, bool cyclic)
{
    CPostprocessAnimator* pp = xr_new<CPostprocessAnimator>(id, cyclic);
    pp->Load(fn);
    Actor()->Cameras().AddPPEffector(pp);
}

void remove_pp_effector(int id)
{
    CPostprocessAnimator* pp = smart_cast<CPostprocessAnimator*>(Actor()->Cameras().GetPPEffector((EEffectorPPType)id));

    if (pp)
        pp->Stop(1.0f);
}

void set_pp_effector_factor(int id, float f, float f_sp)
{
    CPostprocessAnimator* pp = smart_cast<CPostprocessAnimator*>(Actor()->Cameras().GetPPEffector((EEffectorPPType)id));

    if (pp)
        pp->SetDesiredFactor(f, f_sp);
}

void set_pp_effector_factor2(int id, float f)
{
    CPostprocessAnimator* pp = smart_cast<CPostprocessAnimator*>(Actor()->Cameras().GetPPEffector((EEffectorPPType)id));

    if (pp)
        pp->SetCurrentFactor(f);
}

#include "relation_registry.h"

int g_community_goodwill(LPCSTR _community, int _entity_id)
{
    CHARACTER_COMMUNITY c;
    c.set(_community);

    return RELATION_REGISTRY().GetCommunityGoodwill(c.index(), u16(_entity_id));
}

void g_set_community_goodwill(LPCSTR _community, int _entity_id, int val)
{
    CHARACTER_COMMUNITY c;
    c.set(_community);
    RELATION_REGISTRY().SetCommunityGoodwill(c.index(), u16(_entity_id), val);
}

void g_change_community_goodwill(LPCSTR _community, int _entity_id, int val)
{
    CHARACTER_COMMUNITY c;
    c.set(_community);
    RELATION_REGISTRY().ChangeCommunityGoodwill(c.index(), u16(_entity_id), val);
}

int g_get_community_relation(LPCSTR comm_from, LPCSTR comm_to)
{
    CHARACTER_COMMUNITY community_from;
    community_from.set(comm_from);
    CHARACTER_COMMUNITY community_to;
    community_to.set(comm_to);

    return RELATION_REGISTRY().GetCommunityRelation(community_from.index(), community_to.index());
}

void g_set_community_relation(LPCSTR comm_from, LPCSTR comm_to, int value)
{
    CHARACTER_COMMUNITY community_from;
    community_from.set(comm_from);
    CHARACTER_COMMUNITY community_to;
    community_to.set(comm_to);

    RELATION_REGISTRY().SetCommunityRelation(community_from.index(), community_to.index(), value);
}

int g_get_general_goodwill_between(u16 from, u16 to)
{
    CHARACTER_GOODWILL presonal_goodwill = RELATION_REGISTRY().GetGoodwill(from, to);
    VERIFY(presonal_goodwill != NO_GOODWILL);

    CSE_ALifeTraderAbstract* from_obj = smart_cast<CSE_ALifeTraderAbstract*>(ai().alife().objects().object(from));
    CSE_ALifeTraderAbstract* to_obj = smart_cast<CSE_ALifeTraderAbstract*>(ai().alife().objects().object(to));

    if (!from_obj || !to_obj)
    {
        ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError, "RELATION_REGISTRY::get_general_goodwill_between  : cannot convert obj to CSE_ALifeTraderAbstract!");
        return (0);
    }
    CHARACTER_GOODWILL community_to_obj_goodwill = RELATION_REGISTRY().GetCommunityGoodwill(from_obj->Community(), to);
    CHARACTER_GOODWILL community_to_community_goodwill = RELATION_REGISTRY().GetCommunityRelation(from_obj->Community(), to_obj->Community());

    return presonal_goodwill + community_to_obj_goodwill + community_to_community_goodwill;
}

u32 vertex_id(Fvector position) { return (ai().level_graph().vertex_id(position)); }

CUISequencer* g_tutorial = nullptr;
CUISequencer* g_tutorial2 = nullptr;

void start_tutorial(LPCSTR name)
{
    if (load_screen_renderer.IsActive())
        return;

    if (g_tutorial)
    {
        VERIFY(!g_tutorial2);
        g_tutorial2 = g_tutorial;
    };

    g_tutorial = xr_new<CUISequencer>();
    g_tutorial->Start(name);
    if (g_tutorial2)
        g_tutorial->m_pStoredInputReceiver = g_tutorial2->m_pStoredInputReceiver;
}

void stop_tutorial()
{
    if (g_tutorial)
        g_tutorial->Stop();
}

LPCSTR translate_string(LPCSTR str) { return *CStringTable().translate(str); }

bool has_active_tutotial() { return (g_tutorial != nullptr); }

CScriptGameObject* g_get_target_obj()
{
    collide::rq_result& RQ = HUD().GetCurrentRayQuery();
    if (RQ.O)
    {
        CGameObject* game_object = static_cast<CGameObject*>(RQ.O);
        if (game_object)
            return game_object->lua_game_object();
    }
    return nullptr;
}

float g_get_target_dist()
{
    collide::rq_result& RQ = HUD().GetCurrentRayQuery();
    if (RQ.O)
        return RQ.range;

    return 0.f;
}

void block_player_action(EGameActions dik) { Actor()->block_action(dik); }

void unblock_player_action(EGameActions dik) { Actor()->unblock_action(dik); }

float is_ray_intersect_sphere(Fvector pos, Fvector dir, Fvector C, float R)
{
    Fsphere sphere;
    sphere.P = C;
    sphere.R = R;
    dir.normalize_safe();
    float dist;
    return sphere.intersect_ray(pos, dir, dist) == Fsphere::rpNone ? -1.0f : dist;
}

static void shader_set_custom_param_vector(const char* key, const Fvector4 v) { shader_exports.set_custom_params(key, v); }

static Fvector4 shader_get_custom_param_vector(const char* key) { return shader_exports.get_custom_params(key); }

static void shader_set_custom_param(const char* key, float x, float y, float z, float w) { shader_exports.set_custom_params(key, Fvector4{x, y, z, w}); }

static void shader_get_custom_param(const char* key, float& x, float& y, float& z, float& w)
{
    const auto& v = shader_exports.get_custom_params(key);
    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;
}


/////////
u32 PlayHudMotion(u8 hand, LPCSTR hud_section, LPCSTR anm_name, bool bMixIn = true, float speed = 1.f, bool bOverride_item = false)
{
    return g_player_hud->script_anim_play(hand, hud_section, anm_name, bMixIn, speed, bOverride_item);
}

void StopHudMotion() { g_player_hud->script_anim_stop(); }

float MotionLength(LPCSTR hud_section, LPCSTR anm_name, float speed) { return g_player_hud->motion_length_script(hud_section, anm_name, speed); }

bool AllowHudMotion() { return g_player_hud->allow_script_anim(); }

float PlayBlendAnm(LPCSTR name, u8 part, float speed, float power, bool bLooped, bool no_restart)
{
    return g_player_hud->PlayBlendAnm(name, part, speed, power, bLooped, no_restart);
}

void StopBlendAnm(LPCSTR name, bool bForce) { g_player_hud->StopBlendAnm(name, bForce); }

void StopAllBlendAnms(bool bForce) { g_player_hud->StopAllBlendAnms(bForce); }

float SetBlendAnmTime(LPCSTR name, float time) { return g_player_hud->SetBlendAnmTime(name, time); }
/////////


void CLevel::script_register(lua_State* L)
{
    module(L)[(class_<CEnvDescriptor>("CEnvDescriptor")
                  .def_readwrite("fog_density", &CEnvDescriptor::fog_density)
                  .def_readwrite("fog_distance", &CEnvDescriptor::fog_distance)
                  .def_readwrite("far_plane", &CEnvDescriptor::far_plane)
                  .def_readwrite("sun_dir", &CEnvDescriptor::sun_dir)
                  .def_readwrite("wind_velocity", &CEnvDescriptor::wind_velocity)
                  .def_readwrite("wind_direction", &CEnvDescriptor::wind_direction)
                  .def_readwrite("m_fSunShaftsIntensity", &CEnvDescriptor::m_fSunShaftsIntensity)
                  .property("m_identifier", [](CEnvDescriptor* self) { return self->m_identifier.c_str(); })
                  .def("set_env_ambient", &CEnvDescriptor::setEnvAmbient),
              class_<CEnvironment>("CEnvironment")
                  .def("getCurrentWeather", [](CEnvironment* self, const size_t idx) {
                      R_ASSERT(idx < 2);
                      return self->Current[idx];
                  }),

    class_<CPHCommanderScripts::CPHCallScript>("CPHCall").def("set_pause", &CPHCommanderScripts::CPHCallScript::setPause))];

    module(L, "level")
        [(
        // obsolete\deprecated
        def("object_by_id", &get_object_by_id),
#ifdef DEBUG
        def("debug_object", &get_object_by_name), def("debug_actor", &tpfGetActor), def("check_object", &check_object),
#endif

        def("get_weather", &get_weather), def("get_weather_prev", &get_weather_prev), def("get_weather_last_shift", &get_weather_last_shift), def("set_weather", &set_weather),
        def("set_weather_next", &set_weather_next), def("set_weather_fx", &set_weather_fx), def("start_weather_fx_from_time", &start_weather_fx_from_time),
        def("is_wfx_playing", &is_wfx_playing), def("get_wfx_time", &get_wfx_time), def("stop_weather_fx", &stop_weather_fx),

        def("environment", [] { return &g_pGamePersistent->Environment(); }),

        def("set_time_factor", &set_time_factor), def("get_time_factor", &get_time_factor),

        def("set_game_difficulty", &set_game_difficulty), def("get_game_difficulty", &get_game_difficulty),

        def("get_time_days", &get_time_days), def("get_time_hours", &get_time_hours), def("get_time_minutes", &get_time_minutes), def("change_game_time", &change_game_time),

        def("high_cover_in_direction", &high_cover_in_direction), def("low_cover_in_direction", &low_cover_in_direction), def("vertex_in_direction", &vertex_in_direction),
        def("rain_factor", &rain_factor), def("rain_hemi", rain_hemi),
        def("rain_wetness", [] { return g_pGamePersistent->Environment().wetness_factor; }),
        def("set_rain_wetness",
            [](float val) {
                clamp(val, 0.f, 1.f);
                g_pGamePersistent->Environment().wetness_factor = val;
            }),
        def("patrol_path_exists", &patrol_path_exists), def("vertex_position", &vertex_position), def("name", &get_name),
        def("prefetch_sound", &prefetch_sound),

        def("client_spawn_manager", &get_client_spawn_manager),

        def("map_add_object_spot_ser", &map_add_object_spot_ser), def("map_add_object_spot", &map_add_object_spot), def("map_remove_object_spot", &map_remove_object_spot),
        def("map_has_object_spot", &map_has_object_spot), def("map_change_spot_hint", &map_change_spot_hint),

        def("start_stop_menu", start_stop_menu), def("main_input_receiver", main_input_receiver),
        def("add_dialog_to_render", &add_dialog_to_render), def("remove_dialog_to_render", &remove_dialog_to_render), def("hide_indicators", &hide_indicators),
        def("hide_indicators_safe", &hide_indicators_safe),

        def("show_indicators", &show_indicators), def("show_weapon", &show_weapon),
        def("add_call", ((CPHCommanderScripts::CPHCallScript* (*)(const luabind::functor<bool>&, const luabind::functor<void>&)) & add_call)),
        def("add_call", ((CPHCommanderScripts::CPHCallScript* (*)(const luabind::object&, const luabind::functor<bool>&, const luabind::functor<void>&)) & add_call)),
        def("add_call", ((CPHCommanderScripts::CPHCallScript* (*)(const luabind::object&, LPCSTR, LPCSTR)) & add_call)),
        def("remove_call", ((void (*)(const luabind::functor<bool>&, const luabind::functor<void>&))&remove_call)),
        def("remove_call", ((void (*)(const luabind::object&, const luabind::functor<bool>&, const luabind::functor<void>&))&remove_call)),
        def("remove_call", ((void (*)(const luabind::object&, LPCSTR, LPCSTR))&remove_call)), def("remove_calls_for_object", &remove_calls_for_object),
        def("present", &is_level_present), def("disable_input", &disable_input), def("enable_input", &enable_input), def("block_player_action", &block_player_action),
        def("unblock_player_action", &unblock_player_action), def("spawn_phantom", &spawn_phantom),

        def("get_bounding_volume", &get_bounding_volume),

        def("get_ambient_music_timeout", [] { return Level().m_level_sound_manager->GetMusicInterval(); }),
        def("set_ambient_music_timeout", [](const u32 time) { Level().m_level_sound_manager->SetMusicInterval(time); }),
        def("get_ambient_music_use_pause", [] { return Level().m_level_sound_manager->GetMusicUsePause(); }),
        def("set_ambient_music_use_pause", [](const bool val) { Level().m_level_sound_manager->SetMusicUsePause(val); }),

        def("iterate_sounds", &iterate_sounds1), def("iterate_sounds", &iterate_sounds2), def("physics_world", &physics_world_scripted), def("get_snd_volume", &get_snd_volume),
        def("set_snd_volume", &set_snd_volume), def("add_cam_effector", &add_cam_effector), def("add_cam_effector2", &add_cam_effector2),
        def("remove_cam_effector", &remove_cam_effector), def("add_pp_effector", &add_pp_effector), def("set_pp_effector_factor", &set_pp_effector_factor),
        def("set_pp_effector_factor", &set_pp_effector_factor2), def("remove_pp_effector", &remove_pp_effector),

        def("add_complex_effector", &add_complex_effector), def("remove_complex_effector", &remove_complex_effector),

        def("vertex_id", &vertex_id),

        def("game_id", &GameID), def("is_ray_intersect_sphere", &is_ray_intersect_sphere), def("get_target_obj", &g_get_target_obj), def("get_target_dist", &g_get_target_dist))],

    module(L, "actor_stats")[(def("add_points", &add_actor_points), def("add_points_str", &add_actor_points_str), def("get_points", &get_actor_points))];

    module(L)[(def("command_line", &command_line), def("IsDynamicMusic", &IsDynamicMusic), def("IsImportantSave", &IsImportantSave))];

    module(L, "relation_registry")[(def("community_goodwill", &g_community_goodwill), def("set_community_goodwill", &g_set_community_goodwill),
                                   def("change_community_goodwill", &g_change_community_goodwill),

                                   def("community_relation", &g_get_community_relation), def("set_community_relation", &g_set_community_relation),
                                   def("get_general_goodwill_between", &g_get_general_goodwill_between))];

        // установка параметров для шейдеров из скриптов
    module(L)[(def("set_pda_params", [](const Fvector& p) { shader_exports.set_pda_params(p); }),

              def("set_dof_params", [](const float& p1, const float& p2, const float& p3, const float& p4) { shader_exports.set_dof_params(p1, p2, p3, p4); }),

              class_<enum_exporter<collide::rq_target>>("rq_target")
                  .enum_("rq_target")[(value("rqtNone", int(collide::rqtNone)), value("rqtObject", int(collide::rqtObject)), value("rqtStatic", int(collide::rqtStatic)),
                                      value("rqtShape", int(collide::rqtShape)), value("rqtObstacle", int(collide::rqtObstacle)), value("rqtBoth", int(collide::rqtBoth)),
                                      value("rqtDyn", int(collide::rqtDyn)))],

              def("shader_set_custom_param_vector", &shader_set_custom_param_vector), def("shader_get_custom_param_vector", &shader_get_custom_param_vector),
              def("shader_set_custom_param", &shader_set_custom_param),
              def("shader_get_custom_param", &shader_get_custom_param, pure_out_value<2>() + pure_out_value<3>() + pure_out_value<4>() + pure_out_value<5>()))];

    module(L,
           "game")[(class_<xrTime>("CTime")
                       .enum_("date_format")[(value("DateToDay", int(InventoryUtilities::edpDateToDay)), value("DateToMonth", int(InventoryUtilities::edpDateToMonth)),
                                             value("DateToYear", int(InventoryUtilities::edpDateToYear)))]
                       .enum_("time_format")[(value("TimeToHours", int(InventoryUtilities::etpTimeToHours)), value("TimeToMinutes", int(InventoryUtilities::etpTimeToMinutes)),
                                             value("TimeToSeconds", int(InventoryUtilities::etpTimeToSeconds)), value("TimeToMilisecs", int(InventoryUtilities::etpTimeToMilisecs)))]
                       .def(constructor<>())
                       .def(constructor<const xrTime&>())
                       .def(const_self < xrTime())
                       .def(const_self <= xrTime())
                       .def(const_self > xrTime())
                       .def(const_self >= xrTime())
                       .def(const_self == xrTime())
                       .def(self + xrTime())
                       .def(self - xrTime())

                       .def("diffSec", &xrTime::diffSec_script)
                       .def("add", &xrTime::add_script)
                       .def("sub", &xrTime::sub_script)

                       .def("setHMS", &xrTime::setHMS)
                       .def("setHMSms", &xrTime::setHMSms)
                       .def("set", &xrTime::set)
                       .def("get", &xrTime::get, out_value<2>() + out_value<3>() + out_value<4>() + out_value<5>() + out_value<6>() + out_value<7>() + out_value<8>())
                       .def("dateToString", &xrTime::dateToString)
                       .def("timeToString", &xrTime::timeToString),
                   // declarations
                   def("time", &get_time), def("get_game_time", &get_time_struct),

                   def("start_tutorial", &start_tutorial), def("stop_tutorial", &stop_tutorial), def("has_active_tutorial", &has_active_tutotial),
                   def("translate_string", &translate_string),
        def("play_hud_motion", PlayHudMotion), def("stop_hud_motion", StopHudMotion), def("get_motion_length", MotionLength), def("hud_motion_allowed", AllowHudMotion),
        def("play_hud_anm", PlayBlendAnm), def("stop_hud_anm", StopBlendAnm), def("stop_all_hud_anms", StopAllBlendAnms), def("set_hud_anm_time", SetBlendAnmTime)

    )];
}

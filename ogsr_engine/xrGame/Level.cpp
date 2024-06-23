#include "stdafx.h"
#include "../xr_3da/fdemorecord.h"
#include "../xr_3da/fdemoplay.h"
#include "../xr_3da/environment.h"
#include "../xr_3da/IGame_Persistent.h"
#include "ParticlesObject.h"
#include "Level.h"
#include "xrServer.h"
#include "net_queue.h"
#include "game_cl_base.h"
#include "entity_alive.h"
#include "hudmanager.h"
#include "ai_space.h"
#include "ai_debug.h"
#include "PHdynamicdata.h"
#include "Physics.h"
#include "ShootingObject.h"
#include "Level_Bullet_Manager.h"
#include "script_engine.h"
#include "infoportion.h"
#include "patrol_path_storage.h"
#include "date_time.h"
#include "space_restriction_manager.h"
#include "seniority_hierarchy_holder.h"
#include "space_restrictor.h"
#include "client_spawn_manager.h"
#include "ClimableObject.h"
#include "level_graph.h"
#include "mt_config.h"
#include "phcommander.h"
#include "map_manager.h"
#include "../xr_3da/cameramanager.h"
#include "level_sounds.h"
#include "car.h"
#include "trade_parameters.h"
#include "clsid_game.h"
#include "MainMenu.h"
#include "../xr_3da/XR_IOConsole.h"
#include "Debug_Renderer.h"
#include "Actor_Flags.h"
#include "level_changer.h"
#include "player_hud.h"
#include "Actor.h"
#include "PDA.h"

#ifdef DEBUG
#include "level_debug.h"
#include "ai/stalker/ai_stalker.h"
#include "physicobject.h"
#endif

#include "LevelDebugScript.h"
#include "physicobject.h"
#include "UIGameSP.h"
#include "ui/UIPDAWnd.h"
#include "ui/UIBtnHint.h"

#include "embedded_editor/embedded_editor_main.h"

CPHWorld* ph_world = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLevel::CLevel()
    : IPureClient(Device.GetTimerGlobal())
#ifdef PROFILE_CRITICAL_SECTIONS
      ,
      DemoCS(MUTEX_PROFILE_ID(DemoCS))
#endif // PROFILE_CRITICAL_SECTIONS
{
    g_bDebugEvents = strstr(Core.Params, "-debug_ge") ? TRUE : FALSE;

    Server = NULL;

    game = NULL;
    //	game						= xr_new<game_cl_GameState>();
    game_events = xr_new<NET_Queue_Event>();

    game_configured = FALSE;
    m_bGameConfigStarted = FALSE;

    eChangeRP = Engine.Event.Handler_Attach("LEVEL:ChangeRP", this);
    eDemoPlay = Engine.Event.Handler_Attach("LEVEL:PlayDEMO", this);
    eChangeTrack = Engine.Event.Handler_Attach("LEVEL:PlayMusic", this);
    eEnvironment = Engine.Event.Handler_Attach("LEVEL:Environment", this);

    eEntitySpawn = Engine.Event.Handler_Attach("LEVEL:spawn", this);

    m_pBulletManager = xr_new<CBulletManager>();

    m_map_manager = xr_new<CMapManager>();

    physics_step_time_callback = (PhysicsStepTimeCallback*)&PhisStepsCallback;
    m_seniority_hierarchy_holder = xr_new<CSeniorityHierarchyHolder>();

    m_level_sound_manager = xr_new<CLevelSoundManager>();
    m_space_restriction_manager = xr_new<CSpaceRestrictionManager>();
    m_client_spawn_manager = xr_new<CClientSpawnManager>();

    m_debug_renderer = xr_new<CDebugRenderer>();
#ifdef DEBUG
    m_level_debug = xr_new<CLevelDebug>();
#endif

    m_ph_commander = xr_new<CPHCommander>();
    m_ph_commander_scripts = xr_new<CPHCommander>();

#ifdef DEBUG
    m_bSynchronization = false;
#endif
    //---------------------------------------------------------
    pStatGraphR = NULL;
    pStatGraphS = NULL;
    //---------------------------------------------------------
    pCurrentControlEntity = NULL;

    //---------------------------------------------------------
    m_dwCL_PingLastSendTime = 0;
    m_dwCL_PingDeltaSend = 1000;
    m_dwRealPing = 0;

    //---------------------------------------------------------

    //	if ( !strstr( Core.Params, "-tdemo " ) && !strstr(Core.Params,"-tdemof "))
    //	{
    //		Demo_PrepareToStore();
    //	};
    //---------------------------------------------------------
    //	m_bDemoPlayMode = FALSE;
    //	m_aDemoData.clear();
    //	m_bDemoStarted	= FALSE;

    /*
    if (strstr(Core.Params,"-tdemo ") || strstr(Core.Params,"-tdemof ")) {
        string1024				f_name;
        if (strstr(Core.Params,"-tdemo "))
        {
            sscanf					(strstr(Core.Params,"-tdemo ")+7,"%[^ ] ",f_name);
            m_bDemoPlayByFrame = FALSE;

            Demo_Load	(f_name);
        }
        else
        {
            sscanf					(strstr(Core.Params,"-tdemof ")+8,"%[^ ] ",f_name);
            m_bDemoPlayByFrame = TRUE;

            m_lDemoOfs = 0;
            Demo_Load_toFrame(f_name, 100, m_lDemoOfs);
        };
    }
    */
    //---------------------------------------------------------

    m_is_removing_objects = false;

    g_player_hud = xr_new<player_hud>();
    g_player_hud->load_default();
}

extern CAI_Space* g_ai_space;

CLevel::~CLevel()
{
    xr_delete(g_player_hud);
    //	g_pGameLevel		= NULL;
    Msg("- Destroying level");

    Engine.Event.Handler_Detach(eEntitySpawn, this);

    Engine.Event.Handler_Detach(eEnvironment, this);
    Engine.Event.Handler_Detach(eChangeTrack, this);
    Engine.Event.Handler_Detach(eDemoPlay, this);
    Engine.Event.Handler_Detach(eChangeRP, this);

    if (ph_world)
    {
        ph_world->Destroy();
        xr_delete(ph_world);
    }

    // destroy PSs
    for (POIt p_it = m_StaticParticles.begin(); m_StaticParticles.end() != p_it; ++p_it)
        CParticlesObject::Destroy(*p_it);
    m_StaticParticles.clear();

	// Unload sounds
	// unload prefetched sounds
    sound_registry.clear();
	sound_registry_defer.clear();

    // unload static sounds
    for (u32 i = 0; i < static_Sounds.size(); ++i)
    {
        static_Sounds[i]->destroy();
        xr_delete(static_Sounds[i]);
    }
    static_Sounds.clear();

    xr_delete(m_level_sound_manager);

    xr_delete(m_space_restriction_manager);

    xr_delete(m_seniority_hierarchy_holder);

    xr_delete(m_client_spawn_manager);

    xr_delete(m_debug_renderer);

    xr_delete(game);
    xr_delete(game_events);

    // by Dandy
    // destroy fog of war
    //	xr_delete					(m_pFogOfWar);
    // destroy bullet manager
    xr_delete(m_pBulletManager);
    //-----------------------------------------------------------
    xr_delete(pStatGraphR);
    xr_delete(pStatGraphS);

    //-----------------------------------------------------------
    xr_delete(m_ph_commander);
    xr_delete(m_ph_commander_scripts);
    //-----------------------------------------------------------

    ai().unload();
    //-----------------------------------------------------------
#ifdef DEBUG
    xr_delete(m_level_debug);
#endif
    //-----------------------------------------------------------
    xr_delete(m_map_manager);
    //	xr_delete					(m_pFogOfWarMngr);
    //-----------------------------------------------------------

    // here we clean default trade params
    // because they should be new for each saved/loaded game
    // and I didn't find better place to put this code in
    CTradeParameters::clean();

    for (auto& i : m_debug_render_queue)
    {
        xr_delete(i.second);
    }

    m_debug_render_queue.clear();
}

shared_str CLevel::name() const { return (m_name); }

bool CLevel::PrefetchSound(LPCSTR name)
{
    // preprocess sound name
    string_path tmp;
    strcpy_s(tmp, name);
    xr_strlwr(tmp);
    if (strext(tmp))
        *strext(tmp) = 0;
    shared_str snd_name = tmp;
    // find in registry
    SoundRegistryMapIt it = sound_registry.find(snd_name);
    // if find failed - preload sound
    if (it == sound_registry.end())
    {
        sound_registry[snd_name].create(snd_name.c_str(), st_Effect, sg_SourceType);
        return true;
    }
    return false;
}

bool CLevel::PrefetchManySounds(LPCSTR prefix)
{
    bool created = false;
    string_path fn;
    if (FS.exist(fn, "$game_sounds$", prefix, ".ogg"))
        created = PrefetchSound(prefix) || created;
    u32 i = 0;
    while (true)
    {
        string256 name;
        sprintf_s(name, "%s%d", prefix, i);
        if (FS.exist(fn, "$game_sounds$", name, ".ogg"))
            created = PrefetchSound(name) || created;
        else if (i > 0)
            break;
        i++;
    }
    return created;
}

bool CLevel::PrefetchManySoundsLater(LPCSTR prefix)
{
    std::string s(prefix);
    for (const auto& it : sound_registry_defer)
    {
        if (it == s)
            return false;
    }
    sound_registry_defer.push_back(s);
    return true;
}

void CLevel::PrefetchDeferredSounds()
{
    while (!sound_registry_defer.empty())
    {
        std::string s = sound_registry_defer.front();
        sound_registry_defer.pop_front();
        if (PrefetchManySounds(s.c_str()))
            break;
    }
}

void CLevel::CancelPrefetchManySounds(LPCSTR prefix)
{
    std::string s(prefix);
    sound_registry_defer.erase(std::remove(sound_registry_defer.begin(), sound_registry_defer.end(), s), sound_registry_defer.end());
}

// Game interface ////////////////////////////////////////////////////
int CLevel::get_RPID(LPCSTR /**name/**/)
{
    /*
    // Gain access to string
    LPCSTR	params = pLevel->r_string("respawn_point",name);
    if (0==params)	return -1;

    // Read data
    Fvector4	pos;
    int			team;
    sscanf		(params,"%f,%f,%f,%d,%f",&pos.x,&pos.y,&pos.z,&team,&pos.w); pos.y += 0.1f;

    // Search respawn point
    svector<Fvector4,maxRP>	&rp = Level().get_team(team).RespawnPoints;
    for (int i=0; i<(int)(rp.size()); ++i)
        if (pos.similar(rp[i],EPS_L))	return i;
    */
    return -1;
}

BOOL g_bDebugEvents = FALSE;

void CLevel::cl_Process_Event(u16 dest, u16 type, NET_Packet& P)
{
    //			Msg				("--- event[%d] for [%d]",type,dest);
    CObject* O = Objects.net_Find(dest);
    if (0 == O)
    {
#ifdef DEBUG
        Msg("* WARNING: c_EVENT[%d] to [%d]: unknown dest", type, dest);
#endif // DEBUG
        ProcessGameSpawnsDestroy(dest, type, P);
        return;
    }
    CGameObject* GO = smart_cast<CGameObject*>(O);
    if (!GO)
    {
        Msg("! ERROR: c_EVENT[%d] : non-game-object", dest);
        return;
    }

    if (type == GE_DESTROY)
        Game().OnDestroy(GO);

    GO->OnEvent(P, type);
}

void CLevel::ProcessGameEvents()
{
    // Game events
    {
        NET_Packet P;
        u32 svT = timeServer() - NET_Latency;

        /*
        if (!game_events->queue.empty())
            Msg("- d[%d],ts[%d] -- E[svT=%d],[evT=%d]",Device.dwTimeGlobal,timeServer(),svT,game_events->queue.begin()->timestamp);
        */

        m_just_destroyed.clear();

        while (game_events->available(svT))
        {
            u16 ID, dest, type;
            game_events->get(ID, dest, type, P);

            switch (ID)
            {
            case M_SPAWN: {
                u16 dummy16;
                P.r_begin(dummy16);
                cl_Process_Spawn(P);
            }
            break;
            case M_EVENT: {
                cl_Process_Event(dest, type, P);
            }
            break;
            default: {
                VERIFY(0);
            }
            break;
            }
        }
    }

    if (!is_removing_objects())
        Device.add_to_seq_parallel(fastdelegate::MakeDelegate(this, &CLevel::ProcessGameSpawns));
}

void CLevel::OnFrame()
{
    m_feel_deny.update();

    psDeviceFlags.set(rsDisableObjectsAsCrows, false);

    // commit events from bullet manager from prev-frame
    Device.Statistic->TEST0.Begin();
    BulletManager().CommitEvents();
    Device.Statistic->TEST0.End();

    Device.Statistic->netClient1.Begin();

    ClientReceive();

    Device.Statistic->netClient1.End();

    ProcessGameEvents();

    MapManager().Update();
    // Inherited update
    inherited::OnFrame();

    extern bool s_ScriptTime;

    if (!s_ScriptTime)
    {
        g_pGamePersistent->Environment().SetGameTime(GetEnvironmentGameDayTimeSec(), game->GetEnvironmentGameTimeFactor());
    }

    m_ph_commander->update();
    m_ph_commander_scripts->update();

    //просчитать полет пуль
    Device.Statistic->TEST0.Begin();
    BulletManager().CommitRenderSet();
    Device.Statistic->TEST0.End();

    // update static sounds
    if (g_mt_config.test(mtLevelSounds))
        Device.add_to_seq_parallel(fastdelegate::MakeDelegate(m_level_sound_manager, &CLevelSoundManager::Update));
    else
        m_level_sound_manager->Update();

    if (!sound_registry_defer.empty())
        Device.add_to_seq_parallel(fastdelegate::MakeDelegate(this, &CLevel::PrefetchDeferredSounds));

    //-----------------------------------------------------
    if (pStatGraphR)
    {
        static float fRPC_Mult = 10.0f;
        static float fRPS_Mult = 1.0f;

        pStatGraphR->AppendItem(float(m_dwRPC) * fRPC_Mult, 0xffff0000, 1);
        pStatGraphR->AppendItem(float(m_dwRPS) * fRPS_Mult, 0xff00ff00, 0);
    };

    ShowEditor();
}

extern Flags32 dbg_net_Draw_Flags;

extern void draw_wnds_rects();

void CLevel::OnRender()
{
    Render->BeforeWorldRender(); //--#SM+#-- +SecondVP+

    inherited::OnRender();

    Game().OnRender();

    //отрисовать трассы пуль
    // Device.Statistic->TEST1.Begin();
    BulletManager().Render();
    // Device.Statistic->TEST1.End();

    auto pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
    auto pActor = smart_cast<CActor*>(Level().CurrentEntity());
    CPda* Pda = pActor ? pActor->GetPDA() : nullptr;
    const bool need_pda_render = Pda && Pda->Is3DPDA() && psActorFlags.test(AF_3D_PDA) && pGameSP->PdaMenu->IsShown();
    Render->AfterWorldRender(need_pda_render);

    if (need_pda_render)
    {
        HUD().RenderUI();
        if (g_btnHint)
            g_btnHint->OnRender();
        GetUICursor()->OnRender();
        draw_wnds_rects();

        Fvector2 cursor_pos = GetUICursor()->GetCursorPosition();
        Fvector2 cursor_pos_dif{cursor_pos};
        cursor_pos_dif.sub(pGameSP->PdaMenu->last_cursor_pos);
        pGameSP->PdaMenu->last_cursor_pos.set(cursor_pos);
        pGameSP->PdaMenu->MouseMovement(cursor_pos_dif.x, cursor_pos_dif.y);

        Render->AfterUIRender();
    }

    HUD().RenderUI();
    if (g_btnHint)
        g_btnHint->OnRender();
    GetUICursor()->OnRender();
    draw_wnds_rects();

#ifndef DEBUG

    if (psActorFlags.test(AF_ZONES_DBG))
    {
        for (u32 I = 0; I < Level().Objects.o_count(); I++)
        {
            auto _O = Level().Objects.o_get_by_iterator(I);
            auto space_restrictor = smart_cast<CSpaceRestrictor*>(_O);
            if (space_restrictor)
                space_restrictor->OnRender();

            auto level_changer = smart_cast<CLevelChanger*>(_O);
            if (level_changer)
                level_changer->OnRender();

            auto climable = smart_cast<CClimableObject*>(_O);
            if (climable)
                climable->OnRender();
        }
    }

    if (psActorFlags.test(AF_VERTEX_DBG))
    {
        if (ai().get_level_graph())
            ai().level_graph().render();
    }

    if (dbg_net_Draw_Flags.test(1 << 11)) // draw skeleton
    {
        for (u32 I = 0; I < Level().Objects.o_count(); I++)
        {
            CObject* _O = Level().Objects.o_get_by_iterator(I);

            CGameObject* pGO = smart_cast<CGameObject*>(_O);
            if (pGO /*&& pGO != Level().CurrentViewEntity()*/ && !pGO->H_Parent())
            {
                if (pGO->Position().distance_to_sqr(Device.vCameraPosition) < 400.0f)
                {
                    //CPhysicObject* physic_object = smart_cast<CPhysicObject*>(_O);
                    //if (physic_object)
                    //    physic_object->OnRender();

                    pGO->dbg_DrawSkeleton();
                }
            }
        }
    }

#else

    ph_world->OnRender();

    if (ai().get_level_graph())
        ai().level_graph().render();

#ifdef DEBUG_PRECISE_PATH
    test_precise_path();
#endif

    CAI_Stalker* stalker = smart_cast<CAI_Stalker*>(Level().CurrentEntity());
    if (stalker)
        stalker->OnRender();

    if (bDebug)
    {
        ObjectSpace.dbgRender();

        //---------------------------------------------------------------------
        HUD().Font().pFontStat->OutSet(170, 630);
        HUD().Font().pFontStat->SetHeight(16.0f);
        HUD().Font().pFontStat->SetColor(0xffff0000);

        if (Server)
            HUD().Font().pFontStat->OutNext("Client Objects:      [%d]", Server->GetEntitiesNum());
        HUD().Font().pFontStat->OutNext("Server Objects:      [%d]", Objects.o_count());
        HUD().Font().pFontStat->OutNext("Interpolation Steps: [%d]", Level().GetInterpolationSteps());
        HUD().Font().pFontStat->SetHeight(8.0f);
        //---------------------------------------------------------------------
    }

    ScriptDebugRender();

    if (psActorFlags.test(AF_ZONES_DBG))
    {
        DBG().draw_object_info();
        DBG().draw_text();
        DBG().draw_level_info();
    }
#endif

    debug_renderer().render();

#ifdef DEBUG

    if (psAI_Flags.is(aiVision))
    {
        for (u32 I = 0; I < Level().Objects.o_count(); I++)
        {
            CObject* object = Objects.o_get_by_iterator(I);
            CAI_Stalker* stalker = smart_cast<CAI_Stalker*>(object);
            if (!stalker)
                continue;
            stalker->dbg_draw_vision();
        }
    }

    if (psAI_Flags.test(aiDrawVisibilityRays))
    {
        for (u32 I = 0; I < Level().Objects.o_count(); I++)
        {
            CObject* object = Objects.o_get_by_iterator(I);
            CAI_Stalker* stalker = smart_cast<CAI_Stalker*>(object);
            if (!stalker)
                continue;

            stalker->dbg_draw_visibility_rays();
        }
    }

#endif
}

void CLevel::OnEvent(EVENT E, u64 P1, u64 /**P2/**/)
{
    if (E == eEntitySpawn)
    {
        char Name[128];
        Name[0] = 0;
        sscanf(LPCSTR(P1), "%s", Name);
        Level().g_cl_Spawn(Name, 0xff, M_SPAWN_OBJECT_LOCAL, Fvector().set(0, 0, 0));
    }
    else if (E == eChangeRP && P1)
    {}
    else if (E == eDemoPlay && P1)
    {
        char* name = (char*)P1;
        string_path RealName;
        strcpy_s(RealName, name);
        strcat_s(RealName, ".xrdemo");
        Cameras().AddCamEffector(xr_new<CDemoPlay>(RealName, 1.3f, 0));
    }
    else if (E == eChangeTrack && P1)
    {
        // int id = atoi((char*)P1);
        // Environment->Music_Play(id);
    }
    else if (E == eEnvironment)
    {
        // int id=0; float s=1;
        // sscanf((char*)P1,"%d,%f",&id,&s);
        // Environment->set_EnvMode(id,s);
    }
    else
        return;
}

void CLevel::PhisStepsCallback(u32, u32) {}

ALife::_TIME_ID CLevel::GetGameTime()
{
    return (game->GetGameTime());
    //	return			(Server->game->GetGameTime());
}

ALife::_TIME_ID CLevel::GetEnvironmentGameTime()
{
    return (game->GetEnvironmentGameTime());
    //	return			(Server->game->GetGameTime());
}

u8 CLevel::GetDayTime()
{
    u32 dummy32;
    u32 hours;
    GetGameDateTime(dummy32, dummy32, dummy32, hours, dummy32, dummy32, dummy32);
    VERIFY(hours < 256);
    return u8(hours);
}

float CLevel::GetGameDayTimeSec() { return (float(s64(GetGameTime() % (24 * 60 * 60 * 1000))) / 1000.f); }

u32 CLevel::GetGameDayTimeMS() { return (u32(s64(GetGameTime() % (24 * 60 * 60 * 1000)))); }

float CLevel::GetEnvironmentGameDayTimeSec() { return (float(s64(GetEnvironmentGameTime() % (24 * 60 * 60 * 1000))) / 1000.f); }

void CLevel::ScriptDebugRender()
{
    if (!m_debug_render_queue.size())
        return;

    bool hasVisibleObj = false;

    xr_map<u16, DBG_ScriptObject*>::iterator it = m_debug_render_queue.begin();
    xr_map<u16, DBG_ScriptObject*>::iterator it_e = m_debug_render_queue.end();
    for (; it != it_e; ++it)
    {
        DBG_ScriptObject* obj = (*it).second;
        if (obj->m_visible)
        {
            obj->Render();
            hasVisibleObj = true;
        }
    }

	// demonized: fix of showing console window when there are no visible gizmos
    if (hasVisibleObj)
        DRender->OnFrameEnd();
}

void CLevel::GetGameDateTime(u32& year, u32& month, u32& day, u32& hours, u32& mins, u32& secs, u32& milisecs)
{
    split_time(GetGameTime(), year, month, day, hours, mins, secs, milisecs);
}

float CLevel::GetGameTimeFactor()
{
    return (game->GetGameTimeFactor());
    //	return			(Server->game->GetGameTimeFactor());
}

void CLevel::SetGameTimeFactor(const float fTimeFactor)
{
    game->SetGameTimeFactor(fTimeFactor);
    //	Server->game->SetGameTimeFactor(fTimeFactor);
}

void CLevel::SetGameTimeFactor(ALife::_TIME_ID GameTime, const float fTimeFactor)
{
    game->SetGameTimeFactor(GameTime, fTimeFactor);
    //	Server->game->SetGameTimeFactor(fTimeFactor);
}

void CLevel::SetEnvironmentGameTimeFactor(u64 const& GameTime, float const& fTimeFactor)
{
    if (!game)
        return;

    game->SetEnvironmentGameTimeFactor(GameTime, fTimeFactor);
}

void CLevel::GetGameTimeForShaders(u32& hours, u32& minutes, u32& seconds, u32& milliseconds)
{
    u32 unused;
    split_time(GetGameTime(), unused, unused, unused, hours, minutes, seconds, milliseconds);
}

//bool CLevel::IsServer() // always false
//{
//    //if (!Server)
//    //    return false;
//
//    bool r = !Server ? false : (Server->client_Count() != 0);
//    Msg("IsServer = %d", r ? 1 : 0);
//    return r;
//}
//
//bool CLevel::IsClient() // always false
//{
//    //if (!Server)
//    //    return true;
//
//    bool r = !Server ? true : (Server->client_Count() == 0);
//    Msg("IsClient = %d", r ? 1 : 0);
//    return r;
//}

void CLevel::OnSessionTerminate(LPCSTR reason) { MainMenu()->OnSessionTerminate(reason); }

void CLevel::OnDestroyObject(u16 id) { m_just_destroyed.push_back(id); }

void CLevel::OnChangeCurrentWeather(const char* sect)
{
    if (on_change_weather_callback.empty())
        return;
    luabind::functor<void> funct;
    if (ai().script_engine().functor(on_change_weather_callback.c_str(), funct))
        funct(sect);
}

u32 GameID() { return Game().Type(); }

#include "..\xr_3da\IGame_Persistent.h"

GlobalFeelTouch::GlobalFeelTouch() {}

GlobalFeelTouch::~GlobalFeelTouch() {}

struct delete_predicate_by_time
{
    bool operator()(Feel::Touch::DenyTouch const& left, DWORD const expire_time) const
    {
        if (left.Expire <= expire_time)
            return true;
        return false;
    };
};
struct objects_ptrs_equal
{
    bool operator()(Feel::Touch::DenyTouch const& left, CObject const* const right) const
    {
        if (left.O == right)
            return true;
        return false;
    }
};

void GlobalFeelTouch::update()
{
    // we ignore P and R arguments, we need just delete evaled denied objects...
    xr_vector<Feel::Touch::DenyTouch>::iterator new_end =
        std::remove_if(feel_touch_disable.begin(), feel_touch_disable.end(), std::bind(delete_predicate_by_time(), std::placeholders::_1, Device.dwTimeGlobal));
    feel_touch_disable.erase(new_end, feel_touch_disable.end());
}

bool GlobalFeelTouch::is_object_denied(CObject const* O)
{
    /*Fvector temp_vector;
    feel_touch_update(temp_vector, 0.f);*/
    if (std::find_if(feel_touch_disable.begin(), feel_touch_disable.end(), std::bind(objects_ptrs_equal(), std::placeholders::_1, O)) == feel_touch_disable.end())
    {
        return false;
    }
    return true;
}

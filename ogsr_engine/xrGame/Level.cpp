#include "stdafx.h"
#include "../xr_3da/FDemoRecord.h"
#include "../xr_3da/FDemoPlay.h"
#include "../xr_3da/Environment.h"
#include "../xr_3da/GameMtlLib.h"
#include "../xr_3da/IGame_Persistent.h"
#include "ParticlesObject.h"
#include "Level.h"
#include "HUDManager.h"
#include "xrServer.h"
#include "NET_Queue.h"
#include "game_cl_base.h"
#include "entity_alive.h"
#include "ai_space.h"
#include "ai_debug.h"
// #include "PHdynamicdata.h"
// #include "Physics.h"
#include "ShootingObject.h"
#include "GametaskManager.h"
#include "Level_Bullet_Manager.h"
#include "script_engine.h"
#include "team_base_zone.h"
#include "InfoPortion.h"
#include "patrol_path_storage.h"
#include "date_time.h"
#include "space_restriction_manager.h"
#include "seniority_hierarchy_holder.h"
#include "space_restrictor.h"
#include "client_spawn_manager.h"
#include "autosave_manager.h"
#include "ClimableObject.h"
#include "level_graph.h"
#include "mt_config.h"
#include "PHCommander.h"
#include "map_manager.h"
#include "../xr_3da/CameraManager.h"
#include "level_sounds.h"
#include "Car.h"
#include "trade_parameters.h"
#include "MainMenu.h"
#include "../xr_3da/XR_IOConsole.h"
#include "Actor.h"
#include "player_hud.h"
#include "ui/UIGameTutorial.h"
#include "CustomDetector.h"

#include "ingame_editors/embedded_editor_main.h"

#include "../xrPhysics/IPHWorld.h"
#include "../xrPhysics/console_vars.h"
#include "../xr_3da/xr_ioc_cmd.h"

#include "Actor_Flags.h"
#include "debug_renderer.h"
#ifdef DEBUG
#include "level_debug.h"
#include "ai/stalker/ai_stalker.h"
#include "physicobject.h"
#include "phdebug.h"

// Lain:added
#include "debug_text_tree.h"
#endif

// extern BOOL	g_bDebugDumpPhysicsStep;
extern CUISequencer* g_tutorial;
extern CUISequencer* g_tutorial2;

float g_cl_lvInterp = 0.1;
u32 lvInterpSteps = 0;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLevel::CLevel() : IPureClient(Device.GetTimerGlobal())
{
    lua_gc(ai().script_engine().lua(), LUA_GCSTOP, 0);
    lua_gc(ai().script_engine().lua(), LUA_GCSETSTEPMUL, 5);

    g_bDebugEvents = strstr(Core.Params, "-debug_ge") ? TRUE : FALSE;

    Server = nullptr;

    game = nullptr;
    game_events = xr_new<NET_Queue_Event>();

    game_configured = FALSE;
    m_bGameConfigStarted = FALSE;
    m_connect_server_err = xrServer::ErrNoError;

    eChangeRP = Engine.Event.Handler_Attach("LEVEL:ChangeRP", this);
    eDemoPlay = Engine.Event.Handler_Attach("LEVEL:PlayDEMO", this);
    eChangeTrack = Engine.Event.Handler_Attach("LEVEL:PlayMusic", this);
    eEnvironment = Engine.Event.Handler_Attach("LEVEL:Environment", this);

    eEntitySpawn = Engine.Event.Handler_Attach("LEVEL:spawn", this);

    m_pBulletManager = xr_new<CBulletManager>();

    m_map_manager = xr_new<CMapManager>();
    m_game_task_manager = xr_new<CGameTaskManager>();

    //----------------------------------------------------
    m_bNeed_CrPr = false;
    m_bIn_CrPr = false;
    m_dwNumSteps = 0;
    m_dwDeltaUpdate = u32(fixed_step * 1000);
    m_dwLastNetUpdateTime = 0;
    // VERIFY						( physics_world() );
    // physics_world()->set_step_time_callback((PhysicsStepTimeCallback*) &PhisStepsCallback);
    // physics_step_time_callback	= (PhysicsStepTimeCallback*) &PhisStepsCallback;
    m_seniority_hierarchy_holder = xr_new<CSeniorityHierarchyHolder>();

    m_level_sound_manager = xr_new<CLevelSoundManager>();
    m_space_restriction_manager = xr_new<CSpaceRestrictionManager>();
    m_client_spawn_manager = xr_new<CClientSpawnManager>();
    m_autosave_manager = xr_new<CAutosaveManager>();

    m_debug_renderer = xr_new<CDebugRenderer>();
#ifdef DEBUG
    m_level_debug = xr_new<CLevelDebug>();
    m_bEnvPaused = false;
#endif

    m_ph_commander = xr_new<CPHCommander>();
    m_ph_commander_scripts = xr_new<CPHCommanderScripts>();

#ifdef DEBUG
    m_bSynchronization = false;
#endif
    //---------------------------------------------------------
    pObjects4CrPr.clear();
    pActors4CrPr.clear();
    //---------------------------------------------------------
    pCurrentControlEntity = nullptr;

    R_ASSERT(nullptr == g_player_hud);
    g_player_hud = xr_new<player_hud>();
    g_player_hud->load_default();

    hud_zones_list = nullptr;

    Msg("%s", Core.Params);
    
    //---------------------------------------------------------
    m_is_removing_objects = false;
}

CLevel::~CLevel()
{
    xr_delete(g_player_hud);
    delete_data(hud_zones_list);
    hud_zones_list = nullptr;

    Msg("- Destroying level");

    Engine.Event.Handler_Detach(eEntitySpawn, this);

    Engine.Event.Handler_Detach(eEnvironment, this);
    Engine.Event.Handler_Detach(eChangeTrack, this);
    Engine.Event.Handler_Detach(eDemoPlay, this);
    Engine.Event.Handler_Detach(eChangeRP, this);

    if (physics_world())
    {
        destroy_physics_world();
        xr_delete(m_ph_commander_physics_worldstep);
    }

    // destroy PSs
    for (POIt p_it = m_StaticParticles.begin(); m_StaticParticles.end() != p_it; ++p_it)
        CParticlesObject::Destroy(*p_it);
    m_StaticParticles.clear();

    // Unload sounds
    // unload prefetched sounds
    sound_registry.clear();

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

    xr_delete(m_autosave_manager);

    xr_delete(m_debug_renderer);

    xr_delete(game);
    xr_delete(game_events);

    // by Dandy
    // destroy fog of war
    //	xr_delete					(m_pFogOfWar);
    // destroy bullet manager
    xr_delete(m_pBulletManager);

    //-----------------------------------------------------------
    xr_delete(m_ph_commander);
    xr_delete(m_ph_commander_scripts);
    //-----------------------------------------------------------
    pObjects4CrPr.clear();
    pActors4CrPr.clear();

    ai().unload();
    //-----------------------------------------------------------
#ifdef DEBUG
    xr_delete(m_level_debug);
#endif
    //-----------------------------------------------------------
    xr_delete(m_map_manager);
    delete_data(m_game_task_manager);
    //	xr_delete					(m_pFogOfWarMngr);

    // here we clean default trade params
    // because they should be new for each saved/loaded game
    // and I didn't find better place to put this code in
    CTradeParameters::clean();

    if (g_tutorial && g_tutorial->m_pStoredInputReceiver == this)
        g_tutorial->m_pStoredInputReceiver = nullptr;

    if (g_tutorial2 && g_tutorial2->m_pStoredInputReceiver == this)
        g_tutorial2->m_pStoredInputReceiver = nullptr;
}

shared_str CLevel::name() const { return (map_data.m_name); }

void CLevel::PrefetchSound(LPCSTR name)
{
    // preprocess sound name
    string_path tmp;
    xr_strcpy(tmp, name);
    xr_strlwr(tmp);
    if (strext(tmp))
        *strext(tmp) = 0;
    shared_str snd_name = tmp;
    // find in registry
    SoundRegistryMapIt it = sound_registry.find(snd_name);
    // if find failed - preload sound
    if (it == sound_registry.end())
        sound_registry[snd_name].create(snd_name.c_str(), st_Effect, sg_SourceType);
}

// Game interface ////////////////////////////////////////////////////
int CLevel::get_RPID(LPCSTR /**name/**/)
{
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
#ifndef MASTER_GOLD
        Msg("! ERROR: c_EVENT[%d] : non-game-object", dest);
#endif // #ifndef MASTER_GOLD
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
            case M_MOVE_PLAYERS: {
                u8 Count = P.r_u8();
                for (u8 i = 0; i < Count; i++)
                {
                    u16 ID = P.r_u16();
                    Fvector NewPos, NewDir;
                    P.r_vec3(NewPos);
                    P.r_vec3(NewDir);

                    CActor* OActor = smart_cast<CActor*>(Objects.net_Find(ID));
                    if (0 == OActor)
                        break;
                    OActor->MoveActor(NewPos, NewDir);
                };

                NET_Packet PRespond;
                PRespond.w_begin(M_MOVE_PLAYERS_RESPOND);
                Send(PRespond, net_flags(TRUE, TRUE));
            }
            break;
            case M_STATISTIC_UPDATE: {
            }
            break;
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

void CLevel::MakeReconnect()
{
    if (!Engine.Event.Peek("KERNEL:disconnect"))
    {
        Engine.Event.Defer("KERNEL:disconnect");
        char const* server_options = nullptr;
        char const* client_options = nullptr;
        if (m_caServerOptions.c_str())
        {
            server_options = xr_strdup(*m_caServerOptions);
        }
        else
        {
            server_options = xr_strdup("");
        }
        if (m_caClientOptions.c_str())
        {
            client_options = xr_strdup(*m_caClientOptions);
        }
        else
        {
            client_options = xr_strdup("");
        }
        Engine.Event.Defer("KERNEL:start", size_t(server_options), size_t(client_options));
    }
}

void CLevel::OnFrame()
{
#ifdef DEBUG
    DBG_RenderUpdate();
#endif // #ifdef DEBUG

    Fvector temp_vector;
    m_feel_deny.feel_touch_update(temp_vector, 0.f);

    //psDeviceFlags.set(rsDisableObjectsAsCrows, false);

    // commit events from bullet manager from prev-frame
    BulletManager().CommitEvents();

    // Client receive
    if (net_isDisconnected())
    {
        Engine.Event.Defer("kernel:disconnect");
        return;
    }
    else
    {
        ClientReceive();
    }

    ProcessGameEvents();

    if (m_bNeed_CrPr)
        make_NetCorrectionPrediction();

    if (g_mt_config.test(mtMap))
        Device.add_to_seq_parallel(fastdelegate::MakeDelegate(m_map_manager, &CMapManager::Update));
    else
        MapManager().Update();

    if (Device.dwPrecacheFrame == 0)
    {
        if (g_mt_config.test(mtMap))
            Device.add_to_seq_parallel(fastdelegate::MakeDelegate(m_game_task_manager, &CGameTaskManager::UpdateTasks));
        else
            GameTaskManager().UpdateTasks();
    }

    // Inherited update
    inherited::OnFrame();

#ifdef DEBUG
    g_pGamePersistent->Environment().m_paused = m_bEnvPaused;
#endif
    extern bool editor_override_time;

    if (!editor_override_time)
    {
        g_pGamePersistent->Environment().SetGameTime(GetEnvironmentGameDayTimeSec(), game->GetEnvironmentGameTimeFactor());
    }

    m_ph_commander->update();
    m_ph_commander_scripts->update();

    BulletManager().CommitRenderSet();

    // update static sounds
    if (g_mt_config.test(mtLevelSounds))
        Device.add_to_seq_parallel(fastdelegate::MakeDelegate(m_level_sound_manager, &CLevelSoundManager::Update));
    else
        m_level_sound_manager->Update();

    if (ps_lua_gc_method != gc_timeout)
        Device.add_to_seq_parallel(fastdelegate::MakeDelegate(this, &CLevel::script_gc));

    CImGuiEditor& editor = CImGuiEditor::Get();
    editor.ShowEditor();
}

// Immediately stop the current GC iteration. Can be used to avoid unintentional CPU load,
// since we anyway have one full script_gc() above each frame
//void CLevel::stop_gc() { lua_gc(ai().script_engine().lua(), LUA_GCSTOP, 0); }

#ifdef DEBUG
extern Flags32 dbg_net_Draw_Flags;
#endif

extern void draw_wnds_rects();

extern bool use_reshade;
extern void render_reshade_effects();

void CLevel::OnRender()
{
    inherited::OnRender();

    if (!game)
        return;

    Render->Calculate();
    Render->Render();

    ::Render->ui_is_rendering = true;

    BulletManager().Render();

    if (use_reshade)
        render_reshade_effects();

    HUD().RenderUI();

#ifndef DEBUG

    if (psActorFlags.test(AF_ZONES_DBG))
    {
        for (u32 I = 0; I < Level().Objects.o_count(); I++)
        {
            auto _O = Level().Objects.o_get_by_iterator(I);
            auto space_restrictor = smart_cast<CSpaceRestrictor*>(_O);
            if (space_restrictor)
                space_restrictor->OnRender();
        }
    }

    if (psActorFlags.test(AF_VERTEX_DBG))
    {
        if (ai().get_level_graph())
            ai().level_graph().render();
    }

#else
    draw_wnds_rects();
    physics_world()->OnRender();

    if (ai().get_level_graph())
        ai().level_graph().render();

    CAI_Stalker* stalker = smart_cast<CAI_Stalker*>(Level().CurrentEntity());
    if (stalker)
        stalker->OnRender();

    if (bDebug)
    {
        for (u32 I = 0; I < Level().Objects.o_count(); I++)
        {
            CObject* _O = Level().Objects.o_get_by_iterator(I);

            CAI_Stalker* stalker = smart_cast<CAI_Stalker*>(_O);
            if (stalker)
                stalker->OnRender();

            CCustomMonster* monster = smart_cast<CCustomMonster*>(_O);
            if (monster)
                monster->OnRender();

            CPhysicObject* physic_object = smart_cast<CPhysicObject*>(_O);
            if (physic_object)
                physic_object->OnRender();

            CSpaceRestrictor* space_restrictor = smart_cast<CSpaceRestrictor*>(_O);
            if (space_restrictor)
                space_restrictor->OnRender();
            CClimableObject* climable = smart_cast<CClimableObject*>(_O);
            if (climable)
                climable->OnRender();
            CTeamBaseZone* team_base_zone = smart_cast<CTeamBaseZone*>(_O);
            if (team_base_zone)
                team_base_zone->OnRender();

            if (dbg_net_Draw_Flags.test(dbg_draw_skeleton)) // draw skeleton
            {
                CGameObject* pGO = smart_cast<CGameObject*>(_O);
                if (pGO && pGO != Level().CurrentViewEntity() && !pGO->H_Parent())
                {
                    if (pGO->Position().distance_to_sqr(Device.vCameraPosition) < 400.0f)
                    {
                        pGO->dbg_DrawSkeleton();
                    }
                }
            };
        }
        //  [7/5/2005]
        if (Server && Server->game)
            Server->game->OnRender();
        //  [7/5/2005]
        ObjectSpace.dbgRender();

        //---------------------------------------------------------------------
        UI().Font().pFontStat->OutSet(170, 630);
        UI().Font().pFontStat->SetHeight(16.0f);
        UI().Font().pFontStat->SetColor(0xffff0000);

        UI().Font().pFontStat->OutNext("Server Objects:      [%d]", Objects.o_count());
        UI().Font().pFontStat->OutNext("Interpolation Steps: [%d]", Level().GetInterpolationSteps());
        UI().Font().pFontStat->SetHeight(8.0f);
        //---------------------------------------------------------------------
    }

    if (bDebug)
    {
        DBG().draw_object_info();
        DBG().draw_text();
        DBG().draw_level_info();
    }
#endif

    debug_renderer().render();

#ifdef DEBUG
    DBG().draw_debug_text();

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

    ::Render->ui_is_rendering = false;
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
    {
    }
    else if (E == eDemoPlay && P1)
    {
        char* name = (char*)P1;
        string_path RealName;
        xr_strcpy(RealName, name);
        xr_strcat(RealName, ".xrdemo");
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

void CLevel::AddObject_To_Objects4CrPr(CGameObject* pObj)
{
    if (!pObj)
        return;
    for (OBJECTS_LIST_it OIt = pObjects4CrPr.begin(); OIt != pObjects4CrPr.end(); OIt++)
    {
        if (*OIt == pObj)
            return;
    }
    pObjects4CrPr.push_back(pObj);
}
void CLevel::AddActor_To_Actors4CrPr(CGameObject* pActor)
{
    if (!pActor)
        return;
    if (!smart_cast<CActor*>(pActor))
        return;
    for (OBJECTS_LIST_it AIt = pActors4CrPr.begin(); AIt != pActors4CrPr.end(); AIt++)
    {
        if (*AIt == pActor)
            return;
    }
    pActors4CrPr.push_back(pActor);
}

void CLevel::RemoveObject_From_4CrPr(CGameObject* pObj)
{
    if (!pObj)
        return;

    OBJECTS_LIST_it OIt = std::find(pObjects4CrPr.begin(), pObjects4CrPr.end(), pObj);
    if (OIt != pObjects4CrPr.end())
    {
        pObjects4CrPr.erase(OIt);
    }

    OBJECTS_LIST_it AIt = std::find(pActors4CrPr.begin(), pActors4CrPr.end(), pObj);
    if (AIt != pActors4CrPr.end())
    {
        pActors4CrPr.erase(AIt);
    }
}

void CLevel::make_NetCorrectionPrediction()
{
    m_bNeed_CrPr = false;
    m_bIn_CrPr = true;
    u64 NumPhSteps = physics_world()->StepsNum();
    physics_world()->StepsNum() -= m_dwNumSteps;
    if (ph_console::g_bDebugDumpPhysicsStep && m_dwNumSteps > 10)
    {
        Msg("!!!TOO MANY PHYSICS STEPS FOR CORRECTION PREDICTION = %d !!!", m_dwNumSteps);
        m_dwNumSteps = 10;
    };
    //////////////////////////////////////////////////////////////////////////////////
    physics_world()->Freeze();

    // setting UpdateData and determining number of PH steps from last received update
    for (OBJECTS_LIST_it OIt = pObjects4CrPr.begin(); OIt != pObjects4CrPr.end(); OIt++)
    {
        CGameObject* pObj = *OIt;
        if (!pObj)
            continue;
        pObj->PH_B_CrPr();
    };
    //////////////////////////////////////////////////////////////////////////////////
    // first prediction from "delivered" to "real current" position
    // making enought PH steps to calculate current objects position based on their updated state

    for (u32 i = 0; i < m_dwNumSteps; i++)
    {
        physics_world()->Step();

        for (OBJECTS_LIST_it AIt = pActors4CrPr.begin(); AIt != pActors4CrPr.end(); AIt++)
        {
            CGameObject* pActor = *AIt;
            if (!pActor || pActor->CrPr_IsActivated())
                continue;
            pActor->PH_B_CrPr();
        };
    };
    //////////////////////////////////////////////////////////////////////////////////
    for (OBJECTS_LIST_it OIt = pObjects4CrPr.begin(); OIt != pObjects4CrPr.end(); OIt++)
    {
        CGameObject* pObj = *OIt;
        if (!pObj)
            continue;
        pObj->PH_I_CrPr();
    };
    //////////////////////////////////////////////////////////////////////////////////
    if (!InterpolationDisabled())
    {
        for (u32 i = 0; i < lvInterpSteps; i++) // second prediction "real current" to "future" position
        {
            physics_world()->Step();
#ifdef DEBUG
/*
            for	(OBJECTS_LIST_it OIt = pObjects4CrPr.begin(); OIt != pObjects4CrPr.end(); OIt++)
            {
                CGameObject* pObj = *OIt;
                if (!pObj) continue;
                pObj->PH_Ch_CrPr();
            };
*/
#endif
        }
        //////////////////////////////////////////////////////////////////////////////////
        for (OBJECTS_LIST_it OIt = pObjects4CrPr.begin(); OIt != pObjects4CrPr.end(); OIt++)
        {
            CGameObject* pObj = *OIt;
            if (!pObj)
                continue;
            pObj->PH_A_CrPr();
        };
    };
    physics_world()->UnFreeze();

    physics_world()->StepsNum() = NumPhSteps;
    m_dwNumSteps = 0;
    m_bIn_CrPr = false;

    pObjects4CrPr.clear();
    pActors4CrPr.clear();
};

u32 CLevel::GetInterpolationSteps() { return lvInterpSteps; };

void CLevel::UpdateDeltaUpd(u32 LastTime)
{
    u32 CurrentDelta = LastTime - m_dwLastNetUpdateTime;
    if (CurrentDelta < m_dwDeltaUpdate)
        CurrentDelta = iFloor(float(m_dwDeltaUpdate * 10 + CurrentDelta) / 11);

    m_dwLastNetUpdateTime = LastTime;
    m_dwDeltaUpdate = CurrentDelta;

    if (0 == g_cl_lvInterp)
        ReculcInterpolationSteps();
    else if (g_cl_lvInterp > 0)
    {
        lvInterpSteps = iCeil(g_cl_lvInterp / fixed_step);
    }
};

void CLevel::ReculcInterpolationSteps()
{
    lvInterpSteps = iFloor(float(m_dwDeltaUpdate) / (fixed_step * 1000));
    if (lvInterpSteps > 60)
        lvInterpSteps = 60;
    if (lvInterpSteps < 3)
        lvInterpSteps = 3;
};

bool CLevel::InterpolationDisabled() { return g_cl_lvInterp < 0; };

void CLevel::PhisStepsCallback(u32 Time0, u32 Time1)
{
    if (!Level().game)
        return;
    if (GameID() == eGameIDSingle)
        return;

    // #pragma todo("Oles to all: highly inefficient and slow!!!")
    // fixed (Andy)
    /*
    for (xr_vector<CObject*>::iterator O=Level().Objects.objects.begin(); O!=Level().Objects.objects.end(); ++O)
    {
        if( smart_cast<CActor*>((*O)){
            CActor* pActor = smart_cast<CActor*>(*O);
            if (!pActor || pActor->Remote()) continue;
                pActor->UpdatePosStack(Time0, Time1);
        }
    };
    */
};

void CLevel::SetNumCrSteps(u32 NumSteps)
{
    m_bNeed_CrPr = true;
    if (m_dwNumSteps > NumSteps)
        return;
    m_dwNumSteps = NumSteps;
    if (m_dwNumSteps > 1000000)
    {
        VERIFY(0);
    }
};

ALife::_TIME_ID CLevel::GetStartGameTime() { return (game->GetStartGameTime()); }

ALife::_TIME_ID CLevel::GetGameTime() { return (game->GetGameTime()); }

void CLevel::GetGameTimeForShaders(u32& hours, u32& mins, u32& secs, u32& milisecs)
{
    u32 unused;
    split_time(GetGameTime(), unused, unused, unused, hours, mins, secs, milisecs);
}

ALife::_TIME_ID CLevel::GetEnvironmentGameTime() { return (game->GetEnvironmentGameTime()); }

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

void CLevel::GetGameDateTime(u32& year, u32& month, u32& day, u32& hours, u32& mins, u32& secs, u32& milisecs)
{
    split_time(GetGameTime(), year, month, day, hours, mins, secs, milisecs);
}

float CLevel::GetEnvironmentTimeFactor() const
{
    if (!game)
        return 0.0f;
    return game->GetEnvironmentGameTimeFactor();
}

float CLevel::GetGameTimeFactor() { return (game->GetGameTimeFactor()); }

u64 CLevel::GetEnvironmentGameTime() const
{
    if (!game)
        return 0;
    return game->GetEnvironmentGameTime();
}

void CLevel::SetGameTimeFactor(const float fTimeFactor) { game->SetGameTimeFactor(fTimeFactor); }

void CLevel::SetGameTimeFactor(ALife::_TIME_ID GameTime, const float fTimeFactor) { game->SetGameTimeFactor(GameTime, fTimeFactor); }

void CLevel::OnAlifeSimulatorUnLoaded()
{
    MapManager().ResetStorage();
    GameTaskManager().ResetStorage();
}

void CLevel::OnAlifeSimulatorLoaded()
{
    MapManager().ResetStorage();
    GameTaskManager().ResetStorage();
}

void CLevel::OnDestroyObject(u16 id) { m_just_destroyed.push_back(id); }

u32 GameID() { return Game().Type(); }

CZoneList* CLevel::create_hud_zones_list()
{
    hud_zones_list = xr_new<CZoneList>();
    hud_zones_list->clear();
    return hud_zones_list;
}

ICF static BOOL GetPickDist_Callback(collide::rq_result& result, LPVOID params)
{
    collide::rq_result* RQ = (collide::rq_result*)params;
    if (result.O)
    {
        if (Actor())
        {
            if (result.O == Actor())
                return TRUE;
            if (Actor()->Holder())
            {
                CCar* car = smart_cast<CCar*>(Actor()->Holder());
                if (car && result.O == car)
                    return TRUE;
            }
        }
    }
    else
    {
        CDB::TRI* T = Level().ObjectSpace.GetStaticTris() + result.element;
        SGameMtl* pMtl = GMLib.GetMaterialByIdx(T->material);
        if (pMtl && (pMtl->Flags.is(SGameMtl::flPassable) || pMtl->Flags.is(SGameMtl::flActorObstacle)))
            return TRUE;
    }
    *RQ = result;
    return FALSE;
}

collide::rq_result CLevel::GetPickResult(Fvector pos, Fvector dir, float range, CObject* ignore)
{
    collide::rq_result RQ;
    RQ.set(nullptr, range, -1);
    collide::rq_results RQR;
    collide::ray_defs RD(pos, dir, RQ.range, CDB::OPT_FULL_TEST, collide::rqtBoth);
    Level().ObjectSpace.RayQuery(RQR, RD, GetPickDist_Callback, &RQ, nullptr, ignore);
    return RQ;
}

void CLevel::script_gc() const
{
    ZoneScoped;

    // int memory_before_kb = lua_gc(ai().script_engine().lua(), LUA_GCCOUNT, 0);

    switch (ps_lua_gc_method)
    {
    case gc_step: {
        // делает один шаг очистки памяти в lua. там у него свое понимание сколько надо очистить
        lua_gc(ai().script_engine().lua(), LUA_GCSTEP, psLUA_GCSTEP);

        lua_gc(ai().script_engine().lua(), LUA_GCSTOP, 0);
        break;
    }
    case gc_timeout: {
        // чистит столько раз пока не вышел timeout. это типо лучше, НО иногда бывает что какой то шаг очистки может занять сильно больше времени
        lua_gc(ai().script_engine().lua(), LUA_GCTIMEOUT, psLUA_GCTIMEOUT);

        lua_gc(ai().script_engine().lua(), LUA_GCSTOP, 0);
        break;
    }
    case gc_default: {
        // луаджит чистит себе память сам когда захочет.
        lua_gc(ai().script_engine().lua(), LUA_GCRESTART, 0);
        break;
    }
    }

    // int memory_after_kb = lua_gc(ai().script_engine().lua(), LUA_GCCOUNT, 0);

    // Msg("##[%s] script_gc [timeout: %d]: before [%d]kb after [%d]kb", __FUNCTION__, psLUA_GCTIMEOUT, memory_before_kb, memory_after_kb);
}

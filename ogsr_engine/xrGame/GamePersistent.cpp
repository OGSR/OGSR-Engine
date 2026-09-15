#include "stdafx.h"
#include "GamePersistent.h"
#include "../xr_3da/Fmesh.h"
#include "../xr_3da/XR_IOConsole.h"
#include "../xr_3da/GameMtlLib.h"
#include "../Include/xrRender/Kinematics.h"
#include "profiler.h"
#include "MainMenu.h"
#include "UICursor.h"
#include "game_base_space.h"
#include "Level.h"
#include "ParticlesObject.h"
#include "game_base_space.h"
#include "stalker_animation_data_storage.h"
#include "stalker_velocity_holder.h"

#include "ActorEffector.h"
#include "Actor.h"
#include "Spectator.h"

#include "ui/UITextureMaster.h"

#include "ai_space.h"
#include "../xrServerEntities/script_engine.h"

#include "holder_custom.h"
#include "game_cl_base.h"
#include "xrserver_objects_alife_monsters.h"
#include "../xrServerEntities/xrServer_Object_Base.h"
#include "ui/UIGameTutorial.h"
#include "string_table.h"
#include "../xr_3da/x_ray.h"
#include "ui/UILoadingScreen.h"
#include "../xr_3da/DiscordRPC.hpp"
#include "ingame_editors/embedded_editor_main.h"
#include "../xr_3da/device.h"

#ifndef MASTER_GOLD
#include "custommonster.h"
#endif // MASTER_GOLD

#include "ai_debug.h"

CGamePersistent::CGamePersistent(void)
{
    m_game_params.m_e_game_type = eGameIDNoGame;
    ambient_effect_next_time = 0;
    ambient_effect_stop_time = 0;
    ambient_particles = 0;

    ambient_effect_wind_start = 0.f;
    ambient_effect_wind_in_time = 0.f;
    ambient_effect_wind_end = 0.f;
    ambient_effect_wind_out_time = 0.f;
    ambient_effect_wind_on = false;

    ZeroMemory(ambient_sound_next_time, sizeof(ambient_sound_next_time));

    m_pUI_core = nullptr;
    m_pMainMenu = nullptr;
    m_intro = nullptr;
    m_intro_event.bind(this, &CGamePersistent::start_logo_intro);
#ifdef DEBUG
    m_frame_counter = 0;
    m_last_stats_frame = u32(-2);
#endif

    BOOL bDemoMode = (0 != strstr(Core.Params, "-demomode "));
    if (bDemoMode)
    {
        string256 fname;
        LPCSTR name = strstr(Core.Params, "-demomode ") + 10;
        sscanf(name, "%s", fname);
        R_ASSERT2(fname[0], "Missing filename for 'demomode'");
        Msg("- playing in demo mode '%s'", fname);
        pDemoFile = FS.r_open(fname);
        Device.seqFrame.Add(this);
        eDemoStart = Engine.Event.Handler_Attach("GAME:demo", this);
        uTime2Change = 0;
    }
    else
    {
        pDemoFile = nullptr;
        eDemoStart = nullptr;
    }

    eQuickLoad = Engine.Event.Handler_Attach("Game:QuickLoad", this);
}

CGamePersistent::~CGamePersistent(void)
{
    FS.r_close(pDemoFile);
    Device.seqFrame.Remove(this);
    Engine.Event.Handler_Detach(eDemoStart, this);
    Engine.Event.Handler_Detach(eQuickLoad, this);
}

void CGamePersistent::PreStart(LPCSTR op)
{
    pApp->SetLoadingScreen(xr_new<UILoadingScreen>());
    __super::PreStart(op);
}

void CGamePersistent::RegisterModel(IRenderVisual* V)
{
    // Check types
    switch (V->getType())
    {
    case MT_SKELETON_ANIM:
    case MT_SKELETON_RIGID: {
        u16 def_idx = GMLib.GetMaterialIdx("default_object");
        R_ASSERT2(GMLib.GetMaterialByIdx(def_idx)->Flags.is(SGameMtl::flDynamic), "'default_object' - must be dynamic");
        IKinematics* K = smart_cast<IKinematics*>(V);
        VERIFY(K);
        int cnt = K->LL_BoneCount();
        for (u16 k = 0; k < cnt; k++)
        {
            CBoneData& bd = K->LL_GetData(k);
            if (*(bd.game_mtl_name))
            {
                bd.game_mtl_idx = GMLib.GetMaterialIdx(*bd.game_mtl_name);
                R_ASSERT2(GMLib.GetMaterialByIdx(bd.game_mtl_idx)->Flags.is(SGameMtl::flDynamic), "Required dynamic game material");
            }
            else
            {
                bd.game_mtl_idx = def_idx;
            }
        }
    }
    break;
    }
}

extern void clean_game_globals();
extern void init_game_globals();

void CGamePersistent::OnAppStart()
{
    // load game materials
    GMLib.Load();
    init_game_globals();
    __super::OnAppStart();
    m_pUI_core = xr_new<ui_core>();
    m_pMainMenu = xr_new<CMainMenu>();
}

void CGamePersistent::OnAppEnd()
{
    if (m_pMainMenu->IsActive())
        m_pMainMenu->Activate(false);

    xr_delete(m_pMainMenu);

    __super::OnAppEnd();

    clean_game_globals();

    xr_delete(m_pUI_core);

    GMLib.Unload();
}

void CGamePersistent::Start(LPCSTR op) { __super::Start(op); }

void CGamePersistent::Disconnect()
{
    // destroy ambient particles
    CParticlesObject::Destroy(ambient_particles);

    __super::Disconnect();
    // stop all played emitters
    ::Sound->stop_emitters();

    // validate and clean up spatial dbs
    g_SpatialSpace->clear();
    g_SpatialSpacePhysic->clear();

    m_game_params.m_e_game_type = eGameIDNoGame;
}

#include "xr_level_controller.h"

void CGamePersistent::OnGameStart()
{
    __super::OnGameStart();
    UpdateGameType();
}

LPCSTR GameTypeToString(EGameIDs gt, bool)
{
    switch (gt)
    {
    case eGameIDSingle: return "single";
    default: return "---";
    }
}

EGameIDs ParseStringToGameType(LPCSTR str)
{
    if (!xr_strcmp(str, "single"))
        return eGameIDSingle;
    else
        return eGameIDNoGame; // EGameIDs
}

void CGamePersistent::UpdateGameType()
{
    __super::UpdateGameType();

    m_game_params.m_e_game_type = ParseStringToGameType(m_game_params.m_game_type);
}

void CGamePersistent::OnGameEnd()
{
    __super::OnGameEnd();

    xr_delete(g_stalker_animation_data_storage);
    xr_delete(g_stalker_velocity_holder);
}

void CGamePersistent::WeathersUpdate()
{
    if (g_pGameLevel)
    {
        const bool bIndoor = g_pGamePersistent->IsActorInHideout();
        auto& Env = Environment();
        int data_set = (Random.randF() < (1.f - Env.CurrentEnv->weight)) ? 0 : 1;

        CEnvDescriptor* const current_env = Env.Current[0];
        CEnvDescriptor* const _env = Env.Current[data_set];

        if (!current_env || !_env)
            return;

        CEnvAmbient* env_amb = _env->env_ambient;
        if (env_amb)
        {
            CEnvAmbient::SSndChannelVec& vec = current_env->env_ambient->get_snd_channels();
            CEnvAmbient::SSndChannelVecIt I = vec.begin();
            CEnvAmbient::SSndChannelVecIt E = vec.end();

            for (u32 idx = 0; I != E; ++I, ++idx)
            {
                CEnvAmbient::SSndChannel& ch = **I;
                R_ASSERT(idx < 40);
                if (ambient_sound_next_time[idx] == 0) // first
                {
                    ambient_sound_next_time[idx] = Device.dwTimeGlobal + ch.get_rnd_sound_first_time();
                }
                else if (Device.dwTimeGlobal > ambient_sound_next_time[idx])
                {
                    ref_sound& snd = ch.get_rnd_sound();

                    Fvector pos;
                    float angle = ::Random.randF(PI_MUL_2);
                    pos.x = _cos(angle);
                    pos.y = 0;
                    pos.z = _sin(angle);
                    pos.normalize().mul(ch.get_rnd_sound_dist()).add(Device.vCameraPosition);
                    pos.y += 10.f;
                    snd.play_at_pos(0, pos);

#ifdef DEBUG
                    if (!snd._handle() && strstr(Core.Params, "-nosound"))
                        continue;
#endif // DEBUG

                    VERIFY(snd._handle());
                    u32 _length_ms = iFloor(snd.get_length_sec() * 1000.0f);
                    ambient_sound_next_time[idx] = Device.dwTimeGlobal + _length_ms + ch.get_rnd_sound_time();
                }
            }
            // start effect
            if (!bIndoor && !ambient_particles && Device.dwTimeGlobal > ambient_effect_next_time)
            {
                CEnvAmbient::SEffect* eff = env_amb->get_rnd_effect();
                if (eff)
                {
                    Env.wind_gust_factor = eff->wind_gust_factor;
                    ambient_effect_next_time = Device.dwTimeGlobal + env_amb->get_rnd_effect_time();
                    ambient_effect_stop_time = Device.dwTimeGlobal + eff->life_time;
                    ambient_effect_wind_start = Device.fTimeGlobal;
                    ambient_effect_wind_in_time = Device.fTimeGlobal + eff->wind_blast_in_time;
                    ambient_effect_wind_end = Device.fTimeGlobal + eff->life_time / 1000.f;
                    ambient_effect_wind_out_time = Device.fTimeGlobal + eff->life_time / 1000.f + eff->wind_blast_out_time;
                    ambient_effect_wind_on = true;

                    ambient_particles = CParticlesObject::Create(eff->particles.c_str(), FALSE, false);
                    Fvector pos;
                    pos.add(Device.vCameraPosition, eff->offset);
                    ambient_particles->play_at_pos(pos);
                    if (eff->sound._handle())
                        eff->sound.play_at_pos(0, pos);

                    Env.wind_blast_strength_start_value = Env.wind_strength_factor;
                    Env.wind_blast_strength_stop_value = eff->wind_blast_strength;

                    if (Env.wind_blast_strength_start_value == 0.f)
                    {
                        Env.wind_blast_start_time.set(0.f, eff->wind_blast_direction.x, eff->wind_blast_direction.y, eff->wind_blast_direction.z);
                    }
                    else
                    {
                        Env.wind_blast_start_time.set(0.f, Env.wind_blast_direction.x, Env.wind_blast_direction.y, Env.wind_blast_direction.z);
                    }
                    Env.wind_blast_stop_time.set(0.f, eff->wind_blast_direction.x, eff->wind_blast_direction.y, eff->wind_blast_direction.z);
                }
            }
            else if (!ambient_particles && Device.dwTimeGlobal > ambient_effect_next_time)
            {
                CEnvAmbient::SEffect* eff = env_amb->get_rnd_effect();
                if (eff)
                    ambient_effect_next_time = Device.dwTimeGlobal + env_amb->get_rnd_effect_time();
            }
        }
        if (Device.fTimeGlobal >= ambient_effect_wind_start && Device.fTimeGlobal <= ambient_effect_wind_in_time && ambient_effect_wind_on)
        {
            float delta = ambient_effect_wind_in_time - ambient_effect_wind_start;
            float t;
            if (delta != 0.f)
            {
                float cur_in = Device.fTimeGlobal - ambient_effect_wind_start;
                t = cur_in / delta;
            }
            else
            {
                t = 0.f;
            }
            Env.wind_blast_current.slerp(Env.wind_blast_start_time, Env.wind_blast_stop_time, t);

            Env.wind_blast_direction.set(Env.wind_blast_current.x, Env.wind_blast_current.y, Env.wind_blast_current.z);
            Env.wind_strength_factor =
                Env.wind_blast_strength_start_value + t * (Env.wind_blast_strength_stop_value - Env.wind_blast_strength_start_value);
        }

        // stop if time exceed or indoor
        if (bIndoor || Device.dwTimeGlobal >= ambient_effect_stop_time)
        {
            if (ambient_particles)
                ambient_particles->Stop();

            Env.wind_gust_factor = 0.f;
        }

        if (Device.fTimeGlobal >= ambient_effect_wind_end && ambient_effect_wind_on)
        {
            Env.wind_blast_strength_start_value = Env.wind_strength_factor;
            Env.wind_blast_strength_stop_value = 0.f;

            ambient_effect_wind_on = false;
        }

        if (Device.fTimeGlobal >= ambient_effect_wind_end && Device.fTimeGlobal <= ambient_effect_wind_out_time)
        {
            float delta = ambient_effect_wind_out_time - ambient_effect_wind_end;
            float t;
            if (delta != 0.f)
            {
                float cur_in = Device.fTimeGlobal - ambient_effect_wind_end;
                t = cur_in / delta;
            }
            else
            {
                t = 0.f;
            }
            Env.wind_strength_factor =
                Env.wind_blast_strength_start_value + t * (Env.wind_blast_strength_stop_value - Env.wind_blast_strength_start_value);
        }
        if (Device.fTimeGlobal > ambient_effect_wind_out_time && ambient_effect_wind_out_time != 0.f)
        {
            Env.wind_strength_factor = 0.0;
        }

        // if particles not playing - destroy
        if (ambient_particles && !ambient_particles->IsPlaying())
            CParticlesObject::Destroy(ambient_particles);
    }
}

static bool allow_intro() { return false; /*return !!strstr(Core.Params, "-intro");*/ } //Временно отключено

static bool allow_game_intro() { return !strstr(Core.Params, "-nogameintro"); }

void CGamePersistent::start_logo_intro()
{
    if (!allow_intro())
    {
        m_intro_event = nullptr;
        Console->Show();
        Console->Execute("main_menu on");
        return;
    }

    if (Device.dwPrecacheFrame == 0)
    {
        m_intro_event = nullptr;
        if (0 == xr_strlen(m_game_params.m_game_or_spawn) && g_pGameLevel == nullptr)
        {
            VERIFY(nullptr == m_intro);
            m_intro = xr_new<CUISequencer>();
            m_intro->m_on_destroy_event.bind(this, &CGamePersistent::update_logo_intro);
            m_intro->Start("intro_logo");
            Console->Hide();
        }
    }
}

void CGamePersistent::update_logo_intro()
{
    xr_delete(m_intro);
    Msg("intro_delete ::update_logo_intro");
    Console->Execute("main_menu on");
}

extern int g_keypress_on_start;
void CGamePersistent::game_loaded()
{
    if (Device.dwPrecacheFrame <= 2)
    {
        m_intro_event = nullptr;
        //DiscordRPCLevel();
        if (g_pGameLevel && g_pGameLevel->bReady && (allow_game_intro() && g_keypress_on_start) && load_screen_renderer.b_need_user_input &&
            m_game_params.m_e_game_type == eGameIDSingle)
        {
            pApp->LoadForceFinish(); // hack
            VERIFY(nullptr == m_intro);
            m_intro = xr_new<CUISequencer>();
            m_intro->m_on_destroy_event.bind(this, &CGamePersistent::update_game_loaded);
            if (!m_intro->Start("game_loaded"))
                m_intro->Destroy();
        }
    }
}

void CGamePersistent::update_game_loaded()
{
    xr_delete(m_intro);
    load_screen_renderer.stop();
    Msg("intro_delete ::update_game_loaded");
    start_game_intro();
}

void CGamePersistent::start_game_intro()
{
    if (!allow_game_intro())
    {
        return;
    }

    if (g_pGameLevel && g_pGameLevel->bReady && Device.dwPrecacheFrame <= 2)
    {
        if (0 == _stricmp(m_game_params.m_new_or_load, "new"))
        {
            VERIFY(nullptr == m_intro);
            m_intro = xr_new<CUISequencer>();
            m_intro->m_on_destroy_event.bind(this, &CGamePersistent::update_game_intro);
            m_intro->Start("intro_game");
            Msg("intro_start intro_game");
        }
    }
}

void CGamePersistent::update_game_intro()
{
    if (m_intro && (false == m_intro->IsActive()))
    {
        xr_delete(m_intro);
        Msg("intro_delete ::update_game_intro");
        m_intro_event = 0;
    }
    else if (!m_intro)
    {
        m_intro_event = 0;
    }
}

extern CUISequencer* g_tutorial;
extern CUISequencer* g_tutorial2;
extern bool RESET_SECTORS_HACK;

void CGamePersistent::OnFrame()
{
    if (Device.dwPrecacheFrame == 5 && m_intro_event.empty())
    {
        //SetLoadStageTitle();
        m_intro_event.bind(this, &CGamePersistent::game_loaded);
    }

    if (g_tutorial2)
    {
        g_tutorial2->Destroy();
        xr_delete(g_tutorial2);
    }

    if (g_tutorial && !g_tutorial->IsActive())
    {
        xr_delete(g_tutorial);
    }
    if (0 == Device.dwFrame % 200)
        CUITextureMaster::FreeCachedShaders();

#ifdef DEBUG
    ++m_frame_counter;
#endif
    if (!m_intro_event.empty())
        m_intro_event();

    if (Device.dwPrecacheFrame == 0 && load_screen_renderer.b_registered /*&& !GameAutopaused*/)
    {
        RESET_SECTORS_HACK = true;

        Discord.Update(CStringTable().translate(Level().name()).c_str(), Level().name().c_str());
    }

    if (Device.dwPrecacheFrame == 0 && !m_intro && m_intro_event.empty())
        load_screen_renderer.stop();

    if (!m_pMainMenu->IsActive())
        m_pMainMenu->DestroyInternal(false);

    if (!g_pGameLevel)
        return;
    if (!g_pGameLevel->bReady)
        return;

    if (Device.Paused())
    {
#ifndef MASTER_GOLD
        if (Level().CurrentViewEntity())
        {
            if (!g_actor || (g_actor->ID() != Level().CurrentViewEntity()->ID()))
            {
                CCustomMonster* custom_monster = smart_cast<CCustomMonster*>(Level().CurrentViewEntity());
                if (custom_monster) // can be spectator in multiplayer
                    custom_monster->UpdateCamera();
            }
            else
            {
                CCameraBase* C = nullptr;
                if (g_actor)
                {
                    if (!Actor()->Holder())
                        C = Actor()->cam_Active();
                    else
                        C = Actor()->Holder()->Camera();

                    Actor()->Cameras().UpdateFromCamera(C);
                    Actor()->Cameras().ApplyDevice(VIEWPORT_NEAR);
#ifdef DEBUG
                    if (psActorFlags.test(AF_NO_CLIP))
                    {
                        Actor()->dbg_update_cl = 0;
                        Actor()->dbg_update_shedule = 0;
                        Device.dwTimeDelta = 0;
                        Device.fTimeDelta = 0.01f;
                        Actor()->UpdateCL();
                        Actor()->shedule_Update(0);
                        Actor()->dbg_update_cl = 0;
                        Actor()->dbg_update_shedule = 0;

                        CSE_Abstract* e = Level().Server->ID_to_entity(Actor()->ID());
                        VERIFY(e);
                        CSE_ALifeCreatureActor* s_actor = smart_cast<CSE_ALifeCreatureActor*>(e);
                        VERIFY(s_actor);
                        xr_vector<u16>::iterator it = s_actor->children.begin();
                        for (; it != s_actor->children.end(); it++)
                        {
                            CObject* obj = Level().Objects.net_Find(*it);
                            if (obj && Engine.Sheduler.Registered(obj))
                            {
                                obj->dbg_update_shedule = 0;
                                obj->dbg_update_cl = 0;
                                obj->shedule_Update(0);
                                obj->UpdateCL();
                                obj->dbg_update_shedule = 0;
                                obj->dbg_update_cl = 0;
                            }
                        }
                    }
#endif // DEBUG
                }
            }
        }
#else // MASTER_GOLD
        if (g_actor)
        {
            CCameraBase* C = nullptr;
            if (!Actor()->Holder())
                C = Actor()->cam_Active();
            else
                C = Actor()->Holder()->Camera();

            Actor()->Cameras().UpdateFromCamera(C);
            Actor()->Cameras().ApplyDevice();
        }
#endif // MASTER_GOLD
    }
    __super::OnFrame();

    if (!Device.Paused())
        Engine.Sheduler.Update();

    // update weathers ambient
    if (!Device.Paused())
        WeathersUpdate();

    if (0 != pDemoFile)
    {
        if (Device.dwTimeGlobal > uTime2Change)
        {
            // Change level + play demo
            if (pDemoFile->elapsed() < 3)
                pDemoFile->seek(0); // cycle

            // Read params
            string512 params;
            pDemoFile->r_string(params, sizeof(params));
            string256 o_server, o_client, o_demo;
            u32 o_time;
            sscanf(params, "%[^,],%[^,],%[^,],%d", o_server, o_client, o_demo, &o_time);

            // Start _new level + demo
            Engine.Event.Defer("KERNEL:disconnect");
            Engine.Event.Defer("KERNEL:start", size_t(xr_strdup(_Trim(o_server))), size_t(xr_strdup(_Trim(o_client))));
            Engine.Event.Defer("GAME:demo", size_t(xr_strdup(_Trim(o_demo))), u64(o_time));
            uTime2Change = 0xffffffff; // Block changer until Event received
        }
    }
}

#include "game_sv_single.h"
#include "xrServer.h"
#include "UIGameCustom.h"
#include "ui/UIMainIngameWnd.h"
#include "ui/UIPdaWnd.h"

void CGamePersistent::OnEvent(EVENT E, u64 P1, u64 P2)
{
    if (E == eQuickLoad)
    {
        if (Device.Paused())
            Device.Pause(FALSE, TRUE, TRUE, "eQuickLoad");

        if (CurrentGameUI())
        {
            CurrentGameUI()->HideShownDialogs();
            CurrentGameUI()->UIMainIngameWnd->reset_ui();
            CurrentGameUI()->PdaMenu().Reset();
        }

        if (g_tutorial)
            g_tutorial->Stop();

        if (g_tutorial2)
            g_tutorial2->Stop();

        LPSTR saved_name = (LPSTR)(P1);

        Level().remove_objects();
        game_sv_Single* game = smart_cast<game_sv_Single*>(Level().Server->game);
        R_ASSERT(game);
        game->restart_simulator(saved_name);
        xr_free(saved_name);
        return;
    }
    else if (E == eDemoStart)
    {
        string256 cmd;
        LPCSTR demo = LPCSTR(P1);
        xr_sprintf(cmd, "demo_play %s", demo);
        Console->Execute(cmd);
        xr_free(demo);
        uTime2Change = Device.TimerAsync() + u32(P2) * 1000;
    }
}

void CGamePersistent::Statistics(CGameFont* F)
{
}

float CGamePersistent::MtlTransparent(u32 mtl_idx) { return GMLib.GetMaterialByIdx((u16)mtl_idx)->fVisTransparencyFactor; }
static BOOL bRestorePause = FALSE;
static BOOL bEntryFlag = TRUE;

void CGamePersistent::OnAppActivate()
{
    Device.Pause(FALSE, !bRestorePause, TRUE, "CGP::OnAppActivate");

    bEntryFlag = TRUE;
}

void CGamePersistent::OnAppDeactivate()
{
    if (!bEntryFlag)
        return;

    bRestorePause = FALSE;

    bRestorePause = Device.Paused();
    Device.Pause(TRUE, TRUE, TRUE, "CGP::OnAppDeactivate");

    bEntryFlag = FALSE;
}

bool CGamePersistent::OnRenderPPUI_query()
{
    return MainMenu()->OnRenderPPUI_query();
    // enable PP or not
}

extern void draw_wnds_rects();
void CGamePersistent::OnRenderPPUI_main()
{
    // always
    MainMenu()->OnRenderPPUI_main();
    draw_wnds_rects();
}

void CGamePersistent::OnRenderPPUI_PP() { MainMenu()->OnRenderPPUI_PP(); }

void CGamePersistent::LoadTitle(const char* str)
{
    const char* tittle = CStringTable().translate(str).c_str();
    pApp->SetLoadStageTitle(tittle);
    pApp->LoadStage();

    Discord.Update(tittle);
}

void CGamePersistent::SetTip() { pApp->LoadTitleInt(); }

#pragma todo("Оставил на всякий случай старый код под комментами. Думал после переделки на огср-стайл лоадскрин, он вообще перестанет работать, но оказалось что отлично работает, даже 100 советов работают")

/*
void CGamePersistent::SetLoadStageTitle(const char* ls_title)
{
    string256 buff;
    if (ls_title)
    {
        xr_sprintf(buff, "%s%s", CStringTable().translate(ls_title).c_str(), "...");
        pApp->SetLoadStageTitle(buff);
    }
    else
        pApp->SetLoadStageTitle("");
}


void CGamePersistent::LoadTitle(bool change_tip, shared_str map_name)
{
    pApp->LoadStage();
    if (change_tip)
    {
        string512 buff;
        u8 tip_num;
        luabind::functor<u8> m_functor;
        R_ASSERT(ai().script_engine().functor("loadscreen.get_tip_number", m_functor));
        tip_num = m_functor(map_name.c_str());

        xr_sprintf(buff, "%s%d:", CStringTable().translate("ls_tip_number").c_str(), tip_num);
        shared_str tmp = buff;
        xr_sprintf(buff, "ls_tip_%d", tip_num);

        pApp->LoadTitleInt(CStringTable().translate("ls_header").c_str(), tmp.c_str(), CStringTable().translate(buff).c_str());
    }
}
*/
void CGamePersistent::OnSectorChanged(IRender_Sector::sector_id_t sector)
{
    if (CurrentGameUI())
        CurrentGameUI()->UIMainIngameWnd->OnSectorChanged(sector);
}

void CGamePersistent::OnAssetsChanged()
{
    IGame_Persistent::OnAssetsChanged();
    CStringTable().rescan();
}

/*
void CGamePersistent::DiscordRPCLevel() const
{
    if (g_pGameLevel != nullptr)
    {
        static CStringTable strTable;

        // Get level name
        xr_string levelName = strTable.translate("st_discord_level").c_str();
        xr_string levelIcon = Level().name().c_str();

        levelIcon += "_";
        levelIcon += std::to_string(Random.randI(1, 5)).c_str();

        levelName += '\t';
        levelName += strTable.translate(Level().name().c_str()).c_str();

        Discord.setStatus(levelName);
        Discord.setIcon(levelIcon);
    }
}
*/
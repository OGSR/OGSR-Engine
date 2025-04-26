#include "stdafx.h"
#include "hudmanager.h"
#include "game_cl_base.h"
#include "level.h"
#include "GamePersistent.h"
#include "UIGameCustom.h"
#include "script_engine.h"
#include "xr_Level_controller.h"
#include "ui/UIMainIngameWnd.h"
#include "UI/UIGameTutorial.h"
#include "UI/UIMessagesWindow.h"
#include "string_table.h"

game_cl_GameState::game_cl_GameState()
{
    local_player = 0;
    m_game_type_name = 0;

    shedule.t_min = 5;
    shedule.t_max = 20;
    shedule_register();

    m_u16VotingEnabled = 0;
    m_bServerControlHits = true;
    m_need_to_update = true;
}

game_cl_GameState::~game_cl_GameState()
{
    PLAYERS_MAP_IT I = players.begin();
    for (; I != players.end(); ++I)
        xr_delete(I->second);
    players.clear();

    shedule_unregister(true);
}

void game_cl_GameState::net_import_GameTime(NET_Packet& P)
{
    if (!m_need_to_update)
    {
        m_need_to_update = true;
        return;
    }
    // time
    u64 GameTime;
    P.r_u64(GameTime);
    float TimeFactor;
    P.r_float(TimeFactor);

    Level().SetGameTimeFactor(GameTime, TimeFactor);

    u64 GameEnvironmentTime;
    P.r_u64(GameEnvironmentTime);
    float EnvironmentTimeFactor;
    P.r_float(EnvironmentTimeFactor);
}

void game_cl_GameState::net_import_state(NET_Packet& P)
{
    // Generic
    P.r_clientID(local_svdpnid);
    P.r_s32(m_type);

    u16 ph;
    P.r_u16(ph);

    if (Phase() != ph)
        switch_Phase(ph);

    P.r_s32(m_round);
    P.r_u32(m_start_time);
    m_u16VotingEnabled = u16(P.r_u8());
    m_bServerControlHits = !!P.r_u8();
    P.r_u8();

    // Players
    u16 p_count;
    P.r_u16(p_count);

    PLAYERS_MAP players_new;

    /*
        players.clear	();
    */
    PLAYERS_MAP_IT I;
    for (u16 p_it = 0; p_it < p_count; ++p_it)
    {
        ClientID ID;
        P.r_clientID(ID);

        game_PlayerState* IP;
        I = players.find(ID);
        if (I != players.end())
        {
            IP = I->second;
            IP->net_Import(P);
            // Msg("!![%s] Called net_import - 1", __FUNCTION__);
            players_new.insert(mk_pair(ID, IP));
            players.erase(I);
        }
        else
        {
            IP = createPlayerState();
            IP->net_Import(P);
            // Msg("!![%s] Called net_import - 2", __FUNCTION__);
            players_new.insert(mk_pair(ID, IP));
        }
        if (IP->testFlag(GAME_PLAYER_FLAG_LOCAL))
            local_player = IP;
    }

    I = players.begin();
    for (; I != players.end(); ++I)
        xr_delete(I->second);
    players.clear();

    players = players_new;

    net_import_GameTime(P);
}

void game_cl_GameState::net_import_update(NET_Packet& P)
{
    // Read
    ClientID ID;
    P.r_clientID(ID);

    // Update
    PLAYERS_MAP_IT I = players.find(ID);
    if (players.end() != I)
    {
        game_PlayerState* IP = I->second;
        IP->net_Import(P);
        // Msg("!![%s] Called net_import - 3", __FUNCTION__);
    }
    else
    {
        game_PlayerState* PS = createPlayerState();
        PS->net_Import(P);
        // Msg("!![%s] Called net_import - 4", __FUNCTION__);
        xr_delete(PS);
    }

    // Syncronize GameTime
    net_import_GameTime(P);
}

game_PlayerState* game_cl_GameState::GetPlayerByGameID(u32 GameID)
{
    PLAYERS_MAP_IT I = players.begin();
    PLAYERS_MAP_IT E = players.end();

    for (; I != E; ++I)
    {
        game_PlayerState* P = I->second;
        if (P->GameID == GameID)
            return P;
    };
    return NULL;
};

game_PlayerState* game_cl_GameState::GetPlayerByOrderID(u32 idx)
{
    PLAYERS_MAP_IT I = players.begin();
    std::advance(I, idx);
    game_PlayerState* ps = I->second;
    return ps;
}

ClientID game_cl_GameState::GetClientIDByOrderID(u32 idx)
{
    PLAYERS_MAP_IT I = players.begin();
    std::advance(I, idx);
    return I->first;
}

float game_cl_GameState::shedule_Scale() { return 1.0f; }

void game_cl_GameState::shedule_Update(u32 dt) { ISheduled::shedule_Update(dt); }

void game_cl_GameState::StartStopMenu(CUIDialogWnd* pDialog, bool bDoHideIndicators) { HUD().GetUI()->StartStopMenu(pDialog, bDoHideIndicators); }

void game_cl_GameState::sv_EventSend(NET_Packet& P) { Level().Send(P, net_flags(TRUE, TRUE)); }

bool game_cl_GameState::IR_OnKeyboardPress(int dik)
{
    if (local_player && !local_player->IsSkip())
        return OnKeyboardPress(get_binded_action(dik));
    else
        return false;
}

bool game_cl_GameState::IR_OnKeyboardRelease(int dik)
{
    if (local_player && !local_player->IsSkip())
        return OnKeyboardRelease(get_binded_action(dik));
    else
        return false;
}

bool game_cl_GameState::IR_OnMouseMove(int dx, int dy) { return false; }
bool game_cl_GameState::IR_OnMouseWheel(int direction) { return false; }

void game_cl_GameState::u_EventGen(NET_Packet& P, u16 type, u16 dest)
{
    P.w_begin(M_EVENT);
    P.w_u32(Level().timeServer());
    P.w_u16(type);
    P.w_u16(dest);
}

void game_cl_GameState::u_EventSend(NET_Packet& P) { Level().Send(P, net_flags(TRUE, TRUE)); }

void game_cl_GameState::OnSwitchPhase(u32 old_phase, u32 new_phase)
{
    switch (old_phase)
    {
    case GAME_PHASE_INPROGRESS: {
    }
    break;
    default: {
    }
    break;
    };

    switch (new_phase)
    {
    case GAME_PHASE_INPROGRESS: {
    }
    break;
    default: {
    }
    break;
    }
}

void game_cl_GameState::SendPickUpEvent(u16 ID_who, u16 ID_what)
{
    NET_Packet P;
    u_EventGen(P, GE_OWNERSHIP_TAKE, ID_who);
    P.w_u16(ID_what);
    u_EventSend(P);
};

void game_cl_GameState::set_type_name(LPCSTR s)
{
    m_game_type_name = s;
}

void game_cl_GameState::reset_ui()
{
    auto ui = HUD().GetUI();

    auto uigame = ui->UIGame();

    uigame->reset_ui();

    auto mgw = ui->UIMainIngameWnd;

    mgw->reset_ui();

    if (auto MIR = ui->MainInputReceiver())
        ui->StartStopMenu(MIR, true);
}

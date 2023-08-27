#include "stdafx.h"
#include "xrServer.h"
#include "LevelGameDef.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "script_engine.h"
#include "level.h"
#include "xrserver.h"
#include "ai_space.h"
#include "game_sv_event_queue.h"
#include "..\xr_3da\XR_IOConsole.h"
#include "..\xr_3da\xr_ioc_cmd.h"
#include "string_table.h"

BOOL net_sv_control_hit = FALSE;

game_PlayerState* game_sv_GameState::get_it(u32 it)
{
    xrClientData* C = (xrClientData*)m_server->client_Get(it);
    if (0 == C)
        return 0;
    else
        return C->ps;
}

game_PlayerState* game_sv_GameState::get_id(ClientID id)
{
    xrClientData* C = (xrClientData*)m_server->ID_to_client(id);
    if (0 == C)
        return NULL;
    else
        return C->ps;
}

ClientID game_sv_GameState::get_it_2_id(u32 it)
{
    xrClientData* C = (xrClientData*)m_server->client_Get(it);
    if (0 == C)
    {
        ClientID clientID;
        clientID.set(0);
        return clientID;
    }
    else
        return C->ID;
}

u32 game_sv_GameState::get_players_count() { return m_server->client_Count(); }

u16 game_sv_GameState::get_id_2_eid(ClientID id)
{
    xrClientData* C = (xrClientData*)m_server->ID_to_client(id);
    if (0 == C)
        return 0xffff;
    CSE_Abstract* E = C->owner;
    if (0 == E)
        return 0xffff;
    return E->ID;
}

void* game_sv_GameState::get_client(u16 id) // if exist
{
    CSE_Abstract* entity = get_entity_from_eid(id);
    if (entity && entity->owner && entity->owner->ps && entity->owner->ps->GameID == id)
        return entity->owner;
    //-------------------------------------------------
    u32 cnt = get_players_count();
    for (u32 it = 0; it < cnt; ++it)
    {
        xrClientData* C = (xrClientData*)m_server->client_Get(it);
        if (!C || !C->ps)
            continue;
        //		game_PlayerState*	ps	=	get_it	(it);
        if (C->ps->HasOldID(id))
            return C;
    };
    //-------------------------------------------------
    return NULL;
}

CSE_Abstract* game_sv_GameState::get_entity_from_eid(u16 id) { return m_server->ID_to_entity(id); }

void game_sv_GameState::signal_Syncronize() { sv_force_sync = TRUE; }

// Network
void game_sv_GameState::net_Export_State(NET_Packet& P, ClientID to)
{
    // Generic
    P.w_clientID(to);
    P.w_s32(m_type);
    P.w_u16(m_phase);
    P.w_s32(m_round);
    P.w_u32(m_start_time);
    P.w_u8(u8(0 & 0xff));
    P.w_u8(u8(net_sv_control_hit));
    P.w_u8(u8(0));

    // Players
    u32 p_count = 0;
    for (u32 p_it = 0; p_it < get_players_count(); ++p_it)
    {
        xrClientData* C = (xrClientData*)m_server->client_Get(p_it);
        if (!C->net_Ready || (C->ps->IsSkip() && C->ID != to))
            continue;
        p_count++;
    };

    P.w_u16(u16(p_count));
    game_PlayerState* Base = get_id(to);
    for (u32 p_it = 0; p_it < get_players_count(); ++p_it)
    {
        string64 p_name;
        xrClientData* C = (xrClientData*)m_server->client_Get(p_it);
        game_PlayerState* A = get_it(p_it);
        if (!C->net_Ready || (A->IsSkip() && C->ID != to))
            continue;

        CSE_Abstract* C_e = C->owner;
        if (0 == C_e)
            strcpy_s(p_name, "Unknown");
        else
        {
            strcpy_s(p_name, C_e->name_replace());
        }

        A->setName(p_name);
        u16 tmp_flags = A->flags__;

        if (Base == A)
            A->setFlag(GAME_PLAYER_FLAG_LOCAL);

        ClientID clientID = get_it_2_id(p_it);
        P.w_clientID(clientID);
        A->net_Export(P, TRUE);

        A->flags__ = tmp_flags;
    }

    net_Export_GameTime(P);
}

void game_sv_GameState::net_Export_Update(NET_Packet& P, ClientID id_to, ClientID id)
{
    game_PlayerState* A = get_id(id);
    if (A)
    {
        u16 bk_flags = A->flags__;
        if (id == id_to)
        {
            A->setFlag(GAME_PLAYER_FLAG_LOCAL);
        }

        P.w_clientID(id);
        A->net_Export(P);
        A->flags__ = bk_flags;
    };
};

void game_sv_GameState::net_Export_GameTime(NET_Packet& P)
{
    // Syncronize GameTime
    P.w_u64(GetGameTime());
    P.w_float(GetGameTimeFactor());
    // Syncronize EnvironmentGameTime
    P.w_u64(GetEnvironmentGameTime());
    P.w_float(GetEnvironmentGameTimeFactor());
};

void game_sv_GameState::OnPlayerConnect(ClientID /**id_who/**/) { signal_Syncronize(); }

void game_sv_GameState::Create(shared_str& options)
{
}

void game_sv_GameState::u_EventGen(NET_Packet& P, u16 type, u16 dest)
{
    P.w_begin(M_EVENT);
    P.w_u32(Level().timeServer()); // Device.TimerAsync());
    P.w_u16(type);
    P.w_u16(dest);
}

void game_sv_GameState::u_EventSend(NET_Packet& P, u32 dwFlags) { m_server->SendBroadcast(BroadcastCID, P, dwFlags); }

void game_sv_GameState::Update()
{
    for (u32 it = 0; it < m_server->client_Count(); ++it)
    {
        xrClientData* C = (xrClientData*)m_server->client_Get(it);
        C->ps->ping = u16(0);
    }
}

game_sv_GameState::game_sv_GameState()
{
    VERIFY(g_pGameLevel);
    m_server = Level().Server;
    m_event_queue = xr_new<GameEventQueue>();
}

game_sv_GameState::~game_sv_GameState()
{
    xr_delete(m_event_queue);
}

bool game_sv_GameState::change_level(NET_Packet& net_packet, ClientID sender) { return (true); }

void game_sv_GameState::save_game(NET_Packet& net_packet, ClientID sender) {}

bool game_sv_GameState::load_game(NET_Packet& net_packet, ClientID sender) { return (true); }

void game_sv_GameState::switch_distance(NET_Packet& net_packet, ClientID sender) {}

void game_sv_GameState::OnEvent(NET_Packet& tNetPacket, u16 type, u32 time, ClientID sender)
{
    switch (type)
    {
    case GAME_EVENT_PLAYER_CONNECTED: {
        ClientID ID;
        tNetPacket.r_clientID(ID);
        OnPlayerConnect(ID);
    }
    break;

    case GAME_EVENT_ON_HIT: {
        tNetPacket.r_u16();
        u16 id_src = tNetPacket.r_u16();
        CSE_Abstract* e_src = get_entity_from_eid(id_src);

        if (!e_src) // added by andy because of Phantom does not have server entity
            break;

        m_server->SendBroadcast(BroadcastCID, tNetPacket, net_flags(TRUE, TRUE));
    }
    break;
    case GAME_EVENT_CREATE_CLIENT: {
        IClient* CL = (IClient*)m_server->ID_to_client(sender);
        if (CL == NULL)
        {
            break;
        }

        CL->flags.bConnected = TRUE;
        m_server->AttachNewClient(CL);
    }
    break;
    case GAME_EVENT_PLAYER_AUTH: {
        IClient* CL = m_server->ID_to_client(sender);
        m_server->OnBuildVersionRespond(CL, tNetPacket);
    }
    break;
    default: {
        string16 tmp;
        R_ASSERT(0, "Game Event not implemented!!!", itoa(type, tmp, 10));
    }
    }
}

void game_sv_GameState::OnSwitchPhase(u32 old_phase, u32 new_phase)
{
    inherited::OnSwitchPhase(old_phase, new_phase);
    signal_Syncronize();
}

void game_sv_GameState::AddDelayedEvent(NET_Packet& tNetPacket, u16 type, u32 time, ClientID sender)
{
    //	OnEvent(tNetPacket,type,time,sender);
    m_event_queue->Create(tNetPacket, type, time, sender);
}

void game_sv_GameState::ProcessDelayedEvent()
{
    GameEvent* ge = NULL;
    while ((ge = m_event_queue->Retreive()) != 0)
    {
        OnEvent(ge->P, ge->type, ge->time, ge->sender);
        m_event_queue->Release();
    }
}

void game_sv_GameState::teleport_object(NET_Packet& packet, u16 id) {}

void game_sv_GameState::add_restriction(NET_Packet& packet, u16 id) {}

void game_sv_GameState::remove_restriction(NET_Packet& packet, u16 id) {}

void game_sv_GameState::remove_all_restrictions(NET_Packet& packet, u16 id) {}

shared_str game_sv_GameState::level_name(const shared_str& server_options) const
{
    string64 l_name = "";
    VERIFY(_GetItemCount(*server_options, '/'));
    return (_GetItem(*server_options, 0, l_name, '/'));
}

void game_sv_GameState::on_death(CSE_Abstract* e_dest, CSE_Abstract* e_src)
{
    CSE_ALifeCreatureAbstract* creature = smart_cast<CSE_ALifeCreatureAbstract*>(e_dest);
    if (!creature)
        return;

    VERIFY(creature->m_killer_id == ALife::_OBJECT_ID(-1));
    creature->m_killer_id = e_src->ID;
}


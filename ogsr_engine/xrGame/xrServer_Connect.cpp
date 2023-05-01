#include "stdafx.h"
#include "xrServer.h"
#include "game_sv_single.h"
#include "xrMessages.h"
#include "game_cl_single.h"
#include "MainMenu.h"

xrServer::EConnect xrServer::Connect(shared_str& session_name)
{
#ifdef DEBUG
    Msg("* sv_Connect: %s", *session_name);
#endif

    // Parse options and create game
    if (0 == strchr(*session_name, '/'))
        return ErrConnect;

    string1024 options;
    R_ASSERT2(xr_strlen(session_name) <= sizeof(options), "session_name too BIIIGGG!!!");
    strcpy_s(options, strchr(*session_name, '/') + 1);

    // Parse game type
    string1024 type;
    R_ASSERT2(xr_strlen(options) <= sizeof(type), "session_name too BIIIGGG!!!");
    strcpy_s(type, options);
    if (strchr(type, '/'))
        *strchr(type, '/') = 0;
    game = NULL;

    CLASS_ID clsid = game_GameState::getCLASS_ID(type, true);
    game = smart_cast<game_sv_GameState*>(NEW_INSTANCE(clsid));

    // Options
    if (0 == game)
        return ErrConnect;
    csPlayers.Enter();
//	game->type				= type_id;
#ifdef DEBUG
    Msg("* Created server_game %s", game->type_name());
#endif

    game->Create(session_name);
    csPlayers.Leave();

    return IPureServer::Connect(*session_name);
}

IClient* xrServer::new_client(SClientConnectData* cl_data)
{
    IClient* CL = client_Find_Get(cl_data->clientID);
    VERIFY(CL);

    // copy entity
    CL->ID = cl_data->clientID;
    CL->process_id = cl_data->process_id;

    string64 new_name;
    strcpy_s(new_name, cl_data->name);
    CL->name._set(new_name);
    CL->pass._set(cl_data->pass);

    NET_Packet P;
    P.B.count = 0;
    P.r_pos = 0;

    game->AddDelayedEvent(P, GAME_EVENT_CREATE_CLIENT, 0, CL->ID);
    if (client_Count() == 1)
    {
        Update();
    }
    return CL;
}

void xrServer::AttachNewClient(IClient* CL)
{
    MSYS_CONFIG msgConfig;
    msgConfig.sign1 = 0x12071980;
    msgConfig.sign2 = 0x26111975;

    SV_Client = CL;
    CL->flags.bLocal = 1;
    SendTo_LL(SV_Client->ID, &msgConfig, sizeof(msgConfig), net_flags(TRUE, TRUE, TRUE, TRUE));

    CL->m_guid[0] = 0;
}

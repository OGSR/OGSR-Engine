#include "stdafx.h"

#include "net_server.h"

ENGINE_API ClientID BroadcastCID(0xffffffff);

void ip_address::set(LPCSTR src_string) //Это нужно
{
    u32 buff[4];
    int cnt = sscanf(src_string, "%d.%d.%d.%d", &buff[0], &buff[1], &buff[2], &buff[3]);
    if (cnt == 4)
    {
        m_data.a1 = u8(buff[0] & 0xff);
        m_data.a2 = u8(buff[1] & 0xff);
        m_data.a3 = u8(buff[2] & 0xff);
        m_data.a4 = u8(buff[3] & 0xff);
    }
    else
    {
        Msg("! Bad ipAddress format [%s]", src_string);
        m_data.data = 0;
    }
}

IClient::IClient(CTimer* timer) : server(NULL)
{
    dwTime_LastUpdate = 0;
    flags.bLocal = FALSE;
    flags.bConnected = FALSE;
    flags.bReconnect = FALSE;
    flags.bVerified = TRUE;
}

IClient::~IClient() {}

IClient* IPureServer::ID_to_client(ClientID ID, bool ScanAll) //пока не резать. net_Players - не пустой вектор
{
    if (0 == ID.value())
        return NULL;
    csPlayers.Enter();

    for (u32 client = 0; client < net_Players.size(); ++client)
    {
        if (net_Players[client]->ID == ID)
        {
            csPlayers.Leave();
            return net_Players[client];
        }
    }
    if (ScanAll)
    {
        for (u32 client = 0; client < net_Players_disconnected.size(); ++client)
        {
            if (net_Players_disconnected[client]->ID == ID)
            {
                csPlayers.Leave();
                return net_Players_disconnected[client];
            }
        }
    };
    csPlayers.Leave();
    return NULL;
}

IPureServer::IPureServer(CTimer* timer)
#ifdef PROFILE_CRITICAL_SECTIONS
    : csPlayers(MUTEX_PROFILE_ID(IPureServer::csPlayers))
#endif // PROFILE_CRITICAL_SECTIONS
{
    device_timer = timer;
    SV_Client = NULL;
}

IPureServer::~IPureServer() { SV_Client = NULL; }

IPureServer::EConnect IPureServer::Connect(LPCSTR options) // опции вида [имя_сейва/single/alife]
{
    connect_options = options;
    return ErrNoError;
}

void IPureServer::Disconnect() {}

void IPureServer::SendTo_LL(ClientID ID /*DPNID ID*/, void* data, u32 size, u32 dwFlags, u32 dwTimeout) { FATAL(""); }

void IPureServer::SendTo(ClientID ID /*DPNID ID*/, NET_Packet& P, u32 dwFlags, u32 dwTimeout) //Отсюда отправляются данные в IPureClient::OnMessage
{
    /// Msg("~~[%s] Send to id [%u] data: [%p], flags: [%u], dwTimeout: [%u]", __FUNCTION__, ID.value(), P.B.data, dwFlags, dwTimeout);
    SendTo_LL(ID, P.B.data, P.B.count, dwFlags, dwTimeout);
}

void IPureServer::SendBroadcast_LL(ClientID exclude, void* data, u32 size, u32 dwFlags) //Отсюда отправляются данные в IPureClient::OnMessage
{
    csPlayers.Enter();

    for (u32 i = 0; i < net_Players.size(); i++)
    {
        IClient* player = net_Players[i];

        if (player->ID == exclude)
            continue;
        if (!player->flags.bConnected)
            continue;

        /// Msg("~~[%s] Send to id [%u] data: [%p], size: [%u], flags: [%u]", __FUNCTION__, player->ID.value(), data, size, dwFlags);
        SendTo_LL(player->ID, data, size, dwFlags);
    }

    csPlayers.Leave();
}

void IPureServer::SendBroadcast(ClientID exclude, NET_Packet& P, u32 dwFlags)
{
    // Perform broadcasting
    SendBroadcast_LL(exclude, P.B.data, P.B.count, dwFlags);
}

u32 IPureServer::OnMessage(NET_Packet& P, ClientID sender) { return 0; }

#include "stdafx.h"
#include "net_client.h"
#include "net_server.h"
#include "net_messages.h"

INetQueue::INetQueue()
#ifdef PROFILE_CRITICAL_SECTIONS
    : cs(MUTEX_PROFILE_ID(INetQueue))
#endif // PROFILE_CRITICAL_SECTIONS
{
    unused.reserve(128);
    for (int i = 0; i < 16; i++)
        unused.push_back(xr_new<NET_Packet>());
}

INetQueue::~INetQueue()
{
    cs.Enter();
    u32 it;
    for (it = 0; it < unused.size(); it++)
        xr_delete(unused[it]);
    for (it = 0; it < ready.size(); it++)
        xr_delete(ready[it]);
    cs.Leave();
}

static u32 LastTimeCreate = 0;

void INetQueue::CreateCommit(NET_Packet* P)
{
    cs.Enter();
    ready.push_back(P);
    cs.Leave();
}

NET_Packet* INetQueue::CreateGet()
{
    NET_Packet* P = 0;
    cs.Enter();

    if (unused.empty())
    {
        P = xr_new<NET_Packet>();

        LastTimeCreate = GetTickCount();
    }
    else
    {
        P = unused.back();
        unused.pop_back();
    }
    cs.Leave();
    return P;
}

NET_Packet* INetQueue::Retreive()
{
    NET_Packet* P = 0;
    cs.Enter();
    if (!ready.empty())
        P = ready.front();
    //---------------------------------------------
    else
    {
        u32 tmp_time = GetTickCount() - 60000;
        u32 size = unused.size();
        if ((LastTimeCreate < tmp_time) && (size > 32))
        {
            xr_delete(unused.back());
            unused.pop_back();
        }
    }
    //---------------------------------------------
    cs.Leave();
    return P;
}
void INetQueue::Release()
{
    cs.Enter();
    VERIFY(!ready.empty());
    //---------------------------------------------
    u32 tmp_time = GetTickCount() - 60000;
    u32 size = unused.size();
    if ((LastTimeCreate < tmp_time) && (size > 32))
    {
        xr_delete(ready.front());
    }
    else
        unused.push_back(ready.front());
    //---------------------------------------------
    ready.pop_front();
    cs.Leave();
}

IPureClient::IPureClient(CTimer* timer)
{
    device_timer = timer;
    net_Syncronised = FALSE; //Добавил инициализацию сюда, тк она хз где была.
}

IPureClient::~IPureClient() {}

BOOL IPureClient::Connect(LPCSTR) { return TRUE; }

void IPureClient::Disconnect() //Вызывается при выходе из игры и переходе на другой уровень.
{
    net_Connected = EnmConnectionWait;
    net_Syncronised = FALSE;
}

void IPureClient::OnMessage(void* data, u32 size) //Сюда приходят сообщения из IPureServer::SendTo и IPureServer::SendBroadcast_LL
{
    /// Msg("~~[%s] data: [%p], size: [%u]", __FUNCTION__, data, size);

    // One of the messages - decompress it
    NET_Packet* P = net_Queue.CreateGet();

    P->construct(data, size);
    P->timeReceive = timeServer_Async();

    u16 tmp_type;
    P->r_begin(tmp_type);
    net_Queue.CreateCommit(P);
}

void IPureClient::Send(NET_Packet& packet, u32 dwFlags, u32 dwTimeout) { FATAL(""); }

#pragma once

#include "net_shared.h"

struct SClientConnectData
{
    ClientID clientID;
    string64 name;
    string64 pass;
    u32 process_id;

    SClientConnectData()
    {
        name[0] = pass[0] = 0;
        process_id = 0;
    }
};

class IPureServer;

struct ENGINE_API ip_address
{
    union
    {
        struct
        {
            u8 a1;
            u8 a2;
            u8 a3;
            u8 a4;
        };
        u32 data;
    } m_data;
    void set(LPCSTR src_string);

    bool operator==(const ip_address& other) const
    {
        return (m_data.data == other.m_data.data) || ((m_data.a1 == other.m_data.a1) && (m_data.a2 == other.m_data.a2) && (m_data.a3 == other.m_data.a3) && (m_data.a4 == 0));
    }
};

class ENGINE_API IClient
{
public:
    struct Flags
    {
        u32 bLocal : 1;
        u32 bConnected : 1;
        u32 bReconnect : 1;
        u32 bVerified : 1;
    };

    IClient(CTimer* timer);
    virtual ~IClient();

    ClientID ID;
    string128 m_guid;
    shared_str name;
    shared_str pass;

    Flags flags; // local/host/normal
    u32 dwTime_LastUpdate;

    ip_address m_cAddress;
    DWORD m_dwPort;
    u32 process_id;

    IPureServer* server;
    bool operator==(ClientID const&) = delete;
};

class CServerInfo;

class ENGINE_API IPureServer
{
public:
    enum EConnect
    {
        ErrConnect,
        ErrBELoad,
        ErrNoLevel,
        ErrMax,
        ErrNoError = ErrMax,
    };

protected:
    shared_str connect_options;

    xrCriticalSection csPlayers;
    xr_vector<IClient*> net_Players;
    xr_vector<IClient*> net_Players_disconnected;
    IClient* SV_Client;

    CTimer* device_timer;

    IClient* ID_to_client(ClientID ID, bool ScanAll = false);

    virtual IClient* new_client(SClientConnectData* cl_data) = 0;

public:
    IPureServer(CTimer* timer);
    virtual ~IPureServer();

    virtual EConnect Connect(LPCSTR session_name);
    virtual void Disconnect();

    // send
    virtual void SendTo_LL(ClientID ID, void* data, u32 size, u32 dwFlags = DPNSEND_GUARANTEED, u32 dwTimeout = 0);

    void SendTo(ClientID ID, NET_Packet& P, u32 dwFlags = DPNSEND_GUARANTEED, u32 dwTimeout = 0);
    void SendBroadcast_LL(ClientID exclude, void* data, u32 size, u32 dwFlags = DPNSEND_GUARANTEED);
    void SendBroadcast(ClientID exclude, NET_Packet& P, u32 dwFlags = DPNSEND_GUARANTEED);

    // extended functionality
    virtual u32 OnMessage(NET_Packet& P, ClientID sender); // Non-Zero means broadcasting with "flags" as returned

    virtual IClient* client_Create() = 0; // create client info
    virtual void client_Destroy(IClient* C) = 0; // destroy client info

    IC u32 client_Count() { return net_Players.size(); }
    IC IClient* client_Get(u32 num) { return net_Players[num]; }

    IClient* GetServerClient() { return SV_Client; };
};

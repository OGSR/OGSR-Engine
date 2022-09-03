#include "stdafx.h"
#include "Level.h"
#include "Level_Bullet_Manager.h"
#include "xrserver.h"
#include "xrmessages.h"
#include "game_cl_base.h"
#include "PHCommander.h"
#include "net_queue.h"
#include "MainMenu.h"
#include "space_restriction_manager.h"
#include "ai_space.h"
#include "script_engine.h"
#include "stalker_animation_data_storage.h"
#include "client_spawn_manager.h"
#include "seniority_hierarchy_holder.h"
#include "script_vars_storage.h"

constexpr int max_objects_size = 2 * 1024;
constexpr int max_objects_size_in_save = 6 * 1024;

void Remove_all_statics();

void CLevel::remove_objects()
{
    m_is_removing_objects = true;
    BOOL b_stored = psDeviceFlags.test(rsDisableObjectsAsCrows);

    Game().reset_ui();

    {
        VERIFY(Server);
        Server->SLS_Clear();
    }

    snd_Events.clear();
    for (int i = 0; i < 6; ++i)
    {
        // ugly hack for checks that update is twice on frame
        // we need it since we do updates for checking network messages
        ++(Device.dwFrame);
        psDeviceFlags.set(rsDisableObjectsAsCrows, TRUE);

        ClientReceive();
        ProcessGameEvents();

        Objects.Update(true);
        //Sleep(100);
    }


    BulletManager().Clear();
    ph_commander().clear();
    ph_commander_scripts().clear();

    space_restriction_manager().clear();
    psDeviceFlags.set(rsDisableObjectsAsCrows, b_stored);

    ai().script_engine().collect_all_garbage();

    Remove_all_statics();

    stalker_animation_data_storage().clear();

    VERIFY(Render);

    if (!g_prefetch)
        Render->models_Clear(TRUE);
    else
        Render->models_Clear(FALSE);

    Render->clear_static_wallmarks();

#ifdef DEBUG
    if (!client_spawn_manager().registry().empty())
        client_spawn_manager().dump();
#endif // DEBUG
    VERIFY(client_spawn_manager().registry().empty());
    client_spawn_manager().clear();

    for (int i = 0; i < 6; i++)
    {
        ++(Device.dwFrame);
        Objects.Update(true);
    }

    g_pGamePersistent->destroy_particles(false);
    ::Sound->stop_emitters();

    g_ScriptVars.clear();

    shader_exports.set_dof_params(0.f, 0.f, 0.f, 0.f);

    m_is_removing_objects = false;
}

#ifdef DEBUG
extern void show_animation_stats();
#endif // DEBUG

void CLevel::net_Stop()
{
    Msg("- Disconnect");
    bReady = false;
    m_bGameConfigStarted = FALSE;
    game_configured = FALSE;

    remove_objects();

    IGame_Level::net_Stop();
    IPureClient::Disconnect();

    if (Server)
    {
        Server->Disconnect();
        xr_delete(Server);
    }

    ai().script_engine().collect_all_garbage();

    Remove_all_statics();

#ifdef DEBUG
    show_animation_stats();
#endif // DEBUG
}

void CLevel::ClientSend()
{
    for (u32 i = 0; i < Objects.o_count(); i++)
    {
        CObject* O = Objects.o_get_by_iterator(i);
        if (O->net_Relevant() && !O->getDestroy())
        {
            CSE_Abstract* E = Server->ID_to_entity(O->ID());
            if (E)
                O->net_Export(E);
        }
    }
}

#include "xrServer_Objects.h"

void CLevel::ClientSave()
{
    for (u32 i = 0; i < Objects.o_count(); i++)
    {
        CObject* O = Objects.o_get_by_iterator(i);
        CGameObject* P = smart_cast<CGameObject*>(O);
        // Msg( "save:iterating:%d:%s", P->ID(), *P->cName() );
        if (P && !P->getDestroy() && P->net_SaveRelevant())
        {
            NET_Packet Packet;

            P->net_Save(Packet); // вызов save(NET_Packet& output_packet)

            CSE_Abstract* E = Server->ID_to_entity(O->ID());

            if (E)
            {
                E->net_Ready = TRUE;
                E->load(Packet);
            }
        }
    }
}

extern float phTimefactor;

void CLevel::Send(NET_Packet& P, u32 dwFlags, u32 dwTimeout)
{
    // optimize the case when server located in our memory
    ClientID _clid;
    _clid.set(1);
    Server->OnMessage(P, _clid);
}

void CLevel::net_Update()
{
    if (game_configured)
    {
        // If we have enought bandwidth - replicate client data on to server
        Device.Statistic->netClient2.Begin();
        ClientSend();
        Device.Statistic->netClient2.End();
    }
    // If server - perform server-update
    if (Server)
    {
        Device.Statistic->netServer.Begin();
        Server->Update();
        Device.Statistic->netServer.End();
    }
}

struct _NetworkProcessor : public pureFrame
{
    virtual void OnFrame()
    {
        if (g_pGameLevel && !Device.Paused())
            g_pGameLevel->net_Update();
    }
} NET_processor;

pureFrame* g_pNetProcessor = &NET_processor;

const int ConnectionTimeOut = 60000; // 1 min

BOOL CLevel::Connect2Server(LPCSTR options)
{
    NET_Packet P;
    m_bConnectResultReceived = false;
    m_bConnectResult = true;
    if (!Connect(options))
        return FALSE;
    //---------------------------------------------------------------------------
    m_bConnectResultReceived = true;
    u32 EndTime = GetTickCount() + ConnectionTimeOut;
    while (!m_bConnectResultReceived)
    {
        ClientReceive();
        Sleep(5);
        if (Server)
            Server->Update();
        //-----------------------------------------
        u32 CurTime = GetTickCount();
        if (CurTime > EndTime)
        {
            NET_Packet P;
            P.B.count = 0;
            P.r_pos = 0;

            P.w_u8(0);
            P.w_u8(0);
            P.w_stringZ("Data verification failed. Cheater? [1]");

            OnConnectResult(&P);
        }
        if (net_isFails_Connect())
        {
            OnConnectRejected();
            Disconnect();
            return FALSE;
        }
        //-----------------------------------------
    }
    Msg("%c client : connection %s - <%s>", m_bConnectResult ? '*' : '!', m_bConnectResult ? "accepted" : "rejected", m_sConnectResult.c_str());
    if (!m_bConnectResult)
    {
        OnConnectRejected();
        Disconnect();
        return FALSE;
    };

    net_Syncronised = TRUE;

    //---------------------------------------------------------------------------
    P.w_begin(M_CLIENT_REQUEST_CONNECTION_DATA);
    Send(P, net_flags(TRUE, TRUE, TRUE, TRUE));
    //---------------------------------------------------------------------------
    return TRUE;
};

void CLevel::OnBuildVersionChallenge()
{
    NET_Packet P;
    P.w_begin(M_CL_AUTH);
    u64 auth = 0;
    P.w_u64(auth);
    Send(P, net_flags(TRUE, TRUE, TRUE, TRUE));
};

void CLevel::OnConnectResult(NET_Packet* P)
{
    // multiple results can be sent during connection they should be "AND-ed"
    m_bConnectResultReceived = true;
    u8 result = P->r_u8();
    /*u8  res1					=*/P->r_u8(); //Оставлено для совместимости сейвов
    string128 ResultStr;
    P->r_stringZ(ResultStr);
    if (!result)
        m_bConnectResult = false;

    m_sConnectResult = ResultStr;
};

void CLevel::OnConnectRejected() { IPureClient::OnConnectRejected(); };

void CLevel::net_OnChangeSelfName(NET_Packet* P)
{
    if (!P)
        return;
    string64 NewName;
    P->r_stringZ(NewName);
    if (!strstr(*m_caClientOptions, "/name="))
    {
        string1024 tmpstr;
        strcpy_s(tmpstr, *m_caClientOptions);
        strcat_s(tmpstr, "/name=");
        strcat_s(tmpstr, NewName);
        m_caClientOptions = tmpstr;
    }
    else
    {
        string1024 tmpstr;
        strcpy_s(tmpstr, *m_caClientOptions);
        *(strstr(tmpstr, "name=") + 5) = 0;
        strcat_s(tmpstr, NewName);
        const char* ptmp = strchr(strstr(*m_caClientOptions, "name="), '/');
        if (ptmp)
            strcat_s(tmpstr, ptmp);
        m_caClientOptions = tmpstr;
    }
}

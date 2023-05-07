#include "stdafx.h"
#include "entity.h"
#include "xrserver_objects.h"
#include "level.h"
#include "xrmessages.h"
#include "game_cl_base.h"
#include "net_queue.h"
#include "Physics.h"
#include "xrServer.h"
#include "Actor.h"
#include "ai_space.h"
#include "saved_game_wrapper.h"
#include "level_graph.h"
#include "clsid_game.h"

#include "HudManager.h"
#include "UIGameSP.h"

void CLevel::ClientReceive()
{

    m_dwRPC = 0;
    m_dwRPS = 0;

    for (NET_Packet* P = net_msg_Retreive(); P; P = net_msg_Retreive())
    {
        //-----------------------------------------------------
        m_dwRPC++;
        m_dwRPS += P->B.count;
        //-----------------------------------------------------
        u16 m_type;
        P->r_begin(m_type);
        switch (m_type)
        {
        case M_SPAWN: {
            if (!m_bGameConfigStarted || !bReady)
            {
                Msg("Unconventional M_SPAWN received : cgf[%s] | bReady[%s]", (m_bGameConfigStarted) ? "true" : "false", (bReady) ? "true" : "false");
                break;
            }
            /*/
            cl_Process_Spawn(*P);
            /*/
            game_events->insert(*P);
            if (g_bDebugEvents)
                ProcessGameEvents();
            //*/
        }
        break;
        case M_EVENT:
            game_events->insert(*P);
            if (g_bDebugEvents)
                ProcessGameEvents();
            break;
        case M_EVENT_PACK: {
            NET_Packet tmpP;
            while (!P->r_eof())
            {
                tmpP.B.count = P->r_u8();
                P->r(&tmpP.B.data, tmpP.B.count);
                tmpP.timeReceive = P->timeReceive;

                game_events->insert(tmpP);
                if (g_bDebugEvents)
                    ProcessGameEvents();
            }
        }
        break;
        case M_UPDATE: {
            game->net_import_update(*P);
        }
        break;
        case M_SV_CONFIG_NEW_CLIENT: InitializeClientGame(*P); break;
        case M_SV_CONFIG_GAME: game->net_import_state(*P); break;
        case M_SV_CONFIG_FINISHED:
            game_configured = TRUE;
            Msg("- Game configuring : Finished ");
            break;
        case M_RELOAD_GAME:
        case M_LOAD_GAME:
        case M_CHANGE_LEVEL: {

            // костыль для автозакрытия любого диалога в случае смены уровня
            if (HUD().GetUI()->MainInputReceiver())
            {
                HUD().GetUI()->StartStopMenu(HUD().GetUI()->MainInputReceiver(), true);
            }

            if (m_type == M_LOAD_GAME)
            {
                string256 saved_name;
                P->r_stringZ(saved_name);
                if (xr_strlen(saved_name) && ai().get_alife())
                {
                    CSavedGameWrapper wrapper(saved_name);
                    if (wrapper.level_id() == ai().level_graph().level_id())
                    {
                        Engine.Event.Defer("Game:QuickLoad", size_t(xr_strdup(saved_name)), 0);

                        break;
                    }
                }
            }

            Engine.Event.Defer("KERNEL:disconnect");
            Engine.Event.Defer("KERNEL:start", size_t(xr_strdup(*m_caServerOptions)), size_t(xr_strdup(*m_caClientOptions)));
        }
        break;
        case M_SAVE_GAME: {
            //ClientSave();
        }
        break;
        case M_AUTH_CHALLENGE: {
            OnBuildVersionChallenge();
        }
        break;
        case M_CLIENT_CONNECT_RESULT: {
            OnConnectResult(P);
        }
        break;
        case M_CHANGE_LEVEL_GAME: {
            Msg("- M_CHANGE_LEVEL_GAME Received");
            {
                const char* m_SO = m_caServerOptions.c_str();
                //					const char* m_CO = m_caClientOptions.c_str();

                m_SO = strchr(m_SO, '/');
                if (m_SO)
                    m_SO++;
                m_SO = strchr(m_SO, '/');

                string128 LevelName = "";
                string128 GameType = "";

                P->r_stringZ(LevelName);
                P->r_stringZ(GameType);

                string4096 NewServerOptions{};
                sprintf_s(NewServerOptions, "%s/%s", LevelName, GameType);

                if (m_SO)
                    strcat_s(NewServerOptions, m_SO);
                m_caServerOptions = NewServerOptions;

                Engine.Event.Defer("KERNEL:disconnect");
                Engine.Event.Defer("KERNEL:start", size_t(xr_strdup(*m_caServerOptions)), size_t(xr_strdup(*m_caClientOptions)));
            };
        }
        break;
        case M_CHANGE_SELF_NAME: {
            net_OnChangeSelfName(P);
        }
        break;
        default: break;
        }

        net_msg_Release();
    }

    //	if (!g_bDebugEvents) ProcessGameSpawns();
}

void CLevel::OnMessage(void* data, u32 size)
{
    IPureClient::OnMessage(data, size);

};

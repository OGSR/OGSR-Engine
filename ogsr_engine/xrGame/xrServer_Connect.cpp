#include "stdafx.h"
#include "xrServer.h"
#include "game_sv_single.h"
#include "game_sv_deathmatch.h"
#include "game_sv_teamdeathmatch.h"
#include "game_sv_artefacthunt.h"
#include "xrMessages.h"
#include "game_cl_artefacthunt.h"
#include "game_cl_single.h"
#include "MainMenu.h"

#pragma warning(push)
#pragma warning(disable:4995)
#include <malloc.h>
#pragma warning(pop)

xrServer::EConnect xrServer::Connect(shared_str &session_name)
{
#ifdef DEBUG
	Msg						("* sv_Connect: %s",	*session_name);
#endif

	// Parse options and create game
	if (0==strchr(*session_name,'/'))
		return				ErrConnect;

	string1024				options;
	R_ASSERT2(xr_strlen(session_name) <= sizeof(options), "session_name too BIIIGGG!!!");
	strcpy					(options,strchr(*session_name,'/')+1);
	
	// Parse game type
	string1024				type;
	R_ASSERT2(xr_strlen(options) <= sizeof(type), "session_name too BIIIGGG!!!");
	strcpy					(type,options);
	if (strchr(type,'/'))	*strchr(type,'/') = 0;
	game					= NULL;

	CLASS_ID clsid			= game_GameState::getCLASS_ID(type,true);
	game					= smart_cast<game_sv_GameState*> (NEW_INSTANCE(clsid));

	// Options
	if (0==game)			return ErrConnect;
	csPlayers.Enter			();
//	game->type				= type_id;
#ifdef DEBUG
	Msg("* Created server_game %s",game->type_name());
#endif

	game->Create			(session_name);
	csPlayers.Leave			();
	
#ifdef BATTLEYE
	if ( game->get_option_i( *session_name, "battleye", 1) != 0 ) // default => battleye enable (always)
	{
		// if level exist & if server in internet
		if ( g_pGameLevel && (game->get_option_i( *session_name, "public", 0) != 0)  )
		{
			if ( Level().battleye_system.server )
			{
				Msg( "Warning: BattlEye already loaded!" );
			}
			else
			{
				if ( !Level().battleye_system.LoadServer( this ) )
				{
					return ErrBELoad;
				}
			}
		}//g_pGameLevel
	}
/*	if ( g_pGameLevel && Level().battleye_system.server )
	{
		if ( game->get_option_i( *session_name, "battleye_update", 1) != 0 ) // default => battleye auto_update enable (always)
		{
			Level().battleye_system.auto_update = 1;
		}
		else
		{
			Level().battleye_system.auto_update = 0;
		}
	}*/
#endif // BATTLEYE
	
	return IPureServer::Connect(*session_name);
}


IClient* xrServer::new_client( SClientConnectData* cl_data )
{
	IClient* CL		= client_Find_Get( cl_data->clientID );
	VERIFY( CL );
	
	// copy entity
	CL->ID			= cl_data->clientID;
	CL->process_id	= cl_data->process_id;
	
	string64 new_name;
	strcpy_s( new_name, cl_data->name );
	CL->name._set( new_name );
	
	if ( !HasProtected() && game->NewPlayerName_Exists( CL, new_name ) )
	{
		game->NewPlayerName_Generate( CL, new_name );
		game->NewPlayerName_Replace( CL, new_name );
	}
	CL->name._set( new_name );
	CL->pass._set( cl_data->pass );

	NET_Packet		P;
	P.B.count		= 0;
	P.r_pos			= 0;
	
	game->AddDelayedEvent( P, GAME_EVENT_CREATE_CLIENT, 0, CL->ID );
	if ( client_Count() == 1 )
	{
		Update();
	}
	return CL;
}

void xrServer::AttachNewClient			(IClient* CL)
{
	MSYS_CONFIG	msgConfig;
	msgConfig.sign1 = 0x12071980;
	msgConfig.sign2 = 0x26111975;
	msgConfig.is_battleye = 0;

#ifdef BATTLEYE
	msgConfig.is_battleye = (g_pGameLevel && Level().battleye_system.server != 0)? 1 : 0;
#endif // BATTLEYE

	if(psNET_direct_connect)  //single_game
	{
        SV_Client			= CL;
		CL->flags.bLocal	= 1;
		SendTo_LL( SV_Client->ID, &msgConfig, sizeof(msgConfig), net_flags(TRUE,TRUE,TRUE,TRUE) );
	}
	else
	{
		SendTo_LL				(CL->ID,&msgConfig,sizeof(msgConfig), net_flags(TRUE, TRUE, TRUE, TRUE));
		Server_Client_Check		(CL); 
	}

	// gen message
	if (!NeedToCheckClient_GameSpy_CDKey(CL))
	{
	//-------------------------------------------------------------
	Check_GameSpy_CDKey_Success(CL);
 }

	//xrClientData * CL_D=(xrClientData*)(CL); 
	//ip_address				ClAddress;
	//GetClientAddress		(CL->ID, ClAddress);
	CL->m_guid[0]=0;
}



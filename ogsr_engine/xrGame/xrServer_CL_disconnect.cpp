#include "stdafx.h"
#include "hudmanager.h"
#include "xrserver.h"
#include "game_sv_single.h"
#include "alife_simulator.h"
#include "xrserver_objects.h"
#include "level.h"

void xrServer::OnCL_Disconnected	(IClient* CL)
{
	csPlayers.Enter			();

	// Game config (all, info includes deleted player now, excludes at the next cl-update)
	NET_Packet P;
	P.B.count = 0;
	P.w_clientID(CL->ID);
	P.w_stringZ(CL->name);
	xrClientData* xrCData = (xrClientData*)(CL);
	P.w_u16( (NULL != xrCData) ? xrCData->ps->GameID : 0);
	P.r_pos = 0;
	
	ClientID clientID;
	clientID.set(0);
	
	if (xrCData->owner != 0)
	{
		game->AddDelayedEvent(P,GAME_EVENT_PLAYER_DISCONNECTED, 0, clientID);
	};

	if (!(client_Count()>1 && !CL->flags.bLocal))
	{
		// Destroy entities
		while (!entities.empty())		{
			CSE_Abstract*	entity		= entities.begin()->second;
			entity_Destroy	(entity);
		}
	}	
	csPlayers.Leave			();

	Server_Client_Check(CL);
}

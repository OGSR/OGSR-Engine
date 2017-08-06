#include "stdafx.h"
#include "game_sv_teamdeathmatch.h"
#include "xrServer.h"
#include "xrMessages.h"

void	game_sv_TeamDeathmatch::OnEvent (NET_Packet &P, u16 type, u32 time, ClientID sender )
{

	inherited::OnEvent(P, type, time, sender);
	/*
	switch	(type)
	{
	
	case GAME_EVENT_PLAYER_CHANGE_TEAM: //cs & TDM
		{
			xrClientData *l_pC = m_server->ID_to_client(sender);
			s16 l_team; 
			P.r_s16(l_team);
			OnPlayerChangeTeam(l_pC->ID, l_team);
		}break;
	default:
		inherited::OnEvent(P, type, time, sender);
	}
	*/
}
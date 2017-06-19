#include "StdAfx.h"

#include "../xrGameSpyServer.h"
#include "GameSpy_Keys.h"
#include "../Level.h"

#include "../game_sv_artefacthunt.h"
//--------------------------- QR2 callbacks ---------------------------------------
#define ADD_KEY_VAL(g, q, qf, o, gf)		{if (g) {q->qf(o, g->gf);} else q->BufferAdd(o, "");}
#define ADD_KEY_VAL_INT(g, q, qf, o, gf)		{if (g) {q->qf(o, int(g->gf));} else q->BufferAdd(o, "");}
extern u32 g_sv_dwMaxClientPing;
void __cdecl callback_serverkey(int keyid, void* outbuf, void *userdata)
{
	if (!userdata) return;
	xrGameSpyServer* pServer = (xrGameSpyServer*) userdata;
	if (!pServer) return;
	CGameSpy_QR2* pQR2 = pServer->QR2();
	if (!pQR2) return;

	

	game_sv_mp* gmMP = smart_cast<game_sv_mp*>(pServer->game);
	game_sv_Deathmatch* gmDM = smart_cast<game_sv_Deathmatch*>(pServer->game);
	game_sv_TeamDeathmatch* gmTDM = smart_cast<game_sv_TeamDeathmatch*>(pServer->game);
	game_sv_ArtefactHunt* gmAhunt = smart_cast<game_sv_ArtefactHunt*>(pServer->game);

	string4096		game_version;

	switch (keyid)
	{
	case HOSTNAME_KEY:		pQR2->BufferAdd(outbuf, pServer->HostName.c_str()); break;
	case MAPNAME_KEY:		pQR2->BufferAdd(outbuf, pServer->MapName.c_str()); break;
	case GAMEVER_KEY:		pQR2->BufferAdd(outbuf, pQR2->GetGameVersion(game_version)); break;
	case NUMPLAYERS_KEY:	pQR2->BufferAdd_Int(outbuf, pServer->GetPlayersCount()); break;
	case MAXPLAYERS_KEY:	pQR2->BufferAdd_Int(outbuf, pServer->m_iMaxPlayers); break;
	case GAMETYPE_KEY:		ADD_KEY_VAL(pServer->game, pQR2, BufferAdd, outbuf, type_name()); break; //		pQR2->BufferAdd(outbuf, pServer->game->type_name()); break;
	case GAMEMODE_KEY:		pQR2->BufferAdd(outbuf, "openplaying"); break;
	case PASSWORD_KEY:
		if ( 0 == *(pServer->Password) )
		{
			pQR2->BufferAdd_Int( outbuf, 0 );
		}
		else
		{
			pQR2->BufferAdd_Int( outbuf, 1 );
		}
		break;
	case G_USER_PASSWORD_KEY:
		if ( pServer->HasProtected() )
		{
			pQR2->BufferAdd_Int( outbuf, 1 );
		}
		else
		{
			pQR2->BufferAdd_Int( outbuf, 0 );
		}
		break;
	case G_BATTLEYE_KEY:
#ifdef BATTLEYE
		if ( g_pGameLevel && Level().battleye_system.server )
		{
			pQR2->BufferAdd_Int( outbuf, 1 );
		}
		else
		{
			pQR2->BufferAdd_Int( outbuf, 0 );
		}
#endif // BATTLEYE
		break;
	case HOSTPORT_KEY:		pQR2->BufferAdd_Int(outbuf, pServer->GetPort()); break;

	case DEDICATED_KEY:		pQR2->BufferAdd_Int(outbuf, pServer->IsDedicated());		break;
	case GAMETYPE_NAME_KEY: ADD_KEY_VAL(pServer->game, pQR2, BufferAdd_Int, outbuf, Type()); break; //pQR2->BufferAdd_Int(outbuf, pServer->game->Type()); break;
	case NUMTEAMS_KEY:		ADD_KEY_VAL(gmMP, pQR2, BufferAdd_Int, outbuf, GetNumTeams()); break; //pQR2->BufferAdd_Int(outbuf, gmMP->GetNumTeams()); break;		
	case G_MAX_PING_KEY:	pQR2->BufferAdd_Int(outbuf, g_sv_dwMaxClientPing); break;
	//------- game ---------//	
	case G_MAP_ROTATION_KEY:			ADD_KEY_VAL(gmDM, pQR2, BufferAdd_Int, outbuf, HasMapRotation()); break; //if (gmDM) pQR2->BufferAdd_Int(outbuf, gmDM->HasMapRotation());		else pQR2->BufferAdd(outbuf, "");	break;
	case G_VOTING_ENABLED_KEY:			ADD_KEY_VAL(gmDM, pQR2, BufferAdd_Int, outbuf, IsVotingEnabled()); break; //if (gmDM) pQR2->BufferAdd_Int(outbuf, gmDM->IsVotingEnabled());		else pQR2->BufferAdd(outbuf, ""); break;
	case G_SPECTATOR_MODES_KEY:			ADD_KEY_VAL(gmDM, pQR2, BufferAdd_Int, outbuf, GetSpectatorModes()); break; //if (gmDM) pQR2->BufferAdd_Int(outbuf, gmDM->GetSpectatorModes());	else pQR2->BufferAdd(outbuf, "");	 break;		
		//------- deathmatch -------//
	case G_FRAG_LIMIT_KEY:				ADD_KEY_VAL(gmDM, pQR2, BufferAdd_Int, outbuf, GetFragLimit()); break; //if (gmDM)pQR2->BufferAdd_Int(outbuf, gmDM->GetFragLimit());			else pQR2->BufferAdd(outbuf, "");	break;
	case G_TIME_LIMIT_KEY:				ADD_KEY_VAL(gmDM, pQR2, BufferAdd_Int, outbuf, GetTimeLimit()); break; //if (gmDM)pQR2->BufferAdd_Int(outbuf, gmDM->GetTimeLimit());			else pQR2->BufferAdd(outbuf, "");	break;
	case G_DAMAGE_BLOCK_TIME_KEY:		ADD_KEY_VAL(gmDM, pQR2, BufferAdd_Int, outbuf, GetDMBLimit()); break; //if (gmDM)pQR2->BufferAdd_Int(outbuf, gmDM->GetDMBLimit());			else pQR2->BufferAdd(outbuf, "");	break;
	case G_DAMAGE_BLOCK_INDICATOR_KEY:	ADD_KEY_VAL(gmDM, pQR2, BufferAdd_Int, outbuf, IsDamageBlockIndEnabled()); break; //if (gmDM)pQR2->BufferAdd_Int(outbuf, gmDM->IsDamageBlockIndEnabled()); else pQR2->BufferAdd(outbuf, "");	break;
	case G_ANOMALIES_ENABLED_KEY:		ADD_KEY_VAL(gmDM, pQR2, BufferAdd_Int, outbuf, IsAnomaliesEnabled()); break; //if (gmDM)pQR2->BufferAdd_Int(outbuf, gmDM->IsAnomaliesEnabled());	else pQR2->BufferAdd(outbuf, "");	break;
	case G_ANOMALIES_TIME_KEY:			ADD_KEY_VAL(gmDM, pQR2, BufferAdd_Int, outbuf, GetAnomaliesTime()); break; //if (gmDM)pQR2->BufferAdd_Int(outbuf, gmDM->GetAnomaliesTime());		else pQR2->BufferAdd(outbuf, "");	break;
	case G_WARM_UP_TIME_KEY:			ADD_KEY_VAL(gmDM, pQR2, BufferAdd_Int, outbuf, GetWarmUpTime()); break; //if (gmDM)pQR2->BufferAdd_Int(outbuf, gmDM->GetWarmUpTime());			else pQR2->BufferAdd(outbuf, "");	break;
	case G_FORCE_RESPAWN_KEY:			ADD_KEY_VAL(gmDM, pQR2, BufferAdd_Int, outbuf, GetForceRespawn()); break; //if (gmDM)pQR2->BufferAdd_Int(outbuf, gmDM->GetForceRespawn());		else pQR2->BufferAdd(outbuf, "");	break;
		//---- game_sv_teamdeathmatch ----
	case G_AUTO_TEAM_BALANCE_KEY:		ADD_KEY_VAL(gmTDM, pQR2, BufferAdd_Int, outbuf, Get_AutoTeamBalance()); break; //if (gmTDM)pQR2->BufferAdd_Int(outbuf, gmTDM->Get_AutoTeamBalance	());			break;
	case G_AUTO_TEAM_SWAP_KEY:			ADD_KEY_VAL(gmTDM, pQR2, BufferAdd_Int, outbuf, Get_AutoTeamSwap()); break; //if (gmTDM)pQR2->BufferAdd_Int(outbuf, gmTDM->Get_AutoTeamSwap		());			break;
	case G_FRIENDLY_INDICATORS_KEY:		ADD_KEY_VAL(gmTDM, pQR2, BufferAdd_Int, outbuf, Get_FriendlyIndicators()); break; //if (gmTDM)pQR2->BufferAdd_Int(outbuf, gmTDM->Get_FriendlyIndicators	());			break;
	case G_FRIENDLY_NAMES_KEY:			ADD_KEY_VAL(gmTDM, pQR2, BufferAdd_Int, outbuf, Get_FriendlyNames()); break; //if (gmTDM)pQR2->BufferAdd_Int(outbuf, gmTDM->Get_FriendlyNames		());			break;
	case G_FRIENDLY_FIRE_KEY:			ADD_KEY_VAL_INT(gmTDM, pQR2, BufferAdd_Int, outbuf, GetFriendlyFire()*100.0f); break; //if (gmTDM)pQR2->BufferAdd_Int(outbuf, int(gmTDM->GetFriendlyFire()*100.0f));		break;
		//---- game_sv_artefacthunt ----	
	case G_ARTEFACTS_COUNT_KEY:			ADD_KEY_VAL(gmAhunt, pQR2, BufferAdd_Int, outbuf, Get_ArtefactsCount()); break; //if (gmAhunt) pQR2->BufferAdd_Int(outbuf, gmAhunt->Get_ArtefactsCount		());			break;
	case G_ARTEFACT_STAY_TIME_KEY:		ADD_KEY_VAL(gmAhunt, pQR2, BufferAdd_Int, outbuf, Get_ArtefactsStayTime()); break; //if (gmAhunt) pQR2->BufferAdd_Int(outbuf, gmAhunt->Get_ArtefactsStayTime		());			break;
	case G_ARTEFACT_RESPAWN_TIME_KEY:	ADD_KEY_VAL(gmAhunt, pQR2, BufferAdd_Int, outbuf, Get_ArtefactsRespawnDelta()); break; //if (gmAhunt) pQR2->BufferAdd_Int(outbuf, gmAhunt->Get_ArtefactsRespawnDelta	());			break;
	case G_REINFORCEMENT_KEY:			ADD_KEY_VAL(gmAhunt, pQR2, BufferAdd_Int, outbuf, Get_ReinforcementTime()); break; //if (gmAhunt) pQR2->BufferAdd_Int(outbuf, gmAhunt->Get_ReinforcementTime		());			break;
	case G_SHIELDED_BASES_KEY:			ADD_KEY_VAL(gmAhunt, pQR2, BufferAdd_Int, outbuf, Get_ShieldedBases()); break; //if (gmAhunt) pQR2->BufferAdd_Int(outbuf, gmAhunt->Get_ShieldedBases			());			break;
	case G_RETURN_PLAYERS_KEY:			ADD_KEY_VAL(gmAhunt, pQR2, BufferAdd_Int, outbuf, Get_ReturnPlayers()); break; //if (gmAhunt) pQR2->BufferAdd_Int(outbuf, gmAhunt->Get_ReturnPlayers			());			break;
	case G_BEARER_CANT_SPRINT_KEY:		ADD_KEY_VAL(gmAhunt, pQR2, BufferAdd_Int, outbuf, Get_BearerCantSprint()); break; //if (gmAhunt) pQR2->BufferAdd_Int(outbuf, gmAhunt->Get_BearerCantSprint		());			break;
	default:
		{
//			R_ASSERT2(0, "Unknown GameSpy Server key ");
			pQR2->BufferAdd(outbuf, "");
		}break;
	}
	//GSI_UNUSED(userdata);
};

void __cdecl callback_playerkey(int keyid, int index, void* outbuf, void *userdata)
{
	xrGameSpyServer* pServer = (xrGameSpyServer*) userdata;
	if (!pServer) return;
	if (u32(index) >= pServer->client_Count()) return;
	CGameSpy_QR2* pQR2 = pServer->QR2();
	if (!pQR2) return;

	xrGameSpyClientData* pCD = NULL;
	
	if (pServer->IsDedicated())
	{
		if (u32(index+1) >= pServer->client_Count()) return;
		pCD = (xrGameSpyClientData*)pServer->client_Get(index+1);
	}
	else
		pCD = (xrGameSpyClientData*)pServer->client_Get(index);
	if (!pCD || !pCD->ps) return;

	switch (keyid)
	{
	case PLAYER__KEY:	pQR2->BufferAdd(outbuf, pCD->ps->getName()); break;
	case PING__KEY:		pQR2->BufferAdd_Int(outbuf, pCD->ps->ping); break;
	case SCORE__KEY:	pQR2->BufferAdd_Int(outbuf, pCD->ps->frags()); break;
	case DEATHS__KEY:	pQR2->BufferAdd_Int(outbuf, pCD->ps->m_iDeaths); break;
	case SKILL__KEY:	pQR2->BufferAdd_Int(outbuf, pCD->ps->rank); break;
	case TEAM__KEY:		pQR2->BufferAdd_Int(outbuf, pCD->ps->team); break;
	case P_SPECTATOR__KEY: pQR2->BufferAdd_Int(outbuf, pCD->ps->testFlag(GAME_PLAYER_FLAG_SPECTATOR)); break;
	case P_ARTEFACTS__KEY: 
		if (pServer->game->Type() == GAME_ARTEFACTHUNT) 
			pQR2->BufferAdd_Int(outbuf, pCD->ps->af_count); break;
			break;
	default:
		{
			pQR2->BufferAdd(outbuf, "");
		}break;
	}
};

void __cdecl callback_teamkey(int keyid, int index, void* outbuf, void *userdata)
{
	xrGameSpyServer* pServer = (xrGameSpyServer*) userdata;
	if (!pServer) return;
	
	CGameSpy_QR2* pQR2 = pServer->QR2();
	if (!pQR2) return;

	game_sv_Deathmatch* gmDM = smart_cast<game_sv_Deathmatch*>(pServer->game);
	if (!gmDM || u32(index) >= gmDM->GetNumTeams()) return;

	switch (keyid)
	{
	case T_SCORE_T_KEY:	if (gmDM) pQR2->BufferAdd_Int(outbuf, gmDM->GetTeamScore(index)); break;
	default:
		{
			pQR2->BufferAdd(outbuf, "");
		}break;
	};
};

void __cdecl callback_keylist(qr2_key_type keytype, void* keybuffer, void *userdata)
{
	if (!userdata) return;
	xrGameSpyServer* pServer = (xrGameSpyServer*) userdata;
	CGameSpy_QR2* pQR2 = pServer->QR2();
	if (!pQR2) return;
	
	switch (keytype)
	{
	case key_server: 
		{
			pQR2->KeyBufferAdd(keybuffer, HOSTNAME_KEY);
			pQR2->KeyBufferAdd(keybuffer, MAPNAME_KEY);
			pQR2->KeyBufferAdd(keybuffer, GAMEVER_KEY);
			pQR2->KeyBufferAdd(keybuffer, NUMPLAYERS_KEY);		
			pQR2->KeyBufferAdd(keybuffer, MAXPLAYERS_KEY);

			pQR2->KeyBufferAdd(keybuffer, GAMETYPE_KEY);
			pQR2->KeyBufferAdd(keybuffer, PASSWORD_KEY);
			pQR2->KeyBufferAdd(keybuffer, G_USER_PASSWORD_KEY);//user

			pQR2->KeyBufferAdd(keybuffer, HOSTPORT_KEY);
			
			//---- Game Keys
			pQR2->KeyBufferAdd(keybuffer, DEDICATED_KEY);
			pQR2->KeyBufferAdd(keybuffer, GAMETYPE_NAME_KEY);
			pQR2->KeyBufferAdd(keybuffer, NUMTEAMS_KEY);
			pQR2->KeyBufferAdd(keybuffer, G_MAX_PING_KEY);
#ifdef BATTLEYE
			pQR2->KeyBufferAdd(keybuffer, G_BATTLEYE_KEY);
#endif // BATTLEYE
			//---- game_sv_base ---
			pQR2->KeyBufferAdd(keybuffer, G_MAP_ROTATION_KEY);
			pQR2->KeyBufferAdd(keybuffer, G_VOTING_ENABLED_KEY);
			//---- game sv mp ----
			pQR2->KeyBufferAdd(keybuffer, G_SPECTATOR_MODES_KEY);
			//---- game_sv_deathmatch ----
			pQR2->KeyBufferAdd(keybuffer, G_FRAG_LIMIT_KEY);				
			pQR2->KeyBufferAdd(keybuffer, G_TIME_LIMIT_KEY);				
			pQR2->KeyBufferAdd(keybuffer, G_DAMAGE_BLOCK_TIME_KEY);			
			pQR2->KeyBufferAdd(keybuffer, G_DAMAGE_BLOCK_INDICATOR_KEY);
			pQR2->KeyBufferAdd(keybuffer, G_ANOMALIES_ENABLED_KEY);		
			pQR2->KeyBufferAdd(keybuffer, G_ANOMALIES_TIME_KEY);		
			pQR2->KeyBufferAdd(keybuffer, G_WARM_UP_TIME_KEY);			
			pQR2->KeyBufferAdd(keybuffer, G_FORCE_RESPAWN_KEY);			
			//---- game_sv_teamdeathmatch ----
			pQR2->KeyBufferAdd(keybuffer, G_AUTO_TEAM_BALANCE_KEY);		
			pQR2->KeyBufferAdd(keybuffer, G_AUTO_TEAM_SWAP_KEY);		
			pQR2->KeyBufferAdd(keybuffer, G_FRIENDLY_INDICATORS_KEY);	
			pQR2->KeyBufferAdd(keybuffer, G_FRIENDLY_NAMES_KEY);		
			pQR2->KeyBufferAdd(keybuffer, G_FRIENDLY_FIRE_KEY);			
			//---- game_sv_artefacthunt ----	
			pQR2->KeyBufferAdd(keybuffer, G_ARTEFACTS_COUNT_KEY);		
			pQR2->KeyBufferAdd(keybuffer, G_ARTEFACT_STAY_TIME_KEY);	
			pQR2->KeyBufferAdd(keybuffer, G_ARTEFACT_RESPAWN_TIME_KEY);	
			pQR2->KeyBufferAdd(keybuffer, G_REINFORCEMENT_KEY);			
			pQR2->KeyBufferAdd(keybuffer, G_SHIELDED_BASES_KEY);		
			pQR2->KeyBufferAdd(keybuffer, G_RETURN_PLAYERS_KEY);		
			pQR2->KeyBufferAdd(keybuffer, G_BEARER_CANT_SPRINT_KEY);
		} break;
	case key_player:
		{
//			pQR2->KeyBufferAdd(keybuffer, P_NAME__KEY);
//			pQR2->KeyBufferAdd(keybuffer, P_FRAGS__KEY);
//			pQR2->KeyBufferAdd(keybuffer, P_DEATH__KEY);
//			pQR2->KeyBufferAdd(keybuffer, P_RANK__KEY);
//			pQR2->KeyBufferAdd(keybuffer, P_TEAM__KEY);
			pQR2->KeyBufferAdd(keybuffer, PLAYER__KEY);
			pQR2->KeyBufferAdd(keybuffer, SCORE__KEY);
			pQR2->KeyBufferAdd(keybuffer, DEATHS__KEY);
			pQR2->KeyBufferAdd(keybuffer, SKILL__KEY);
			pQR2->KeyBufferAdd(keybuffer, TEAM__KEY);
			pQR2->KeyBufferAdd(keybuffer, P_SPECTATOR__KEY);
			pQR2->KeyBufferAdd(keybuffer, P_ARTEFACTS__KEY);		
		}break;
	case key_team:
		{
			pQR2->KeyBufferAdd(keybuffer, T_SCORE_T_KEY);
		}break;
	};

	//GSI_UNUSED(userdata);
};

int __cdecl callback_count(qr2_key_type keytype, void *userdata)
{
	if (!userdata) return 0;
	xrGameSpyServer* pServer = (xrGameSpyServer*) userdata;
	switch (keytype)
	{
	case key_player:
		{
			return pServer->GetPlayersCount();
		}break;
	case key_team:
		{
			if (!pServer->game) return 0;
			switch (pServer->game->Type())
			{
			case GAME_DEATHMATCH:
				return 1;
			case GAME_TEAMDEATHMATCH:
				return 2;
			case GAME_ARTEFACTHUNT:
				return 2;
			}
		}break;
	default:
		return 0;
	}

	//GSI_UNUSED(userdata);

	return 0;
};

void __cdecl callback_adderror(qr2_error_t error, char *errmsg, void *userdata)
{
	Msg("! Error while adding this server to master list ->%s.", errmsg);
	xrGameSpyServer* pServer = (xrGameSpyServer*) userdata;
	if (pServer) pServer->OnError_Add(error);
	
};

void __cdecl callback_nn(int cookie, void *userdata)
{
};

void __cdecl callback_cm(char *data, int len, void *userdata)
{
};

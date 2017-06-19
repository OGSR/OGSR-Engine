#include "stdafx.h"
#include "game_sv_mp.h"
#include "xrServer.h"
#include "xrMessages.h"
#include "xrServer_Object_Base.h"
#include "xrServer_Objects.h"
#include "level.h"
#include "xrserver_objects_alife_monsters.h"
#include "actor.h"
#include "clsid_game.h"
#include "../XR_IOConsole.h"
#include "../igame_persistent.h"
#include "date_time.h"
#include "game_cl_base.h"
#include "Spectator.h"
#include "game_cl_base_weapon_usage_statistic.h"
#include "xrGameSpyServer.h"

#include "game_sv_mp_vote_flags.h"

u32		g_dwMaxCorpses = 10;
//-----------------------------------------------------------------
BOOL		g_sv_mp_bSpectator_FreeFly		= FALSE;
BOOL		g_sv_mp_bSpectator_FirstEye		= TRUE;
BOOL		g_sv_mp_bSpectator_LookAt		= TRUE;
BOOL		g_sv_mp_bSpectator_FreeLook		= TRUE;
BOOL		g_sv_mp_bSpectator_TeamCamera	= TRUE;
int			g_sv_mp_iDumpStatsPeriod		= 0;
int			g_sv_mp_iDumpStats_last			= 0;
BOOL		g_sv_mp_bCountParticipants		= FALSE;
float		g_sv_mp_fVoteQuota			= VOTE_QUOTA;
float		g_sv_mp_fVoteTime				= VOTE_LENGTH_TIME;
//-----------------------------------------------------------------

extern xr_token	round_end_result_str[];

#include "ui\UIBuyWndShared.h"

game_sv_mp::game_sv_mp() :inherited()
{
	m_strWeaponsData		= xr_new<CItemMgr>();
	m_bVotingActive = false;	
	//------------------------------------------------------
//	g_pGamePersistent->Environment().SetWeather("mp_weather");
	m_aRanks.clear();	
	//------------------------------------------------------	
	round_end_reason = eRoundEnd_Force; //unknown
}

game_sv_mp::~game_sv_mp()
{
	xr_delete(m_strWeaponsData);
}

void	game_sv_mp::Update	()
{
	inherited::Update();

	// remove corpses if their number exceed limit
	for (u32 i=0; i<m_CorpseList.size(); )
	{
		if (m_CorpseList.size() <= g_dwMaxCorpses) break;

		u16 CorpseID				= m_CorpseList[i];
		
		CSE_Abstract* pCorpseObj	= get_entity_from_eid(CorpseID);
		
		if (!pCorpseObj)
		{
			m_CorpseList.erase(m_CorpseList.begin() + i);
			Msg("corpse [%d] not found [%d]",CorpseID, Device.dwFrame);
			continue;
		}
		if (!pCorpseObj->children.empty())
		{
			Msg("corpse [%d] childern not empty [%d]",CorpseID, Device.dwFrame);
			i++;
			continue;
		}

		//---------------------------------------------
		NET_Packet			P;
		u_EventGen			(P,GE_DESTROY,CorpseID);
		Level().Send		(P,net_flags(TRUE,TRUE));
		m_CorpseList.erase	(m_CorpseList.begin() + i);
		Msg("corpse [%d] send destroy [%d]",CorpseID, Device.dwFrame);
	}

	if (IsVotingEnabled() && IsVotingActive()) UpdateVote();
	//-------------------------------------------------------
	UpdatePlayersMoney();

	if(g_sv_mp_iDumpStatsPeriod)
	{
		int curr_minutes = iFloor(Device.fTimeGlobal/60.0f);
		if(g_sv_mp_iDumpStats_last+g_sv_mp_iDumpStatsPeriod <= curr_minutes )
		{
			if(Phase()==GAME_PHASE_INPROGRESS)
			{
				DumpOnlineStatistic();
				g_sv_mp_iDumpStats_last	= curr_minutes;
			}
		}
	}
}

void game_sv_mp::OnRoundStart()
{
	inherited::OnRoundStart();
	
	if( g_pGameLevel && Level().game )
	{
		Game().m_WeaponUsageStatistic->Clear();
	}
	
	m_CorpseList.clear();

	switch_Phase		(GAME_PHASE_INPROGRESS);
	++m_round;
	m_round_start_time	= Level().timeServer();
	timestamp			(m_round_start_time_str);

	// clear "ready" flag
	u32		cnt		= get_players_count	();
	for		(u32 it=0; it<cnt; ++it)	
	{
		game_PlayerState*	ps	=	get_it	(it);
		ps->resetFlag(GAME_PLAYER_FLAG_READY+GAME_PLAYER_FLAG_VERY_VERY_DEAD);
		ps->m_online_time = Level().timeServer();
	};

	// 1. We have to destroy all player-entities and entities
	m_server->SLS_Clear	();

	// 2. We have to create them at respawn points and/or specified positions
	m_server->SLS_Default	();

	//send "RoundStarted" Message To Allclients
	NET_Packet			P;
//	P.w_begin			(M_GAMEMESSAGE);
	GenerateGameMessage (P);
	P.w_u32				(GAME_EVENT_ROUND_STARTED);
	u_EventSend(P);

	signal_Syncronize();
}

void game_sv_mp::OnRoundEnd()
{
	inherited::OnRoundEnd();

	string64 res_str;
	strcpy_s( res_str, get_token_name( round_end_result_str, round_end_reason ) );
	
	OnVoteStop();

	switch_Phase		(GAME_PHASE_PENDING);
	//send "RoundOver" Message To All clients
	NET_Packet			P;
//	P.w_begin			(M_GAMEMESSAGE);
	GenerateGameMessage (P);
	P.w_u32				(GAME_EVENT_ROUND_END);
	P.w_stringZ			(res_str);
	u_EventSend(P);
	//-------------------------------------------------------
}


void	game_sv_mp::KillPlayer				(ClientID id_who, u16 GameID)
{
	CObject* pObject =  Level().Objects.net_Find(GameID);
	if (!pObject || pObject->CLS_ID != CLSID_OBJECT_ACTOR) return;
	// Remove everything	
	xrClientData* xrCData	=	m_server->ID_to_client(id_who);
	
	if (xrCData && xrCData->ps->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD)) return;
	if (xrCData) 
	{
		//-------------------------------------------------------
		OnPlayerKillPlayer(xrCData->ps, xrCData->ps, KT_HIT, SKT_NONE, NULL);
		xrCData->ps->m_bClearRun = false;
	};
	//-------------------------------------------------------
	CActor* pActor = smart_cast <CActor*>(pObject);
	if (pActor)
	{
		if (!pActor->g_Alive())
		{
			return;
		}
		pActor->set_death_time		();
		pActor->m_bAllowDeathRemove = true;
		m_CorpseList.push_back(pActor->ID());
	}
	//-------------------------------------------------------
	u16 PlayerID = (xrCData != 0) ? xrCData->ps->GameID : GameID;
	//-------------------------------------------------------
	SendPlayerKilledMessage(PlayerID, KT_HIT, PlayerID, 0, SKT_NONE);
	//-------------------------------------------------------
	// Kill Player on all clients
	NET_Packet			P;
	u_EventGen(P, GE_DIE, PlayerID);
	P.w_u16				(PlayerID);
	P.w_clientID		(id_who);

	u_EventSend(P, net_flags(TRUE, TRUE, FALSE, TRUE));
	
	if (xrCData) SetPlayersDefItems		(xrCData->ps);
	signal_Syncronize();
	//-------------------------------------------------------	
	
};


void	game_sv_mp::OnEvent (NET_Packet &P, u16 type, u32 time, ClientID sender )
{
	switch	(type)
	{	
	case GAME_EVENT_PLAYER_KILLED:  //playerKillPlayer
		{
			OnPlayerKilled(P);
		}break;
	case GAME_EVENT_PLAYER_HITTED:
		{
			OnPlayerHitted(P);			
		}break;
	case GAME_EVENT_PLAYER_READY:// cs & dm 
		{
			xrClientData *l_pC = m_server->ID_to_client(sender);
			if (!l_pC) break;
			OnPlayerReady		(l_pC->ID);
		}break;
	case GAME_EVENT_PLAYER_BUY_SPAWN:
		{
			xrClientData *l_pC = m_server->ID_to_client(sender);
			if (!l_pC) break;
			OnPlayerBuySpawn	(l_pC->ID);
		}break;
	case GAME_EVENT_VOTE_START:
		{
			if (!IsVotingEnabled()) break;
			string1024 VoteCommand;
			P.r_stringZ(VoteCommand);
			OnVoteStart(VoteCommand, sender);
		}break;
	case GAME_EVENT_VOTE_YES:
		{
			if (!IsVotingEnabled()) break;
			OnVoteYes(sender);
		}break;
	case GAME_EVENT_VOTE_NO:
		{
			if (!IsVotingEnabled()) break;
			OnVoteNo(sender);
		}break;
	case GAME_EVENT_PLAYER_NAME:
		{
			OnPlayerChangeName(P, sender);
		}break;
	case GAME_EVENT_SPEECH_MESSAGE:
		{
			OnPlayerSpeechMessage(P, sender);
		}break;
	case GAME_EVENT_PLAYER_GAME_MENU:
		{
			OnPlayerGameMenu(P, sender);
//			OnPlayerSelectSpectator(P, sender);
		}break;
	default:
		inherited::OnEvent(P, type, time, sender);
	};//switch

}

bool g_bConsoleCommandsCreated = false;
extern	float	g_fTimeFactor;
void game_sv_mp::Create (shared_str &options)
{
	SetVotingActive(false);
	inherited::Create(options);
	//-------------------------------------------------------------------	
	if (!g_bConsoleCommandsCreated)
	{
		g_bConsoleCommandsCreated = true;
	}
	
	//------------------------------------------------------------------
	LoadRanks();
	//------------------------------------------------------------------
	Set_RankUp_Allowed(false);
};

u8	game_sv_mp::SpectatorModes_Pack		()
{
	u8 res = 0;
	
	res |= g_sv_mp_bSpectator_FreeFly	  ? (1<<CSpectator::eacFreeFly	) : 0;
	res |= g_sv_mp_bSpectator_FirstEye	  ? (1<<CSpectator::eacFirstEye	) : 0;
	res |= g_sv_mp_bSpectator_LookAt	  ? (1<<CSpectator::eacLookAt	) : 0;
	res |= g_sv_mp_bSpectator_FreeLook	  ? (1<<CSpectator::eacFreeLook	) : 0;
	res |= g_sv_mp_bSpectator_TeamCamera	? (1<<CSpectator::eacMaxCam	) : 0;	
	return res;
}

void game_sv_mp::SpectatorModes_UnPack		(u8 SpectrModesPacked)
{
	g_sv_mp_bSpectator_FreeFly	= (SpectrModesPacked & (1<<CSpectator::eacFreeFly	)) != 0;
	g_sv_mp_bSpectator_FirstEye	= (SpectrModesPacked & (1<<CSpectator::eacFirstEye	)) != 0;
	g_sv_mp_bSpectator_LookAt		= (SpectrModesPacked & (1<<CSpectator::eacLookAt	)) != 0;
	g_sv_mp_bSpectator_FreeLook	= (SpectrModesPacked & (1<<CSpectator::eacFreeLook	)) != 0;
	g_sv_mp_bSpectator_TeamCamera = (SpectrModesPacked & (1<<CSpectator::eacMaxCam	)) != 0;	
};

void game_sv_mp::net_Export_State		(NET_Packet& P, ClientID id_to)
{
	inherited::net_Export_State(P, id_to);
	//-------------------------------------
	m_u8SpectatorModes = SpectatorModes_Pack();	

	P.w_u8	(m_u8SpectatorModes);
};

void	game_sv_mp::RespawnPlayer			(ClientID id_who, bool NoSpectator)
{
	//------------------------------------------------------------
	xrClientData* xrCData	=	m_server->ID_to_client(id_who);
	if (!xrCData || !xrCData->owner) return;
//	game_PlayerState*	ps	=	&(xrCData->ps);
	CSE_Abstract* pOwner = xrCData->owner;
	CSE_ALifeCreatureActor	*pA	=	smart_cast<CSE_ALifeCreatureActor*>(pOwner);
	CSE_Spectator			*pS =	smart_cast<CSE_Spectator*>(pOwner);

	if (pA)
	{
		//------------------------------------------------------------			
		AllowDeadBodyRemove(id_who, xrCData->ps->GameID);
		//------------------------------------------------------------
		m_CorpseList.push_back(pOwner->ID);
		//------------------------------------------------------------
	};

	if (pA && !NoSpectator)
	{
		//------------------------------------------------------------
		SpawnPlayer(id_who, "spectator");
		//------------------------------------------------------------
	}
	else
	{
		//------------------------------------------------------------
		if (pOwner->owner != m_server->GetServerClient())
		{
			pOwner->owner = (xrClientData*)m_server->GetServerClient();
		};
		//------------------------------------------------------------
		//remove spectator entity
		if (pS)
		{
			NET_Packet			P;
			u_EventGen			(P,GE_DESTROY,pS->ID);
			Level().Send(P,net_flags(TRUE,TRUE));
		};
		//------------------------------------------------------------
		SpawnPlayer(id_who, "mp_actor");
		//------------------------------------------------------------
//		SpawnWeaponsForActor(xrCData->owner, ps);
		//------------------------------------------------------------
	};
};


void	game_sv_mp::SpawnPlayer(ClientID id, LPCSTR N)
{
	xrClientData* CL	= m_server->ID_to_client(id);
	//-------------------------------------------------
	CL->net_PassUpdates = TRUE;
	//-------------------------------------------------
	game_PlayerState*	ps_who	=	CL->ps;
	ps_who->setFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD);
	
	// Spawn "actor"
	CSE_Abstract*	E				=	spawn_begin	(N);													// create SE
	
	E->set_name_replace		( get_name_id(id) );					// name

	E->s_flags.assign		(M_SPAWN_OBJECT_LOCAL | M_SPAWN_OBJECT_ASPLAYER);	// flags

	CSE_ALifeCreatureActor	*pA	=	smart_cast<CSE_ALifeCreatureActor*>(E);
	CSE_Spectator			*pS	=	smart_cast<CSE_Spectator*>(E);

	R_ASSERT2	(pA || pS,"Respawned Client is not Actor nor Spectator");
	
	if (pA) 
	{
		pA->s_team				=	u8(ps_who->team);
		assign_RP				(pA, ps_who);
		SetSkin					(E, pA->s_team, ps_who->skin);
		ps_who->resetFlag		(GAME_PLAYER_FLAG_VERY_VERY_DEAD);

		if (!ps_who->RespawnTime)
			OnPlayerEnteredGame(id);

		ps_who->RespawnTime = Device.dwTimeGlobal;

		Game().m_WeaponUsageStatistic->OnPlayerSpawned(ps_who);
	}
	else
		if (pS)
		{
			Fvector Pos, Angle;
			if (!GetPosAngleFromActor(id, Pos, Angle)) assign_RP				(E, ps_who);
			else
			{
				E->o_Angle.set(Angle);
				E->o_Position.set(Pos);				
			}
		};
	
	Msg		("* %s respawned as %s", get_name_id(id) , (0 == pA) ? "spectator" : "actor");
	spawn_end				(E,id);

	ps_who->SetGameID(CL->owner->ID);

	signal_Syncronize();
}

void game_sv_mp::AllowDeadBodyRemove(ClientID id, u16 GameID)
{
	CSE_Abstract* pSObject = get_entity_from_eid(GameID);

	if (pSObject)
		pSObject->owner = (xrClientData*)m_server->GetServerClient();

	CObject* pObject =  Level().Objects.net_Find(GameID);
	

	if (pObject && pObject->CLS_ID == CLSID_OBJECT_ACTOR)
	{
		CActor* pActor = smart_cast <CActor*>(pObject);
		if (pActor)
		{
			pActor->set_death_time		();
			pActor->m_bAllowDeathRemove = true;
		};
	};	
};

void game_sv_mp::OnPlayerConnect			(ClientID id_who)
{
	inherited::OnPlayerConnect (id_who);
}

void game_sv_mp::OnPlayerDisconnect		(ClientID id_who, LPSTR Name, u16 GameID)
{
	//---------------------------------------------------
	NET_Packet					P;
	GenerateGameMessage			(P);
	P.w_u32						(GAME_EVENT_PLAYER_DISCONNECTED);
	P.w_stringZ					(Name);
	u_EventSend					(P);
	//---------------------------------------------------
	KillPlayer					(id_who, GameID);
	
	AllowDeadBodyRemove			(id_who, GameID);
	m_CorpseList.push_back		(GameID);

	inherited::OnPlayerDisconnect (id_who, Name, GameID);
}

void	game_sv_mp::SetSkin					(CSE_Abstract* E, u16 Team, u16 ID)
{
	if (!E) return;
	//-------------------------------------------
	CSE_Visual* pV = smart_cast<CSE_Visual*>(E);
	if (!pV) return;
	//-------------------------------------------
	string256 SkinName;
	std::strcpy(SkinName, pSettings->r_string("mp_skins_path", "skin_path"));
	//загружены ли скины для этой комманды

	if (!TeamList.empty()	&&
		TeamList.size() > Team	&&
		!TeamList[Team].aSkins.empty())
	{
		//загружено ли достаточно скинов для этой комманды
		if (TeamList[Team].aSkins.size() > ID)
		{
			std::strcat(SkinName, TeamList[Team].aSkins[ID].c_str());
		}
		else
			std::strcat(SkinName, TeamList[Team].aSkins[0].c_str());
	}
	else
	{
		//скины для такой комманды не загружены
		switch (Team)
		{
		case 0:
			std::strcat(SkinName, "stalker_hood_multiplayer");
			break;
		case 1:
			std::strcat(SkinName, "soldat_beret");
			break;
		case 2:
			std::strcat(SkinName, "stalker_black_mask");
			break;
		default:
			R_ASSERT2(0,"Unknown Team");
			break;
		};
	};
	std::strcat(SkinName, ".ogf");
//.	Msg("* Skin - %s", SkinName);
	int len = xr_strlen(SkinName);
	R_ASSERT2(len < 64, "Skin Name is too LONG!!!");
	pV->set_visual(SkinName);
	//-------------------------------------------
};

#include "../CameraBase.h"

bool	game_sv_mp::GetPosAngleFromActor				(ClientID id, Fvector& Pos, Fvector &Angle)
{
	xrClientData* xrCData	=	m_server->ID_to_client(id);
	if (!xrCData || !xrCData->owner) return false;
	
	CObject* pObject =  Level().Objects.net_Find(xrCData->owner->ID);
///	R_ASSERT2	((pObject && pObject->CLS_ID == CLSID_OBJECT_ACTOR),"Dead Player is not Actor");

	if (!pObject || pObject->CLS_ID != CLSID_OBJECT_ACTOR) return false;

	CActor* pActor = smart_cast <CActor*>(pObject);
	if (!pActor) return false;

	Angle.set(-pActor->cam_Active()->pitch, -pActor->cam_Active()->yaw, 0);
	Pos.set(pActor->cam_Active()->vPosition);
	return true;
};

TeamStruct* game_sv_mp::GetTeamData				(u32 Team)
{
	VERIFY(TeamList.size());
	if (TeamList.empty()) return NULL;
	
	VERIFY(TeamList.size()>Team);
	if (TeamList.size()<=Team) return NULL;

	return &(TeamList[Team]);
};

void	game_sv_mp::SpawnWeaponForActor		(u16 actorId,  LPCSTR N, bool isScope, bool isGrenadeLauncher, bool isSilencer)
{
		u8 addon_flags = 0;
		if(isScope)
			addon_flags |= CSE_ALifeItemWeapon::eWeaponAddonScope;

		if(isGrenadeLauncher)
			addon_flags |= CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher;

		if(isSilencer)
			addon_flags |= CSE_ALifeItemWeapon::eWeaponAddonSilencer;

		SpawnWeapon4Actor(actorId, N, addon_flags);
}

void	game_sv_mp::SpawnWeapon4Actor		(u16 actorId,  LPCSTR N, u8 Addons)
{
	if (!N) return;
	
	CSE_Abstract			*E	=	spawn_begin	(N);
	E->ID_Parent = actorId;

	E->s_flags.assign		(M_SPAWN_OBJECT_LOCAL);	// flags
	/////////////////////////////////////////////////////////////////////////////////
	//если это оружие - спавним его с полным магазином
	CSE_ALifeItemWeapon		*pWeapon	=	smart_cast<CSE_ALifeItemWeapon*>(E);
	if (pWeapon)
	{
		pWeapon->a_elapsed = pWeapon->get_ammo_magsize();

		pWeapon->m_addon_flags.assign(Addons);
	};
	/////////////////////////////////////////////////////////////////////////////////

	spawn_end				(E,m_server->GetServerClient()->ID);
};

void game_sv_mp::OnDestroyObject			(u16 eid_who)
{
	CORPSE_LIST_it it = std::find(m_CorpseList.begin(), m_CorpseList.end(), eid_who);
	if (it != m_CorpseList.end())
	{
		m_CorpseList.erase(it);
	};
};

bool game_sv_mp::OnNextMap				()
{
	if (!m_bMapRotation) return false;
	Msg("m_bMapSwitched - %s", m_bMapSwitched ? "true" : "false");
	if (m_bMapSwitched) return false;
	if (!m_pMapRotation_List.size()) return false;

	xr_string MapName = m_pMapRotation_List.front();
	m_pMapRotation_List.pop_front();
	m_pMapRotation_List.push_back(MapName);

	MapName = m_pMapRotation_List.front();

	Msg("Going to level %s", MapName.c_str());
	m_bMapSwitched = true;

	/*
	if (!stricmp(MapName.c_str(), Level().name().c_str()))
	{
		m_bMapSwitched		= false;
		return				false;
	}*/
	string1024 Command;
	sprintf_s(Command, "sv_changelevel %s", MapName.c_str());
	Console->Execute(Command);
	return true;
};

void game_sv_mp::OnPrevMap				()
{
	if (!m_bMapRotation) return;
	Msg("m_bMapSwitched - %s", m_bMapSwitched ? "true" : "false");
	if (m_bMapSwitched) return;
	if (!m_pMapRotation_List.size()) return;
	
	xr_string MapName = m_pMapRotation_List.back();
	m_pMapRotation_List.pop_back();
	m_pMapRotation_List.push_front(MapName);

	Msg("Goint to level %s", MapName.c_str());
	m_bMapSwitched = true;

//.	if (!stricmp(MapName.c_str(), Level().name().c_str())) return;

	string1024	Command;
	sprintf_s(Command, "sv_changelevel %s", MapName.c_str());
	Console->Execute(Command);
};

struct _votecommands		{
	char*	name;
	char*	command;
	u16		flag;	
};

_votecommands	votecommands[] = {
	{ "restart",		"g_restart"	,				flVoteRestart},
	{ "restart_fast",	"g_restart_fast",			flVoteRestartFast	},
	{ "kick",			"sv_kick",					flVoteKick			},
	{ "ban",			"sv_banplayer",				flVoteBan			},
	{ "changemap",		"sv_changelevel",			flVoteMap			},
	{ "changeweather",	"sv_setenvtime",			flVoteWeather		},
	{ NULL, 			NULL }
};

void game_sv_mp::OnVoteStart				(LPCSTR VoteCommand, ClientID sender)
{
	if (!IsVotingEnabled()) return;
	char	CommandName[256];	CommandName[0]=0;
	char	CommandParams[256];	CommandParams[0]=0;
	string1024 resVoteCommand = "";
	sscanf	(VoteCommand,"%s ", CommandName);
	if (xr_strlen(CommandName)+1 < xr_strlen(VoteCommand))
	{
		strcpy(CommandParams, VoteCommand + xr_strlen(CommandName)+1);
	}

	if (CommandName[0] == '$' && !IsVotingEnabled(flVoteText))
		return;

	int i=0;
	m_bVotingReal = false;
	while (votecommands[i].command)
	{
		if (!stricmp(votecommands[i].name, CommandName))
		{
			m_bVotingReal = true;
			if (!IsVotingEnabled(votecommands[i].flag))
				return;
			break;
		};
		i++;
	};
	if (!m_bVotingReal && CommandName[0] != '$') 
	{
		Msg("Unknown Vote Command - %s", CommandName);
		return;
	};

	//-----------------------------------------------------------------------------
	SetVotingActive(true);
	u32 CurTime = Level().timeServer();
	m_uVoteStartTime = CurTime;
	if (m_bVotingReal)
	{
		if (!stricmp(votecommands[i].name, "changeweather"))
		{
			string256 WeatherTime = "", WeatherName = "";
			sscanf(CommandParams, "%s %s", WeatherName, WeatherTime );

			m_pVoteCommand.sprintf("%s %s", votecommands[i].command, WeatherTime);
			sprintf_s(resVoteCommand, "%s %s", votecommands[i].name, WeatherName);
		}
		else
		{
			m_pVoteCommand.sprintf("%s %s", votecommands[i].command, CommandParams);
			strcpy(resVoteCommand, VoteCommand);
		}		
	}
	else
	{
		m_pVoteCommand.sprintf("%s", VoteCommand+1);
	};

	xrClientData *pStartedPlayer = NULL;
	u32	cnt = get_players_count();	
	for(u32 it=0; it<cnt; it++)	
	{
		xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
		if (!l_pC) continue;
		if (l_pC->ID == sender)
		{
			l_pC->ps->m_bCurrentVoteAgreed = 1;
			pStartedPlayer = l_pC;
		}
		else
			l_pC->ps->m_bCurrentVoteAgreed = 2;
	};

	signal_Syncronize();
	//-----------------------------------------------------------------------------
	NET_Packet P;
	GenerateGameMessage (P);
	P.w_u32(GAME_EVENT_VOTE_START);
	if (m_bVotingReal)
		P.w_stringZ(resVoteCommand);
	else
		P.w_stringZ(VoteCommand+1);
	P.w_stringZ(pStartedPlayer ? pStartedPlayer->ps->getName() : "");
	P.w_u32(u32(g_sv_mp_fVoteTime*60000));
	u_EventSend(P);
	//-----------------------------------------------------------------------------	
};


void		game_sv_mp::UpdateVote				()
{
	if (!IsVotingEnabled() || !IsVotingActive()) return;

	u32 NumAgreed = 0;
	u32 NumParticipated = 0;
	u32 NumToCount = 0;
	u32	cnt = get_players_count();	
	for(u32 it=0; it<cnt; it++)	
	{
		xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
		game_PlayerState* ps	= l_pC->ps;
		if (!l_pC || !l_pC->net_Ready || !ps || ps->IsSkip()) continue;
		if (ps->m_bCurrentVoteAgreed != 2) NumParticipated++;
		if (ps->m_bCurrentVoteAgreed == 1) NumAgreed++;
		NumToCount++;
	};

	bool VoteSucceed = false;
	u32 CurTime = Level().timeServer();
	
	if (m_uVoteStartTime + u32(g_sv_mp_fVoteTime*60000) > CurTime)
	{
		if (NumToCount == NumAgreed) VoteSucceed = true;
		else
			VoteSucceed = (float(NumAgreed)/float(NumToCount)) >= g_sv_mp_fVoteQuota;
		if (!VoteSucceed) return;
	}
	else
	{
		if (g_sv_mp_bCountParticipants) 
			VoteSucceed = (float(NumAgreed)/float(NumParticipated)) >= g_sv_mp_fVoteQuota;
		else
			VoteSucceed = (float(NumAgreed)/float(NumToCount)) >= g_sv_mp_fVoteQuota;
	};

	SetVotingActive(false);

	if (!VoteSucceed) 
	{
		NET_Packet P;
		GenerateGameMessage (P);
		P.w_u32(GAME_EVENT_VOTE_END);
		P.w_stringZ("st_mp_voting_failed");
		u_EventSend(P);
		return;
	};

	NET_Packet P;
	GenerateGameMessage (P);
	P.w_u32(GAME_EVENT_VOTE_END);
	P.w_stringZ("st_mp_voting_succeed");
	u_EventSend(P);

	if (m_bVotingReal)
		Console->Execute(m_pVoteCommand.c_str());
};


void		game_sv_mp::OnVoteYes				(ClientID sender)
{
	game_PlayerState* ps = get_id(sender);
	if (!ps) return;
	ps->m_bCurrentVoteAgreed = 1;
	signal_Syncronize();
};

void		game_sv_mp::OnVoteNo				(ClientID sender)
{
	game_PlayerState* ps = get_id(sender);
	if (!ps) return;
	ps->m_bCurrentVoteAgreed = 0;
	signal_Syncronize();
};

void		game_sv_mp::OnVoteStop				()
{
	if (!IsVotingActive()) return;
	SetVotingActive(false);
	//-----------------------------------------------------------------
	NET_Packet P;
	GenerateGameMessage (P);
	P.w_u32(GAME_EVENT_VOTE_STOP);
	u_EventSend(P);
	//-----------------------------------------------------------------
	signal_Syncronize();
};

void		game_sv_mp::OnPlayerEnteredGame		(ClientID id_who)
{
	xrClientData* xrCData	=	m_server->ID_to_client(id_who);
	if (!xrCData) return;

	NET_Packet			P;
	GenerateGameMessage (P);
	P.w_u32				(GAME_EVENT_PLAYER_ENTERED_GAME);
	P.w_stringZ			( xrCData->name.c_str() );
	u_EventSend(P);
};

void	game_sv_mp::ClearPlayerItems		(game_PlayerState* ps)
{
	ps->pItemList.clear();
	ps->LastBuyAcount = 0;
//	ps->m_bClearRun = false;
};

void	game_sv_mp::SetPlayersDefItems		(game_PlayerState* ps)
{
	ClearPlayerItems(ps);
	if (ps->team<0) return;
	//-------------------------------------------
	//fill player with default items
	if (ps->team < s16(TeamList.size()))
	{
		DEF_ITEMS_LIST	aDefItems = TeamList[ps->team].aDefaultItems;

		for (u16 i=0; i<aDefItems.size(); i++)
		{
			ps->pItemList.push_back(aDefItems[i]);
		}
	};
	//---------------------------------------------------
	string16 RankStr;
	string256 ItemStr;
	string256 NewItemStr;
	char tmp[5];
	for (int i=1; i<=ps->rank; i++)
	{
		strconcat(sizeof(RankStr),RankStr,"rank_",itoa(i,tmp,10));
		if (!pSettings->section_exist(RankStr)) continue;
		for (u32 it=0; it<ps->pItemList.size(); it++)
		{
			u16* pItemID = &(ps->pItemList[it]);
//			WeaponDataStruct* pWpnS = NULL;
//			if (!GetTeamItem_ByID(&pWpnS, &(TeamList[ps->team].aWeapons), *pItemID)) continue;
			if (m_strWeaponsData->GetItemsCount() <= *pItemID) continue;
			shared_str WeaponName = m_strWeaponsData->GetItemName((*pItemID) & 0x00FF);
//			strconcat(ItemStr, "def_item_repl_", pWpnS->WeaponName.c_str());
			strconcat(sizeof(ItemStr),ItemStr, "def_item_repl_", *WeaponName);
			if (!pSettings->line_exist(RankStr, ItemStr)) continue;
			
			strcpy_s(NewItemStr,sizeof(NewItemStr),pSettings->r_string(RankStr, ItemStr));
//			if (!GetTeamItem_ByName(&pWpnS, &(TeamList[ps->team].aWeapons), NewItemStr)) continue;
			if (m_strWeaponsData->GetItemIdx(NewItemStr) == u32(-1)) continue;

//			*pItemID = pWpnS->SlotItem_ID;
			*pItemID = u16(m_strWeaponsData->GetItemIdx(NewItemStr) & 0xffff);
		}
	}
	//---------------------------------------------------
	for (u32 it=0; it<ps->pItemList.size(); it++)
	{
		u16* pItemID = &(ps->pItemList[it]);
//		WeaponDataStruct* pWpnS = NULL;
//		if (!GetTeamItem_ByID(&pWpnS, &(TeamList[ps->team].aWeapons), *pItemID)) continue;
		if (m_strWeaponsData->GetItemsCount() <= *pItemID) continue;
		
		shared_str WeaponName = m_strWeaponsData->GetItemName((*pItemID) & 0x00FF);
		if (!xr_strcmp(*WeaponName, "mp_wpn_knife")) continue;
		u16 AmmoID = u16(-1);
		if (pSettings->line_exist(WeaponName, "ammo_class"))
		{
			string1024 wpnAmmos, BaseAmmoName;
			std::strcpy(wpnAmmos, pSettings->r_string(WeaponName, "ammo_class"));
			_GetItem(wpnAmmos, 0, BaseAmmoName);
			AmmoID = u16(m_strWeaponsData->GetItemIdx(BaseAmmoName)&0xffff);
		};
//		if (!pWpnS->WeaponBaseAmmo.size()) continue;
//		WeaponDataStruct* pWpnAmmo = NULL;
//		if (!GetTeamItem_ByName(&pWpnAmmo, &(TeamList[ps->team].aWeapons), *(pWpnS->WeaponBaseAmmo))) continue;
		if (AmmoID == u16(-1)) continue;
		
//		ps->pItemList.push_back(pWpnAmmo->SlotItem_ID);
//		ps->pItemList.push_back(pWpnAmmo->SlotItem_ID);
		if (Type() == GAME_ARTEFACTHUNT)
		{
			ps->pItemList.push_back(AmmoID);
			ps->pItemList.push_back(AmmoID);
		}		
	};
};

void game_sv_mp::ClearPlayerState(game_PlayerState* ps)
{
	if (!ps) return;

	ps->m_iRivalKills		= 0;
	ps->m_iSelfKills		= 0;
	ps->m_iTeamKills		= 0;
	ps->m_iDeaths			= 0;

	ps->m_iKillsInRowCurr	= 0;
	ps->m_iKillsInRowMax	= 0;

	ps->lasthitter			= 0;
	ps->lasthitweapon		= 0;

	ClearPlayerItems		(ps);
};

void game_sv_mp::OnPlayerKilled(NET_Packet P)
{
	u16 KilledID			= P.r_u16();
	KILL_TYPE KillType		= KILL_TYPE(P.r_u8());
	u16 KillerID			= P.r_u16();
	u16	WeaponID			= P.r_u16();
	SPECIAL_KILL_TYPE SpecialKill = SPECIAL_KILL_TYPE(P.r_u8());

	game_PlayerState* ps_killer = get_eid(KillerID);
	game_PlayerState* ps_killed = get_eid(KilledID);
	
	// in case of team kill and kick, we can't erase this message from queue.
	// the simplest solve is to return
	if (!ps_killed)	
	{
#ifdef DEBUG
		Msg("! ERROR: killed player [%d] state is NULL", KilledID);
#endif
		return;
	}
	CSE_Abstract* pWeaponA = get_entity_from_eid(WeaponID);

	OnPlayerKillPlayer		(ps_killer, ps_killed, KillType, SpecialKill, pWeaponA);
	//---------------------------------------------------
	SendPlayerKilledMessage	((ps_killed)?ps_killed->GameID:KilledID, KillType, (ps_killer)?ps_killer->GameID:KillerID, WeaponID, SpecialKill);
};

void game_sv_mp::OnPlayerHitted(NET_Packet P)
{
	u16		id_hitted = P.r_u16();
	u16     id_hitter = P.r_u16();
	float	dHealth = P.r_float()*100;
	game_PlayerState* PSHitter		=	get_eid			(id_hitter);
	if (!PSHitter) return;
	game_PlayerState* PSHitted		=	get_eid			(id_hitted);
	if (!PSHitted) return;
	if (PSHitted == PSHitter) return;
	if (!CheckTeams() || (PSHitted->team != PSHitter->team))
	{
		Rank_Struct* pCurRank = &(m_aRanks[PSHitter->rank]);
		Player_AddExperience(PSHitter, dHealth*pCurRank->m_aRankDiff_ExpBonus[PSHitted->rank]);
	};
};
	
void	game_sv_mp::SendPlayerKilledMessage	(u16 KilledID, KILL_TYPE KillType, u16 KillerID, u16 WeaponID, SPECIAL_KILL_TYPE SpecialKill)
{
	NET_Packet			P;
	GenerateGameMessage (P);
	P.w_u32				(GAME_EVENT_PLAYER_KILLED);

	P.w_u8	(u8(KillType));
	P.w_u16	(KilledID);
	P.w_u16	(KillerID);
	P.w_u16	(WeaponID);
	P.w_u8	(u8(SpecialKill));

	u32	cnt = get_players_count();	
	for( u32 it = 0; it < cnt; it++ )
	{
		xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
		game_PlayerState* ps	= l_pC->ps;
		if (!l_pC || !l_pC->net_Ready || !ps) continue;
		m_server->SendTo(l_pC->ID, P);
	};
};

void	game_sv_mp::OnPlayerChangeName		(NET_Packet& P, ClientID sender)
{
	string1024 NewName = "";
	P.r_stringZ(NewName);
	xrClientData*	pClient	= (xrClientData*)m_server->ID_to_client	(sender);
	
	if (!pClient || !pClient->net_Ready) return;
	game_PlayerState* ps = pClient->ps;
	if (!ps) return;

	if( ((xrGameSpyServer*)m_server)->HasProtected() )
	{
		Msg( "Player \"%s\" try to change name on \"%s\" at protected server.", ps->getName(), NewName );

		NET_Packet			P;
		GenerateGameMessage (P);
		P.w_u32				(GAME_EVENT_SERVER_STRING_MESSAGE);
		P.w_stringZ			("Server is protected. Can\'t change player name!");
		m_server->SendTo( sender, P );
		return;
	}

	if (NewPlayerName_Exists(pClient, NewName))
	{
		NewPlayerName_Generate(pClient, NewName);
	};

	if (pClient->owner)
	{
		NET_Packet			P;
		GenerateGameMessage(P);
		P.w_u32(GAME_EVENT_PLAYER_NAME);
		P.w_u16(pClient->owner->ID);
		P.w_s16(ps->team);
		P.w_stringZ(ps->getName());
		P.w_stringZ(NewName);
		//---------------------------------------------------		
		u32	cnt = get_players_count();	
		for(u32 it=0; it<cnt; it++)	
		{
			xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
			game_PlayerState* ps	= l_pC->ps;
			if (!l_pC || !l_pC->net_Ready || !ps) continue;
			m_server->SendTo(l_pC->ID, P);
		};
		//---------------------------------------------------
		pClient->owner->set_name_replace(NewName);
		NewPlayerName_Replace(pClient, NewName);
	};

	Game().m_WeaponUsageStatistic->ChangePlayerName( ps->name, NewName );
	ps->setName(NewName);

	signal_Syncronize();
};

void		game_sv_mp::OnPlayerSpeechMessage	(NET_Packet& P, ClientID sender)
{
	xrClientData*	pClient	= (xrClientData*)m_server->ID_to_client	(sender);

	if (!pClient || !pClient->net_Ready) return;
	game_PlayerState* ps = pClient->ps;
	if (!ps) return;

	if (pClient->owner)
	{
		NET_Packet			NP;
		GenerateGameMessage(NP);
		NP.w_u32(GAME_EVENT_SPEECH_MESSAGE);
		NP.w_u16(ps->GameID);
		NP.w_u8(P.r_u8());
		NP.w_u8(P.r_u8());
		NP.w_u8(P.r_u8());		
		//---------------------------------------------------		
		u32	cnt = get_players_count();	
		for(u32 it=0; it<cnt; it++)	
		{
			xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
			game_PlayerState* ps	= l_pC->ps;
			if (!l_pC || !l_pC->net_Ready || !ps) continue;
			m_server->SendTo(l_pC->ID, NP, net_flags(TRUE, TRUE, TRUE));
		};
	};
};

void		game_sv_mp::OnPlayerGameMenu(NET_Packet& P, ClientID sender)
{
	u8 SubEvent = P.r_u8();
	switch (SubEvent)
	{
	case PLAYER_SELECT_SPECTATOR:
		{
			OnPlayerSelectSpectator(P, sender);
		}break;
	case PLAYER_CHANGE_TEAM:
		{
			OnPlayerSelectTeam(P, sender);
		}break;
	case PLAYER_CHANGE_SKIN:
		{
			OnPlayerSelectSkin(P, sender);
		}break;
	}
}
void		game_sv_mp::OnPlayerSelectSpectator(NET_Packet& P, ClientID sender)
{
	xrClientData*	pClient	= (xrClientData*)m_server->ID_to_client	(sender);

	if (!pClient || !pClient->net_Ready) return;
	game_PlayerState* ps = pClient->ps;
	if (!ps) return;
	
	KillPlayer(sender, ps->GameID);
	ps->setFlag(GAME_PLAYER_FLAG_SPECTATOR);
	//-------------------------------------------
	if (pClient->owner)
	{
		CSE_ALifeCreatureActor	*pA	=	smart_cast<CSE_ALifeCreatureActor*>(pClient->owner);
		if (pA)
		{
			SpawnPlayer(sender, "spectator");
		};
	}
}

void	game_sv_mp::LoadRanks	()
{
	m_aRanks.clear();
	int NumRanks = 0;
	while(1)
	{
		string256 RankSect;
		sprintf_s(RankSect, "rank_%d",NumRanks);
		if (!pSettings->section_exist(RankSect)) break;
		NumRanks++;
	};

	for (int i=0; ; i++)
	{
		string256 RankSect;
		sprintf_s(RankSect, "rank_%d",i);
		if (!pSettings->section_exist(RankSect)) break;
		Rank_Struct NewRank; 
		
		NewRank.m_sTitle = pSettings->r_string(RankSect, "rank_name");
		NewRank.m_iBonusMoney = READ_IF_EXISTS(pSettings, r_s32, RankSect, "rank_aquire_money", 0);
		shared_str RDEB_str = pSettings->r_string(RankSect, "rank_diff_exp_bonus");
		int RDEB_Count = _GetItemCount(RDEB_str.c_str());
		for (int r=0; r<RDEB_Count; r++)
		{
			string16						temp;
			float f = 1.0f;
			if (r <= NumRanks)
				f = float(atof(_GetItem(RDEB_str.c_str(), r, temp)));
			NewRank.m_aRankDiff_ExpBonus.push_back(f);
		};

		shared_str sTerms = pSettings->r_string(RankSect, "rank_exp");
		int TermsCount = _GetItemCount(sTerms.c_str());
		R_ASSERT2((TermsCount != 0 && TermsCount <= MAX_TERMS), "Error Number of Terms for Rank");

		for (int t =0; t<TermsCount; t++)
		{
			string16						temp;			
			NewRank.m_iTerms[t] = atoi(_GetItem(sTerms.c_str(), t, temp));
		}
		m_aRanks.push_back(NewRank);
	};
};

void	game_sv_mp::Player_AddExperience	(game_PlayerState* ps, float Exp)
{
	if (!ps) return;

	ps->experience_New += Exp;
	
	if (Player_Check_Rank(ps) && Player_RankUp_Allowed()) Player_Rank_Up(ps);	

	if (ps->rank==m_aRanks.size()-1) ps->experience_D = 1.0f;
	else
	{
		int CurExp = m_aRanks[ps->rank].m_iTerms[0];
		int NextExp = m_aRanks[ps->rank+1].m_iTerms[0];
		if ((ps->experience_Real+ps->experience_New) > NextExp) ps->experience_D = 1.0f;
		else ps->experience_D = 1.0f - (NextExp - ps->experience_Real- ps->experience_New)/(NextExp - CurExp);
		clamp(ps->experience_D, 0.0f, 1.0f);
	};
};

bool	game_sv_mp::Player_Check_Rank		(game_PlayerState* ps)
{
	if (!ps) return false;
	if (ps->rank==m_aRanks.size()-1) return false;
	int NextExp = m_aRanks[ps->rank+1].m_iTerms[0];
	if ((ps->experience_Real+ps->experience_New) < NextExp) return false;
	return true;
}

void	game_sv_mp::Player_Rank_Up		(game_PlayerState* ps)
{
	if (!ps) return;

	if (ps->rank==m_aRanks.size()-1) return;
	
	
	ps->rank++;
	Player_AddBonusMoney(ps, m_aRanks[ps->rank].m_iBonusMoney, SKT_NEWRANK);
	Player_ExperienceFin(ps);
};

void	game_sv_mp::Player_ExperienceFin	(game_PlayerState* ps)
{
	if (!ps) return;
	ps->experience_Real += ps->experience_New;
	ps->experience_New = 0;
}

void	game_sv_mp::UpdatePlayersMoney		()
{
	u32	cnt = get_players_count();	
	for(u32 it=0; it<cnt; it++)	
	{
		xrClientData *l_pC = (xrClientData*)	m_server->client_Get	(it);
		game_PlayerState* ps	= l_pC->ps;
		if (!l_pC || !l_pC->net_Ready || !ps) continue;
		if (!ps->money_added && ps->m_aBonusMoney.empty()) continue;
		//-----------------------------------------------------------
		NET_Packet P;
		
		GenerateGameMessage (P);
		P.w_u32		(GAME_EVENT_PLAYERS_MONEY_CHANGED);

		P.w_s32(ps->money_for_round);
		P.w_s32(ps->money_added);	
		ps->money_added = 0;
		P.w_u8(u8(ps->m_aBonusMoney.size() & 0xff));
		if (!ps->m_aBonusMoney.empty())
		{
			for (u32 i=0; i<ps->m_aBonusMoney.size(); i++)
			{
				Bonus_Money_Struct* pBMS = &(ps->m_aBonusMoney[i]);
				P.w_s32(pBMS->Money);
				P.w_u8(u8(pBMS->Reason & 0xff));
				if (pBMS->Reason == SKT_KIR) P.w_u8(pBMS->Kills);
			};
			ps->m_aBonusMoney.clear();
		};		

		m_server->SendTo(l_pC->ID, P);
	};
};
/*
bool	game_sv_mp::GetTeamItem_ByID		(WeaponDataStruct** pRes, TEAM_WPN_LIST* pWpnList, u16 ItemID)
{
	if (!pWpnList) return false;
	TEAM_WPN_LIST_it pWpnI	= std::find(pWpnList->begin(), pWpnList->end(), (ItemID));
	if (pWpnI == pWpnList->end() || !((*pWpnI) == (ItemID))) return false;
	*pRes = &(*pWpnI);
	return true;
};

bool	game_sv_mp::GetTeamItem_ByName		(WeaponDataStruct** pRes,TEAM_WPN_LIST* pWpnList, LPCSTR ItemName)
{
	if (!pWpnList) return false;
	TEAM_WPN_LIST_it pWpnI	= std::find(pWpnList->begin(), pWpnList->end(), ItemName);
	if (pWpnI == pWpnList->end() || !((*pWpnI) == ItemName)) return false;
	*pRes = &(*pWpnI);
	return true;
};
*/
void	game_sv_mp::Player_AddBonusMoney	(game_PlayerState* ps, s32 MoneyAmount, SPECIAL_KILL_TYPE Reason, u8 Kill)
{
	if (!ps) return;
	//-----------------------------
	if (MoneyAmount) ps->m_aBonusMoney.push_back(Bonus_Money_Struct(MoneyAmount, u8(Reason & 0xff), Kill));
	//-----------------------------
	Player_AddMoney(ps, MoneyAmount);
	//-----------------------------
	ps->money_added -= MoneyAmount;
}
void	game_sv_mp::Player_AddMoney			(game_PlayerState* ps, s32 MoneyAmount)
{
	if (!ps) return;
	TeamStruct* pTeam		= GetTeamData(u8(ps->team));

	s64 TotalMoney = ps->money_for_round;

	TotalMoney	+= MoneyAmount;
	ps->money_added += MoneyAmount;

	if (TotalMoney<pTeam->m_iM_Min) 
		TotalMoney = pTeam->m_iM_Min;
	if (TotalMoney > 1000000)
		TotalMoney = 1000000;

	ps->money_for_round = s32(TotalMoney);
	//---------------------------------------
	Game().m_WeaponUsageStatistic->OnPlayerAddMoney(ps, MoneyAmount);
	//---------------------------------------	
};
//---------------------------------------------------------------------
extern u32 g_sv_dwMaxClientPing;
void	game_sv_mp::ReadOptions				(shared_str &options)
{
	inherited::ReadOptions(options);

	u8 SpectatorModes = SpectatorModes_Pack();
	SpectatorModes = u8(get_option_i(*options,"spectrmds",s32(SpectatorModes)) & 0x00ff);
	SpectatorModes_UnPack(SpectatorModes);

	g_sv_dwMaxClientPing = get_option_i(*options,"maxping",g_sv_dwMaxClientPing);

	string64	StartTime, TimeFactor;
	strcpy(StartTime,get_option_s		(*options,"estime","12:00:00"));
	strcpy(TimeFactor,get_option_s		(*options,"etimef","1"));

	u32 year = 1, month = 1, day = 1, hours = 0, mins = 0, secs = 0, milisecs = 0;
	sscanf				(StartTime,"%d:%d:%d.%d",&hours,&mins,&secs,&milisecs);
	u64 StartEnvGameTime	= generate_time	(year,month,day,hours,mins,secs,milisecs);
	float EnvTimeFactor = float(atof(TimeFactor))*GetEnvironmentGameTimeFactor();

	SetEnvironmentGameTimeFactor(StartEnvGameTime,EnvTimeFactor);
	SetGameTimeFactor(StartEnvGameTime,g_fTimeFactor);
};

static bool g_bConsoleCommandsCreated_MP = false;
void game_sv_mp::ConsoleCommands_Create	()
{
};

void game_sv_mp::ConsoleCommands_Clear	()
{
};
#include "string_table.h"
void game_sv_mp::DumpOnlineStatistic()
{
	xrGameSpyServer* srv		= smart_cast<xrGameSpyServer*>(m_server);

	string_path					fn;
	FS.update_path				(fn,"$logs$","mp_stats\\");
	strcat_s					(fn, srv->HostName.c_str());
	strcat_s					(fn, "\\online\\dmp" );

	string64					t_stamp;
	timestamp					(t_stamp);
	strcat_s					(fn, t_stamp );
	strcat_s					(fn, ".ltx" );

	CInifile					ini(fn, FALSE, FALSE, TRUE);
	shared_str					current_section = "global";
	string256					str_buff;

	ini.w_u32					(current_section.c_str(), "players_total_cnt", m_server->client_Count());

	sprintf_s					(str_buff,"\"%s\"",CStringTable().translate(Level().name().c_str()).c_str());
	ini.w_string				(current_section.c_str(), "current_map_name", str_buff);

	sprintf_s					(str_buff,"%s",CStringTable().translate(type_name()).c_str() );
	ini.w_string				(current_section.c_str(), "game_mode", str_buff);

	MAP_ROTATION_LIST_it it		= m_pMapRotation_List.begin();
	MAP_ROTATION_LIST_it it_e	= m_pMapRotation_List.end();
	for(u32 idx=0;it!=it_e;++it,++idx)
	{
		string16					num_buf;
		sprintf_s					(num_buf,"%d",idx);
		sprintf_s					(str_buff,"\"%s\"", CStringTable().translate((*it).c_str()).c_str());
		ini.w_string				("map_rotation", num_buf, str_buff);
	}

	for(u32 idx=0; idx<m_server->client_Count(); ++idx)
	{
		xrClientData *l_pC			= (xrClientData*)m_server->client_Get(idx);
		
		if(m_server->GetServerClient()==l_pC && g_dedicated_server) 
			continue;
		
		if(!l_pC->net_Ready)
			continue;

		string16					num_buf;
		sprintf_s					(num_buf,"player_%d",idx);

		WritePlayerStats			(ini,num_buf,l_pC);
	}
	WriteGameState				(ini, current_section.c_str(), false);
}

void game_sv_mp::WritePlayerStats(CInifile& ini, LPCSTR sect, xrClientData* pCl)
{
	ini.w_string(sect,"player_name",	pCl->ps->getName());
	ini.w_u32	(sect,"player_team",	pCl->ps->team);
	ini.w_u32	(sect,"kills_rival",	pCl->ps->m_iRivalKills);
	ini.w_u32	(sect,"kills_self",		pCl->ps->m_iSelfKills);
	ini.w_u32	(sect,"kills_self",		pCl->ps->m_iSelfKills);
	ini.w_u32	(sect,"team_kills",		pCl->ps->m_iTeamKills);
	ini.w_u32	(sect,"deaths",			pCl->ps->m_iDeaths);

	ini.w_string(sect,"player_ip",		pCl->m_cAddress.to_string().c_str());
	ini.w_u32	(sect,"kills_in_row",	pCl->ps->m_iKillsInRowMax);
	ini.w_u32	(sect,"rank",			pCl->ps->rank);
	ini.w_u32	(sect,"artefacts",		pCl->ps->af_count);
	ini.w_u32	(sect,"ping",			pCl->ps->ping);
	ini.w_u32	(sect,"money",			pCl->ps->money_for_round);
	ini.w_u32	(sect,"online_time_sec",(Level().timeServer()-pCl->ps->m_online_time)/1000);

	if(Game().m_WeaponUsageStatistic->CollectData())
	{
		Player_Statistic& plstats		= *(Game().m_WeaponUsageStatistic->FindPlayer(pCl->ps->getName()));
		u32 hs		= plstats.m_dwSpecialKills[0];
		u32 bks		= plstats.m_dwSpecialKills[1];
		u32 knf		= plstats.m_dwSpecialKills[2];

		ini.w_u32	(sect,"headshots_kills",	hs);
		ini.w_u32	(sect,"backstab_kills",		bks);
		ini.w_u32	(sect,"knife_kills",		knf);
	}
}

void game_sv_mp::WriteGameState(CInifile& ini, LPCSTR sect, bool bRoundResult)
{
	if(!bRoundResult)
		ini.w_u32						(sect, "online_time_sec", Device.dwTimeGlobal/1000);
}

void game_sv_mp::DumpRoundStatistics()
{
	if ( !g_sv_mp_iDumpStatsPeriod ) return;

	string_path					fn;
	xrGameSpyServer* srv		= smart_cast<xrGameSpyServer*>(m_server);

	FS.update_path				(fn,"$logs$","mp_stats\\");
	string64					t_stamp;
	timestamp					(t_stamp);
	strcat_s					(fn, srv->HostName.c_str() );
	strcat_s					(fn, "\\games\\dmp" );
	strcat_s					(fn, t_stamp );
	strcat_s					(fn, ".ltx" );

	CInifile					ini(fn, FALSE, FALSE, TRUE);
	shared_str					current_section = "global";
	string256					str_buff;

	ini.w_string				(current_section.c_str(),"start_time", m_round_start_time_str);

	sprintf_s					(str_buff,"%s",CStringTable().translate(type_name()).c_str() );
	ini.w_string				(current_section.c_str(), "game_mode", str_buff);

	sprintf_s					(str_buff,"\"%s\"",CStringTable().translate(Level().name().c_str()).c_str());
	ini.w_string				(current_section.c_str(), "current_map_name", str_buff);

	sprintf_s					(str_buff,"\"%s\"",Level().name().c_str());
	ini.w_string				(current_section.c_str(), "current_map_name_internal", str_buff);

	for(u32 idx=0; idx<m_server->client_Count(); ++idx)
	{
		xrClientData *l_pC			= (xrClientData*)m_server->client_Get(idx);
		if(m_server->GetServerClient()==l_pC && g_dedicated_server) 
			continue;

		string16					num_buf;
		sprintf_s					(num_buf,"player_%d",idx);

		WritePlayerStats			(ini,num_buf,l_pC);
	}
	WriteGameState					(ini,current_section.c_str(), true);

	Game().m_WeaponUsageStatistic->SaveDataLtx(ini);
	//Game().m_WeaponUsageStatistic->Clear();
}

void game_sv_mp::SvSendChatMessage(LPCSTR str)
{
	NET_Packet			P;	
	P.w_begin			(M_CHAT_MESSAGE);
	P.w_s16				(0);
	P.w_stringZ			("ServerAdmin");
	P.w_stringZ			(str);
	P.w_s16				(0);
	u_EventSend			(P);
}

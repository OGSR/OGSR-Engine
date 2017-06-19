#include "stdafx.h"
#include "xrServer.h"
#include "LevelGameDef.h"
#include "script_process.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "script_engine.h"
#include "script_engine_space.h"
#include "level.h"
#include "xrserver.h"
#include "ai_space.h"
#include "game_sv_event_queue.h"
#include "../XR_IOConsole.h"
#include "../xr_ioc_cmd.h"
#include "string_table.h"

#include "debug_renderer.h"

ENGINE_API	bool g_dedicated_server;

#define			MAPROT_LIST_NAME		"maprot_list.ltx"
string_path		MAPROT_LIST		= "";
BOOL	net_sv_control_hit	= FALSE		;
BOOL	g_bCollectStatisticData = FALSE;

//-----------------------------------------------------------------
u32		g_sv_base_dwRPointFreezeTime	= 0;
int		g_sv_base_iVotingEnabled		= 0x00ff;
//-----------------------------------------------------------------

xr_token	round_end_result_str[]=
{
	{ "Finish",					eRoundEnd_Finish			},
	{ "Game restarted",			eRoundEnd_GameRestarted		},
	{ "Game restarted fast",	eRoundEnd_GameRestartedFast	},
	{ "Time limit",				eRoundEnd_TimeLimit			},
	{ "Frag limit",				eRoundEnd_FragLimit			},
	{ "Artefact limit",			eRoundEnd_ArtrefactLimit	},
	{ "Unknown",				eRoundEnd_Force				},
	{ 0,						0							}
};

// Main
game_PlayerState*	game_sv_GameState::get_it					(u32 it)
{
	xrClientData*	C	= (xrClientData*)m_server->client_Get			(it);
	if (0==C)			return 0;
	else				return C->ps;
}

game_PlayerState*	game_sv_GameState::get_id					(ClientID id)							
{
	xrClientData*	C	= (xrClientData*)m_server->ID_to_client	(id);
	if (0==C)			return NULL;
	else				return C->ps;
}

ClientID				game_sv_GameState::get_it_2_id				(u32 it)
{
	xrClientData*	C	= (xrClientData*)m_server->client_Get		(it);
	if (0==C){
		ClientID clientID;clientID.set(0);
		return clientID;
	}
	else				return C->ID;
}

LPCSTR				game_sv_GameState::get_name_it				(u32 it)
{
	xrClientData*	C	= (xrClientData*)m_server->client_Get		(it);
	if (0==C)			return 0;
	else				return *C->name;
}

LPCSTR				game_sv_GameState::get_name_id				(ClientID id)							
{
	xrClientData*	C	= (xrClientData*)m_server->ID_to_client	(id);
	if (0==C)			return 0;
	else				return *C->name;
}

LPCSTR				game_sv_GameState::get_player_name_id				(ClientID id)								
{
	xrClientData* xrCData	=	m_server->ID_to_client(id);
	if(xrCData)
		return xrCData->name.c_str();
	else 
		return "unknown";
}

u32					game_sv_GameState::get_players_count		()
{
	return				m_server->client_Count();
}

u16					game_sv_GameState::get_id_2_eid				(ClientID id)
{
	xrClientData*	C	= (xrClientData*)m_server->ID_to_client	(id);
	if (0==C)			return 0xffff;
	CSE_Abstract*	E	= C->owner;
	if (0==E)			return 0xffff;
	return E->ID;
}

game_PlayerState*	game_sv_GameState::get_eid (u16 id) //if exist
{
	CSE_Abstract* entity = get_entity_from_eid(id);

	if (entity)
	{
		if (entity->owner)
		{
			if(entity->owner->ps)
			{
				if (entity->owner->ps->GameID == id)
					return entity->owner->ps;
			}
		}
	}
	//-------------------------------------------------
	u32		cnt		= get_players_count	();
	for		(u32 it=0; it<cnt; ++it)	
	{
		game_PlayerState*	ps	=	get_it	(it);
		if (!ps) continue;
		if (ps->HasOldID(id)) return ps;
	};
	//-------------------------------------------------
	return NULL;
}

void* game_sv_GameState::get_client (u16 id) //if exist
{
	CSE_Abstract* entity = get_entity_from_eid(id);
	if (entity && entity->owner && entity->owner->ps && entity->owner->ps->GameID == id)
		return entity->owner;
	//-------------------------------------------------
	u32		cnt		= get_players_count	();
	for		(u32 it=0; it<cnt; ++it)	
	{
		xrClientData*	C	= (xrClientData*)m_server->client_Get		(it);
		if (!C || !C->ps) continue;
//		game_PlayerState*	ps	=	get_it	(it);
		if (C->ps->HasOldID(id)) return C;
	};
	//-------------------------------------------------
	return NULL;
}

CSE_Abstract*		game_sv_GameState::get_entity_from_eid		(u16 id)
{
	return				m_server->ID_to_entity(id);
}

// Utilities
u32					game_sv_GameState::get_alive_count			(u32 team)
{
	u32		cnt		= get_players_count	();
	u32		alive	= 0;
	for		(u32 it=0; it<cnt; ++it)	
	{
		game_PlayerState*	ps	=	get_it	(it);
		if (!ps) continue;
		if (u32(ps->team) == team)	alive	+=	(ps->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD))?0:1;
	}
	return alive;
}

xr_vector<u16>*		game_sv_GameState::get_children				(ClientID id)
{
	xrClientData*	C	= (xrClientData*)m_server->ID_to_client	(id);
	if (0==C)			return 0;
	CSE_Abstract* E	= C->owner;
	if (0==E)			return 0;
	return	&(E->children);
}

s32					game_sv_GameState::get_option_i				(LPCSTR lst, LPCSTR name, s32 def)
{
	string64		op;
	strconcat		(sizeof(op),op,"/",name,"=");
	if (strstr(lst,op))	return atoi	(strstr(lst,op)+xr_strlen(op));
	else				return def;
}

float					game_sv_GameState::get_option_f				(LPCSTR lst, LPCSTR name, float def)
{
	string64		op;
	strconcat		(sizeof(op),op,"/",name,"=");
	LPCSTR			found =	strstr(lst,op);

	if (found)
	{	
		float		val;
		int cnt		= sscanf(found+xr_strlen(op),"%f",&val);
		VERIFY		(cnt==1);
		return		val;
//.		return atoi	(strstr(lst,op)+xr_strlen(op));
	}else
		return def;
}

string64&			game_sv_GameState::get_option_s				(LPCSTR lst, LPCSTR name, LPCSTR def)
{
	static string64	ret;

	string64		op;
	strconcat		(sizeof(op),op,"/",name,"=");
	LPCSTR			start	= strstr(lst,op);
	if (start)		
	{
		LPCSTR			begin	= start + xr_strlen(op); 
		sscanf			(begin, "%[^/]",ret);
	}
	else			
	{
		if (def)	strcpy		(ret,def);
		else		ret[0]=0;
	}
	return ret;
}
void				game_sv_GameState::signal_Syncronize		()
{
	sv_force_sync	= TRUE;
}

// Network
void game_sv_GameState::net_Export_State						(NET_Packet& P, ClientID to)
{
	// Generic
	P.w_clientID	(to);
	P.w_s32			(m_type);
	P.w_u16			(m_phase);
	P.w_s32			(m_round);
	P.w_u32			(m_start_time);
	P.w_u8			(u8(g_sv_base_iVotingEnabled&0xff));
	P.w_u8			(u8(net_sv_control_hit));
	P.w_u8			(u8(g_bCollectStatisticData));

	// Players
//	u32	p_count			= get_players_count() - ((g_dedicated_server)? 1 : 0);
	u32 p_count = 0;
	for (u32 p_it=0; p_it<get_players_count(); ++p_it)
	{
		xrClientData*	C		=	(xrClientData*)	m_server->client_Get	(p_it);		
		if (!C->net_Ready || (C->ps->IsSkip() && C->ID != to)) continue;
		p_count++;
	};

	P.w_u16				(u16(p_count));
	game_PlayerState*	Base	= get_id(to);
	for (u32 p_it=0; p_it<get_players_count(); ++p_it)
	{
		string64	p_name;
		xrClientData*	C		=	(xrClientData*)	m_server->client_Get	(p_it);
		game_PlayerState* A		=	get_it			(p_it);
		if (!C->net_Ready || (A->IsSkip() && C->ID != to)) continue;
		if (0==C)	strcpy(p_name,"Unknown");
		else 
		{
			CSE_Abstract* C_e		= C->owner;
			if (0==C_e)		strcpy(p_name,"Unknown");
			else 
			{
				strcpy	(p_name,C_e->name_replace());
			}
		}

		A->setName(p_name);
		u16 tmp_flags = A->flags__;

		if (Base==A)	
			A->setFlag(GAME_PLAYER_FLAG_LOCAL);

		ClientID clientID = get_it_2_id	(p_it);
		P.w_clientID			(clientID);
		A->net_Export			(P, TRUE);
		
		A->flags__ = tmp_flags;
	}

	net_Export_GameTime(P);
}

void game_sv_GameState::net_Export_Update(NET_Packet& P, ClientID id_to, ClientID id)
{
	game_PlayerState* A		= get_id		(id);
	if (A)
	{
		u16 bk_flags = A->flags__;
		if (id==id_to)	
		{
			A->setFlag(GAME_PLAYER_FLAG_LOCAL);
		}

		P.w_clientID	(id);
		A->net_Export	(P);
		A->flags__		= bk_flags;
	};
};

void game_sv_GameState::net_Export_GameTime						(NET_Packet& P)
{
	//Syncronize GameTime 
	P.w_u64(GetGameTime());
	P.w_float(GetGameTimeFactor());
	//Syncronize EnvironmentGameTime 
	P.w_u64(GetEnvironmentGameTime());
	P.w_float(GetEnvironmentGameTimeFactor());
};


void game_sv_GameState::OnPlayerConnect			(ClientID /**id_who/**/)
{
	signal_Syncronize	();
}

void game_sv_GameState::OnPlayerDisconnect		(ClientID /**id_who/**/, LPSTR, u16 )
{
	signal_Syncronize	();
}

static float							rpoints_Dist [TEAM_COUNT] = {1000.f, 1000.f, 1000.f, 1000.f};
void game_sv_GameState::Create					(shared_str &options)
{
	string_path	fn_game;	
	if (FS.exist(fn_game, "$level$", "level.game")) 
	{
		IReader *F = FS.r_open	(fn_game);
		IReader *O = 0;

		// Load RPoints
		if (0!=(O = F->open_chunk	(RPOINT_CHUNK)))
		{ 
			for (int id=0; O->find_chunk(id); ++id)
			{
				RPoint					R;
				u8						team;
				u8						type;
				u8						GameType;

				O->r_fvector3			(R.P);
				O->r_fvector3			(R.A);
				team					= O->r_u8	();	VERIFY(team>=0 && team<4);
				type					= O->r_u8	();
				GameType				= O->r_u8	();
				//u16 res					= 
				O->r_u8	();

				if (GameType != rpgtGameAny)
				{
					if ((GameType == rpgtGameDeathmatch && Type() != GAME_DEATHMATCH) ||
						(GameType == rpgtGameTeamDeathmatch && Type() != GAME_TEAMDEATHMATCH)	||
						(GameType == rpgtGameArtefactHunt && Type() != GAME_ARTEFACTHUNT)
						)
					{
						continue;
					};
				};
				switch (type)
				{
				case rptActorSpawn:
					{
						rpoints[team].push_back	(R);
						for (int i=0; i<int(rpoints[team].size())-1; i++)
						{
							RPoint rp = rpoints[team][i];
							float dist = R.P.distance_to_xz(rp.P)/2;
							if (dist<rpoints_MinDist[team])
								rpoints_MinDist[team] = dist;
							dist = R.P.distance_to(rp.P)/2;
							if (dist<rpoints_Dist[team])
								rpoints_Dist[team] = dist;
						};
					}break;
				};
			};
			O->close();
		}

		FS.r_close	(F);
	}

	if (!g_dedicated_server)
	{
		// loading scripts
		ai().script_engine().remove_script_process(ScriptEngine::eScriptProcessorGame);
		string_path					S;
		FS.update_path				(S,"$game_config$","script.ltx");
		CInifile					*l_tpIniFile = xr_new<CInifile>(S);
		R_ASSERT					(l_tpIniFile);

		if( l_tpIniFile->section_exist( type_name() ) )
			if (l_tpIniFile->r_string(type_name(),"script"))
				ai().script_engine().add_script_process(ScriptEngine::eScriptProcessorGame,xr_new<CScriptProcess>("game",l_tpIniFile->r_string(type_name(),"script")));
			else
				ai().script_engine().add_script_process(ScriptEngine::eScriptProcessorGame,xr_new<CScriptProcess>("game",""));

		xr_delete					(l_tpIniFile);
	}

	//---------------------------------------------------------------------
	ConsoleCommands_Create();
	//---------------------------------------------------------------------
//	CCC_LoadCFG_custom*	pTmp = xr_new<CCC_LoadCFG_custom>("sv_");
//	pTmp->Execute				(Console->ConfigFile);
//	xr_delete					(pTmp);
	//---------------------------------------------------------------------
	LPCSTR		svcfg_ltx_name = "-svcfg ";
	if (strstr(Core.Params, svcfg_ltx_name))
	{
		string_path svcfg_name = "";
		int		sz = xr_strlen(svcfg_ltx_name);
		sscanf		(strstr(Core.Params,svcfg_ltx_name)+sz,"%[^ ] ",svcfg_name);
//		if (FS.exist(svcfg_name))
		{
			Console->ExecuteScript(svcfg_name);
		}
	};
	//---------------------------------------------------------------------
	ReadOptions(options);	
}

void	game_sv_GameState::ReadOptions				(shared_str &options)
{
	g_sv_base_dwRPointFreezeTime = get_option_i(*options, "rpfrz", g_sv_base_dwRPointFreezeTime/1000) * 1000;

//.	strcpy(MAPROT_LIST, MAPROT_LIST_NAME);
//.	if (!FS.exist(MAPROT_LIST))
	FS.update_path(MAPROT_LIST, "$app_data_root$", MAPROT_LIST_NAME);
	if (FS.exist(MAPROT_LIST))
		Console->ExecuteScript(MAPROT_LIST);
	
	g_sv_base_iVotingEnabled = get_option_i(*options,"vote",(g_sv_base_iVotingEnabled));
	//---------------------------
	//Convert old vote param
	if (g_sv_base_iVotingEnabled != 0)
	{
		if (g_sv_base_iVotingEnabled == 1)
			g_sv_base_iVotingEnabled = 0x00ff;
	}
};
//-----------------------------------------------------------
static bool g_bConsoleCommandsCreated_SV_Base = false;
void	game_sv_GameState::ConsoleCommands_Create	()
{
};

void	game_sv_GameState::ConsoleCommands_Clear	()
{
};

void	game_sv_GameState::assign_RP				(CSE_Abstract* E, game_PlayerState* ps_who)
{
	VERIFY				(E);

	u8					l_uc_team = u8(-1);
	CSE_Spectator		*tpSpectator = smart_cast<CSE_Spectator*>(E);
	if (tpSpectator)
		l_uc_team = tpSpectator->g_team();
	else {
		CSE_ALifeCreatureAbstract	*tpTeamed = smart_cast<CSE_ALifeCreatureAbstract*>(E);
		if (tpTeamed)
			l_uc_team = tpTeamed->g_team();
		else
			R_ASSERT2(tpTeamed,"Non-teamed object is assigning to respawn point!");
	}
	xr_vector<RPoint>&	rp	= rpoints[l_uc_team];
	//-----------------------------------------------------------
	xr_vector<u32>	xrp;//	= rpoints[l_uc_team];
	for (u32 i=0; i<rp.size(); i++)
	{
		if (rp[i].TimeToUnfreeze < Level().timeServer())
			xrp.push_back(i);
	}
	u32 rpoint = 0;
	if (xrp.size() && !tpSpectator)
	{
		rpoint = xrp[::Random.randI((int)xrp.size())];
	}
	else
	{
		if (!tpSpectator)
		{
			for (u32 i=0; i<rp.size(); i++)
			{
				rp[i].TimeToUnfreeze = 0;
			};
		};
		rpoint = ::Random.randI((int)rp.size());
	}
	//-----------------------------------------------------------
	RPoint&				r	= rp[rpoint];
	if (!tpSpectator)
	{
		r.TimeToUnfreeze	= Level().timeServer() + g_sv_base_dwRPointFreezeTime;
	};
	E->o_Position.set	(r.P);
	E->o_Angle.set		(r.A);
}

bool				game_sv_GameState::IsPointFreezed			(RPoint* rp)
{
	return rp->TimeToUnfreeze > Level().timeServer();
};

void				game_sv_GameState::SetPointFreezed		(RPoint* rp)
{
	R_ASSERT(rp);
	rp->TimeToUnfreeze	= Level().timeServer() + g_sv_base_dwRPointFreezeTime;
}

CSE_Abstract*		game_sv_GameState::spawn_begin				(LPCSTR N)
{
	CSE_Abstract*	A	=   F_entity_Create(N);	R_ASSERT(A);	// create SE
	A->s_name			=   N;							// ltx-def
	A->s_gameid			=	u8(m_type);							// game-type
	A->s_RP				=	0xFE;								// use supplied
	A->ID				=	0xffff;								// server must generate ID
	A->ID_Parent		=	0xffff;								// no-parent
	A->ID_Phantom		=	0xffff;								// no-phantom
	A->RespawnTime		=	0;									// no-respawn
	return A;
}

CSE_Abstract*		game_sv_GameState::spawn_end				(CSE_Abstract* E, ClientID id)
{
	NET_Packet						P;
	u16								skip_header;
	E->Spawn_Write					(P,TRUE);
	P.r_begin						(skip_header);
	CSE_Abstract* N = m_server->Process_spawn	(P,id);
	F_entity_Destroy				(E);

	return N;
}

void game_sv_GameState::GenerateGameMessage (NET_Packet &P)
{ 
	P.w_begin(M_GAMEMESSAGE); 
};

void game_sv_GameState::u_EventGen(NET_Packet& P, u16 type, u16 dest)
{
	P.w_begin	(M_EVENT);
	P.w_u32		(Level().timeServer());//Device.TimerAsync());
	P.w_u16		(type);
	P.w_u16		(dest);
}

void game_sv_GameState::u_EventSend(NET_Packet& P, u32 dwFlags)
{
	m_server->SendBroadcast(BroadcastCID,P,dwFlags);
}

void game_sv_GameState::Update		()
{
	for (u32 it=0; it<m_server->client_Count(); ++it) {
		xrClientData*	C			= (xrClientData*)	m_server->client_Get(it);
		C->ps->ping					= u16(C->stats.getPing());
	}
	
	if (!g_dedicated_server)
	{
		if (Level().game) {
			CScriptProcess				*script_process = ai().script_engine().script_process(ScriptEngine::eScriptProcessorGame);
			if (script_process)
				script_process->update	();
		}
	}
}

game_sv_GameState::game_sv_GameState()
{
	VERIFY(g_pGameLevel);
	m_server					= Level().Server;
	m_event_queue = xr_new<GameEventQueue>();

	m_bMapRotation = false;
	m_bMapSwitched = false;
	m_bMapNeedRotation = false;
	m_bFastRestart = false;
	m_pMapRotation_List.clear();

	for (int i=0; i<TEAM_COUNT; i++) rpoints_MinDist[i] = 1000.0f;	
}

game_sv_GameState::~game_sv_GameState()
{
	if (!g_dedicated_server)
		ai().script_engine().remove_script_process(ScriptEngine::eScriptProcessorGame);
	xr_delete(m_event_queue);

	SaveMapList();

	m_pMapRotation_List.clear();
	//-------------------------------------------------------
	ConsoleCommands_Clear();
}

bool game_sv_GameState::change_level (NET_Packet &net_packet, ClientID sender)
{
	return						(true);
}

void game_sv_GameState::save_game (NET_Packet &net_packet, ClientID sender)
{
}

bool game_sv_GameState::load_game (NET_Packet &net_packet, ClientID sender)
{
	return						(true);
}

void game_sv_GameState::reload_game (NET_Packet &net_packet, ClientID sender)
{
}

void game_sv_GameState::switch_distance (NET_Packet &net_packet, ClientID sender)
{
}

void game_sv_GameState::OnHit (u16 id_hitter, u16 id_hitted, NET_Packet& P)
{
	CSE_Abstract*		e_hitter		= get_entity_from_eid	(id_hitter	);
	CSE_Abstract*		e_hitted		= get_entity_from_eid	(id_hitted	);
	if (!e_hitter || !e_hitted) return;

//	CSE_ALifeCreatureActor*		a_hitter		= smart_cast <CSE_ALifeCreatureActor*> (e_hitter);
	CSE_ALifeCreatureActor*		a_hitted		= smart_cast <CSE_ALifeCreatureActor*> (e_hitted);

	if (a_hitted/* && a_hitter*/)
	{
		OnPlayerHitPlayer(id_hitter, id_hitted, P);
		return;
	};
}

void game_sv_GameState::OnEvent (NET_Packet &tNetPacket, u16 type, u32 time, ClientID sender )
{
	switch	(type)
	{	
	case GAME_EVENT_PLAYER_CONNECTED:
		{
			ClientID ID;
			tNetPacket.r_clientID(ID);
			OnPlayerConnect(ID);
		}break;

	case GAME_EVENT_PLAYER_DISCONNECTED:
		{
			ClientID ID;
			tNetPacket.r_clientID(ID);
			string1024 PlayerName;
			tNetPacket.r_stringZ(PlayerName);
			u16		GameID = tNetPacket.r_u16();
			OnPlayerDisconnect(ID, PlayerName, GameID);
		}break;

	case GAME_EVENT_PLAYER_KILLED:
		{
		}break	;
	case GAME_EVENT_ON_HIT:
		{
			u16		id_dest				= tNetPacket.r_u16();
			u16     id_src				= tNetPacket.r_u16();
			CSE_Abstract*	e_src		= get_entity_from_eid	(id_src	);

			if(!e_src)  // && !IsGameTypeSingle() added by andy because of Phantom does not have server entity
			{
				if( IsGameTypeSingle() ) break;

				game_PlayerState* ps	= get_eid(id_src);
				if (!ps)				break;
				id_src					= ps->GameID;
			}

			OnHit(id_src, id_dest, tNetPacket);
			m_server->SendBroadcast		(BroadcastCID,tNetPacket,net_flags(TRUE,TRUE));
		}break;
	case GAME_EVENT_CREATE_CLIENT:
		{
			IClient* CL					= (IClient*)m_server->ID_to_client(sender);
			if ( CL == NULL ) { break; }
			
			CL->flags.bConnected		= TRUE;
			m_server->AttachNewClient	(CL);
		}break;
	case GAME_EVENT_PLAYER_AUTH:
		{
			IClient*	CL	=	m_server->ID_to_client		(sender);
			m_server->OnBuildVersionRespond(CL, tNetPacket);
		}break;
	default:
		{
			string16 tmp;
			R_ASSERT3	(0,"Game Event not implemented!!!", itoa(type, tmp, 10));
		};
	};
}

bool game_sv_GameState::NewPlayerName_Exists( void* pClient, LPCSTR NewName )
{
	if ( !pClient || !NewName ) return false;
	IClient* CL = (IClient*)pClient;
	if ( !CL->name || xr_strlen( CL->name.c_str() ) == 0 ) return false;

	u32	cnt	= get_players_count();
	for ( u32 it = 0; it < cnt; ++it )	
	{
		IClient*	pIC	= m_server->client_Get(it);
		if ( !pIC || pIC == CL ) continue;
		string64 xName;
		strcpy( xName, pIC->name.c_str() );
		if ( !xr_strcmp(NewName, xName) ) return true;
	};
	return false;
}

void game_sv_GameState::NewPlayerName_Generate( void* pClient, LPSTR NewPlayerName )
{
	if ( !pClient || !NewPlayerName ) return;
	NewPlayerName[21] = 0;
	for ( int i = 1; i < 100; ++i )
	{
		string64 NewXName;
		sprintf_s( NewXName, "%s_%d", NewPlayerName, i );
		if ( !NewPlayerName_Exists( pClient, NewXName ) )
		{
			strcpy( NewPlayerName, NewXName );
			return;
		}
	}
}

void game_sv_GameState::NewPlayerName_Replace( void* pClient, LPCSTR NewPlayerName )
{
	if ( !pClient || !NewPlayerName ) return;
	IClient* CL = (IClient*)pClient;
	if ( !CL->name || xr_strlen( CL->name.c_str() ) == 0 ) return;
	
	CL->name._set( NewPlayerName );
	
	//---------------------------------------------------------
	NET_Packet P;
	P.w_begin( M_CHANGE_SELF_NAME );
	P.w_stringZ( NewPlayerName );	
	m_server->SendTo( CL->ID, P );
}

void game_sv_GameState::OnSwitchPhase(u32 old_phase, u32 new_phase)
{
	inherited::OnSwitchPhase(old_phase, new_phase);
	signal_Syncronize	(); 
}

void game_sv_GameState::AddDelayedEvent(NET_Packet &tNetPacket, u16 type, u32 time, ClientID sender )
{
//	OnEvent(tNetPacket,type,time,sender);
	m_event_queue->Create(tNetPacket,type,time,sender);
}

void game_sv_GameState::ProcessDelayedEvent		()
{
	GameEvent* ge = NULL;
	while ((ge = m_event_queue->Retreive()) != 0) {
		OnEvent(ge->P,ge->type,ge->time,ge->sender);
		m_event_queue->Release();
	}
}

u32 game_sv_GameState::getRPcount (u16 team_idx)
{
	if ( !(team_idx<TEAM_COUNT) )
		return 0;
	else
		return rpoints[team_idx].size();
}

RPoint game_sv_GameState::getRP (u16 team_idx, u32 rp_idx)
{
	if( (team_idx<TEAM_COUNT) && (rp_idx<rpoints[team_idx].size()) )
	return rpoints[team_idx][rp_idx];
	else 
		return RPoint();
};

void game_sv_GameState::teleport_object	(NET_Packet &packet, u16 id)
{
}

void game_sv_GameState::add_restriction	(NET_Packet &packet, u16 id)
{
}

void game_sv_GameState::remove_restriction(NET_Packet &packet, u16 id)
{
}

void game_sv_GameState::remove_all_restrictions	(NET_Packet &packet, u16 id)
{
}

void game_sv_GameState::MapRotation_AddMap		(LPCSTR MapName)
{
	m_pMapRotation_List.push_back(MapName);
	if (m_pMapRotation_List.size() > 1)
		m_bMapRotation = true;
	else
		m_bMapRotation = false;
};

void game_sv_GameState::MapRotation_ListMaps	()
{
	if (m_pMapRotation_List.empty())
	{
		Msg ("- Currently there are no any maps in list.");
		return;
	}
	CStringTable st;
	Msg("- ----------- Maps ---------------");
	for (u32 i=0; i<m_pMapRotation_List.size(); i++)
	{
		xr_string MapName = m_pMapRotation_List[i];
		if (i==0)
			Msg("~   %d. %s (%s) (current)", i+1, *st.translate(MapName.c_str()), MapName.c_str());
		else
			Msg("  %d. %s (%s)", i+1, *st.translate(MapName.c_str()), MapName.c_str());
	}
	Msg("- --------------------------------");
};

void game_sv_GameState::OnRoundStart			()
{ 
	m_bMapNeedRotation = false;
	m_bFastRestart = false;

	for (int t=0; t<TEAM_COUNT; t++)
	{
		for (u32 i=0; i<rpoints[t].size(); i++)
		{
			RPoint rp = rpoints[t][i];
			rp.Blocked = false;
		}
	};
	rpointsBlocked.clear();
}// старт раунда

void game_sv_GameState::OnRoundEnd()
{ 
	if ( round_end_reason == eRoundEnd_GameRestarted || round_end_reason == eRoundEnd_GameRestartedFast )
	{
		m_bMapNeedRotation = false;
	}
	else
	{
		m_bMapNeedRotation = true;
	}

	m_bFastRestart = false;
	if ( round_end_reason == eRoundEnd_GameRestartedFast )
	{
		m_bFastRestart = true;
	}
}// конец раунда

void game_sv_GameState::SaveMapList				()
{
	if (0==MAPROT_LIST[0])				return;
	if (m_pMapRotation_List.empty())	return;
	IWriter*		fs	= FS.w_open(MAPROT_LIST);
	while(m_pMapRotation_List.size())
	{
		xr_string MapName = m_pMapRotation_List.front();
		m_pMapRotation_List.pop_front();

		fs->w_printf("sv_addmap %s\n", MapName.c_str());
	};
	FS.w_close		(fs);
};

shared_str game_sv_GameState::level_name		(const shared_str &server_options) const
{
	string64			l_name = "";
	VERIFY				(_GetItemCount(*server_options,'/'));
	return				(_GetItem(*server_options,0,l_name,'/'));
}

void game_sv_GameState::on_death	(CSE_Abstract *e_dest, CSE_Abstract *e_src)
{
	CSE_ALifeCreatureAbstract	*creature = smart_cast<CSE_ALifeCreatureAbstract*>(e_dest);
	if (!creature)
		return;

	VERIFY						(creature->m_killer_id == ALife::_OBJECT_ID(-1));
	creature->m_killer_id		= e_src->ID;
}

//  [7/5/2005]
#ifdef DEBUG
extern	Flags32	dbg_net_Draw_Flags;

void		game_sv_GameState::OnRender				()
{
	Fmatrix T; T.identity();
	Fvector V0, V1;
	u32 TeamColors[TEAM_COUNT] = {D3DCOLOR_XRGB(255, 0, 0), D3DCOLOR_XRGB(0, 255, 0), D3DCOLOR_XRGB(0, 0, 255), D3DCOLOR_XRGB(255, 255, 0)};
//	u32 TeamColorsDist[TEAM_COUNT] = {color_argb(128, 255, 0, 0), color_argb(128, 0, 255, 0), color_argb(128, 0, 0, 255), color_argb(128, 255, 255, 0)};

	if (dbg_net_Draw_Flags.test(1<<9))
	{
		for (int t=0; t<TEAM_COUNT; t++)
		{
			for (u32 i=0; i<rpoints[t].size(); i++)
			{
				RPoint rp = rpoints[t][i];
				V1 = V0 = rp.P;
				V1.y +=1.0f;

				T.identity();
				Level().debug_renderer().draw_line(Fidentity, V0, V1, TeamColors[t]);

				bool Blocked = false;
				for (u32 p_it=0; p_it<get_players_count(); ++p_it)
				{
					game_PlayerState* PS		=	get_it			(p_it);
					if (!PS) continue;
					if (PS->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD)) continue;
					CObject* pPlayer = Level().Objects.net_Find(PS->GameID);
					if (!pPlayer) continue;

					if (rp.P.distance_to(pPlayer->Position())<=0.4f)
					{
						Blocked = true;
						break;
					}
				};
				if (rp.Blocked) continue;

				float r = .3f;
				T.identity();
				T.scale(r, r, r);
				T.translate_add(rp.P);
				Level().debug_renderer().draw_ellipse(T, TeamColors[t]);
/*
				r = rpoints_MinDist[t];
				T.identity();
				T.scale(r, r, r);
				T.translate_add(rp.P);
				Level().debug_renderer().draw_ellipse(T, TeamColorsDist[t]);

				r = rpoints_Dist[t];
				T.identity();
				T.scale(r, r, r);
				T.translate_add(rp.P);
				Level().debug_renderer().draw_ellipse(T, TeamColorsDist[t]);
*/
			}
		}
	};

	if (dbg_net_Draw_Flags.test(1<<0))
	{
		for (u32 p_it=0; p_it<get_players_count(); ++p_it)
		{
			game_PlayerState* PS		=	get_it			(p_it);
			if (!PS) continue;
			if (PS->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD)) continue;
			CObject* pPlayer = Level().Objects.net_Find(PS->GameID);
			if (!pPlayer) continue;

			float r = .4f;
			T.identity();
			T.scale(r, r, r);
			T.translate_add(pPlayer->Position());
			Level().debug_renderer().draw_ellipse(T, TeamColors[PS->team]);
		};

	}
};
#endif
//  [7/5/2005]

BOOL	game_sv_GameState::IsVotingEnabled			()	{return g_sv_base_iVotingEnabled != 0;};
BOOL	game_sv_GameState::IsVotingEnabled			(u16 flag) {return (g_sv_base_iVotingEnabled&flag) != 0;};
#include "stdafx.h"
#include "../xr_ioconsole.h"
#include "../xr_ioc_cmd.h"
#include "level.h"
#include "xrServer.h"
#include "game_cl_base.h"
#include "actor.h"
#include "xrServer_Object_base.h"
#include "RegistryFuncs.h"
#include "gamepersistent.h"
#include "MainMenu.h"
#include "UIGameCustom.h"
#include "game_sv_deathmatch.h"
#include "game_sv_artefacthunt.h"
#include "date_time.h"
#include "game_cl_base_weapon_usage_statistic.h"

extern	float	g_cl_lvInterp;
extern	int		g_cl_InterpolationType; //0 - Linear, 1 - BSpline, 2 - HSpline
extern	u32		g_cl_InterpolationMaxPoints;
extern string64	gsCDKey;
extern	u32		g_dwMaxCorpses;
extern	float	g_fTimeFactor;
extern	BOOL	g_b_COD_PickUpMode		;
extern	int		g_iWeaponRemove			;
extern	int		g_iCorpseRemove			;
extern	BOOL	g_bCollectStatisticData ;
//extern	BOOL	g_bStatisticSaveAuto	;
extern	BOOL	g_SV_Disable_Auth_Check	;

extern  int		g_sv_mp_iDumpStatsPeriod;
extern	BOOL	g_SV_Force_Artefact_Spawn;
extern	int		g_Dump_Update_Write;
extern	int		g_Dump_Update_Read;
extern	u32		g_sv_base_dwRPointFreezeTime	;
extern	int		g_sv_base_iVotingEnabled		;
extern	BOOL	g_sv_mp_bSpectator_FreeFly		;
extern	BOOL	g_sv_mp_bSpectator_FirstEye		;
extern	BOOL	g_sv_mp_bSpectator_LookAt		;
extern	BOOL	g_sv_mp_bSpectator_FreeLook		;
extern	BOOL	g_sv_mp_bSpectator_TeamCamera	;
extern	BOOL	g_sv_mp_bCountParticipants		;
extern	float	g_sv_mp_fVoteQuota				;
extern	float	g_sv_mp_fVoteTime				;
extern	u32		g_sv_dm_dwForceRespawn			;
extern	s32		g_sv_dm_dwFragLimit				;
extern	s32		g_sv_dm_dwTimeLimit				;
extern	BOOL	g_sv_dm_bDamageBlockIndicators	;
extern	u32		g_sv_dm_dwDamageBlockTime		;
extern	BOOL	g_sv_dm_bAnomaliesEnabled		;
extern	u32		g_sv_dm_dwAnomalySetLengthTime	;
extern	BOOL	g_sv_dm_bPDAHunt				;
extern	u32		g_sv_dm_dwWarmUp_MaxTime		;
extern	BOOL	g_sv_dm_bDMIgnore_Money_OnBuy	;
extern	BOOL	g_sv_tdm_bAutoTeamBalance		;
extern	BOOL	g_sv_tdm_bAutoTeamSwap			;
extern	BOOL	g_sv_tdm_bFriendlyIndicators	;
extern	BOOL	g_sv_tdm_bFriendlyNames			;
extern	float	g_sv_tdm_fFriendlyFireModifier	;
extern	BOOL	g_bLeaveTDemo;
extern	int		g_sv_tdm_iTeamKillLimit			;
extern	int		g_sv_tdm_bTeamKillPunishment	;
extern	u32		g_sv_ah_dwArtefactRespawnDelta	;
extern	int		g_sv_ah_dwArtefactsNum			;
extern	u32		g_sv_ah_dwArtefactStayTime		;
extern	int		g_sv_ah_iReinforcementTime		;
extern	BOOL	g_sv_ah_bBearerCantSprint		;
extern	BOOL	g_sv_ah_bShildedBases			;
extern	BOOL	g_sv_ah_bAfReturnPlayersToBases ;
extern	u32		g_dwDemoDeltaFrame;
extern u32		g_sv_dwMaxClientPing;
extern	int		g_be_message_out;

extern	int		g_sv_Skip_Winner_Waiting;
extern	int 	g_sv_Wait_For_Players_Ready;
extern	int 	G_DELAYED_ROUND_TIME;	
extern	int		g_sv_Pending_Wait_Time;
extern	int		g_sv_Client_Reconnect_Time;
		int		g_dwEventDelay			= 0	;

void XRNETSERVER_API DumpNetCompressorStats	(bool brief);
BOOL XRNETSERVER_API g_net_compressor_enabled;
BOOL XRNETSERVER_API g_net_compressor_gather_stats;

class CCC_Restart : public IConsole_Command {
public:
					CCC_Restart		(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void	Execute			(LPCSTR args) 
	{
		if (!OnServer())			return;
		if(Level().Server)
		{
			Level().Server->game->round_end_reason = eRoundEnd_GameRestarted;
			Level().Server->game->OnRoundEnd();
		}
	}
	virtual void	Info	(TInfo& I){strcpy(I,"restart game");}
};

class CCC_RestartFast : public IConsole_Command {
public:
					CCC_RestartFast	(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void	Execute			(LPCSTR args) 
	{
		if (!OnServer())		
									return;
		if(Level().Server)
		{
			Level().Server->game->round_end_reason = eRoundEnd_GameRestartedFast;
			Level().Server->game->OnRoundEnd();
		}
	}
	virtual void	Info			(TInfo& I) {strcpy(I,"restart game fast");}
};

class CCC_Kill : public IConsole_Command {
public:
					CCC_Kill		(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void	Execute			(LPCSTR args) 
	{
		if (IsGameTypeSingle())		
										return;
		
		if (Game().local_player && 
			Game().local_player->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD)) 
										return;
		
		CObject *l_pObj					= Level().CurrentControlEntity();
		CActor *l_pPlayer				= smart_cast<CActor*>(l_pObj);
		if(l_pPlayer) 
		{
			NET_Packet					P;
			l_pPlayer->u_EventGen		(P,GE_GAME_EVENT,l_pPlayer->ID()	);
			P.w_u16						(GAME_EVENT_PLAYER_KILL);
			P.w_u16						(u16(l_pPlayer->ID())	);
			l_pPlayer->u_EventSend		(P);
		}
	}
	virtual void	Info	(TInfo& I)	{ strcpy(I,"player kill"); }
};

class CCC_Net_CL_Resync : public IConsole_Command {
public:
						CCC_Net_CL_Resync	(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void		Execute				(LPCSTR args) 
	{
		Level().net_Syncronize();
	}
	virtual void	Info	(TInfo& I)		{strcpy(I,"resyncronize client");}
};

class CCC_Net_CL_ClearStats : public IConsole_Command {
public:
						CCC_Net_CL_ClearStats	(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void		Execute					(LPCSTR args)
	{
		Level().ClearStatistic();
	}
	virtual void		Info	(TInfo& I){strcpy(I,"clear client net statistic");}
};

class CCC_Net_SV_ClearStats : public IConsole_Command {
public:
						CCC_Net_SV_ClearStats	(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void		Execute					(LPCSTR args) 
	{
		Level().Server->ClearStatistic();
	}
	virtual void	Info	(TInfo& I){strcpy(I,"clear server net statistic"); }
};

#ifdef DEBUG
class CCC_Dbg_NumObjects : public IConsole_Command {
public:
						CCC_Dbg_NumObjects	(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void		Execute				(LPCSTR args) 
	{
		
		u32 SVObjNum	= (OnServer()) ? Level().Server->GetEntitiesNum() : 0;
		xr_vector<u16>	SObjID;
		for (u32 i=0; i<SVObjNum; i++)
		{
			CSE_Abstract* pEntity = Level().Server->GetEntity(i);
			SObjID.push_back(pEntity->ID);
		};
		std::sort(SObjID.begin(), SObjID.end());

		u32 CLObjNum	= Level().Objects.o_count();
		xr_vector<u16>	CObjID;
		for (i=0; i<CLObjNum; i++)
		{
			CObjID.push_back(Level().Objects.o_get_by_iterator(i)->ID());
		};
		std::sort(CObjID.begin(), CObjID.end());

		Msg("Client Objects : %d", CLObjNum);
		Msg("Server Objects : %d", SVObjNum);

		for (u32 CO= 0; CO<_max(CLObjNum, SVObjNum); CO++)
		{
			if (CO < CLObjNum && CO < SVObjNum)
			{
				CSE_Abstract* pEntity = Level().Server->ID_to_entity(SObjID[CO]);
				CObject* pObj = Level().Objects.net_Find(CObjID[CO]);
				char color = (pObj->ID() == pEntity->ID) ? '-' : '!';

				Msg("%c%4d: Client - %20s[%5d] <===> Server - %s [%d]", color, CO+1, 
					*(pObj->cNameSect()), pObj->ID(),
					pEntity->s_name.c_str(), pEntity->ID);
			}
			else
			{
				if (CO<CLObjNum)
				{
					CObject* pObj = Level().Objects.net_Find(CObjID[CO]);
					Msg("! %2d: Client - %s [%d] <===> Server - -----------------", CO+1, 
						*(pObj->cNameSect()), pObj->ID());
				}
				else
				{
					CSE_Abstract* pEntity = Level().Server->ID_to_entity(SObjID[CO]);
					Msg("! %2d: Client - ----- <===> Server - %s [%d]", CO+1, 
						pEntity->s_name.c_str(), pEntity->ID);
				}
			}
		};

		Msg("Client Objects : %d", CLObjNum);
		Msg("Server Objects : %d", SVObjNum);
	}
	virtual void	Info	(TInfo& I){strcpy(I,"dbg Num Objects"); }
};
#endif // DEBUG

class CCC_GSCDKey: public CCC_String{
public:
						CCC_GSCDKey		(LPCSTR N, LPSTR V, int _size) : CCC_String(N, V, _size)  { bEmptyArgsHandled = false; };
	virtual void		Execute			(LPCSTR arguments)
	{
		CCC_String::Execute(arguments);	

		WriteRegistry_StrValue(REGISTRY_VALUE_GSCDKEY, value);

		if (g_pGamePersistent && MainMenu()) MainMenu()->ValidateCDKey();
	}
	virtual void		Save			(IWriter *F)	{};
};

class CCC_KickPlayerByName : public IConsole_Command {
public:
					CCC_KickPlayerByName(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = false; };
	virtual void	Execute				(LPCSTR args) 
	{
		if (!OnServer())		return;

		if (!xr_strlen(args))	return;
		if (strchr(args, '/'))
		{
			Msg("!  '/' is not allowed in names!");
			return;
		}
		string4096 PlayerName	= "";
		if (xr_strlen(args)>17)
		{
			strncpy				(PlayerName, args, 17);
			PlayerName[17]		= 0;
		}else
			strcpy(PlayerName, args);

		xr_strlwr			(PlayerName);

		Level().Server->clients_Lock();
		u32	cnt					= Level().Server->game->get_players_count();
		for(u32 it=0; it<cnt; it++)	
		{
			xrClientData *l_pC = (xrClientData*)	Level().Server->client_Get	(it);
			if (l_pC)
			{
				string64			_low_name;
				strcpy				(_low_name,l_pC->ps->getName());
				xr_strlwr			(_low_name);

				if (!xr_strcmp(_low_name, PlayerName))
				{
					if (Level().Server->GetServerClient() != l_pC)
					{
						Msg("Disconnecting : %s", l_pC->ps->getName());
						Level().Server->DisconnectClient(l_pC);
						break;
					}else
						Msg("! Can't disconnect server's client");
				}
			}			
		};
		if (it == cnt) 
		{
			Msg("! No such player found : %s", PlayerName);
		}
		Level().Server->clients_Unlock();		
	};

	virtual void	Info	(TInfo& I)	{strcpy(I,"Kick Player by name"); }
};


class CCC_BanPlayerByName : public IConsole_Command {
public:
					CCC_BanPlayerByName	(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = false; };
	virtual void	Execute				(LPCSTR args_) 
	{
		if (!g_pGameLevel || !Level().Server || !Level().Server->game) return;
		string4096				buff;
		strcpy					(buff, args_);
		u32 len					= xr_strlen(buff);
		
		if (0==len) 
			return;

		string1024				digits;
		LPSTR					p = buff+len-1;
		while(isdigit(*p))
		{
			if (p == buff) break;
			--p;
		}
		R_ASSERT				(p>=buff);
		strcpy					(digits,p);
		*p						= 0;
		if (!xr_strlen(buff))
		{
			Msg("incorrect parameter passed. bad name.");
			return;
		}
		u32 ban_time			= atol(digits);
		if(ban_time==0)
		{
			Msg("incorrect parameters passed.  name and time required");
			return;
		}
		string4096 PlayerName		= "";
		if (xr_strlen(buff)>17)
		{

			strncpy				(PlayerName, buff, 17);
			PlayerName[17]		= 0;
		}else
			strcpy				(PlayerName, buff);

		xr_strlwr			(PlayerName);

		Level().Server->clients_Lock();
		u32	cnt					= Level().Server->game->get_players_count();
		for(u32 it=0; it<cnt; it++)	
		{
			xrClientData *l_pC = (xrClientData*)	Level().Server->client_Get	(it);
			if (l_pC)
			{
				string64			_low_name;
				strcpy				(_low_name,l_pC->ps->getName());
				xr_strlwr			(_low_name);

				if (!xr_strcmp(_low_name, PlayerName))
				{
					if (Level().Server->GetServerClient() != l_pC)
					{
						Msg("Disconnecting and Banning: %s", l_pC->ps->getName());
						Level().Server->BanClient(l_pC, ban_time);
						Level().Server->DisconnectClient(l_pC);
						break;
					}else
					{
						Msg("! Can't disconnect server's client");
						break;
					}
				}
			}			
		};
		if (it == cnt)
			Msg("! No such player found : %s", PlayerName);

		Level().Server->clients_Unlock();	
	};

	virtual void	Info	(TInfo& I){strcpy(I,"Ban Player by Name"); }
};


class CCC_BanPlayerByIP : public IConsole_Command {
public:
					CCC_BanPlayerByIP	(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = false; };
	virtual void	Execute				(LPCSTR args_) 
	{
		if (!g_pGameLevel || !Level().Server) return;
//-----------
		string4096				buff;
		strcpy					(buff, args_);
		u32 len					= xr_strlen(buff);
		
		if (0==len) 
			return;

		string1024				digits;
		LPSTR					p = buff+len-1;
		while(isdigit(*p))
		{
			if (p == buff) break;
			--p;
		}
		R_ASSERT				(p>=buff);
		strcpy					(digits,p);
		*p						= 0;
		if (!xr_strlen(buff))
		{
			Msg("incorrect parameter passed. bad IP address.");
			return;
		}
		u32 ban_time			= atol(digits);
		if(ban_time==0)
		{
			Msg("incorrect parameters passed.  IP and time required");
			return;
		}

		string1024				s_ip_addr;
		strcpy					(s_ip_addr, buff);
//-----------

		ip_address							Address;
		Address.set							(s_ip_addr);
		Level().Server->clients_Lock		();
		Msg									("Disconnecting and Banning: %s",Address.to_string().c_str() ); 
		Level().Server->BanAddress			(Address, ban_time);
		Level().Server->DisconnectAddress	(Address);
		Level().Server->clients_Unlock		();
	};

	virtual void	Info	(TInfo& I){strcpy(I,"Ban Player by IP"); }
};

class CCC_UnBanPlayerByIP : public IConsole_Command {
public:
					CCC_UnBanPlayerByIP	(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = false; };
	virtual void	Execute				(LPCSTR args) 
	{
		if (!g_pGameLevel || !Level().Server) return;

		if (!xr_strlen(args)) return;

		ip_address						Address;
		Address.set						(args);
		Level().Server->clients_Lock	();
		Level().Server->UnBanAddress	(Address);
		Level().Server->clients_Unlock	();
	};

	virtual void	Info	(TInfo& I){strcpy(I,"UnBan Player by IP");}
};

class CCC_ListPlayers : public IConsole_Command {
public:
					CCC_ListPlayers	(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void	Execute			(LPCSTR args) 
	{
		if (!OnServer())	return;
		
		u32	cnt = Level().Server->game->get_players_count();
		Msg("------------------------");
		Msg("- Total Players : %d", cnt);
		for(u32 it=0; it<cnt; it++)	
		{
			xrClientData *l_pC	= (xrClientData*)	Level().Server->client_Get	(it);
			if (!l_pC)			continue;
			ip_address			Address;
			DWORD dwPort		= 0;

			Level().Server->GetClientAddress(l_pC->ID, Address, &dwPort);
			Msg("%d : %s - %s port[%u] ping[%u]", it+1, l_pC->ps->getName(),
				Address.to_string().c_str(),
				dwPort,
				l_pC->ps->ping);
		};
		Msg("------------------------");
	};

	virtual void	Info	(TInfo& I){strcpy(I,"List Players"); }
};

class CCC_ListPlayers_Banned : public IConsole_Command {
public:
					CCC_ListPlayers_Banned	(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void	Execute					(LPCSTR args) 
	{
		if (!OnServer())	return;
		Msg("------------------------");
        Level().Server->Print_Banned_Addreses();
		Msg("------------------------");
	};

	virtual void	Info	(TInfo& I){strcpy(I,"List of Banned Players"); }
};
class CCC_ChangeLevelGameType : public IConsole_Command {
public:
					CCC_ChangeLevelGameType	(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = false; };
	virtual void	Execute					(LPCSTR args) 
	{
		if (!OnServer())	return;

		string256		LevelName;	
		LevelName[0]	=0;
		string256		GameType;	
		GameType[0]		=0;
		
		sscanf	(args,"%s %s", LevelName, GameType);

		if (!xr_strcmp(GameType, "dm")) sprintf_s(GameType, "deathmatch");
		else
			if (!xr_strcmp(GameType, "tdm")) sprintf_s(GameType, "teamdeathmatch");
			else
				if (!xr_strcmp(GameType, "artifacthunt")) sprintf_s(GameType, "artefacthunt");
				else
					if (!xr_strcmp(GameType, "ah")) sprintf_s(GameType, "artefacthunt");

		if (xr_strcmp(GameType, "deathmatch"))
			if (xr_strcmp(GameType, "teamdeathmatch"))
				if (xr_strcmp(GameType, "artefacthunt"))
				{
					Msg ("! Unknown gametype - %s", GameType);
					return;
				};
		//-----------------------------------------
		s32 GameTypeID = 0;
		if (!xr_strcmp(GameType, "deathmatch")) GameTypeID = GAME_DEATHMATCH;
		else
			if (!xr_strcmp(GameType, "teamdeathmatch")) GameTypeID = GAME_TEAMDEATHMATCH;
			else
				if (!xr_strcmp(GameType, "artefacthunt")) GameTypeID = GAME_ARTEFACTHUNT;
		//-----------------------------------------

		const SGameTypeMaps& M		= gMapListHelper.GetMapListFor((EGameTypes)GameTypeID);
		u32 cnt						= M.m_map_names.size();
		bool bMapFound				= false;
		for(u32 i=0; i<cnt; ++i)
		{
			const shared_str& _map_name = M.m_map_names[i];
			if ( 0==xr_strcmp(_map_name.c_str(), LevelName) )
			{
				bMapFound = true;
				break;
			}
		}
		if (!bMapFound)
		{
			Msg("! Level [%s] not registered for [%s]!", LevelName, GameType);
#ifdef NDEBUG
			return;
#endif
		}

		NET_Packet			P;
		P.w_begin			(M_CHANGE_LEVEL_GAME);
		P.w_stringZ			(LevelName);
		P.w_stringZ			(GameType);
		Level().Send		(P);
	};

	virtual void	Info	(TInfo& I)	{strcpy(I,"Changing level and game type"); }
};

class CCC_ChangeGameType : public CCC_ChangeLevelGameType {
public:
					CCC_ChangeGameType	(LPCSTR N) : CCC_ChangeLevelGameType(N)  { bEmptyArgsHandled = false; };
	virtual void	Execute				(LPCSTR args) 
	{

		if (!OnServer())	return;

		string256			GameType;	
		GameType[0]			=0;
		sscanf				(args,"%s", GameType);

		string1024			argsNew;
		sprintf_s				(argsNew, "%s %s", Level().name().c_str(), GameType);

		CCC_ChangeLevelGameType::Execute((LPCSTR)argsNew);
	};

	virtual void	Info	(TInfo& I)	{strcpy(I,"Changing Game Type"); };
};

class CCC_ChangeLevel : public CCC_ChangeLevelGameType {
public:
					CCC_ChangeLevel	(LPCSTR N) : CCC_ChangeLevelGameType(N)  { bEmptyArgsHandled = false; };
	virtual void	Execute			(LPCSTR args) 
	{
		if (!OnServer())	return;

		string256		LevelName;	
		LevelName[0]	=0;
		sscanf			(args,"%s", LevelName);

		string1024		argsNew;
		sprintf_s			(argsNew, "%s %s", LevelName, Level().Server->game->type_name());

		CCC_ChangeLevelGameType::Execute((LPCSTR)argsNew);
	};

	virtual void	Info	(TInfo& I){	strcpy(I,"Changing Game Type"); }
};

class CCC_AddMap : public IConsole_Command {
public:
	CCC_AddMap(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = false; };
	virtual void Execute(LPCSTR args) 
	{
		char	MapName[256] = {0};
		sscanf	(args,"%s", MapName);
		if (!g_pGameLevel || !Level().Server || !Level().Server->game) return;
		Level().Server->game->MapRotation_AddMap(MapName);
	};

	virtual void	Info	(TInfo& I)		
	{
		strcpy(I,"Adds map to map rotation list"); 
	}
};

class CCC_ListMaps : public IConsole_Command {
public:
					CCC_ListMaps	(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void	Execute			(LPCSTR args) 
	{
		if (!g_pGameLevel || !Level().Server || !Level().Server->game) return;
		Level().Server->game->MapRotation_ListMaps();
	};

	virtual void	Info	(TInfo& I){strcpy(I,"List maps in map rotation list"); }
};

class CCC_NextMap : public IConsole_Command {
public:
					CCC_NextMap		(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void	Execute			(LPCSTR args) 
	{
		if (!OnServer())	return;

		Level().Server->game->OnNextMap();
	};

	virtual void	Info	(TInfo& I){strcpy(I,"Switch to Next Map in map rotation list"); }
};

class CCC_PrevMap : public IConsole_Command {
public:
	CCC_PrevMap(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void Execute(LPCSTR args) 
	{
		if (!OnServer())	return;

		Level().Server->game->OnPrevMap();
	};

	virtual void	Info	(TInfo& I)	{strcpy(I,"Switch to Previous Map in map rotation list"); }
};

class CCC_AnomalySet : public IConsole_Command {
public:
	CCC_AnomalySet(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = false; };
	virtual void Execute(LPCSTR args) 
	{
		if (!OnServer())		return;

		game_sv_Deathmatch* gameDM = smart_cast<game_sv_Deathmatch *>(Level().Server->game);
		if (!gameDM) return;

		string256			AnomalySet;		
		sscanf				(args,"%s", AnomalySet);
		gameDM->StartAnomalies(atol(AnomalySet));
	};

	virtual void	Info	(TInfo& I)	{strcpy(I,"Activating pointed Anomaly set"); }
};

class CCC_Vote_Start : public IConsole_Command {
public:
					CCC_Vote_Start		(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = false; };
	virtual void	Execute				(LPCSTR args) 
	{
		if (IsGameTypeSingle())
		{
			Msg("! Only for multiplayer games!");
			return;
		}

		if (!Game().IsVotingEnabled())
		{
			Msg("! Voting is disabled by server!");
			return;
		}
		if (Game().IsVotingActive())
		{
			Msg("! There is voting already!");
			return;
		}

		if (Game().Phase() != GAME_PHASE_INPROGRESS)
		{
			Msg("! Voting is allowed only when game is in progress!");
			return;
		};

		Game().SendStartVoteMessage(args);		
	};

	virtual void	Info	(TInfo& I){strcpy(I,"Starts Voting"); };
};

class CCC_Vote_Stop : public IConsole_Command {
public:
					CCC_Vote_Stop	(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void	Execute			(LPCSTR args) 
	{
		if (!OnServer()) return;

		if (IsGameTypeSingle())
		{
			Msg("! Only for multiplayer games!");
			return;
		}

		if (!Level().Server->game->IsVotingEnabled())
		{
			Msg("! Voting is disabled by server!");
			return;
		}

		if (!Level().Server->game->IsVotingActive())
		{
			Msg("! Currently there is no active voting!");
			return;
		}

		if (Level().Server->game->Phase() != GAME_PHASE_INPROGRESS)
		{
			Msg("! Voting is allowed only when game is in progress!");
			return;
		};

		Level().Server->game->OnVoteStop();
	};

	virtual void	Info	(TInfo& I)	{strcpy(I,"Stops Current Voting"); };
};

class CCC_Vote_Yes : public IConsole_Command {
public:
					CCC_Vote_Yes(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void	Execute(LPCSTR args) 
	{
		if (IsGameTypeSingle())
		{
			Msg("! Only for multiplayer games!");
			return;
		}

		if (!Game().IsVotingEnabled())
		{
			Msg("! Voting is disabled by server!");
			return;
		}

		if (!Game().IsVotingActive())
		{
			Msg("! Currently there is no active voting!");
			return;
		}

		if (Game().Phase() != GAME_PHASE_INPROGRESS)
		{
			Msg("! Voting is allowed only when game is in progress!");
			return;
		};

		Game().SendVoteYesMessage();
	};

	virtual void	Info	(TInfo& I){strcpy(I,"Vote Yes"); };
};

class CCC_Vote_No : public IConsole_Command {
public:
					CCC_Vote_No	(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void	Execute		(LPCSTR args) 
	{
		if (IsGameTypeSingle())
		{
			Msg("! Only for multiplayer games!");
			return;
		}

		if (!Game().IsVotingEnabled())
		{
			Msg("! Voting is disabled by server!");
			return;
		}

		if (!Game().IsVotingActive())
		{
			Msg("! Currently there is no active voting!");
			return;
		}

		if (Game().Phase() != GAME_PHASE_INPROGRESS)
		{
			Msg("! Voting is allowed only when game is in progress!");
			return;
		};

		Game().SendVoteNoMessage();
	};

	virtual void	Info	(TInfo& I)	{strcpy(I,"Vote No"); };
};

class CCC_StartTimeEnvironment: public IConsole_Command {
public:
					CCC_StartTimeEnvironment	(LPCSTR N) : IConsole_Command(N) {};
	virtual void	Execute						(LPCSTR args)
	{
		u32 year = 1, month = 1, day = 1, hours = 0, mins = 0, secs = 0, milisecs = 0;
		
		sscanf				(args,"%d:%d:%d.%d",&hours,&mins,&secs,&milisecs);
		u64 NewTime			= generate_time	(year,month,day,hours,mins,secs,milisecs);

		if (!g_pGameLevel)
			return;

		if (!Level().Server)
			return;

		if (!Level().Server->game)
			return;

		Level().Server->game->SetEnvironmentGameTimeFactor(NewTime,g_fTimeFactor);
		Level().Server->game->SetGameTimeFactor(NewTime,g_fTimeFactor);
	}
};
class CCC_SetWeather : public IConsole_Command {
public:
					CCC_SetWeather	(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = false; };
	virtual void	Execute			(LPCSTR args) 
	{
		if (!g_pGamePersistent) return;
		if (!OnServer())		return;

		string256				weather_name;		
		weather_name[0]			= 0;
		sscanf					(args,"%s", weather_name);
		if (!weather_name[0])	return;
		g_pGamePersistent->Environment().SetWeather(weather_name);		
	};

	virtual void	Info	(TInfo& I){strcpy(I,"Set new weather"); }
};

class CCC_SaveStatistic : public IConsole_Command {
public:
					CCC_SaveStatistic	(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void	Execute				(LPCSTR args) {
		Game().m_WeaponUsageStatistic->SaveData();
	}
	virtual void	Info	(TInfo& I)	{strcpy(I,"saving statistic data"); }
};

class CCC_AuthCheck : public CCC_Integer {
public:
					CCC_AuthCheck	(LPCSTR N, int* V, int _min=0, int _max=999) :CCC_Integer(N,V,_min,_max){};
	  virtual void	Save			(IWriter *F)	{};
};

class CCC_ReturnToBase: public IConsole_Command {
public:
					CCC_ReturnToBase(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = false; };
	virtual void	Execute(LPCSTR args) 
	{
		if (!OnServer())						return;
		if (GameID() != GAME_ARTEFACTHUNT)		return;

		game_sv_ArtefactHunt* g = smart_cast<game_sv_ArtefactHunt*>(Level().Server->game);
		g->MoveAllAlivePlayers();
	}
};

class CCC_GetServerAddress : public IConsole_Command {
public:
					CCC_GetServerAddress	(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void	Execute					(LPCSTR args) 
	{
		ip_address Address;
		DWORD dwPort = 0;
		
		Level().GetServerAddress(Address, &dwPort);

		Msg("Server Address - %s:%i",Address.to_string().c_str(), dwPort);
	};

	virtual void	Info	(TInfo& I){strcpy(I,"List Players"); }
};

class CCC_StartTeamMoney : public IConsole_Command {
public:
					CCC_StartTeamMoney(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void	Execute(LPCSTR args) 
	{
		if (!OnServer())	return;

		game_sv_mp* pGameMP		= smart_cast<game_sv_Deathmatch *>(Level().Server->game);
		if (!pGameMP)		return;

		string128			Team = "";
		s32 TeamMoney		= 0;
		sscanf				(args,"%s %i", Team, &TeamMoney);

		if (!Team[0])
		{
			Msg("- --------------------");
			Msg("Teams start money:");
			u32 TeamCount = pGameMP->GetTeamCount();
			for (u32 i=0; i<TeamCount; i++)
			{
				TeamStruct* pTS = pGameMP->GetTeamData(i);
				if (!pTS) continue;
				Msg ("Team %d: %d", i, pTS->m_iM_Start);
			}
			Msg("- --------------------");
			return;
		}else
		{
			u32 TeamID			= 0;
			s32 TeamStartMoney	= 0;
			int cnt = sscanf				(args,"%i %i", &TeamID, &TeamStartMoney);
			if(cnt!=2)
			{
				Msg("invalid args. (int int) expected");
				return;
			}
			TeamStruct* pTS		= pGameMP->GetTeamData(TeamID);
			if (pTS) 
				pTS->m_iM_Start = TeamStartMoney;
		}
	};

	virtual void	Info	(TInfo& I)	{strcpy(I,"Set Start Team Money");}
};
class CCC_SV_Integer : public CCC_Integer {
public:
	CCC_SV_Integer(LPCSTR N, int* V, int _min=0, int _max=999) :CCC_Integer(N,V,_min,_max)  {};

	  virtual void	Execute	(LPCSTR args)
	  {
		  CCC_Integer::Execute(args);

		  if (!g_pGameLevel || !Level().Server || !Level().Server->game) return;

		  Level().Server->game->signal_Syncronize();
	  }
};

class CCC_SV_Float : public CCC_Float {
public:
	CCC_SV_Float(LPCSTR N, float* V, float _min=0, float _max=1) : CCC_Float(N,V,_min,_max) {};

	  virtual void	Execute	(LPCSTR args)
	  {
		  CCC_Float::Execute(args);
		  if (!g_pGameLevel || !Level().Server || !Level().Server->game) return;
		  Level().Server->game->signal_Syncronize();
	  }
};
class CCC_RadminCmd: public IConsole_Command
{
public:
	CCC_RadminCmd(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = false; };
	virtual void Execute(LPCSTR arguments)
	{
		if(IsGameTypeSingle())
			return;

		if(strstr(arguments,"login")==arguments)
		{
			string256			user;
			string256			pass;
			if(2==sscanf		(arguments+xr_strlen("login")+1, "%s %s", user, pass))
			{
				NET_Packet		P;			
				P.w_begin		(M_REMOTE_CONTROL_AUTH);
				P.w_stringZ		(user);
				P.w_stringZ		(pass);

				Level().Send(P,net_flags(TRUE,TRUE));
			}else
				Msg("2 args(user pass) needed");
		}
		else
		if(strstr(arguments,"logout")==arguments)
		{
			NET_Packet		P;			
			P.w_begin		(M_REMOTE_CONTROL_AUTH);
			P.w_stringZ		("logoff");

			Level().Send(P,net_flags(TRUE,TRUE));
		}//logoff
		else
		{
			NET_Packet		P;			
			P.w_begin		(M_REMOTE_CONTROL_CMD);
			P.w_stringZ		(arguments);

			Level().Send(P,net_flags(TRUE,TRUE));
		}
	}
	virtual void	Save	(IWriter *F)	{};
};

class CCC_SwapTeams : public IConsole_Command {
public:
					CCC_SwapTeams(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void	Execute(LPCSTR args) {
		if (!OnServer()) return;
		if(Level().Server && Level().Server->game) 
		{
			game_sv_ArtefactHunt* pGame = smart_cast<game_sv_ArtefactHunt *>(Level().Server->game);
			if (pGame)
			{
				pGame->SwapTeams();
				Level().Server->game->round_end_reason = eRoundEnd_GameRestartedFast;
				Level().Server->game->OnRoundEnd();
			}			
		}
	}
	virtual void	Info	(TInfo& I){strcpy(I,"swap teams for artefacthunt game"); }
};


#ifdef BATTLEYE

class CCC_BattlEyeSrv : public IConsole_Command
{
public:
	CCC_BattlEyeSrv( LPCSTR N ) : IConsole_Command(N)  { bEmptyArgsHandled = true; }
	bool  ExecuteGetState( LPCSTR args )
	{
		if ( args[0] == 0 )
		{
			if ( Level().battleye_system.server )
			{
				Msg( "BattlEye Server is enabled" );
			}
			else
			{
				Msg( "BattlEye Server is disabled" );
			}
			return true;
		}
		return false;
	}
	bool  ExecuteBattlEyeServerCmd( LPCSTR args )
	{
		if ( Level().battleye_system.server )
		{
			Level().battleye_system.server->Command( (char*)args );
			return true;
		}
		return false;
	}
	virtual void  Execute( LPCSTR args )
	{
		if ( !g_pGameLevel || !OnServer() )
		{
			return;
		}
		if ( ExecuteGetState( args ) ) return;
		ExecuteBattlEyeServerCmd( args );
	}
	virtual void Info( TInfo& I )
	{
		strcpy( I, "BattlEye Server commands" );
	}
};

class CCC_BattlEyeCl : public IConsole_Command
{
public:
	CCC_BattlEyeCl( LPCSTR N ) : IConsole_Command(N)  { bEmptyArgsHandled = true; }
	bool  ExecuteGetState( LPCSTR args )
	{
		if ( args[0] == 0 )
		{
			if ( Level().battleye_system.client )
			{
				Msg("BattlEye Client is enabled");
			}
			else
			{
				Msg("BattlEye Client is disabled");
			}
			return true;
		}
		return false;
	}
	bool ExecuteBattlEyeClientCmd( LPCSTR args )
	{
		if ( Level().battleye_system.client )
		{
			Level().battleye_system.client->Command( (char*)args );
			return true;
		}
		return false;
	}
	virtual void Execute( LPCSTR args )
	{
		if ( !g_pGameLevel )
		{
			return;
		}
		if ( ExecuteGetState( args ) ) return;
		ExecuteBattlEyeClientCmd( args );
	}

	virtual void  Info( TInfo& I )
	{
		strcpy( I, "BattlEye Client commands" );
	}
};

#endif // BATTLEYE

class CCC_Name : public IConsole_Command
{
public:
	CCC_Name(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = false; };
	virtual void	Status	(TStatus& S)
	{ 
		S[0]=0;
		if( IsGameTypeSingle() )									return;
		if (!(&Level()))											return;
		if (!(&Game()))												return;
		if( !Game().local_player || !Game().local_player->name )	return;
		sprintf_s( S, "is \"%s\" ", Game().local_player->name );
	}

	virtual void	Save	(IWriter *F)	{}

	virtual void Execute(LPCSTR args) 
	{
		if (IsGameTypeSingle())		return;
		if (!(&Level()))			return;
		if (!(&Game()))				return;
		if (!Game().local_player)	return;
		
		if (!xr_strlen(args)) return;
		if (strchr(args, '/'))
		{
			Msg("!  '/' is not allowed in names!");
			return;
		}
		string4096 NewName = "";
		if (xr_strlen(args)>17)
		{
			strncpy(NewName, args, 17);
			NewName[17] = 0;
		}
		else
			strcpy(NewName, args);
	
		NET_Packet				P;
		Game().u_EventGen		(P,GE_GAME_EVENT,Game().local_player->GameID);
		P.w_u16					(GAME_EVENT_PLAYER_NAME);
		P.w_stringZ				(NewName);
		Game().u_EventSend		(P);
	}

	virtual void	Info	(TInfo& I)	{strcpy(I,"player name"); }
};

class CCC_SvStatus : public IConsole_Command {
public:
					CCC_SvStatus(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void	Execute(LPCSTR args) {
		if (!OnServer()) return;
		if(Level().Server && Level().Server->game) 
		{
			Console->Execute		("cfg_load all_server_settings");
		}
	}
	virtual void	Info	(TInfo& I){strcpy(I,"Shows current server settings"); }
};

class CCC_SvChat : public IConsole_Command{
public:
					CCC_SvChat(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = false; };
	virtual void	Execute(LPCSTR args) {
		if (!OnServer())	return;
		if(Level().Server && Level().Server->game) 
		{
			game_sv_mp* game = smart_cast<game_sv_mp*>(Level().Server->game);
			if(game)
				game->SvSendChatMessage(args);
		}
	}
};

class CCC_MpStatistics : public IConsole_Command {
public:
					CCC_MpStatistics(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = true; };
	virtual void	Execute(LPCSTR args) {
		if (!OnServer()) return;
		if(Level().Server && Level().Server->game) 
		{
			Level().Server->game->DumpOnlineStatistic	();
		}
	}
	virtual void	Info	(TInfo& I){strcpy(I,"Shows current server settings"); }
};
class CCC_CompressorStatus : public IConsole_Command {
public:
					CCC_CompressorStatus(LPCSTR N) : IConsole_Command(N)  { bEmptyArgsHandled = false; };
	virtual void	Execute(LPCSTR args) 
	{
		if(strstr(args,"info_full"))
			DumpNetCompressorStats	(false);
		else
		if(strstr(args,"info"))
			DumpNetCompressorStats	(true);
		else
			InvalidSyntax		();
	}
	virtual void	Info	(TInfo& I){strcpy(I,"valid arguments is [info info_full on off]"); }
};

void register_mp_console_commands()
{
	CMD1(CCC_Restart,				"g_restart"				);
	CMD1(CCC_RestartFast,			"g_restart_fast"		);
	CMD1(CCC_Kill,					"g_kill"				);

	// Net Interpolation
	CMD4(CCC_Float,					"net_cl_interpolation",		&g_cl_lvInterp,				-1,1);
	CMD4(CCC_Integer,				"net_cl_icurvetype",		&g_cl_InterpolationType,	0, 2)	;
	CMD4(CCC_Integer,				"net_cl_icurvesize",		(int*)&g_cl_InterpolationMaxPoints,	0, 2000)	;
	
	CMD1(CCC_Net_CL_Resync,			"net_cl_resync" );
	CMD1(CCC_Net_CL_ClearStats,		"net_cl_clearstats" );
	CMD1(CCC_Net_SV_ClearStats,		"net_sv_clearstats" );

	// Network
#ifdef DEBUG
	CMD4(CCC_Integer,	"net_cl_update_rate",	&psNET_ClientUpdate,20,		100				);
	CMD4(CCC_Integer,	"net_cl_pending_lim",	&psNET_ClientPending,0,		10				);
#endif
	CMD4(CCC_Integer,	"net_sv_update_rate",	&psNET_ServerUpdate,1,		100				);
	CMD4(CCC_Integer,	"net_sv_pending_lim",	&psNET_ServerPending,0,		10				);
	CMD4(CCC_Integer,	"net_sv_gpmode",	    &psNET_GuaranteedPacketMode,0, 2)	;
	CMD3(CCC_Mask,		"net_sv_log_data",		&psNET_Flags,		NETFLAG_LOG_SV_PACKETS	);
	CMD3(CCC_Mask,		"net_cl_log_data",		&psNET_Flags,		NETFLAG_LOG_CL_PACKETS	);
#ifdef DEBUG
	CMD3(CCC_Mask,		"net_dump_size",		&psNET_Flags,		NETFLAG_DBG_DUMPSIZE	);
	CMD1(CCC_Dbg_NumObjects,"net_dbg_objects"				);
#endif // DEBUG
	CMD3(CCC_GSCDKey,	"cdkey",				gsCDKey,			sizeof(gsCDKey)			);
	CMD4(CCC_Integer,	"g_eventdelay",			&g_dwEventDelay,	0,	1000);
	CMD4(CCC_Integer,	"g_corpsenum",			(int*)&g_dwMaxCorpses,		0,	100);


	CMD1(CCC_KickPlayerByName,	"sv_kick"					);

	CMD1(CCC_BanPlayerByName,	"sv_banplayer"				);
	CMD1(CCC_BanPlayerByIP,		"sv_banplayer_ip"			);

	CMD1(CCC_UnBanPlayerByIP,	"sv_unbanplayer_ip"			);

	CMD1(CCC_ListPlayers,			"sv_listplayers"			);		
	CMD1(CCC_ListPlayers_Banned,	"sv_listplayers_banned"			);		
	
	CMD1(CCC_ChangeGameType,		"sv_changegametype"			);
	CMD1(CCC_ChangeLevel,			"sv_changelevel"			);
	CMD1(CCC_ChangeLevelGameType,	"sv_changelevelgametype"	);	

	CMD1(CCC_AddMap,		"sv_addmap"				);	
	CMD1(CCC_ListMaps,		"sv_listmaps"				);	
	CMD1(CCC_NextMap,		"sv_nextmap"				);	
	CMD1(CCC_PrevMap,		"sv_prevmap"				);
	CMD1(CCC_AnomalySet,	"sv_nextanomalyset"			);

	CMD1(CCC_Vote_Start,	"cl_votestart"				);
	CMD1(CCC_Vote_Stop,		"sv_votestop"				);
	CMD1(CCC_Vote_Yes,		"cl_voteyes"				);
	CMD1(CCC_Vote_No,		"cl_voteno"				);

	CMD1(CCC_StartTimeEnvironment,	"sv_setenvtime");

	CMD1(CCC_SetWeather,	"sv_setweather"			);

	CMD4(CCC_Integer,		"cl_cod_pickup_mode",	&g_b_COD_PickUpMode,	0, 1)	;

	CMD4(CCC_Integer,		"sv_remove_weapon",		&g_iWeaponRemove, -1, 1);
	CMD4(CCC_Integer,		"sv_remove_corpse",		&g_iCorpseRemove, -1, 1);

	CMD4(CCC_Integer,		"sv_statistic_collect", &g_bCollectStatisticData, 0, 1);
	CMD1(CCC_SaveStatistic,	"sv_statistic_save");
//	CMD4(CCC_Integer,		"sv_statistic_save_auto", &g_bStatisticSaveAuto, 0, 1);

	CMD4(CCC_AuthCheck,		"sv_no_auth_check",				&g_SV_Disable_Auth_Check, 0, 1);

	CMD4(CCC_Integer,		"sv_artefact_spawn_force",		&g_SV_Force_Artefact_Spawn, 0, 1);

	CMD4(CCC_Integer,		"net_dbg_dump_update_write",	&g_Dump_Update_Write, 0, 1);
	CMD4(CCC_Integer,		"net_dbg_dump_update_read",	&g_Dump_Update_Read, 0, 1);

	CMD1(CCC_ReturnToBase,	"sv_return_to_base");
	CMD1(CCC_GetServerAddress,"get_server_address");		

#ifdef DEBUG
	CMD4(CCC_Integer,		"cl_leave_tdemo",		&g_bLeaveTDemo, 0, 1);

	CMD4(CCC_Integer,		"sv_skip_winner_waiting",		&g_sv_Skip_Winner_Waiting, 0, 1);

	CMD4(CCC_Integer,		"sv_wait_for_players_ready",	&g_sv_Wait_For_Players_Ready, 0, 1);
#endif
	CMD1(CCC_StartTeamMoney,"sv_startteammoney"		);		

	CMD4(CCC_Integer,		"sv_hail_to_winner_time",		&G_DELAYED_ROUND_TIME, 0, 60);

	//. CMD4(CCC_Integer,		"sv_pending_wait_time",		&g_sv_Pending_Wait_Time, 0, 60000);

	CMD4(CCC_Integer,		"sv_client_reconnect_time",		&g_sv_Client_Reconnect_Time, 0, 60);

	CMD4(CCC_SV_Integer,	"sv_rpoint_freeze_time", (int*)&g_sv_base_dwRPointFreezeTime, 0, 60000);
	CMD4(CCC_SV_Integer,	"sv_vote_enabled", &g_sv_base_iVotingEnabled, 0, 0x00FF);

	CMD4(CCC_SV_Integer,	"sv_spectr_freefly"			,	(int*)&g_sv_mp_bSpectator_FreeFly	, 0, 1);
	CMD4(CCC_SV_Integer,	"sv_spectr_firsteye"		,	(int*)&g_sv_mp_bSpectator_FirstEye	, 0, 1);
	CMD4(CCC_SV_Integer,	"sv_spectr_lookat"			,	(int*)&g_sv_mp_bSpectator_LookAt	, 0, 1);
	CMD4(CCC_SV_Integer,	"sv_spectr_freelook"		,	(int*)&g_sv_mp_bSpectator_FreeLook	, 0, 1);
	CMD4(CCC_SV_Integer,	"sv_spectr_teamcamera"		,	(int*)&g_sv_mp_bSpectator_TeamCamera, 0, 1);	
	
	CMD4(CCC_SV_Integer,	"sv_vote_participants"		,	(int*)&g_sv_mp_bCountParticipants	,	0,	1);	
	CMD4(CCC_SV_Float,		"sv_vote_quota"				,	&g_sv_mp_fVoteQuota					, 0.0f,1.0f);
	CMD4(CCC_SV_Float,		"sv_vote_time"				,	&g_sv_mp_fVoteTime					, 0.5f,10.0f);

	CMD4(CCC_SV_Integer,	"sv_forcerespawn"			,	(int*)&g_sv_dm_dwForceRespawn		,	0,3600);	//sec
	CMD4(CCC_SV_Integer,	"sv_fraglimit"				,	&g_sv_dm_dwFragLimit				,	0,100);
	CMD4(CCC_SV_Integer,	"sv_timelimit"				,	&g_sv_dm_dwTimeLimit				,	0,180);		//min
	CMD4(CCC_SV_Integer,	"sv_dmgblockindicator"		,	(int*)&g_sv_dm_bDamageBlockIndicators,	0, 1);
	CMD4(CCC_SV_Integer,	"sv_dmgblocktime"			,	(int*)&g_sv_dm_dwDamageBlockTime	,	0, 600);	//sec
	CMD4(CCC_SV_Integer,	"sv_anomalies_enabled"		,	(int*)&g_sv_dm_bAnomaliesEnabled	,	0, 1);
	CMD4(CCC_SV_Integer,	"sv_anomalies_length"		,	(int*)&g_sv_dm_dwAnomalySetLengthTime,	0, 180); //min
	CMD4(CCC_SV_Integer,	"sv_pda_hunt"				,	(int*)&g_sv_dm_bPDAHunt				,	0, 1);
	CMD4(CCC_SV_Integer,	"sv_warm_up"				,	(int*)&g_sv_dm_dwWarmUp_MaxTime		,	0, 3600); //sec

	CMD4(CCC_Integer,		"sv_max_ping_limit"			,	(int*)&g_sv_dwMaxClientPing		,	1, 2000);

	CMD4(CCC_SV_Integer,	"sv_auto_team_balance"		,	(int*)&g_sv_tdm_bAutoTeamBalance	,	0,1);
	CMD4(CCC_SV_Integer,	"sv_auto_team_swap"			,	(int*)&g_sv_tdm_bAutoTeamSwap		,	0,1);
	CMD4(CCC_SV_Integer,	"sv_friendly_indicators"	,	(int*)&g_sv_tdm_bFriendlyIndicators	,	0,1);
	CMD4(CCC_SV_Integer,	"sv_friendly_names"			,	(int*)&g_sv_tdm_bFriendlyNames		,	0,1);
	CMD4(CCC_SV_Float,		"sv_friendlyfire"			,	&g_sv_tdm_fFriendlyFireModifier		,	0.0f,2.0f);
	CMD4(CCC_SV_Integer,	"sv_teamkill_limit"			,	&g_sv_tdm_iTeamKillLimit			,	0,100);
	CMD4(CCC_SV_Integer,	"sv_teamkill_punish"		,	(int*)&g_sv_tdm_bTeamKillPunishment	,	0,1);

	CMD4(CCC_SV_Integer,	"sv_artefact_respawn_delta"	,	(int*)&g_sv_ah_dwArtefactRespawnDelta	,0,600);	//sec
	CMD4(CCC_SV_Integer,	"sv_artefacts_count"		,	(int*)&g_sv_ah_dwArtefactsNum			, 1,100);
	CMD4(CCC_SV_Integer,	"sv_artefact_stay_time"		,	(int*)&g_sv_ah_dwArtefactStayTime		, 0,180);	//min
	CMD4(CCC_SV_Integer,	"sv_reinforcement_time"		,	(int*)&g_sv_ah_iReinforcementTime		, -1,3600); //sec
	CMD4(CCC_SV_Integer,	"sv_bearercantsprint"		,	(int*)&g_sv_ah_bBearerCantSprint				, 0, 1)	;
	CMD4(CCC_SV_Integer,	"sv_shieldedbases"			,	(int*)&g_sv_ah_bShildedBases					, 0, 1)	;
	CMD4(CCC_SV_Integer,	"sv_returnplayers"			,	(int*)&g_sv_ah_bAfReturnPlayersToBases		, 0, 1)	;
	CMD1(CCC_SwapTeams,		"g_swapteams"				);
#ifdef DEBUG
	CMD4(CCC_SV_Integer,	"sv_demo_delta_frame"	,	(int*)&g_dwDemoDeltaFrame	,	0,100);
	CMD4(CCC_SV_Integer,	"sv_ignore_money_on_buy"	,	(int*)&g_sv_dm_bDMIgnore_Money_OnBuy,	0, 1);
#endif

	CMD1(CCC_RadminCmd,		"ra");
	CMD1(CCC_Name,			"name");
	CMD1(CCC_SvStatus,		"sv_status");
	CMD1(CCC_SvChat,		"chat");
#ifdef BATTLEYE
	CMD1(CCC_BattlEyeSrv,	"beserver" );
	CMD1(CCC_BattlEyeCl,	"beclient" );
	CMD4(CCC_SV_Integer,	"bemsg"    , (int*)&g_be_message_out, 0, 1 );
#endif // BATTLEYE
	
	CMD1(CCC_CompressorStatus,"net_compressor_status");
	CMD4(CCC_SV_Integer,	"net_compressor_enabled"		,	(int*)&g_net_compressor_enabled	,	0,1);
	CMD4(CCC_SV_Integer,	"net_compressor_gather_stats"	,	(int*)&g_net_compressor_gather_stats,0,1);
	CMD1(CCC_MpStatistics,	"sv_dump_online_statistics");
	CMD4(CCC_SV_Integer,	"sv_dump_online_statistics_period"	,	(int*)&g_sv_mp_iDumpStatsPeriod	,	0,60); //min
}
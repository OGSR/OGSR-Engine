#pragma once
#include "game_sv_base.h"
#include "game_sv_mp_team.h"
#include "game_base_kill_type.h"
#include "game_base_menu_events.h"

class		CItemMgr;
class		xrClientData;
#define		VOTE_LENGTH_TIME		1
#define		VOTE_QUOTA				0.51f

#define MAX_TERMS  2
struct Rank_Struct
{
	shared_str			m_sTitle;
	int					m_iTerms[MAX_TERMS];
	int					m_iBonusMoney;
	xr_vector<float>	m_aRankDiff_ExpBonus;

	Rank_Struct () 
	{
		m_sTitle = NULL;
		ZeroMemory(m_iTerms, sizeof(m_iTerms));
		m_iBonusMoney = 0;
		m_aRankDiff_ExpBonus.clear();
	};
};

class game_sv_mp :public game_sv_GameState
{
	typedef game_sv_GameState inherited;

protected:
	//список трупов для удаления
	DEF_DEQUE(CORPSE_LIST, u16);

	CORPSE_LIST						m_CorpseList;

	DEF_VECTOR(RANKS_LIST, Rank_Struct);

	RANKS_LIST						m_aRanks;
	bool							m_bRankUp_Allowed;

	TEAM_DATA_LIST					TeamList;
	CItemMgr*						m_strWeaponsData;

	//-------------------------------------------------------
	bool			m_bVotingActive;
	bool			m_bVotingReal;
	u32				m_uVoteStartTime;	
	shared_str		m_pVoteCommand;
	
	virtual		void				LoadRanks				();
	virtual		void				Player_AddExperience	(game_PlayerState* ps, float Exp);
	virtual		bool				Player_Check_Rank		(game_PlayerState* ps);
	virtual		void				Player_Rank_Up			(game_PlayerState* ps);
	virtual		bool				Player_RankUp_Allowed	() {return m_bRankUp_Allowed;};
	virtual		void				Player_ExperienceFin	(game_PlayerState* ps);
	virtual		void				Set_RankUp_Allowed		(bool RUA) {m_bRankUp_Allowed = RUA;};

	virtual		void				UpdatePlayersMoney		();

	u8			m_u8SpectatorModes		;
	
protected:

	virtual		void				SendPlayerKilledMessage	(u16 KilledID, KILL_TYPE KillType, u16 KillerID, u16 WeaponID, SPECIAL_KILL_TYPE SpecialKill);
	virtual		void				RespawnPlayer			(ClientID id_who, bool NoSpectator);
				void				SpawnPlayer				(ClientID id, LPCSTR N);
	virtual		void				SetSkin					(CSE_Abstract* E, u16 Team, u16 ID);
				bool				GetPosAngleFromActor	(ClientID id, Fvector& Pos, Fvector &Angle);				
				void				AllowDeadBodyRemove		(ClientID id, u16 GameID);
				void				SpawnWeapon4Actor		(u16 actorId,  LPCSTR N, u8 Addons );
				void				SpawnWeaponForActor		(u16 actorId,  LPCSTR N, bool isScope, bool isGrenadeLauncher, bool isSilencer);

	virtual		void				Player_AddMoney			(game_PlayerState* ps, s32 MoneyAmount);
	virtual		void				Player_AddBonusMoney	(game_PlayerState* ps, s32 MoneyAmount, SPECIAL_KILL_TYPE Reason, u8 Kill = 0);

				u8					SpectatorModes_Pack		();
				void				SpectatorModes_UnPack	(u8 SpectrModesPacked);


public:
									game_sv_mp				();
				virtual				~game_sv_mp				();
	virtual		void				Create					(shared_str &options);
	virtual		void				OnPlayerConnect			(ClientID id_who);
	virtual		void				OnPlayerDisconnect		(ClientID id_who, LPSTR Name, u16 GameID);
	virtual		BOOL				OnTouch					(u16 eid_who, u16 eid_target, BOOL bForced = FALSE){return true;};			// TRUE=allow ownership, FALSE=denied
	virtual		void				OnDetach				(u16 eid_who, u16 eid_target){};
	virtual		void				OnPlayerKillPlayer		(game_PlayerState* ps_killer, game_PlayerState* ps_killed, KILL_TYPE KillType, SPECIAL_KILL_TYPE SpecialKillType, CSE_Abstract* pWeaponA){};
	virtual		void				OnPlayerKilled			(NET_Packet P);
	virtual		bool				CheckTeams				() { return false; };
	virtual		void				OnPlayerHitted			(NET_Packet P);
	virtual		void				OnPlayerEnteredGame		(ClientID id_who);

	virtual		void				OnDestroyObject			(u16 eid_who);			

	virtual		void				net_Export_State		(NET_Packet& P, ClientID id_to);

	virtual		void				OnRoundStart			();												// старт раунда
	virtual		void				OnRoundEnd				();	//round_end_reason							// конец раунда
	virtual		bool				OnNextMap				();
	virtual		void				OnPrevMap				();
	
	virtual		void				OnVoteStart				(LPCSTR VoteCommand, ClientID sender);
	virtual		bool				IsVotingActive			()	{ return m_bVotingActive; };
	virtual		void				SetVotingActive			( bool Active )	{ m_bVotingActive = Active; };
	virtual		void				UpdateVote				();
	virtual		void				OnVoteYes				(ClientID sender);
	virtual		void				OnVoteNo				(ClientID sender);
	virtual		void				OnVoteStop				();
	virtual		void				OnPlayerChangeName		(NET_Packet& P, ClientID sender);
	virtual		void				OnPlayerSpeechMessage	(NET_Packet& P, ClientID sender);
	virtual		void				OnPlayerGameMenu		(NET_Packet& P, ClientID sender);
	
	virtual		void				OnPlayerSelectSpectator	(NET_Packet& P, ClientID sender);
	virtual		void				OnPlayerSelectTeam		(NET_Packet& P, ClientID sender) {};
	virtual		void				OnPlayerSelectSkin		(NET_Packet& P, ClientID sender) {};
	virtual		void				OnPlayerBuySpawn		(ClientID sender)					{};
	
	virtual		void				OnEvent					(NET_Packet &tNetPacket, u16 type, u32 time, ClientID sender );
	virtual		void				Update					();
				void				KillPlayer				(ClientID id_who, u16 GameID);
	virtual		BOOL				CanHaveFriendlyFire		()	{return TRUE;};
	
	virtual		void				ClearPlayerState		(game_PlayerState* ps);
	virtual		void				ClearPlayerItems		(game_PlayerState* ps);
	virtual		void				SetPlayersDefItems		(game_PlayerState* ps);

	virtual		void				ReadOptions				(shared_str &options);
	virtual		void				ConsoleCommands_Create	();
	virtual		void				ConsoleCommands_Clear	();

	virtual		u32					GetTeamCount			()	{return TeamList.size();};	
				TeamStruct*			GetTeamData				(u32 Team);

	virtual		u8					GetSpectatorModes		() {return m_u8SpectatorModes;};
	virtual		u32					GetNumTeams				() {return 0;};

	virtual		void				DumpOnlineStatistic		();
	void							SvSendChatMessage		(LPCSTR str);
protected:
	virtual		void				WriteGameState			(CInifile& ini, LPCSTR sect, bool bRoundResult);
	virtual		void				WritePlayerStats		(CInifile& ini, LPCSTR sect, xrClientData* pCl);
				void				DumpRoundStatistics		();
public:
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(game_sv_mp)
#undef script_type_list
#define script_type_list save_type_list(game_sv_mp)


#include "stdafx.h"
#include "game_cl_deathmatch.h"
#include "xrMessages.h"
#include "hudmanager.h"
#include "UIGameDM.h"
#include "Spectator.h"
#include "level.h"
#include "xr_level_controller.h"
#include "clsid_game.h"
#include "actor.h"
#include "ui/UIMainIngameWnd.h"
#include "ui/UISkinSelector.h"
#include "ui/UIPdaWnd.h"
#include "ui/UIInventoryWnd.h"
#include "ui/UIMapDesc.h"
#include "ui/UIMessageBoxEx.h"
#include "ui/UIVote.h"
#include "dinput.h"
#include "gamepersistent.h"
#include "string_table.h"
#include "map_manager.h"
#include "map_location.h"

#include "game_cl_deathmatch_snd_messages.h"
#include "game_base_menu_events.h"

#include "ActorCondition.h"

#ifdef _new_buy_wnd
	#include	"ui\UIMpTradeWnd.h"
#else
	#include	"ui\UIBuyWnd.h"
#endif


#define	TEAM0_MENU		"deathmatch_team0"

game_cl_Deathmatch::game_cl_Deathmatch()
{
	pCurBuyMenu		= NULL;
	
	PresetItemsTeam0.clear();
	PlayerDefItems.clear();
	pCurPresetItems	= NULL;;

	pCurSkinMenu	= NULL;

	m_bBuyEnabled	= TRUE;

	m_bSkinSelected	= FALSE;	

	m_game_ui		= NULL;

	m_iCurrentPlayersMoney = 0;
	Actor_Spawn_Effect = "";

	LoadSndMessages();
	m_cl_dwWarmUp_Time = 0;
	m_bMenuCalledFromReady = FALSE;
	m_bFirstRun = TRUE;
}

void game_cl_Deathmatch::Init ()
{
	LoadTeamData(TEAM0_MENU);

	if (pSettings->line_exist("deathmatch_gamedata", "actor_spawn_effect"))
		Actor_Spawn_Effect = pSettings->r_string("deathmatch_gamedata", "actor_spawn_effect");
}

game_cl_Deathmatch::~game_cl_Deathmatch()
{
	PresetItemsTeam0.clear();
	PlayerDefItems.clear();

	xr_delete(pCurBuyMenu);
	xr_delete(pCurSkinMenu);
}


CUIGameCustom* game_cl_Deathmatch::createGameUI()
{
	game_cl_mp::createGameUI();
	CLASS_ID clsid			= CLSID_GAME_UI_DEATHMATCH;
	m_game_ui	= smart_cast<CUIGameDM*> ( NEW_INSTANCE ( clsid ) );
	R_ASSERT(m_game_ui);
	m_game_ui->SetClGame(this);
	m_game_ui->Init();
	return m_game_ui;
}

void game_cl_Deathmatch::SetCurrentSkinMenu		()	
{
	if (!pCurSkinMenu)
		pCurSkinMenu	= InitSkinMenu(0);	
}

void game_cl_Deathmatch::net_import_state	(NET_Packet& P)
{
	inherited::net_import_state	(P);

	m_s32FragLimit				= P.r_s32();
	m_s32TimeLimit				= P.r_s32() * 60000;
	m_u32ForceRespawn			= P.r_u32() * 1000;
	m_cl_dwWarmUp_Time			= P.r_u32();
	m_bDamageBlockIndicators	= !!P.r_u8();
	// Teams
	u16							t_count;
	P.r_u16						(t_count);
	teams.clear					();

	for (u16 t_it=0; t_it<t_count; ++t_it)
	{
		game_TeamState	ts;
		P.r				(&ts,sizeof(game_TeamState));
		teams.push_back	(ts);
	};

	switch (Phase())
	{
	case GAME_PHASE_PLAYER_SCORES:
		{
			P.r_stringZ(WinnerName);
			bool NeedSndMessage = (xr_strlen(WinnerName) != 0);
			if (NeedSndMessage && local_player && !xr_strcmp(WinnerName, local_player->getName()))
			{
				PlaySndMessage(ID_YOU_WON);
			}
		}break;
	}
}

void	game_cl_Deathmatch::net_import_update		(NET_Packet& P)
{
	inherited::net_import_update(P);
	//-----------------------------------
	if (pCurBuyMenu && local_player)
	{
		if (local_player->rank != pCurBuyMenu->GetRank() && !pCurBuyMenu->IsIgnoreMoneyAndRank())
		{
			pCurBuyMenu->SetRank(local_player->rank);
			LoadDefItemsForRank(pCurBuyMenu);
			ChangeItemsCosts(pCurBuyMenu);
		}
	}
}

IBuyWnd* game_cl_Deathmatch::InitBuyMenu			(const shared_str& BasePriceSection, s16 Team)
{
	if (Team == -1)
	{
		Team						= local_player->team;
	};

	cl_TeamStruct *pTeamSect		= &TeamList[ModifyTeam(Team)];
	
	IBuyWnd* pMenu					= xr_new<BUY_WND_TYPE>();
	pMenu->Init						(pTeamSect->caSection, BasePriceSection);
	pMenu->SetSkin					(0);
	return							pMenu;
};

CUISkinSelectorWnd* game_cl_Deathmatch::InitSkinMenu			(s16 Team)
{
	if (Team == -1)
	{
		Team						= local_player->team;
	};

	cl_TeamStruct *pTeamSect		= &TeamList[ModifyTeam(Team)];	

	CUISkinSelectorWnd* pMenu		= xr_new<CUISkinSelectorWnd>	((char*)pTeamSect->caSection.c_str(), Team);
	return							pMenu;
};

void game_cl_Deathmatch::OnMapInfoAccept			()
{
	if (CanCallSkinMenu())
		StartStopMenu(pCurSkinMenu, true);
};

void game_cl_Deathmatch::OnSkinMenuBack			()
{
	StartStopMenu(m_game_ui->m_pMapDesc, true);
};

void game_cl_Deathmatch::OnSkinMenu_Ok			()
{
	CObject *l_pObj = Level().CurrentEntity();

	CGameObject *l_pPlayer = smart_cast<CGameObject*>(l_pObj);
	if(!l_pPlayer) return;

	NET_Packet		P;
	l_pPlayer->u_EventGen		(P, GE_GAME_EVENT, l_pPlayer->ID()	);
	P.w_u16(GAME_EVENT_PLAYER_GAME_MENU);
	P.w_u8(PLAYER_CHANGE_SKIN);



    P.w_s8	((u8)pCurSkinMenu->GetActiveIndex());
	l_pPlayer->u_EventSend		(P);
	//-----------------------------------------------------------------
	m_bSkinSelected = TRUE;

	// second stub here
	if (pCurBuyMenu) 
	{
		if (-1 == pCurSkinMenu->GetActiveIndex())
			pCurBuyMenu->SetSkin(0);
		else
            pCurBuyMenu->SetSkin((u8)pCurSkinMenu->GetActiveIndex());
	}
};

void game_cl_Deathmatch::OnSkinMenu_Cancel		()
{
	if (!m_bSkinSelected && !m_bSpectatorSelected)
	{
		if (CanCallSkinMenu() && !pCurSkinMenu->IsShown())
		{
			StartStopMenu(pCurSkinMenu, true);
			return;
		}
	}
	m_bMenuCalledFromReady = FALSE;
};

BOOL game_cl_Deathmatch::CanCallBuyMenu			()
{
	if (Phase()!=GAME_PHASE_INPROGRESS) return false;	
	if (Level().CurrentEntity() && Level().CurrentEntity()->CLS_ID != CLSID_SPECTATOR)
	{
		return FALSE;
	};
	if (!m_bSkinSelected || m_bSpectatorSelected) return FALSE;
	if (pCurSkinMenu && pCurSkinMenu->IsShown())
	{
		return FALSE;
	};
	if (m_game_ui->m_pInventoryMenu && m_game_ui->m_pInventoryMenu->IsShown())
	{
		return FALSE;
	};
	return m_bBuyEnabled;
};

BOOL game_cl_Deathmatch::CanCallSkinMenu			()
{
	if (Phase()!=GAME_PHASE_INPROGRESS) return false;
	if (m_game_ui->m_pInventoryMenu && m_game_ui->m_pInventoryMenu->IsShown())
	{
		return FALSE;
	};
	if (pCurBuyMenu && pCurBuyMenu->IsShown())
	{
		return FALSE;
	};
	SetCurrentSkinMenu();
	if(!pCurSkinMenu)	return FALSE;
	if (!pCurSkinMenu->IsShown())
		pCurSkinMenu->SetCurSkin(local_player->skin);
	return TRUE;
};

BOOL game_cl_Deathmatch::CanCallInventoryMenu			()
{
	if (Phase()!=GAME_PHASE_INPROGRESS) return false;
	if (Level().CurrentEntity() && Level().CurrentEntity()->CLS_ID != CLSID_OBJECT_ACTOR)
	{
		return FALSE;
	}
	if (pCurSkinMenu && pCurSkinMenu->IsShown())
	{
		return FALSE;
	};
	return TRUE;
};


void game_cl_Deathmatch::SetCurrentBuyMenu	()	
{
	if (!pCurBuyMenu)
	{
		pCurBuyMenu	= InitBuyMenu(GetBaseCostSect(), 0);
		LoadTeamDefaultPresetItems(GetTeamMenu(0), pCurBuyMenu, &PresetItemsTeam0);
		pCurPresetItems	= &PresetItemsTeam0;
		LoadDefItemsForRank(pCurBuyMenu);
	}	
	//-----------------------------------
	if (m_cl_dwWarmUp_Time != 0) pCurBuyMenu->IgnoreMoneyAndRank(true);
	else pCurBuyMenu->IgnoreMoneyAndRank(false);
	//-----------------------------------
	if (!local_player) return;
};

void game_cl_Deathmatch::ClearBuyMenu			()
{
	if (!pCurBuyMenu) return;
	pCurBuyMenu->ClearSlots();
	pCurBuyMenu->ClearRealRepresentationFlags();
};

bool game_cl_Deathmatch::CanBeReady				()
{
	if (!local_player) return false;

	m_bMenuCalledFromReady = TRUE;

	SetCurrentSkinMenu();

	SetCurrentBuyMenu();
	
	if (pCurBuyMenu && !pCurBuyMenu->IsShown())
	{
		pCurBuyMenu->ResetItems();
		SetBuyMenuItems		(&PlayerDefItems);
	}
	
	if (!m_bSkinSelected)
	{
		m_bMenuCalledFromReady = FALSE;
		if (CanCallSkinMenu())
			StartStopMenu(pCurSkinMenu,true);
		return false;
	};

	if (pCurBuyMenu)
	{		
		const preset_items& _p	= pCurBuyMenu->GetPreset(_preset_idx_last);
		bool Passed = false;
		Passed = (_p.size()==0) ? 1 : (s32(pCurBuyMenu->GetPresetCost(_preset_idx_last)) <= local_player->money_for_round);
		Passed |= pCurBuyMenu->IsIgnoreMoneyAndRank();
		if (!Passed)
		{
			if (CanCallBuyMenu())
			{
				ShowBuyMenu();
			}
			return false;
		}
	};
	
	m_bMenuCalledFromReady = FALSE;

	OnBuyMenu_Ok();
	return true;
};

void	game_cl_Deathmatch::OnSpectatorSelect		()
{
	m_bMenuCalledFromReady = FALSE;
	m_bSkinSelected = FALSE;
	inherited::OnSpectatorSelect();
};


char*	game_cl_Deathmatch::getTeamSection(int Team)
{
	return "deathmatch_team0";
};

void game_cl_Deathmatch::Check_Invincible_Players()
{
};

void game_cl_Deathmatch::ConvertTime2String		(string64* str, u32 Time)
{
	if (!str) return;
	
	u32 RHour = Time / 3600000;
	Time %= 3600000;
	u32 RMinutes = Time / 60000;
	Time %= 60000;
	u32 RSecs = Time / 1000;

	sprintf_s(*str,"%02d:%02d:%02d", RHour, RMinutes, RSecs);
};

int game_cl_Deathmatch::GetPlayersPlace			(game_PlayerState* ps)
{
	if (!ps) return -1;
	game_cl_GameState::PLAYERS_MAP_IT I=Game().players.begin();
	game_cl_GameState::PLAYERS_MAP_IT E=Game().players.end();

	// create temporary map (sort by kills)
	xr_vector<game_PlayerState*>	Players;
	for (;I!=E;++I)		Players.push_back(I->second);
	std::sort			(Players.begin(),Players.end(),DM_Compare_Players);

	int Place = 1;
	for (u32 i=0; i<Players.size(); i++)
	{
		if (Players[i] == ps)
			return Place;
		Place++;
	};
	return -1;
}

string16 places[] = {
	"1st", "2nd", "3rd", "4th", "5th", "6th", "7th", "8th",
	"9th", "10th", "11th", "12th", "13th", "15th", "15th", "16th",
	"17th", "18th", "19th", "20th", "21th", "22th", "23th", "24th",
	"25th", "26th", "27th", "28th", "29th", "30th", "31th", "32th"
};

void game_cl_Deathmatch::shedule_Update			(u32 dt)
{
	CStringTable st;

	inherited::shedule_Update(dt);

	if(g_dedicated_server)	return;

	//fake	
	if(!m_game_ui && HUD().GetUI() ) m_game_ui = smart_cast<CUIGameDM*>( HUD().GetUI()->UIGame() );
	if(m_game_ui)
	{
		m_game_ui->SetTimeMsgCaption("");
		m_game_ui->SetRoundResultCaption("");
		m_game_ui->SetSpectatorMsgCaption("");
		m_game_ui->SetPressJumpMsgCaption("");
		m_game_ui->SetPressBuyMsgCaption("");
		m_game_ui->SetForceRespawnTimeCaption("");
		m_game_ui->SetWarmUpCaption("");
	};

	if (HUD().GetUI() && HUD().GetUI()->UIMainIngameWnd)
		HUD().GetUI()->UIMainIngameWnd->GetPDAOnline()->SetText("");

	switch (Phase())
	{
	case GAME_PHASE_INPROGRESS:
		{
			m_game_ui->ShowPlayersList(false);

			Check_Invincible_Players();

			if (m_s32TimeLimit && m_cl_dwWarmUp_Time == 0)
			{
				if (Level().timeServer()<(StartTime() + m_s32TimeLimit))
				{
					u32 lts = Level().timeServer();
					u32 Rest = (StartTime() + m_s32TimeLimit) - lts;
					string64 S;
					ConvertTime2String(&S, Rest);
					if(m_game_ui)
						m_game_ui->SetTimeMsgCaption(S);
				}
				else
				{
					if(m_game_ui)
						m_game_ui->SetTimeMsgCaption("00:00:00");
				}
			};
			
			if(local_player && !local_player->IsSkip())
			{
				if (m_bFirstRun)
				{
					m_bFirstRun = FALSE;
					if (m_game_ui->m_pMapDesc) StartStopMenu(m_game_ui->m_pMapDesc, TRUE);
				};

				if (m_game_ui)
				{
					string256 MoneyStr;
					sprintf_s(MoneyStr, "%d", local_player->money_for_round);
					m_game_ui->ChangeTotalMoneyIndicator(MoneyStr);
				}				

				m_game_ui->SetPressJumpMsgCaption("");
				m_game_ui->SetPressBuyMsgCaption("");				

				if (m_cl_dwWarmUp_Time > Level().timeServer())
				{
					u32 TimeRemains = m_cl_dwWarmUp_Time - Level().timeServer();
					string64 S;
					ConvertTime2String(&S, TimeRemains);
					string1024 tmpStr = "";
					if (TimeRemains > 10000)
						strconcat(sizeof(tmpStr),tmpStr, *st.translate("mp_time2start"), " ", S);
					else
					{
						if (TimeRemains < 1000)
							strconcat(sizeof(tmpStr),tmpStr, *st.translate("mp_go"), "");
						else
						{
							static u32 dwLastTimeRemains = 10;
							u32 dwCurTimeRemains = TimeRemains/1000;
							if (dwLastTimeRemains != dwCurTimeRemains)
							{
								if (dwCurTimeRemains > 0 && dwCurTimeRemains <= 5)
									PlaySndMessage(ID_COUNTDOWN_1 + dwCurTimeRemains - 1);
							}
							dwLastTimeRemains = dwCurTimeRemains;
							_itoa(dwCurTimeRemains, S, 10);								
							strconcat(sizeof(tmpStr),tmpStr, *st.translate("mp_ready"), "...", S);
						}
					};
					
					m_game_ui->SetWarmUpCaption(tmpStr);
				}

				if (Level().CurrentEntity() && Level().CurrentEntity()->CLS_ID == CLSID_SPECTATOR)
				{
					if (!(pCurBuyMenu && pCurBuyMenu->IsShown()) && 
						!(pCurSkinMenu && pCurSkinMenu->IsShown()) &&
						!(m_game_ui->m_pMapDesc && m_game_ui->m_pMapDesc->IsShown()) &&
						(HUD().GetUI() && HUD().GetUI()->GameIndicatorsShown())
						)
					{
						if (!m_bSkinSelected)
							m_game_ui->SetPressJumpMsgCaption("mp_press_jump2select_skin");
						else
							m_game_ui->SetPressJumpMsgCaption("mp_press_jump2start");

						if (CanCallBuyMenu())
							m_game_ui->SetPressBuyMsgCaption("mp_press_to_buy");						
					};
				};

				if (Level().CurrentControlEntity() && 
					Level().CurrentControlEntity()->CLS_ID == CLSID_SPECTATOR &&
					(HUD().GetUI() && HUD().GetUI()->GameIndicatorsShown())
					)
				{
					
					CSpectator* pSpectator = smart_cast<CSpectator*>(Level().CurrentControlEntity());
					if (pSpectator)
					{
						string1024 SpectatorStr = "";
						pSpectator->GetSpectatorString(SpectatorStr);
						m_game_ui->SetSpectatorMsgCaption(SpectatorStr);
					}
				}

				u32 CurTime = Level().timeServer();
				if (IsVotingEnabled() && IsVotingActive() && m_dwVoteEndTime>=CurTime)
				{
					u32 TimeLeft = m_dwVoteEndTime - Level().timeServer();
					string1024 VoteTimeResStr;
					u32 SecsLeft = (TimeLeft % 60000) / 1000;
					u32 MinitsLeft = (TimeLeft - SecsLeft) / 60000;

					u32 NumAgreed = 0;
					PLAYERS_MAP_IT I;
					I	= players.begin();
					for(;I!=players.end(); ++I)
					{
						game_PlayerState* ps = I->second;
						if (ps->m_bCurrentVoteAgreed == 1) NumAgreed++;
					}
					
					sprintf_s	(VoteTimeResStr, st.translate("mp_timeleft").c_str(), MinitsLeft, SecsLeft, float(NumAgreed)/players.size());
					if (m_game_ui)
						m_game_ui->SetVoteTimeResultMsg(VoteTimeResStr);
				};

				if (local_player->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD) && 
					m_u32ForceRespawn &&
					!local_player->testFlag(GAME_PLAYER_FLAG_SPECTATOR) )
				{
					if(m_game_ui)
					{
						u32 Rest			= m_u32ForceRespawn - local_player->DeathTime;
						string64			S;
						ConvertTime2String	(&S, Rest);
						string128			FullS;
						sprintf_s				(FullS, "%s : %s", *st.translate("mp_time2respawn"), S);

						m_game_ui->SetForceRespawnTimeCaption(FullS);
					};
				};


				if (Level().CurrentViewEntity() && m_game_ui)
				{
					game_PlayerState* ps = GetPlayerByGameID(Level().CurrentViewEntity()->ID());
					
					if (ps && m_game_ui) 
						m_game_ui->SetRank(ps->team, ps->rank);

					if (ps && m_game_ui) 
						m_game_ui->SetFraglimit(ps->frags(), m_s32FragLimit);
				}
			};
		}break;
	case GAME_PHASE_PENDING:
		{
			if (m_game_ui)
				m_game_ui->ShowPlayersList(true);
		}break;
	case GAME_PHASE_PLAYER_SCORES:
		{
			string128 resstring;
			sprintf_s(resstring, *st.translate("mp_player_wins"), WinnerName);
			m_game_ui->SetRoundResultCaption(resstring);

			SetScore();
			m_game_ui->ShowPlayersList(false);
		}break;
	};
	
	//-----------------------------------------
	if (!CanCallBuyMenu()) HideBuyMenu();
		
	if (pCurSkinMenu && pCurSkinMenu->IsShown() && !CanCallSkinMenu())
		StartStopMenu(pCurSkinMenu, true);
	//-----------------------------------------------
	
	//-----------------------------------------------
	if (m_game_ui->m_pInventoryMenu && m_game_ui->m_pInventoryMenu->IsShown() && !CanCallInventoryMenu())
		StartStopMenu(m_game_ui->m_pInventoryMenu,true);
	//-----------------------------------------

	u32 cur_game_state = Phase();
	if(m_game_ui->m_pMapDesc && m_game_ui->m_pMapDesc->IsShown() && cur_game_state!=GAME_PHASE_INPROGRESS)
	{
		m_game_ui->m_pMapDesc->GetHolder()->StartStopMenu(m_game_ui->m_pMapDesc, true);
	}

	if(pCurSkinMenu && pCurSkinMenu->IsShown() && cur_game_state!=GAME_PHASE_INPROGRESS)
	{
		pCurSkinMenu->GetHolder()->StartStopMenu(pCurSkinMenu, true);
	}
	
	if(cur_game_state!=GAME_PHASE_INPROGRESS) HideBuyMenu();	
}

void	game_cl_Deathmatch::SetScore				()
{
	if (Level().CurrentViewEntity() && m_game_ui)
	{
		game_PlayerState* ps = GetPlayerByGameID(Level().CurrentViewEntity()->ID());
		if (ps&&m_game_ui) 
			m_game_ui->SetRank(ps->team, ps->rank);

		if (ps&&m_game_ui) 
			m_game_ui->SetFraglimit(ps->frags(), m_s32FragLimit);
	}
};

bool	game_cl_Deathmatch::OnKeyboardPress			(int key)
{
	if (kSCORES == key && Phase() == GAME_PHASE_INPROGRESS)
	{
		if(m_game_ui)
#ifndef NDEBUG
			if (Level().IR_GetKeyState(DIK_LCONTROL))
				m_game_ui->ShowStatistic(true);
			else
#endif
				m_game_ui->ShowFragList(true);
		return true;
	};

	if (kINVENTORY == key )
	{
		if (Level().CurrentControlEntity() && Level().CurrentControlEntity()->CLS_ID == CLSID_OBJECT_ACTOR)
		{
			if (m_game_ui){
				if (m_game_ui->m_pInventoryMenu->IsShown())
					StartStopMenu(m_game_ui->m_pInventoryMenu,true);
				else
				{
					if (CanCallInventoryMenu())
					{
						StartStopMenu(m_game_ui->m_pInventoryMenu,true);
					};
				};
				return true;
			}			
		}		
	};

	if (kBUY == key )
	{
		if (pCurBuyMenu && pCurBuyMenu->IsShown())
			HideBuyMenu();
		else
		{		
			if(CanCallBuyMenu())
			{
				SetCurrentBuyMenu	();
				pCurBuyMenu->ResetItems();

				if (pCurBuyMenu && !pCurBuyMenu->IsShown())
					SetBuyMenuItems		(&PlayerDefItems);				

				ShowBuyMenu();
			}
		};

		return true;
	};
	
	if (kSKIN == key )
	{
		if (pCurSkinMenu && pCurSkinMenu->IsShown())
			StartStopMenu(pCurSkinMenu,true);
		else
		{
			if (CanCallSkinMenu())
			{
				SetCurrentSkinMenu();
				StartStopMenu(pCurSkinMenu,true);
			}
		}
		return true;
	};
	//---------------------------------------------
	if( kMAP == key)
	{
		if (m_game_ui)
		{
			if (m_game_ui->m_pPdaMenu && m_game_ui->m_pPdaMenu->IsShown())
				StartStopMenu(m_game_ui->m_pPdaMenu,true);
			else
			{
				m_game_ui->m_pPdaMenu->SetActiveSubdialog(eptMap);
				StartStopMenu(m_game_ui->m_pPdaMenu,true);
			};		
			return true;
		}
	};
	//---------------------------------------------

	return inherited::OnKeyboardPress(key);
}

bool	game_cl_Deathmatch::OnKeyboardRelease		(int key)
{
	if (kSCORES == key )
	{
		if (m_game_ui)
		{
			m_game_ui->ShowFragList(false);
			m_game_ui->ShowStatistic(false);
			
		};
		return true;
	};
	return inherited::OnKeyboardRelease(key);
}

#define MAX_VOTE_PARAMS		5
void game_cl_Deathmatch::OnVoteStart(NET_Packet& P)
{
	CStringTable st;
	inherited::OnVoteStart(P);

	string1024	Command = "";
	string64	Player = "";
	P.r_stringZ(Command);
	P.r_stringZ(Player);
	m_dwVoteEndTime = Level().timeServer() + P.r_u32();
	
	if(m_game_ui)
	{
		string4096 CmdName = "";
		string1024 NewCmd; strcpy(NewCmd, Command);
		string1024 CmdParams[MAX_VOTE_PARAMS] = {"", "", "", "", ""};
		sscanf	(Command,"%s %s %s %s %s %s", CmdName, CmdParams[0], CmdParams[1], CmdParams[2], CmdParams[3], CmdParams[4]);

		if (!xr_strcmp(CmdName, "restart"))
		{
			sprintf_s(NewCmd, "%s", 
				*st.translate("mp_restart")
				);
		}
		else if (!xr_strcmp(CmdName, "restart_fast"))
		{
			sprintf_s(NewCmd, "%s", 
				*st.translate("mp_restart_fast")
				);
		}
		else if (!xr_strcmp(CmdName, "kick"))
		{
			sprintf_s(NewCmd, "%s %s", 
				*st.translate("mp_kick"), 
				CmdParams[0]
				);
			for (int i=1; i<MAX_VOTE_PARAMS; i++)
			{
				if (xr_strlen(CmdParams[i]))
				{
					strcat(NewCmd, " ");
					strcat(NewCmd, CmdParams[i]);
				}
			}
		}
		else if (!xr_strcmp(CmdName, "ban"))
		{
			sprintf_s(NewCmd, "%s %s", 
				*st.translate("mp_ban"), 
				CmdParams[0]
				);
			for (int i=1; i<MAX_VOTE_PARAMS; i++)
			{
				if (xr_strlen(CmdParams[i]))
				{
					strcat(NewCmd, " ");
					strcat(NewCmd, CmdParams[i]);
				}
			}
		}
		else if (!xr_strcmp(CmdName, "changemap"))
		{
			sprintf_s(NewCmd, "%s %s", 
				*st.translate("mp_change_map"), 
				*st.translate(CmdParams[0])
				);
		}
		else if (!xr_strcmp(CmdName, "changeweather"))
		{
			sprintf_s(NewCmd, "%s %s", 
				*st.translate("mp_change_weather"), 
				*st.translate(CmdParams[0])
				);
		}

		
		string1024 VoteStr;
		sprintf_s(VoteStr, *st.translate("mp_voting_started"), NewCmd, Player);		
		


		m_game_ui->SetVoteMessage(VoteStr);
		m_game_ui->SetVoteTimeResultMsg("");
		if (!m_pVoteRespondWindow)
            m_pVoteRespondWindow = xr_new<CUIVote>();
		m_pVoteRespondWindow->SetVoting(VoteStr);
	};
};

void game_cl_Deathmatch::OnVoteStop				(NET_Packet& P)	
{
	inherited::OnVoteStop(P);
	if(m_game_ui)
	{
		m_game_ui->SetVoteMessage(NULL);
		m_game_ui->SetVoteTimeResultMsg(NULL);
	}
};

void game_cl_Deathmatch::OnVoteEnd				(NET_Packet& P)
{
	inherited::OnVoteEnd(P);
	if(m_game_ui)
	{
		m_game_ui->SetVoteMessage(NULL);
		m_game_ui->SetVoteTimeResultMsg(NULL);
	}
};

void game_cl_Deathmatch::GetMapEntities(xr_vector<SZoneMapEntityData>& dst)
{
	/*
	SZoneMapEntityData D;
	u32 color_self_team		=		0xff00ff00;
	D.color					=		color_self_team;

	PLAYERS_MAP_IT it = players.begin();
	for(;it!=players.end();++it)
	{
		game_PlayerState* ps = it->second;
		u16 id = ps->GameID;
		if (ps->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD)) continue;
		CObject* pObject = Level().Objects.net_Find(id);
		if (!pObject) continue;
		if (!pObject || pObject->CLS_ID != CLSID_OBJECT_ACTOR) continue;

		VERIFY(pObject);
		D.pos = pObject->Position();
		dst.push_back(D);
	}
	*/
}

bool		game_cl_Deathmatch::IsEnemy					(game_PlayerState* ps)
{
	return true;
}

bool		game_cl_Deathmatch::IsEnemy					(CEntityAlive* ea1, CEntityAlive* ea2)
{
	return true;
};

void		game_cl_Deathmatch::OnRender				()
{
	if (m_bDamageBlockIndicators && local_player)
	{
		PLAYERS_MAP_IT it = players.begin();
		for(;it!=players.end();++it)
		{
			game_PlayerState* ps = it->second;
			u16 id = ps->GameID;
			if (ps->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD)) continue;
			if (!ps->testFlag(GAME_PLAYER_FLAG_INVINCIBLE)) continue;
			CObject* pObject = Level().Objects.net_Find(id);
			if (!pObject) continue;
			if (!pObject || pObject->CLS_ID != CLSID_OBJECT_ACTOR) continue;
			if (ps == local_player) continue;
			if (!IsEnemy(ps)) continue;
			cl_TeamStruct *pTS = &TeamList[ModifyTeam(ps->team)]; 

			VERIFY(pObject);
			CActor* pActor = smart_cast<CActor*>(pObject);
			VERIFY(pActor);
			pActor->RenderIndicator(pTS->IndicatorPos, pTS->Indicator_r1, pTS->Indicator_r2, pTS->InvincibleShader);
		}
	};
}

IC bool	DM_Compare_Players(game_PlayerState* p1, game_PlayerState* p2)
{
	if (p1->testFlag(GAME_PLAYER_FLAG_SPECTATOR) && !p2->testFlag(GAME_PLAYER_FLAG_SPECTATOR)) return false;
	if (!p1->testFlag(GAME_PLAYER_FLAG_SPECTATOR) && p2->testFlag(GAME_PLAYER_FLAG_SPECTATOR)) return true;	
	if (p1->frags()==p2->frags())
	{
		return p1->m_iDeaths < p2->m_iDeaths;
	}
	return p1->frags() > p2->frags();
};

void game_cl_Deathmatch::PlayParticleEffect(LPCSTR EffName, Fvector& pos)
{
	if (!EffName) return;
	// вычислить позицию и направленность партикла
	Fmatrix M; 
	M.translate(pos);

//	CParticlesPlayer::MakeXFORM(pObj,0,Fvector().set(0.f,1.f,0.f),Fvector().set(0.f,0.f,0.f),pos);

	// установить particles
	CParticlesObject* ps = NULL;

	ps = CParticlesObject::Create(EffName,TRUE);

	ps->UpdateParent(M,Fvector().set(0.f,0.f,0.f));
	GamePersistent().ps_needtoplay.push_back(ps);
}

void game_cl_Deathmatch::OnSpawn(CObject* pObj)
{
	inherited::OnSpawn(pObj);
	if (!pObj) return;
	if (pObj->CLS_ID == CLSID_OBJECT_ACTOR)
	{
		if (xr_strlen(Actor_Spawn_Effect))
			PlayParticleEffect(Actor_Spawn_Effect.c_str(), pObj->Position());
	};
}

void game_cl_Deathmatch::LoadSndMessages()
{
	LoadSndMessage("dm_snd_messages", "you_won", ID_YOU_WON);

	LoadSndMessage("dm_snd_messages", "dm_rank1", ID_RANK_1);
	LoadSndMessage("dm_snd_messages", "dm_rank2", ID_RANK_2);
	LoadSndMessage("dm_snd_messages", "dm_rank3", ID_RANK_3);
	LoadSndMessage("dm_snd_messages", "dm_rank4", ID_RANK_4);

	LoadSndMessage("dm_snd_messages", "countdown_5", ID_COUNTDOWN_5);
	LoadSndMessage("dm_snd_messages", "countdown_4", ID_COUNTDOWN_4);
	LoadSndMessage("dm_snd_messages", "countdown_3", ID_COUNTDOWN_3);
	LoadSndMessage("dm_snd_messages", "countdown_2", ID_COUNTDOWN_2);
	LoadSndMessage("dm_snd_messages", "countdown_1", ID_COUNTDOWN_1);
};

void				game_cl_Deathmatch::OnSwitchPhase_InProgress()
{
	HideBuyMenu();
	LoadTeamDefaultPresetItems(GetTeamMenu(0), pCurBuyMenu, &PresetItemsTeam0);
};

void				game_cl_Deathmatch::OnSwitchPhase			(u32 old_phase, u32 new_phase)
{
	inherited::OnSwitchPhase(old_phase, new_phase);
	switch (new_phase)
	{
	case GAME_PHASE_INPROGRESS:
		{
			WinnerName[0] = 0;
		}break;
	case GAME_PHASE_PLAYER_SCORES:
		{
			if (local_player)
			{
				if (!xr_strcmp(WinnerName, local_player->getName()))
				{
					PlaySndMessage(ID_YOU_WON);
				}
			}			
		}break;
	default:
		{			
		}break;
	};
}

void				game_cl_Deathmatch::OnGameRoundStarted				()
{
	inherited::OnGameRoundStarted();
	if (pCurBuyMenu && pCurBuyMenu->IsShown())
		StartStopMenu(pCurBuyMenu, true);
	if (local_player)
	{
		if (pCurBuyMenu) 
		{
			pCurBuyMenu->IgnoreMoneyAndRank(false);
			pCurBuyMenu->SetRank(local_player->rank);
		}
		ClearBuyMenu();
		LoadDefItemsForRank(pCurBuyMenu);
		ChangeItemsCosts(pCurBuyMenu);
		if (pCurBuyMenu && pCurPresetItems)
		{
			LoadTeamDefaultPresetItems(GetTeamMenu(local_player->team), pCurBuyMenu, pCurPresetItems);
		}
	}
	if (pCurBuyMenu) pCurBuyMenu->ClearPreset(_preset_idx_last);
	//-----------------------------------------------------------------
	if (m_game_ui && m_game_ui->m_pInventoryMenu && m_game_ui->m_pInventoryMenu->IsShown())
	{
		StartStopMenu(m_game_ui->m_pInventoryMenu,true);
	}
}

void				game_cl_Deathmatch::OnRankChanged			(u8 OldRank)
{
	inherited::OnRankChanged(OldRank);
	if (pCurBuyMenu) pCurBuyMenu->SetRank(local_player->rank);	
	LoadDefItemsForRank(pCurBuyMenu);
	ChangeItemsCosts(pCurBuyMenu);
	//---------------------------------------------
	PlayRankChangesSndMessage();
};

void				game_cl_Deathmatch::PlayRankChangesSndMessage()
{
	if (local_player)
	{
		switch (local_player->rank)
		{
		case 0:
			break;		
		default:
			PlaySndMessage(ID_RANK_0+local_player->rank);
			break;
		}
	}
}

void game_cl_Deathmatch::OnTeamChanged()
{
	if (!pCurBuyMenu)				return;
	if (pCurBuyMenu)				pCurBuyMenu->SetRank(local_player->rank);
	LoadDefItemsForRank				(pCurBuyMenu);
	ChangeItemsCosts				(pCurBuyMenu);
};

void game_cl_Deathmatch::LoadPlayerDefItems(char* TeamName, IBuyWnd* pBuyMenu)
{
	if (!local_player)				return;
	LoadTeamDefaultPresetItems		(TeamName, pBuyMenu, &PlayerDefItems);
};

void game_cl_Deathmatch::OnGameMenuRespond_ChangeSkin(NET_Packet& P)
{
	s8 NewSkin						= P.r_s8();
	local_player->skin				= NewSkin;
	
	if (pCurSkinMenu && pCurSkinMenu->IsShown())
		StartStopMenu				(pCurSkinMenu, true);

	if (m_game_ui->m_pMapDesc && m_game_ui->m_pMapDesc->IsShown())
		StartStopMenu				(m_game_ui->m_pMapDesc, TRUE);

	SetCurrentSkinMenu				();
	if (pCurSkinMenu)				pCurSkinMenu->SetCurSkin(local_player->skin);
	SetCurrentBuyMenu				();
	if (pCurBuyMenu)				pCurBuyMenu->SetSkin(local_player->skin);	
	m_bSpectatorSelected			= FALSE;
	
	if (m_bMenuCalledFromReady)
	{
		OnKeyboardPress				(kJUMP);
	}
};

void game_cl_Deathmatch::OnPlayerFlagsChanged(game_PlayerState* ps)
{
	inherited::OnPlayerFlagsChanged	(ps);
	if (!ps)						return;

	if (ps->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD)) return;

	CObject* pObject				= Level().Objects.net_Find(ps->GameID);
	if (!pObject)					return;

	if (pObject->CLS_ID != CLSID_OBJECT_ACTOR) return;

	CActor* pActor					= smart_cast<CActor*>(pObject);
	if (!pActor)					return;

	pActor->conditions().SetCanBeHarmedState(!ps->testFlag(GAME_PLAYER_FLAG_INVINCIBLE));
};

void game_cl_Deathmatch::SendPickUpEvent(u16 ID_who, u16 ID_what)
{
	NET_Packet						P;
	u_EventGen						(P,GE_OWNERSHIP_TAKE_MP_FORCED, ID_who);
	P.w_u16							(ID_what);
	u_EventSend						(P);
};

const shared_str game_cl_Deathmatch::GetTeamMenu(s16 team)
{
	return TEAM0_MENU;
}

#define SELF_LOCATION	"mp_self_location"
void game_cl_Deathmatch::UpdateMapLocations		()
{
	inherited::UpdateMapLocations();
	if (local_player)
	{
		if (!Level().MapManager().HasMapLocation(SELF_LOCATION, local_player->GameID))
		{
			(Level().MapManager().AddMapLocation(SELF_LOCATION, local_player->GameID))->EnablePointer();
		}
	}
}

void		game_cl_Deathmatch::ShowBuyMenu				()
{
	if (!local_player) return;
	if (!pCurBuyMenu || pCurBuyMenu->IsShown()) return;
	StartStopMenu(pCurBuyMenu, true);
	if (local_player->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD))
	{
		const preset_items& _p	= pCurBuyMenu->GetPreset(_preset_idx_last);	
		if (_p.size() != 0) pCurBuyMenu->TryUsePreset(_preset_idx_last);
	}
};

void		game_cl_Deathmatch::HideBuyMenu				()
{
	if (!pCurBuyMenu || !pCurBuyMenu->IsShown()) return;
	StartStopMenu(pCurBuyMenu, true);
}
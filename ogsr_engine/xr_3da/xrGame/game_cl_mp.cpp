#include "stdafx.h"
#include "game_cl_mp.h"
#include "xr_level_controller.h"
#include "xrMessages.h"
#include "GameObject.h"
#include "Actor.h"
#include "level.h"
#include "hudmanager.h"
#include "ui/UIChatWnd.h"
#include "ui/UIGameLog.h"
#include "clsid_game.h"
#include <dinput.h>
#include "UIGameCustom.h"
#include "ui/UIInventoryUtilities.h"
#include "ui/UIMessagesWindow.h"
#include "ui/UIMainIngameWnd.h"
#include "ui/UIMessageBoxEx.h"
#include "CustomZone.h"
#include "game_base_kill_type.h"
#include "game_base_menu_events.h"
#include "UIGameDM.h"
#include "ui/UITextureMaster.h"
#include "ui/UIVotingCategory.h"
#include "ui/UIVote.h"
#include "ui/UIMessageBoxEx.h"
#include "string_table.h"
#include "../IGame_Persistent.h"
#include "MainMenu.h"


#define EQUIPMENT_ICONS "ui\\ui_mp_icon_kill"
#define KILLEVENT_ICONS "ui\\ui_hud_mp_icon_death"
#define RADIATION_ICONS "ui\\ui_mn_radiations_hard"
#define BLOODLOSS_ICONS "ui\\ui_mn_wounds_hard"
#define RANK_ICONS		"ui\\ui_mp_icon_rank"

#define KILLEVENT_GRID_WIDTH	64
#define KILLEVENT_GRID_HEIGHT	64

#include "game_cl_mp_snd_messages.h"

game_cl_mp::game_cl_mp()
{
	m_bVotingActive = false;
	m_pVoteStartWindow = NULL;
	m_pVoteRespondWindow = NULL;
	m_pMessageBox = NULL;
	
	m_pSndMessages.clear();
	LoadSndMessages();
	m_bJustRestarted = true;
	m_pSndMessagesInPlay.clear();
	m_aMessageMenus.clear();

	m_bSpectatorSelected = FALSE;
	//-------------------------------------
	m_u8SpectatorModes		= 0xff;
	m_bSpectator_FreeFly	= true;
	m_bSpectator_FirstEye	= true;
	m_bSpectator_LookAt		= true;
	m_bSpectator_FreeLook	= true;
	m_bSpectator_TeamCamera	= true;
	//-------------------------------------
	LoadBonuses();
};

game_cl_mp::~game_cl_mp()
{
	CL_TEAM_DATA_LIST_it it = TeamList.begin();
	for(;it!=TeamList.end();++it)
	{
		if (it->IndicatorShader)
			it->IndicatorShader.destroy();
		if (it->InvincibleShader)
			it->InvincibleShader.destroy();
	};
	TeamList.clear();

	if (m_EquipmentIconsShader)
		m_EquipmentIconsShader.destroy();
	
	if (m_KillEventIconsShader)
		m_KillEventIconsShader.destroy();

	if (m_RadiationIconsShader)
		m_RadiationIconsShader.destroy();

	if (m_BloodLossIconsShader)
		m_BloodLossIconsShader.destroy();
	
	m_pSndMessagesInPlay.clear_and_free();
	m_pSndMessages.clear_and_free();

//	xr_delete(m_pSpeechMenu);
	DestroyMessagesMenus();

//	xr_delete(pBuySpawnMsgBox);

	m_pBonusList.clear();

	xr_delete(m_pVoteRespondWindow);
	xr_delete(m_pVoteStartWindow);
	xr_delete(m_pMessageBox);
};

CUIGameCustom*		game_cl_mp::createGameUI			()
{
//	m_pSpeechMenu = xr_new<CUISpeechMenu>("test_speech_section");
	HUD().GetUI()->m_pMessagesWnd->SetChatOwner(this);
		
	return NULL;
};

bool game_cl_mp::CanBeReady	()
{
	return true;
}

bool game_cl_mp::NeedToSendReady_Actor(int key, game_PlayerState* ps)
{
	return ((GAME_PHASE_PENDING == Phase() ) || 
			true == ps->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD) ) && 
			(kWPN_FIRE == key);
}

bool game_cl_mp::NeedToSendReady_Spectator(int key, game_PlayerState* ps)
{
	return (GAME_PHASE_PENDING==Phase() && kWPN_FIRE==key) || 
			(	kJUMP==key && 
				GAME_PHASE_INPROGRESS==Phase() && 
				CanBeReady() && 
				ps->DeathTime > 1000);
}

bool game_cl_mp::OnKeyboardPress(int key)
{
	CStringTable st;
	if ( kJUMP == key || kWPN_FIRE == key )
	{
		bool b_need_to_send_ready = false;

		CObject* curr = Level().CurrentControlEntity();
		if (!curr) return(false);

		bool is_actor		= curr->CLS_ID==CLSID_OBJECT_ACTOR;
		bool is_spectator	= curr->CLS_ID==CLSID_SPECTATOR;
		
		game_PlayerState* ps	= local_player;

		if (is_actor){
			b_need_to_send_ready = NeedToSendReady_Actor(key, ps);
		};
		if(is_spectator){
			b_need_to_send_ready =	NeedToSendReady_Spectator(key, ps);

		};
		if(b_need_to_send_ready){
				CGameObject* GO = smart_cast<CGameObject*>(curr);
				NET_Packet			P;
				GO->u_EventGen		(P,GE_GAME_EVENT,GO->ID()	);
				P.w_u16(GAME_EVENT_PLAYER_READY);
				GO->u_EventSend			(P);
				return true;
		}
		else
			return false;
	};

	if( (Phase() != GAME_PHASE_INPROGRESS) && (kQUIT != key) && (kCONSOLE != key))
		return true;

	if (Phase() == GAME_PHASE_INPROGRESS)
	{
		switch (key)
		{
		case kCHAT:
		case kCHAT_TEAM:
			{
				shared_str prefix;
				

				CUIChatWnd* pChatWnd = HUD().GetUI()->m_pMessagesWnd->GetChatWnd();

				if (kCHAT_TEAM == key)
				{
					prefix.sprintf("%s> ", *st.translate("st_mp_say_to_team"));
					
					pChatWnd->TeamChat();
				}
				else
				{
					prefix.sprintf("%s> ", *st.translate("st_mp_say_to_all"));					
					pChatWnd->AllChat();
				}

				pChatWnd->SetEditBoxPrefix(prefix);

				StartStopMenu(pChatWnd, false);
				if (!pChatWnd->IsShown() && xr_strlen(pChatWnd->UIEditBox.GetText()) > 0)
				{
					shared_str phrase = pChatWnd->UIEditBox.GetText();
					//				pChatWnd->Say(phrase);
					(kCHAT == key) ? ChatSayAll(phrase) : ChatSayTeam(phrase);
					pChatWnd->UIEditBox.SetText("");
				}
				return false;
			}break;
		case kVOTE_BEGIN:
			{
				if (IsVotingEnabled() && !IsVotingActive())
					VotingBegin();
				else
				{
					if (!IsVotingEnabled())
						OnCantVoteMsg(*st.translate("st_mp_disabled_voting"));
					else						
						OnCantVoteMsg(*st.translate("st_mp_only_one_voting"));
				};
			}break;
		case kVOTE:
			{
				if (IsVotingEnabled() && IsVotingActive())
					Vote();
				else
				{
					if (!IsVotingEnabled())
						OnCantVoteMsg(*st.translate("st_mp_disabled_voting"));
					else
						OnCantVoteMsg(*st.translate("st_mp_no_current_voting"));
				}
			}break;
		case kVOTEYES:
			{
				if (IsVotingEnabled() && IsVotingActive())
					SendVoteYesMessage();
			}break;
		case kVOTENO:
			{
				if (IsVotingEnabled() && IsVotingActive())
					SendVoteNoMessage();
			}break;
		case kSPEECH_MENU_0:
		case kSPEECH_MENU_1:
		case kSPEECH_MENU_2:
		case kSPEECH_MENU_3:
		case kSPEECH_MENU_4:
		case kSPEECH_MENU_5:
		case kSPEECH_MENU_6:
		case kSPEECH_MENU_7:
		case kSPEECH_MENU_8:
		case kSPEECH_MENU_9:
			{
				if (!local_player || local_player->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD)) break;

				u32 MenuID = key - kSPEECH_MENU_0;
				if (MenuID >= m_aMessageMenus.size()) break;
				cl_MessageMenu* pCurMenu = &(m_aMessageMenus[MenuID]);
				HideMessageMenus();
				StartStopMenu(pCurMenu->m_pSpeechMenu, FALSE);
				return true;
			}break;
		}		
	}

	return inherited::OnKeyboardPress(key);
}

void	game_cl_mp::VotingBegin()
{
	if(!m_pVoteStartWindow)
		m_pVoteStartWindow		= xr_new<CUIVotingCategory>();

	StartStopMenu(m_pVoteStartWindow, true);
}

void	game_cl_mp::Vote()
{
	if(!m_pVoteRespondWindow)
		m_pVoteRespondWindow	= xr_new<CUIVote>();

	StartStopMenu			(m_pVoteRespondWindow, true);
}

void	game_cl_mp::OnCantVoteMsg(LPCSTR Text)
{
	if (!m_pMessageBox)
		m_pMessageBox = xr_new<CUIMessageBoxEx>();

	m_pMessageBox->Init("cant_vote");
	m_pMessageBox->SetText(Text);
	StartStopMenu(m_pMessageBox, true);
}

bool	game_cl_mp::OnKeyboardRelease		(int key)
{
	return inherited::OnKeyboardRelease(key);
}

char	Color_Weapon[]	= "%c[255,255,1,1]";
u32		Color_Teams_u32[3]	= {color_rgba(255,240,190,255), color_rgba(64,255,64,255), color_rgba(64,64,255,255)};
LPSTR	Color_Teams[3]	= {"%c[255,255,240,190]", "%c[255,64,255,64]", "%c[255,64,64,255]"};
char	Color_Main[]	= "%c[255,192,192,192]";
char	Color_Radiation[]	= "%c[255,0,255,255]";
char	Color_Neutral[]	= "%c[255,255,0,255]";
u32		Color_Neutral_u32	= color_rgba(255,0,255,255);
char	Color_Red[]	= "%c[255,255,1,1]";
char	Color_Green[]	= "%c[255,1,255,1]";

void game_cl_mp::TranslateGameMessage	(u32 msg, NET_Packet& P)
{
	string512 Text;
	CStringTable st;

	switch(msg)	{
	
	case GAME_EVENT_PLAYER_KILLED: //dm
		{
			OnPlayerKilled(P);
		}break;
	case GAME_EVENT_VOTE_START:
		{
			sprintf_s(Text, "%s%s", Color_Main, *st.translate("mp_voting_started_msg"));
			CommonMessageOut(Text);
			OnVoteStart(P);
		}break;
	case GAME_EVENT_VOTE_STOP:
		{
			sprintf_s(Text, "%s%s", Color_Main, *st.translate("mp_voting_broken"));
			CommonMessageOut(Text);
			OnVoteStop(P);
		}break;
	case GAME_EVENT_VOTE_END:
		{
			string512 Reason;
			P.r_stringZ(Reason);
			sprintf_s(Text, "%s%s", Color_Main, *st.translate(Reason));
			CommonMessageOut(Text);
			OnVoteEnd(P);
		}break;
	case GAME_EVENT_PLAYER_NAME:
		{
			OnPlayerChangeName(P);
		}break;
	case GAME_EVENT_SPEECH_MESSAGE:
		{
			OnSpeechMessage(P);
		}break;
	case GAME_EVENT_PLAYERS_MONEY_CHANGED:
		{
			OnEventMoneyChanged(P);
		}break;
	case GAME_EVENT_PLAYER_GAME_MENU_RESPOND:
		{
			OnGameMenuRespond(P);
		}break;
	case GAME_EVENT_ROUND_STARTED:
		{
			OnGameRoundStarted();
		}break;
	case GAME_EVENT_ROUND_END:
		{
			string64 reason;
			P.r_stringZ(reason);
		}break;
	case GAME_EVENT_SERVER_STRING_MESSAGE:
		{
			string1024 mess;
			P.r_stringZ(mess);
			sprintf_s( Text, "%s%s", Color_Red, *st.translate(mess) );
			CommonMessageOut(Text);
		}break;
	case GAME_EVENT_SERVER_DIALOG_MESSAGE:
		{
			string1024 mess;
			P.r_stringZ(mess);
			Msg( mess );
			if ( MainMenu() && !g_dedicated_server )
			{
				MainMenu()->OnSessionTerminate( mess );
			}
		}break;
	default:
		inherited::TranslateGameMessage(msg,P);
	}
}


//////////////////////////////////////////////////////////////////////////

void game_cl_mp::ChatSayAll(const shared_str &phrase)
{
	NET_Packet	P;	
	P.w_begin(M_CHAT_MESSAGE);
	P.w_s16(0);
	P.w_stringZ(local_player->getName());
	P.w_stringZ(phrase.c_str());
	P.w_s16(local_player->team);
	u_EventSend(P);
}

//////////////////////////////////////////////////////////////////////////

void game_cl_mp::ChatSayTeam(const shared_str &phrase)
{

	NET_Packet	P;
	P.w_begin(M_CHAT_MESSAGE);
	P.w_s16(local_player->team);
	P.w_stringZ(local_player->getName());
	P.w_stringZ(phrase.c_str());
	P.w_s16(local_player->team);
	u_EventSend(P);
}

void game_cl_mp::OnWarnMessage(NET_Packet* P)
{
	u8 msg_type = P->r_u8();
	if(msg_type==1)
	{
		u16 _ping				= P->r_u16	();
		u8	_cnt				= P->r_u8	();
		u8	_total				= P->r_u8	();
		
		if(HUD().GetUI())
		{
			string512				_buff;
			sprintf_s					(_buff,"max_ping_warn_%d", _cnt);
			SDrawStaticStruct* ss	= HUD().GetUI()->AddInfoMessage(_buff);
			
			sprintf_s					(_buff,"%d ms.", _ping);
			ss->m_static->SetText	(_buff);
			CUIWindow*	w			= ss->m_static->FindChild("auto_static_0");
			if(w)
			{
				sprintf_s				(_buff,"%d/%d", _cnt, _total);
				CUIStatic* s		= smart_cast<CUIStatic*>(w);
				s->SetText			(_buff);
			}
		}
	}
}

void game_cl_mp::OnChatMessage(NET_Packet* P)
{
	P->r_s16();
	string256 PlayerName;
	P->r_stringZ(PlayerName);
	string256 ChatMsg;
	P->r_stringZ(ChatMsg);
	s16 team;
	P->r_s16(team);
///#ifdef DEBUG
	CStringTable st;
	switch (team)
	{
	case 0: Msg("%s: %s : %s",		*st.translate("mp_chat"), PlayerName, ChatMsg); break;
	case 1: Msg("- %s: %s : %s",	*st.translate("mp_chat"), PlayerName, ChatMsg); break;
	case 2: Msg("~ %s: %s : %s",	*st.translate("mp_chat"), PlayerName, ChatMsg); break;
	}
	
//#endif
	if(g_dedicated_server)	return;

	string256 colPlayerName;
	sprintf_s(colPlayerName, "%s%s:%s", Color_Teams[team], PlayerName, "%c[default]");
	if (Level().CurrentViewEntity() && HUD().GetUI())
		HUD().GetUI()->m_pMessagesWnd->AddChatMessage(ChatMsg, colPlayerName);
};

void game_cl_mp::CommonMessageOut		(LPCSTR msg)
{
	if(g_dedicated_server)	return;

	if (HUD().GetUI())
        HUD().GetUI()->m_pMessagesWnd->AddLogMessage(msg);
};


void game_cl_mp::shedule_Update(u32 dt)
{
	UpdateSndMessages();

	inherited::shedule_Update(dt);
	//-----------------------------------------

	if(g_dedicated_server)	return;

	switch (Phase())
	{
	case GAME_PHASE_PENDING:
		{
			CUIChatWnd* pChatWnd = HUD().GetUI()->m_pMessagesWnd->GetChatWnd();
			if (pChatWnd && pChatWnd->IsShown())
				StartStopMenu(pChatWnd, false);

			if (m_bJustRestarted)
			{
				if (Level().CurrentViewEntity())
				{
					PlaySndMessage(ID_READY);
					m_bJustRestarted = false;
				};
			}
		}break;
	case GAME_PHASE_INPROGRESS:
		{
			if (!local_player || local_player->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD))
			{
				HideMessageMenus();
			};
			
		}break;
	default:
		{
			CUIChatWnd* pChatWnd = HUD().GetUI()->m_pMessagesWnd->GetChatWnd();
			if (pChatWnd && pChatWnd->IsShown())
				StartStopMenu(pChatWnd, false);
		}break;
	}
	UpdateMapLocations();	

	u32 cur_game_state = Phase();

	if(m_pVoteStartWindow && m_pVoteStartWindow->IsShown() && cur_game_state!=GAME_PHASE_INPROGRESS)
	{
		m_pVoteStartWindow->GetHolder()->StartStopMenu(m_pVoteStartWindow, true);
	}
	if(m_pVoteRespondWindow && m_pVoteRespondWindow->IsShown() && (cur_game_state!=GAME_PHASE_INPROGRESS || !IsVotingActive()))
	{
		m_pVoteRespondWindow->GetHolder()->StartStopMenu(m_pVoteRespondWindow, true);
	}

	if(m_pMessageBox && m_pMessageBox->IsShown() && cur_game_state!=GAME_PHASE_INPROGRESS)
	{
		m_pMessageBox->GetHolder()->StartStopMenu(m_pMessageBox, true);
	}
	
}

void game_cl_mp::SendStartVoteMessage	(LPCSTR args)
{
	if (!args) return;
	if (!IsVotingEnabled()) return;
	NET_Packet P;
	Game().u_EventGen		(P,GE_GAME_EVENT,Game().local_player->GameID);
	P.w_u16(GAME_EVENT_VOTE_START);
	P.w_stringZ(args);
	Game().u_EventSend		(P);
};

void game_cl_mp::SendVoteYesMessage		()	
{
	if (!IsVotingEnabled() || !IsVotingActive()) return;
	NET_Packet P;
	Game().u_EventGen		(P,GE_GAME_EVENT,Game().local_player->GameID);
	P.w_u16(GAME_EVENT_VOTE_YES);
	Game().u_EventSend		(P);
};
void game_cl_mp::SendVoteNoMessage		()	
{
	if (!IsVotingEnabled() || !IsVotingActive()) return;
	NET_Packet P;
	Game().u_EventGen		(P,GE_GAME_EVENT,Game().local_player->GameID);
	P.w_u16(GAME_EVENT_VOTE_NO);
	Game().u_EventSend		(P);
};

void game_cl_mp::OnVoteStart				(NET_Packet& P)	
{
	SetVotingActive(true);
};
void game_cl_mp::OnVoteStop				(NET_Packet& P)	
{
	SetVotingActive(false);
	if(m_pVoteRespondWindow && m_pVoteRespondWindow->IsShown())
	{
		StartStopMenu			(m_pVoteRespondWindow, true);
	}
};

void game_cl_mp::OnVoteEnd				(NET_Packet& P)
{
	SetVotingActive(false);
};
void game_cl_mp::OnPlayerVoted			(game_PlayerState* ps)
{
	if (!IsVotingActive()) return;
	if (ps->m_bCurrentVoteAgreed == 2) return;

	CStringTable st;
	string1024 resStr;
	sprintf_s(resStr, "%s\"%s\" %s%s %s\"%s\"", Color_Teams[ps->team], ps->getName(), Color_Main, *st.translate("mp_voted"),
		ps->m_bCurrentVoteAgreed ? Color_Green : Color_Red, *st.translate(ps->m_bCurrentVoteAgreed ? "mp_voted_yes" : "mp_voted_no"));
	CommonMessageOut(resStr);
}
void game_cl_mp::LoadTeamData			(const shared_str& TeamName)
{
	cl_TeamStruct			Team;
	Team.IndicatorPos.set	(0.f,0.f,0.f);
	Team.Indicator_r1		= 0.f;
	Team.Indicator_r2		= 0.f;

	Team.caSection = TeamName;
	if (pSettings->section_exist(TeamName))
	{
		Team.Indicator_r1 =  pSettings->r_float(TeamName, "indicator_r1");
		Team.Indicator_r2 =  pSettings->r_float(TeamName, "indicator_r2");

		Team.IndicatorPos.x =  pSettings->r_float(TeamName, "indicator_x");
		Team.IndicatorPos.y =  pSettings->r_float(TeamName, "indicator_y");
		Team.IndicatorPos.z =  pSettings->r_float(TeamName, "indicator_z");
		
		LPCSTR ShaderType	= pSettings->r_string(TeamName, "indicator_shader");
		LPCSTR ShaderTexture = pSettings->r_string(TeamName, "indicator_texture");
		Team.IndicatorShader.create(ShaderType, ShaderTexture);

		ShaderType	= pSettings->r_string(TeamName, "invincible_shader");
		ShaderTexture = pSettings->r_string(TeamName, "invincible_texture");
		Team.InvincibleShader.create(ShaderType, ShaderTexture);
	};
	TeamList.push_back(Team);
}

void game_cl_mp::OnSwitchPhase_InProgress()
{
};

void game_cl_mp::OnSwitchPhase			(u32 old_phase, u32 new_phase)
{
	inherited::OnSwitchPhase(old_phase, new_phase);
	switch (new_phase)
	{
	case GAME_PHASE_INPROGRESS:
		{
			m_bSpectatorSelected = FALSE;

			if (Level().pHUD && HUD().GetUI())
			{
				HUD().GetUI()->ShowGameIndicators();
			};
		}break;
	case GAME_PHASE_PENDING:
		{
			m_bJustRestarted = true;
			HideMessageMenus();
		};

	case GAME_PHASE_TEAM1_SCORES:
	case GAME_PHASE_TEAM2_SCORES:
	case GAME_PHASE_TEAM1_ELIMINATED:
	case GAME_PHASE_TEAM2_ELIMINATED:
	case GAME_PHASE_TEAMS_IN_A_DRAW:
	case GAME_PHASE_PLAYER_SCORES:
			HideMessageMenus();
	break;

	default:
		{
			if (Level().pHUD && HUD().GetUI())
			{
				HUD().GetUI()->HideGameIndicators();
			};
			HideMessageMenus();
		}break;
	}
}

ref_shader game_cl_mp::GetEquipmentIconsShader	()
{
	if (m_EquipmentIconsShader) return m_EquipmentIconsShader;

	m_EquipmentIconsShader.create("hud\\default", EQUIPMENT_ICONS);
	return m_EquipmentIconsShader;
}

ref_shader game_cl_mp::GetKillEventIconsShader	()
{
	return GetEquipmentIconsShader();
	/*
	if (m_KillEventIconsShader) return m_KillEventIconsShader;

	m_KillEventIconsShader.create("hud\\default", KILLEVENT_ICONS);
	return m_KillEventIconsShader;
	*/
}

ref_shader game_cl_mp::GetRadiationIconsShader	()
{
	return GetEquipmentIconsShader();
	/*
	if (m_RadiationIconsShader) return m_RadiationIconsShader;

	m_RadiationIconsShader.create("hud\\default", RADIATION_ICONS);
	return m_RadiationIconsShader;
	*/
}

ref_shader game_cl_mp::GetBloodLossIconsShader	()
{
	return GetEquipmentIconsShader();
	/*
	if (m_BloodLossIconsShader) return m_BloodLossIconsShader;

	m_BloodLossIconsShader.create("hud\\default", BLOODLOSS_ICONS);
	return m_BloodLossIconsShader;
	*/
}
ref_shader		game_cl_mp::GetRankIconsShader()
{
	if (m_RankIconsShader) return m_RankIconsShader;

	m_RankIconsShader.create("hud\\default", RANK_ICONS);
	return m_RankIconsShader;
}

void game_cl_mp::OnPlayerKilled			(NET_Packet& P)
{
	CStringTable st;
	//-----------------------------------------------------------
	KILL_TYPE KillType = KILL_TYPE(P.r_u8());
	u16 KilledID = P.r_u16();
	u16 KillerID = P.r_u16();
	u16	WeaponID = P.r_u16();
	SPECIAL_KILL_TYPE SpecialKill = SPECIAL_KILL_TYPE(P.r_u8());
	//-----------------------------------------------------------
	CObject* pOKiller = Level().Objects.net_Find(KillerID);
	CObject* pWeapon = Level().Objects.net_Find(WeaponID);

	game_PlayerState* pPlayer = GetPlayerByGameID(KilledID);
	if (!pPlayer)
	{
		Msg("! Non existant player[%d] killed by [%d] with [%d]", KilledID, KillerID, WeaponID);
		return;
	}
	R_ASSERT(pPlayer);
	game_PlayerState* pKiller = GetPlayerByGameID(KillerID);
//	R_ASSERT(pKiller);
	//-----------------------------------------------------------
	KillMessageStruct KMS;
	KMS.m_victim.m_name = pPlayer->name;
	KMS.m_victim.m_color = Color_Teams_u32[pPlayer->team];

	KMS.m_killer.m_name = NULL;
	KMS.m_killer.m_color = color_rgba(255,255,255,255);

	KMS.m_initiator.m_shader = NULL;
	KMS.m_ext_info.m_shader = NULL;

	switch (KillType)
	{
		//-----------------------------------------------------------
	case KT_HIT:			//from hit
		{
			string1024	sWeapon = "", sSpecial = "";
			if (pWeapon)
			{
				CInventoryItem* pIItem = smart_cast<CInventoryItem*>(pWeapon);
				if (pIItem)
				{
					KMS.m_initiator.m_shader = GetEquipmentIconsShader();
					KMS.m_initiator.m_rect.x1 = pIItem->GetKillMsgXPos();
					KMS.m_initiator.m_rect.y1 = pIItem->GetKillMsgYPos();
					KMS.m_initiator.m_rect.x2 = KMS.m_initiator.m_rect.x1 + pIItem->GetKillMsgWidth();
					KMS.m_initiator.m_rect.y2 = KMS.m_initiator.m_rect.y1 + pIItem->GetKillMsgHeight();
					sprintf_s(sWeapon, "%s %s", st.translate("mp_from").c_str(), pIItem->NameShort());
				}
				else
				{
					CCustomZone* pAnomaly = smart_cast<CCustomZone*>(pWeapon);
					if (pAnomaly)
					{
						KMS.m_initiator.m_shader = GetKillEventIconsShader();
						KMS.m_initiator.m_rect.x1 = 1;
						KMS.m_initiator.m_rect.y1 = 202;
						KMS.m_initiator.m_rect.x2 = KMS.m_initiator.m_rect.x1 + 31;
						KMS.m_initiator.m_rect.y2 = KMS.m_initiator.m_rect.y1 + 30;
						sprintf_s(sWeapon, *st.translate("mp_by_anomaly"));
					}
				}
			}

			if (pKiller || pOKiller)
			{
				if (!pKiller)
				{
					CCustomZone* pAnomaly = smart_cast<CCustomZone*>(pOKiller);
					if (pAnomaly)
					{
						KMS.m_initiator.m_shader = GetKillEventIconsShader();
						KMS.m_initiator.m_rect.x1 = 1;
						KMS.m_initiator.m_rect.y1 = 202;
						KMS.m_initiator.m_rect.x2 = KMS.m_initiator.m_rect.x1 + 31;
						KMS.m_initiator.m_rect.y2 = KMS.m_initiator.m_rect.y1 + 30;
						Msg("%s killed by anomaly", *KMS.m_victim.m_name);
						break;
					}
				};

				if (pKiller)
				{
					KMS.m_killer.m_name = pKiller ? pKiller->name : *(pOKiller->cNameSect());
					KMS.m_killer.m_color = pKiller ? Color_Teams_u32[pKiller->team] : Color_Neutral_u32;
				};
			};
			//-------------------------------------------
			switch (SpecialKill)
			{
			case SKT_NONE:		// not special
				{
					if (pOKiller && pOKiller==Level().CurrentViewEntity())
					{
						if (pWeapon && pWeapon->CLS_ID == CLSID_OBJECT_W_KNIFE)
							PlaySndMessage(ID_BUTCHER);
					};
				}break;
			case SKT_HEADSHOT:		// Head Shot
				{
					BONUSES_it it = std::find(m_pBonusList.begin(), m_pBonusList.end(), "headshot");
					if (it != m_pBonusList.end() && (*it == "headshot")) 
					{
						Bonus_Struct* pBS = &(*it);
						KMS.m_ext_info.m_shader = pBS->IconShader;
						KMS.m_ext_info.m_rect.x1 = pBS->IconRects[0].x1;
						KMS.m_ext_info.m_rect.y1 = pBS->IconRects[0].y1;
						KMS.m_ext_info.m_rect.x2 = pBS->IconRects[0].x1 + pBS->IconRects[0].x2;
						KMS.m_ext_info.m_rect.y2 = pBS->IconRects[0].y1 + pBS->IconRects[0].y2;
					};

					sprintf_s(sSpecial, *st.translate("mp_with_headshot"));

					if (pOKiller && pOKiller==Level().CurrentViewEntity())
						PlaySndMessage(ID_HEADSHOT);
				}break;
			case SKT_BACKSTAB:		// BackStab
				{
					BONUSES_it it = std::find(m_pBonusList.begin(), m_pBonusList.end(), "backstab");
					if (it != m_pBonusList.end() && (*it == "backstab")) 
					{
						Bonus_Struct* pBS = &(*it);
						KMS.m_ext_info.m_shader = pBS->IconShader;
						KMS.m_ext_info.m_rect.x1 = pBS->IconRects[0].x1;
						KMS.m_ext_info.m_rect.y1 = pBS->IconRects[0].y1;
						KMS.m_ext_info.m_rect.x2 = pBS->IconRects[0].x1 + pBS->IconRects[0].x2;
						KMS.m_ext_info.m_rect.y2 = pBS->IconRects[0].y1 + pBS->IconRects[0].y2;
					};

					sprintf_s(sSpecial, *st.translate("mp_with_backstab"));

					if (pOKiller && pOKiller==Level().CurrentViewEntity())
						PlaySndMessage(ID_ASSASSIN);					
				}break;
			}
			//suicide
			if (KilledID == KillerID)
			{
				KMS.m_victim.m_name = NULL;

				KMS.m_ext_info.m_shader = GetKillEventIconsShader();
				KMS.m_ext_info.m_rect.x1 = 32;
				KMS.m_ext_info.m_rect.y1 = 202;
				KMS.m_ext_info.m_rect.x2 = KMS.m_ext_info.m_rect.x1 + 30;
				KMS.m_ext_info.m_rect.y2 = KMS.m_ext_info.m_rect.y1 + 30;
				//-------------------------------------
				Msg(sWeapon[0] ? "%s killed himself by %s" : "%s killed himself" , *KMS.m_killer.m_name, sWeapon[0] ? sWeapon+5 : "");
			}
			else
			{
				//-------------------------------------
				Msg("%s killed %s %s%s", *KMS.m_killer.m_name, *KMS.m_victim.m_name, sWeapon, sSpecial[0] ? sSpecial : "");
			}
		}break;
		//-----------------------------------------------------------
	case KT_BLEEDING:			//from bleeding
		{
			KMS.m_initiator.m_shader = GetBloodLossIconsShader();
			KMS.m_initiator.m_rect.x1 = 238;
			KMS.m_initiator.m_rect.y1 = 31;
			KMS.m_initiator.m_rect.x2 = KMS.m_initiator.m_rect.x1 + 17;
			KMS.m_initiator.m_rect.y2 = KMS.m_initiator.m_rect.y1 + 26;

			if (!pKiller)
			{
				CCustomZone* pAnomaly = smart_cast<CCustomZone*>(pOKiller);
				if (pAnomaly)
				{
					KMS.m_ext_info.m_shader = GetKillEventIconsShader();
						KMS.m_ext_info.m_rect.x1 = 1;
						KMS.m_ext_info.m_rect.y1 = 202;
						KMS.m_ext_info.m_rect.x2 = KMS.m_ext_info.m_rect.x1 + 31;
						KMS.m_ext_info.m_rect.y2 = KMS.m_ext_info.m_rect.y1 + 30;

					Msg("%s died from bleeding, thanks to anomaly", *KMS.m_victim.m_name);
					break;
				}
			};

			if (pKiller)
			{
				KMS.m_killer.m_name = pKiller ? pKiller->name : *(pOKiller->cNameSect());
				KMS.m_killer.m_color = pKiller ? Color_Teams_u32[pKiller->team] : Color_Neutral_u32;
				//-----------------------------------------------------------------------				
				Msg("%s died from bleeding, thanks to %s ", *KMS.m_victim.m_name, *KMS.m_killer.m_name);
			}
			else
			{
				//-----------------------------------------------------------------
				Msg("%s died from bleeding", *KMS.m_victim.m_name);
			};			
		}break;
		//-----------------------------------------------------------
	case KT_RADIATION:			//from radiation
		{			
			KMS.m_initiator.m_shader = GetRadiationIconsShader();
			KMS.m_initiator.m_rect.x1 = 215;
			KMS.m_initiator.m_rect.y1 = 195;
			KMS.m_initiator.m_rect.x2 = KMS.m_initiator.m_rect.x1 + 24;
			KMS.m_initiator.m_rect.y2 = KMS.m_initiator.m_rect.y1 + 24;
			//---------------------------------------------------------
			Msg("%s killed by radiation", *KMS.m_victim.m_name);
		}break;
	default:
		break;
	}
	if (HUD().GetUI() && HUD().GetUI()->m_pMessagesWnd)
		HUD().GetUI()->m_pMessagesWnd->AddLogMessage(KMS);
};

void	game_cl_mp::OnPlayerChangeName		(NET_Packet& P)
{
	CStringTable st;

	u16 ObjID = P.r_u16();
	s16 Team = P.r_s16();
	string1024 OldName, NewName;
	P.r_stringZ(OldName);
	P.r_stringZ(NewName);

	string1024 resStr;
	sprintf_s(resStr, "%s\"%s\" %s%s %s\"%s\"", Color_Teams[Team], OldName, Color_Main, *st.translate("mp_is_now"),Color_Teams[Team], NewName);
	CommonMessageOut(resStr);
	//-------------------------------------------
	CObject* pObj = Level().Objects.net_Find(ObjID);
	if (pObj)
		pObj->cName_set(NewName);

}

void	game_cl_mp::LoadSndMessages				()
{
	LoadSndMessage("mp_snd_messages", "headshot", ID_HEADSHOT);
	LoadSndMessage("mp_snd_messages", "butcher", ID_BUTCHER);
	LoadSndMessage("mp_snd_messages", "assassin", ID_ASSASSIN);
	LoadSndMessage("mp_snd_messages", "ready", ID_READY);
	LoadSndMessage("mp_snd_messages", "match_started", ID_MATCH_STARTED);
};

void	game_cl_mp::OnRankChanged	(u8 OldRank)
{
	CStringTable st;
	string256 tmp;
	string1024 RankStr;
	sprintf_s(tmp, "rank_%d",local_player->rank);
	sprintf_s(RankStr, "%s : %s", *st.translate("mp_your_rank"), *st.translate(READ_IF_EXISTS(pSettings, r_string, tmp, "rank_name", "")));
	CommonMessageOut(RankStr);	
#ifdef DEBUG
	Msg("- %s", RankStr);
#endif
};

void	game_cl_mp::net_import_update		(NET_Packet& P)
{
	u8 OldRank = u8(-1);
	s16 OldTeam = -1;
	if (local_player) 
	{
		OldRank = local_player->rank;
		OldTeam = local_player->team;
	};
	//---------------------------------------------
	inherited::net_import_update(P);
	//---------------------------------------------
	if (local_player)
	{
		if (OldTeam != local_player->team)	OnTeamChanged();
		if (OldRank != local_player->rank)	OnRankChanged(OldRank);
	};
}

void	game_cl_mp::net_import_state		(NET_Packet& P)
{
	u8 OldRank = u8(-1);
	s16 OldTeam = -1;
	if (local_player) 
	{
		OldRank = local_player->rank;
		OldTeam = local_player->team;
	};

	inherited::net_import_state(P);

	if (local_player)
	{
		if (OldTeam != local_player->team)	OnTeamChanged();
		if (OldRank != local_player->rank)	OnRankChanged(OldRank);
	};
	//-------------------------------------------------------------
	m_u8SpectatorModes = P.r_u8();
	
	m_bSpectator_FreeFly	= (m_u8SpectatorModes & (1<<CSpectator::eacFreeFly	)) != 0;
	m_bSpectator_FirstEye	= (m_u8SpectatorModes & (1<<CSpectator::eacFirstEye	)) != 0;
	m_bSpectator_LookAt		= (m_u8SpectatorModes & (1<<CSpectator::eacLookAt	)) != 0;
	m_bSpectator_FreeLook	= (m_u8SpectatorModes & (1<<CSpectator::eacFreeLook	)) != 0;
	m_bSpectator_TeamCamera = (m_u8SpectatorModes & (1<<CSpectator::eacMaxCam	)) != 0;
}

bool	game_cl_mp::Is_Spectator_Camera_Allowed			(CSpectator::EActorCameras Camera)
{
	/*
	switch (Camera)
	{
	case CSpectator::eacFreeFly		 : return m_bSpectator_FreeFly	;
	case CSpectator::eacFirstEye	 : return m_bSpectator_FirstEye	;
	case CSpectator::eacLookAt		 : return m_bSpectator_LookAt	;
	case CSpectator::eacFreeLook	 : return m_bSpectator_FreeLook	;	
	}
	return false;
	*/
	return (!!(m_u8SpectatorModes & (1<<Camera)));
};

void	game_cl_mp::OnEventMoneyChanged			(NET_Packet& P)
{
	if (!local_player) return;
	CUIGameDM* pUIDM = smart_cast<CUIGameDM*>(m_game_ui_custom);
	local_player->money_for_round = P.r_s32();
	OnMoneyChanged();
	{
		if(pUIDM)
		{
			string256					MoneyStr;
			itoa(local_player->money_for_round, MoneyStr, 10);
			pUIDM->ChangeTotalMoneyIndicator	(MoneyStr);
		}
	}
	s32 Money_Added = P.r_s32();
	if (Money_Added != 0)
	{
		if(pUIDM)
		{
			string256					MoneyStr;
			sprintf_s						(MoneyStr,(Money_Added>0)?"+%d":"%d", Money_Added);
			pUIDM->DisplayMoneyChange	(MoneyStr);
		}
	};
	u8 NumBonuses = P.r_u8();
	s32 TotalBonusMoney = 0;
	shared_str BonusStr = (NumBonuses > 1) ? "Your bonuses : " : ((NumBonuses == 1) ? "Your bonus : " : "");
	for (u8 i=0; i<NumBonuses; i++)
	{
		s32 BonusMoney = P.r_s32();
		SPECIAL_KILL_TYPE BonusReason = SPECIAL_KILL_TYPE(P.r_u8());
		u8 BonusKills = (BonusReason == SKT_KIR)? P.r_u8() : 0;
		TotalBonusMoney += BonusMoney;
		//---------------------------------------------------------
		KillMessageStruct BMS;
		string256	MoneyStr;
		if (BonusMoney >=0)
			sprintf_s		(MoneyStr, "+%d", BonusMoney);
		else
			sprintf_s		(MoneyStr, "-%d", BonusMoney);
		BMS.m_victim.m_name = MoneyStr;
		BMS.m_victim.m_color = 0xff00ff00;
		u32 RectID = 0;
		//---------------------------------------------------------
		shared_str BName = "";
		switch (BonusReason)
		{
		case SKT_HEADSHOT: 
			{
				BName = "headshot"; 
			}break;
		case SKT_BACKSTAB: 
			{
				BName = "backstab"; 
			}break;
		case SKT_KNIFEKILL: 
			{
				BName = "knife_kill"; 
			}break;
		case SKT_PDA: 
			{
				BName = "pda_taken"; 
			}break;
		case SKT_KIR: 
			{				
				BName.sprintf("%d_kill_in_row", BonusKills);
				
				sprintf_s		(MoneyStr, sizeof(MoneyStr), "%d", BonusKills);
				BMS.m_killer.m_name = MoneyStr;
				BMS.m_killer.m_color = 0xffff0000;
			}break;
		case SKT_NEWRANK:
			{
				BName = "new_rank"; 
				RectID = (local_player->rank+1)*2 + ModifyTeam(local_player->team);
			}break;
		};
		BONUSES_it it = std::find(m_pBonusList.begin(), m_pBonusList.end(), BName.c_str());
		if (it != m_pBonusList.end() && (*it == BName.c_str())) 
		{
			Bonus_Struct* pBS = &(*it);
						
			BMS.m_initiator.m_shader = pBS->IconShader;
			BMS.m_initiator.m_rect.x1 = pBS->IconRects[RectID].x1;
			BMS.m_initiator.m_rect.y1 = pBS->IconRects[RectID].y1;
			BMS.m_initiator.m_rect.x2 = pBS->IconRects[RectID].x1 + pBS->IconRects[RectID].x2;
			BMS.m_initiator.m_rect.y2 = pBS->IconRects[RectID].y1 + pBS->IconRects[RectID].y2;		
		};

		if (pUIDM) pUIDM->DisplayMoneyBonus(BMS);
	};
};

void	game_cl_mp::OnSpectatorSelect		()
{
	CObject *l_pObj = Level().CurrentEntity();

	CGameObject *l_pPlayer = smart_cast<CGameObject*>(l_pObj);
	if(!l_pPlayer) return;

	NET_Packet		P;
	l_pPlayer->u_EventGen		(P, GE_GAME_EVENT, l_pPlayer->ID()	);
//	P.w_u16(GAME_EVENT_PLAYER_SELECT_SPECTATOR);
	P.w_u16(GAME_EVENT_PLAYER_GAME_MENU);
	P.w_u8(PLAYER_SELECT_SPECTATOR);
	l_pPlayer->u_EventSend		(P);

	m_bSpectatorSelected = TRUE;	
};

void	game_cl_mp::OnGameMenuRespond		(NET_Packet& P)
{
	u8 Respond = P.r_u8();
	switch (Respond)
	{
	case PLAYER_SELECT_SPECTATOR:
		{
			OnGameMenuRespond_Spectator(P);
		}break;
	case PLAYER_CHANGE_TEAM:
		{
			OnGameMenuRespond_ChangeTeam(P);
		}break;
	case PLAYER_CHANGE_SKIN:
		{
			OnGameMenuRespond_ChangeSkin(P);
		}break;
	}
};

void	game_cl_mp::OnGameRoundStarted				()
{
	//			sprintf_s(Text, "%sRound started !!!",Color_Main);
	string512 Text;
	CStringTable st;
	sprintf_s(Text, "%s%s",Color_Main, *st.translate("mp_match_started"));
	CommonMessageOut(Text);
	OnSwitchPhase_InProgress();
	//-------------------------------
	PlaySndMessage(ID_MATCH_STARTED);
}

void game_cl_mp::OnBuySpawn(CUIWindow* pWnd, void* p)
{
	OnBuySpawnMenu_Ok();
};

void game_cl_mp::LoadBonuses				()
{
	if (!pSettings->section_exist("mp_bonus_money")) return;
	m_pBonusList.clear();
	u32 BonusCount = pSettings->line_count("mp_bonus_money");
	for (u32 i=0; i<BonusCount; i++)
	{
		LPCSTR line, name;
		pSettings->r_line("mp_bonus_money", i, &name, &line);
		//-------------------------------------
		string1024 tmp0, tmp1, IconStr;
		_GetItem(line, 0, tmp0);
		_GetItem(line, 1, tmp1);
		if (strstr(name, "kill_in_row")) 
		{
			sprintf_s(tmp1, "%s Kill", tmp1);
			sprintf_s(IconStr, "kill_in_row");
		}
		else
			sprintf_s(IconStr, "%s",name);
		//-------------------------------------
		Bonus_Struct	NewBonus;
		NewBonus.BonusTypeName = name;
		NewBonus.BonusName = tmp1;
		NewBonus.MoneyStr = tmp0;
		NewBonus.Money = atol(tmp0);
		//-------------------------------------
		if (!strstr(name, "new_rank"))
		{
			string1024 IconShader, IconX, IconY, IconW, IconH;
			sprintf_s(IconShader, "%s_shader", IconStr);
			sprintf_s(IconX, "%s_x", IconStr);
			sprintf_s(IconY, "%s_y", IconStr);
			sprintf_s(IconW, "%s_w", IconStr);
			sprintf_s(IconH, "%s_h", IconStr);
			if (pSettings->line_exist("mp_bonus_icons", IconShader))
			{			
				NewBonus.IconShader.create("hud\\default", pSettings->r_string("mp_bonus_icons", IconShader));
			}
			Frect IconRect;
			IconRect.x1 = READ_IF_EXISTS(pSettings, r_float, "mp_bonus_icons", IconX,0);
			IconRect.y1 = READ_IF_EXISTS(pSettings, r_float, "mp_bonus_icons", IconY,0);
			IconRect.x2 = READ_IF_EXISTS(pSettings, r_float, "mp_bonus_icons", IconW,0);
			IconRect.y2 = READ_IF_EXISTS(pSettings, r_float, "mp_bonus_icons", IconH,0);
			NewBonus.IconRects.push_back(IconRect);
		}
		else
		{
			LPCSTR IconShader = CUITextureMaster::GetTextureFileName("ui_hud_status_blue_01");			
			NewBonus.IconShader.create("hud\\default", IconShader);

			Frect IconRect;
			for (u32 r=1; r<=5; r++)
			{
				string256 rankstr;				

				sprintf_s(rankstr, "ui_hud_status_green_0%d", r);
				IconRect = CUITextureMaster::GetTextureRect(rankstr);
				IconRect.x2 -= IconRect.x1;
				IconRect.y2 -= IconRect.y1;
				NewBonus.IconRects.push_back(IconRect);

				sprintf_s(rankstr, "ui_hud_status_blue_0%d", r);
				IconRect = CUITextureMaster::GetTextureRect(rankstr);
				IconRect.x2 -= IconRect.x1;
				IconRect.y2 -= IconRect.y1;
				NewBonus.IconRects.push_back(IconRect);
			}			
		};
		//--------------------------------------
		m_pBonusList.push_back(NewBonus);
	};
};

void game_cl_mp::OnRadminMessage(u16 type, NET_Packet* P)
{
	switch(type)
	{
	case M_REMOTE_CONTROL_CMD:
		{
				string4096		buff;
				P->r_stringZ	(buff);
				Msg				("# srv: %s",buff);
		}break;
	}
}

#pragma once

#include "UIGameCustom.h"
#include "ui/KillMessageStruct.h"


class CUIDMPlayerList;
class CUIDMStatisticWnd;
class CUISkinSelectorWnd;
class game_cl_Deathmatch;
class CUIMoneyIndicator;
class CUIRankIndicator;
class UIVoteStatusWnd;

class CUIInventoryWnd;
class CUIPdaWnd;
class CUIMapDesc;

class CUIGameDM: public CUIGameCustom
{
private:
	game_cl_Deathmatch *	m_game;
	typedef CUIGameCustom inherited;

public:
	CUIInventoryWnd*	m_pInventoryMenu;
	CUIPdaWnd*			m_pPdaMenu;
	CUIMapDesc*			m_pMapDesc;

protected:
	enum{
		flShowFragList	= (1<<1),
		fl_force_dword	= u32(-1)	};


//.	DEF_VECTOR			(PLAYERS_LISTS, CUIDMPlayerList*);

	CUIWindow*				m_pFragLists;
	CUIWindow*				m_pPlayerLists;
	//-----------------------------------------
	CUIWindow*				m_pStatisticWnds;
	//-----------------------------------------
	shared_str					m_time_caption;		
	shared_str					m_spectrmode_caption;		
	
	shared_str					m_spectator_caption;
	shared_str					m_pressjump_caption;
	shared_str					m_pressbuy_caption;
//.	shared_str					m_vote_caption0, m_vote_caption1;
//.	shared_str					m_votetimeresult_caption;
	shared_str					m_round_result_caption;		
	shared_str					m_force_respawn_time_caption;
	shared_str					m_demo_play_caption;
	shared_str					m_warm_up_caption;


	virtual	void					ClearLists				();

	CUIMoneyIndicator*				m_pMoneyIndicator;
	CUIRankIndicator*				m_pRankIndicator;
	CUIStatic*						m_pFragLimitIndicator;
	UIVoteStatusWnd*				m_voteStatusWnd;
public:
									CUIGameDM				();
	virtual 						~CUIGameDM				();

	virtual void					SetClGame				(game_cl_GameState* g);
	virtual	void					Init					();
	virtual void					Render					();
	virtual void					OnFrame					();

	void							SetRank							(s16 team, u8 rank);

	void							ChangeTotalMoneyIndicator		(LPCSTR newMoneyString);
	void							DisplayMoneyChange				(LPCSTR deltaMoney);
	void							DisplayMoneyBonus				(KillMessageStruct bonus);
	virtual void					SetFraglimit					(int local_frags, int fraglimit);

			void					SetTimeMsgCaption				(LPCSTR str);
			void					SetSpectrModeMsgCaption			(LPCSTR str);

			void					SetSpectatorMsgCaption			(LPCSTR str);
			void					SetPressJumpMsgCaption			(LPCSTR str);
			void					SetPressBuyMsgCaption			(LPCSTR str);
			void					SetRoundResultCaption			(LPCSTR str);
			void					SetForceRespawnTimeCaption		(LPCSTR str);
			void					SetDemoPlayCaption				(LPCSTR str);
			void					SetWarmUpCaption				(LPCSTR str);

			void					SetVoteMessage					(LPCSTR str);
			void					SetVoteTimeResultMsg			(LPCSTR str);

	virtual bool		IR_OnKeyboardPress		(int dik);
	virtual bool		IR_OnKeyboardRelease	(int dik);


			


			void					ShowFragList			(bool bShow);
			void					ShowPlayersList			(bool bShow);
			void					ShowStatistic			(bool bShow);
	virtual void					ReInitShownUI		(); 
	virtual	void					reset_ui				();
};

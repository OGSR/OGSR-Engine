#pragma once

#include "UIGameCustom.h"
#include "UIGameDM.h"

#include "ui/UIDialogWnd.h"
#include "ui/UIInventoryWnd.h"
#include "ui/UISpawnWnd.h"

// refs 

class CUITDMFragList;
class CUITDMPlayerList;
class CUISkinSelectorWnd;
class game_cl_TeamDeathmatch;
class CUIStatic;
class CUISpawnWnd;

class CUIGameTDM: public CUIGameDM
{
private:
	game_cl_TeamDeathmatch * m_game;
	typedef CUIGameDM inherited;

public:
	CUISpawnWnd*		m_pUITeamSelectWnd;

protected:
	CUIStatic*			m_team1_icon;
	CUIStatic*			m_team2_icon;
	CUIStatic*			m_team1_score;
	CUIStatic*			m_team2_score;

public:
	CUIGameTDM			();
	virtual 			~CUIGameTDM				();
	virtual void		SetClGame				(game_cl_GameState* g);
	virtual	void		Init					();
			void		SetScoreCaption			(int t1, int t2);			
	virtual void		SetFraglimit			(int local_frags, int fraglimit);
	virtual void		Render					();
	virtual void		OnFrame					();
	
	virtual	bool		IR_OnKeyboardPress		( int dik );
	virtual bool		IR_OnKeyboardRelease	( int dik );
	virtual	void		reset_ui				();
};

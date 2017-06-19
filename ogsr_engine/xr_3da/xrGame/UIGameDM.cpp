#include "stdafx.h"
#include "UIGameDM.h"

//.#include "UIDMPlayerList.h"
//.#include "UIDMFragList.h"
#include "UIDMStatisticWnd.h"
#include "ui/UISkinSelector.h"
#include "ui/UIInventoryWnd.h"
#include "ui/UIPdaWnd.h"
#include "ui/UIMapDesc.h"
#include "HUDManager.h"
#include "level.h"
#include "game_cl_base.h"
#include "Spectator.h"
#include "Inventory.h"
#include "xrserver_objects_alife_items.h"
#include "xr_level_controller.h"
#include "ui/xrUIXmlParser.h"
#include "ui/UIFrags.h"
#include "game_cl_Deathmatch.h"
#include "ui/UIMoneyIndicator.h"
#include "ui/UIRankIndicator.h"
#include "ui/UIVoteStatusWnd.h"

#include "object_broker.h"

#define MSGS_OFFS 510

#define TIME_MSG_COLOR		0xffff0000
#define SPECTRMODE_MSG_COLOR		0xffff0000
#define NORMAL_MSG_COLOR	0xffffffff
#define ROUND_RESULT_COLOR	0xfff0fff0
#define VOTE0_MSG_COLOR	0xffff0000
#define VOTE1_MSG_COLOR	0xff00ff00
#define DEMOPLAY_COLOR	0xff00ff00
#define WARM_UP_COLOR	0xff00ff00


#define DI2PX(x) float(iFloor((x+1)*float(UI_BASE_WIDTH)*0.5f))
#define DI2PY(y) float(iFloor((y+1)*float(UI_BASE_HEIGHT)*0.5f))
#define SZ(x) x*UI_BASE_WIDTH
//--------------------------------------------------------------------
CUIGameDM::CUIGameDM()
{
	m_game			= NULL; 
	m_pFragLists					= xr_new<CUIWindow>();
	m_pPlayerLists					= xr_new<CUIWindow>();
	m_pStatisticWnds					= xr_new<CUIWindow>();

	m_time_caption = "timelimit";
	GameCaptions()->addCustomMessage(m_time_caption, DI2PX(0.0f), DI2PY(-0.8f), SZ(0.03f), HUD().Font().pFontGraffiti19Russian, CGameFont::alCenter, TIME_MSG_COLOR, "");
	m_spectrmode_caption = "spetatormode";
	GameCaptions()->addCustomMessage(m_spectrmode_caption, DI2PX(0.0f), DI2PY(-0.7f), SZ(0.03f), HUD().Font().pFontGraffiti19Russian, CGameFont::alCenter, SPECTRMODE_MSG_COLOR, "");
	m_spectator_caption = "spectator";
	GameCaptions()->addCustomMessage(m_spectator_caption, DI2PX(0.0f), DI2PY(0.0f), SZ(0.03f), HUD().Font().pFontGraffiti19Russian, CGameFont::alCenter, NORMAL_MSG_COLOR, "");
	m_pressjump_caption = "pressjump";
	GameCaptions()->addCustomMessage(m_pressjump_caption, DI2PX(0.0f), DI2PY(0.9f), SZ(0.02f), HUD().Font().pFontGraffiti19Russian, CGameFont::alCenter, NORMAL_MSG_COLOR, "");
	m_pressbuy_caption = "pressbuy";
	GameCaptions()->addCustomMessage(m_pressbuy_caption, DI2PX(0.0f), DI2PY(0.95f), SZ(0.02f), HUD().Font().pFontGraffiti19Russian, CGameFont::alCenter, NORMAL_MSG_COLOR, "");
	m_round_result_caption =	"round_result";
	GameCaptions()->addCustomMessage(m_round_result_caption, DI2PX(0.0f), DI2PY(-0.1f), SZ(0.03f), HUD().Font().pFontGraffiti19Russian, CGameFont::alCenter, ROUND_RESULT_COLOR, "");
	m_force_respawn_time_caption =	"force_respawn_time";
	GameCaptions()->addCustomMessage(m_force_respawn_time_caption, DI2PX(0.0f), DI2PY(-0.9f), SZ(0.02f), HUD().Font().pFontGraffiti19Russian, CGameFont::alCenter, NORMAL_MSG_COLOR, "");
	m_demo_play_caption =	"demo_play";
	GameCaptions()->addCustomMessage(m_demo_play_caption, DI2PX(-1.0f), DI2PY(-0.95f), SZ(0.05f), HUD().Font().pFontGraffiti19Russian, CGameFont::alLeft, DEMOPLAY_COLOR, "");
	
	m_warm_up_caption =	"warm_up";
	GameCaptions()->addCustomMessage(m_warm_up_caption, DI2PX(0.0f), DI2PY(-0.75f), SZ(0.05f), HUD().Font().pFontGraffiti19Russian, CGameFont::alCenter, WARM_UP_COLOR, "");

	CUIXml							uiXml;
	uiXml.Init						(CONFIG_PATH, UI_PATH, "ui_game_dm.xml");
	m_pMoneyIndicator				= xr_new<CUIMoneyIndicator>();
	m_pMoneyIndicator->InitFromXML	(uiXml);
	m_pRankIndicator				= xr_new<CUIRankIndicator>();
	m_pRankIndicator->InitFromXml	(uiXml);
	m_pFragLimitIndicator			= xr_new<CUIStatic>();
	CUIXmlInit::InitStatic			(uiXml,"fraglimit",0,m_pFragLimitIndicator);

//.	m_voteStatusWnd					= xr_new<UIVoteStatusWnd>();
//.	m_voteStatusWnd->InitFromXML	(uiXml);
//.	m_voteStatusWnd->Show			(false);
	m_voteStatusWnd					= NULL;

	m_pInventoryMenu	= xr_new<CUIInventoryWnd>	();
	m_pPdaMenu			= xr_new<CUIPdaWnd>			();
	m_pMapDesc			= NULL;//xr_new<CUIMapDesc>		();

}
//--------------------------------------------------------------------
void CUIGameDM::SetClGame (game_cl_GameState* g)
{
	inherited::SetClGame(g);
	m_game = smart_cast<game_cl_Deathmatch*>(g);
	R_ASSERT(m_game);

	if (m_pMapDesc && m_pMapDesc->IsShown())
	{
		HUD().GetUI()->StartStopMenu(m_pMapDesc, true);
	}
	delete_data(m_pMapDesc);
	m_pMapDesc			= xr_new<CUIMapDesc>		();
}

void	CUIGameDM::Init				()
{
	CUIXml xml_doc;
	bool xml_result = xml_doc.Init(CONFIG_PATH, UI_PATH, "stats.xml");
	R_ASSERT2(xml_result, "xml file not found");

	CUIFrags* pFragList		= xr_new<CUIFrags>();
	CUIFrags* pPlayerList	= xr_new<CUIFrags>();
	CUIDMStatisticWnd* pStatisticWnd = xr_new<CUIDMStatisticWnd>();
	pFragList->SetAutoDelete(true);
	pPlayerList->SetAutoDelete(true);
	pStatisticWnd->SetAutoDelete(true);


	float ScreenW = UI_BASE_WIDTH;
	float ScreenH = UI_BASE_HEIGHT;
	//-----------------------------------------------------------
	pFragList->Init(xml_doc, "stats_wnd","frag_wnd_dm");
	pPlayerList->Init(xml_doc,"players_wnd","frag_wnd_dm");

	Frect FrameRect = pFragList->GetWndRect();
	float FrameW	= FrameRect.right - FrameRect.left;
	float FrameH	= FrameRect.bottom - FrameRect.top;
	pFragList->SetWndPos((ScreenW-FrameW)/2.0f, (ScreenH - FrameH)/2.0f);


	m_pFragLists->AttachChild(pFragList);
	//-----------------------------------------------------------
	FrameRect = pPlayerList->GetWndRect ();
	FrameW	= FrameRect.right - FrameRect.left;
	FrameH	= FrameRect.bottom - FrameRect.top;
	pPlayerList->SetWndPos((ScreenW-FrameW)/2.0f, (ScreenH - FrameH)/2.0f);

	m_pPlayerLists->AttachChild(pPlayerList);
	//-----------------------------------------------------------
	FrameRect = pStatisticWnd->GetFrameRect ();
	FrameW	= FrameRect.right - FrameRect.left;
	FrameH	= FrameRect.bottom - FrameRect.top;
	pStatisticWnd->SetWndRect((ScreenW-FrameW)/2.0f, (ScreenH - FrameH)/2.0f, FrameW, FrameH);

	m_pStatisticWnds->AttachChild(pStatisticWnd);

};
//--------------------------------------------------------------------

void	CUIGameDM::ClearLists ()
{
	m_pFragLists->DetachAll		();
	m_pPlayerLists->DetachAll	();
	m_pStatisticWnds->DetachAll	();
}
//--------------------------------------------------------------------
CUIGameDM::~CUIGameDM()
{
	ClearLists					();
	xr_delete					(m_pFragLists);
	xr_delete					(m_pPlayerLists);
	xr_delete					(m_pStatisticWnds);
	xr_delete					(m_pMoneyIndicator);
	xr_delete					(m_pRankIndicator);
	xr_delete					(m_pFragLimitIndicator);
	xr_delete					(m_voteStatusWnd);
	//---------------------------------------------------
	delete_data(m_pInventoryMenu);
	delete_data(m_pPdaMenu);	
	delete_data(m_pMapDesc);	
}

void	CUIGameDM::ReInitShownUI		() 
{
	if (m_pInventoryMenu && m_pInventoryMenu->IsShown())
	{
		m_pInventoryMenu->InitInventory();
	}
};
//--------------------------------------------------------------------

void CUIGameDM::SetTimeMsgCaption		(LPCSTR str)
{
		GameCaptions()->setCaption(m_time_caption, str, TIME_MSG_COLOR, true);
}

void CUIGameDM::ShowFragList			(bool bShow)
{
	if(bShow)
		AddDialogToRender(m_pFragLists);
	else
		RemoveDialogToRender(m_pFragLists);

}

void CUIGameDM::ShowPlayersList			(bool bShow)
{
	if(bShow)
		AddDialogToRender(m_pPlayerLists);
	else
		RemoveDialogToRender(m_pPlayerLists);
}
void CUIGameDM::ShowStatistic			(bool bShow)
{
	if(bShow)
		AddDialogToRender(m_pStatisticWnds);
	else
		RemoveDialogToRender(m_pStatisticWnds);
}

void CUIGameDM::SetSpectrModeMsgCaption		(LPCSTR str)
{
		GameCaptions()->setCaption(m_spectrmode_caption, str, SPECTRMODE_MSG_COLOR, true);
}

void CUIGameDM::SetSpectatorMsgCaption		(LPCSTR str)
{
		if(GameCaptions()) GameCaptions()->setCaption(m_spectator_caption, str, NORMAL_MSG_COLOR, true);
}

void CUIGameDM::SetPressJumpMsgCaption		(LPCSTR str)
{
		GameCaptions()->setCaption(m_pressjump_caption, str, NORMAL_MSG_COLOR, true);
}

void CUIGameDM::SetPressBuyMsgCaption		(LPCSTR str)
{
		GameCaptions()->setCaption(m_pressbuy_caption, str, NORMAL_MSG_COLOR, true);
}


void CUIGameDM::SetRoundResultCaption(LPCSTR str)
{
	GameCaptions()->setCaption(m_round_result_caption, str, ROUND_RESULT_COLOR, true);
}

void CUIGameDM::SetForceRespawnTimeCaption(LPCSTR str)
{
	GameCaptions()->setCaption(m_force_respawn_time_caption, str, NORMAL_MSG_COLOR, true);
}

void CUIGameDM::SetDemoPlayCaption(LPCSTR str)
{
	GameCaptions()->setCaption(m_demo_play_caption, str, DEMOPLAY_COLOR, true);
}

void CUIGameDM::SetWarmUpCaption				(LPCSTR str)
{
	GameCaptions()->setCaption(m_warm_up_caption, str, WARM_UP_COLOR, true);
}

void CUIGameDM::SetVoteMessage					(LPCSTR str)
{
	if(!str)
		xr_delete(m_voteStatusWnd);
	else{
		if(!m_voteStatusWnd)
		{
			CUIXml							uiXml;
			uiXml.Init						(CONFIG_PATH, UI_PATH, "ui_game_dm.xml");
			m_voteStatusWnd					= xr_new<UIVoteStatusWnd>();
			m_voteStatusWnd->InitFromXML	(uiXml);
		}
		m_voteStatusWnd->Show				(true);
		m_voteStatusWnd->SetVoteMsg			(str);
	}
};

void CUIGameDM::SetVoteTimeResultMsg			(LPCSTR str)
{
	if(m_voteStatusWnd)
		m_voteStatusWnd->SetVoteTimeResultMsg(str);
}

bool		CUIGameDM::IR_OnKeyboardPress		(int dik)
{
	if(inherited::IR_OnKeyboardPress(dik)) return true;

	EGameActions cmd  = get_binded_action(dik);
	switch ( cmd )
	{
	case kINVENTORY: 
	case kBUY:
	case kSKIN:
	case kTEAM:
		case kMAP:
		{
			return Game().OnKeyboardPress( cmd );
		}break;
	}
	return false;
};

bool CUIGameDM::IR_OnKeyboardRelease	(int dik)
{
	return false;
};

void CUIGameDM::OnFrame()
{
	inherited::OnFrame				();
	m_pMoneyIndicator->Update		();
	m_pRankIndicator->Update		();
	
	m_pFragLimitIndicator->Update	();
	if(m_voteStatusWnd && m_voteStatusWnd->IsShown()) 
		m_voteStatusWnd->Update		();
}

void CUIGameDM::Render()
{
	inherited::Render				();
	m_pMoneyIndicator->Draw			();
	m_pRankIndicator->Draw			();

	m_pFragLimitIndicator->Draw		();
	if(m_voteStatusWnd && m_voteStatusWnd->IsShown()) 
		m_voteStatusWnd->Draw		();
}

void CUIGameDM::DisplayMoneyChange(LPCSTR deltaMoney)
{
	m_pMoneyIndicator->SetMoneyChange(deltaMoney);
}

void CUIGameDM::DisplayMoneyBonus(KillMessageStruct bonus){
	m_pMoneyIndicator->AddBonusMoney(bonus);
}

void CUIGameDM::ChangeTotalMoneyIndicator(LPCSTR newMoneyString)
{
	m_pMoneyIndicator->SetMoneyAmount(newMoneyString);
}

void	CUIGameDM::SetRank(s16 team, u8 rank)
{
	m_pRankIndicator->SetRank(u8(m_game->ModifyTeam(team)), rank);
};

void CUIGameDM::SetFraglimit(int local_frags, int fraglimit)
{
	string64 str;
	if(fraglimit)
		sprintf_s(str,"%d/%d", local_frags, fraglimit);
	else
		sprintf_s(str,"%d", local_frags);

	m_pFragLimitIndicator->SetText(str);
}

void CUIGameDM::reset_ui				()
{
	inherited::reset_ui		();
	m_pInventoryMenu->Reset	();
	m_pPdaMenu->Reset		();
}
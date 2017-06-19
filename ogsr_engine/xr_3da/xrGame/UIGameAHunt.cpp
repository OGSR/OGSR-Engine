#include "stdafx.h"
#include "UIGameAHunt.h"

//.#include "UIAHuntPlayerList.h"
//.#include "UIAHuntFragList.h"
#include "UIDMStatisticWnd.h"

#include "hudmanager.h"
#include "team_base_zone.h"
#include "level.h"
#include "game_cl_ArtefactHunt.h"
#include "ui/UIFrags2.h"
#include "ui/UIProgressShape.h"
#include "ui/UIXmlInit.h"
#include "ui/UIMessageBoxEx.h"

#include "object_broker.h"

#define MSGS_OFFS 510

#define BUY_MSG_COLOR		0xffffff00
#define SCORE_MSG_COLOR		0xffffffff
#define REINFORCEMENT_MSG_COLOR		0xff8080ff
#define TODO_MSG_COLOR		0xff00ff00

#define DI2PX(x) float(iFloor((x+1)*float(UI_BASE_WIDTH)*0.5f))
#define DI2PY(y) float(iFloor((y+1)*float(UI_BASE_HEIGHT)*0.5f))
#define SZ(x) x*UI_BASE_WIDTH

CUIGameAHunt::CUIGameAHunt()
{
	CUIXml							uiXml;
	uiXml.Init						(CONFIG_PATH, UI_PATH, "ui_game_ahunt.xml");
	if(m_pFragLimitIndicator)
		xr_delete(m_pFragLimitIndicator);

	m_pFragLimitIndicator			= xr_new<CUIStatic>();
	CUIXmlInit::InitStatic			(uiXml, "fraglimit",0,		m_pFragLimitIndicator);

    m_pReinforcementInidcator = xr_new<CUIProgressShape>();

	CUIXmlInit::InitProgressShape	(uiXml, "reinforcement", 0, m_pReinforcementInidcator);			
	//-------------------------------------------------------------
	m_pBuySpawnMsgBox	= NULL;
}
//--------------------------------------------------------------------
void CUIGameAHunt::SetClGame (game_cl_GameState* g)
{
	inherited::SetClGame(g);
	m_game = smart_cast<game_cl_ArtefactHunt*>(g);
	R_ASSERT(m_game);
	//-----------------------------------------------------------------------
	delete_data(m_pBuySpawnMsgBox);
	m_pBuySpawnMsgBox	= xr_new<CUIMessageBoxEx>();	
	m_pBuySpawnMsgBox->Init("message_box_buy_spawn");
	m_pBuySpawnMsgBox->SetText("");

	game_cl_mp* clmp_game = smart_cast<game_cl_mp*>(g);
	m_pBuySpawnMsgBox->AddCallback("msg_box", MESSAGE_BOX_YES_CLICKED, CUIWndCallback::void_function(clmp_game, &game_cl_mp::OnBuySpawn));
}

void CUIGameAHunt::Init	()
{
	CUIXml xml_doc;
	bool xml_result = xml_doc.Init(CONFIG_PATH, UI_PATH, "stats.xml");
	R_ASSERT2(xml_result, "xml file not found");

	CUIFrags2* pFragList		= xr_new<CUIFrags2>();			pFragList->SetAutoDelete(true);
	//-----------------------------------------------------------
	CUIDMStatisticWnd* pStatisticWnd = xr_new<CUIDMStatisticWnd>(); pStatisticWnd->SetAutoDelete(true);

	pFragList->Init(xml_doc, "stats_wnd", "frag_wnd_tdm");

	float ScreenW = UI_BASE_WIDTH;
	float ScreenH = UI_BASE_HEIGHT;
	//-----------------------------------------------------------
	Frect FrameRect = pFragList->GetWndRect ();
	float FrameW	= FrameRect.right - FrameRect.left;
	float FrameH	= FrameRect.bottom - FrameRect.top;

	pFragList->SetWndPos((ScreenW-FrameW)/2.0f, (ScreenH - FrameH)/2.0f);

	//-----------------------------------------------------------
	m_pFragLists->AttachChild(pFragList);
	//-----------------------------------------------------------

	CUIFrags2* pPlayerListT1	= xr_new<CUIFrags2>	();pPlayerListT1->SetAutoDelete(true);

	pPlayerListT1->Init(xml_doc, "players_wnd", "frag_wnd_tdm");
	//-----------------------------------------------------------
	FrameRect = pPlayerListT1->GetWndRect ();
	FrameW	= FrameRect.right - FrameRect.left;
	FrameH	= FrameRect.bottom - FrameRect.top;

	pPlayerListT1->SetWndPos((ScreenW-FrameW)/2.0f, (ScreenH - FrameH)/2.0f);
	//-----------------------------------------------------------
	m_pPlayerLists->AttachChild(pPlayerListT1);
	//-----------------------------------------------------------

	m_todo_caption					=	"ah_todo";
	GameCaptions()->addCustomMessage(m_todo_caption, 0.0f, -0.8f, 0.02f, HUD().Font().pFontGraffiti19Russian, CGameFont::alCenter, TODO_MSG_COLOR, "");
	GameCaptions()->customizeMessage(m_todo_caption, CUITextBanner::tbsFlicker)->fPeriod = 0.5f;

	m_buy_msg_caption				=	"ah_buy";
	GameCaptions()->addCustomMessage(m_buy_msg_caption, DI2PX(0.0f), DI2PY(0.9f), SZ(0.02f), HUD().Font().pFontGraffiti19Russian, CGameFont::alCenter, BUY_MSG_COLOR, "");
	//-----------------------------------------------------------
	FrameRect = pStatisticWnd->GetFrameRect ();
	FrameW	= FrameRect.right - FrameRect.left;
	FrameH	= FrameRect.bottom - FrameRect.top;
	pStatisticWnd->SetWndRect((ScreenW-FrameW)/2.0f, (ScreenH - FrameH)/2.0f, FrameW, FrameH);

	m_pStatisticWnds->AttachChild(pStatisticWnd);

};
//--------------------------------------------------------------------

CUIGameAHunt::~CUIGameAHunt()
{
	xr_delete(m_pReinforcementInidcator);
	delete_data(m_pBuySpawnMsgBox);
}


void CUIGameAHunt::SetTodoCaption(LPCSTR str)
{
	GameCaptions()->setCaption(m_todo_caption, str, TODO_MSG_COLOR, true);
}

void CUIGameAHunt::SetBuyMsgCaption(LPCSTR str)
{
	GameCaptions()->setCaption(m_buy_msg_caption, str, BUY_MSG_COLOR, true);
}

void CUIGameAHunt::Render()
{
	m_pReinforcementInidcator->Draw();
	inherited::Render();
}

void CUIGameAHunt::OnFrame()
{
	inherited::OnFrame();
	m_pReinforcementInidcator->Update();
}

void CUIGameAHunt::reset_ui()
{
	inherited::reset_ui();
	m_pBuySpawnMsgBox->Reset();
}
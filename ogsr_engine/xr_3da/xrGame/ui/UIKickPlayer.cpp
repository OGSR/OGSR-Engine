#include "StdAfx.h"

#include "UIKickPlayer.h"
#include "UIVotingCategory.h"
#include "UIXmlInit.h"
#include "UI3tButton.h"
#include "UIListBox.h"
#include "UIListBoxItem.h"
#include "UISpinNum.h"
#include "../level.h"
#include "../game_cl_base.h"
#include "../game_cl_teamdeathmatch.h"
#include "../../xr_ioconsole.h"

CUIKickPlayer::CUIKickPlayer()
{
	m_prev_upd_time				= 0;
	bkgrnd						= xr_new<CUIStatic>(); 
	bkgrnd->SetAutoDelete		(true);
	AttachChild					(bkgrnd);

	header						= xr_new<CUIStatic>(); 
	header->SetAutoDelete		(true);
	AttachChild					(header);

	lst_back					= xr_new<CUIFrameWindow>(); 
	lst_back->SetAutoDelete		(true);
	AttachChild					(lst_back);

	m_ui_players_list			= xr_new<CUIListBox>(); 
	m_ui_players_list->SetAutoDelete(true);
	AttachChild					(m_ui_players_list);

	btn_ok						= xr_new<CUI3tButton>(); 
	btn_ok->SetAutoDelete		(true);
	AttachChild					(btn_ok);

	btn_cancel					= xr_new<CUI3tButton>(); 
	btn_cancel->SetAutoDelete	(true);
	AttachChild					(btn_cancel);

	m_spin_ban_sec				= xr_new<CUISpinNum>();
	m_spin_ban_sec->SetAutoDelete(true);
	m_spin_ban_sec->SetMax		(3000000);
	m_spin_ban_sec->SetMin		(60);
	AttachChild					(m_spin_ban_sec);

	m_ban_sec_label				= xr_new<CUIStatic>();
	m_ban_sec_label->SetAutoDelete(true);
	AttachChild					(m_ban_sec_label);

	mode						= MODE_KICK;
}

void CUIKickPlayer::Init(CUIXml& xml_doc)
{
	CUIXmlInit::InitWindow		(xml_doc, "kick_ban",				0, this);
	CUIXmlInit::InitStatic		(xml_doc, "kick_ban:background",	0, bkgrnd);
	CUIXmlInit::InitFrameWindow	(xml_doc, "kick_ban:list_back",		0, lst_back);
	CUIXmlInit::InitListBox		(xml_doc, "kick_ban:list",			0, m_ui_players_list);
	CUIXmlInit::Init3tButton	(xml_doc, "kick_ban:btn_ok",		0, btn_ok);
	CUIXmlInit::Init3tButton	(xml_doc, "kick_ban:btn_cancel",	0, btn_cancel);
	CUIXmlInit::InitSpin		(xml_doc, "kick_ban:spin_ban_time",	0, m_spin_ban_sec);
	CUIXmlInit::InitStatic		(xml_doc, "kick_ban:ban_time_lbl",	0, m_ban_sec_label);
}

void CUIKickPlayer::InitBan(CUIXml& xml_doc)
{
	CUIXmlInit::InitStatic		(xml_doc, "kick_ban:header_ban", 0, header);
    Init						(xml_doc);
	mode						= MODE_BAN;
	m_spin_ban_sec->Show		(true);
	m_ban_sec_label->Show		(true);
}

void CUIKickPlayer::InitKick(CUIXml& xml_doc)
{
	CUIXmlInit::InitStatic		(xml_doc, "kick_ban:header_kick", 0, header);
	Init						(xml_doc);
	mode						= MODE_KICK;
	m_spin_ban_sec->Show		(false);
	m_ban_sec_label->Show		(false);
}

#include <dinput.h>
bool CUIKickPlayer::OnKeyboard(int dik, EUIMessages keyboard_action)
{
	if (dik == DIK_ESCAPE)
	{
		OnBtnCancel	();
		return		true;
	}
	return CUIDialogWnd::OnKeyboard(dik, keyboard_action);
}

void CUIKickPlayer::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if (LIST_ITEM_SELECT == msg && pWnd == m_ui_players_list)
	{		
		CUIListBoxItem* itm		= smart_cast<CUIListBoxItem*>(m_ui_players_list->GetSelected());
		m_selected_item_text	= itm->GetText();
	}
	else if (BUTTON_CLICKED == msg)
	{
		if (pWnd == btn_ok)
			OnBtnOk		();
		else 
		if (pWnd == btn_cancel)
			OnBtnCancel	();
	}
}

void CUIKickPlayer::OnBtnOk()
{
	CUIListBoxItem* item = smart_cast<CUIListBoxItem*>(m_ui_players_list->GetSelected());
	if (item)
	{
		string512 command;	
		switch (mode)
		{
			case MODE_KICK:
                sprintf_s(command, "cl_votestart kick %s", item->GetText());
				break;
			case MODE_BAN:
				{
					sprintf_s(command, "cl_votestart ban %s %d", item->GetText(), m_spin_ban_sec->Value());
				}break;
		}
		Console->Execute			(command);
		game_cl_mp* game			= smart_cast<game_cl_mp*>(&Game());
		game->StartStopMenu			(this, true);
	}
	else
		return;
}

void CUIKickPlayer::OnBtnCancel()
{
    game_cl_mp* game				= smart_cast<game_cl_mp*>(&Game());
	game->StartStopMenu				(this, true);
}

IC bool	DM_Compare_Players(game_PlayerState* v1, game_PlayerState* v2);

DEFINE_VECTOR	(game_PlayerState*,ItemVec,ItemIt);
/*
LPCSTR _names[] = {
	"1.andy",
	"2.dima",
	"3.slipch",
	"4.bes",
	"5.anton",
	"6.andy-2",
	"7.dima-2",
	"8.slipch-2",
	"9.bes-2",
	"10.anton-2",
	"11.andy-3",
	"12.dima-3",
	"13.slipch-3",
	"14.bes-3",
	"15.anton-3",
	"16.andy-4",
	"17.dima-4",
	"18.slipch-4",
	"19.bes-4",
	"20.anton-4",
};

game_cl_GameState::PLAYERS_MAP	test_map1, test_map2, test_map3;
u32	ids1[15]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14};
u32	ids2[14]={5,7,9,11,13,6,8,10,12,5,7,9,11,13};
u32	ids3[15]={14,13,12,11,10,9,8,7,6,5,4,3,2,1,0};
u32 szs[3]={15,14,15};

xr_vector<game_PlayerState*>	g_ps;
*/

void CUIKickPlayer::Update()
{
	CUIDialogWnd::Update();

	if (m_prev_upd_time > Device.dwTimeContinual - 1000) return;

	m_prev_upd_time						= Device.dwTimeContinual;
#if 1
	const game_cl_GameState::PLAYERS_MAP& items = Game().players;
#else
	ClientID				cid;

	//.init
	static bool b_inited = false;
	if(!b_inited)
	{
		for(u32 e=0;e<15;++e)
		{
			game_PlayerState* ps	= xr_new<game_PlayerState>();
			strcpy					(ps->name, _names[e]);
			g_ps.push_back			(ps);
		}
		for(u32 _i=0; _i<3; ++_i)
		{
			game_cl_GameState::PLAYERS_MAP& items1	= (_i==0)?test_map1:(_i==1)?test_map2:test_map3;
			u32* vn									= (_i==0)?ids1:(_i==1)?ids2:ids3;
			for(u32 i=0; i<szs[_i];++i)
			{
				cid.set				(_i*500 + i);
				items1[cid]			= g_ps[vn[i]];
			}
		}
		b_inited = true;
	}
	static int iiii = 0;
	const game_cl_GameState::PLAYERS_MAP& items =	(iiii==0)?test_map1:(iiii==1)?test_map2:test_map3;

#endif

	game_cl_GameState::PLAYERS_MAP_CIT I = items.begin();
	game_cl_GameState::PLAYERS_MAP_CIT E = items.end();

	bool bNeedRefresh			= false;
	bool bHasSelected			= false;

	xr_vector<game_PlayerState*>::iterator fit;//, fite;
	for( ; I != E; ++I)
	{
		game_PlayerState* pI = I->second;
		if( m_selected_item_text.size() && !stricmp(pI->name, m_selected_item_text.c_str()) )
			bHasSelected		= true;


		fit = std::find(m_current_set.begin(), m_current_set.end(), pI);
		if(fit==m_current_set.end())
			bNeedRefresh = true;
		else
		if( stricmp( (*fit)->name, pI->name) )
			bNeedRefresh = true;
	}
	if(m_current_set.size() != items.size())
		bNeedRefresh = true;

	if(bNeedRefresh)
	{
		I								= items.begin();
		m_ui_players_list->Clear		();
		m_current_set.clear				();
	
		for ( ;I!=E; ++I)
		{
			game_PlayerState* p			= I->second;
			m_current_set.push_back		(p);
			m_ui_players_list->AddItem	(p->name);
		}
		if( bHasSelected )
			m_ui_players_list->SetSelectedText(m_selected_item_text.c_str());
	}
}
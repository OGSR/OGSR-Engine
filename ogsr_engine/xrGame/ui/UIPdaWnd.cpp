#include "stdafx.h"
#include "UIPdaWnd.h"
#include "../Pda.h"

#include "xrUIXmlParser.h"
#include "UIXmlInit.h"
#include "UIInventoryUtilities.h"

#include "../HUDManager.h"
#include "../level.h"
#include "../game_cl_base.h"

#include "UIStatic.h"
#include "UIFrameWindow.h"
#include "UITabControl.h"
//#include "UIPdaCommunication.h"
#include "UIPdaContactsWnd.h"
#include "UIMapWnd.h"
#include "UIDiaryWnd.h"
#include "UIFrameLineWnd.h"
#include "UIEncyclopediaWnd.h"
#include "UIStalkersRankingWnd.h"
#include "UIActorInfo.h"
#include "UIEventsWnd.h"
#include "../object_broker.h"
#include "UIMessagesWindow.h"
#include "UIMainIngameWnd.h"
#include "UITabButton.h"

#define		PDA_XML					"pda.xml"
u32			g_pda_info_state		= 0;

void RearrangeTabButtons(CUITabControl* pTab, xr_vector<Fvector2>& vec_sign_places);

CUIPdaWnd::CUIPdaWnd()
{
	UIMapWnd				= NULL;
	UIPdaContactsWnd		= NULL;
	UIEncyclopediaWnd		= NULL;
	UIDiaryWnd				= NULL;
	UIActorInfo				= NULL;
	UIStalkersRanking		= NULL;
	UIEventsWnd				= NULL;
	m_updatedSectionImage	= NULL;
	m_oldSectionImage		= NULL;
	Init					();
}

CUIPdaWnd::~CUIPdaWnd()
{
	delete_data		(UIMapWnd);
	delete_data		(UIPdaContactsWnd);
	delete_data		(UIEncyclopediaWnd);
	delete_data		(UIDiaryWnd);
	delete_data		(UIActorInfo);
	delete_data		(UIStalkersRanking);
	delete_data		(UIEventsWnd);
	delete_data		(m_updatedSectionImage);
	delete_data		(m_oldSectionImage);
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaWnd::Init()
{
	CUIXml uiXml;
	bool xml_result			= uiXml.Init(CONFIG_PATH, UI_PATH,PDA_XML);
	R_ASSERT3				(xml_result, "xml file not found", PDA_XML);

	CUIXmlInit xml_init;
	
	m_pActiveDialog			= NULL;


	xml_init.InitWindow		(uiXml, "main", 0, this);

	UIMainPdaFrame			= xr_new<CUIStatic>(); UIMainPdaFrame->SetAutoDelete(true);
	AttachChild				(UIMainPdaFrame);
	xml_init.InitStatic		(uiXml, "background_static", 0, UIMainPdaFrame);

	//Ёлементы автоматического добавлени€
	xml_init.InitAutoStatic	(uiXml, "auto_static", this);

	// Main buttons background
	UIMainButtonsBackground = xr_new<CUIFrameLineWnd>(); UIMainButtonsBackground->SetAutoDelete(true);
	UIMainPdaFrame->AttachChild(UIMainButtonsBackground);
	xml_init.InitFrameLine	(uiXml, "mbbackground_frame_line", 0, UIMainButtonsBackground);

	// Timer background
	UITimerBackground		= xr_new<CUIFrameLineWnd>(); UITimerBackground->SetAutoDelete(true);
	UIMainPdaFrame->AttachChild(UITimerBackground);
	xml_init.InitFrameLine	(uiXml, "timer_frame_line", 0, UITimerBackground);

	// Oкно карты
	UIMapWnd				= xr_new<CUIMapWnd>();
	UIMapWnd->Init			("pda_map.xml","map_wnd");

	if( IsGameTypeSingle() )
	{
		// Oкно коммуникaции
		UIPdaContactsWnd		= xr_new<CUIPdaContactsWnd>();
		UIPdaContactsWnd->Init	();


		// Oкно новостей
		UIDiaryWnd				= xr_new<CUIDiaryWnd>();
		UIDiaryWnd->Init		();

		// ќкно энциклопедии
		UIEncyclopediaWnd		= xr_new<CUIEncyclopediaWnd>();
		UIEncyclopediaWnd->Init	();

		// ќкно статистики о актере
		UIActorInfo				= xr_new<CUIActorInfoWnd>();
		UIActorInfo->Init		();

		// ќкно рейтинга сталкеров
		UIStalkersRanking		= xr_new<CUIStalkersRankingWnd>();
		UIStalkersRanking->Init	();

		UIEventsWnd				= xr_new<CUIEventsWnd>();
		UIEventsWnd->Init		();
	}
	// Tab control
	UITabControl				= xr_new<CUITabControl>(); UITabControl->SetAutoDelete(true);
	UIMainPdaFrame->AttachChild	(UITabControl);
	xml_init.InitTabControl		(uiXml, "tab", 0, UITabControl);
	UITabControl->SetMessageTarget(this);

	if(GameID()!=GAME_SINGLE){
		UITabControl->GetButtonsVector()->at(0)->Enable(false);
		UITabControl->GetButtonsVector()->at(2)->Enable(false);
		UITabControl->GetButtonsVector()->at(3)->Enable(false);
		UITabControl->GetButtonsVector()->at(4)->Enable(false);
		UITabControl->GetButtonsVector()->at(5)->Enable(false);
		UITabControl->GetButtonsVector()->at(6)->Enable(false);
	}
	
	m_updatedSectionImage			= xr_new<CUIStatic>();
	xml_init.InitStatic				(uiXml, "updated_section_static", 0, m_updatedSectionImage);

	m_oldSectionImage				= xr_new<CUIStatic>();
	xml_init.InitStatic				(uiXml, "old_section_static", 0, m_oldSectionImage);

	m_pActiveSection				= eptNoActiveTab;

	RearrangeTabButtons			(UITabControl, m_sign_places_main);
}

void CUIPdaWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if(pWnd == UITabControl){
		if (TAB_CHANGED == msg){
			SetActiveSubdialog	((EPdaTabs)UITabControl->GetActiveIndex());
		}
	}else 
	{
		R_ASSERT(m_pActiveDialog);
		m_pActiveDialog->SendMessage(pWnd, msg, pData);
	}
}

void CUIPdaWnd::Show()
{
	InventoryUtilities::SendInfoToActor("ui_pda");

	inherited::Show();
}

void CUIPdaWnd::Hide()
{
	inherited::Hide();

	InventoryUtilities::SendInfoToActor("ui_pda_hide");
	HUD().GetUI()->UIMainIngameWnd->SetFlashIconState_(CUIMainIngameWnd::efiPdaTask, false);

}

void CUIPdaWnd::UpdateDateTime()
{
	static shared_str prevStrTime = " ";
	xr_string strTime = *InventoryUtilities::GetGameTimeAsString(InventoryUtilities::etpTimeToMinutes);
				strTime += " ";
				strTime += *InventoryUtilities::GetGameDateAsString(InventoryUtilities::edpDateToDay);

	if (xr_strcmp(strTime.c_str(), prevStrTime))
	{
		UITimerBackground->UITitleText.SetText(strTime.c_str());
		prevStrTime = strTime.c_str();
	}
}

void CUIPdaWnd::Update()
{
	inherited::Update		();
	UpdateDateTime			();
}

void CUIPdaWnd::SetActiveSubdialog(EPdaTabs section)
{
	if(	m_pActiveSection == section) return;

	if (m_pActiveDialog){
		UIMainPdaFrame->DetachChild(m_pActiveDialog);
		m_pActiveDialog->Show(false);
	}

	switch (section) 
	{
	case eptDiary:
		m_pActiveDialog			= smart_cast<CUIWindow*>(UIDiaryWnd);
		InventoryUtilities::SendInfoToActor("ui_pda_events");
		g_pda_info_state		&= ~pda_section::diary;
		break;
	case eptContacts:
		m_pActiveDialog			= smart_cast<CUIWindow*>(UIPdaContactsWnd);
		InventoryUtilities::SendInfoToActor("ui_pda_contacts");
		g_pda_info_state		&= ~pda_section::contacts;
		break;
	case eptMap:
		m_pActiveDialog			= smart_cast<CUIWindow*>(UIMapWnd);
		g_pda_info_state		&= ~pda_section::map;
		break;
	case eptEncyclopedia:
		m_pActiveDialog			= smart_cast<CUIWindow*>(UIEncyclopediaWnd);
		InventoryUtilities::SendInfoToActor("ui_pda_encyclopedia");
		g_pda_info_state		&= ~pda_section::encyclopedia;
		break;
	case eptActorStatistic:
		m_pActiveDialog			= smart_cast<CUIWindow*>(UIActorInfo);
		InventoryUtilities::SendInfoToActor("ui_pda_actor_info");
		g_pda_info_state		&= ~pda_section::statistics;
		break;
	case eptRanking:
		m_pActiveDialog			= smart_cast<CUIWindow*>(UIStalkersRanking);
		g_pda_info_state		&= ~pda_section::ranking;
		InventoryUtilities::SendInfoToActor("ui_pda_ranking");
		break;
	case eptQuests:
		m_pActiveDialog			= smart_cast<CUIWindow*>(UIEventsWnd);
		g_pda_info_state		&= ~pda_section::quests;
		break;
	default:
		Msg("not registered button identifier [%d]",UITabControl->GetActiveIndex());
	}
	UIMainPdaFrame->AttachChild		(m_pActiveDialog);
	m_pActiveDialog->Show			(true);

	if(UITabControl->GetActiveIndex()!=section)
		UITabControl->SetNewActiveTab	(section);

	m_pActiveSection = section;
}


void CUIPdaWnd::Draw()
{
	inherited::Draw									();
	DrawUpdatedSections								();
}

void CUIPdaWnd::PdaContentsChanged	(pda_section::part type)
{
	bool b = true;

	if(type==pda_section::encyclopedia){
		UIEncyclopediaWnd->ReloadArticles	();
	}else
	if(type==pda_section::news){
		UIDiaryWnd->AddNews					();
		UIDiaryWnd->MarkNewsAsRead			(UIDiaryWnd->IsShown());
	}else
	if(type==pda_section::quests){
		UIEventsWnd->Reload					();
	}else
	if(type==pda_section::contacts){
		UIPdaContactsWnd->Reload		();
		b = false;
	}

	if(b){
		g_pda_info_state |= type;
		HUD().GetUI()->UIMainIngameWnd->SetFlashIconState_(CUIMainIngameWnd::efiPdaTask, true);
	}

}
void draw_sign		(CUIStatic* s, Fvector2& pos)
{
	s->SetWndPos		(pos);
	s->Draw				();
}

void CUIPdaWnd::DrawUpdatedSections				()
{
	m_updatedSectionImage->Update				();
	m_oldSectionImage->Update					();
	
	Fvector2									tab_pos;
	UITabControl->GetAbsolutePos				(tab_pos);

	Fvector2 pos;

	pos = m_sign_places_main[eptQuests];
	pos.add(tab_pos);
	if(g_pda_info_state&pda_section::quests)
		draw_sign								(m_updatedSectionImage, pos);
	else
		draw_sign								(m_oldSectionImage, pos);

	pos = m_sign_places_main[eptMap];
	pos.add(tab_pos);
	if(g_pda_info_state&pda_section::map)
		draw_sign								(m_updatedSectionImage, pos);
	else
		draw_sign								(m_oldSectionImage, pos);

	pos = m_sign_places_main[eptDiary];
	pos.add(tab_pos);
	if(g_pda_info_state&pda_section::diary)
		draw_sign								(m_updatedSectionImage, pos);
	else
		draw_sign								(m_oldSectionImage, pos);

	pos = m_sign_places_main[eptContacts];
	pos.add(tab_pos);
	if(g_pda_info_state&pda_section::contacts)
		draw_sign								(m_updatedSectionImage, pos);
	else
		draw_sign								(m_oldSectionImage, pos);

	pos = m_sign_places_main[eptRanking];
	pos.add(tab_pos);
	if(g_pda_info_state&pda_section::ranking)
		draw_sign								(m_updatedSectionImage, pos);
	else
		draw_sign								(m_oldSectionImage, pos);

	pos = m_sign_places_main[eptActorStatistic];
	pos.add(tab_pos);
	if(g_pda_info_state&pda_section::statistics)
		draw_sign								(m_updatedSectionImage, pos);
	else
		draw_sign								(m_oldSectionImage, pos);

	pos = m_sign_places_main[eptEncyclopedia];
	pos.add(tab_pos);
	if(g_pda_info_state&pda_section::encyclopedia)
		draw_sign								(m_updatedSectionImage, pos);
	else
		draw_sign								(m_oldSectionImage, pos);
	
}

void CUIPdaWnd::Reset()
{
	inherited::Reset		();
	if (UIMapWnd)			UIMapWnd->Reset			();
	if (UIPdaContactsWnd)	UIPdaContactsWnd->Reset	();
	if (UIEncyclopediaWnd)	UIEncyclopediaWnd->Reset();
	if (UIDiaryWnd)			UIDiaryWnd->Reset		();
	if (UIActorInfo)		UIActorInfo->Reset		();
	if (UIStalkersRanking)	UIStalkersRanking->Reset();
	if (UIEventsWnd)		UIEventsWnd->Reset		();
}

void RearrangeTabButtons(CUITabControl* pTab, xr_vector<Fvector2>& vec_sign_places)
{
	TABS_VECTOR *	btn_vec		= pTab->GetButtonsVector();
	TABS_VECTOR::iterator it	= btn_vec->begin();
	TABS_VECTOR::iterator it_e	= btn_vec->end();
	vec_sign_places.clear		();
	vec_sign_places.resize		(btn_vec->size());

	Fvector2					pos;
	pos.set						((*it)->GetWndPos());
	Fvector2					sign_sz;
	sign_sz.set					(9.0f+3.0f, 11.0f);
	u32 idx						= 0;
	float	btn_text_len		= 0.0f;
	CUIStatic* st				= NULL;

	for(;it!=it_e;++it,++idx)
	{
		if(idx!=0)
		{
			st = xr_new<CUIStatic>(); st->SetAutoDelete(true);pTab->AttachChild(st);
			st->SetFont((*it)->GetFont());
			st->SetTextColor	(color_rgba(90,90,90,255));
			st->SetText("//");
			st->SetWndSize		((*it)->GetWndSize());
			st->AdjustWidthToText();
			st->SetWndPos		(pos);
			pos.x				+= st->GetWndSize().x;
		}

		vec_sign_places[idx].set(pos);
		vec_sign_places[idx].y	+= iFloor(((*it)->GetWndSize().y - sign_sz.y)/2.0f);
		vec_sign_places[idx].y	= (float)iFloor(vec_sign_places[idx].y);
		pos.x					+= sign_sz.x;

		(*it)->SetWndPos		(pos);
		(*it)->AdjustWidthToText();
		btn_text_len			= (*it)->GetWndSize().x;
		pos.x					+= btn_text_len+3.0f;
	}

}

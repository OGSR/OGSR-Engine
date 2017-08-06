#include "stdafx.h"
#include "UINewsItemWnd.h"
#include "UIXmlInit.h"
#include "UIStatic.h"
#include "../game_news.h"
#include "../date_time.h"

CUINewsItemWnd::CUINewsItemWnd()
{}

CUINewsItemWnd::~CUINewsItemWnd()
{}

void CUINewsItemWnd::Init				(LPCSTR xml_name, LPCSTR start_from)
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, xml_name);
	R_ASSERT3(xml_result, "xml file not found", xml_name);

	CUIXmlInit xml_init;

	string512 str;

	strcpy(str,start_from);
	xml_init.InitWindow			(uiXml,str,0,this);

	strconcat(sizeof(str),str,start_from,":image");
	m_UIImage					= xr_new<CUIStatic>();	m_UIImage->SetAutoDelete(true);
	xml_init.InitStatic			(uiXml,str,0,m_UIImage);
	AttachChild					(m_UIImage);

	strconcat(sizeof(str),str,start_from,":text_cont");
	m_UIText					= xr_new<CUIStatic>();	m_UIText->SetAutoDelete(true);
	xml_init.InitStatic			(uiXml,str,0,m_UIText);
	AttachChild					(m_UIText);

	strconcat(sizeof(str),str,start_from,":date_text_cont");
	m_UITextDate				= xr_new<CUIStatic>();	m_UITextDate->SetAutoDelete(true);
	xml_init.InitStatic			(uiXml,str,0,m_UITextDate);
	AttachChild					(m_UITextDate);

	strconcat(sizeof(str),str,start_from,":auto");
	xml_init.InitAutoStaticGroup(uiXml, str, 0, this);

}

void CUINewsItemWnd::Setup			(GAME_NEWS_DATA& news_data)
{
	m_UIText->SetTextST				(news_data.news_text.c_str());
	m_UIText->AdjustHeightToText	();
	float h1						= m_UIText->GetWndPos().y + m_UIText->GetHeight();

	string128						_time;
	u32 years, months, days, hours, minutes, seconds, milliseconds;
	split_time						(news_data.receive_time, years, months, days, hours, minutes, seconds, milliseconds);
	sprintf_s							(_time, "%02i/%02i/%04i %02i:%02i", days, months, years, hours, minutes);

	m_UITextDate->SetText			(_time);
	m_UITextDate->AdjustHeightToText();
	float h2						= m_UITextDate->GetWndPos().y + m_UITextDate->GetHeight();

	m_UIImage->InitTexture			(*news_data.texture_name);
	m_UIImage->SetOriginalRect		(news_data.tex_rect.x1,news_data.tex_rect.y1,news_data.tex_rect.x2,news_data.tex_rect.y2);
	float h3						= m_UIImage->GetWndPos().y + m_UIImage->GetHeight();
	h1								= _max(h1,h2);
	h1								= _max(h1,h3);
	SetHeight						(h1);
}

// UISleepWnd.cpp:  окошко для выбора того, сколько спать
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UISleepWnd.h"
#include "../alife_space.h"
#include "UIXmlInit.h"
#include "UIButton.h"

CUISleepWnd::CUISleepWnd()
	: m_Hours		(0),
	  m_Minutes		(0)
{
}

CUISleepWnd::~CUISleepWnd()
{
}


void CUISleepWnd::Init()
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, "sleep_dialog_new.xml");
	R_ASSERT2(xml_result, "xml file not found sleep_dialog_new.xml");

	CUIXmlInit	xml_init;

	// Statics
	UIStaticRestAmount		= xr_new<CUIStatic>();UIStaticRestAmount->SetAutoDelete(true);
	AttachChild				(UIStaticRestAmount);
	xml_init.InitStatic		(uiXml, "rest_amount_static", 0, UIStaticRestAmount);

	// Plus, minus time
	UIPlusBtn				= xr_new<CUIButton>();UIPlusBtn->SetAutoDelete(true);
	AttachChild				(UIPlusBtn);
	xml_init.InitButton		(uiXml, "plus_button", 0, UIPlusBtn);

	UIMinusBtn				= xr_new<CUIButton>();UIMinusBtn->SetAutoDelete(true);
	AttachChild(UIMinusBtn);
	xml_init.InitButton(uiXml, "minus_button", 0, UIMinusBtn);

	// Perform sleep
	UIRestBtn				= xr_new<CUIButton>();UIRestBtn->SetAutoDelete(true);
	AttachChild(UIRestBtn);
	xml_init.InitButton(uiXml, "rest_button", 0, UIRestBtn);

	// Update timerest meter
	ResetTime();
}

void CUISleepWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	const s8 deltaMinutes = 30;

	if(pWnd == UIRestBtn && msg == BUTTON_CLICKED)
	{
		u32 restMsec = (m_Hours * 3600 + m_Minutes * 60) * 1000;
		if (restMsec != 0)
			GetMessageTarget()->SendMessage(this, SLEEP_WND_PERFORM_BUTTON_CLICKED, reinterpret_cast<void*>(&restMsec));

	}
	else if(pWnd == UIPlusBtn && msg == BUTTON_CLICKED)
	{
		// Add fixed amount of minutes and hours
		ModifyRestTime(0, deltaMinutes);
	}
	else if(pWnd == UIMinusBtn && msg == BUTTON_CLICKED)
	{
		// Add fixed amount of minutes and hours
		ModifyRestTime(0, -deltaMinutes);
	}
	else if ((UIPlusBtn == pWnd || UIMinusBtn == pWnd || UIRestBtn == pWnd) && BUTTON_DOWN == msg)
	{
		CUIButton *pBtn = smart_cast<CUIButton*>(pWnd);
		R_ASSERT(pBtn);

		pBtn->EnableTextHighlighting(false);
	}
	
	if ((UIPlusBtn == pWnd || UIMinusBtn == pWnd || UIRestBtn == pWnd) && BUTTON_CLICKED == msg)
	{
		CUIButton *pBtn = smart_cast<CUIButton*>(pWnd);
		R_ASSERT(pBtn);

		pBtn->EnableTextHighlighting(true);
	}

	inherited::SendMessage(pWnd, msg, pData);
}


void CUISleepWnd::ModifyRestTime(s8 dHours, s8 dMinutes)
{

	// Проверка на влидный интервал
	R_ASSERT(dMinutes < 61);

	s8 oldMin = m_Minutes;

	// Увеличиваем минуты
	m_Minutes = m_Minutes + dMinutes;

	// Проверяем выходы за пределы допуска
	if (m_Minutes > 59)
	{
		++m_Hours;
		m_Minutes = m_Minutes - 60;
	}
	else if (m_Minutes < 0)
	{
		--m_Hours;
		m_Minutes = m_Minutes + 60;
	}

	// Тоже самое и для часов
	m_Hours = m_Hours + dHours;
	if (m_Hours > 99)
	{
		m_Hours = 99;
		m_Minutes = oldMin;
	}
	else if (m_Hours < 0)
	{
		m_Minutes = oldMin;
		m_Hours = 0;
	}

	SetRestTime(m_Hours, m_Minutes);
}

void CUISleepWnd::SetRestTime(u8 hours, u8 minutes)
{
	string32					buf;
	static s8 _h				= pSettings->r_s8("actor", "max_sleep_hours");
	m_Minutes					= minutes;
	m_Hours						= hours;
	if(m_Hours>=_h)				m_Minutes = 0;
	clamp						(m_Hours,s8(0),_h);
	sprintf_s						(buf, "%02i:%02i", m_Hours, m_Minutes);
	UIStaticRestAmount->SetText	(buf);
}

void CUISleepWnd::SetText(LPCSTR str){
	this->UIRestBtn->SetText(str);
}
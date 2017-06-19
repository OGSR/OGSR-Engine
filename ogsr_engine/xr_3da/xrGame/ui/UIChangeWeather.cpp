#include "stdafx.h"
#include "UIChangeWeather.h"
#include "UIXmlInit.h"
#include "UI3tButton.h"
#include "../game_cl_teamdeathmatch.h"
#include "UIKickPlayer.h"
#include "UIChangeMap.h"
//#include "UIMapList.h"

CUIChangeWeather::CUIChangeWeather(){
	bkgrnd = xr_new<CUIStatic>(); 
	bkgrnd->SetAutoDelete(true);
	AttachChild(bkgrnd);

	header = xr_new<CUIStatic>();
	header->SetAutoDelete(true);
	AttachChild(header);

	btn_cancel = xr_new<CUI3tButton>();
	btn_cancel->SetAutoDelete(true);
	AttachChild(btn_cancel);

	for (int i = 0; i<4; i++)
	{
		btn[i] = xr_new<CUI3tButton>();
		btn[i]->SetAutoDelete(true);
		AttachChild(btn[i]);

		m_data[i].m_static = xr_new<CUIStatic>();
		m_data[i].m_static->SetAutoDelete(true);
		AttachChild(m_data[i].m_static);
	}

	weather_counter = 0;
}

void CUIChangeWeather::Init(CUIXml& xml_doc){
	CUIXmlInit::InitWindow(xml_doc, "change_weather", 0, this);

	CUIXmlInit::InitStatic(xml_doc, "change_weather:header", 0, header);
	CUIXmlInit::InitStatic(xml_doc, "change_weather:background", 0, bkgrnd);

	string256 _path;
	for (int i = 0; i<4; i++){
		sprintf_s(_path, "change_weather:btn_%d", i + 1);
		CUIXmlInit::Init3tButton(xml_doc, _path, 0, btn[i]);
		sprintf_s(_path, "change_weather:txt_%d", i + 1);
		CUIXmlInit::InitStatic(xml_doc, _path, 0, m_data[i].m_static);
	}

	CUIXmlInit::Init3tButton(xml_doc, "change_weather:btn_cancel", 0, btn_cancel);

	ParseWeather();
}

void CUIChangeWeather::SendMessage(CUIWindow* pWnd, s16 msg, void* pData){
	if (BUTTON_CLICKED == msg)
	{
		if (btn_cancel == pWnd)
			OnBtnCancel();
		for (int i=0; i<4; i++){
			if (btn[i] == pWnd){
				OnBtn(i);
				return;
			}
		}
	}

}

#include <dinput.h>

bool CUIChangeWeather::OnKeyboard(int dik, EUIMessages keyboard_action){
	CUIDialogWnd::OnKeyboard(dik, keyboard_action);
	if (WINDOW_KEY_PRESSED == keyboard_action){
		if (DIK_ESCAPE == dik){
			OnBtnCancel();
			return true;
		}
		if (dik >= DIK_1 && dik <= DIK_4){
			OnBtn(dik - DIK_1);
			return true;
		}
	}

	return false;
}

#include "../../xr_ioconsole.h"

void CUIChangeWeather::OnBtn(int i){
	game_cl_mp* game		= smart_cast<game_cl_mp*>(&Game());
	string1024				command;
	sprintf_s					(command, "cl_votestart changeweather %s %s", *m_data[i].m_weather_name, *m_data[i].m_weather_time);
	Console->Execute		(command);
	game->StartStopMenu(this, true);
}

void CUIChangeWeather::OnBtnCancel(){
	game_cl_mp* game = smart_cast<game_cl_mp*>(&Game());
	game->StartStopMenu(this, true);
}

#include "UIMapList.h"
#include "../UIGameCustom.h"

void CUIChangeWeather::ParseWeather()
{
	weather_counter = 0;

	GAME_WEATHERS game_weathers = gMapListHelper.GetGameWeathers();
	GAME_WEATHERS_CIT it		= game_weathers.begin();
	GAME_WEATHERS_CIT it_e		= game_weathers.end();
	
	for( ;it!=it_e; ++it)
	{
		AddWeather			( (*it).m_weather_name, (*it).m_start_time);
	}
};

void CUIChangeWeather::AddWeather(const shared_str& weather, const shared_str& time){
	m_data[weather_counter].m_static->SetTextST	(*weather);
	m_data[weather_counter].m_weather_name		= weather;
	m_data[weather_counter].m_weather_time		= time;
	weather_counter++;
}

